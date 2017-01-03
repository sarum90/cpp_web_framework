

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
gcloud:
	cd third_party/gcloud/ && $(MAKE)

.PHONY: deps
deps:
	cd third_party && $(MAKE)

.PHONY: clean
clean:
	cd third_party && $(MAKE) clean

.PHONY: test
test:
	$(MAKE) -C modules/mestring/ test
	$(MAKE) -C modules/webserver/ test
	$(MAKE) -C modules/reax/ test
	$(MAKE) -C modules/reax-ssl/ test
	$(MAKE) -C modules/base64/ test
	$(MAKE) -C modules/oauth/ test
	$(MAKE) -C modules/descbuild/ test
