
include ./common.mk


URL=https://github.com/jbeder/yaml-cpp/archive/3757b2023b71d183a341677feee693c71c2e0766.zip
FILENAME=yaml-cpp-3757b2023b71d183.zip
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)
LOCAL_SRC:=$(SOURCES)/yaml-cpp
LOCAL_INNER_SRC=$(LOCAL_SRC)/yaml-cpp-3757b2023b71d183a341677feee693c71c2e0766
LOCAL_SRC_TOKEN:=$(LOCAL_SRC)/my_makefile_token

NATIVE_YAML_CPP_PREFIX:=$(NATIVE_PREFIX)/yaml-cpp
YAML_CPP_NATIVE_TAG=$(NATIVE_TAGS)/yaml-cpp


$(YAML_CPP_NATIVE_TAG): $(LOCAL_SRC_TOKEN)
	mkdir -p $(NATIVE_YAML_CPP_PREFIX)
	mkdir -p $(LOCAL_INNER_SRC)/build
	cd $(LOCAL_INNER_SRC)/build && $(WRAPPER) cmake ..
	$(WRAPPER) $(MAKE) -C $(LOCAL_INNER_SRC)/build
	$(WRAPPER) $(MAKE) DESTDIR=$(NATIVE_YAML_CPP_PREFIX) install -C $(LOCAL_INNER_SRC)/build install
	touch $(YAML_CPP_NATIVE_TAG)

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
	rm -rf $(YAML_CPP_NATIVE_TAG)
	rm -rf $(NATIVE_YAML_CPP_PREFIX)
	rm -rf $(LOCAL_SRC_TOKEN)
	rm -rf $(LOCAL_SRC)
