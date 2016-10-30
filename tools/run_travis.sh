#!/bin/bash

set -u
set -e

(cd travis-secrets/ && ./decrypt.sh)

./tools/get_deps.sh

sudo -E apt-add-repository -y "ppa:ubuntu-toolchain-r/test"
sudo -E apt-get -yq update &>> ~/apt-get-update.log
sudo -E apt-get -yq --no-install-suggests --no-install-recommends --force-yes install gcc-4.8 g++-4.8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.8


export PATH=$PWD/build/root/native/cmake/bin:$PATH
source build/root/native/emsdk/emsdk_env.sh

