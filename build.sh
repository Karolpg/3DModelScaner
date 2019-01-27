#!/bin/bash

rm -r bin
rm -r lib
rm -r build

mkdir -p build/build_sh
cd build/build_sh
cmake ../..
make -j$(nproc)

