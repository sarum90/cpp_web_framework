

.PHONY: all
all:
	cd modules && $(MAKE)

.PHONY: emsdk
emsdk:
	cd third_party/emsdk/ && $(MAKE)

.PHONY: deps
deps:
	cd third_party && $(MAKE)

.PHONY: clean
clean:
	cd third_party && $(MAKE) clean
