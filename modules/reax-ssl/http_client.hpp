
#include "reactor.hpp"
#include "mestring.hpp"

struct response {
  response(const std::string& s): content(s){}

  std::string content;
  std::string first_line;
  std::vector<std::pair<std::string, std::string>> headers;
};

template<class T>
class connection {
public:
  connection(reactor* r, T&& t):
    _r(r), _t(std::forward<T>(t)){}

  connection(connection&& c):
    _r(c._r), _t(std::forward<T>(c._t)){}


private:
  reactor * _r;
  T _t;
};

template<class T>
auto make_connection(reactor * r, T&& t) {
  return connection<T>(r, std::forward<T>(t));
}

future<response> http_get(reactor * r, const mes::mestring& url) {
  auto p = r->make_promise<response>();
  auto ret = p.get_future();
  boost::asio::ip::tcp::resolver::query query("localhost", "80");
  net::resolve_tcp(r, query).then([](auto ep) {
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
}
