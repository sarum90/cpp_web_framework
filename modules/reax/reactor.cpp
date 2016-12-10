
#include "reactor.hpp"

static reactor** current_reactor() {
  static __thread reactor* r = nullptr;
  return &r;
}

static reactor& engine() {
	return **current_reactor();
}

static void set_reactor(reactor* val) {
  *current_reactor() = val;
}

reactor_setter::reactor_setter(reactor* newval) : oldval(*current_reactor()){
  set_reactor(newval);
}

reactor_setter::~reactor_setter() {
  set_reactor(oldval);
}

void report_failed_future(std::exception_ptr eptr) {
	try {
			if (eptr) {
					std::rethrow_exception(eptr);
			}
	} catch(const std::exception& e) {
      std::cerr << "Exceptional future ignored: " <<  e.what() << std::endl;
	}
}

__thread bool g_need_preempt;


namespace net {

future<boost::asio::ip::tcp::resolver::iterator> resolve_tcp(
    reactor* r, const boost::asio::ip::tcp::resolver::endpoint_type& endpoint) {
  auto ret_promise = r->make_promise<boost::asio::ip::tcp::resolver::iterator>();
  auto retval = ret_promise.get_future();
  boost::asio::ip::tcp::resolver resolver(r->io_service_);
  auto x = [p=std::move(ret_promise)](const auto& error, auto iter) mutable {
    p.set_value(iter);
  };
  auto handler = std::make_shared<decltype(x)>(std::move(x));
  resolver.async_resolve(endpoint, [h=std::move(handler)](const auto& error, auto iter) mutable {
      return (*h)(error, iter);
  });
  return std::move(retval);
}

}
