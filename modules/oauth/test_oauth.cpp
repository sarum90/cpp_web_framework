#include <string>

#include "oauth.hpp"

#include <mettle.hpp>
using namespace meoauth;
using namespace mettle;

bool_attr rs("requires_secrets", test_action::skip);

decltype(auto) requires_secrets(const std::string& s) {
  return rs("Requires Test secrets: " + s);
}

suite<> oauth_basic("oauth to google.", {requires_secrets("Uses Google Storage.")},  [](auto &_) {

  _.test("Simple oauth and get bucket", []() {
    std::string at;
    std::string c;
    std::unique_ptr<http::client> cl;
    {
      executing_reactor e;
      cl = std::make_unique<http::client>(&e.r);

      authenticate(
          cl.get(),
          "https://www.googleapis.com/auth/devstorage.read_only",
          "../../travis-secrets/decrypted/mewert-cpp-project-6899912be999.json"
      ).then([&at=at, cl=cl.get()](auto x) {
        at = x;
        return cl->request(
            "GET",
            "https://www.googleapis.com/storage/v1/b/mewert-cpp-project-test-resources/o/test%2ftest?alt=media",
            "",
            {{"Authorization", mes::make_mestring(at)}}
        );
      }).then([&c=c](auto x) {
        c = x.content;
      });
    }
    expect(c, equal_to("Hello World\n"));
  });

});
