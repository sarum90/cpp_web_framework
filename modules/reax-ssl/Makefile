
include ../../common.mk
#SRCS:=../reax/reactor.cpp boost-sample.cpp
SRCS := test.cpp ../reax/reactor.cpp ../mestring/mestring.cpp
INCLUDES := -I../reax/ -I../mestring/

.PHONY: test
test: all

.PHONY: test
test: all
	#$(WRAPPER) ./test
	valgrind ./test

.PHONY: all
all:
	#$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lmettle -pthread -lboost_system -lssl -lcrypto $(INCLUDES)'
	$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lmettle -pthread -lboost_system -lssl -lcrypto $(INCLUDES)
