
include ../../common.mk

.PHONY: test
test: build
	./test_mestring

.PHONY: build
build:
	$(CXX) -std=c++1z test_mestring.cpp mestring.cpp -o test_mestring
