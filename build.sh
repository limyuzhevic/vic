#!/bin/bash

# Build script for NLM
# Usage: ./build.sh [debug|release]

set -e

cd "$(dirname "$0")"

BUILD_TYPE="${1:-release}"
BUILD_DIR="build"

echo "Building NLM $BUILD_TYPE..."

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

if [ "$BUILD_TYPE" = "debug" ]; then
    cmake .. -DCMAKE_BUILD_TYPE=Debug
else
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi

make -j$(nproc)

echo "Build complete!"
echo "Executables in $(pwd):"
ls -la | grep -E "nlm|benchmark"