#pragma once

#include "reactor.hpp"
#include "net.hpp"
#include "mestring.hpp"
#include "buffered_reader.hpp"

namespace http {

struct response {
  std::string protocol;
  std::string version;
  int status_code;
  std::string status;
  std::vector<std::pair<std::string, std::string>> headers;
  std::string content;

  mes::mestring get_header(mes::mestring header, mes::mestring def) const {
    for (const auto& h : headers) {
      if (header == h.first) {
        return mes::make_mestring(h.second.c_str(), h.second.size());
      }
    }
    return def;
  }
};

inline std::ostream& operator<<(std::ostream& o, const response& r) {
  o << r.protocol << " " << r.version << " " << r.status_code << " " << r.status << "\n";
  for (const auto& h: r.headers) {
    o << h.first << ": " << h.second << "\n";
  }
  o << r.content;
  return o;
}

template <class R>
class parser {
public:
  parser(reactor * r, buffered_reader<R>* br): r_(r), reader_(br){}

  future<mes::mestring_cat> read_n(int n) {
    return reader_->read_some().then([n=n, p=this](const mes::mestring& str) {
        int bytes_left = n - p->current_.size();
        if (bytes_left > str.size()) {
          p->current_ += str;
          return p->read_n(n);
        } else {
          auto new_ms = mes::substr(&str[0], &str[bytes_left]);
          p->current_ += new_ms;
          p->reader_->replace_bytes(str.size()-new_ms.size());
          mes::mestring_cat resp = std::move(p->current_);
          p->current_.reset();
          return p->r_->template make_ready_future<mes::mestring_cat>(std::move(resp));
        }
    });
  }

  future<mes::mestring_cat> read_to(char c) {
    return reader_->read_some().then([c=c, p=this](const mes::mestring& str) {
        int idx = str.find_index(c) + 1;
        if (idx == 0) {
          p->current_ += str;
          return p->read_to(c);
        } else {
          auto new_ms = mes::make_mestring(&str[0], idx);
          p->current_ += new_ms;
          p->reader_->replace_bytes(str.size()-new_ms.size());
          mes::mestring_cat resp = std::move(p->current_);
          p->current_.reset();
          return p->r_->template make_ready_future<mes::mestring_cat>(std::move(resp));
        }
    });
  }

  future<mes::mestring_cat> read_to(mes::mestring str) {
    return reader_->read_some().then([s=str, p=this](const mes::mestring& str) {
        p->current_ += str;
        int idx = mes::find_index(p->current_, s) + s.size();
        if (idx < s.size()) {
          return p->read_to(s);
        } else {
          int extra = str.size()-idx;
          auto mc = mes::mestring_cat::truncated(std::move(p->current_), -extra);
          p->current_.reset();
          p->reader_->replace_bytes(extra);
          return p->r_->template make_ready_future<mes::mestring_cat>(std::move(mc));
        }
    });
  }

  future<mes::mestring_cat> read_up_to(mes::mestring str) {
    return reader_->read_some().then([s=str, p=this](mes::mestring str) {
        p->current_ += str;
        int idx = mes::find_index(p->current_, s) + s.size();
        if (idx < s.size()) {
          return p->read_to(s);
        } else {
          int extra = str.size()-idx;
          auto mc = mes::mestring_cat::truncated(std::move(p->current_), -(extra + s.size()));
          p->current_.reset();
          p->reader_->replace_bytes(extra);
          return p->r_->template make_ready_future<mes::mestring_cat>(std::move(mc));
        }
    });
  }

private:
  reactor * r_;
  buffered_reader<R>* reader_;
  mes::mestring_cat current_;
};

template<class R>
parser<R> make_parser(reactor * r, buffered_reader<R>* br) {
  return parser<R>{r, br};
}

class client {
public:
  client(reactor * r): r_(r) {}

