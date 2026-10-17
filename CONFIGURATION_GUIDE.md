# NLM - Configuration Guide

## Overview

NLM uses a comprehensive configuration system to control all aspects of neural simulation, from basic brain parameters to advanced learning mechanisms. This guide provides detailed information on configuration options, file formats, and best practices.

## Configuration System Architecture

### Configuration Hierarchy

NLM configuration uses a hierarchical key-value structure organized by functional categories:

```
brain
├── neuron_count
├── synapse_density
├── plasticity
│   ├── stdp
│   ├── hebbian
│   └── structural
├── memory
│   ├── working
│   ├── episodic
│   └── associative
├── neuromodulation
│   ├── dopamine
│   ├── curiosity
│   └── novelty
├── development
│   ├── initial_plasticity
│   ├── maturation_rate
│   └── critical_period
└── prediction
    ├── model_type
    ├── horizon
    └── confidence_threshold
```

### Configuration Sources

1. **Default Configuration**: Built-in defaults
2. **Configuration File**: JSON or custom format
3. **Runtime Parameters**: Via C++ or Python API
4. **Environment Variables**: System-wide settings

## Configuration File Format

### JSON Configuration

NLM configuration files use standard JSON format:

```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15,
        "v_thresh": -55.0,
        "v_rest": -65.0,
        "tau_mem": 25.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002,
            "tau_plus": 25.0,
            "tau_minus": 25.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0005
        },
        "structural": {
            "enable": true,
            "growth_rate": 0.001
        }
    },
    "memory": {
        "working": {
            "capacity": 200,
            "trace_length": 10,
            "decay_rate": 0.95
        },
        "episodic": {
            "capacity": 2000,
            "replay_interval": 100,
            "consolidation_strength": 0.8
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.2,
            "baseline": 0.1,
            "learning_modulation": true
        },
        "curiosity": {
            "enable": true,
            "sensitivity": 1.0,
            "threshold": 0.05
        },
        "novelty": {
            "enable": true,
            "detection_method": "prediction_error",
            "weight": 0.7
        }
    },
    "development": {
        "initial_plasticity": 1.5,
        "maturation_rate": 0.001,
        "critical_period": 60.0,
        "plasticity_decay": 0.9995,
        "reorganization_threshold": 0.3
    },
    "prediction": {
        "model_type": "bayesian",
        "horizon": 15,
        "confidence_threshold": 0.6,
        "error_tolerance": 0.15,
        "prediction_weight": 0.4
    }
}
```

### Alternative Configuration Formats

#### Python Config

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set values
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Save to JSON
import json
config_dict = config.to_dict()
with open("config.json", 'w') as f:
    json.dump(config_dict, f, indent=2)
