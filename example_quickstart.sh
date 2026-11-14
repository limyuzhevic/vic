#!/usr/bin/env bash
# NLM Example: Quick Start Guide
# This script demonstrates the simplest way to get started with NLM

# Check if NLM is installed
if ! command -v nlm &> /dev/null; then
    echo "Error: NLM executable not found. Please build the project first."
    echo "Run: mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4"
    exit 1
fi

# Create an example configuration
echo "Creating example configuration..."
cat > example_config.cfg << 'EOF'
# NLM Example Configuration
# This is a minimal configuration for testing NLM

# Network parameters
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Simulation parameters
random_seed = 42
simulation_timestep = 0.001
simulation_steps = 1000

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Logging
log_level = info
EOF

echo "Example configuration saved to: example_config.cfg"
echo ""
echo "Run the NLM with this configuration:"
echo "  nlm --config example_config.cfg --verbose"
echo ""
echo "For help on all available options:"
echo "  nlm --help"
