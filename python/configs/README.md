# NLM Configuration and Environment Setup

This directory contains configuration files and environment setup for the NLM Python bindings.

## Overview

Configuration management provides a flexible, Pythonic way to configure NLM simulations,
experiments, and runtime behavior. It supports multiple configuration formats,
validation, and environment-specific settings.

## Configuration Structure

### Configuration Files

`configs/` contains configuration files for different use cases:

- **`default.cfg`**: Default configuration for basic simulations
- **`phase6_default.json`**: Configuration for Phase 6 integration tests
- **`experiment_templates/`**: Templates for common experiments
- **`benchmark/`**: Configuration for performance benchmarking

### Environment Configuration

`.env` files for development and production environments:

- **`.env.dev`**: Development environment settings
- **`.env.prod`**: Production environment settings
- **`.env.test`**: Testing environment settings

## Configuration Management

### Configuration Classes

#### Base Configuration (`Config`)

The base configuration class provides:

- **Key-value storage**: Generic configuration storage
- **Type safety**: Type checking and validation
- **Default values**: Built-in default configurations
- **File I/O**: Save and load configurations to/from files
- **CLI integration**: Command-line argument parsing

**Example Usage**:

```python
from pynlm import Config

# Create configuration
config = Config()

# Set values
config.set("neuron_count", 1000)
config.set("connection_probability", 0.15)
config.set("stdp_ltp_weight", 0.01)

# Get values
neuron_count = config.get("neuron_count")
prob = config.get("connection_probability")

# Get with defaults
region_count = config.get("region_count", 1)
```

#### Configuration Validation

Configuration validation ensures configurations are valid:

```python
from pynlm import Config

config = Config()

# Add validation rules
config.add_validator("neuron_count", lambda x: x > 0, "Neuron count must be positive")
config.add_validator("connection_probability", lambda x: 0.0 <= x <= 1.0,
                     "Connection probability must be between 0 and 1")

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration is invalid:", config.get_errors())
```

#### Configuration Sources

Configurations can be loaded from multiple sources:

```python
from pynlm import Config

config = Config()

# Load from JSON file
config.load_from_json("config.json")

# Load from .cfg file
config.load_from_cfg("config.cfg")

# Load from Python dict
config_dict = {"neuron_count": 100, "region_count": 1}
config.load_from_dict(config_dict)

# Override with command-line arguments
import sys
config.load_from_args(sys.argv)
```

### Advanced Configuration Features

#### Configuration Inheritance

Configurations can inherit from parent configurations:

```python
from pynlm import Config

# Create base configuration
base_config = Config()
base_config.set("neuron_count", 100)
base_config.set("connection_probability", 0.15)

# Create child configuration that inherits from base
child_config = Config(base_config)

# Child can override specific values
child_config.set("neuron_count", 200)  # Override base
```

#### Configuration Templates

Configuration templates provide predefined configurations:

```python
from pynlm import ConfigTemplates

# Get learning experiment template
learning_config = ConfigTemplates.learning_experiment()
learning_config.set("neuron_count", 500)

# Get developmental experiment template
dev_config = ConfigTemplates.development_experiment()
dev_config.set("enable_development", True)
```

#### Configuration Comparison

Compare different configurations:

```python
from pynlm import Config

config1 = Config()
config1.set("neuron_count", 100)
config1.set("connection_probability", 0.15)

config2 = Config()
config2.set("neuron_count", 200)
config2.set("connection_probability", 0.2)

# Compare configurations
if config1.equals(config2):
    print("Configurations are identical")
else:
    print("Configurations differ")

# Get differences
differences = config1.get_differences(config2)
for key, (val1, val2) in differences.items():
    print(f"{key}: {val1} -> {val2}")
```

## Configuration Files

### Default Configuration Files

#### `default.cfg`

Basic configuration for standard simulations:

```ini
# Default NLM Configuration
# This file contains default settings for NLM simulations

# Neural Network Configuration
neuron_count = 100
region_count = 1
connection_probability = 0.15

# Plasticity Configuration
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Structural Plasticity Configuration
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Development Configuration
enable_development = false
development_rate = 0.001

# Simulation Configuration
simulation_timestep = 0.001
max_steps = 10000
enable_checkpointing = false
enable_replay = false

# Neuromodulation Configuration
dopamine_learning_rate = 0.01
curiosity_threshold = 0.5
novelty_sensitivity = 0.1

# Memory Configuration
working_memory_capacity = 100
episodic_memory_capacity = 1000
```

