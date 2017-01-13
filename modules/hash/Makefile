
include ../../common.mk

SRCS := test.cpp ../mestring/mestring.cpp
INCLUDES := -I$(NATIVE_PREFIX)/smhasher/usr/local/include/ -I../

LIBS := -lmmh3 -L$(NATIVE_PREFIX)/smhasher/usr/local/lib/

.PHONY: test
test:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SRCS) -g -o test -lboost_system -lpthread -lmettle $(INCLUDES) $(LIBS)
	mettle --show-terminal --output=verbose --color=always $(METTLE_ARGS) ./test
