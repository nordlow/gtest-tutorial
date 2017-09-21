#!/bin/bash

mkdir -p build
pushd build
cmake ..
make test
popd