#### `phase6_default.json`

Configuration for Phase 6 integration tests:

```json
{
    "name": "Phase 6 Default Configuration",
    "description": "Configuration for Phase 6 final integration tests",
    "version": "1.0.0",
    
    "neural": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.1
    },
    
    "plasticity": {
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    
    "development": {
        "enable_development": true,
        "development_rate": 0.001,
        "maturation_rate": 0.0005
    },
    
    "neuromodulation": {
        "dopamine_learning_rate": 0.01,
        "curiosity_threshold": 0.5,
        "novelty_sensitivity": 0.1
    },
    
    "memory": {
        "working_memory_capacity": 100,
        "episodic_memory_capacity": 1000
    },
    
    "simulation": {
        "simulation_timestep": 0.001,
        "max_steps": 2000,
        "enable_checkpointing": true,
        "enable_replay": true
    },
    
    "experiment": {
        "enable_plasticity_experiment": true,
        "enable_stdp_verification": true,
        "enable_development_experiment": true
    }
}
```

### Experiment Templates

#### Learning Experiment Template

`configs/experiment_templates/learning_experiment.json`:

```json
{
    "name": "Learning Experiment",
    "description": "Template for learning experiments with plasticity",
    "template_type": "learning",
    
    "neural": {
        "neuron_count": 200,
        "region_count": 1,
        "connection_probability": 0.2
    },
    
    "plasticity": {
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "enable_hebbian": true,
        "enable_stdp": true,
        "enable_structural": false
    },
    
    "neuromodulation": {
        "dopamine_learning_rate": 0.02,
        "curiosity_threshold": 0.3
    },
    
    "simulation": {
        "max_steps": 5000,
        "enable_checkpointing": true
    },
    
    "objectives": [
        "demonstrate_synaptic_plasticity",
        "measure_learning_performance",
        "test_memory_consolidation"
    ]
}
```

#### Development Experiment Template

`configs/experiment_templates/development_experiment.json`:

```json
{
    "name": "Development Experiment",
    "description": "Template for developmental experiments",
    "template_type": "development",
    
    "neural": {
        "neuron_count": 300,
        "region_count": 1,
        "connection_probability": 0.1
    },
    
    "development": {
        "enable_development": true,
        "development_rate": 0.001,
        "maturation_rate": 0.0005,
        "synaptogenesis_enabled": true,
        "pruning_enabled": true
    },
    
    "plasticity": {
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.01
    },
    
    "simulation": {
        "max_steps": 8000,
        "enable_replay": true
    },
    
    "objectives": [
        "study_developmental_plasticity",
        "observe_neural_maturation",
        "test_memory_developmental_effects"
    ]
}
```

#### Neuromodulation Experiment Template

`configs/experiment_templates/neuromodulation_experiment.json`:

```json
{
    "name": "Neuromodulation Experiment",
    "description": "Template for neuromodulation experiments",
    "template_type": "neuromodulation",
    
    "neural": {
        "neuron_count": 150,
        "region_count": 1,
        "connection_probability": 0.25
    },
    
    "neuromodulation": {
        "dopamine_learning_rate": 0.03,
        "curiosity_threshold": 0.2,
        "novelty_sensitivity": 0.15,
        "curiosity_decay": 0.01,
        "dopamine_decay": 0.02
    },
    
    "plasticity": {
        "stdp_ltp_weight": 0.025,
        "stdp_ltd_weight": 0.02
    },
    
    "simulation": {
        "max_steps": 3000,
        "enable_checkpointing": true,
        "enable_replay": true
    },
    
    "objectives": [
        "study_reward_learning",
        "investigate_curiosity_driven_exploration",
        "analyze_novelty_detection"
    ]
}
```

### Benchmark Configuration

`configs/benchmark/` directory contains configurations for performance benchmarking:

- `small_scale.cfg`: Configuration for small-scale testing (100 neurons)
- `medium_scale.cfg`: Configuration for medium-scale testing (1000 neurons)
- `large_scale.cfg`: Configuration for large-scale testing (10000 neurons)
- `extreme_scale.cfg`: Configuration for extreme-scale testing (100000 neurons)

## Environment Configuration

### Development Environment (`.env.dev`)

