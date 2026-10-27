#!/usr/bin/env bash
# Example 1: Basic NLM Usage
# This is the simplest possible example to get NLM running

echo "=== Example 1: Basic NLM Usage ==="
echo "This example shows how to create and run a minimal brain simulation."
echo ""

# Create a minimal config
cat > minimal_config.cfg << 'EOF'
# Minimal NLM configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.1
EOF

echo "Created minimal_config.cfg"
echo ""
echo "To run this example with your compiled NLM binary:"
echo "  ./nlm --config minimal_config.cfg --steps 1000"
echo ""
echo "Expected output:"
echo "  - Neural connectivity test"
echo "  - Plasticity learning experiment"
echo "  - STDP verification"
echo "  - Learning detected through synaptic changes"
echo ""
echo "=== End of Example 1 ==="
echo ""
