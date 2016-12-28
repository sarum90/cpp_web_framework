
include ../../common.mk
#SRCS:=reactor.cpp test_reactor.cpp test_net.cpp
SRCS:=reactor.cpp test_net.cpp

.PHONY: default
default: test

.PHONY: all
all:
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lmettle -pthread -lboost_system -lssl -lcrypto -I../mestring'

.PHONY: test
test: all
	$(WRAPPER) mettle --show-terminal --output=verbose --color=always ./test
