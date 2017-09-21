#!/bin/bash

# this script should be run as sudo

cd /usr/src/googletest/googletest
mkdir build
cd build
cmake ../..
make
cp libgtest* /usr/lib/
cd ..
#rm -rf build
