#!/bin/bash

set -e
set -u

export PATH=`pwd`/build/root/native/cmake/bin:$PATH
source ./build/root/native/emsdk/emsdk_env.sh

clang++ --version && make -C modules/mestring/ && ./modules/mestring/test_mestring && echo "PASSED!"
