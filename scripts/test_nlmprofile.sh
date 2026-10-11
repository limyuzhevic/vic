#!/usr/bin/bash
# Test NLMProfile tool

echo "=== Testing NLMProfile Tool ==="

# Create test config
echo "neuron_count = 1000" > test_config.cfg
echo "region_count = 2" >> test_config.cfg
echo "connection_probability = 0.1" >> test_config.cfg

# Test performance profiling (short duration)
python3 tools/NLMProfile/main.py --performance 5 test_config.cfg

# Test memory profiling
python3 tools/NLMProfile/main.py --memory test_config.cfg

# Test memory leak detection (short duration)
python3 tools/NLMProfile/main.py --memory-leak 10 test_config.cfg

# Test bottleneck detection
python3 tools/NLMProfile/main.py --bottlenecks test_config.cfg

# Test configuration benchmarking
python3 tools/NLMProfile/main.py --benchmark "neuron_count=500,region_count=1" test_config.cfg

# Test all profiling
python3 tools/NLMProfile/main.py --all test_config.cfg

# Test report generation
python3 tools/NLMProfile/main.py --performance 5 --output profile.txt test_config.cfg
python3 tools/NLMProfile/main.py --performance 5 --output profile.json --output-format json test_config.cfg

echo "NLMProfile testing complete"
