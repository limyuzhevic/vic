#!/usr/bin/bash
# NLM Examples Setup Script
# This script creates example configuration files and demonstrates usage

# Create examples directory
mkdir -p examples/config
mkdir -p examples/scripts
mkdir -p examples/output
mkdir -p examples/results

# Create example configuration files

# 1. Basic configuration (simple .cfg format)
cat > examples/config/basic.cfg << 'EOF'
# Basic NLM Configuration
# Simple key=value format for quick setup

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 500

# Number of brain regions
region_count = 1

# Connection probability for random connectivity
connection_probability = 0.15

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Neuromodulation settings
dopamine_baseline = 0.1
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
EOF

# 2. Advanced configuration (JSON format)
cat > examples/config/advanced.json << 'EOF'
{
  "nlm": {
    "version": "2.0.0",
    "configuration": {
      "random_seed": 42,
      "simulation_timestep": 0.001,
      "neuron_count": 1000,
      "region_count": 2,
      "connection_probability": 0.2,
      "plasticity_learning_rate": 0.02,
      "stdp_ltp_weight": 0.03,
      "stdp_ltd_weight": 0.025,
      "stdp_time_constant": 25.0,
      "synaptogenesis_rate": 0.00015,
      "pruning_rate": 0.00002,
      "dopamine_baseline": 0.15,
      "reward_discount_factor": 0.98,
      "environment_name": "MazeWorld",
      "environment_width": 20,
      "environment_height": 20,
      "log_level": "DEBUG",
      "log_to_file": true,
      "log_filename": "nlm_advanced.log",
      "max_simulation_steps": 50000,
      "simulation_time_limit": 1000.0,
      "visualization_enabled": true,
      "visualization_update_rate": 60.0
    }
  }
}
EOF

# 3. Production configuration (minimal)
cat > examples/config/production.cfg << 'EOF'
# Production NLM Configuration
# Minimal settings for production use

random_seed = 12345
simulation_timestep = 0.0001
neuron_count = 2000
region_count = 3
connection_probability = 0.1

# Essential plasticity settings
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.008

# Production logging
log_level = WARNING
log_to_file = true
log_filename = nlm_production.log

# Performance settings
max_simulation_steps = 100000
simulation_time_limit = 0.0
EOF

# 4. Development configuration
cat > examples/config/development.cfg << 'EOF'
# Development NLM Configuration
# Settings optimized for development and debugging

random_seed = 0  # Use random seed for development

simulation_timestep = 0.001
neuron_count = 100  # Small for quick testing
region_count = 1
connection_probability = 0.3  # Higher connectivity for testing

# Aggressive plasticity for faster learning
plasticity_learning_rate = 0.1
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.04

# Debug logging
log_level = DEBUG
log_to_file = false

# Short simulation for development
max_simulation_steps = 1000
simulation_time_limit = 0.0
EOF

# 5. Schema configuration
cat > examples/config/config_schema.json << 'EOF'
{
  "schema": {
    "version": "2.0.0",
    "entries": [
      {
        "key": "random_seed",
        "type": "int64",
        "description": "Random seed for reproducible experiments",
        "required": true,
        "default": 42,
        "allowed_values": [],
        "validator": "range_0_100000"
      },
      {
        "key": "simulation_timestep",
        "type": "double",
        "description": "Simulation timestep in seconds",
        "required": true,
        "default": 0.001,
        "allowed_values": [],
        "validator": "range_0_001_1_0"
      },
      {
        "key": "neuron_count",
        "type": "int64",
        "description": "Number of neurons in the brain",
        "required": true,
        "default": 500,
        "allowed_values": [],
        "validator": "range_1_100000"
      },
      {
        "key": "region_count",
        "type": "int64",
        "description": "Number of brain regions",
        "required": true,
        "default": 1,
        "allowed_values": [],
        "validator": "range_1_10"
      },
      {
        "key": "connection_probability",
        "type": "double",
        "description": "Connection probability for random connectivity",
        "required": true,
        "default": 0.15,
        "allowed_values": [],
        "validator": "range_0_1"
      },
      {
        "key": "log_level",
        "type": "string",
        "description": "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)",
        "required": true,
        "default": "INFO",
        "allowed_values": ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"],
        "validator": "enum"
      },
      {
        "key": "visualization_enabled",
        "type": "bool",
        "description": "Enable visualization",
        "required": true,
        "default": false,
        "allowed_values": [],
        "validator": "boolean"
      }
    ]
  }
}
EOF

# 6. Script examples

# Basic run script
cat > examples/scripts/basic_run.sh << 'EOF'
#!/bin/bash

# Basic NLM run script
# This script demonstrates how to run NLM with basic configuration

