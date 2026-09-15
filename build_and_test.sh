#!/bin/bash
# Check if build files exist
if [ -f CMakeLists.txt ]; then
    echo "CMakeLists.txt exists in current directory"
    # Try to make directory structure
    mkdir -p build
    cd build
    cmake ..
    make -j4
else
    echo "CMakeLists.txt not found"
fi
