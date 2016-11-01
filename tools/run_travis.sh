#!/bin/bash

set -u
set -e

(cd travis-secrets/ && ./decrypt.sh)

./tools/get_deps.sh

export PATH=$PWD/build/root/native/cmake/bin:$PATH
source build/root/native/emsdk/emsdk_env.sh
