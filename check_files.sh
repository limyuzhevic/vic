#!/bin/bash

# Extract files from CMakeLists.txt
grep -E "^\s*src/" CMakeLists.txt | sed 's/^\s*//' | grep "\.cpp" > cmake_files.txt

# Find all .cpp files in src
find src -name "*.cpp" > actual_files.txt

# Check missing files
echo "Missing files:"
comm -23 <(sort cmake_files.txt) <(sort actual_files.txt)

echo -e "\nExtra files in src:"
comm -13 <(sort cmake_files.txt) <(sort actual_files.txt)
