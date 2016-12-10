
include ../../common.mk
SRCS:=reactor.cpp test_reactor.cpp

.PHONY: all
all:
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lmettle -pthread -lboost_system'

.PHONY: test
test: all
	$(WRAPPER) mettle --output=verbose --color=always ./test
