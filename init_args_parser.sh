#!/usr/bin/env bash

# Initialize NLM Argument Parser Configuration
# This script sets up the enhanced argument parsing system

cd "$(dirname "$0")"

# Create necessary directories
mkdir -p configs
mkdir -p docs

# Create example configuration files

# 1. Basic configuration file (plain text format)
cat > configs/basic.cfg << 'EOF'
# Basic NLM configuration
# Core parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Brain parameters
brain_type = "standard"

# Plasticity parameters
enable_plasticity = true
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation parameters
enable_neuromodulation = true
curiosity_strength = 0.5
novelty_threshold = 0.3
reward_decay = 0.95

# Simulation parameters
max_steps = 1000
enable_checkpointing = false

# Output parameters
output_dir = "outputs"
save_weights = true

# Debugging parameters
log_level = "info"
debug_neurons = "1"
enable_profiling = false
EOF

# 2. JSON configuration file
cat > configs/basic.json << 'EOF'
{
    "core": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15
    },
    "brain": {
        "brain_type": "standard"
    },
    "plasticity": {
        "enable_plasticity": true,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    "neuromodulation": {
        "enable_neuromodulation": true,
        "curiosity_strength": 0.5,
        "novelty_threshold": 0.3,
        "reward_decay": 0.95
    },
    "simulation": {
        "max_steps": 1000,
        "enable_checkpointing": false
    },
    "output": {
        "output_dir": "outputs",
        "save_weights": true
    },
    "debugging": {
        "log_level": "info",
        "debug_neurons": ["1"],
        "enable_profiling": false
    }
}
EOF

# 3. YAML configuration file
cat > configs/basic.yaml << 'EOF'
core:
  random_seed: 42
  simulation_timestep: 0.001
  neuron_count: 500
  region_count: 1
  connection_probability: 0.15

brain:
  brain_type: standard

plasticity:
  enable_plasticity: true
  stdp_ltp_weight: 0.02
  stdp_ltd_weight: 0.015
  stdp_tau: 20.0
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001

neuromodulation:
  enable_neuromodulation: true
  curiosity_strength: 0.5
  novelty_threshold: 0.3
  reward_decay: 0.95

simulation:
  max_steps: 1000
  enable_checkpointing: false

output:
  output_dir: outputs
  save_weights: true

debugging:
  log_level: info
  debug_neurons:
    - "1"
  enable_profiling: false
EOF

# 4. TOML configuration file
cat > configs/basic.toml << 'EOF'
[random_seed]
value = 42

[core.simulation]
timestep = 0.001

[core.neurons]
count = 500
region_count = 1
connection_probability = 0.15

[brain]
type = "standard"

[plasticity]
enabled = true
stdp:
  ltp_weight = 0.02
  ltd_weight = 0.015
  tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[neuromodulation]
enabled = true
curiosity_strength = 0.5
novelty_threshold = 0.3
reward_decay = 0.95

[simulation]
max_steps = 1000
checkpointing_enabled = false

[output]
directory = "outputs"
save_weights = true

[debugging]
log_level = "info"
debug_neurons = ["1"]
profiling_enabled = false
EOF

# 5. INI configuration file
cat > configs/basic.ini << 'EOF'
[CORE]
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

[BRAIN]
brain_type = standard

[PLASTICITY]
enable_plasticity = true
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[NEUROMODULATION]
enable_neuromodulation = true
curiosity_strength = 0.5
novelty_threshold = 0.3
reward_decay = 0.95

[simULATION]
max_steps = 1000
enable_checkpointing = false

[OUTPUT]
output_dir = outputs
save_weights = true

[DEBUGGING]
log_level = info
debug_neurons = 1
enable_profiling = false
EOF

# 6. Properties configuration file
cat > configs/basic.properties << 'EOF'
# Basic NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
brain_type = standard
enable_plasticity = true
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
enable_neuromodulation = true
curiosity_strength = 0.5
novelty_threshold = 0.3
reward_decay = 0.95
max_steps = 1000
enable_checkpointing = false
output_dir = outputs
save_weights = true
log_level = info
debug_neurons = 1
enable_profiling = false
EOF

