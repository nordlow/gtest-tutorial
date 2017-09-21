#!/bin/bash

# this script should be run as sudo

pushd /usr/src/googletest
mkdir build
pushd build
cmake ../..
make
cp libgtest* /usr/lib/
pushd ..
#rm -rf build

popd
popd
popd
