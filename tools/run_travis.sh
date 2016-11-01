#!/bin/bash

set -u
set -e

(cd travis-secrets/ && ./decrypt.sh)

./tools/get_deps.sh

export PATH=$PWD/build/root/native/cmake/bin:$PATH
