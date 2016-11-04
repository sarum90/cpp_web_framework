
include ./common.mk

URL=https://github.com/jimporter/mettle/archive/2d150c1aff1e5a62b9772c08364ea53118d079f8.zip
FILENAME=mettle-2d150c1aff1e5.zip
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)
LOCAL_SRC:=$(SOURCES)/mettle
LOCAL_INNER_SRC=$(LOCAL_SRC)/mettle-2d150c1aff1e5a62b9772c08364ea53118d079f8
LOCAL_SRC_TOKEN:=$(LOCAL_SRC)/my_makefile_token

NATIVE_METTLE_PREFIX:=$(NATIVE_PREFIX)/mettle
METTLE_NATIVE_TAG=$(NATIVE_TAGS)/mettle
METTLE_NATIVE_TAG=$(NATIVE_TAGS)/mettle


$(METTLE_NATIVE_TAG): $(BFG9000_NATIVE_TAG) $(LOCAL_SRC_TOKEN)
	cd $(LOCAL_INNER_SRC) && scripts/vendorize_bencode.py
	mkdir -p $(NATIVE_METTLE_PREFIX)
	cd $(LOCAL_INNER_SRC) && $(WRAPPER) 9k --prefix=$(NATIVE_METTLE_PREFIX) build/ --backend=make
	cd $(LOCAL_INNER_SRC)/build && $(WRAPPER) make
	cd $(LOCAL_INNER_SRC)/build && $(WRAPPER) make install
	touch $(METTLE_NATIVE_TAG)

$(LOCAL_SRC_TOKEN): $(LOCAL_DL)
	mkdir -p $(LOCAL_SRC)
	cd $(LOCAL_SRC) && unzip $(LOCAL_DL)
	touch $(LOCAL_SRC_TOKEN)

$(LOCAL_DL):
	mkdir -p $(DOWNLOADS)
	curl -L $(URL) -o $(LOCAL_DL)

.PHONY: clean
clean:
	rm -rf $(METTLE_NATIVE_TAG)
	rm -rf $(NATIVE_METTLE_PREFIX)
	rm -rf $(LOCAL_SRC_TOKEN)
	rm -rf $(LOCAL_SRC)
