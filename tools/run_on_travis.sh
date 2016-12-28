#!/bin/bash

set -x
set -u
set -e

export PATH=`pwd`/build/root/native/cmake/bin:$PATH
source ./build/root/native/emsdk/emsdk_env.sh

./tools/wrapper.sh make test
echo "PASSED!"
