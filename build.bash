#! /usr/bin/env bash

cmake -G Ninja -H. -Bbuild.dir -DCMAKE_TOOLCHAIN_FILE=$(readlink -f cmake/CoroutinesToolchain.cmake) -DCMAKE_BUILD_TYPE=Debug
cmake --build build.dir
