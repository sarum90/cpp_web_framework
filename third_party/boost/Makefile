
include ./common.mk

URL:=http://pilotfiber.dl.sourceforge.net/project/boost/boost/1.62.0/boost_1_62_0.tar.bz2
SHA_256:=36c96b0f6155c98404091d8ceb48319a28279ca0333fba1ad8611eb90afb2ca0
#SHA_256:=12c96b0f6155c98404091d8ceb48319a28279ca0333fba1ad8611eb90afb2ca0
FILENAME:=boost_1_62_0.tar.bz2
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)
LOCAL_PRE:=$(LOCAL_DL).precheck
LOCAL_SHACHECK:=$(LOCAL_DL).shacheck
LOCAL_SRC:=$(SOURCES)/boost
LOCAL_SRC_TOKEN:=$(LOCAL_SRC)/my_makefile_token

BOOST_NATIVE_TAG=$(NATIVE_TAGS)/boost_tag

NATIVE_BOOST_PREFIX:=$(NATIVE_PREFIX)/boost

$(BOOST_NATIVE_TAG): $(LOCAL_SRC_TOKEN)
	mkdir -p $(NATIVE_BOOST_PREFIX)
	cd $(LOCAL_SRC)/boost_1_62_0 && ./bootstrap.sh --with-toolset=clang --prefix=$(NATIVE_BOOST_PREFIX)
	cd $(LOCAL_SRC)/boost_1_62_0 && ./b2 install
	mkdir -p $(NATIVE_TAGS)
	touch $(BOOST_NATIVE_TAG)

$(LOCAL_SRC_TOKEN): $(LOCAL_DL)
	mkdir -p $(LOCAL_SRC)
	cd $(LOCAL_SRC) && tar xvjf $(LOCAL_DL)
	touch $(LOCAL_SRC_TOKEN)

$(LOCAL_DL): $(LOCAL_PRE)
	echo "$(SHA_256) $(LOCAL_PRE)" > $(LOCAL_SHACHECK)
	sha256sum -c $(LOCAL_SHACHECK)
	ln -s $(LOCAL_PRE) $(LOCAL_DL)


$(LOCAL_PRE):
	mkdir -p $(DOWNLOADS)
	curl $(URL) -o $(LOCAL_PRE)

.PHONY: clean
clean:
	rm -rf $(NATIVE_BOOST_PREFIX)
	rm -rf $(LOCAL_SRC)
	rm $(LOCAL_DL)
	rm $(LOCAL_PRE)
