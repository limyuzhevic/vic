# ===== CONFIGURATION SCHEMA DOCUMENTATION =====

# NLM Brain Configuration Schema

## Overview
This document defines the configuration schema for NLM Phase 6: Integrated Artificial Brain. The configuration system supports multiple file formats (JSON, YAML, and simple key=value) and provides comprehensive validation, type checking, and default values for all brain parameters.

## Configuration Architecture

The NLM configuration system is organized into several logical sections:

### Core Simulation Parameters
- **neuron_count**: Number of neurons in the brain
- **region_count**: Number of neural regions  
- **simulation_timestep**: Integration timestep in seconds
- **connection_probability**: Probability of synaptic connections

### Plasticity System Parameters
- **stdp_ltp_weight**: STDP Long-Term Potentiation weight
- **stdp_ltd_weight**: STDP Long-Term Depression weight
- **stdp_tau**: STDP time constant (milliseconds)
- **plasticity_learning_rate**: General plasticity learning rate

### Neuromodulation Parameters
- **dopamine_baseline**: Baseline dopamine level
- **curiosity_threshold**: Novelty threshold for exploration
- **novelty_decay_rate**: Rate of novelty decay
- **reward_discount_factor**: Reward prediction error discount

### Development System Parameters
- **synaptogenesis_rate**: Rate of new synapse formation
- **pruning_rate**: Rate of synapse elimination
- **critical_period_duration**: Duration of developmental critical period

### Memory System Parameters
- **working_memory_decay**: Decay rate for working memory
- **episodic_memory_capacity**: Maximum episodic memory entries
- **replay_interval**: Memory replay frequency
- **consolidation_threshold**: Memory consolidation threshold

### Simulation Control Parameters
- **max_simulation_steps**: Maximum simulation steps
- **simulation_time_limit**: Absolute time limit (seconds)
- **checkpoint_interval**: Checkpoint save frequency
- **log_level**: Logging verbosity

## Configuration File Formats

### JSON Configuration
```json
{
  "neuron_count": 10000,
  "region_count": 10,
  "simulation_timestep": 0.001,
  "connection_probability": 0.1,
  "stdp": {
    "ltp_weight": 0.01,
    "ltd_weight": 0.012,
    "tau": 20.0
  },
  "neuromodulation": {
    "dopamine": {
      "baseline": 0.0,
      "plasticity_factor": 1.0
    },
    "curiosity": {
      "threshold": 0.5,
      "decay_rate": 0.95
    }
  },
  "development": {
    "synaptogenesis_rate": 0.001,
    "pruning_rate": 0.0001,
    "critical_period_duration": 100
  },
  "memory": {
    "working_memory": {
      "decay_rate": 0.99,
      "capacity": 1000
    },
    "episodic": {
      "capacity": 1000,
      "replay_interval": 100,
      "consolidation_threshold": 0.3
    }
  },
  "simulation": {
    "max_steps": 10000,
    "time_limit": 0.0,
    "checkpoint_interval": 100,
    "log_level": "INFO"
  }
}
```

### YAML Configuration
```yaml
neuron_count: 10000
region_count: 10
simulation_timestep: 0.001
connection_probability: 0.1

stdp:
  ltp_weight: 0.01
  ltd_weight: 0.012
  tau: 20.0

neuromodulation:
  dopamine:
    baseline: 0.0
    plasticity_factor: 1.0
  curiosity:
    threshold: 0.5
    decay_rate: 0.95
  novelty:
    decay_rate: 0.95

 development:
   synaptogenesis_rate: 0.001
   pruning_rate: 0.0001
   critical_period_duration: 100

memory:
  working_memory:
    decay_rate: 0.99
    capacity: 1000
  episodic:
    capacity: 1000
    replay_interval: 100
    consolidation_threshold: 0.3

simulation:
  max_steps: 10000
  time_limit: 0.0
  checkpoint_interval: 100
  log_level: "INFO"
```

### Simple Key=Value Configuration
```
# NLM Configuration File
# Default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.0
curiosity_threshold = 0.5
novelty_decay_rate = 0.95

# Reward settings
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
```