CONFIG_FILE="config/basic.cfg"
OUTPUT_DIR="results"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Run NLM with basic configuration
./nlm run --config "$CONFIG_FILE" --steps 1000 --verbose

# Check if simulation completed successfully
if [ $? -eq 0 ]; then
    echo "✓ Basic simulation completed successfully"
    echo "Results saved to: $OUTPUT_DIR/"
else
    echo "✗ Basic simulation failed"
    exit 1
fi
EOF

# Advanced run script
cat > examples/scripts/advanced_run.sh << 'EOF'
#!/bin/bash

# Advanced NLM run script
# This script demonstrates how to run NLM with advanced configuration and options

CONFIG_FILE="config/advanced.json"
OUTPUT_DIR="results"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Validate configuration first
echo "Validating configuration..."
nlm run --config "$CONFIG_FILE" --validate-only

if [ $? -eq 0 ]; then
    echo "✓ Configuration validation passed"
    
    # Run advanced simulation
    echo "Running advanced simulation..."
    nlm run --config "$CONFIG_FILE" --steps 5000 --verbose --output "$OUTPUT_DIR/advanced_results.json"
    
    if [ $? -eq 0 ]; then
        echo "✓ Advanced simulation completed successfully"
        echo "Results saved to: $OUTPUT_DIR/"
    else
        echo "✗ Advanced simulation failed"
        exit 1
    fi
else
    echo "✗ Configuration validation failed"
    exit 1
fi
EOF

# Development run script
cat > examples/scripts/development_run.sh << 'EOF'
#!/bin/bash

# Development NLM run script
# This script demonstrates how to run NLM for development and debugging

CONFIG_FILE="config/development.cfg"
OUTPUT_DIR="results"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Run development simulation with debug output
echo "Running development simulation..."
nlm run --config "$CONFIG_FILE" --steps 500 --verbose

if [ $? -eq 0 ]; then
    echo "✓ Development simulation completed successfully"
    echo "Results saved to: $OUTPUT_DIR/"
else
    echo "✗ Development simulation failed"
    exit 1
fi
EOF

# Memory management script
cat > examples/scripts/memory_analysis.sh << 'EOF'
#!/bin/bash

# Memory analysis script
# This script demonstrates how to analyze memory usage

echo "=== NLM Memory Analysis ==="
echo ""

echo "1. Checking available memory systems..."
echo "Available memory types:"
echo "  - Working Memory (LRU eviction)"
echo "  - Working Memory (LFU eviction)"
echo "  - Working Memory (FIFO eviction)"
echo "  - Working Memory (HEBBIAN eviction)"
echo "  - Working Memory (ADAPTIVE eviction)"
echo "  - Episodic Memory (with replay)"
echo "  - Neural Working Memory"
echo "  - Neural Episodic Memory"
echo ""

echo "2. Example memory configurations:"
echo "Working Memory capacities: 100, 500, 1000, 5000, 10000"
echo "Episodic Memory capacities: 1000, 5000, 10000, 50000"
echo ""

echo "3. Memory usage optimization tips:"
echo "  - Use appropriate eviction policies for your use case"
echo "  - Monitor memory occupancy ratios"
echo "  - Use decay models to manage memory growth"
echo "  - Consider batch operations for efficiency"
echo "  - Implement custom memory management for specialized needs"
echo ""

echo "4. Memory statistics available:"
echo "  - Current size and capacity"
echo "  - Occupancy ratios"
echo "  - Access patterns"
echo "  - Importance distributions"
echo "  - Decay statistics"
echo ""

echo "=== Memory Analysis Complete ==="
EOF

# Configuration management script
cat > examples/scripts/config_manager.sh << 'EOF'
#!/bin/bash

# Configuration management script
# This script demonstrates how to manage configuration files

CONFIG_DIR="config"
SCHEMA_FILE="config_schema.json"

