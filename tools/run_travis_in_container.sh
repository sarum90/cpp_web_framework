#!/bin/bash

set -e
set -u

mkdir -p /home/travis/build
cd /home/travis/build
git clone /repo repo
cd repo

# Just take the private key manually.
mkdir -p travis-secrets/private_key/
cp /repo/travis-secrets/private_key/private_key.pem travis-secrets/private_key/

./tools/run_travis.sh

