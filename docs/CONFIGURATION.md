# NLM Configuration Guide

## Overview

This document provides comprehensive guidance for configuring the NLM (熙然) neural system, including configuration file formats, command-line options, and runtime configuration.

## Configuration File Format

NLM uses JSON format for configuration files. Configuration values support multiple types including primitives and collections.

### Basic JSON Example

```json
{
  "nlm": {
    "version": "0.1.0",
    "simulation": {
      "max_steps": 1000,
      "dt": 0.001,
      "time_unit": "ms"
    },
    "brain": {
      "neuron_count": 100,
      "region_count": 5,
      "plasticity_enabled": true
    },
    "memory": {
      "working_memory_capacity": 50,
      "episodic_memory_enabled": true,
      "association_threshold": 0.5
    },
    "sensory": {
      "vision_enabled": true,
      "audio_enabled": true,
      "feature_dimensions": 64
    },
    "motor": {
      "action_types": ["move", "grasp", "rotate"],
      "exploration_rate": 0.1
    },
    "neuromodulation": {
      "dopamine_enabled": true,
      "curiosity_enabled": true,
      "novelty_enabled": true,
      "prediction_error_enabled": true
    },
    "development": {
      "stages": 3,
      "plasticity_decay_rate": 0.95
    },
    "logging": {
      "level": "info",
      "file": "nlm.log",
      "max_size_mb": 100
    }
  }
}
```

## Configuration Structure

### Top-Level Keys

#### `nlm` (object)
The root configuration object containing all NLM settings.

#### `simulation` (object)
Controls simulation parameters and timing.

##### Simulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `max_steps` | integer | 1000 | Maximum number of simulation steps |
| `dt` | number | 0.001 | Time step duration (seconds) |
| `time_unit` | string | "ms" | Time unit for display and output |
| `random_seed` | integer | -1 | Fixed seed for reproducible experiments |

#### `brain` (object)
Configuration for the central neural system.

##### Brain Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | integer | 100 | Total number of neurons |
| `region_count` | integer | 5 | Number of brain regions |
| `plasticity_enabled` | boolean | true | Enable plasticity mechanisms |
| `development_enabled` | boolean | true | Enable developmental stages |
| `prediction_enabled` | boolean | true | Enable prediction system |

#### `memory` (object)
Configuration for memory systems.

##### Working Memory Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | integer | 50 | Maximum number of active memory traces |
| `working_memory_decay_rate` | number | 0.01 | Decay rate per timestep |
| `working_memory_reinforcement_factor` | number | 1.1 | Strength of memory strengthening |

##### Episodic Memory Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `episodic_memory_enabled` | boolean | true | Enable episodic memory system |
| `episodic_memory_capacity` | integer | 1000 | Maximum episodic memories |
| `episodic_memory_consolidation_interval` | integer | 100 | Steps between consolidation cycles |

##### Associative Memory Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `association_threshold` | number | 0.5 | Activation threshold for associations |
| `association_decay_rate` | number | 0.05 | Decay rate for associations |

#### `sensory` (object)
Configuration for sensory processing systems.

##### Vision Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `vision_enabled` | boolean | true | Enable vision processing |
| `vision_sensitivity` | number | 1.0 | Sensitivity to visual input |
| `vision_noise_level` | number | 0.01 | Level of visual input noise |

##### Audio Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `audio_enabled` | boolean | true | Enable audio processing |
| `audio_sensitivity` | number | 1.0 | Sensitivity to audio input |
| `audio_frequency_range` | array | [20, 20000] | Hz range for audio processing |

##### Feature Processing

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `feature_dimensions` | integer | 64 | Dimensionality of feature vectors |
| `feature_extraction_method` | string | "autoencoder" | Method for feature extraction |

#### `motor` (object)
Configuration for motor control and action selection.

##### Action Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `action_types` | array | ["move", "grasp", "rotate"] | Available action types |
| `action_noise_level` | number | 0.01 | Level of action output noise |
| `exploration_rate` | number | 0.1 | Probability of exploratory actions |

##### Selection Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `selection_method` | string | "softmax" | Action selection algorithm |
| `selection_temperature` | number | 1.0 | Temperature for stochastic selection |
| `coherence_threshold` | number | 0.5 | Minimum neural coherence for action selection |

