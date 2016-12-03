

#include "webserver.hpp"

using namespace webserver;

static reactor** current_reactor() {
  static reactor* r = nullptr;
  return &r;
}

static reactor& engine() {
	return **current_reactor();
}

static void set_reactor(reactor* val) {
  *current_reactor() = val;
}

struct reactor_setter {
  public:
    reactor_setter(reactor* newval)
      : oldval(*current_reactor()){
      set_reactor(newval);
    }

    ~reactor_setter() {
      set_reactor(oldval);
    }
  private:
    reactor* oldval;
};

void schedule(std::unique_ptr<task> t) {
    engine().add_task(std::move(t));
}

auto setup_server(reactor* r) {
  auto s = create_server([](auto& _) constexpr {
    _.addRoute("/", [](auto& req, auto& resp) constexpr {
      return plaintext_response(resp, "Hello World");
    });
  });
  register_server(s, r);
  return s;
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

int main(int argc, char ** argv) {
  reactor r;
  reactor_setter rs(&r);
  auto x = setup_server(&r);
  r.run();
}
