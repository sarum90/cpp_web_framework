
include ../../common.mk

SRCS=test.cpp reactor.cpp

.PHONY: all
all:
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lboost_system -lpthread'
	$(WRAPPER) ./test
	#$(WRAPPER) valgrind --leak-check=full  --track-origins=yes ./test