## Configuration Validation

### Required Parameters
The following parameters must be set for valid configuration:

| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| `neuron_count` | Integer | [10, 1,000,000] | 1000 | Total number of neurons |
| `region_count` | Integer | [1, 100] | 1 | Number of neural regions |
| `simulation_timestep` | Float | [0.0001, 1.0] | 0.001 | Integration timestep |
| `connection_probability` | Float | [0.0, 1.0] | 0.1 | Synaptic connection probability |

### Optional Parameters with Defaults

#### Plasticity System
| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| `stdp_ltp_weight` | Float | [0.0, 1.0] | 0.01 | STDP potentiation weight |
| `stdp_ltd_weight` | Float | [0.0, 1.0] | 0.012 | STDP depression weight |
| `stdp_tau` | Float | [1.0, 100.0] | 20.0 | STDP time constant (ms) |
| `plasticity_learning_rate` | Float | [0.0, 1.0] | 0.01 | General learning rate |

#### Neuromodulation System
| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| `dopamine_baseline` | Float | [0.0, 2.0] | 0.0 | Baseline dopamine level |
| `curiosity_threshold` | Float | [0.0, 1.0] | 0.5 | Novelty detection threshold |
| `novelty_decay_rate` | Float | [0.0, 1.0] | 0.95 | Novelty decay rate |
| `reward_discount_factor` | Float | [0.0, 1.0] | 0.99 | Reward discount factor |

#### Development System
| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| `synaptogenesis_rate` | Float | [0.0, 0.1] | 0.001 | New synapse formation rate |
| `pruning_rate` | Float | [0.0, 0.01] | 0.0001 | Synapse elimination rate |
| `critical_period_duration` | Integer | [0, 1000] | 100 | Critical period duration (steps) |

#### Memory System
| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| `working_memory_decay` | Float | [0.0, 1.0] | 0.99 | Working memory decay rate |
| `episodic_memory_capacity` | Integer | [100, 10000] | 1000 | Maximum episodic entries |
| `replay_interval` | Integer | [10, 1000] | 100 | Memory replay frequency |
| `consolidation_threshold` | Float | [0.0, 1.0] | 0.3 | Memory consolidation threshold |

#### Simulation Control
| Parameter | Type | Range | Default | Description |
|-----------|------|-------|---------|-------------|
| `max_simulation_steps` | Integer | [100, 1000000] | 10000 | Maximum simulation steps |
| `simulation_time_limit` | Float | [0.0, 1000.0] | 0.0 | Absolute time limit |
| `checkpoint_interval` | Integer | [10, 10000] | 100 | Checkpoint save frequency |
| `log_level` | String | ["DEBUG", "INFO", "WARN", "ERROR"] | "INFO" | Logging verbosity |

## Schema Validation Examples

### Valid Configuration
```json
{
  "neuron_count": 5000,
  "region_count": 5,
  "simulation_timestep": 0.0005,
  "connection_probability": 0.15,
  "stdp_ltp_weight": 0.015,
  "stdp_ltd_weight": 0.018,
  "curiosity_threshold": 0.3,
  "reward_discount_factor": 0.95
}
```

### Invalid Configuration (missing required parameters)
```json
{
  "neuron_count": 5000,
  "connection_probability": 0.15
}
```

### Configuration with Invalid Values
```json
{
  "neuron_count": 5000,
  "region_count": 10,
  "simulation_timestep": -0.001,
  "connection_probability": 1.5
}
```

## Programmatic Configuration API

### C++ Configuration Usage
```cpp
#include "Config.hpp"

// Create default configuration
Config config;

// Load from JSON
config.loadFromFile("brain_config.json");

// Set values programmatically
config.set("neuron_count", 10000);
config.set("region_count", 10);
config.set("simulation_timestep", 0.001f);
config.set("connection_probability", 0.1f);
config.set("curiosity_threshold", 0.5f);

// Get values with type conversion
int neurons = config.getOr<int>("neuron_count", 1000);
float timestep = config.getOr<float>("simulation_timestep", 0.001f);
bool curiosity = config.getOr<bool>("enable_curiosity", true);

// Check if parameter exists
if (config.has("stdp_ltp_weight")) {
    float ltp_weight = config.getOr<float>("stdp_ltp_weight", 0.01f);
}

// Save configuration
config.saveToFile("output_config.json");
```

