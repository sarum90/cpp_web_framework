
ROOT:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD:=$(ROOT)/build
DOWNLOADS:=$(BUILD)/downloads
SOURCES:=$(BUILD)/src
PREFIX:=$(BUILD)/root
EMS_PREFIX:=$(PREFIX)/ems
EMS_TAGS:=$(EMS_PREFIX)/tags
NATIVE_PREFIX:=$(PREFIX)/native
NATIVE_TAGS=$(NATIVE_PREFIX)/tags

PYENV=$(NATIVE_PREFIX)/python_env
PYSOURCE=$(PYENV)/bin/activate
PIP_ENV_NATIVE_TAG=$(NATIVE_TAGS)/pip_env_tag

WRAPPER=$(ROOT)/tools/wrapper.sh
CLIENT_WRAPPER=$(ROOT)/tools/client-wrapper.sh

#
# The following mechanism sets up skipping all tests that require secrets if
# the sample secret does not exist. This is a half-baked attempt to support
# running tests even if you do not have the secrets.
#
ifneq ("$(wildcard $(ROOT)/travis-secrets/decrypted/sample_secret.txt)","")
METTLE_ARGS := --attr=requires_secrets --attr=!skip 
else
METTLE_ARGS := 
endif
