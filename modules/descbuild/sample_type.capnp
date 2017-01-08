@0x80a0d71e101a9361;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("sample::test");

struct Inner @0xe93515a2734b9d83 {
  text @0 :Text;
  int @1 :Int64;
  recurse @2 :Inner;
}

struct Outer @0x92bc03023b2039ca {
  text @0 :Text;
  int64 @1 :Int64;
  uint64 @2 :UInt64;
  int32 @3 :Int32;
  uint32 @4 :UInt32;
  int16 @5 :Int16;
  uint16 @6 :UInt16;
  int8 @7 :Int8;
  uint8 @8 :UInt8;
  float32 @9 :Float32;
  float64 @10 :Float64;
  data @11 :Data;
  str @12 :Inner;

  intlist @13 :List(Int64);
  strlist @14 :List(Inner);

  enm @15 :Enm;

  enum Enm {
    a @0;
    b @1;
    c @2;
  }

  uni :union {
    def @16 :Void;
    void @17 :Void;
    text @18 :Text;
    int64 @19 :Int64;
  }

  bool @20 :Bool;

  lli  @21 :List(List(Int64));
}


