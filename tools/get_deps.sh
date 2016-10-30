#!/bin/bash

set -u
set -e
set -x

DEP_HASH=$(./tools/dep_hash.sh)

if ! curl --fail --head https://storage.googleapis.com/mewert-cpp-project-test-resources/deps/${DEP_HASH}.tar.gz >/dev/null
then
  echo "No dependencies to download for ${DEP_HASH}. Please create new dependency bundle."
  exit -1
fi

mkdir -p build
cd build
curl https://storage.googleapis.com/mewert-cpp-project-test-resources/deps/${DEP_HASH}.tar.gz | tar xz
