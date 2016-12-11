#!/bin/bash

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

ROOT="$( cd $DIR/.. && pwd)"


source ${ROOT}/build/root/native/emsdk/emsdk_env.sh
source ${ROOT}/build/root/native/python_env/bin/activate

set -u
set -e

PATHS="
build/root/native/mettle/bin
build/root/native/patchelf/bin
build/root/native/gcloud/bin
"

export PATH="$(for x in $PATHS; do echo -n ${ROOT}/${x}:; done):$PATH"
export CXX=em++
export CC=emcc

LD_DIRS="
build/root/ems/boost/lib
build/root/ems/mettle/lib
"

LD_LIBRARY_PATH=${LD_LIBRARY_PATH:-}
export LD_LIBRARY_PATH="$(for x in $LD_DIRS; do echo -n ${ROOT}/${x}:; done):$LD_LIBRARY_PATH"

export BOOST_ROOT=${ROOT}/build/root/ems/boost

INCLUDES="
build/root/ems/boost/include
"
INCLUDE_FLAGS=$(for x in $INCLUDES; do echo -n "-I${ROOT}/$x "; done)

COMMON_FLAGS="${INCLUDE_FLAGS}"

export CXXFLAGS="$COMMON_FLAGS -std=c++1z"
export CCFLAGS="$COMMON_FLAGS"


LIBDIRS="
build/root/ems/boost/lib/
build/root/ems/boost/bin/
"
LIBDIR_FLAGS=$(for x in $LIBDIRS; do echo -n "-L${ROOT}/$x "; done)

export LDFLAGS="$LIBDIR_FLAGS"

echo PATH = $PATH
echo CXX = $CXX
echo CC = $CC
echo CXXFLAGS = $CXXFLAGS
echo CCFLAGS = $CCFLAGS
echo LDFLAGS = $LDFLAGS

echo "RUNNING:" "$@"

"$@"

