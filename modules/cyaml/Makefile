
include ../../common.mk

SRCS := test.cpp sample_type.capnp.c++ ../mestring/mestring.cpp
INCLUDES := -I../reax/ -I../mestring/ -I$(NATIVE_PREFIX)/yaml-cpp/usr/local/include/ -I../

LIBS := -lcapnp -lkj -lyaml-cpp -L$(NATIVE_PREFIX)/yaml-cpp/usr/local/lib/

.PHONY: test
test: proto
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SRCS) -g -o test -lboost_system -lpthread -lmettle $(INCLUDES) $(LIBS)
	mettle --show-terminal --output=verbose --color=always $(METTLE_ARGS) ./test

proto:
	capnp compile -oc++ sample_type.capnp -I$(NATIVE_PREFIX)/capnproto/usr/local/include/


