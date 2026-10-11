#!/bin/bash

# NLM (熙然) - Neural Learning Machine
# Example Usage Script
#
# This script demonstrates how to use the NLM system with various configurations
# and provides examples for different experiment scenarios.

# Configuration directory
CONFIG_DIR="configs"

# Create configs directory if it doesn't exist
mkdir -p "$CONFIG_DIR"

# Example 1: Basic Configuration
# Creates a simple 2-region brain with 500 neurons
echo "Creating basic configuration..."
cat > "$CONFIG_DIR/basic.cfg" << 'EOF'
# NLM Configuration File (Generated)
# Source: 0 (default)
# Basic configuration for quick testing and demonstration
neuron_count = 500
region_count = 2
simulation_timestep = 0.001
connection_probability = 0.15
random_seed = 42
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
EOF

# Example 2: Large-Scale Configuration
# Creates a 5-region brain with 10,000 neurons for comprehensive testing
echo "Creating large-scale configuration..."
cat > "$CONFIG_DIR/large.cfg" << 'EOF'
# NLM Configuration File (Generated)
# Source: 0 (default)
# Large-scale configuration for comprehensive experiments
neuron_count = 10000
region_count = 5
simulation_timestep = 0.0005
connection_probability = 0.05
random_seed = 456
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_tau = 30.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.00002
EOF

# Example 3: Fast Simulation Configuration
# Creates a small brain with larger timestep for rapid testing
echo "Creating fast simulation configuration..."
cat > "$CONFIG_DIR/fast.cfg" << 'EOF'
# NLM Configuration File (Generated)
# Source: 0 (default)
# Fast simulation configuration for rapid prototyping
neuron_count = 100
region_count = 1
simulation_timestep = 0.01
connection_probability = 0.2
random_seed = 789
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_tau = 10.0
synaptogenesis_rate = 0.0005
pruning_rate = 0.000005
EOF

# Example 4: Learning-Focused Configuration
# Configuration optimized for learning experiments
echo "Creating learning-focused configuration..."
cat > "$CONFIG_DIR/learning.cfg" << 'EOF'
# NLM Configuration File (Generated)
# Source: 0 (default)
# Configuration optimized for learning experiments
neuron_count = 1000
region_count = 3
simulation_timestep = 0.001
connection_probability = 0.1
random_seed = 12345
stdp_ltp_weight = 0.018
stdp_ltd_weight = 0.017
stdp_tau = 25.0
synaptogenesis_rate = 0.0003
pruning_rate = 0.000003
EOF

# Example 5: Attention-Focused Configuration
# Configuration optimized for attentional selection experiments
echo "Creating attention-focused configuration..."
cat > "$CONFIG_DIR/attention.cfg" << 'EOF'
# NLM Configuration File (Generated)
# Source: 0 (default)
# Configuration optimized for attentional selection experiments
neuron_count = 800
region_count = 4
simulation_timestep = 0.001
connection_probability = 0.12
random_seed = 67890
stdp_ltp_weight = 0.016
stdp_ltd_weight = 0.019
stdp_tau = 15.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.000002
EOF

echo "Configuration files created in $CONFIG_DIR/"
echo "Available configurations:"
ls -la "$CONFIG_DIR"
echo ""
echo "Usage examples:"
echo "  ./nlm --config $CONFIG_DIR/basic.cfg"
echo "  ./nlm --config $CONFIG_DIR/large.cfg --dry-run"
echo "  ./nlm --config $CONFIG_DIR/fast.cfg --neurons 200"
echo "  ./nlm --config $CONFIG_DIR/learning.cfg --seed 999"
echo ""
echo "For more information, see DOCS.md"
