
include ./common.mk

URL:=https://cmake.org/files/v3.6/cmake-3.6.2-Linux-x86_64.tar.gz

FILENAME:=cmake-3.6.2-Linux-x86_64.tar.gz
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)

CMAKE_NATIVE_TAG=$(NATIVE_TAGS)/cmake_tag

NATIVE_CMAKE_PREFIX:=$(NATIVE_PREFIX)/cmake

$(CMAKE_NATIVE_TAG): $(LOCAL_DL)
	mkdir -p $(NATIVE_CMAKE_PREFIX)
	cd $(NATIVE_CMAKE_PREFIX) && tar xvzf $(LOCAL_DL) --strip-components=1
	mkdir -p $(NATIVE_TAGS)
	touch $(CMAKE_NATIVE_TAG)

$(LOCAL_DL):
	mkdir -p $(DOWNLOADS)
	curl $(URL) -o $(LOCAL_DL)
