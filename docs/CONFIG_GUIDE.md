# NLM Configuration System

## Overview

The NLM (Neural Learning Machine) Configuration system provides a flexible and powerful way to configure neural simulation parameters. It supports multiple file formats (JSON, YAML, and simple key=value), command-line arguments, and runtime configuration.

## Configuration Files

### JSON Format (.json)
```json
{
    "name": "NLM Simulation",
    "description": "A neural simulation with multiple regions",
    "parameters": {
        "neuron_count": 1000,
        "region_count": 4,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001
    }
}
```

### YAML Format (.yml, .yaml)
```yaml
name: NLM Simulation
description: A neural simulation with multiple regions
parameters:
  neuron_count: 1000
  region_count: 4
  connection_probability: 0.1
  simulation_timestep: 0.001
```

### Simple Format (.cfg)
```
# NLM Configuration File
# neuron count for the simulation
neuron_count = 1000

# number of brain regions
region_count = 4

# probability of connection between neurons
connection_probability = 0.1

# simulation time step in seconds
simulation_timestep = 0.001
```

## Configuration Parameters

### Core Parameters
- **neuron_count**: Total number of neurons in the simulation
- **region_count**: Number of neural regions
- **connection_probability**: Probability of connection between neurons
- **simulation_timestep**: Time step for simulation (in seconds)
- **random_seed**: Random seed for reproducible results

### STDP Parameters
- **stdp_ltp_weight**: Long-term potentiation weight
- **stdp_ltd_weight**: Long-term depression weight
- **stdp_tau**: STDP time constant (in ms)

### Plasticity Parameters
- **synaptogenesis_rate**: Rate of new synapse formation
- **pruning_rate**: Rate of synapse elimination

### Neuromodulation Parameters
- **enable_reward_modulation**: Enable reward-based learning
- **enable_curiosity**: Enable curiosity-driven exploration
- **enable_development**: Enable developmental changes
- **enable_structural_plasticity**: Enable structural plasticity

## Configuration Loading

The configuration system loads from multiple sources with the following priority:

1. **Command-line arguments** (highest priority)
2. **JSON/YAML configuration files**
3. **Simple configuration files**
4. **Default values** (lowest priority)

## API Usage

### Python API
```python
from nlm import Config, createBrain

# Create configuration from file
config = Config()
config.loadFromFile("config.json")

# Create brain with configuration
brain = createBrain(config)
brain.initialize()

# Override configuration values
brain.getConfig().set("random_seed", 42)

# Save configuration
brain.getConfig().saveToFile("output_config.json")
```

### C++ API
```cpp
#include <nlm/Config.h>

auto config = std::make_shared<nlm::Config>();
config->loadFromFile("config.json");
config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);

auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();
```

## Error Handling

The configuration system provides comprehensive error handling:

- **File not found**: Returns false with warning message
- **Invalid format**: Falls back to simpler format if JSON/YAML parsing fails
- **Missing required parameters**: Warns but continues with defaults
- **Invalid values**: Validates ranges and logs warnings

## Advanced Configuration

### Performance Configuration
```json
{
    "neuron_count": 10000,
    "region_count": 4,
    "connection_probability": 0.1,
    "simulation_timestep": 0.0001,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20,
    "synaptogenesis_rate": 0.00005,
    "pruning_rate": 0.000005
}
```

### Research Configuration
```json
{
    "neuron_count": 5000,
    "region_count": 2,
    "connection_probability": 0.05,
    "simulation_timestep": 0.001,
    "enable_reward_modulation": true,
    "enable_curiosity": true,
    "enable_development": true,
    "enable_structural_plasticity": true,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 15
}
```

## Validation

Use the validation function to check configuration:
```python
from nlm import validateConfig

config = Config()
config.loadFromFile("my_config.json")

errors, warnings = validateConfig(config)
if errors:
    print("Configuration errors:", errors)
if warnings:
    print("Configuration warnings:", warnings)
```

## Version Information

- **NLM Version**: 6.0 (Phase 6 Final Integration)
- **Python Bindings**: 1.0.0
- **C++ Standard**: C++20
- **Last Updated**: 2026-09-24

## Further Reading

- [NLM Documentation](https://nlm.readthedocs.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)