```bash
# Development environment settings

# Python configuration
PYTHONPATH=/workspace/app/python
PYTHONUNBUFFERED=1

# NLM configuration
NLM_DEBUG=1
NLM_LOG_LEVEL=debug
NLM_ENABLE_PROFILING=1

# Testing configuration
NLM_TEST_COVERAGE=1
NLM_TEST_VERBOSE=1

# Simulation configuration
NLM_SIMULATION_TIMESTEP=0.001
NLM_SIMULATION_MAX_STEPS=1000

# File paths
NLM_CONFIG_PATH=configs/
NLM_DATA_PATH=data/
NLM_LOG_PATH=logs/
```

### Production Environment (`.env.prod`)

```bash
# Production environment settings

# Python configuration
PYTHONPATH=/workspace/app/python

# NLM configuration
NLM_DEBUG=0
NLM_LOG_LEVEL=info
NLM_ENABLE_PROFILING=0

# Testing configuration
NLM_TEST_COVERAGE=0
NLM_TEST_VERBOSE=0

# Performance configuration
NLM_SIMULATION_TIMESTEP=0.0005
NLM_SIMULATION_MAX_STEPS=5000

# File paths
NLM_CONFIG_PATH=/etc/nlm/configs/
NLM_DATA_PATH=/var/lib/nlm/data/
NLM_LOG_PATH=/var/log/nlm/
```

### Testing Environment (`.env.test`)

```bash
# Testing environment settings

# Python configuration
PYTHONPATH=/workspace/app/python
PYTHONUNBUFFERED=1

# NLM configuration
NLM_DEBUG=1
NLM_LOG_LEVEL=info

# Testing configuration
NLM_TEST_COVERAGE=1
NLM_TEST_VERBOSE=1
NLM_TEST_BENCHMARK_MODE=1

# Simulation configuration
NLM_SIMULATION_TIMESTEP=0.001
NLM_SIMULATION_MAX_STEPS=100

# File paths
NLM_CONFIG_PATH=configs/
NLM_DATA_PATH=test_data/
NLM_LOG_PATH=test_logs/
```

## Configuration Loading

### Priority Order

Configurations are loaded in the following priority order (later sources override earlier ones):

1. **Default Configuration**: Built-in defaults
2. **Configuration File**: Specified in `configs/` directory
3. **Environment Variables**: Set in `.env` files
4. **Command Line Arguments**: Arguments passed to the program
5. **Runtime Overrides**: Programmatic overrides

### Loading Process

```python
from pynlm import Config

# Create configuration
config = Config()

# Load configuration in priority order
config.load_defaults()                    # Built-in defaults
config.load_from_file("configs/default.cfg") # File configuration
config.load_from_env()                    # Environment variables
config.load_from_args()                   # Command line arguments
```

### Configuration Validation

Configuration validation ensures configurations are valid:

```python
from pynlm import Config

config = Config()
config.load_from_file("configs/experiment.json")

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Validation errors:", config.get_errors())
    for error in config.get_errors():
        print(f"  - {error}")
```

## Configuration Usage Examples

### Basic Usage

```python
from pynlm import Config, Brain

# Create configuration from file
config = Config.load_from_file("configs/experiment.json")

# Create brain with configuration
brain = Brain(config)
brain.initialize()

# Run simulation
for step in range(config.get("max_steps", 1000)):
    brain.step(step, config.get("simulation_timestep", 0.001))
```

### Configuration Comparison

```python
from pynlm import Config

# Load two configurations
config1 = Config.load_from_file("configs/experiment1.json")
config2 = Config.load_from_file("configs/experiment2.json")

# Compare configurations
if config1.equals(config2):
    print("Configurations are identical")
else:
    print("Differences:")
    for key in set(config1.keys()) | set(config2.keys()):
        if config1.get(key) != config2.get(key):
            print(f"  {key}: {config1.get(key)} -> {config2.get(key)}")
```

### Configuration Modification

```python
from pynlm import Config

# Load configuration
config = Config.load_from_file("configs/experiment.json")

# Modify configuration
config.set("neuron_count", 500)
config.set("max_steps", 2000)

# Save configuration
config.save_to_file("modified_experiment.json")
```

### Configuration Templating

```python
from pynlm import ConfigTemplates

# Create configuration from template
config = ConfigTemplates.learning_experiment()

# Modify configuration
config.set("neuron_count", 1000)
config.set("enable_checkpointing", True)

# Save configuration
config.save_to_file("my_learning_experiment.json")
```

## Configuration API Reference

### Config Class

#### Methods

