# NLM Configuration System
# ==============================

# This document describes the configuration system for NLM (Neural Learning Machine)
# Phase 6 - Final Integration

# Overview
The NLM configuration system provides flexible ways to configure brain simulations,
experiment parameters, and system behavior. It supports multiple formats:

- **C++ Config Class**: Native C++ configuration system
- **JSON Configuration**: Machine-readable configuration files
- **YAML Configuration**: Human-readable configuration files
- **Command Line**: Runtime configuration via CLI arguments

# Configuration Formats

## C++ Config Class (src/core/Config/Config.hpp)

The native configuration system uses key-value pairs with type safety:

```cpp
#include "core/Config/Config.hpp"

// Create configuration
auto config = std::make_shared<Config>();

// Set configuration values
config->set("brain.neuron_count", 1000);
config->set("plasticity.stdp.learning_rate", 0.001);
config->set("neuromod.dopamine.scale", 1.5f);

// Get configuration values
int neurons = config->getOr<int>("brain.neuron_count", 500);
float stdpRate = config->getOr<float>("plasticity.stdp.learning_rate", 0.001f);
```

Configuration supports the following data types:
- Integers (int64_t)
- Floating point numbers (float)
- Strings (std::string)
- Booleans (bool)

### Configuration Categories

#### Brain Configuration
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Plasticity Configuration
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for LTP (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for LTD (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Rate of synapse elimination |

#### Neuromodulation Configuration
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale factor |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.sensitivity` | float | 1.0 | Prediction error sensitivity |

#### Development Configuration
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `development.stage` | string | "Initial" | Developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging) |
| `development.plasticity_modulation` | float | 1.0 | Plasticity rate modulation factor |
| `development.synapse_replacement_rate` | float | 0.00001 | Rate of synapse replacement |

#### Memory Configuration
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `memory.working.capacity` | float | 0.1 | Working memory capacity as fraction of neuron count |
| `memory.episodic.max_episodes` | int | 1000 | Maximum number of episodic memory episodes |
| `memory.associative.pattern_count` | int | 100 | Number of patterns in associative memory |
| `memory.consolidation.interval` | int | 1000 | Steps between memory consolidation |

#### Experiment Configuration
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `experiment.max_steps` | int | 10000 | Maximum number of simulation steps |
| `experiment.checkpoint_interval` | int | 1000 | Steps between checkpoints |
| `experiment.enable_replay` | bool | true | Enable memory replay |
| `experiment.enable_development` | bool | true | Enable developmental processes |

# JSON Configuration

## Format
JSON configuration files use a hierarchical structure with the following format:

```json
{
  "nlm_version": "0.1.0",
  "phase": "6",
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "connection_probability": 0.05
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true
    }
  },
  "experiment": {
    "max_steps": 10000,
    "enable_replay": true,
    "enable_development": true
  }
}
```

## File Extensions
- `.nlm.json` - Standard NLM configuration
- `.nlm.cfg` - Compact configuration
- `.brain.json` - Brain-specific configuration

## Example JSON Configuration

### Simple Configuration
```json
{
  "nlm_version": "0.1.0",
  "description": "Simple exploration simulation",
  "brain": {
    "neuron_count": 500,
    "connection_probability": 0.15
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.01
    }
  },
  "experiment": {
    "max_steps": 2000,
    "enable_development": true
  }
}
```

### Advanced Configuration
```json
{
  "nlm_version": "0.1.0",
  "description": "Complex developmental simulation",
  "brain": {
    "neuron_count": 2000,
    "region_count": 3,
    "connection_probability": 0.08,
    "initial_weight_mean": 0.6,
    "initial_weight_std": 0.15
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.005,
      "tau_plus": 25.0,
      "tau_minus": 30.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.003
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0002,
      "pruning_rate": 0.00002
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 1.2
    },
    "curiosity": {
      "enable": true,
      "exploration_rate": 0.3
    },
    "novelty": {
      "enable": true,
      "sensitivity": 1.5
    },
    "prediction_error": {
      "sensitivity": 1.0
    }
  },
  "development": {
    "stage": "CriticalPeriod",
    "plasticity_modulation": 0.8,
    "synapse_replacement_rate": 0.00003
  },
  "memory": {
    "working": {
      "capacity": 0.15
    },
    "episodic": {
      "max_episodes": 5000,
      "consolidation_interval": 500
    },
    "associative": {
      "pattern_count": 200
    }
  },
  "experiment": {
    "max_steps": 50000,
    "checkpoint_interval": 5000,
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true,
    "replay_interval": 200,
    "consolidation_interval": 2000
  },
  "logging": {
    "level": "Info",
    "file": "nlm_simulation.log",
    "console": true
  }
}
```

# YAML Configuration

## Format
YAML configuration files provide a more human-readable format:

```yaml
---
nlm_version: "0.1.0"
phase: "6"
description: "Advanced developmental brain simulation"

brain:
  neuron_count: 1000
  synapse_density: 0.1
  connection_probability: 0.05
  initial_weight_mean: 0.5
  initial_weight_std: 0.1

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.003
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
    exploration_rate: 0.3
  novelty:
    enable: true
    sensitivity: 1.5
  prediction_error:
    sensitivity: 1.0

development:
  stage: "CriticalPeriod"
  plasticity_modulation: 0.8
  synapse_replacement_rate: 0.00003

