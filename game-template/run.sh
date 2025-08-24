#!/bin/bash

cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make
if [ $? -eq 0 ]; then
  ./game
fi
