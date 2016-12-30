include ../../common.mk
SRCS := test_oauth.cpp oauth.cpp ../reax/reactor.cpp ../mestring/mestring.cpp 
INCLUDES := -I../reax/ -I../mestring/ -I../base64/ -I../reax-ssl/

.PHONY: test
test: all
	mettle --show-terminal --output=verbose --color=always $(METTLE_ARGS) ./test

.PHONY: all
all:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SRCS) -o test -lmettle -pthread -lboost_system -lssl -lcrypto $(INCLUDES)
