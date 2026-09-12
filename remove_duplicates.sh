#!/bin/bash
set -e

echo "Removing duplicate files..."

# Remove src/environment/Action.cpp if it exists
if [ -f "src/environment/Action.cpp" ]; then
    rm -f src/environment/Action.cpp
    echo "Removed src/environment/Action.cpp"
fi

# Remove src/environment/Observation.cpp if it exists
if [ -f "src/environment/Observation.cpp" ]; then
    rm -f src/environment/Observation.cpp
    echo "Removed src/environment/Observation.cpp"
fi

# Verify removal
echo "Checking remaining files in src/environment/:"
ls -la src/environment/ 2>/dev/null || echo "src/environment/ directory or files not found"

echo "Checking source files:"
find src -name "*.cpp" -o -name "*.hpp" | grep -E "(Action|Observation)" | sort

echo "Done."