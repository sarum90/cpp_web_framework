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
        auto ptr = std::move(_queue.front());
        _queue.pop_front();
        ptr->run();
      }
      if(!_queue.empty()) {
        schedule_work();
      }
    }

    std::deque<std::unique_ptr<task>> _queue;

  public:
    boost::asio::io_service io_service_;
};

struct reactor_setter {
  public:
    reactor_setter(reactor* newval);

    ~reactor_setter();
  private:
    reactor* oldval;
};

namespace net {

future<boost::asio::ip::tcp::resolver::iterator> resolve_tcp(
    reactor* r, const boost::asio::ip::tcp::resolver::endpoint_type& endpoint);

}
