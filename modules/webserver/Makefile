
include ../../common.mk

.PHONY: all
all:
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} test.cpp -o test -lboost_system -lpthread'
	$(WRAPPER) ./test
	#$(WRAPPER) valgrind --leak-check=full  --track-origins=yes ./test
