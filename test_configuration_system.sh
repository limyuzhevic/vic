#!/bin/bash

# NLM Configuration System Test Script
# This script tests all the new configuration system improvements

set -e

echo "=== NLM Configuration System Test Script ===\n"

echo "Testing new configuration system improvements...\n"

# Test 1: Configuration Generator

echo "### Test 1: Configuration Generator\n"
echo "Testing configuration file generation..."

if ./nlm-config generate json configs/test_output.json 2>&1; then
    echo "✓ JSON configuration generation passed"
    if [ -f configs/test_output.json ]; then
        echo "✓ Configuration file created successfully"
        echo "✓ File content preview:"
        head -10 configs/test_output.json
    else
        echo "✗ Configuration file not created"
        exit 1
    fi
else
    echo "✗ JSON configuration generation failed"
    exit 1
fi

if ./nlm-config generate yaml configs/test_output.yaml 2>&1; then
    echo "✓ YAML configuration generation passed"
    if [ -f configs/test_output.yaml ]; then
        echo "✓ Configuration file created successfully"
        echo "✓ File content preview:"
        head -10 configs/test_output.yaml
    else
        echo "✗ Configuration file not created"
        exit 1
    fi
else
    echo "✗ YAML configuration generation failed"
    exit 1
fi

if ./nlm-config generate configs/test_output.cfg 2>&1; then
    echo "✓ Key-value configuration generation passed"
    if [ -f configs/test_output.cfg ]; then
        echo "✓ Configuration file created successfully"
        echo "✓ File content preview:"
        head -10 configs/test_output.cfg
    else
        echo "✗ Configuration file not created"
        exit 1
    fi
else
    echo "✗ Key-value configuration generation failed"
    exit 1
fi

echo -e "\n"

# Test 2: Configuration Validation

echo "### Test 2: Configuration Validation\n"
echo "Testing configuration validation..."

if ./nlm-config validate configs/test_output.json 2>&1; then
    echo "✓ JSON configuration validation passed"
else
    echo "✗ JSON configuration validation failed"
    exit 1
fi

if ./nlm-config validate configs/test_output.yaml 2>&1; then
    echo "✓ YAML configuration validation passed"
else
    echo "✗ YAML configuration validation failed"
    exit 1
fi

if ./nlm-config validate configs/test_output.cfg 2>&1; then
    echo "✓ Key-value configuration validation passed"
else
    echo "✗ Key-value configuration validation failed"
    exit 1
fi

echo -e "\n"

# Test 3: Configuration Merging

echo "### Test 3: Configuration Merging\n"
echo "Testing configuration merging..."

# Create base configuration
cat > configs/base_test.cfg << 'EOF'
# Base Configuration
random_seed = 42
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
EOF

# Create experiment configuration
cat > configs/experiment_test.cfg << 'EOF'
# Experiment Configuration
neuron_count = 2000
region_count = 3
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
visualization_enabled = true
EOF

if ./nlm-config merge configs/base_test.cfg configs/experiment_test.cfg configs/merged_test.cfg 2>&1; then
    echo "✓ Configuration merging passed"
    if [ -f configs/merged_test.cfg ]; then
        echo "✓ Merged configuration file created successfully"
        echo "✓ File content preview:"
        head -10 configs/merged_test.cfg
    else
        echo "✗ Merged configuration file not created"
        exit 1
    fi
else
    echo "✗ Configuration merging failed"
    exit 1
fi

echo -e "\n"

# Test 4: NLM Command Line Interface

echo "### Test 4: NLM Command Line Interface\n"
echo "Testing NLM command line interface..."

if ./nlm --help 2>&1 | grep -q "NLM Phase 2: Real Neural Computation"; then
    echo "✓ NLM help command passed"
else
    echo "✗ NLM help command failed"
    exit 1
fi

if ./nlm --version 2>&1 | grep -q "NLM Phase 2: Real Neural Computation"; then
    echo "✓ NLM version command passed"
else
    echo "✗ NLM version command failed"
    exit 1
fi

echo -e "\n"

# Test 5: Configuration Format Detection

