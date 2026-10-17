#!/bin/bash

# Extract header files from CMakeLists.txt
grep -E "^\s*src/" CMakeLists.txt | sed 's/^\s*//' | grep "\.hpp" | head -20 > cmake_headers.txt

# Find all .hpp files in src
find src -name "*.hpp" > actual_headers.txt

# Check missing headers
echo "Missing headers:"
comm -23 <(sort cmake_headers.txt) <(sort actual_headers.txt)

echo -e "\nExtra headers in src:"
comm -13 <(sort cmake_headers.txt) <(sort actual_headers.txt)
