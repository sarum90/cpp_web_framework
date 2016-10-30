#!/bin/bash

set -e
set -u

mkdir -p /home/travis/build
cd /home/travis/build
git clone /repo repo
cd repo
ls -la
ls -la tools/
./tools/travis_make_deps.sh