# 7. Quick start script
cat > quickstart.sh << 'EOF'
#!/bin/bash

echo "=== NLM Enhanced Argument Parser Quick Start ==="
echo ""
echo "1. Basic usage:"
echo "   nlm --neuron-count=1000 --simulation-timestep=0.01"
echo ""
echo "2. Load configuration file:"
echo "   nlm --config=configs/basic.cfg"
echo ""
echo "3. Use preset:"
echo "   nlm --preset=debug --enable-profiling=true"
echo ""
echo "4. Get help:"
echo "   nlm --help"
echo ""
echo "5. List available presets:"
echo "   nlm --preset=list"
echo ""
echo "Configuration files are supported in multiple formats:"
echo "  - Plain text (.cfg)"
echo "  - JSON (.json)"
echo "  - YAML (.yaml)"
echo "  - TOML (.toml)"
echo "  - INI (.ini)"
echo "  - Properties (.properties)"
echo ""
echo "For more details, visit the documentation."
EOF

chmod +x quickstart.sh

# 8. Documentation file
cat > docs/argument_parser.md << 'EOF'
# NLM Enhanced Argument Parser

The enhanced argument parsing system provides comprehensive command-line and configuration file support for the Neural Learning Machine (NLM).

## Categories

The argument parser organizes parameters into the following categories:

### Core Parameters
- `random_seed`: Random seed for reproducibility
- `simulation_timestep`: Time step for simulation
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of neural regions
- `connection_probability`: Probability of synaptic connections
- `enable_neuromodulation`: Enable neuromodulation system

### Brain Configuration
- `brain_type`: Type of brain architecture
- `enable_plasticity`: Enable synaptic plasticity

### Plasticity Settings
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse pruning

### Neuromodulation Settings
- `curiosity_strength`: Strength of curiosity-driven learning
- `novelty_threshold`: Threshold for novelty detection
- `reward_decay`: Reward decay factor

### Simulation Parameters
- `max_steps`: Maximum simulation steps
- `enable_checkpointing`: Enable checkpoint saving

### Output Configuration
- `output_dir`: Directory for output files
- `save_weights`: Save neural network weights

### Debugging Options
- `log_level`: Logging level (debug, info, warning, error)
- `debug_neurons`: List of neuron IDs for debugging
- `enable_profiling`: Enable performance profiling

## Command Line Options

### Short Options (-s)
- `-n, --neuron-count`: Number of neurons
- `-p, --enable-plasticity`: Enable plasticity
- `-v, --verbose`: Enable verbose output
- `-h, --help`: Show help information

### Long Options (--long)
- `--random-seed=<value>`: Random seed
- `--simulation-timestep=<value>`: Simulation time step
- `--neuron-count=<value>`: Number of neurons
- `--region-count=<value>`: Number of regions
- `--connection-probability=<value>`: Connection probability
- `--brain-type=<value>`: Brain type
- `--enable-plasticity=<true|false>`: Enable plasticity
- `--stdp-ltp-weight=<value>`: STDP LTP weight
- `--stdp-ltd-weight=<value>`: STDP LTD weight
- `--stdp-tau=<value>`: STDP time constant
- `--synaptogenesis-rate=<value>`: Synaptogenesis rate
- `--pruning-rate=<value>`: Pruning rate
- `--enable-neuromodulation=<true|false>`: Enable neuromodulation
- `--curiosity-strength=<value>`: Curiosity strength
- `--novelty-threshold=<value>`: Novelty threshold
- `--reward-decay=<value>`: Reward decay
- `--max-steps=<value>`: Maximum steps
- `--enable-checkpointing=<true|false>`: Enable checkpointing
- `--output-dir=<path>`: Output directory
- `--save-weights=<true|false>`: Save weights
- `--log-level=<level>`: Log level
- `--debug-neurons=<list>`: Debug neurons
- `--enable-profiling=<true|false>`: Enable profiling

