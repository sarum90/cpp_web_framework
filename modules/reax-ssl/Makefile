
include ../../common.mk
#SRCS:=../reax/reactor.cpp boost-sample.cpp
SRCS := test_http_client.cpp ../reax/reactor.cpp ../mestring/mestring.cpp
INCLUDES := -I../reax/ -I../mestring/

.PHONY: test
test: all

.PHONY: test
test: all
	$(WRAPPER) mettle --show-terminal --output=verbose --color=always ./test

.PHONY: all
all:
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lmettle -pthread -lboost_system -lssl -lcrypto $(INCLUDES)'
