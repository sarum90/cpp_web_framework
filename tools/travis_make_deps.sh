#!/bin/bash

set -e
set -u

DEP_HASH=$(./tools/dep_hash.sh)

if curl --fail --head https://storage.googleapis.com/mewert-cpp-project-test-resources/deps/${DEP_HASH}.tar.gz >/dev/null
then
  echo "Dependencies for ${DEP_HASH} already exist."
  exit 0
fi

(cd travis-secrets/ && ./decrypt.sh)

make gcloud
GCLOUD=./build/root/native/gcloud/bin/gcloud
GSUTIL=./build/root/native/gcloud/bin/gsutil

$GCLOUD auth activate-service-account --key-file=./travis-secrets/decrypted/mewert-cpp-project-6899912be999.json

sudo -E apt-add-repository -y "ppa:ubuntu-toolchain-r/test"
sudo -E apt-get -yq update &>> ~/apt-get-update.log
sudo -E apt-get -yq --no-install-suggests --no-install-recommends --force-yes install gcc-4.8 g++-4.8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.8
gcc --version && g++ --version
make cmake
#export PATH=$PWD/build/root/native/cmake/bin:$PATH
#cmake --version
#make emsdk
#source build/root/native/emsdk/emsdk_env.sh
#clang++ --version
#make deps

(cd build && tar cvzf ${DEP_HASH}.tar.gz)
$GSUTIL cp -a public-read build/${DEP_HASH}.tar.gz gs://mewert-cpp-project-test-resources/deps/${DEP_HASH}.tar.gz
