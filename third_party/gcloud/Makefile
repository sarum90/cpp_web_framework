
include ./common.mk

URL:=https://dl.google.com/dl/cloudsdk/channels/rapid/downloads/google-cloud-sdk-132.0.0-linux-x86_64.tar.gz

FILENAME:=google-cloud-sdk-132.0.0-linux-x86_64.tar.gz
LOCAL_DL:=$(DOWNLOADS)/$(FILENAME)

GCLOUD_NATIVE_TAG=$(NATIVE_TAGS)/gcloud_tag

NATIVE_GCLOUD_PREFIX:=$(NATIVE_PREFIX)/gcloud

$(GCLOUD_NATIVE_TAG): $(LOCAL_DL)
	mkdir -p $(NATIVE_GCLOUD_PREFIX)
	cd $(NATIVE_GCLOUD_PREFIX) && tar xvzf $(LOCAL_DL) --strip-components=1
	mkdir -p $(NATIVE_TAGS)
	touch $(GCLOUD_NATIVE_TAG)

$(LOCAL_DL):
	mkdir -p $(DOWNLOADS)
	curl $(URL) -o $(LOCAL_DL)