- `load_from_file(filename)`: Load configuration from file
- `load_from_json(filename)`: Load configuration from JSON file
- `load_from_cfg(filename)`: Load configuration from .cfg file
- `load_from_dict(dictionary)`: Load configuration from dictionary
- `load_from_env()`: Load configuration from environment variables
- `load_from_args(args=None)`: Load configuration from command line arguments
- `save_to_file(filename)`: Save configuration to file
- `save_to_json(filename)`: Save configuration to JSON file
- `save_to_cfg(filename)`: Save configuration to .cfg file
- `validate()`: Validate configuration
- `get_errors()`: Get validation errors
- `add_validator(key, validator, message)`: Add validation rule
- `set_default(key, value)`: Set default value
- `get(key, default=None)`: Get value with default
- `get_int(key, default=0)`: Get integer value
- `get_float(key, default=0.0)`: Get float value
- `get_bool(key, default=False)`: Get boolean value
- `get_string(key, default="")`: Get string value
- `has_key(key)`: Check if key exists
- `keys()`: Get all keys
- `values()`: Get all values
- `items()`: Get all key-value pairs
- `equals(other_config)`: Compare with another configuration
- `get_differences(other_config)`: Get differences with another configuration

#### Properties

- `is_valid`: Whether configuration is valid
- `error_count`: Number of validation errors

### ConfigTemplates Module

#### Templates

- `learning_experiment()`: Learning experiment template
- `development_experiment()`: Development experiment template
- `neuromodulation_experiment()`: Neuromodulation experiment template
- `phase6_integration()`: Phase 6 integration template
- `benchmark_template(scale)`: Benchmark template for specified scale

#### Template Methods

- `get_template(name)`: Get template by name
- `list_templates()`: List all available templates
- `create_from_template(name)`: Create configuration from template

### Configuration Validators

Built-in validators:

- `positive_int(value)`: Value must be positive integer
- `non_negative_int(value)`: Value must be non-negative integer
- `float_range(min, max)`: Value must be within range
- `probability(value)`: Value must be between 0 and 1
- `neuron_count(value)`: Valid neuron count
- `region_count(value)`: Valid region count
- `connection_probability(value)`: Valid connection probability

## Configuration Troubleshooting

### Common Issues

1. **File Not Found**: Ensure configuration file exists
2. **Invalid Format**: Check file format matches expected type
3. **Validation Errors**: Check configuration values against validation rules
4. **Missing Dependencies**: Ensure all required dependencies are installed

### Solutions

1. **Check File Existence**:

```python
import os

if not os.path.exists("configs/experiment.json"):
    print("Configuration file not found")
```

2. **Check File Format**:

```python
import json

try:
    with open("configs/experiment.json", "r") as f:
        config = json.load(f)
except json.JSONDecodeError as e:
    print(f"Invalid JSON format: {e}")
```

3. **Check Configuration Values**:

```python
from pynlm import Config

config = Config.load_from_file("configs/experiment.json")
if not config.validate():
    print("Configuration validation errors:")
    for error in config.get_errors():
        print(f"  - {error}")
```

## Configuration Best Practices

### 1. Use Descriptive Names

```python
# Good
config.set("learning_rate", 0.01)

# Bad
config.set("param1", 0.01)
```

### 2. Document Configuration

```python
config.add_comment("Neural network parameters", "neuron_count", "region_count")
config.add_comment("Learning parameters", "stdp_ltp_weight", "stdp_ltd_weight")
```

### 3. Use Constants for Magic Numbers

```python
from pynlm import Config

# Define constants
DEFAULT_NEURON_COUNT = 100
DEFAULT_CONNECTION_PROBABILITY = 0.15

# Use constants
config.set("neuron_count", DEFAULT_NEURON_COUNT)
config.set("connection_probability", DEFAULT_CONNECTION_PROBABILITY)
```

### 4. Validate Configuration Early

```python
from pynlm import Config

config = Config.load_from_file("configs/experiment.json")

# Validate configuration before use
if not config.validate():
    raise ValueError(f"Invalid configuration: {config.get_errors()}")
```

### 5. Use Configuration Files for Different Environments

```python
# Different configurations for different environments
if os.environ.get("NLM_ENVIRONMENT") == "production":
    config = Config.load_from_file("configs/production.json")
elif os.environ.get("NLM_ENVIRONMENT") == "testing":
    config = Config.load_from_file("configs/testing.json")
else:
    config = Config.load_from_file("configs/development.json")
```

---
*This configuration management system provides a flexible, powerful way to manage NLM configurations across different environments and use cases.*
