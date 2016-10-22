
ROOT:=$(abspath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD:=$(ROOT)/build
DOWNLOADS:=$(BUILD)/downloads
SOURCES:=$(BUILD)/src
PREFIX:=$(BUILD)/root
EMS_PREFIX:=$(PREFIX)/emscripten
NATIVE_PREFIX:=$(PREFIX)/native
NATIVE_TAGS=$(NATIVE_PREFIX)/tags