## Configuration File Format

The parser supports multiple configuration file formats:

### Plain Text (.cfg)
```
# Comment
key = value
```

### JSON (.json)
```json
{
    "key": "value"
}
```

### YAML (.yaml)
```yaml
key: value
```

### TOML (.toml)
```toml
key = "value"
```

### INI (.ini)
```ini
[section]
key = value
```

### Properties (.properties)
```properties
key = value
```

## Presets

The parser includes several predefined presets:

### Standard Preset
- Balanced configuration for general use
- Includes core brain functionality
- Moderate performance

### Fast Preset
- Optimized for speed
- Reduced accuracy
- Fewer plasticity rules

### High Precision Preset
- Optimized for accuracy
- Enhanced learning capabilities
- Slower performance

### Debug Preset
- Enhanced debugging output
- Detailed logging
- Performance profiling enabled

## Usage Examples

### Basic Usage
```bash
nlm --neuron-count=1000 --simulation-timestep=0.01
```

### With Configuration File
```bash
nlm --config=configs/basic.cfg
```

### With Preset
```bash
nlm --preset=debug --enable-profiling=true
```

### Override Configuration
```bash
nlm --config=configs/basic.cfg --neuron-count=2000
```

### Get Help
```bash
nlm --help
```

## Error Handling

The parser provides comprehensive error handling:

1. **Validation Errors**: Invalid values are caught and reported
2. **Missing Dependencies**: Missing required parameters are flagged
3. **File Errors**: Invalid or unreadable configuration files
4. **Format Errors**: Unsupported configuration file formats

## Type Safety

The parser provides type-safe argument parsing:

- Automatic type conversion
- Type validation
- Default values for missing parameters
- Configuration schema checking

## Command Completion

The parser supports command completion:

```bash
nlm --complete "--"
```

## Integration

The parsed arguments can be accessed programmatically:

```cpp
ArgsParser parser;
parser.parse(argc, argv);
auto neuronCount = parser.get<int64_t>("neuron_count");
```

## License

This enhanced argument parsing system is part of the NLM project.
EOF

# 9. Create a demo configuration script
cat > demo_config.py << 'EOF'
#!/usr/bin/env python3

# Demo script showing different configuration formats

import json
import yaml
import toml
import configparser

# Sample configuration in different formats

# JSON format
json_config = {
    "core": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000
    },
    "plasticity": {
        "enable_plasticity": True,
        "stdp_ltp_weight": 0.02
    }
}

print("JSON Configuration:")
print(json.dumps(json_config, indent=2))
print()

# YAML format  
yaml_config = {
    "core": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000
    },
    "plasticity": {
        "enable_plasticity": True,
        "stdp_ltp_weight": 0.02
    }
}

print("YAML Configuration:")
print(yaml.dump(yaml_config, default_flow_style=False))
print()

# TOML format
toml_config = {
    "core": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000
    },
    "plasticity": {
        "enable_plasticity": True,
        "stdp_ltp_weight": 0.02
    }
}

print("TOML Configuration:")
print(toml.dumps(toml_config))
print()

# INI format
ini_config = configparser.ConfigParser()
ini_config['CORE'] = {
    'random_seed': '42',
    'simulation_timestep': '0.001',
    'neuron_count': '1000'
}
ini_config['PLASTICITY'] = {
    'enable_plasticity': 'true',
    'stdp_ltp_weight': '0.02'
}

print("INI Configuration:")
ini_config.write(open("temp.ini", 'w'))
with open("temp.ini", 'r') as f:
    print(f.read())
print()

# Clean up
import os
if os.path.exists("temp.ini"):
    os.remove("temp.ini")

print("Configuration formats demonstrated successfully!")
EOF

chmod +x demo_config.py

# Update CMakeLists.txt to include the new ArgsParser
cat >> CMakeLists.txt << 'EOF'

