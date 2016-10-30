

.PHONY: all
all:
	cd modules && $(MAKE)

.PHONY: cmake
cmake:
	cd third_party/cmake/ && $(MAKE)

.PHONY: emsdk
emsdk:
	cd third_party/emsdk/ && $(MAKE)

.PHONY: gcloud
emsdk:
	cd third_party/gcloud/ && $(MAKE)

.PHONY: deps
deps:
	cd third_party && $(MAKE)

.PHONY: clean
clean:
	cd third_party && $(MAKE) clean
