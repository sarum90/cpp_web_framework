
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
          return p->r_->make_ready_future(std::move(resp));
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
          return p->r_->make_ready_future(std::move(resp));
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
          return p->r_->make_ready_future(std::move(mc));
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
          return p->r_->make_ready_future(std::move(mc));
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

  future<response> get(mes::mestring url) {
    client* c = this;
    auto p_hpa = mes::static_split_first<2>(url, "://");
    auto hpa = std::move(std::get<1>(p_hpa));
    auto it = mes::find_it(hpa, "/");
    auto path = mes::substr(it, hpa.end());
    auto hp = mes::substr(hpa.begin(), it);
    int port = 80;
    auto col = mes::find_it(hp, ":");
    auto host = mes::substr(hp.begin(), col);
    if (col != hp.end()) {
      ++col;
      port = mes::parse_int(mes::substr(col, hp.end()));
    }
    return net::resolve_tcp(r_, host, port).then([t=c, path=path](auto ep) mutable {
      return net::connect_to(t->r_, ep);
    }).then([r=r_, path=path](auto socket) {
      auto skt = std::make_unique<net::socket>(std::move(socket));
      auto br = make_buffered_reader(r, skt.get());
      return do_with(std::move(skt), std::move(br), response{}, [r=r, path=path](auto& skt, auto& reader, auto& res) mutable {
        auto* s = skt.get();
        auto* rdr = &reader;
        mes::mestring_cat mc;
        mc += "GET ";
        mc += path;
        mc += " HTTP/1.1\r\n\r\n";
        return reax::write(r, s, std::move(mc))
        .then([r=r, s=s, rdr=rdr, res=&res]() mutable {
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
              }).then([&p=p, res=res]() {
                auto m = res->get_header("Content-Length", "-1");
                int len = mes::parse_int(m);
                return p.read_n(len);
              }).then([res=res](auto m) {
                res->content = std::move(m);
                return std::move(*res);
              });
          });
        });
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

  reactor * r_;
};


decltype(auto) get(reactor * r, mes::mestring url) {
  return do_with(client{r}, [url=url](client & c) {
    return c.get(url);
  });
}

}


/*future<response> http_get(reactor * r, const mes::mestring& url) {
  auto p = r->make_promise<response>();
  auto ret = p.get_future();
  net::resolve_tcp(r, "localhost", 80).then([](auto ep) {
    return net::ipv4_endpoint(ep->endpoint());
  }).then([r=r, p=std::move(p)](auto endpoint) mutable {
    auto s = std::make_unique<boost::asio::ip::tcp::socket>(r->io_service_);
    auto& socket = *s;
    boost::asio::ip::tcp::endpoint ep(endpoint);
    auto l =[r=r, s=std::move(s), p=std::move(p)](auto err, auto it) mutable {
      if (err) {
        throw err;
        //p.set_exception(err);
      } else {
        //return *it;
        //auto c = std::make_unique<connection<decltype(s)>>(make_connection(r, std::move(s)));

        //auto &conn = *c;

        //auto response;
        //conn.write("GET /test.txt HTTP/1.1\r\n\r\n").then([conn=conn]() {
        //  return conn.read_to(make_mestring("\r\n"));
        //}).then([conn=conn](auto& s ) {

        //})

        std::cout << "A" << std::endl;
        std::string req = "GET /test.txt HTTP/1.1\r\n\r\n";
        boost::asio::write(*s, boost::asio::buffer(&req[0], req.size()));
        std::cout << "B" << std::endl;
        std::cout << "B" << std::endl;
        std::array<char, 4096> buff;
        boost::system::error_code ec;
        size_t n = boost::asio::read(*s, boost::asio::buffer(buff), ec);
        std::cout << "C" << std::endl;
        std::cout << &buff[0] << std::endl;
        //p.set_value(response{"Hello world!!"});
      }
    };

    auto h = std::make_shared<decltype(l)>(std::move(l));
    boost::asio::async_connect(socket, &ep, [h=h](auto error, auto it) {
      (*h)(error, it);
    });
  });
  return ret;
}*/
