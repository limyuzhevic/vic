#!/bin/bash

# Setup nlm-config script for immediate use
if [ -f "/workspace/app/nlm-config" ]; then
    # Copy nlm-config to current directory for easier access
    cp /workspace/app/nlm-config ./nlm-config
    chmod +x ./nlm-config
    echo "nlm-config tool copied to current directory"
    echo "Usage: ./nlm-config [command] [options]"
else
    echo "nlm-config not found in /workspace/app"
fi

# Create sample configuration files if they don't exist
if [ ! -f "simple.cfg" ]; then
    cat > simple.cfg << 'EOF'
# Simple NLM Configuration Example
# This configuration is optimized for basic functionality testing

# Basic settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1

# Connectivity settings
connection_probability = 0.05

# Plasticity settings (enabled)
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Development settings (enabled)
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Logging settings
log_level = INFO
log_to_file = false
EOF
    echo "Created simple.cfg"
fi

if [ ! -f "advanced.json" ]; then
    cat > advanced.json << 'EOF'
{
  "nlm": {
    "version": "1.0",
    "description": "Advanced NLM configuration in JSON format",
    "configuration": {
      "random_seed": 42,
      "simulation_timestep": 0.001,
      "neuron_count": 500,
      "region_count": 2,
      "connection_probability": 0.1,
      "plasticity": {
        "learning_rate": 0.01,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
      },
      "development": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
      },
      "log_level": "INFO",
      "log_to_file": true,
      "log_filename": "advanced.log"
    }
  }
}
EOF
    echo "Created advanced.json"
fi

if [ ! -f "development.yaml" ]; then
    cat > development.yaml << 'EOF'
nlm:
  version: "1.0"
  description: "Development configuration in YAML format"
  configuration:
    random_seed: 42
    simulation_timestep: 0.001
    neuron_count: 1000
    region_count: 3
    connection_probability: 0.15
    plasticity:
      learning_rate: 0.02
      stdp_ltp_weight: 0.025
      stdp_ltd_weight: 0.02
    development:
      synaptogenesis_rate: 0.002
      pruning_rate: 0.0002
    log_level: "DEBUG"
    log_to_file: true
    log_filename: "development.log"
EOF
    echo "Created development.yaml"
fi

# Create examples directory
mkdir -p examples/config

# Copy configuration examples to examples directory
if [ ! -f "examples/config/simple_example.cfg" ]; then
    cp simple.cfg examples/config/simple_example.cfg
    echo "Created examples/config/simple_example.cfg"
fi

if [ ! -f "examples/config/json_example.json" ]; then
    cp advanced.json examples/config/json_example.json
    echo "Created examples/config/json_example.json"
fi

if [ ! -f "examples/config/yaml_example.yaml" ]; then
    cp development.yaml examples/config/yaml_example.yaml
    echo "Created examples/config/yaml_example.yaml"
fi

# Create a test run script
cat > test_config.sh << 'EOF'
#!/bin/bash

# Test configuration management with nlm-config

echo "NLM Configuration Management Test"
echo "================================="

# Test 1: Validate configuration files
echo "\n1. Validating configuration files:"
if [ -f "./nlm-config" ]; then
    echo "   Testing simple.cfg:"
    ./nlm-config validate simple.cfg
    echo "   Testing advanced.json:"
    ./nlm-config validate advanced.json
    echo "   Testing development.yaml:"
    ./nlm-config validate development.yaml
fi

# Test 2: Compare configuration files
echo "\n2. Comparing configuration files:"
if [ -f "./nlm-config" ]; then
    echo "   Comparing simple.cfg with simple.cfg (should be identical):"
    ./nlm-config compare simple.cfg simple.cfg
fi

# Test 3: Create preset
echo "\n3. Creating test preset:"
if [ -f "./nlm-config" ]; then
    ./nlm-config create test_preset
fi

# Test 4: List presets
echo "\n4. Listing presets:"
if [ -f "./nlm-config" ]; then
    ./nlm-config list
fi

# Test 5: Show preset info
echo "\n5. Showing preset info:"
if [ -f "./nlm-config" ]; then
    ./nlm-config info ~/.nlm-config/presets/test_preset.cfg
fi

# Test 6: Create backup
echo "\n6. Creating backup:"
if [ -f "./nlm-config" ]; then
    ./nlm-config backup
fi

# Test 7: Export configuration
echo "\n7. Exporting configuration:"
if [ -f "./nlm-config" ]; then
    ./nlm-config export simple.cfg simple_output.json
fi

# Test 8: Import configuration
echo "\n8. Importing configuration:"
if [ -f "./nlm-config" ]; then
    ./nlm-config import advanced.json simple_imported.cfg
fi

# Test 9: Merge configurations
echo "\n9. Merging configurations:"
if [ -f "./nlm-config" ]; then
    ./nlm-config merge simple.cfg simple.cfg merged.cfg
fi

echo "\nConfiguration management test completed!"
EOF

chmod +x test_config.sh

echo "Created test_config.sh for testing configuration management"

echo "\nSetup complete! You can now:
   - Use ./nlm-config to manage configurations
   - Run ./test_config.sh to test the configuration tools
   - Use simple.cfg, advanced.json, or development.yaml as examples
   - Find more examples in examples/config/
"