# List all configuration files
function list_configs() {
    echo "=== Available Configuration Files ==="
    echo ""
    
    if [ -d "$CONFIG_DIR" ]; then
        echo "Basic configuration files:"
        for config in "$CONFIG_DIR"/*.cfg; do
            if [ -f "$config" ]; then
                echo "  $(basename "$config")"
            fi
        done
        
        echo ""
        echo "JSON configuration files:"
        for config in "$CONFIG_DIR"/*.json; do
            if [ -f "$config" ]; then
                echo "  $(basename "$config") (JSON)"
            fi
        done
        
        echo ""
        echo "Schema files:"
        if [ -f "$SCHEMA_FILE" ]; then
            echo "  $(basename "$SCHEMA_FILE")"
        else
            echo "  No schema file found"
        fi
    else
        echo "No configuration directory found"
    fi
}

# Create configuration from template
function create_config() {
    local template=$1
    local name=$2
    
    if [ -f "$CONFIG_DIR/$template" ]; then
        cp "$CONFIG_DIR/$template" "$CONFIG_DIR/${name}.cfg"
        echo "Created configuration: $CONFIG_DIR/${name}.cfg"
    else
        echo "Template not found: $template"
        exit 1
    fi
}

# Validate configuration
function validate_config() {
    local config=$1
    
    if [ -f "$CONFIG_DIR/$config" ]; then
        echo "Validating configuration: $config"
        nlm run --config "$CONFIG_DIR/$config" --validate-only
        return $?
    else
        echo "Configuration not found: $config"
        return 1
    fi
}

# Show configuration
function show_config() {
    local config=$1
    local key=$2
    
    if [ -f "$CONFIG_DIR/$config" ]; then
        echo "Showing configuration: $config"
        nlm config show "$key"
    else
        echo "Configuration not found: $config"
    fi
}

# Main script
case "$1" in
    list)
        list_configs
        ;;
    create)
        create_config "$2" "$3"
        ;;
    validate)
        validate_config "$2"
        ;;
    show)
        show_config "$2" "$3"
        ;;
    *)
        echo "Usage: $0 {list|create|validate|show}"
        echo ""
        echo "Commands:"
        echo "  list                    List all configuration files"
        echo "  create <template> <name> Create configuration from template"
        echo "  validate <config>       Validate configuration file"
        echo "  show <config> <key>     Show specific configuration value"
        echo ""
        echo "Examples:"
        echo "  $0 list"
        echo "  $0 create basic.cfg myconfig"
        echo "  $0 validate myconfig"
        echo "  $0 show myconfig neuron_count"
        ;;
esac
EOF

# Memory optimization script
cat > examples/scripts/memory_optimize.sh << 'EOF'
#!/bin/bash

# Memory optimization script
# This script demonstrates how to optimize memory usage

echo "=== NLM Memory Optimization ==="
echo ""

echo "1. Working Memory Optimization:"
echo "   - Choose appropriate eviction policy:"
echo "     * FIFO: Simple, predictable eviction"
echo "     * LRU: Good for frequently accessed items"
echo "     * LFU: Good for usage-based importance"
echo "     * HEBBIAN: Good for learning-based importance"
echo "     * ADAPTIVE: Balanced approach"
echo ""

echo "2. Episodic Memory Optimization:"
echo "   - Use appropriate capacity:"
echo "     * 1000: Small, fast operations"
echo "     * 5000: Medium, balanced"
echo "     * 10000: Large, comprehensive"
echo "     * 50000: Very large, research use"
echo ""

echo "3. Decay Configuration:"
echo "   - Exponential decay: Good for natural decay"
echo "   - Linear decay: Simple, predictable"
echo "   - Contextual decay: Adaptive to usage"
echo "   - No decay: For permanent storage"
echo ""

echo "4. Memory Monitoring:"
echo "   Monitor these metrics regularly:"
echo "   - Current size / capacity ratio"
echo "   - Access patterns"
echo "   - Importance distribution"
echo "   - Decay effectiveness"
echo ""

echo "5. Optimization Tips:"
echo "   - Use batch operations for efficiency"
echo "   - Monitor memory occupancy"
echo "   - Adjust eviction policies as needed"
echo "   - Consider custom memory management"
echo "   - Use appropriate decay models"
echo ""

echo "=== Memory Optimization Complete ==="
EOF

# Set executable permissions
chmod +x examples/scripts/*.sh

echo "=== NLM Examples Setup Complete ==="
echo ""
echo "Created the following example files:"
echo ""
echo "Configuration files:"
echo "  examples/config/basic.cfg - Basic configuration"
echo "  examples/config/advanced.json - Advanced JSON configuration"
echo "  examples/config/production.cfg - Production configuration"
echo "  examples/config/development.cfg - Development configuration"
echo "  examples/config/config_schema.json - Configuration schema"
echo ""
echo "Script examples:"
echo "  examples/scripts/basic_run.sh - Basic run script"
echo "  examples/scripts/advanced_run.sh - Advanced run script"
echo "  examples/scripts/development_run.sh - Development run script"
echo "  examples/scripts/memory_analysis.sh - Memory analysis script"
echo "  examples/scripts/config_manager.sh - Configuration management script"
echo "  examples/scripts/memory_optimize.sh - Memory optimization script"
echo ""
echo "Usage examples:"
echo "  ./examples/scripts/basic_run.sh"
echo "  ./examples/scripts/advanced_run.sh"
echo "  ./examples/scripts/development_run.sh"
echo ""
echo "Configuration validation:"
echo "  ./examples/scripts/config_manager.sh validate basic"
echo "  ./examples/scripts/config_manager.sh list"
echo ""
echo "Memory analysis:"
echo "  ./examples/scripts/memory_analysis.sh"
echo ""
echo "=== Setup Complete ==="