```

#### Environment Variables

```bash
export NLM_NEURON_COUNT=2000
export NLM_SYNAPSE_DENSITY=0.15
export NLM_STDP_LEARNING_RATE=0.002
export NLM_ENABLE_DOPAMINE=true
```

## Configuration Categories

### Brain Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `brain.neuron_count` | int | 1000 | [10, 100000] | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | [0.01, 1.0] | Synapse connectivity probability |
| `brain.connection_probability` | float | 0.05 | [0.001, 0.5] | Direct connection probability |
| `brain.initial_weight_mean` | float | 0.5 | [-1.0, 1.0] | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | [0.01, 0.5] | Standard deviation of weights |
| `brain.v_thresh` | float | -50.0 | [-60, -40] | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | [-80, -60] | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | [-80, -70] | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | [10.0, 50.0] | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | [1.0, 10.0] | Refractory period (ms) |
| `brain.synaptic_delay_mean` | float | 5.0 | [1.0, 20.0] | Mean synaptic delay (ms) |
| `brain.synaptic_delay_std` | float | 2.0 | [0.5, 5.0] | Standard deviation of delays |

### Plasticity Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `plasticity.stdp.enable` | bool | true | - | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | [0.0001, 0.01] | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | [10.0, 50.0] | STDP potentiation time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | [10.0, 50.0] | STDP depression time constant |
| `plasticity.stdp.max_weight` | float | 1.0 | [0.1, 2.0] | Maximum synaptic weight |
| `plasticity.stdp.min_weight` | float | 0.0 | [0.0, 0.5] | Minimum synaptic weight |
| `plasticity.hebbian.enable` | bool | true | - | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.0001 | [0.00001, 0.001] | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | - | Enable structural plasticity |
| `plasticity.structural.growth_rate` | float | 0.001 | [0.0001, 0.01] | Neural growth rate |
| `plasticity.structural.pruning_rate` | float | 0.0001 | [0.00001, 0.001] | Neural pruning rate |

### Memory Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `memory.working.capacity` | int | 100 | [10, 1000] | Working memory capacity |
| `memory.working.trace_length` | int | 10 | [5, 50] | Trace length for working memory |
| `memory.working.decay_rate` | float | 0.95 | [0.8, 0.99] | Activity decay rate |
| `memory.episodic.capacity` | int | 1000 | [100, 10000] | Episodic memory capacity |
| `memory.episodic.replay_interval` | float | 100.0 | [10.0, 1000.0] | Replay interval (ms) |
| `memory.episodic.consolidation_strength` | float | 0.8 | [0.1, 1.0] | Memory consolidation strength |
| `memory.associative.pattern_count` | int | 100 | [10, 500] | Number of associative patterns |
| `memory.associative.association_strength` | float | 0.6 | [0.1, 1.0] | Association strength |

### Neuromodulation Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | [0.1, 3.0] | Dopamine modulation scale |
| `neuromod.dopamine.baseline` | float | 0.1 | [0.0, 0.5] | Baseline dopamine level |
| `neuromod.dopamine.learning_modulation` | bool | true | - | Enable dopamine learning modulation |
| `neuromod.curiosity.enable` | bool | true | - | Enable curiosity-driven exploration |
| `neuromod.curiosity.sensitivity` | float | 1.0 | [0.1, 5.0] | Curiosity sensitivity |
| `neuromod.curiosity.threshold` | float | 0.05 | [0.01, 0.2] | Curiosity activation threshold |
| `neuromod.novelty.enable` | bool | true | - | Enable novelty detection |
| `neuromod.novelty.detection_method` | string | "prediction_error" | "prediction_error", "variance" | Novelty detection method |
| `neuromod.novelty.weight` | float | 0.7 | [0.1, 1.0] | Novelty influence weight |

### Development Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `development.initial_plasticity` | float | 1.0 | [0.5, 3.0] | Initial plasticity rate |
| `development.maturation_rate` | float | 0.001 | [0.0001, 0.01] | Maturation rate |
| `development.critical_period` | float | 50.0 | [10.0, 200.0] | Critical period duration |
| `development.plasticity_decay` | float | 0.9995 | [0.99, 0.9999] | Plasticity decay rate per timestep |
| `development.reorganization_threshold` | float | 0.3 | [0.1, 0.5] | Threshold for structural changes |
| `development.synaptic_pruning` | bool | true | - | Enable synaptic pruning |
| `development.excitatory_inhibitory_balance` | float | 0.8 | [0.5, 1.5] | Target E/I balance |

### Prediction Configuration

| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| `prediction.model_type` | string | "bayesian" | "bayesian", "kalman", "adaptive" | Prediction model type |
| `prediction.horizon` | int | 10 | [1, 50] | Prediction horizon |
| `prediction.confidence_threshold` | float | 0.5 | [0.1, 1.0] | Confidence threshold |
| `prediction.error_tolerance` | float | 0.1 | [0.05, 0.5] | Error tolerance |
| `prediction.prediction_weight` | float | 0.3 | [0.1, 0.8] | Weight of prediction in decision making |
| `prediction.adaptive_learning` | bool | true | - | Enable adaptive prediction learning |
| `prediction.memory_integration` | bool | true | - | Integrate prediction with memory |

## Configuration Validation

### Parameter Validation Rules

1. **Type Validation**: Ensure parameters have correct types
2. **Range Validation**: Check values are within acceptable ranges
3. **Constraint Validation**: Verify relationships between parameters
4. **Dependency Validation**: Check required parameters for features

### Validation Examples

#### Python API

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration
validation_errors = config.validate()

if validation_errors:
    for error in validation_errors:
        print(f"ERROR: {error}")
else:
    print("Configuration is valid!")

# Example of validation
print(config.get("brain.neuron_count"))  # Access valid parameter
print(config.has("brain.neuron_count"))  # Check existence
```

