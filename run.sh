#!/bin/bash

cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. -G "Ninja"
ninja
if [ $? -eq 0 ]; then
  ./opeGL
fi
