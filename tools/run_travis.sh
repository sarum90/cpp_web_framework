#!/bin/bash

set -x
set -u
set -e

(cd travis-secrets/ && ./decrypt.sh)

./tools/get_deps.sh

./build/root/native/emsdk/emsdk activate

echo "DONE with setup_travis"
