#!/usr/bin/bash
# Test NLMAnalyze tool

echo "=== Testing NLMAnalyze Tool ==="

# Create test config
echo "neuron_count = 1000" > test_config.cfg
echo "region_count = 2" >> test_config.cfg
echo "connection_probability = 0.1" >> test_config.cfg

# Test connectivity analysis
python3 tools/NLMAnalyze/main.py --connectivity test_config.cfg

# Test clustering analysis
python3 tools/NLMAnalyze/main.py --clustering test_config.cfg

# Test topology analysis
python3 tools/NLMAnalyze/main.py --topology test_config.cfg

# Test performance analysis
python3 tools/NLMAnalyze/main.py --performance test_config.cfg

# Test all analyses
python3 tools/NLMAnalyze/main.py --all test_config.cfg

# Test report generation
python3 tools/NLMAnalyze/main.py --connectivity --output analysis.txt test_config.cfg
python3 tools/NLMAnalyze/main.py --connectivity --output analysis.json --output-format json test_config.cfg

echo "NLMAnalyze testing complete"
