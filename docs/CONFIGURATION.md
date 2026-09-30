# NLM Configuration Guide

## Overview

This document provides comprehensive documentation for the NLM (熙然) configuration system, including all configuration options and their usage.

## Configuration File Structure

NLM uses JSON-based configuration files. The primary configuration file is `configs/default.cfg`.

### Configuration Categories

1. **Core Simulation Parameters**
2. **Neural Network Parameters**
3. **Plasticity Parameters**
4. **Development Parameters**
5. **Neuromodulation Parameters**
6. **Memory System Parameters**
7. **Prediction System Parameters**
8. **Cognition System Parameters**
9. **Environment Parameters**
10. **Logging Parameters**

## Core Simulation Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int64_t | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `max_simulation_steps` | size_t | 10000 | Maximum number of simulation steps |
| `simulation_time_limit` | double | 0.0 | Time limit in seconds (0.0 = no limit) |

## Neural Network Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | size_t | 1000 | Total number of neurons in the brain |
| `region_count` | size_t | 2 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability for random connectivity |

## Plasticity Parameters

### STDP Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity_learning_rate` | float | 0.01 | General plasticity learning rate |
| `stdp_ltp_weight` | float | 0.01 | Spike-Timing-Dependent Plasticity LTP weight |
| `stdp_ltd_weight` | float | 0.012 | Spike-Timing-Dependent Plasticity LTD weight |
| `stdp_time_constant` | float | 20.0 | STDP time constant (ms) |

### Synaptic Dynamics Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `synaptic_facilitation_tau` | float | 100.0 | Short-term facilitation time constant (ms) |
| `synaptic_depression_tau` | float | 200.0 | Short-term depression time constant (ms) |
| `eligibility_decay_rate` | float | 0.001 | Reward-modulated eligibility decay rate |
| `min_efficacy` | float | 0.5 | Minimum synaptic efficacy |
| `max_efficacy` | float | 1.5 | Maximum synaptic efficacy |

## Development Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development_synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `development_pruning_rate` | float | 0.0001 | Rate of synapse elimination |

## Neuromodulation Parameters

### Dopamine Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level |

### Other Neuromodulators
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `curiosity_weight` | float | 0.5 | Curiosity modulation weight |
| `novelty_sensitivity` | float | 0.3 | Novelty detection sensitivity |
| `reward_discount_factor` | float | 0.99 | Reward discount factor |

## Memory System Parameters

### Working Memory
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory_capacity` | size_t | 100 | Maximum number of working memory traces |

### Episodic Memory
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `episodic_memory_max_episodes` | size_t | 1000 | Maximum number of stored episodes |

## Prediction System Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction_confidence_threshold` | float | 0.7 | Confidence threshold for predictions |

## Cognition System Parameters

### Neural Planner
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `planning_depth` | size_t | 5 | Planning depth for action sequences |

### Attention System
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `attention_inhibition_strength` | float | 0.5 | Strength of attentional inhibition |
| `attention_excitation_strength` | float | 1.5 | Strength of attentional excitation |

## Environment Parameters

### GridWorld Environment
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment_name` | string | "GridWorld" | Environment type name |
| `environment_width` | size_t | 10 | Environment width (cells) |
| `environment_height` | size_t | 10 | Environment height (cells) |

## Logging Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `log_level` | string | "INFO" | Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL) |
| `log_to_file` | bool | false | Whether to log to file |
| `log_filename` | string | "nlm.log" | Log file name |

## Configuration File Examples

### Basic Configuration
```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
```

### Advanced Configuration
```
# NLM Configuration File - Advanced
random_seed = 12345
simulation_timestep = 0.0005
max_simulation_steps = 50000

# Neural network
neuron_count = 5000
region_count = 4
connection_probability = 0.15

# Plasticity
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0
synaptic_facilitation_tau = 150.0
synaptic_depression_tau = 250.0
eligibility_decay_rate = 0.0005

# Neuromodulation
dopamine_baseline = 0.2
reward_discount_factor = 0.995

# Development
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002

# Environment
environment_name = GridWorld
environment_width = 20
environment_height = 20

# Logging
log_level = INFO
log_to_file = true
log_filename = nlm_advanced.log
```

### Configuration with Integration Tests
```
# NLM Configuration File - Integration Test Setup
random_seed = 42
simulation_timestep = 0.001
max_simulation_steps = 1000

# Neural network for integration tests
neuron_count = 100
region_count = 1
connection_probability = 0.2

# Enhanced plasticity for integration testing
plasticity_learning_rate = 0.05
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.04
stdp_time_constant = 50.0

# Strong neuromodulation for integration testing
dopamine_baseline = 0.5
reward_discount_factor = 0.9

# Development parameters
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging for debugging
log_level = DEBUG
log_to_file = true
log_filename = nlm_integration.log
```

## Programmatic Configuration

### Using C++ API
```cpp
#include "core/Config/Config.hpp"

// Create configuration
auto config = std::make_shared<nlm::Config>();

// Set values
config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::User);
config->set("simulation_timestep", 0.0005, nlm::ConfigSource::User);
config->set("dopamine_baseline", 0.2, nlm::ConfigSource::User);

