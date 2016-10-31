
include ./common.mk

URL:=https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz

FILENAME:=emsdk-portable.tar.gz
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)
LOCAL_SRC:=$(SOURCES)/emsdk
LOCAL_SRC_TOKEN:=$(LOCAL_SRC)/my_makefile_token

EMSDK_NATIVE_TAG=$(NATIVE_TAGS)/emsdk_tag

NATIVE_EMSDK_PREFIX:=$(NATIVE_PREFIX)/emsdk
EMSDK_INSTALLED_TAG:=$(NATIVE_PREFIX)/emsdk/install_tag

CLANG_CMAKE_BUILD_DIR:=$(NATIVE_EMSDK_PREFIX)/clang/tag-e1.36.13/build_tag-e1.36.13_32
CLANG_INSTALL_DIR:=$(NATIVE_EMSDK_PREFIX)/clang/tag-e1.36.13/temp_install

$(EMSDK_NATIVE_TAG): $(EMSDK_INSTALLED_TAG)
	mkdir -p $(CLANG_INSTALL_DIR)
	cd $(CLANG_CMAKE_BUILD_DIR) && make DESTDIR=$(CLANG_INSTALL_DIR) install
	cd $(CLANG_INSTALL_DIR)/usr/local && for x in `find lib -name '*.a' -type 'f'`; do strip --strip-unneeded $$x; done || true
	cd $(CLANG_INSTALL_DIR)/usr/local && for x in `find lib -name '*.so' -type 'f'`; do strip --strip-unneeded $$x; done || true
	cd $(CLANG_INSTALL_DIR)/usr/local && for x in `find lib -name '*.so.*' -type 'f'`; do strip --strip-unneeded $$x; done || true
	cd $(CLANG_INSTALL_DIR)/usr/local && for x in `find bin -type 'f'`; do strip --strip-unneeded $$x; done || true
	rm -rf $(CLANG_CMAKE_BUILD_DIR)
	mv $(CLANG_INSTALL_DIR)/usr/local $(CLANG_CMAKE_BUILD_DIR)
	rm -rf $(CLANG_INSTALL_DIR)
	mkdir -p $(NATIVE_TAGS)
	touch $(EMSDK_NATIVE_TAG)

$(EMSDK_INSTALLED_TAG): $(LOCAL_DL)
	mkdir -p $(NATIVE_EMSDK_PREFIX)
	cd $(NATIVE_EMSDK_PREFIX) && tar xvzf $(LOCAL_DL) --strip-components=1
	cd $(NATIVE_EMSDK_PREFIX) && ./emsdk update
	cd $(NATIVE_EMSDK_PREFIX) && ./emsdk install sdk-tag-1.36.13-32bit
	cd $(NATIVE_EMSDK_PREFIX) && ./emsdk activate sdk-tag-1.36.13-32bit
	touch $(EMSDK_INSTALLED_TAG)

$(LOCAL_DL):
	mkdir -p $(DOWNLOADS)
	curl $(URL) -o $(LOCAL_DL)
