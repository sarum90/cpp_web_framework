#!/bin/bash

set -u
set -e

(cd travis-secrets/ && ./decrypt.sh)

./tools/get_deps.sh

ls ./build/root/native/cmake

export PATH=$PWD/build/root/native/cmake/bin:$PATH
source build/root/native/emsdk/emsdk_env.sh

clang++ --version && make -C modules/mestring/ && ./modules/mestring/test_mestring && echo "PASSED!"

