#!/bin/sh

set -e

if [ "$CXX" = "g++" ]; then export CXX="g++-4.9"; fi

mkdir build_cmake
cd build_cmake
cmake -G "$GENERATOR" $CMAKE_PARAMS ..
cmake --build .
