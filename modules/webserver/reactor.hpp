#pragma once

#include <deque>
#include <iostream>

#include <boost/asio.hpp>
#include "future/future.hh"

class reactor {
  public:
    reactor(): io_service_(1){}

    void run() {
      io_service_.run();
    }

    void stop() {

    }

    void add_task(std::unique_ptr<task> t) {
      _queue.push_back(std::move(t));
      schedule_work();
    }

  private:
    void schedule_work() {
      io_service_.dispatch(
          [this]() mutable {
            run_work();
          }
      );
    }

    void run_work() {
      if(!_queue.empty()) {
        _queue.front()->run();
        _queue.pop_front();
      }
      if(!_queue.empty()) {
        schedule_work();
      }
    }

    std::deque<std::unique_ptr<task>> _queue;

  public:
    boost::asio::io_service io_service_;
};

namespace net {



future<boost::asio::ip::tcp::resolver::iterator> resolve_tcp(
    reactor* r, const boost::asio::ip::tcp::resolver::endpoint_type& endpoint) {
  promise<boost::asio::ip::tcp::resolver::iterator> ret_promise;
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
