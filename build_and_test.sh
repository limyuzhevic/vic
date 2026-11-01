#!/usr/bin/bash
# Compilation script for NLM enhanced configuration system

set -e

echo "Building NLM Enhanced Configuration System..."

echo "\n1. Compiling demo program..."
g++ -std=c++20 -O2 -Wall -Wextra -I. demo_config_system.cpp -o demo_config_system

echo "\n2. Compiling enhanced CLI..."
g++ -std=c++20 -O2 -Wall -Wextra -I. nlm_cli.cpp -o nlm_cli

echo "\n3. Testing configuration system demo..."
./demo_config_system

echo "\n4. Testing enhanced CLI help..."
./nlm_cli --help

echo "\n5. Testing enhanced CLI version..."
./nlm_cli --version

echo "\n6. Testing enhanced CLI with sample arguments..."
./nlm_cli --neuron_count=2000 --simulation_timestep=0.005 --verbose

echo "\n7. Testing enhanced CLI validation...")
./nlm_cli --neuron_count=-1000 --simulation_timestep=0.0

echo "\n8. Testing enhanced CLI auto-fix...")
./nlm_cli --neuron_count=-1000 --simulation_timestep=0.0 --validate

echo "\n9. Testing enhanced CLI with config file...")
cat > test_config.cfg << EOF
# Test configuration
neuron_count = 1500
simulation_timestep = 0.002
connection_probability = 0.12
EOF

./nlm_cli --config=test_config.cfg

# Cleanup
echo "\nCleaning up test files..."
rm -f demo_config_system nlm_cli test_config.cfg

echo "\nBuild and test completed successfully!"