#### C++ API

```cpp
#include "core/Config/Config.hpp"

nlm::Config config;

// Set values
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);

// Validate
std::vector<std::string> errors = config.validate();

if (!errors.empty()) {
    for (const auto& error : errors) {
        std::cerr << "ERROR: " << error << std::endl;
    }
}
```

### Common Configuration Issues

#### Issue 1: Invalid Neuron Count
**Problem**: Setting neuron_count to 0 or negative value
**Solution**: Ensure neuron_count is positive and within range

```python
config.set("brain.neuron_count", max(10, user_input))
```

#### Issue 2: Conflicting Plasticity Settings
**Problem**: STDP and Hebbian disabled while structural plasticity enabled
**Solution**: Enable at least one plasticity mechanism

```python
if not (config.get("plasticity.stdp.enable") or 
        config.get("plasticity.hebbian.enable") or
        config.get("plasticity.structural.enable")):
    config.set("plasticity.stdp.enable", True)
```

#### Issue 3: Memory Capacity Issues
**Problem**: Setting memory capacity too high for system resources
**Solution**: Calculate based on available memory

```python
import psutil
available_memory = psutil.virtual_memory().available
max_neurons = available_memory // (100 * 4)  # 100 bytes per neuron
config.set("brain.neuron_count", max_neurons)
```

## Configuration Optimization

### Performance Optimization

#### Memory-Optimized Configuration
```json
{
    "brain": {
        "neuron_count": 500,
        "synapse_density": 0.05
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
```

#### Speed-Optimized Configuration
```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0005
        }
    },
    "development": {
        "initial_plasticity": 0.8
    }
}
```

### Configuration Templates

#### Research Template
```json
{
    "brain": {"neuron_count": 5000, "synapse_density": 0.15},
    "plasticity": {
        "stdp": {"enable": true, "learning_rate": 0.002},
        "hebbian": {"enable": true, "learning_rate": 0.001},
        "structural": {"enable": true, "growth_rate": 0.002}
    },
    "memory": {
        "working": {"capacity": 500, "decay_rate": 0.9},
        "episodic": {"capacity": 5000, "consolidation_strength": 0.9}
    },
    "neuromodulation": {
        "dopamine": {"scale": 1.5, "learning_modulation": true},
        "curiosity": {"enable": true, "sensitivity": 1.5}
    }
}
```

#### Development Template
```json
{
    "brain": {"neuron_count": 100, "synapse_density": 0.05},
    "plasticity": {
        "stdp": {"enable": true, "learning_rate": 0.001},
        "hebbian": {"enable": true, "learning_rate": 0.0001}
    },
    "development": {
        "initial_plasticity": 2.0,
        "maturation_rate": 0.01,
        "critical_period": 100.0
    }
}
```

#### Production Template
```json
{
    "brain": {"neuron_count": 1000, "synapse_density": 0.1},
    "plasticity": {
        "stdp": {"enable": true, "learning_rate": 0.001}
    },
    "development": {
        "initial_plasticity": 1.0,
        "maturation_rate": 0.001
    }
}
```

## Configuration Management

### Configuration Lifecycle

1. **Creation**: Create initial configuration
2. **Validation**: Validate configuration parameters
3. **Testing**: Test configuration with simulation
4. **Deployment**: Use configuration in production
5. **Monitoring**: Monitor configuration effectiveness
6. **Optimization**: Optimize configuration based on performance

### Configuration Backup and Recovery

```python
import pynlm
import json
import os
from datetime import datetime

def save_configuration(config, path="backup_config.json"):
    """Save configuration with timestamp."""
    backup_data = {
        "timestamp": datetime.now().isoformat(),
        "config": config.to_dict()
    }
    
    with open(path, 'w') as f:
        json.dump(backup_data, f, indent=2)
    
    print(f"Configuration saved to {path}")

def load_configuration(path="backup_config.json"):
    """Load configuration from backup."""
    if not os.path.exists(path):
        print(f"Backup file {path} not found!")
        return None
    
    with open(path, 'r') as f:
        backup_data = json.load(f)
    
    config = pynlm.createConfig(backup_data["config"])
    print(f"Configuration loaded from {path}")
    return config
```

