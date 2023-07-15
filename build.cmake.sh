#!/bin/bash
cmake -S . -B build --fresh
pushd build
cmake --build . --parallel 9 
popd