### Python Configuration Usage
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set values
config.set("brain.neuron_count", 10000)
config.set("brain.region_count", 10)
config.set("brain.simulation_timestep", 0.001)
config.set("brain.connection_probability", 0.1)

# Load from file
config.loadFromFile("brain_config.json")

# Save to file
config.saveToFile("my_config.json")

# Get values
neurons = config.getOr<int>("brain.neuron_count", 1000)
```

## Configuration Environment Variables

NLM supports configuration via environment variables for command-line tools:

```bash
export NLM_NEURON_COUNT=10000
export NLM_REGION_COUNT=10
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_CONNECTION_PROBABILITY=0.1
export NLM_LOG_LEVEL=INFO
```

## Configuration Command Line Interface

### Command Line Arguments
```bash
# Basic usage
./nlm --neuron_count=10000 --region_count=10 --simulation_timestep=0.001

# With key=value pairs
./nlm -n 10000 -r 10 -t 0.001 -p 0.1

# From configuration file
./nlm --config_file=neuron_config.json

# Override specific parameters
./nlm --neuron_count=5000 --enable_curiosity

# List available parameters
./nlm --help
```

### Configuration Examples

#### Small Experimental Brain
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "simulation_timestep": 0.001,
  "connection_probability": 0.05,
  "curiosity_threshold": 0.7
}
```

#### Large Scale Brain
```json
{
  "neuron_count": 100000,
  "region_count": 20,
  "simulation_timestep": 0.0001,
  "connection_probability": 0.2,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.024,
  "critical_period_duration": 200,
  "synaptogenesis_rate": 0.002,
  "pruning_rate": 0.0002
}
```

#### Development Configuration
```json
{
  "neuron_count": 10000,
  "region_count": 5,
  "simulation_timestep": 0.0005,
  "connection_probability": 0.15,
  "curiosity_threshold": 0.3,
  "reward_discount_factor": 0.9,
  "critical_period_duration": 150
}
```

## Configuration Best Practices

### 1. Start with Conservative Values
- Begin with smaller `neuron_count` for testing
- Use larger timesteps initially for faster simulation
- Set high `connection_probability` for exploratory behaviors

### 2. Validate Configuration
- Always validate configuration before running simulations
- Check for missing required parameters
- Verify parameter ranges and types

### 3. Use Meaningful Parameter Names
- Use descriptive parameter names
- Document parameter units and expected ranges
- Group related parameters logically

### 4. Save Configuration After Testing
- Save successful configurations to files
- Version control configuration files
- Create configuration templates for different use cases

### 5. Monitor Configuration Changes
- Track parameter changes across simulation runs
- Log configuration at start of each simulation
- Use configuration versioning for reproducibility

## Troubleshooting Common Issues

### Configuration File Not Found
```bash
# Solution: Ensure file exists at correct path
./nlm --config_file=./configs/brain_config.json
```

### Invalid JSON/YAML Syntax
```bash
# Solution: Validate configuration file format
python -m json.tool brain_config.json
# or
python -c "import yaml; yaml.safe_load(open('brain_config.yaml'))"
```

### Missing Required Parameters
```bash
# Solution: Add missing parameters
./nlm --neuron_count=1000 --region_count=1 --simulation_timestep=0.001
```

### Parameter Type Mismatch
```bash
# Solution: Ensure correct parameter types
./nlm --neuron_count=1000 --connection_probability=0.1
```

## Configuration Migration Guide

### From Phase 1 to Phase 2
1. **File Format**: Update from `.cfg` to `.json` or `.yaml`
2. **Parameter Names**: Use camelCase for JSON/YAML
3. **New Parameters**: Add Phase 2-specific parameters
4. **Validation**: Enable schema validation