// Get values
int64_t neuronCount = config->get<int64_t>("neuron_count").value_or(1000);
double timestep = config->get<double>("simulation_timestep").value_or(0.001);
float dopamine = config->get<float>("dopamine_baseline").value_or(0.1f);

// Check if value exists
if (auto opt = config->get<bool>("log_to_file")) {
    bool logToFile = *opt;
}
```

### Configuration Validation
```cpp
// Validate configuration
bool validateConfiguration(const nlm::Config& config) {
    // Check required parameters
    if (!config.get<size_t>("neuron_count")) {
        std::cerr << "Error: neuron_count not specified" << std::endl;
        return false;
    }
    
    // Check parameter ranges
    size_t neuronCount = config.get<size_t>("neuron_count").value_or(0);
    if (neuronCount == 0 || neuronCount > 100000) {
        std::cerr << "Error: neuron_count out of range" << std::endl;
        return false;
    }
    
    float timestep = config.get<float>("simulation_timestep").value_or(0.0f);
    if (timestep <= 0.0f || timestep > 0.1f) {
        std::cerr << "Error: simulation_timestep out of valid range" << std::endl;
        return false;
    }
    
    return true;
}
```

## Configuration Sources

NLM supports multiple configuration sources with different priorities:

### Configuration Source Priority
1. **Programmatic**: `ConfigSource::Programmatic` (highest priority)
2. **User**: `ConfigSource::User`
3. **Command Line**: `ConfigSource::CommandLine`
4. **File**: `ConfigSource::File`
5. **Default**: `ConfigSource::Default` (lowest priority)

### Configuration Merge Behavior
When multiple sources provide the same parameter, the one with higher priority wins:
```cpp
auto config = std::make_shared<nlm::Config>();

// Load from file (default source)
config->loadFromFile("configs/default.cfg");

// Override with user values
config->set("neuron_count", 5000, nlm::ConfigSource::User);
config->set("dopamine_baseline", 0.2, nlm::ConfigSource::User);

// Final values:
// neuron_count = 5000 (User priority)
// dopamine_baseline = 0.2 (User priority)  
// random_seed = 42 (from file)
```

## Configuration Error Handling

### Common Configuration Errors

1. **Missing Required Parameters**
```cpp
if (!config.get<size_t>("neuron_count")) {
    // Handle missing parameter
    std::cerr << "Error: neuron_count is required" << std::endl;
    return 1;
}
```

2. **Invalid Parameter Values**
```cpp
size_t neuronCount = config.get<size_t>("neuron_count").value_or(0);
if (neuronCount == 0) {
    std::cerr << "Error: neuron_count must be > 0" << std::endl;
    return 1;
}
if (neuronCount > 100000) {
    std::cerr << "Error: neuron_count must be <= 100000" << std::endl;
    return 1;
}
```

3. **Invalid Parameter Types**
```cpp
auto timestepOpt = config.get<double>("simulation_timestep");
if (timestepOpt && *timestepOpt <= 0.0) {
    std::cerr << "Error: simulation_timestep must be > 0" << std::endl;
    return 1;
}
```

## Configuration Examples for Different Use Cases

### Research Use Case
```
# Research configuration for studying plasticity
random_seed = 12345
simulation_timestep = 0.0005
max_simulation_steps = 100000

neuron_count = 10000
region_count = 8
connection_probability = 0.12

plasticity_learning_rate = 0.05
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.024
stdp_time_constant = 30.0

# Strong neuromodulation for research
dopamine_baseline = 0.3
reward_discount_factor = 0.99

# Development for longer experiments
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.00005

# Environment for complex tasks
environment_name = GridWorld
environment_width = 50
environment_height = 50

# Detailed logging for analysis
log_level = INFO
log_to_file = true
log_filename = nlm_research.log
```

### Performance Testing Use Case
```
# Performance testing configuration
random_seed = 42
simulation_timestep = 0.01
max_simulation_steps = 1000

neuron_count = 100
region_count = 2
connection_probability = 0.15

# Faster plasticity for performance testing
plasticity_learning_rate = 0.1
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.06

# Fast neuromodulation
reward_discount_factor = 0.95

# Small environment for quick testing
environment_name = GridWorld
environment_width = 5
environment_height = 5

# Basic logging
log_level = WARNING
log_to_file = false
```

### Development Use Case
```
# Development configuration with extensive logging
random_seed = 42
simulation_timestep = 0.001
max_simulation_steps = 5000

neuron_count = 500
region_count = 3
connection_probability = 0.1

# Conservative plasticity for debugging
plasticity_learning_rate = 0.001
stdp_ltp_weight = 0.005
stdp_ltd_weight = 0.006

# Strong neuromodulation for debugging
dopamine_baseline = 0.5

# Development for longer sessions
development_synaptogenesis_rate = 0.0005
development_pruning_rate = 0.0001