memory:
  working:
    capacity: 0.1
  episodic:
    max_episodes: 1000
    consolidation_interval: 1000
  associative:
    pattern_count: 100

experiment:
  max_steps: 10000
  checkpoint_interval: 1000
  enable_checkpointing: true
  enable_replay: true
  enable_development: true
  replay_interval: 100
  consolidation_interval: 1000

logging:
  level: "Info"
  file: "nlm_simulation.log"
  console: true
```

## Advantages of YAML
- More readable and intuitive
- Supports complex nested structures
- Easier to edit manually
- Good support for comments
- Native support in many tools

# Configuration Loading and Saving

## Loading Configuration

### From File
```cpp
#include "core/Config/Config.hpp"
#include <fstream>

bool loadConfig(const std::string& filename, std::shared_ptr<Config>& config) {
    config = std::make_shared<Config>();
    
    // Try different formats based on file extension
    if (filename.substr(filename.find_last_of(".") + 1) == "json") {
        return config->loadFromJSON(filename);
    } else if (filename.substr(filename.find_last_of(".") + 1) == "yaml" || 
               filename.substr(filename.find_last_of(".") + 1) == "yml") {
        return config->loadFromYAML(filename);
    } else {
        // Try native config file format
        return config->loadFromFile(filename);
    }
}
```

### From Command Line
```bash
nlm run --config config.json --neurons 1000 --steps 5000
```

## Saving Configuration

### To File
```cpp
bool saveConfig(const std::string& filename, const std::shared_ptr<Config>& config) {
    // Determine format based on file extension
    if (filename.substr(filename.find_last_of(".") + 1) == "json") {
        return config->saveToJSON(filename);
    } else if (filename.substr(filename.find_last_of(".") + 1) == "yaml" || 
               filename.substr(filename.find_last_of(".") + 1) == "yml") {
        return config->saveToYAML(filename);
    } else {
        return config->saveToFile(filename);
    }
}
```

# Configuration Validation

## Required Fields
- `brain.neuron_count` - Must be positive integer
- `brain.connection_probability` - Must be between 0.0 and 1.0

## Validation Rules
- `neuron_count` > 0
- `connection_probability` ∈ [0.0, 1.0]
- `max_steps` > 0
- Developmental stage must be valid

## Example Validation
```cpp
bool validateConfig(const std::shared_ptr<Config>& config) {
    bool valid = true;
    
    // Check brain configuration
    auto neuronCount = config->get<int>("brain.neuron_count");
    if (!neuronCount || *neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid brain.neuron_count: must be positive");
        valid = false;
    }
    
    auto connProb = config->get<float>("brain.connection_probability");
    if (!connProb || *connProb < 0.0f || *connProb > 1.0f) {
        NLM_LOG_ERROR("Invalid brain.connection_probability: must be between 0.0 and 1.0");
        valid = false;
    }
    
    // Check other configurations...
    
    return valid;
}
```

# Configuration Templates

## Template Files
Configuration templates are available for common use cases:

### Template: Simple Exploration
```json
{
  "nlm_version": "0.1.0",
  "description": "Simple exploration without development",
  "brain": {
    "neuron_count": 500,
    "connection_probability": 0.1
  },
  "plasticity": {
    "stdp": { "enable": true, "learning_rate": 0.01 }
  },
  "experiment": {
    "max_steps": 2000,
    "enable_development": false
  }
}
```

### Template: Developmental Learning
```json
{
  "nlm_version": "0.1.0",
  "description": "Full developmental learning simulation",
  "brain": {
    "neuron_count": 1000,
    "region_count": 2
  },
  "development": {
    "stage": "CriticalPeriod",
    "plasticity_modulation": 0.8
  },
  "memory": {
    "episodic": { "max_episodes": 5000 }
  },
  "experiment": {
    "max_steps": 50000,
    "enable_development": true,
    "enable_replay": true
  }
}
```

# Configuration CLI Commands

The NLM CLI provides commands for configuration management:

## Available Commands

### `nlm config show`
Display current configuration

### `nlm config load <file>`
Load configuration from file

### `nlm config save <file>`
Save configuration to file

### `nlm config validate <file>`
Validate configuration file

### `nlm config list-templates`
List available configuration templates

## Example Usage

```bash
# Show current configuration
nlm config show

# Load a JSON configuration
nlm config load experiment_config.json

# Save current configuration
nlm config save my_config.json

# Validate configuration
nlm config validate config.json

# List templates
nlm config list-templates
```

# Configuration Best Practices

## File Organization
- Store configuration files in a `configs/` directory
- Use descriptive filenames
- Keep configuration files in version control

## Environment Variables
Environment variables can override configuration:

```bash
export NLM_NEURON_COUNT=2000
export NLM_SIMULATION_STEPS=10000
export NLM_RANDOM_SEED=42
```

## Debugging
Enable debug logging by setting the log level:

```bash
nlm run --debug
```

# Summary

The NLM configuration system provides comprehensive support for:

- **Multiple formats**: C++, JSON, YAML
- **Hierarchical structure**: Nested configuration parameters
- **Validation**: Automatic configuration validation
- **Templates**: Pre-configured templates for common use cases
- **CLI integration**: Command-line configuration management
- **Environment variables**: Runtime configuration overrides

This system enables users to easily configure and customize NLM simulations for research,
experiments, and development purposes while maintaining consistency and reliability.