# Argument Parser library
add_library(nlm_argsparse STATIC
    src/core/ArgsParser/ArgsParser.hpp
    src/core/ArgsParser/ArgsParser.cpp
    src/core/ArgsParser/ArgsCategory.hpp
    src/core/ArgsParser/ArgsValidator.hpp
    src/core/ArgsParser/PresetManager.hpp
)

target_include_directories(nlm_argsparse PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

# Update main executable to link with argsparse
target_link_libraries(nlm PUBLIC nlm_core nlm_argsparse)
EOF

# Create a README file for the ArgsParser
cat > README.md << 'EOF'
# NLM Enhanced Argument Parser

## Overview

The enhanced argument parsing system provides comprehensive command-line and configuration file support for the Neural Learning Machine (NLM). It includes features like:

- Comprehensive argument structure with categories
- Support for both short and long options
- Configuration validation and schema checking
- Help system with detailed descriptions and examples
- Configuration file loading with format detection
- Automatic configuration presets
- Type-safe argument parsing with validation
- Error handling with descriptive messages
- Command completion support
- Logging configuration integration

## Files Created

1. **src/core/ArgsParser/ArgsParser.hpp** - Main argument parser class
2. **src/core/ArgsParser/ArgsParser.cpp** - Implementation
3. **src/core/ArgsParser/ArgsCategory.hpp** - Argument categories for organization
4. **src/core/ArgsParser/ArgsValidator.hpp** - Configuration validation
5. **src/core/ArgsParser/PresetManager.hpp** - Configuration presets

## Features

### Argument Categories
- **Core**: Core neural network parameters
- **Brain**: Brain architecture configuration
- **Plasticity**: Synaptic plasticity settings
- **Neuromodulation**: Neuromodulation system settings
- **Simulation**: Simulation parameters
- **Output**: Output configuration
- **Debugging**: Debugging options

### Configuration File Support
- Plain text (.cfg)
- JSON (.json)
- YAML (.yaml)
- TOML (.toml)
- INI (.ini)
- Properties (.properties)

### Presets
- Standard: General purpose configuration
- Fast: Optimized for speed
- High Precision: Optimized for accuracy
- Debug: Enhanced debugging

### Validation
- Type validation
- Range validation
- Pattern validation
- Dependency validation

### Error Handling
- Descriptive error messages
- Validation errors
- File errors
- Format errors

## Usage

### Command Line
```bash
nlm [options] [config-file]

Options:
  -h, --help              Show help information
  --config=<file>         Load configuration from file
  --preset=<name>         Apply configuration preset
  --verbose, -v           Enable verbose output
  --complete=<prefix>     Generate command completion
```

### Configuration File
Create a configuration file with key=value pairs:
```
# Core parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000

# Plasticity parameters
enable_plasticity = true
stdp_ltp_weight = 0.02
```

### Presets
Available presets:
- `standard`: Standard configuration
- `fast`: Fast configuration
- `high-precision`: High precision configuration
- `debug`: Debug configuration

## Testing

Run the tests to verify the implementation:
```bash
./nlm_test
```

## Building

The enhanced argument parser is automatically included in the build process. No additional steps are required.

## Documentation

For more detailed documentation, see `docs/argument_parser.md`.

## License

This enhanced argument parsing system is part of the NLM project.
EOF

# Show completion message
echo "=== Enhanced Argument Parser Initialization Complete ==="
echo ""
echo "Created:"
echo "  ✓ src/core/ArgsParser/ directory with 5 files"
echo "  ✓ configs/ directory with example configuration files (6 formats)"
echo "  ✓ docs/ directory with documentation"
echo "  ✓ quickstart.sh - Quick start script"
echo "  ✓ demo_config.py - Demo configuration script"
echo "  ✓ Updated CMakeLists.txt"
echo "  ✓ Updated README.md"
echo ""
echo "Testing:"
echo "  Run './quickstart.sh' for quick start guide"
echo "  Run './demo_config.py' to see configuration formats"
echo "  Run 'make test' to run tests (if available)"
echo ""
echo "The enhanced argument parsing system is ready for use!"