# Debug logging for development
log_level = DEBUG
log_to_file = true
log_filename = nlm_development.log
```

## Configuration Best Practices

### For Research
1. **Use different random seeds** for each experiment run
2. **Set simulation_timestep** to match your research needs
3. **Document all parameters** used in research publications
4. **Use descriptive config filenames** (e.g., `research_config.cfg`)

### For Production
1. **Use optimized parameters** for performance
2. **Set appropriate simulation_time_limit** to prevent runaway simulations
3. **Configure logging** to balance debug info and performance
4. **Use production-grade config files** with minimal debug output

### For Development
1. **Use DEBUG logging** for detailed troubleshooting
2. **Start with conservative plasticity rates** to avoid instability
3. **Use smaller environments** for faster iteration
4. **Document configuration** changes for team collaboration

## Configuration Migration Guide

### From Older Versions

When upgrading from older NLM versions, consider these configuration changes:

1. **New Parameters**: Added new synaptic dynamics parameters
2. **Changed Parameter Names**: Some parameter names changed
3. **Default Value Changes**: Some defaults were updated
4. **New Configuration Sources**: Additional configuration sources available

### Migration Examples

#### Upgrading to Phase 6
```
# OLD configuration (Phase 5)
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01

# NEW configuration (Phase 6 enhanced)
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01

# NEW synaptic dynamics (not present in Phase 5)
synaptic_facilitation_tau = 100.0
synaptic_depression_tau = 200.0
eligibility_decay_rate = 0.001
```

#### Configuring for Integration Tests
```
# OLD: Basic configuration
max_simulation_steps = 10000

# NEW: Integration test configuration
max_simulation_steps = 1000
neuron_count = 100
region_count = 1
connection_probability = 0.2

# Enhanced plasticity for integration testing
plasticity_learning_rate = 0.05
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.04
```

## Configuration Troubleshooting

### Common Issues and Solutions

1. **Simulation Doesn't Start**
```
# Check for required parameters
simulation_timestep must be > 0
neuron_count must be > 0
region_count must be > 0
```

2. **Configuration Not Loading**
```
# Check file exists and is readable
# Verify file format is JSON
# Check permissions
```

3. **Unexpected Behavior**
```
# Check configuration values
# Compare with documentation
# Verify expected types and ranges
```

4. **Performance Issues**
```
# Reduce neuron_count for debugging
# Increase simulation_timestep for speed
# Reduce max_simulation_steps
```

### Debugging Configuration
```cpp
// Add debugging to configuration loading
auto config = std::make_shared<nlm::Config>();
config->loadFromFile("configs/default.cfg");

std::cout << "Loaded configuration:" << std::endl;
std::cout << "  neuron_count: " << config->get<size_t>("neuron_count").value_or(0) << std::endl;
std::cout << "  simulation_timestep: " << config->get<double>("simulation_timestep").value_or(0.0) << std::endl;
std::cout << "  dopamine_baseline: " << config->get<float>("dopamine_baseline").value_or(0.0f) << std::endl;
```

## Configuration Command Line Usage

### Basic Usage
```bash
# Use specific configuration file
./nlm --config custom.cfg

# Use configuration file with parameters
./nlm --config configs/default.cfg --neuron_count 5000 --region_count 4

# List configuration options
./nlm --help
```

### Configuration File Format
NLM configuration files use JSON format:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "dopamine_baseline": 0.1,
    "log_level": "INFO"
}
```

## Configuration Validation Script

### Simple Validation Script
```bash
#!/bin/bash
# validate_config.sh - Validate NLM configuration file

CONFIG_FILE="$1"

if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: Configuration file '$CONFIG_FILE' not found"
    exit 1
fi

# Check if file is valid JSON (basic check)
if ! python3 -c "import json; json.load(open('$CONFIG_FILE'))" 2>/dev/null; then
    echo "Error: Configuration file '$CONFIG_FILE' is not valid JSON"
    exit 1
fi

# Check required parameters
python3 << EOF
import json
import sys

try:
    with open('$CONFIG_FILE') as f:
        config = json.load(f)
    
    required = ['random_seed', 'simulation_timestep', 'neuron_count', 'region_count']
    for param in required:
        if param not in config:
            print(f"Error: Required parameter '{param}' is missing")
            sys.exit(1)
    
    # Check parameter ranges
    if config['neuron_count'] <= 0:
        print("Error: neuron_count must be > 0")
        sys.exit(1)
    
    if config['region_count'] <= 0:
        print("Error: region_count must be > 0")
        sys.exit(1)
    
    if config['simulation_timestep'] <= 0:
        print("Error: simulation_timestep must be > 0")
        sys.exit(1)
    
    print("Configuration file '$CONFIG_FILE' is valid")
    
except Exception as e:
    print(f"Error validating configuration: {e}")
    sys.exit(1)
EOF

if [ $? -eq 0 ]; then
    echo "Configuration validation successful"
else
    echo "Configuration validation failed"
    exit 1
fi
```

## Conclusion

Configuration is a critical aspect of NLM operations. Proper configuration ensures:

1. **Simulation stability** with appropriate parameters
2. **Research reproducibility** with documented settings
3. **Performance optimization** with tuned parameters
4. **Development efficiency** with comprehensive documentation

This configuration guide provides comprehensive information for configuring NLM across different use cases and experience levels.