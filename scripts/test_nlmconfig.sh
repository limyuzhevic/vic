#!/usr/bin/bash
# Test NLMConfig tool

echo "=== Testing NLMConfig Tool ==="

# Test with simple config
echo "neuron_count = 1000" > test_config.cfg
echo "region_count = 2" >> test_config.cfg
echo "connection_probability = 0.1" >> test_config.cfg

# Test Python version
echo "Testing Python version..."
python3 tools/NLMConfig/main.py --validate test_config.cfg

# Test generation
python3 tools/NLMConfig/main.py --generate generated_config.cfg

# Test export
python3 tools/NLMConfig/main.py --export-json export_config.json test_config.cfg

# Test validation with invalid config
echo "neuron_count = -1" > invalid_config.cfg
echo "region_count = 2" >> invalid_config.cfg
echo "connection_probability = 0.1" >> invalid_config.cfg
python3 tools/NLMConfig/main.py --validate invalid_config.cfg

echo "NLMConfig testing complete"
