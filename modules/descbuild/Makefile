
include ../../common.mk

SRCS := test.cpp
INCLUDES := -I../reax/ -I../mestring/ -I$(NATIVE_PREFIX)/yaml-cpp/usr/local/include/

LIBS := -lyaml-cpp -L$(NATIVE_PREFIX)/yaml-cpp/usr/local/lib/

.PHONY: test
test:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SRCS) -o test -lboost_system -lpthread $(INCLUDES) $(LIBS)
	./test

proto:
	capnp compile -oc++ target.capnp