  future<response> request_over_socket(
      mes::mestring_cat method,
      mes::mestring_cat host,
      mes::mestring_cat path,
      net::readwritable* skt,
      mes::mestring_cat content,
      std::vector<std::pair<mes::mestring_cat, mes::mestring_cat>> headers)
  {
    auto* r = r_;
    auto br = make_buffered_reader(r, skt);
    return do_with(std::move(br), response{}, [headers=std::move(headers), content=content, method=method, s=skt, r=r, path=path, host=host](auto& reader, auto& res) mutable {
      auto* rdr = &reader;
      mes::mestring_cat mc;
      mc += method;
      mc += " ";
      mc += path;
      mc += " HTTP/1.1\r\nHost: ";
      mc += host;
      mc += "\r\nContent-Length: ";
      return reax::write(r, s, std::move(mc))
      .then([headers=std::move(headers), r=r, s=s, content=content]() {
        return do_with(std::to_string(content.size()), [headers=std::move(headers), r=r, s=s, content=content](auto& str) {
          mes::mestring_cat mc;
          mc += mes::make_mestring(str);
          for (const auto& h: headers) {
            mc += "\r\n";
            mc += h.first;
            mc += ": ";
            mc += h.second;
          }
          mc += "\r\n\r\n";
          mc += content;
          return reax::write(r, s, std::move(mc));
        });
      }).then([r=r, s=s, rdr=rdr, res=&res]() mutable {
        return do_with(make_parser(r, rdr), [r=r, res=res](auto& p){
            return p.read_up_to("/").then([&p=p, res=res](auto m) {
              res->protocol = std::move(m);
              return p.read_up_to(" ");
            }).then([&p=p, res=res](auto m) {
              res->version = std::move(m);
              return p.read_up_to(" ");
            }).then([&p=p, res=res](mes::mestring_cat m) {
              res->status_code = mes::parse_int(std::move(m));
              return p.read_up_to("\r\n");
            }).then([&p=p, r=r, res=res](auto m) mutable {
              res->status = std::move(m);
              return client::read_headers(&p, r, res);
            }).then([&p=p, r=r, res=res]() {
              auto tr = res->get_header("Transfer-Encoding", "xx");
              if (tr == "chunked") {
                return client::read_chunked(&p, r, mes::mestring_cat{});
              }
              auto m = res->get_header("Content-Length", "-1");
              if (m != "-1") {
                int len = mes::parse_int(m);
                return p.read_n(len);
              }
              return p.read_n(10);
            }).then([res=res](auto m) {
              res->content = std::move(m);
              return std::move(*res);
            });
        });
      });
    });
  }

  future<response> get(mes::mestring url) {
    return request("GET", url, "", {});
  }

  future<response> request(
      mes::mestring_cat method,
      mes::mestring url,
      mes::mestring_cat content,
      std::vector<std::pair<mes::mestring_cat, mes::mestring_cat>> headers) {
    client* c = this;
    auto p_hpa = mes::static_split_first<2>(url, "://");
    auto proto = std::get<0>(p_hpa);
    auto hpa = std::move(std::get<1>(p_hpa));
    auto it = mes::find_it(hpa, "/");
    auto path = mes::substr(it, hpa.end());
    if (path == "") {
      path = mes::make_mestring("/");
    }
    auto hp = mes::substr(hpa.begin(), it);
    int port = 0;
    if (proto == "http") {
      port = 80;
    } else if (proto == "https") {
      port = 443;
    } else {
      throw std::runtime_error("Unknown protocol");
    }
    auto col = mes::find_it(hp, ":");
    auto host = mes::substr(hp.begin(), col);
    if (col != hp.end()) {
      ++col;
      port = mes::parse_int(mes::substr(col, hp.end()));
    }
    return net::resolve_tcp(r_, host, port).then([t=c, path=path, proto=proto, host=host](auto ep) mutable {
      if (proto == "http") {
        return net::connect_to(t->r_, ep).then([](auto s) -> std::unique_ptr<net::readwritable> {
          return std::make_unique<decltype(s)>(std::move(s));
        });
      }
      return net::connect_default_ssl_to(t->r_, ep, host).then([](auto s) -> std::unique_ptr<net::readwritable> {
        return std::make_unique<decltype(s)>(std::move(s));
      });
    }).then([headers=std::move(headers), content=content, method=method, me=this, path=path, host=host](std::unique_ptr<net::readwritable> skt) {
      return do_with(std::move(skt), [headers=std::move(headers), content=content, method=method, me=me, host=host, path=path](auto& s) {
        return me->request_over_socket(method, host, path, s.get(), content, headers);
      });
    });
  }

private:

  template<class T>
  static future<> read_headers(parser<T> * p, reactor * r, response * res)  {
    return p->read_up_to("\r\n").then([p=p, r=r, res=res](auto m) {
      if (m.size() == 0) {
        return r->make_ready_future<>();
      } else {
        auto k_v = mes::static_split_first<2>(m, ':');
        std::string k = std::move(std::get<0>(k_v));
        std::string v = mes::lstrip(std::move(std::get<1>(k_v)));
        res->headers.push_back(std::make_pair(std::move(k), std::move(v)));
        return read_headers(p, r, res);
      }
    });
  }

  template<class T>
  static future<mes::mestring_cat> read_chunked(parser<T> * p, reactor * r, mes::mestring_cat res)  {
    return p->read_up_to("\r\n").then([p=p, r=r, res=std::move(res)](auto m) {
      auto val = mes::substr(m.begin(), mes::find_it(m, ";"));
      auto count = mes::parse_int_16(val);
      if (count == 0) {
        return p->read_n(2).then([res=std::move(res)](auto x) {return res;});
      } else {
        return p->read_n(count).then([p=p, res=std::move(res)](auto me) mutable {
          res += me;
          return p->read_n(2).then([res=std::move(res)](auto x) {return res;});
        }).then([p=p, r=r](auto res){
          return read_chunked(p, r, std::move(res));
        });
      }
    });
  }

  reactor * r_;
};


inline decltype(auto) get(reactor * r, mes::mestring url) {
  return do_with(client{r}, [url=url](client & c) {
    return c.get(url);
  });
}

}