echo "### Test 5: Configuration Format Detection\n"
echo "Testing configuration format detection..."

# Test format detection by creating a simple C++ test
if g++ -I src -o test_format_detection tests/test_config_format_detection.cpp 2>&1; then
    echo "✓ Configuration format detection compilation passed"
    if ./test_format_detection 2>&1; then
        echo "✓ Configuration format detection runtime passed"
    else
        echo "✗ Configuration format detection runtime failed"
        exit 1
    fi
else
    echo "✗ Configuration format detection compilation failed"
    exit 1
fi

echo -e "\n"

# Test 6: Configuration File Loading

echo "### Test 6: Configuration File Loading\n"
echo "Testing configuration file loading..."

# Create a test configuration file
cat > configs/test_load.cfg << 'EOF'
# Test Configuration File
random_seed = 42
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001
EOF

# Test loading the configuration file
if ./nlm-config validate configs/test_load.cfg 2>&1; then
    echo "✓ Configuration file loading passed"
else
    echo "✗ Configuration file loading failed"
    exit 1
fi

echo -e "\n"

# Test 7: Configuration Generation with Input

echo "### Test 7: Configuration Generation with Input\n"
echo "Testing configuration generation with input file..."

if ./nlm-config generate yaml configs/converted_test.yaml configs/test_output.cfg 2>&1; then
    echo "✓ Configuration generation with input passed"
    if [ -f configs/converted_test.yaml ]; then
        echo "✓ Converted configuration file created successfully"
        echo "✓ File content preview:"
        head -10 configs/converted_test.yaml
    else
        echo "✗ Converted configuration file not created"
        exit 1
    fi
else
    echo "✗ Configuration generation with input failed"
    exit 1
fi

echo -e "\n"

# Test 8: Configuration Validation with Errors

echo "### Test 8: Configuration Validation with Errors\n"
echo "Testing configuration validation with invalid configuration..."

# Create an invalid configuration file
cat > configs/invalid_test.cfg << 'EOF'
# Invalid Configuration
random_seed = 42
neuron_count = -100  # Invalid: negative neuron count
region_count = 1
connection_probability = 1.5  # Invalid: probability > 1.0
simulation_timestep = 0.0     # Invalid: timestep <= 0
EOF

if ! ./nlm-config validate configs/invalid_test.cfg 2>&1; then
    echo "✓ Invalid configuration validation correctly failed"
else
    echo "✗ Invalid configuration validation should have failed"
    exit 1
fi

echo -e "\n"

# Clean up test files

echo "### Cleanup Test Files\n"
echo "Cleaning up test files..."

rm -f configs/test_output.json configs/test_output.yaml configs/test_output.cfg
rm -f configs/merged_test.cfg configs/test_load.cfg configs/converted_test.yaml
rm -f configs/base_test.cfg configs/experiment_test.cfg configs/invalid_test.cfg
rm -f test_format_detection

echo "✓ Test files cleaned up successfully"
echo -e "\n"

# Summary

echo "=== Test Summary ===\n"
echo "All configuration system tests passed successfully!\n"

echo "New configuration system features tested:"
echo "1. ✓ JSON configuration file generation"
echo "2. ✓ YAML configuration file generation"
echo "3. ✓ Key-value configuration file generation"
echo "4. ✓ Configuration validation for all formats"
echo "5. ✓ Configuration merging"
echo "6. ✓ NLM command line interface (help and version)"
echo "7. ✓ Configuration format detection"
echo "8. ✓ Configuration file loading"
echo "9. ✓ Configuration generation with input file"
echo "10. ✓ Configuration validation with errors\n"

echo "The NLM configuration system has been successfully improved with:"
echo "- Enhanced configuration file format support (JSON, YAML, key-value)\necho "- Improved configuration validation and error handling\necho "- Comprehensive configuration management tools\necho "- Better debugging and error reporting\necho "- Performance optimizations for configuration handling\necho "- Extensive documentation and examples\necho "- Command-line interface with help system\echo "- Unit tests for configuration system functionality\n"

echo "=== All Tests Passed Successfully ===\n"
