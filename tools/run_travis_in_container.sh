#!/bin/bash

# This script should stay roughly in-sync with .travis.yml
# Main differences (because travis has different mechanisms):
#  - Secrets management.
#  - Dependency installation.
#
# In some ideal world this would just parse .travis.yml to do these operations,
# but this is good enough.

set -e
set -u

# Setup Repo
mkdir -p /home/travis/build
cd /home/travis/build
git clone /repo repo
cd repo

# Install deps.
sudo -E apt-add-repository -y "ppa:ubuntu-toolchain-r/test"
sudo -E apt-get -yq update &>> ~/apt-get-update.log
sudo -E apt-get -yq --no-install-suggests --no-install-recommends --force-yes install libstdc++-6-dev libstdc++6 libc6:i386 libstdc++6:i386

# Just take the private key manually.
mkdir -p travis-secrets/private_key/
cp /repo/travis-secrets/private_key/private_key.pem travis-secrets/private_key/

# Run travis setup script.
./tools/run_travis.sh

# Run travis test script.
./tools/run_on_travis.sh
