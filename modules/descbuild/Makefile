
include ../../common.mk

SRCS := test.cpp target.capnp.c++
INCLUDES := -I../reax/ -I../mestring/ -I$(NATIVE_PREFIX)/yaml-cpp/usr/local/include/

LIBS := -lcapnp -lkj -lyaml-cpp -L$(NATIVE_PREFIX)/yaml-cpp/usr/local/lib/

.PHONY: test
test: proto
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SRCS) -o test -lboost_system -lpthread -lmettle $(INCLUDES) $(LIBS)
	mettle --show-terminal --output=verbose --color=always $(METTLE_ARGS) ./test

proto:
	capnp compile -oc++ target.capnp

