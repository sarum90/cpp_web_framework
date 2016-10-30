
include ./common.mk

URL:=https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz

FILENAME:=emsdk-portable.tar.gz
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)
LOCAL_SRC:=$(SOURCES)/emsdk
LOCAL_SRC_TOKEN:=$(LOCAL_SRC)/my_makefile_token

EMSDK_NATIVE_TAG=$(NATIVE_TAGS)/emsdk_tag

NATIVE_EMSDK_PREFIX:=$(NATIVE_PREFIX)/emsdk

$(EMSDK_NATIVE_TAG): $(LOCAL_DL)
	mkdir -p $(NATIVE_EMSDK_PREFIX)
	cd $(NATIVE_EMSDK_PREFIX) && tar xvzf $(LOCAL_DL) --strip-components=1
	cd $(NATIVE_EMSDK_PREFIX) && ./emsdk update
	cd $(NATIVE_EMSDK_PREFIX) && ./emsdk install sdk-tag-1.36.13-32bit
	cd $(NATIVE_EMSDK_PREFIX) && ./emsdk activate sdk-tag-1.36.13-32bit
	mkdir -p $(NATIVE_TAGS)
	touch $(EMSDK_NATIVE_TAG)

$(LOCAL_DL):
	mkdir -p $(DOWNLOADS)
	curl $(URL) -o $(LOCAL_DL)
