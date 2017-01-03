#include "yaml-capnproto.hpp"
#include "target.capnp.h"

#include <mettle.hpp>
using namespace mettle;

suite<> cyaml_basic("simple_encode", [](auto &_) {

  _.test("Parse Yaml to serialized capnproto.", []() {
    auto yaml_blob = ""
      "headers: [a.h, b.h]\n"
      "sources: [a.cpp, b.cpp]\n"
      "dependencies: [foo, bar]\n"
      "description: cat\n"
      "vals: [0, 2, 3]\n"
      "ver: 123\n";

	  YAML::Node yml = YAML::Load(yaml_blob);
		std::string val_contiguous;
		{
			auto buff = cyaml::encode_yaml_to<CppTarget>(yml);
      auto c = buff.to_mestring_cat();
      val_contiguous = c;
    }

		auto message = cyaml::make_reader(val_contiguous);
		CppTarget::Reader cpp_target = message.getRoot<CppTarget>();
    expect(cpp_target.getHeaders(), each({"a.h", "b.h"}, equal_to<std::string>));
    expect(cpp_target.getSources(), each({"a.cpp", "b.cpp"}, equal_to<std::string>));
    expect(cpp_target.getDependencies(), each({"foo", "bar"}, equal_to<std::string>));
    expect(cpp_target.getDescription(), equal_to("cat"));
    expect(cpp_target.getVals(), each({0, 2, 3}, equal_to<const int&>));
    expect(cpp_target.getVer(), equal_to(123));
  });

  _.test("Serialize capnproto to JSON.", []() {
      capnp::MallocMessageBuilder builder;
      auto target = builder.initRoot<CppTarget>();
      target.setHeaders({"a.h", "b.h"});

      auto s = cyaml::to_json_yaml(target.asReader());

      expect(s, equal_to("{\"headers\": [\"a.h\", \"b.h\"], \"ver\": 0}"));
  });

  _.test("Serialize capnproto to YAML", []() {
      capnp::MallocMessageBuilder builder;
      auto target = builder.initRoot<CppTarget>();
      target.setHeaders({"a.h", "b.h"});

      auto s = cyaml::to_yaml(target.asReader());

      auto result = ""
        "headers:\n"
        "  - a.h\n"
        "  - b.h\n"
        "ver: 0";

      expect(s, equal_to(result));
  });

});
