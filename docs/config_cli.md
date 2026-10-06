# NLM Config CLI Documentation

# Overview

The NLM Config CLI is a comprehensive command-line toolkit for managing NLM configurations. It provides users with convenient tools to work with configuration files without needing Python knowledge or manual file editing.

# Installation

## Prerequisites

- Python 3.8 or higher
- PyYAML (for YAML support)
- NLM Python bindings

## Installation Steps

1. **Clone the NLM repository**
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

2. **Install NLM Python bindings**
```bash
pip install -e python/
```

3. **Install additional dependencies**
```bash
pip install PyYAML
```

4. **Make CLI tools executable**
```bash
chmod +x scripts/nlm-config scripts/config_validation.py
```

# Usage

## Basic Commands

### List Configuration Keys

List all keys and values in a configuration file:

```bash
nlm-config list --file config.yaml
```

Output:
```
Configuration keys and values:
  neuron_count: 1000
  region_count: 3
  simulation_timestep: 0.001
```

### Get Configuration Value

Get the value of a specific key:

```bash
nlm-config get --file config.yaml neuron_count
```

Output:
```
neuron_count: 1000
```

### Set Configuration Value

Set or update a configuration value (modifies the file):

```bash
nlm-config set --file config.yaml neuron_count 2000
```

Output:
```
Set neuron_count = 2000
Configuration saved to config.yaml
```

## Configuration File Loading

### From File

Load configurations from various file formats:

```bash
# JSON format
nlm-config list --file config.json

# YAML format
nlm-config list --file config.yaml

# Simple text format (key=value)
nlm-config list --file config.txt
```

### From Command Line

Load configuration directly from command line arguments:

```bash
nlm-config neuron_count=500 region_count=2 simulation_timestep=0.001 list
```

### From Environment Variables

Load configuration from environment variables:

```bash
export NLM_NEURON_COUNT=1000
export NLM_REGION_COUNT=3
nlm-config env-load
```

## Configuration Templates

Generate predefined configuration templates:

```bash
# Default configuration
nlm-config template default --output default_config.yaml

# Debug configuration
nlm-config template debug --output debug_config.yaml

# Performance configuration
nlm-config template performance --output performance_config.yaml

# Creative exploration configuration
nlm-config template creative --output creative_config.yaml
```

## Configuration Validation

Validate configurations against schemas and constraints:

### Basic Validation

```bash
nlm-config validate --file config.yaml
```

Output:
```
✓ Configuration is valid
```

### Detailed Validation

```bash
nlm-config validate --file config.yaml --verbose
```

### Validation with Schema

```bash
nlm-config validate --file config.yaml --schema schema.yaml
```

## Configuration Comparison

Compare two configuration files:

```bash
nlm-config compare config1.yaml config2.yaml
```

Output:
```
Configuration Comparison:
  Keys only in config1.yaml: ['old_param']
  Keys only in config2.yaml: ['new_param']
  Common keys: 15
  Differences:
    neuron_count: 1000 -> 2000
    region_count: 3 -> 5
```

## Configuration Merging

Merge multiple configuration files:

```bash
nlm-config merge config1.yaml config2.yaml config3.yaml
```

## Saving Configuration

Save configuration in different formats:

```bash
# Save as JSON
nlm-config save --file config.yaml --output config.json --format json

# Save as YAML
nlm-config save --file config.yaml --output config.yaml --format yaml

# Save as simple text
nlm-config save --file config.yaml --output config.txt --format simple
```

## Environment Variable Integration

### Export to Environment

Export configuration to environment variables:

```bash
nlm-config env-export --file config.yaml
```

This will set environment variables like:
```bash
export NLM_NEURON_COUNT=1000
export NLM_REGION_COUNT=3
```

### Unset Environment Variables

Unset environment variables with a specific prefix:

```bash
nlm-config env-export --file config.yaml --unset
```

## Configuration Validation Tools

The `config_validation.py` script provides additional validation capabilities:

### Validate Configuration

```bash
python3 config_validation.py validate config.yaml
```

### Generate Validation Report

```bash
python3 config_validation.py validate --report report.txt config.yaml
```

### Compare Configurations

```bash
python3 config_validation.py compare config1.yaml config2.yaml
```

### Validate Schema

```bash
python3 config_validation.py validate-schema schema.yaml
```

### Load from Environment

```bash
python3 config_validation.py env-load
```

### Export to Environment

```bash
python3 config_validation.py env-export config.yaml
```

# Configuration File Formats

## JSON Format

JSON configuration files are straightforward:

```json
{
  "neuron_count": 1000,
  "region_count": 3,
  "connection_probability": 0.1,
  "enable_structural_plasticity": true
}
```

## YAML Format

YAML is preferred for human-readable configurations:

```yaml
neuron_count: 1000
region_count: 3
connection_probability: 0.1
enable_structural_plasticity: true
```

## Simple Format

Simple key=value format for quick configurations:

```
# config.txt
neuron_count=1000
region_count=3
connection_probability=0.1
enable_structural_plasticity=true
```

# Configuration Schemas

## Basic Schema Structure

Schemas define validation rules for configuration keys:

```yaml
neuron_count:
  type: int
  min: 10
  max: 100000
  required: true

region_count:
  type: int
  min: 1
  max: 1000
  required: true

simulation_timestep:
  type: float
  min: 0.0001
  max: 1.0
  required: false

enable_structural_plasticity:
  type: bool
  required: true
```

## Schema Validation Examples

### Type Validation

```yaml
neuron_count:
  type: int
  min: 1
  max: 10000

connection_probability:
  type: float
  min: 0.0
  max: 1.0
```

