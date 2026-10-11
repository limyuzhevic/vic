#!/usr/bin/bash
# Test NLMIntegrate tool

echo "=== Testing NLMIntegrate Tool ==="

# Create test config
echo "neuron_count = 1000" > test_config.cfg
echo "region_count = 2" >> test_config.cfg
echo "connection_probability = 0.1" >> test_config.cfg

# Test integration verification
python3 tools/NLMIntegrate/main.py --verify test_config.cfg

# Test quick integration test
python3 tools/NLMIntegrate/main.py --quick-test test_config.cfg

# Test comprehensive integration test
python3 tools/NLMIntegrate/main.py --comprehensive-test test_config.cfg

# Test all integration tests
python3 tools/NLMIntegrate/main.py --all-tests test_config.cfg

# Test health check
python3 tools/NLMIntegrate/main.py --health-check test_config.cfg

# Test report generation
python3 tools/NLMIntegrate/main.py --generate-report integration_report.txt test_config.cfg

echo "NLMIntegrate testing complete"
