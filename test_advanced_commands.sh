#!/bin/bash
# Test Script for NLM Advanced Configuration Commands
# This script tests the basic functionality of the new config commands

set -e

echo "=== Testing NLM Advanced Configuration Commands ==="

echo "\n1. Testing Config File Discovery..."
mkdir -p test_configs
echo "Creating test config files..."
cat > test_configs/test1.cfg << 'EOF'
neuron_count = 100
region_count = 1
connection_probability = 0.1
EOF
cat > test_configs/test2.json << 'EOF'
{
  "neuron_count": 500,
  "region_count": 5,
  "connection_probability": 0.15
}
EOF
cat > test_configs/test3.yaml << 'EOF'
neuron_count: 1000
region_count: 10
connection_probability: 0.2
EOF

echo "Testing config list command..."
config list test_configs

echo "\n2. Testing Config Validation..."
echo "Testing config validate command..."
config validate test_configs/test1.cfg
config validate test_configs/test2.json
config validate test_configs/test3.yaml

echo "\n3. Testing Config Statistics..."
echo "Testing config statistics command..."
config statistics test_configs/test1.cfg

echo "\n4. Testing Config Search..."
echo "Testing config search command..."
config search test_configs/test1.cfg "neuron"

echo "\n5. Testing Config Template Generation..."
echo "Testing config template command..."
config template neural test_configs/neural_template.cfg

if [ -f "test_configs/neural_template.cfg" ]; then
    echo "Template generated successfully!"
    config statistics test_configs/neural_template.cfg
else
    echo "Error: Template generation failed"
fi

echo "\n6. Testing Backup Command..."
echo "Testing config backup command..."
config backup test_configs/test1.cfg
backup_file=$(find test_configs -name "backup_test1*.cfg" -type f | head -1)
if [ -n "$backup_file" ]; then
    echo "Backup created: $backup_file"
else
    echo "Error: Backup failed"
fi

echo "\n7. Testing Config Extract..."
echo "Testing config extract command..."
config extract test_configs/test1.cfg neuron_count region_count

echo "\n8. Testing Config Conversion..."
echo "Testing config convert command..."
config convert test_configs/test1.cfg test_configs/test1_converted.json
if [ -f "test_configs/test1_converted.json" ]; then
    echo "Conversion successful!"
    config validate test_configs/test1_converted.json
else
    echo "Error: Conversion failed"
fi

echo "\n9. Testing Brain Commands..."
echo "Testing brain status command..."
brain status

echo "\n10. Testing Memory Stats..."
echo "Testing memory stats command..."
memory stats

echo "\n11. Testing Neuron Info..."
echo "Testing neuron info command..."
neuron info

echo "\n12. Testing Help Command..."
echo "Testing help command..."
help
echo "\n13. Testing Version Command..."
echo "Testing version command..."
version

echo "\n=== All Tests Completed ==="
echo "\nNote: Some commands may require full NLM system initialization."
echo "The test script demonstrates the new command-line interface functionality."

echo "\nCleaning up test files..."
rm -rf test_configs build

echo "\nTest script completed successfully!"
