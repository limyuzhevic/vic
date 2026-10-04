#!/bin/bash

# Quick NLM Build Script
# Simple version for quick builds

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -- -j$(nproc)
./nlm
