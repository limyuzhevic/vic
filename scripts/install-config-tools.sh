#!/bin/bash

# NLM Configuration Tool Installation Script
# This script installs the nlm-config command-line tool and related utilities

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_color() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# Function to print error and exit
error_exit() {
    print_color $RED "Error: $1"
    exit 1
}

# Function to print warning
warning() {
    print_color $YELLOW "Warning: $1"
}

# Main installation function
main() {
    print_color $GREEN "Installing NLM Configuration Tools..."
    
    # Check if we're in the right directory
    if [ ! -f "/workspace/app/CMakeLists.txt" ]; then
        error_exit "CMakeLists.txt not found. Please run this script from the NLM project root."
    fi
    
    # Create necessary directories
    mkdir -p /workspace/app/scripts /workspace/app/tools /workspace/app/presets /workspace/app/configs
    
    # Check for required dependencies
    check_dependencies() {
        local deps=("cmake" "g++" "make")
        local missing=()
        
        for dep in "${deps[@]}"; do
            if ! command -v $dep &> /dev/null; then
                missing+=($dep)
            fi
        done
        
        if [ ${#missing[@]} -gt 0 ]; then
            print_color $YELLOW "Warning: The following dependencies are missing: ${missing[*]}"
            print_color $YELLOW "The configuration tools may not work properly without these dependencies."
        fi
    }
    
    check_dependencies
    
    # Build the NLM configuration tools
    print_color $GREEN "Building NLM configuration tools..."
    
    if [ ! -d "/workspace/app/build" ]; then
        mkdir -p /workspace/app/build
        cd /workspace/app/build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
    else
        cd /workspace/app/build
        make -j4
    fi
    
    # Install the configuration tool
    print_color $GREEN "Installing configuration tool..."
    
    if [ -f "/workspace/app/build/nlm-config" ]; then
        cp /workspace/app/build/nlm-config /usr/local/bin/
        print_color $GREEN "Configuration tool installed to /usr/local/bin/nlm-config"
    else
        warning "Configuration tool executable not found. Please build the project first."
        warning "You can run './nlm-config' from the build directory."
    fi
    
    # Create configuration tools directory
    mkdir -p /usr/local/share/nlm-config
    
    # Copy configuration templates
    if [ -f "/workspace/app/configs/default.cfg" ]; then
        cp /workspace/app/configs/default.cfg /usr/local/share/nlm-config/
        print_color $GREEN "Default configuration copied to /usr/local/share/nlm-config/"
    fi
    
    # Create presets directory
    mkdir -p ~/.nlm-config/presets
    
    # Copy example presets
    cat > ~/.nlm-config/presets/development.cfg << 'EOF'
# Development configuration preset
# Optimized for rapid development and testing

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep for fast testing
simulation_timestep = 0.0001

# Small brain for quick testing
neuron_count = 500
region_count = 1

# Moderate connectivity
connection_probability = 0.1

# Enable all features for development
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Detailed logging for debugging
log_level = DEBUG
log_to_file = true
log_filename = development.log
EOF

    cat > ~/.nlm-config/presets/testing.cfg << 'EOF'
# Testing configuration preset
# Optimized for comprehensive testing

# Fixed random seed for reproducible tests
random_seed = 42

# Standard timestep for testing
simulation_timestep = 0.001

# Medium-sized brain for testing
neuron_count = 1000
region_count = 2

# Standard connectivity
connection_probability = 0.1

# Standard plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Standard development settings
development_synaptogenesis_rate = 0.0001
development_pruning_rate = 0.00001

# Standard logging
log_level = INFO
log_to_file = true
log_filename = testing.log
EOF

    cat > ~/.nlm-config/presets/production.cfg << 'EOF'
# Production configuration preset
# Optimized for production performance and stability

# Production random seed
random_seed = 12345

# Standard timestep for production
simulation_timestep = 0.001

# Standard-sized brain for production
neuron_count = 1000
region_count = 2

# Conservative connectivity
connection_probability = 0.05

# Conservative plasticity settings
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.01

# Conservative development settings
development_synaptogenesis_rate = 0.00005
development_pruning_rate = 0.000005

# Minimal logging for production
log_level = WARNING
log_to_file = true
log_filename = production.log
EOF

    print_color $GREEN "Development, testing, and production presets created in ~/.nlm-config/presets/"
    
    # Create example configuration files
    mkdir -p /workspace/app/examples/config
    
    cat > /workspace/app/examples/config/simple_example.cfg << 'EOF'
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

    cat > /workspace/app/examples/config/json_example.json << 'EOF'
{
  "nlm": {
    "version": "1.0",
    "description": "Example NLM configuration in JSON format",
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
      "log_filename": "json_example.log"
    }
  }
}
EOF

    cat > /workspace/app/examples/config/yaml_example.yaml << 'EOF'
nlm:
  version: "1.0"
  description: "Example NLM configuration in YAML format"
  configuration:
    random_seed: 42
    simulation_timestep: 0.001
    neuron_count: 500
    region_count: 2
    connection_probability: 0.1
    plasticity:
      learning_rate: 0.01
      stdp_ltp_weight: 0.02
      stdp_ltd_weight: 0.015
    development:
      synaptogenesis_rate: 0.0001
      pruning_rate: 0.00001
    log_level: "INFO"
    log_to_file: true
    log_filename: "yaml_example.log"
EOF

    print_color $GREEN "Example configuration files created in /workspace/app/examples/config/"
    
    # Create installation information
    cat > /usr/local/share/nlm-config/INSTALLATION_INFO.txt << EOF
NLM Configuration Tools Installation Information
===============================================

Installation Date: $(date)
Version: $(grep -o 'VERSION [0-9.]*' /workspace/app/CMakeLists.txt | cut -d' ' -f2)

Configuration Tool Locations:
- Executable: /usr/local/bin/nlm-config
- Configuration directory: /usr/local/share/nlm-config/
- Presets directory: ~/.nlm-config/presets/

Configuration File Formats:
- Basic: .cfg, .config (key=value format)
- JSON: .json
- YAML: .yaml, .yml

Example Configuration Files:
- Simple example: /workspace/app/examples/config/simple_example.cfg
- JSON example: /workspace/app/examples/config/json_example.json
- YAML example: /workspace/app/examples/config/yaml_example.yaml

Available Presets:
- Development: ~/.nlm-config/presets/development.cfg
- Testing: ~/.nlm-config/presets/testing.cfg
- Production: ~/.nlm-config/presets/production.cfg

Configuration Commands:
- nlm-config validate <file> - Validate configuration file
- nlm-config compare <file1> <file2> - Compare two configuration files
- nlm-config merge <file1> <file2> <output> - Merge configuration files
- nlm-config export <file> <format> - Export to different formats
- nlm-config import <file> <format> - Import from different formats
- nlm-config list - List available presets
- nlm-config create <name> - Create new preset
- nlm-config template <name> - Generate template
- nlm-config info <file> - Show configuration information

Configuration Usage:
1. Basic usage: ./nlm --config=your_config.cfg
2. With presets: ./nlm --config=~/.nlm-config/presets/development.cfg
3. With command-line overrides: ./nlm --config=my.cfg --neuron_count=500 --regions=1

For more information, visit: https://github.com/your-repo/nlm

Installation completed successfully!
EOF
    
    print_color $GREEN "Installation completed successfully!"
    print_color $GREEN "\nNext Steps:"
    print_color $YELLOW "1. Add /usr/local/bin to your PATH if not already present"
    print_color $YELLOW "2. Copy example configuration files to your project"
    print_color $YELLOW "3. Use presets or create your own configuration"
    print_color $YELLOW "4. Validate your configuration with: nlm-config validate your_config.cfg"
    print_color $YELLOW "5. Run your application: ./nlm --config=your_config.cfg"
    
    print_color $GREEN "\nConfiguration tools are now ready to use!"
}

# Run main function
main "$@"
