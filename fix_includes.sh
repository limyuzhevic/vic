#!/bin/bash

# Find all .cpp files in src directory and fix include paths
# Replace "../something" with "something" and "../../something" with "something"

find /workspace/app/src -name "*.cpp" -o -name "*.hpp" | while read file; do
    # Skip CMakeLists.txt files
    if [[ "$file" == */CMakeLists.txt ]]; then
        continue
    fi
    
    # Make a backup
    cp "$file" "${file}.bak"
    
    # Replace ../something with something
    sed -i 's/#include "\.\.\//#include "g' "$file"
    
    # Replace ../../something with something
    sed -i 's/#include "\.\.\./#include "g' "$file"
    
    # Check if file was changed
    if ! cmp -s "$file" "${file}.bak"; then
        echo "Updated: $file"
    fi
    
    # Remove backup
    rm "${file}.bak"
    
done

echo "Include path standardization complete."