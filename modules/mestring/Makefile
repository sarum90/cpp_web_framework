include ../../common.mk
SRCS:= ./test_mestring.cpp mestring.cpp

.PHONY: default
default: test

.PHONY: all
all:
	#$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lmettle -pthread -lboost_system'
	bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lmettle -pthread -lboost_system'

.PHONY: test
test: all
	#$(WRAPPER) mettle --show-terminal --output=verbose --color=always ./test
	mettle --show-terminal --output=verbose --color=always ./test
