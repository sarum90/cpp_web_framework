#pragma once

#include "mestring.hpp"

#include "yaml-cpp/yaml.h"
#include <capnp/schema.h>
#include <capnp/dynamic.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>

namespace cyaml {

class encoded_capnproto {
public:
  encoded_capnproto(): message(std::make_unique<::capnp::MallocMessageBuilder>()) {
  }

  mes::mestring_cat to_mestring_cat() {
    auto a = message->getSegmentsForOutput();
    mes::mestring_cat retval;
    segments.set(a.size() - 1);
    seg_lengths.resize(a.size());
    retval += mes::make_mestring(reinterpret_cast<const char *>(&segments), sizeof(segments));
    retval += mes::make_mestring(
        reinterpret_cast<const char *>(&seg_lengths[0]),
        sizeof(seg_lengths[0]) * a.size()
    );
    int ind = 0;
    for (auto& i: a) {
      seg_lengths[ind].set(i.size());
      typedef decltype(*i.begin()) val_t;
      retval += mes::make_mestring(reinterpret_cast<const char *>(i.begin()), sizeof(val_t) * i.size());
      ind++;
    }
    return retval;
  }

  ::capnp::_::WireValue<uint32_t> segments;
  std::vector<::capnp::_::WireValue<uint32_t>> seg_lengths;
  std::unique_ptr<::capnp::MallocMessageBuilder> message;
};

template<class PROTO, class YY>
encoded_capnproto encode_yaml_to(const YY& yaml) {

  encoded_capnproto p;

  auto& message = *p.message;

  auto schema = capnp::Schema::from<PROTO>();

  auto outmessage = message.initRoot<::capnp::DynamicStruct>(schema);

  for (auto const& t: yaml) {
    if (t.second.IsSequence()) {
      auto v = outmessage.init(
          t.first.template as<std::string>().c_str(),
          t.second.size()
      ).template as<::capnp::DynamicList>();
      for (int i = 0; i < t.second.size(); i++) {
        if(v[i].getType() == capnp::DynamicValue::TEXT) {
          v.set(i, t.second[i].template as<std::string>().c_str());
        } else {
          v.set(i, t.second[i].template as<int>());
        }
      }
    } else {
      auto s = t.first.template as<std::string>();
      const auto* c = s.c_str();
      if (outmessage.get(c).getType() == capnp::DynamicValue::TEXT) {
        outmessage.set(c, t.second.template as<std::string>().c_str());
      } else {
        outmessage.set(c, t.second.template as<int>());
      }
    }
  }

  return p;
}

capnp::FlatArrayMessageReader make_reader(const std::string& s) {
  typedef ::capnp::word val_t;
  auto x = kj::ArrayPtr<const val_t>{
      reinterpret_cast<const val_t *>(s.c_str()), s.size() / sizeof(val_t)
  };
  return capnp::FlatArrayMessageReader(x);
}

namespace {

inline void to_yaml_impl(capnp::DynamicValue::Reader value,  YAML::Emitter& out) {
  switch (value.getType()) {
    case capnp::DynamicValue::BOOL: {
      out << value.as<bool>();
      break;
    }
    case capnp::DynamicValue::FLOAT: {
      out << value.as<double>();
      break;
    }
    case capnp::DynamicValue::UINT: {
      out << value.as<unsigned int>();
      break;
    }
    case capnp::DynamicValue::INT: {
      out << value.as<int>();
      break;
		}
    case capnp::DynamicValue::DATA: {
      auto d = value.as<capnp::Data>();
      out << YAML::Binary(d.begin(), d.size());
      break;
		}
    case capnp::DynamicValue::TEXT: {
      out << value.as<capnp::Text>().cStr();
      break;
		}
    case capnp::DynamicValue::STRUCT: {
      auto structValue = value.as<capnp::DynamicStruct>();
      out << YAML::BeginMap;
      for (auto field: structValue.getSchema().getFields()) {
        if (!structValue.has(field)) continue;
        out << YAML::Key << field.getProto().getName().cStr();
        out << YAML::Value;
				to_yaml_impl(structValue.get(field), out);
      }
      out << YAML::EndMap;
			
      break;
    }
    case capnp::DynamicValue::LIST: {
      out << YAML::BeginSeq;
      for (auto element: value.as<capnp::DynamicList>()) {
        to_yaml_impl(element, out);
      }
      out << YAML::EndSeq;
      break;
    }
    case capnp::DynamicValue::ENUM:
    case capnp::DynamicValue::UNKNOWN:
    case capnp::DynamicValue::VOID:
    case capnp::DynamicValue::CAPABILITY:
    case capnp::DynamicValue::ANY_POINTER:
      throw std::runtime_error("Unhandled capnproto type");
  }
  
}

}

template<class PROTO>
std::string to_json_yaml(const PROTO& p) {
  auto reader = capnp::toDynamic(p);

  YAML::Emitter out;
  out << YAML::Flow;
  out << YAML::DoubleQuoted;
  out << YAML::TrueFalseBool;

  to_yaml_impl(reader, out);

  return std::string(out.c_str());
}

template<class PROTO>
std::string to_yaml(const PROTO& p) {
  auto reader = capnp::toDynamic(p);

  YAML::Emitter out;

  to_yaml_impl(reader, out);

  return std::string(out.c_str());
}



}
