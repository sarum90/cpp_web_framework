#pragma once

#include <deque>
#include <iostream>

#include <boost/asio.hpp>
#include "future/future.hh"
#include "future/do_with.hh"
#include "mestring.hpp"

class reactor : public scheduler {
  public:
    reactor(): io_service_(1){}

    template <class... T>
    promise<T...> make_promise() {
      return promise<T...>(this);
    }

    template <class... T>
    future<T...> make_ready_future(T&& ... args) {
      return ::make_ready_future<T...>(this, std::forward<T>(args)...);
    }

    template <class... T>
    future<T...> make_exception_future(std::exception_ptr eptr) {
      return ::make_exception_future<T...>(this, eptr);
    }

    void run() {
      io_service_.run();
      if (!_exception_queue.empty()) {
        std::rethrow_exception(_exception_queue.front());
      }
    }

    void stop() {

    }

    void schedule(std::unique_ptr<task> t) final override {
      _queue.push_back(std::move(t));
      schedule_work();
    }


    void report_failed_future(std::exception_ptr eptr) final override {
      _exception_queue.push_back(eptr);
      //try {
      //    if (eptr) {
      //        std::rethrow_exception(eptr);
      //    }
      //} catch(const std::exception& e) {
      //    std::cerr << "Excdafseptional future ignored: " <<  e.what() << std::endl;
      //}
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
    std::deque<std::exception_ptr> _exception_queue;

  public:
    boost::asio::io_service io_service_;
};

struct executing_reactor {
  ~executing_reactor() {r.run();}
  reactor * get_reactor() {return &r;}
  reactor r;
};

template <class T>
void schedule(reactor& r, T&& t) {
  r.schedule(make_task(std::forward<T>(t)));
}

namespace reax {

template <class T>
class concluding_ptr {
  public:
    concluding_ptr(std::unique_ptr<T> t): t_(std::move(t)){}

    concluding_ptr(const concluding_ptr& other) = delete;
    concluding_ptr(concluding_ptr&& other) = default;

    concluding_ptr& operator=(const concluding_ptr& other) = delete;
    concluding_ptr& operator=(concluding_ptr&& other) = default;

    ~concluding_ptr() {
      // Conclude t_, and delete it once it is concluded.
      auto * tt = t_.get();
      tt->conclude().then([t=std::move(t_)](){});
    }

    decltype(auto) get() {
      return t_->get();
    }

    decltype(auto) operator->() {
      return t_.get();
    }

    decltype(auto) operator*() {
      return *t_;
    }

    explicit operator bool() const {
      return t_;
    }

  private:
    std::unique_ptr<T> t_;
};

template<typename T, typename... Args>
concluding_ptr<T> make_concluding(Args&&... args)
{
    return concluding_ptr<T>(std::make_unique<T>(std::forward<Args>(args)...));
}


template <class T>
future<> write(reactor * r, T* simple_writable, const mes::mestring& m) {
  if (m.size() == 0) {
    return r->make_ready_future();
  }
  return simple_writable->write(&m[0], m.size()).then([r=r, sw=simple_writable, m=m](std::size_t s) {
    return write(r, sw, mes::make_mestring(&m[s], m.size()-s));
  });
}

namespace {

template <class T, class U>
future<> write(reactor * r, T* sw, U mci, U end) {
  return reax::write<T>(r, sw, *mci).then([=]() mutable {
    ++mci;
    if (mci == end) {
      return r->make_ready_future();
    } else {
      return write(r, sw, mci, end);
    }
  });
}

}

template <class T>
future<> write(reactor * r, T* simple_writable, const mes::mestring_cat& mc) {
  auto& mcs = mc.mestrings();
  return write(r, simple_writable, mcs.begin(), mcs.end());
}

namespace {

template <class T>
future<> readsomeline(reactor * r, std::string* s, T* simple_readable) {
  return do_with(std::array<char,1>{}, [s=s, r=r, sr=simple_readable](std::array<char, 1>& buff) {
    return sr->read(&buff[0], 1).then([s=s, r=r, sr=sr, &buff=buff](size_t sz) {
      if (sz > 0) {
        std::string v = "a";
        v[0] = buff[0];
        (*s) += v;
        if (v == "\n") {
          return r->make_ready_future<>();
        }
      }
      return readsomeline(r, s, sr);
    });
  });
}

}

template <class T>
future<std::string> readline(reactor * r, T* simple_readable) {
  return do_with(std::string{}, [=](std::string& s) {
      return readsomeline(r, &s, simple_readable).then([&s=s](){
          return std::move(s);
      });
  });
}

}
