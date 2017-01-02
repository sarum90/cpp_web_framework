include ./common.mk

URL=https://capnproto.org/capnproto-c++-0.5.3.tar.gz
FILENAME=capnproto-c++-0.5.3.tar.gz
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)
LOCAL_SRC:=$(SOURCES)/capnproto
LOCAL_INNER_SRC=$(LOCAL_SRC)/capnproto-c++-0.5.3
LOCAL_SRC_TOKEN:=$(LOCAL_SRC)/my_makefile_token

NATIVE_CAPNPROTO_PREFIX:=$(NATIVE_PREFIX)/capnproto
CAPNPROTO_NATIVE_TAG=$(NATIVE_TAGS)/capnproto


$(CAPNPROTO_NATIVE_TAG): $(LOCAL_SRC_TOKEN)
	cd $(LOCAL_INNER_SRC) && $(WRAPPER) ./configure
	$(WRAPPER) $(MAKE) check -C $(LOCAL_INNER_SRC)
	$(WRAPPER) $(MAKE) DESTDIR=$(NATIVE_CAPNPROTO_PREFIX) install -C $(LOCAL_INNER_SRC)
	touch $(CAPNPROTO_NATIVE_TAG)

$(LOCAL_SRC_TOKEN): $(LOCAL_DL)
	mkdir -p $(LOCAL_SRC)
	cd $(LOCAL_SRC) && tar xvzf $(LOCAL_DL)
	touch $(LOCAL_SRC_TOKEN)

$(LOCAL_DL):
	mkdir -p $(DOWNLOADS)
	curl -L $(URL) -o $(LOCAL_DL).tmp
	mv $(LOCAL_DL).tmp $(LOCAL_DL)

.PHONY: clean
clean:
	rm -rf $(CAPNPROTO_NATIVE_TAG)
	rm -rf $(NATIVE_CAPNPROTO_PREFIX)
	rm -rf $(LOCAL_SRC_TOKEN)
	rm -rf $(LOCAL_SRC)
