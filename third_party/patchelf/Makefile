
include ./common.mk

URL=https://github.com/NixOS/patchelf/archive/327d80443672c397970738f9e216a7e86cbf3ad7.zip
FILENAME=patchelf-327d80443672c397970738f9e216a7e86cbf3ad7.zip
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)
LOCAL_SRC:=$(SOURCES)/patchelf
LOCAL_INNER_SRC=$(LOCAL_SRC)/patchelf-327d80443672c397970738f9e216a7e86cbf3ad7
LOCAL_SRC_TOKEN:=$(LOCAL_SRC)/my_makefile_token

NATIVE_PATCHELF_PREFIX:=$(NATIVE_PREFIX)/patchelf
PATCHELF_NATIVE_TAG=$(NATIVE_TAGS)/patchelf


$(PATCHELF_NATIVE_TAG): $(PATCHELF_NATIVE_TAG) $(LOCAL_SRC_TOKEN)
	mkdir -p $(NATIVE_PATCHELF_PREFIX)
	cd $(LOCAL_INNER_SRC) && ls -la
	cd $(LOCAL_INNER_SRC) && $(WRAPPER) ./bootstrap.sh
	cd $(LOCAL_INNER_SRC) && $(WRAPPER) ./configure --prefix=$(NATIVE_PATCHELF_PREFIX)
	cd $(LOCAL_INNER_SRC) && $(WRAPPER) make
	cd $(LOCAL_INNER_SRC) && $(WRAPPER) make install
	touch $(PATCHELF_NATIVE_TAG)

$(LOCAL_SRC_TOKEN): $(LOCAL_DL)
	mkdir -p $(LOCAL_SRC)
	cd $(LOCAL_SRC) && unzip $(LOCAL_DL)
	touch $(LOCAL_SRC_TOKEN)

$(LOCAL_DL):
	mkdir -p $(DOWNLOADS)
	curl -L $(URL) -o $(LOCAL_DL)

.PHONY: clean
clean:
	rm -rf $(PATCHELF_NATIVE_TAG)
	rm -rf $(NATIVE_PATCHELF_PREFIX)
	rm -rf $(LOCAL_SRC_TOKEN)
	rm -rf $(LOCAL_SRC)
