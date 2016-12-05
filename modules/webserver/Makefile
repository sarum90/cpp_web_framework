
include ../../common.mk

SRCS=test.cpp reactor.cpp

.PHONY: all
all:
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lboost_system -lpthread'
	$(WRAPPER) python integration_test.py
