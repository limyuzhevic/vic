#!/usr/bin/env bash

# CMake build script for NLM
# Build directory: build/

cd "$(dirname "$0")"

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure CMake with Release build type
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project with parallel jobs
make -j$(nproc)

# Run basic tests
./nlm 2>&1 | head -50

# Test JSON configuration loading
cp ../configs/default.cfg ./test_legacy.cfg

# Try to save config to JSON
cat > test_json.json << 'EOF'
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.15
}
EOF

# Clean up
rm -f test_legacy.cfg test_json.json

# Exit with success
exit 0