#### `neuromodulation` (object)
Configuration for neuromodulatory systems.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_enabled` | boolean | true | Enable dopamine (reward) system |
| `curiosity_enabled` | boolean | true | Enable curiosity (exploration) system |
| `novelty_enabled` | boolean | true | Enable novelty detection |
| `prediction_error_enabled` | boolean | true | Enable prediction error signals |
| `dopamine_sensitivity` | number | 1.0 | Dopamine effect magnitude |

#### `development` (object)
Configuration for developmental processes.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `stages` | integer | 3 | Number of developmental stages |
| `plasticity_decay_rate` | number | 0.95 | Rate at which plasticity changes over time |
| `excitability_change_rate` | number | 0.05 | Rate of neural excitability changes |

#### `logging` (object)
Configuration for logging and output.

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `level` | string | "info" | Logging verbosity level |
| `file` | string | "nlm.log" | Log file path |
| `max_size_mb` | integer | 100 | Maximum log file size |
| `flush_interval_ms` | integer | 1000 | Log flush interval |

## Command Line Configuration

NLM can be configured via command line arguments. The following options are supported:

### Basic Options

```bash
nlm [options]

Options:
  --help                    Show help message
  --version                 Show version information
  --config FILE             Load configuration from file
  --log-level LEVEL         Set logging level (debug, info, warning, error)
  --log-file FILE           Set log file path
  --max-steps N             Set maximum simulation steps
  --seed N                  Set random seed
  --device DEVICE           Set computation device (cpu, gpu)
  --threads N               Set number of threads
```

### Advanced CLI Options

```bash
# Simulation parameters
--simulation.max_steps N              Maximum simulation steps
--simulation.dt VALUE                  Time step duration
--simulation.time_unit UNIT            Time unit (ms, s)

# Brain parameters
--brain.neuron_count N                Total neurons
--brain.region_count N                 Number of regions
--brain.plasticity_enabled FLAG        Enable plasticity

# Memory parameters
--memory.working_memory_capacity N     Working memory capacity
--memory.episodic_memory_enabled FLAG  Enable episodic memory
--memory.association_threshold VALUE   Association threshold

# Neuromodulation parameters
--neuromodulation.dopamine_enabled FLAG   Enable dopamine
--neuromodulation.curiosity_enabled FLAG  Enable curiosity
--neuromodulation.novelty_enabled FLAG    Enable novelty

# Development parameters
--development.stages N                 Number of developmental stages
--development.plasticity_decay_rate VALUE

# Sensory parameters
--sensory.vision_enabled FLAG          Enable vision
--sensory.audio_enabled FLAG           Enable audio
--sensory.feature_dimensions N          Feature vector dimensionality

# Motor parameters
--motor.action_types TYPES             Comma-separated action types
--motor.exploration_rate VALUE         Exploration probability
```

### Example Command Lines

```bash
# Run with default configuration
./nlm

# Run with custom configuration file
./nlm --config my_config.json

# Run with command line overrides
./nlm --max-steps 5000 --seed 42 --log-level debug

# Run with specific settings for debugging
./nlm --simulation.max_steps 100 \
      --brain.plasticity_enabled=true \
      --memory.working_memory_capacity 200 \
      --neuromodulation.dopamine_enabled=true \
      --neuromodulation.curiosity_enabled=false
```

## Runtime Configuration

### Programmatic Configuration

In C++ code, configuration can be set programmatically:

```cpp
#include <nlm/core/Config.hpp>

// Create configuration
auto config = std::make_shared<nlm::Config>();

// Set basic values
config->set("nlm.simulation.max_steps", 5000);
config->set("nlm.brain.plasticity_enabled", true);
config->set("nlm.memory.working_memory_capacity", 200);

// Set complex values
config->set("nlm.sensory.feature_dimensions", std::vector<int>{128});
config->set("nlm.motor.action_types", std::vector<std::string>{"move", "grasp", "rotate"});

// Load from command line
config->loadFromArgs(argc, argv);
```

### Python Configuration

In Python, use the NLM bindings:

```python
import nlm

# Create configuration
config = nlm.Config()

# Set values
config.set("nlm.simulation.max_steps", 5000)
config.set("nlm.brain.plasticity_enabled", True)
config.set("nlm.memory.working_memory_capacity", 200)

