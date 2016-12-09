
include ../../common.mk

SRCS=test.cpp reactor.cpp

.PHONY: run
run: client.js
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lboost_system -lpthread'
	$(WRAPPER) ./test

.PHONY: all
all:
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lboost_system -lpthread'
	$(WRAPPER) python integration_test.py

client.js: client.cpp
	$(CLIENT_WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} client.cpp -o client.js'
