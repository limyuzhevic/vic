#!/bin/bash

# Find all .cpp files
find /workspace/app/src -name "*.cpp" | while read cpp_file; do
    # Get the directory and basename
    dir=$(dirname "$cpp_file")
    base=$(basename "$cpp_file" .cpp)
    
    # Check if .hpp file exists
    if [ ! -f "$dir/$base.hpp" ]; then
        echo "$cpp_file: NO HEADER"
    fi
done