# Load from file
config.load_from_file("config.json")

# Load from command line
config.load_from_args()
```

## Configuration Validation

### Type Validation

The configuration system validates that values match expected types:

```json
{
  "nlm": {
    "simulation": {
      "max_steps": 5000,        // Valid: integer
      "dt": 0.001,              // Valid: number
      "time_unit": "ms"         // Valid: string
    },
    "brain": {
      "plasticity_enabled": true,  // Valid: boolean
      "neuron_count": "100"        // Invalid: string instead of integer
    }
  }
}
```

### Value Validation

NLM validates configuration values for logical consistency:

```json
{
  "nlm": {
    "simulation": {
      "max_steps": -100,        // Invalid: negative steps
      "dt": -0.001,             // Invalid: negative time step
      "time_unit": "invalid"    // Invalid: unknown time unit
    },
    "memory": {
      "working_memory_capacity": -10,  // Invalid: negative capacity
      "working_memory_decay_rate": 2.0  // Invalid: decay rate > 1.0
    }
  }
}
```

### Warnings

Non-critical configuration issues generate warnings:

```bash
WARNING: Configuration value 'nlm.brain.neuron_count' is missing, using default (100)
WARNING: Configuration value 'nlm.sensory.feature_dimensions' is deprecated, use 'nlm.sensory.feature_extraction.feature_dimensions' instead
WARNING: Configuration value 'nlm.neuromodulation.curiosity_enabled' is set to false, but curiosity is required for Phase 4+ experiments
```

## Configuration Templates

### Template: Simple Learning

```json
{
  "nlm": {
    "simulation": {
      "max_steps": 1000,
      "dt": 0.001
    },
    "brain": {
      "neuron_count": 200,
      "region_count": 3,
      "plasticity_enabled": true
    },
    "memory": {
      "working_memory_capacity": 50,
      "episodic_memory_enabled": true
    },
    "neuromodulation": {
      "dopamine_enabled": true,
      "curiosity_enabled": true
    }
  }
}
```

### Template: High-Performance

```json
{
  "nlm": {
    "simulation": {
      "max_steps": 10000,
      "dt": 0.0005,
      "time_unit": "ms"
    },
    "brain": {
      "neuron_count": 1000,
      "region_count": 10,
      "plasticity_enabled": true
    },
    "memory": {
      "working_memory_capacity": 500,
      "episodic_memory_capacity": 5000,
      "episodic_memory_enabled": true
    },
    "sensory": {
      "feature_dimensions": 256,
      "vision_enabled": true,
      "audio_enabled": true
    },
    "motor": {
      "exploration_rate": 0.05,
      "coherence_threshold": 0.7
    },
    "neuromodulation": {
      "dopamine_enabled": true,
      "curiosity_enabled": true,
      "novelty_enabled": true,
      "prediction_error_enabled": true
    },
    "development": {
      "stages": 5,
      "plasticity_decay_rate": 0.9
    },
    "logging": {
      "level": "warning",
      "max_size_mb": 500
    }
  }
}
```

### Template: Research Experiment

```json
{
  "nlm": {
    "simulation": {
      "max_steps": 5000,
      "dt": 0.001,
      "random_seed": 12345
    },
    "brain": {
      "neuron_count": 500,
      "region_count": 8,
      "plasticity_enabled": true
    },
    "memory": {
      "working_memory_capacity": 100,
      "working_memory_decay_rate": 0.02,
      "working_memory_reinforcement_factor": 1.2,
      "episodic_memory_enabled": true,
      "episodic_memory_capacity": 2000,
      "episodic_memory_consolidation_interval": 200,
      "association_threshold": 0.3
    },
    "sensory": {
      "vision_enabled": true,
      "vision_sensitivity": 0.8,
      "audio_enabled": true,
      "feature_dimensions": 128,
      "feature_extraction_method": "autoencoder"
    },
    "motor": {
      "action_types": ["move", "grasp", "rotate", "push", "pull"],
      "exploration_rate": 0.15,
      "selection_method": "softmax",
      "selection_temperature": 2.0
    },
    "neuromodulation": {
      "dopamine_enabled": true,
      "curiosity_enabled": true,
      "novelty_enabled": true,
      "prediction_error_enabled": true,
      "dopamine_sensitivity": 0.8
    },
    "development": {
      "stages": 4,
      "plasticity_decay_rate": 0.97,
      "excitability_change_rate": 0.03
    },
    "logging": {
      "level": "info",
      "file": "experiments/experiment_001/nlm.log",
      "max_size_mb": 200
    }
  }
}
```

## Configuration Debugging

### Environment Variables

Set environment variables to control configuration:

```bash
export NLM_CONFIG_FILE=config.json
export NLM_LOG_LEVEL=debug
export NLM_RANDOM_SEED=12345
export NLM_DEVICE=cpu
```

### Debug Mode

Enable debug mode for detailed configuration information:

```bash
./nlm --config debug.json --log-level debug
```

### Configuration Inspector

Use the configuration inspector to validate and display current configuration:

```bash
# Display current configuration
./nlm --config my_config.json --inspect