### Configuration Comparison

```python
def compare_configurations(config1, config2):
    """Compare two configurations and show differences."""
    dict1 = config1.to_dict()
    dict2 = config2.to_dict()
    
    differences = []
    
    # Recursive comparison
    def compare_dicts(d1, d2, path=""):
        for key in set(d1.keys()) | set(d2.keys()):
            current_path = f"{path}.{key}" if path else key
            
            if key in d1 and key in d2:
                if isinstance(d1[key], dict) and isinstance(d2[key], dict):
                    compare_dicts(d1[key], d2[key], current_path)
                elif d1[key] != d2[key]:
                    differences.append(f"{current_path}: {d1[key]} -> {d2[key]}")
            elif key in d1:
                differences.append(f"{current_path}: present -> removed")
            else:
                differences.append(f"{current_path}: removed -> present")
    
    compare_dicts(dict1, dict2)
    
    if differences:
        print("Configuration differences:")
        for diff in differences:
            print(f"  {diff}")
    else:
        print("Configurations are identical")
```

## Configuration Best Practices

### 1. Use Descriptive Parameter Names
```json
{
    "brain": {
        "cognitive_neuron_count": 500,
        "sensory_synapse_density": 0.1
    }
}
```

### 2. Document Configuration Choices
```json
{
    "brain": {
        "neuron_count": 1000,
        "comment": "Sufficient for basic cognitive tasks"
    }
}
```

### 3. Version Control Configuration
```json
{
    "version": "1.0.0",
    "brain": {
        "neuron_count": 1000,
        "configuration_changes": [
            "Initial setup",
            "Added plasticity mechanisms"
        ]
    }
}
```

### 4. Environment-Specific Configuration
```json
{
    "environment": "development",
    "brain": {
        "neuron_count": 100,
        "debug_mode": true
    }
}
```

## Configuration Tools

### Configuration Editor
```python
import pynlm
from rich.console import Console
from rich.table import Table

def display_config_table(config):
    """Display configuration in a formatted table."""
    console = Console()
    
    # Create table for brain configuration
    brain_table = Table(title="Brain Configuration")
    brain_table.add_column("Parameter", style="cyan")
    brain_table.add_column("Value", style="magenta")
    
    brain_params = ["neuron_count", "synapse_density", "v_thresh", "v_rest"]
    for param in brain_params:
        if config.has(param):
            brain_table.add_row(param, str(config.get(param)))
    
    console.print(brain_table)

# Usage
config = pynlm.createDefaultConfig()
display_config_table(config)
```

### Configuration Validator
```python
import pynlm
import argparse

def validate_config_file(file_path):
    """Validate configuration file."""
    try:
        # Load configuration
        config = pynlm.createConfigFromFile(file_path)
        
        # Validate
        errors = config.validate()
        
        if errors:
            print(f"Validation failed for {file_path}:")
            for error in errors:
                print(f"  - {error}")
            return False
        else:
            print(f"Configuration {file_path} is valid!")
            return True
            
    except Exception as e:
        print(f"Error loading {file_path}: {e}")
        return False

# Command line usage
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Validate NLM configuration")
    parser.add_argument("config_file", help="Path to configuration file")
    
    args = parser.parse_args()
    validate_config_file(args.config_file)
```

## Conclusion

The configuration system in NLM provides comprehensive control over brain simulation parameters, enabling users to customize behavior for specific research needs, performance requirements, and experimental conditions. By following best practices for configuration management, users can ensure reproducible results, optimal performance, and maintainable code.

Key takeaways:

1. **Understand the hierarchy** of configuration parameters
2. **Validate configurations** before running simulations
3. **Use templates** for common use cases
4. **Document configuration choices** for reproducibility
5. **Test configurations** in development before deployment

The configuration system is a powerful tool that enables the flexibility and extensibility of the NLM framework while maintaining stability and performance.

---

*This configuration guide provides comprehensive information about the NLM configuration system.*
*For API reference, see API_REFERENCE.md.*
*For development practices, see DEVELOPMENT_GUIDE.md.*

**Last updated**: 2024-01-17
**Version**: 1.0.0