### Example Migration
```yaml
# Phase 1 (old format)
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Phase 2 (new format)
neuron_count: 1000
region_count: 2
simulation_timestep: 0.001
connection_probability: 0.1
curiosity:
  threshold: 0.5
  decay_rate: 0.95
```

## Configuration Validation Script

```python
#!/usr/bin/env python3
"""
NLM Configuration Validation Script

Validates NLM configuration files for syntax and semantic correctness.
"""

import json
import yaml
import argparse
import sys
from pathlib import Path

def validate_json_config(filepath):
    """Validate JSON configuration file."""
    try:
        with open(filepath, 'r') as f:
            config = json.load(f)
        
        # Check required parameters
        required = ['neuron_count', 'region_count', 'simulation_timestep']
        for param in required:
            if param not in config:
                print(f"ERROR: Missing required parameter: {param}")
                return False
        
        # Validate parameter ranges
        if config['neuron_count'] < 10 or config['neuron_count'] > 1000000:
            print(f"ERROR: Invalid neuron_count: {config['neuron_count']}")
            return False
        
        if config['region_count'] < 1 or config['region_count'] > 100:
            print(f"ERROR: Invalid region_count: {config['region_count']}")
            return False
        
        if config['simulation_timestep'] < 0.0001 or config['simulation_timestep'] > 1.0:
            print(f"ERROR: Invalid simulation_timestep: {config['simulation_timestep']}")
            return False
        
        print(f"SUCCESS: Valid JSON configuration with {len(config)} parameters")
        return True
        
    except json.JSONDecodeError as e:
        print(f"ERROR: Invalid JSON syntax: {e}")
        return False
    except Exception as e:
        print(f"ERROR: Validation failed: {e}")
        return False

def validate_yaml_config(filepath):
    """Validate YAML configuration file."""
    try:
        with open(filepath, 'r') as f:
            config = yaml.safe_load(f)
        
        # Check required parameters
        required = ['neuron_count', 'region_count', 'simulation_timestep']
        for param in required:
            if param not in config:
                print(f"ERROR: Missing required parameter: {param}")
                return False
        
        # Validate parameter ranges (same as JSON)
        if config['neuron_count'] < 10 or config['neuron_count'] > 1000000:
            print(f"ERROR: Invalid neuron_count: {config['neuron_count']}")
            return False
        
        if config['region_count'] < 1 or config['region_count'] > 100:
            print(f"ERROR: Invalid region_count: {config['region_count']}")
            return False
        
        if config['simulation_timestep'] < 0.0001 or config['simulation_timestep'] > 1.0:
            print(f"ERROR: Invalid simulation_timestep: {config['simulation_timestep']}")
            return False
        
        print(f"SUCCESS: Valid YAML configuration with {len(config)} parameters")
        return True
        
    except yaml.YAMLError as e:
        print(f"ERROR: Invalid YAML syntax: {e}")
        return False
    except Exception as e:
        print(f"ERROR: Validation failed: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(description='Validate NLM configuration files')
    parser.add_argument('filepath', help='Path to configuration file')
    
    args = parser.parse_args()
    filepath = Path(args.filepath)
    
    if not filepath.exists():
        print(f"ERROR: File not found: {filepath}")
        sys.exit(1)
    
    # Determine file type and validate
    if filepath.suffix.lower() == '.json':
        success = validate_json_config(filepath)
    elif filepath.suffix.lower() in ['.yaml', '.yml']:
        success = validate_yaml_config(filepath)
    else:
        print(f"ERROR: Unsupported file type: {filepath.suffix}")
        print("Supported formats: .json, .yaml, .yml")
        sys.exit(1)
    
    sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()
```

## Conclusion

This configuration schema documentation provides comprehensive guidance for configuring NLM brain simulations. It covers:

- **File formats**: JSON, YAML, and legacy key=value formats
- **Parameter validation**: Type checking and range validation
- **Usage examples**: Code examples for different programming languages
- **Best practices**: Guidelines for effective configuration management
- **Troubleshooting**: Common issues and solutions
- **Migration**: Guides for upgrading between NLM versions

The configuration system enables flexible, reproducible, and well-documented brain simulation setups while maintaining backward compatibility with existing Phase 1 configurations.