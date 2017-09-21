#!/bin/bash

# this script should be run as sudo

pushd /usr/src/googletest
mkdir -p build
pushd build
cmake ..
make
cp googlemock/libg*  googlemock/gtest/libg* /usr/lib/
pushd ..

#rm -rf build

popd
popd
popd
