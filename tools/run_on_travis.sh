#!/bin/bash

set -x
set -u
set -e

export PATH=`pwd`/build/root/native/cmake/bin:$PATH
source ./build/root/native/emsdk/emsdk_env.sh

make -C modules/mestring/ test
bash
make -C modules/webserver/ test
make -C modules/reax/ test
echo "PASSED!"
