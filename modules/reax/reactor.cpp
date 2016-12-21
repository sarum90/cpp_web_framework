
#include "reactor.hpp"
#include "net.hpp"

void report_failed_future(scheduler * s, std::exception_ptr eptr) {
  s->report_failed_future(eptr);
}

__thread bool g_need_preempt;

namespace net {

future<boost::asio::ip::tcp::resolver::iterator> resolve_tcp(
    reactor* r, const boost::asio::ip::tcp::resolver::endpoint_type& endpoint) {
  auto ret_promise = r->make_promise<boost::asio::ip::tcp::resolver::iterator>();
  auto retval = ret_promise.get_future();
  auto rr = std::make_unique<boost::asio::ip::tcp::resolver>(r->io_service_);
  boost::asio::ip::tcp::resolver& resolver = *rr;
  auto x = [p=std::move(ret_promise), rr=std::move(rr)](const auto& error, auto iter) mutable {
    p.set_value(iter);
  };
  auto handler = std::make_shared<decltype(x)>(std::move(x));
  resolver.async_resolve(endpoint, [h=std::move(handler)](const auto& error, auto iter) mutable {
      return (*h)(error, iter);
  });
  return std::move(retval);
}

    //boost::asio::ip::tcp::resolver::query query(argv[1], argv[2]);

future<boost::asio::ip::tcp::resolver::iterator> resolve_tcp(
    reactor* r, const boost::asio::ip::tcp::resolver::query& query) {
  auto ret_promise = r->make_promise<boost::asio::ip::tcp::resolver::iterator>();
  auto retval = ret_promise.get_future();
  auto rr = std::make_unique<boost::asio::ip::tcp::resolver>(r->io_service_);
  boost::asio::ip::tcp::resolver& resolver = *rr;
  auto x = [p=std::move(ret_promise), rr=std::move(rr)](const auto& error, auto iter) mutable {
    if (error) {
        std::cout << "Error! " << error << std::endl;
        p.set_exception(error);
    } else {
      p.set_value(iter);
    }
  };
  auto handler = std::make_shared<decltype(x)>(std::move(x));
  resolver.async_resolve(query, [h=std::move(handler)](const auto& error, auto iter) mutable {
      return (*h)(error, iter);
  });
  return std::move(retval);
}

future<net::ipv4_endpoint> resolve_tcp(
    reactor* r, const mes::mestring& host, unsigned short port) {
  boost::asio::ip::tcp::resolver::query query(host, "");
  return resolve_tcp(r, query).then([r=r, port=port](auto ep) {
    return net::ipv4_endpoint{ep->endpoint().address().to_v4().to_ulong(), port};
  });
}

}