# Validate configuration
./nlm --config my_config.json --validate

# Show configuration differences
./nlm --config base.json --compare new_config.json
```

## Configuration Best Practices

### 1. Use Configuration Files for Most Settings

Command line arguments should be used for:
- Experiment-specific variations
- Debugging overrides
- Development testing

Use configuration files for:
- Production settings
- Experiment parameters
- Research configurations

### 2. Validate Configuration Before Use

Always validate configuration after loading:

```cpp
if (!config->validate()) {
    std::cerr << "Configuration validation failed!" << std::endl;
    return 1;
}
```

### 3. Use Descriptive Configuration Names

Use clear, descriptive names for configuration keys:

```json
{
  "nlm": {
    "simulation": {
      "max_simulation_steps": 1000,  // More descriptive
      "time_step_duration_ms": 1.0    // Clear units
    }
  }
}
```

### 4. Document Configuration Usage

Add documentation for custom configuration sections:

```json
{
  "nlm": {
    "my_custom_feature": {
      "enabled": true,
      "parameter1": 1.0,
      "parameter2": "value",
      "description": "Custom feature for experiment X"
    }
  }
}
```

### 5. Test Configuration Changes

Test configuration changes thoroughly, especially for:
- Large parameter changes
- Disabled critical components
- Extreme value ranges
- Complex nested configurations

## Configuration Migration

### From Version 0.0.x to 0.1.0

Configuration structure changes between versions:

**Old Keys:**
- `max_steps`
- `neuron_count`
- `plasticity`

**New Keys:**
- `nlm.simulation.max_steps`
- `nlm.brain.neuron_count`
- `nlm.brain.plasticity_enabled`

Migration script:

```bash
#!/bin/bash
# migrate_config.sh

python3 << EOF
import json
import sys

def migrate_config(input_file, output_file):
    with open(input_file, 'r') as f:
        config = json.load(f)
    
    # Migrate from old structure
    if 'max_steps' in config:
        if 'nlm' not in config:
            config['nlm'] = {}
        if 'simulation' not in config['nlm']:
            config['nlm']['simulation'] = {}
        config['nlm']['simulation']['max_steps'] = config.pop('max_steps')
    
    if 'neuron_count' in config:
        if 'nlm' not in config:
            config['nlm'] = {}
        if 'brain' not in config['nlm']:
            config['nlm']['brain'] = {}
        config['nlm']['brain']['neuron_count'] = config.pop('neuron_count')
    
    with open(output_file, 'w') as f:
        json.dump(config, f, indent=2)

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Migrate NLM configuration')
    parser.add_argument('input', help='Input configuration file')
    parser.add_argument('output', help='Output configuration file')
    args = parser.parse_args()
    
    migrate_config(args.input, args.output)
EOF
python3 migrate_config.sh old_config.json new_config.json
```

## Conclusion

Configuration is a critical aspect of NLM operation. Proper configuration ensures:

- **Correct Simulation Behavior:** Parameters match experimental requirements
- **System Performance:** Appropriate resource allocation
- **Reproducible Experiments:** Fixed random seeds and parameters
- **Debuggability:** Clear logging and error reporting
- **Extensibility:** Easy addition of new components

Use this guide as a foundation for configuring NLM for your specific needs and research requirements.
