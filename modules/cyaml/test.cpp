#include "yaml-capnproto.hpp"
#include "sample_type.capnp.h"

#include <mettle.hpp>
using namespace mettle;

suite<> cyaml_basic("simple_encode", [](auto &_) {

  _.test("Basic coverage of all types", []() {
    auto yaml_blob = ""
      "bool: true\n"
      "text: amessage\n"
      "int64: -1000\n"
      "int32: -1001\n"
      "int16: -101\n"
      "int8: -3\n"
      "uint64: 2000\n"
      "uint32: 2001\n"
      "uint16: 201\n"
      "uint8: 2\n"
      "float32: 123.25\n"
      "float64: 123.125\n"
      "data: \"/wClpQ8P8PA=\"\n"
      "str:\n"
      " text: t\n"
      " int: 12\n"
      " recurse:\n"
      "  text: u\n"
      "  int: 24\n"
      "intlist: [0,1,2,3,4]\n"
      "strlist:\n"
      " - text: cat\n"
      "   int: 12\n"
      " - text: dog\n"
      "   int: 21\n"
      "enm: b\n"
      "uni:\n"
      "  text: val\n"
      "lli: [[1],[],[2,3]]\n"
      "";

	  YAML::Node yml = YAML::Load(yaml_blob);
		std::string val_contiguous;
		{
			auto buff = cyaml::encode_yaml_to<sample::test::Outer>(yml);
      auto c = buff.to_mestring_cat();
      val_contiguous = c;
    }

		auto message = cyaml::make_reader(val_contiguous);
    auto outer = message.getRoot<sample::test::Outer>();
    expect(outer.getBool(), equal_to(true));
    expect(outer.getText(), equal_to("amessage"));
    expect(outer.getInt64(), equal_to(-1000));
    expect(outer.getInt32(), equal_to(-1001));
    expect(outer.getInt16(), equal_to(-101));
    expect(outer.getInt8(), equal_to(-3));
    expect(outer.getUint64(), equal_to(2000));
    expect(outer.getUint32(), equal_to(2001));
    expect(outer.getUint16(), equal_to(201));
    expect(outer.getUint8(), equal_to(2));
    expect(outer.getFloat32(), equal_to(123.25));
    expect(outer.getFloat64(), equal_to(123.125));
    auto d = outer.getData();
    expect(base64_encode(mes::make_mestring_u(d.begin(), d.size())), equal_to(base64_encode(mes::mestring("\xff\x00\xa5\xa5\x0f\x0f\xf0\xf0"))));
    expect(outer.getStr().getText(), equal_to("t"));
    expect(outer.getStr().getInt(), equal_to(12));
    expect(outer.getStr().getRecurse().getText(), equal_to("u"));
    expect(outer.getStr().getRecurse().getInt(), equal_to(24));

    expect(outer.getIntlist(), each({0, 1, 2, 3, 4}, equal_to<const int&>));
    expect(outer.getStrlist()[0].getText(), equal_to("cat"));
    expect(outer.getStrlist()[0].getInt(), equal_to(12));
    expect(outer.getStrlist()[1].getText(), equal_to("dog"));
    expect(outer.getStrlist()[1].getInt(), equal_to(21));

    expect(outer.getEnm(), equal_to(sample::test::Outer::Enm::B));
    expect(outer.getUni().getText(), equal_to("val"));

    expect(outer.getLli().size(), equal_to(3));
    expect(outer.getLli()[0].size(), equal_to(1));
    expect(outer.getLli()[0][0], equal_to(1));
    expect(outer.getLli()[1].size(), equal_to(0));
    expect(outer.getLli()[2].size(), equal_to(2));
    expect(outer.getLli()[2][0], equal_to(2));
    expect(outer.getLli()[2][1], equal_to(3));
  });

  _.test("Serialize capnproto to JSON.", []() {
      capnp::MallocMessageBuilder builder;
      auto obj = builder.initRoot<sample::test::Outer>();
      obj.setText("ttt");
      obj.setInt64(-1234);
      obj.setInt32(-123);
      obj.setInt16(-12);
      obj.setInt8(-1);
      obj.setUint64(1234);
      obj.setUint32(123);
      obj.setUint16(12);
      obj.setUint8(1);
      obj.setFloat64(1234.25);
      obj.setFloat32(1234.125);
      auto st = mes::make_mestring("\xff\x00\xa5\x5a", 4);
      auto ha = kj::heapArray<const capnp::byte>(st.begin(), st.end());
      obj.setData(ha);
      obj.getStr().setText("abc");
      obj.getStr().setInt(42);
      obj.getStr().getRecurse().setText("ddd");
      obj.getStr().getRecurse().setInt(98);
			obj.setBool(true);
			auto lli = obj.initLli(2);
			lli.init(0, 0);
			auto lll = lli.init(1, 2);
			lll.set(0, 99);
			lll.set(1, 88);

			auto s = cyaml::to_json_yaml(obj.asReader());
			expect(s, equal_to(std::string("{"
				"\"text\": \"ttt\", "
				"\"int64\": -1234, "
				"\"uint64\": 1234, "
				"\"int32\": -123, "
				"\"uint32\": 123, "
				"\"int16\": -12, "
				"\"uint16\": 12, "
				"\"int8\": -1, "
				"\"uint8\": 1, "
				"\"float32\": 1234.125, "
				"\"float64\": 1234.25, "
				"\"data\": \"_wClWg==\", "
				"\"str\": {"
				  "\"text\": \"abc\", "
					"\"int\": 42, "
					"\"recurse\": {"
						"\"text\": \"ddd\", "
						"\"int\": 98}}, "
				"\"enm\": \"a\", "
				"\"uni\": {\"def\": \"1\"}, "
				"\"bool\": true, "
				"\"lli\": [[], [99, 88]]}")));


			auto ss = cyaml::to_yaml(obj.asReader());
			expect(ss, equal_to(std::string{
				"text: ttt\n"
				"int64: -1234\n"
				"uint64: 1234\n"
				"int32: -123\n"
				"uint32: 123\n"
				"int16: -12\n"
				"uint16: 12\n"
				"int8: -1\n"
				"uint8: 1\n"
				"float32: 1234.125\n"
				"float64: 1234.25\n"
				"data: _wClWg==\n"
				"str:\n"
				"  text: abc\n"
				"  int: 42\n"
				"  recurse:\n"
				"    text: ddd\n"
				"    int: 98\n"
				"enm: a\n"
				"uni:\n"
				"  def: 1\n"
				"bool: true\n"
				"lli:\n"
				"  -\n"
				"    []\n"
				"  -\n"
				"    - 99\n"
				"    - 88"}));

			for (const auto& sss: {s, ss}) {
				YAML::Node yml = YAML::Load(sss);
				std::string val_contiguous;
				{
					auto buff = cyaml::encode_yaml_to<sample::test::Outer>(yml);
					auto c = buff.to_mestring_cat();
					val_contiguous = c;
				}

				auto message = cyaml::make_reader(val_contiguous);
				auto outer = message.getRoot<sample::test::Outer>();

				expect(outer.toString().flatten(), ::mettle::equal_to(obj.asReader().toString().flatten()));
			}
  });

});
