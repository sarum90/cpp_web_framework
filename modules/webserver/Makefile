
include ../../common.mk

SRCS := test.cpp ../reax/reactor.cpp
INCLUDES := -I../reax/

.PHONY: run
run: server
	$(WRAPPER) ./test

.PHONY: test
test: server
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lboost_system -lpthread $(INCLUDES)'
	$(WRAPPER) python integration_test.py

.PHONY: server
server: client.js
	$(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} $(SRCS) -o test -lboost_system -lpthread $(INCLUDES)'

client.js: client.cpp
	$(CLIENT_WRAPPER) bash -c '$${CXX} $${CXXFLAGS} $${LDFLAGS} client.cpp -o client.js $(INCLUDES)'
