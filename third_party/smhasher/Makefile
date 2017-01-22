
include ./common.mk


URL=https://github.com/aappleby/smhasher/archive/61a0530f28277f2e850bfc39600ce61d02b518de.zip
FILENAME=smhasher-61a0530f28277f2e.zip
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)
LOCAL_SRC:=$(SOURCES)/smhasher
LOCAL_INNER_SRC=$(LOCAL_SRC)/smhasher-61a0530f28277f2e850bfc39600ce61d02b518de
LOCAL_SRC_TOKEN:=$(LOCAL_SRC)/my_makefile_token

NATIVE_SMHASHER_PREFIX:=$(NATIVE_PREFIX)/smhasher
SMHASHER_NATIVE_TAG=$(NATIVE_TAGS)/smhasher


$(SMHASHER_NATIVE_TAG): $(LOCAL_SRC_TOKEN)
	mkdir -p $(NATIVE_SMHASHER_PREFIX)
	mkdir -p $(LOCAL_INNER_SRC)/build
	cd $(LOCAL_INNER_SRC)/build && $(WRAPPER) bash -c '$${CXX} $${CXXFLAGS} -c ../src/MurmurHash3.cpp -o ./MurmurHash3.o'
	cd $(LOCAL_INNER_SRC)/build && $(WRAPPER) bash -c 'ar rcs libmmh3.a ./MurmurHash3.o'
	mkdir -p $(NATIVE_SMHASHER_PREFIX)/usr/local/include/smhasher
	mkdir -p $(NATIVE_SMHASHER_PREFIX)/usr/local/lib
	cp $(LOCAL_INNER_SRC)/build/libmmh3.a $(NATIVE_SMHASHER_PREFIX)/usr/local/lib/
	cp $(LOCAL_INNER_SRC)/src/MurmurHash3.h $(NATIVE_SMHASHER_PREFIX)/usr/local/include/smhasher/
	touch $(SMHASHER_NATIVE_TAG)

$(LOCAL_SRC_TOKEN): $(LOCAL_DL)
	mkdir -p $(LOCAL_SRC)
	cd $(LOCAL_SRC) && unzip $(LOCAL_DL)
	touch $(LOCAL_SRC_TOKEN)

$(LOCAL_DL):
	mkdir -p $(DOWNLOADS)
	curl -L $(URL) -o $(LOCAL_DL).tmp
	mv $(LOCAL_DL).tmp $(LOCAL_DL)

.PHONY: clean
clean:
	rm -rf $(SMHASHER_NATIVE_TAG)
	rm -rf $(NATIVE_SMHASHER_PREFIX)
	rm -rf $(LOCAL_SRC_TOKEN)
	rm -rf $(LOCAL_SRC)
