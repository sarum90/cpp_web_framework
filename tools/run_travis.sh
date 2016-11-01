#!/bin/bash

set -x
set -u
set -e

(cd travis-secrets/ && ./decrypt.sh)

./tools/get_deps.sh

echo "DONE with setup_travis"
