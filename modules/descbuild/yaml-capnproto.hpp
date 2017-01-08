#pragma once

#include "mestring.hpp"

#include "yaml-cpp/yaml.h"
#include <capnp/schema.h>
#include <capnp/dynamic.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "base64/base64.hpp"

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

namespace {

void encode_into(capnp::DynamicStruct::Builder& b, const YAML::Node& y);

struct list_ops {
  template <class T>
  void set(T x) {list.set(index, x);}

  decltype(auto) init(){return list[index];}

  decltype(auto) init_list(int ii){return list.init(index, ii);}

  capnp::DynamicList::Builder list;
  int index;
};

struct struct_ops {
  template <class T>
  void set(T x) {str.set(key, x);}

  decltype(auto) init() {return str.init(key);}

  decltype(auto) init_list(int i) {return str.init(key, i);}

  capnp::DynamicStruct::Builder str;
  const char * key;
};

template<typename OPS>
void encode_val(::capnp::Type ty, const YAML::Node& y, OPS ops) {
    switch (ty.which()) {
      case capnp::schema::Type::DATA: {
        auto s = y.template as<std::string>();
        auto dec = std::string(base64_decode(mes::make_mestring(s)));
        auto ha = kj::heapArray<const capnp::byte>(dec.begin(), dec.end());
        ops.set(::capnp::Data::Reader(ha));
        break;
      }
      case capnp::schema::Type::TEXT:
        ops.set(y.template as<std::string>().c_str());
        break;
      case capnp::schema::Type::FLOAT32:
      case capnp::schema::Type::FLOAT64:
        ops.set(y.template as<double>());
        break;
      case capnp::schema::Type::INT64:
      case capnp::schema::Type::INT32:
      case capnp::schema::Type::INT16:
      case capnp::schema::Type::INT8:
        ops.set(y.template as<int>());
        break;
      case capnp::schema::Type::UINT64:
      case capnp::schema::Type::UINT32:
      case capnp::schema::Type::UINT16:
      case capnp::schema::Type::UINT8:
        ops.set(y.template as<unsigned int>());
        break;
      case capnp::schema::Type::BOOL:
        ops.set(y.template as<bool>());
        break;
      case capnp::schema::Type::VOID:
        break;
      case capnp::schema::Type::STRUCT: {
        auto ds = ops.init().template as<::capnp::DynamicStruct>();
        encode_into(ds, y);
        break;
      }
      case capnp::schema::Type::ENUM:
        ops.set(y.template as<std::string>().c_str());
        break;
      case capnp::schema::Type::LIST: {
        auto ll = ops.init_list(y.size()).template as<capnp::DynamicList>();
        for (int i = 0; i < y.size(); i++) {
          list_ops lo;
          lo.index = i;
          lo.list = ll;
          encode_val(ty.asList().getElementType(), y[i], lo);
        }
        break;
      }
      case capnp::schema::Type::INTERFACE:
      case capnp::schema::Type::ANY_POINTER:
        throw std::runtime_error("Unhandled encode_into");
        break;
    }
  };

void encode_into(capnp::DynamicStruct::Builder& b, const YAML::Node& y) {
  for (auto const& t: y) {
    const auto key = t.first.as<std::string>().c_str();
    auto ty = b.getSchema().getFieldByName(key).getType();
    if (ty.isList()) {
      auto v = b.init(
          key,
          t.second.size()
      ).as<::capnp::DynamicList>();
      for (int i = 0; i < t.second.size(); i++) {
        list_ops lo;
        lo.index = i;
        lo.list = v;
        encode_val(v.getSchema().getElementType(), t.second[i], lo);
      }
    } else {
      struct_ops so;
      so.str = b;
      so.key = key;
      encode_val(ty, t.second, so);
    }
  }
} 

} // anonymous namespace

template<class PROTO, class YY>
encoded_capnproto encode_yaml_to(const YY& yaml) {

  encoded_capnproto p;

  auto& message = *p.message;

  auto schema = capnp::Schema::from<PROTO>();

  auto outmessage = message.initRoot<::capnp::DynamicStruct>(schema);

  encode_into(outmessage, yaml);

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
      std::string str = base64_encode(mes::make_mestring(
            reinterpret_cast<const char *>(d.begin()), d.size()));
      out << str;
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
    case capnp::DynamicValue::ENUM: {
      auto enumValue =  value.as<capnp::DynamicEnum>();
      KJ_IF_MAYBE(enumerant, enumValue.getEnumerant()) {
        out << enumerant->getProto().getName().cStr();
      } else {
        out << enumValue.getRaw();
      }
      break;
	  }
    case capnp::DynamicValue::VOID:
      out << "1";
      break;
    case capnp::DynamicValue::UNKNOWN:
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
