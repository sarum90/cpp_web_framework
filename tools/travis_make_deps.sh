#!/bin/bash

set -e
set -u

(cd travis-secrets/ && ./decrypt.sh)
cat travis-secrets/decrypted/sample_secret.txt
sudo -E apt-add-repository -y "ppa:ubuntu-toolchain-r/test"
sudo -E apt-get -yq update &>> ~/apt-get-update.log
sudo -E apt-get -yq --no-install-suggests --no-install-recommends --force-yes install gcc-4.8 g++-4.8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.8
gcc --version && g++ --version
make cmake
export PATH=$PWD/build/root/native/cmake/bin:$PATH
cmake --version
make emsdk
source build/root/native/emsdk/emsdk_env.sh
clang++ --version
make deps
