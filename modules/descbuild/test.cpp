#include "yaml-capnproto.hpp"
#include "target.capnp.h"

#include <mettle.hpp>
using namespace mettle;

/*
int main(int argc, char ** argv) {
	YAML::Node doc = YAML::LoadFile("sample-dbuild.yaml");
	for(const auto& t: doc["cpptargets"]) {
		std::string val_contiguous;
		{
			auto buff = encode_yaml_to<CppTarget>(t.second);
      auto c = buff.to_mestring_cat();
      val_contiguous = c;
    }

		auto message = make_reader(val_contiguous);
		CppTarget::Reader cpp_target = message.getRoot<CppTarget>();
		for (const auto& i: cpp_target.getHeaders()) {
			std::cout << i.cStr() << std::endl;
		}
		
	}
  return 0;
}
*/


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
    expect(cpp_target.getHeaders(), array(equal_to("a.h"), equal_to("b.h")));
		/*for (const auto& i: cpp_target.getHeaders()) {
			std::cout << i.cStr() << std::endl;
		}*/


  });

});
