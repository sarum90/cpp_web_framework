#!/bin/bash

set -u
set -e

(cd travis-secrets/ && ./decrypt.sh)

./tools/get_deps.sh

ls ./build/root/native/cmake

clang++ --version && make -C modules/mestring/ && ./modules/mestring/test_mestring && echo "PASSED!"

