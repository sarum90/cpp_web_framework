#include <cctype>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/variant.hpp>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#include "base64.hpp"
#include "http_client.hpp"
#include "mestring.hpp"
#include "oauth.hpp"
#include "reactor.hpp"

namespace meoauth {

namespace {

std::string url_encode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (auto i = value.begin(), n = value.end(); i != n; ++i) {
        auto c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

std::string to_url_encoded(const std::vector<std::pair<std::string, std::string>>& in) {
	std::ostringstream o;
	bool first = true;
  for (const auto& c: in) {
		if (!first) {
			o << "&";
		}
		o << url_encode(c.first) << "=" << url_encode(c.second);
		first = false;
  }
  return o.str();
}

struct val {
  val (const std::string& s):str_val_(s), int_val_(0), is_int_(false){}
  template <int N>
  val (const char (&c)[N]):str_val_(c, N-1), int_val_(0), is_int_(false){}
  val (int i):str_val_(), int_val_(i), is_int_(true){}

  std::string str_val_;
  int int_val_;
  bool is_int_;
};

std::string escape_json(const std::string &s) {
    std::ostringstream o;
		o << "\"";
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u"
                  << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
            } else {
                o << *c;
            }
        }
    }
		o << "\"";
    return o.str();
}

std::string to_json(const std::vector<std::pair<std::string, val>>& in) {
	std::ostringstream o;
	o << "{";
	bool first = true;
  for (const auto& c: in) {
		if (!first) {
			o << ",";
		}
		o << escape_json(c.first) << ":";
    if (c.second.is_int_) {
      o << c.second.int_val_;
    } else {
      o << escape_json(c.second.str_val_);
    }
		first = false;
  }
  o << "}";
  return o.str();
}


decltype(auto) get_rsa_private_key(std::string keystr) {
  auto bio_free = [](BIO* x){BIO_free(x);};
  std::unique_ptr<BIO, decltype(bio_free)> mem{
    BIO_new_mem_buf(reinterpret_cast<void *>(&keystr[0]), keystr.size()), bio_free};


  auto kk = PEM_read_bio_RSAPrivateKey(mem.get(), nullptr, 0, nullptr);
  if (kk == NULL) {
    throw std::runtime_error("Failure to read private key");
  }
  auto rsa_free = [](RSA* k){RSA_free(k);};
  std::unique_ptr<RSA, decltype(rsa_free)> key{kk, rsa_free};


  if (!RSA_check_key(key.get())) {
    throw std::runtime_error("Private key failed check.");
  }

  return key;
}

std::string digest(std::string thing) {
  EVP_MD_CTX *mdctx;
  const EVP_MD *md;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len;

  md = EVP_sha256();

  if(!md) {
    throw std::runtime_error("Could not load sha256");
  }

  mdctx = EVP_MD_CTX_create();
  EVP_DigestInit_ex(mdctx, md, NULL);
  EVP_DigestUpdate(mdctx, thing.c_str(), thing.size());
  EVP_DigestFinal_ex(mdctx, md_value, &md_len);
  EVP_MD_CTX_destroy(mdctx);

  return std::string(reinterpret_cast<char *>(&md_value[0]), md_len);
}

template<class T>
std::string sign(std::string digest, const T& key) {
  char outdata[4096];

  unsigned int out_len = 4096;

  int i = RSA_sign(
      NID_sha256,
      reinterpret_cast<const unsigned char *>(digest.c_str()), digest.size(),
      reinterpret_cast<unsigned char *>(&outdata[0]), &out_len, key.get());
 
  if (!i) {
    auto err = ERR_get_error();
    std::string message(ERR_error_string(err, nullptr));
    throw std::runtime_error(message);
  }

  return std::string{outdata, out_len};
}

std::string base64_encoded_claim_set(std::string email, std::string scopes) {
  auto header = to_json({
      {"alg", "RS256"},
      {"typ", "JWT"},
  });

  auto now = time(nullptr);
  auto expire = now + 3600;

  auto claims = to_json({
      {"aud", "https://www.googleapis.com/oauth2/v4/token"},
      {"exp", expire},
      {"iat", now},
      {"iss", email},
      {"scope", scopes},
  });

  std::string ds;
  {
    std::ostringstream o;
    o << base64_encode_noeq(mes::make_mestring(header)) << "."
              << base64_encode_noeq(mes::make_mestring(claims));
    ds = o.str();
  }
  return ds;
}

template <class T>
std::string digest_and_sign (std::string x, const T& key) {
  std::string dts = digest(x);
  std::string outdata = sign(dts, key);

  std::string ss;
  {
    std::ostringstream o;
    o << base64_encode_noeq(mes::make_mestring(outdata));
    ss = o.str();
  }

  return ss;
}

}


future<std::string> authenticate(
    http::client* cl,
    mes::mestring_cat scopes,
    mes::mestring_cat credentials_json_file
) {
  boost::property_tree::ptree pt;
  read_json(credentials_json_file, pt);
  auto private_key = pt.get<std::string>("private_key");
  auto email = pt.get<std::string>("client_email");

  std::string claims = base64_encoded_claim_set(email, scopes);

  auto key = get_rsa_private_key(std::move(private_key));

  std::string sig = digest_and_sign(claims, key);

	auto jwt = claims + "." + sig;

	auto body = to_url_encoded({
      {"grant_type", "urn:ietf:params:oauth:grant-type:jwt-bearer"},
      {"assertion", jwt}
  });

  return do_with(std::move(body), [cl=cl](auto& body) {
      return cl->request(
        "POST",
        "https://www.googleapis.com/oauth2/v4/token",
        mes::make_mestring(body),
        {{"Content-Type", "application/x-www-form-urlencoded"}}
      ).then([](auto a) {
        boost::property_tree::ptree pt;
        std::istringstream iss(a.content);
        read_json(iss, pt);
        return std::string{"Bearer " + pt.get<std::string>("access_token")};
      });
  });
}

}
