#!/usr/bin/bash
# Test NLMExport tool

echo "=== Testing NLMExport Tool ==="

# Create test config
echo "neuron_count = 1000" > test_config.cfg
echo "region_count = 2" >> test_config.cfg
echo "connection_probability = 0.1" >> test_config.cfg

# Test configuration export
python3 tools/NLMExport/main.py --export-config export_config.json json test_config.cfg

# Test brain data export
python3 tools/NLMExport/main.py --export-brain-data brain_data.json json test_config.cfg

# Test simulation results export
python3 tools/NLMExport/main.py --export-simulation-results simulation_results.json test_config.cfg

# Test analysis data export
python3 tools/NLMExport/main.py --export-analysis-data analysis_data.json test_config.cfg

# Test all export to directory
python3 tools/NLMExport/main.py --export-all output_data test_config.cfg

echo "NLMExport testing complete"
