#!/usr/bin/env bash
# Quick Start Script for NLM
# This script provides a fast way to try out NLM with minimal setup

echo "=== NLM Quick Start ==="
echo "Getting started with Neural Learning Machine..."
echo ""

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
    cd build
    cmake ..
else
    cd build
fi

echo ""
echo "Building NLM..."
make -j$(nproc) 2>&1 | tail -20
echo ""

if [ ! -f "nlm" ]; then
    echo "ERROR: Build failed. Please check the output above."
    echo "You can try building manually with:"
    echo "  mkdir build"
    echo "  cd build"
    echo "  cmake .."
    echo "  make"
    exit 1
fi

echo ""
echo "=== Running Simple Examples ==="
echo ""

# Example 1: Basic connectivity test
echo "1. Running basic connectivity test..."
./nlm --steps 100 2>&1 | grep -E "(Test|SUCCESS|ERROR|Basic)" | head -10
echo ""

# Example 2: Running with custom configuration
echo "2. Creating custom config and running simulation..."
cat > ../custom_config.cfg << EOF
# Custom configuration for NLM simulation
random_seed = 123
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.12
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
EOF

./nlm --config ../custom_config.cfg --steps 200 2>&1 | grep -E "(Configuration|Connectivity|Plasticity|STDP|SUCCESS)" | head -10
echo ""

# Example 3: Save and load checkpoint
echo "3. Testing save/load functionality..."
./nlm --steps 50 --save ../test_checkpoint.bin 2>&1 | grep -E "(save|loaded|ERROR)" | head -5

echo ""
echo "=== Testing Advanced Features ==="
echo ""

# Test brain tool
echo "4. Testing brain analysis tool..."
if [ -f "brain_tool" ]; then
    echo "   Running brain_tool analyze..."
    ./brain_tool 2>&1 <<< "analyze" | head -20
fi
echo ""

# Test Phase 6 demo
echo "5. Running Phase 6 integration demo..."
if [ -f "nlm_phase6_demo" ]; then
    echo "   Note: This may take a while as it runs comprehensive tests..."
    ./nlm_phase6_demo --help 2>&1 | head -5
fi
echo ""

echo "=== Examples Complete ==="
echo ""
echo "You can find more examples in the examples/ directory:"
echo "  examples/01_basic_usage.sh"
echo "  examples/02_configuration_examples.sh"
echo "  examples/03_advanced_simulation.sh"
echo "  examples/04_brain_tool_examples.sh"
echo ""
echo "For comprehensive documentation:"
echo "  Read docs/ARCHITECTURE.md for technical details"
echo "  Read docs/SCIENCE.md for scientific background"
echo "  Read docs/ROADMAP.md for future development plans"
echo ""
echo "Need help? Check the HOW_TO_USE.md file for user instructions." >&2
exit 0