### Enum Validation

```yaml
neuron_count:
  type: int
  enum: [100, 500, 1000, 2000]

learning_rate:
  type: float
  enum: [0.001, 0.01, 0.1]
```

# Environment Variable Integration

## Environment Variable Prefix

All environment variables loaded from NLM use the `NLM_` prefix by default:

```bash
export NLM_NEURON_COUNT=1000
export NLM_REGION_COUNT=3
export NLM_CONNECTION_PROBABILITY=0.1
```

## Custom Prefix

Use a custom prefix for your applications:

```bash
export MY_APP_NEURON_COUNT=1000
export MY_APP_REGION_COUNT=3
```

# Configuration Templates

## Default Configuration

The default configuration provides balanced settings for general use:

```yaml
neuron_count: 1000
region_count: 3
simulation_timestep: 0.001
random_seed: 42
working_memory_capacity: 100
episodic_memory_max_episodes: 1000
enable_structural_plasticity: true
enable_development: true
enable_reward_modulation: true
enable_curiosity: true
enable_novelty_detection: false
```

## Debug Configuration

Optimized for development and debugging:

```yaml
neuron_count: 100
region_count: 2
simulation_timestep: 0.01
random_seed: 42
working_memory_capacity: 20
episodic_memory_max_episodes: 100
enable_structural_plasticity: true
enable_development: false
```

## Performance Configuration

Optimized for maximum performance:

```yaml
neuron_count: 5000
region_count: 1
simulation_timestep: 0.001
random_seed: 42
working_memory_capacity: 500
episodic_memory_max_episodes: 5000
enable_structural_plasticity: true
enable_development: true
enable_reward_modulation: true
enable_curiosity: true
```

## Creative Configuration

Optimized for exploration and creativity:

```yaml
neuron_count: 2000
region_count: 3
simulation_timestep: 0.001
random_seed: 12345
working_memory_capacity: 200
episodic_memory_max_episodes: 2000
enable_structural_plasticity: true
enable_development: true
enable_reward_modulation: false
enable_curiosity: true
enable_novelty_detection: true
```

# Examples

## Complete Workflow Example

This example demonstrates a complete workflow using the NLM Config CLI:

```bash
#!/bin/bash

# Step 1: Create a debug configuration
nlm-config template debug --output debug_config.yaml

# Step 2: Modify a parameter
nlm-config set --file debug_config.yaml neuron_count 200

# Step 3: Validate the configuration
nlm-config validate --file debug_config.yaml

# Step 4: Compare with default
nlm-config compare debug_config.yaml templates/default_config.yaml

# Step 5: Export to environment variables
nlm-config env-export --file debug_config.yaml

# Step 6: Load and validate from environment
echo "NLM_NEURON_COUNT=$NLM_NEURON_COUNT"
nlm-config env-load
```

## Configuration Management Script

A more comprehensive script for configuration management:

```bash
#!/bin/bash

# Configuration Management Script
CONFIG_DIR="configs"
BACKUP_DIR="backups"

# Create backup
create_backup() {
    if [ -f "$1" ]; then
        cp "$1" "$BACKUP_DIR/$(basename $1).$(date +%Y%m%d_%H%M%S)"
        echo "Created backup: $BACKUP_DIR/$(basename $1).$(date +%Y%m%d_%H%M%S)"
    fi
fi

# Generate from template
generate_from_template() {
    nlm-config template "$1" --output "$CONFIG_DIR/$(basename $1)"
}

# Validate all configurations
validate_all() {
    for config in "$CONFIG_DIR"/*.yaml; do
        echo "Validating $config..."
        nlm-config validate --file "$config"
    done
}

# Compare with default
compare_with_default() {
    for config in "$CONFIG_DIR"/*.yaml; do
        echo "Comparing $config with default..."
        nlm-config compare "$config" "templates/default_config.yaml"
    done
}

# Main menu
case "$1" in
    backup)
        create_backup "$2"
        ;;
    template)
        generate_from_template "$2"
        ;;
    validate-all)
        validate_all
        ;;
    compare-all)
        compare_with_default
        ;;
    *)
        echo "Usage: $0 {backup|template|validate-all|compare-all} [args]"
        exit 1
        ;;
esac
```

# Troubleshooting

## Common Issues

### "ModuleNotFoundError: No module named 'pynlm'"

**Solution:** Install the NLM Python bindings:

```bash
pip install -e python/
```

### "Error: Unsupported file format"

**Solution:** Ensure the file has a supported extension (.json, .yaml, .yml, .txt):

```bash
# Rename file or use correct extension
mv config.conf config.yaml
nlm-config list --file config.yaml
```

### "Error: Key not found in configuration"

**Solution:** Check that the key exists in the configuration:

```bash
nlm-config list --file config.yaml  # See all available keys
nlm-config get --file config.yaml existing_key
```

### Environment variable not loaded

**Solution:** Ensure environment variables use the correct prefix:

```bash
# Set with NLM_ prefix
export NLM_NEURON_COUNT=1000

# Load using nlm-config
nlm-config env-load
```

## Help and Support

For more information, see:

- `docs/config_cli.md` - This documentation
- `README.md` - Project overview
- `examples/` - Usage examples
- `templates/` - Configuration templates

# Configuration File Locations

The CLI tool looks for configuration files in the following locations (in order):

1. Current directory
2. `configs/` subdirectory
3. Environment variables
4. Command line arguments

# Version Information

- CLI Tool: v1.0.0
- Dependencies: NLM Python bindings v0.1.0
- Python: 3.8+
- License: MIT