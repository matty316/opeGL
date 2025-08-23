#!/bin/bash

cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DASSIMP_BUILD_ZLIB=ON .. -G "Ninja"
ninja
