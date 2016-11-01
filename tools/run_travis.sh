#!/bin/bash

set -u
set -e

(cd travis-secrets/ && ./decrypt.sh)

./tools/get_deps.sh

export PATH=`pwd`/build/root/native/cmake/bin:$PATH

echo "DONE with run_travis"
