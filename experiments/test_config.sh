#!/bin/bash
# NLM Configuration Test Script
# Tests configuration parameter management

echo "=== NLM Configuration Test Script ==="

if [[ ! -f "./build/nlm" ]]; then
    echo "Building NLM..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4
    cd ..
fi

# Test setting and getting parameters
echo "--- Testing parameter management ---"

# Set various parameters
./build/nlm set neuron_count 2000
./build/nlm set region_count 2
./build/nlm set simulation_timestep 0.0005
./build/nlm set connection_probability 0.15
./build/nlm set stdp_ltp_weight 0.025
./build/nlm set stdp_ltd_weight 0.02

# Get and display parameters
./build/nlm get neuron_count
./build/nlm get region_count
./build/nlm get simulation_timestep

# Test configuration file loading
echo ""
echo "--- Testing configuration file ---"
./build/nlm --config configs/default.cfg

echo ""
echo "=== Configuration Tests Complete ==="
