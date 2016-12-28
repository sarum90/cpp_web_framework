include ../../common.mk
SRCS:= ./test_base64.cpp ../mestring/mestring.cpp

.PHONY: default
default: test

.PHONY: all
all:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SRCS) -o test -lmettle -pthread -lboost_system -I../mestring

.PHONY: test
test: all
	mettle --show-terminal --output=verbose --color=always ./test
