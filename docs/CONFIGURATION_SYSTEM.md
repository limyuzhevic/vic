# NLM Configuration System

## Overview
NLM's configuration system provides a flexible and extensible way to configure neural network parameters, simulation settings, and plugin behavior.

## Configuration Architecture

### Configuration Types

#### Global Configuration
- **Config.hpp**: Core configuration management
- **AdvancedConfig.hpp**: Extended configuration features
- **ConfigSchema.hpp**: Configuration validation and schemas

#### Plugin-Specific Configuration
- Each plugin can define its own configuration section
- Configuration is merged with global settings
- Plugin-specific validation and defaults

### Configuration Management

#### Configuration Loading
1. **File-based**: Load from configuration files
2. **Command-line**: Override with command-line arguments
3. **Programmatic**: Set configuration programmatically
4. **Environment**: Environment variable substitution

#### Configuration Validation
- Schema-based validation
- Type checking
- Value range validation
- Cross-parameter validation

### Configuration Schema

#### Configuration Schema Features
- Define configuration parameters with types
- Set default values
- Validate parameter ranges
- Document configuration options
- Support for nested configuration

#### Schema Example
```yaml
parameters:
  neuron_count:
    type: int
    default: 1000
    min: 10
    max: 100000
    description: Number of neurons in the brain

  plasticity:
    type: nested
    parameters:
      learning_rate:
        type: float
        default: 0.01
        min: 0.0
        max: 1.0
        description: Learning rate for plasticity rules
```

## Configuration File Formats

### NLM Configuration Format (.cfg)
```
# NLM Configuration Example
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001

# Plasticity parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity
synaptogenesis_rate = 0.0001n
pruning_rate = 0.00001n

# Memory parameters
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
```

### YAML Configuration Format (.yaml)
```yaml
nlm:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.15
  simulation_timestep: 0.001

  plasticity:
    stdp:
      ltp_weight: 0.02
      ltd_weight: 0.015
      tau: 20.0
    structural:
      synaptogenesis_rate: 0.0001
      pruning_rate: 0.00001

  memory:
    working_memory:
      capacity: 100
    episodic_memory:
      max_episodes: 1000
```

### JSON Configuration Format (.json)
```json
{
  "nlm": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "plasticity": {
      "stdp": {
        "ltp_weight": 0.02,
        "ltd_weight": 0.015,
        "tau": 20.0
      },
      "structural": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
      }
    },
    "memory": {
      "working_memory": {
        "capacity": 100
      },
      "episodic_memory": {
        "max_episodes": 1000
      }
    }
  }
}
```

## Configuration Parameters

### Core Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| neuron_count | int | 1000 | Number of neurons |
| region_count | int | 2 | Number of brain regions |
| connection_probability | float | 0.1 | Probability of connection between neurons |
| simulation_timestep | float | 0.001 | Simulation timestep in seconds |

### Plasticity Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| plasticity_learning_rate | float | 0.01 | Global learning rate |
| stdp_ltp_weight | float | 0.01 | STDP LTP weight |
| stdp_ltd_weight | float | 0.012 | STDP LTD weight |
| stdp_time_constant | float | 20.0 | STDP time constant |

### Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| development_synaptogenesis_rate | float | 0.001 | Synaptogenesis rate |
| development_pruning_rate | float | 0.0001 | Pruning rate |

### Neuromodulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| dopamine_baseline | float | 0.0 | Baseline dopamine level |
| curiosity_rate | float | 0.1 | Curiosity modulation rate |

### Memory Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| working_memory_capacity | int | 100 | Working memory capacity |
| episodic_memory_max_episodes | int | 1000 | Maximum episodic memory episodes |

## Configuration Validation

### Validation Rules
1. **Type validation**: Parameters must be of correct type
2. **Range validation**: Parameters must be within valid ranges
3. **Dependency validation**: Required parameters must be present
4. **Consistency validation**: Parameters must be internally consistent

### Validation Example
```cpp
bool validateConfiguration(const Config& config) {
    // Check neuron count
    if (config.neuron_count < 10 || config.neuron_count > 100000) {
        throw ConfigurationError("Invalid neuron_count");
    }
    
    // Check connection probability
    if (config.connection_probability < 0.0 || config.connection_probability > 1.0) {
        throw ConfigurationError("Invalid connection_probability");
    }
    
    // Check timestep
    if (config.simulation_timestep <= 0.0) {
        throw ConfigurationError("Invalid simulation_timestep");
    }
    
    return true;
}
```

## Configuration Templates

### Basic Configuration
```
# NLM Basic Configuration
neuron_count = 100
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
plasticity_learning_rate = 0.01
max_simulation_steps = 1000
```

### Advanced Configuration
```
# NLM Advanced Configuration
neuron_count = 5000
region_count = 3
connection_probability = 0.15
simulation_timestep = 0.0005
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
curiosity_rate = 0.2
working_memory_capacity = 500
episodic_memory_max_episodes = 5000
max_simulation_steps = 100000
```

### Experiment Configuration
```
# NLM Experiment Configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.15
simulation_timestep = 0.001
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
max_simulation_steps = 10000
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

## Configuration Usage

### Loading Configuration from File
```cpp
#include "src/core/Config/Config.hpp"
#include <memory>

bool loadConfiguration(const std::string& configPath) {
    auto config = std::make_shared<Config>();
    
    if (config->loadFromFile(configPath)) {
        std::cout << "Configuration loaded from: " << configPath << std::endl;
        std::cout << config->summary() << std::endl;
        return true;
    } else {
        std::cerr << "Failed to load configuration from: " << configPath << std::endl;
        return false;
    }
}
```

### Overriding Configuration
```cpp
#include "src/core/Config/Config.hpp"
#include <memory>

void overrideConfiguration(std::shared_ptr<Config>& config) {
    // Override command-line values
    config->set("neuron_count", 500);
    config->set("region_count", 1);
    config->set("connection_probability", 0.2);
    
    // Set new values
    config->set("plasticity_learning_rate", 0.02);
    config->set("max_simulation_steps", 5000);
}
```

### Configuration Monitoring
```cpp
#include "src/core/Config/Config.hpp"
#include <memory>

void monitorConfiguration(std::shared_ptr<Config>& config) {
    // Get configuration summary
    std::cout << config->summary() << std::endl;
    
    // Check for invalid configurations
    if (!config->validate()) {
        std::cerr << "Configuration validation failed" << std::endl;
    }
}
```