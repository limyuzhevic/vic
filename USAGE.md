# NLM Configuration System - Usage Guide

## Overview

This document provides a comprehensive guide for using the NLM (Neural Learning Machine) configuration system that replaces all magic numbers with configurable parameters.

## Quick Start

### 1. Initialize Configuration System

```cpp
#include "core/Config/Config.hpp"
#include "core/Config/ConfigManager.hpp"

// Initialize the configuration system
nlm::initializeConfigManager();
nlm::ConfigManager& config = nlm::getConfigManager();
```

### 2. Access Configuration Values

```cpp
// Get configuration values with defaults
float timestep = config.getOr<float>("simulation_timestep", 0.001f);
int neurons = config.getOr<int>("neuron_count", 1000);
bool devEnabled = config.getOr<bool>("development_enabled", true);
```

### 3. Set Configuration Values

```cpp
// Set configuration values
config.setParameter<float>("simulation_timestep", 0.002f, "simulation", "New timestep");
config.setParameter<int>("neuron_count", 2000, "simulation", "New neuron count");
```

### 4. Load Configuration File

```cpp
// Load from file
bool success = config.loadFromFile("configs/experiment.cfg");
if (success) {
    std::cout << "Configuration loaded successfully" << std::endl;
}
```

### 5. Save Configuration

```cpp
// Save to file
bool success = config.saveToFile("configs/my_config.cfg");
```

### 6. Command Line Interface

```bash
# Basic usage
./nlm --config-file configs/experiment.cfg

# Command line overrides
./nlm --config-file configs/base.cfg --simulation_timestep 0.002 --neuron_count 2000

# Save configuration
./nlm --save-config my_config.cfg

# Load configuration
./nlm --load-config experiment.cfg
```

## Configuration File Examples

### 1. Basic Configuration (default.cfg)
```ini
# Simulation Parameters
simulation_timestep = 0.001
neuron_count = 1000
connection_probability = 0.1

# Neural Parameters
membrane_potential_threshold = -55.0
resting_potential = -70.0
inhibitory_strength = 0.5

# Plasticity Parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
```

### 2. Research Scenario: Memory Consolidation
```ini
# Memory Consolidation Study
simulation_timestep = 0.001
simulation_steps = 50000
neuron_count = 2000
replay_interval = 50
consolidation_interval = 500
working_memory_capacity = 200
episodic_memory_limit = 2000
working_memory_decay_rate = 0.9
episodic_memory_decay_rate = 0.99
visualization_enabled = true
```

### 3. Research Scenario: Neuromodulation
```ini
# Neuromodulation Study
simulation_timestep = 0.001
simulation_steps = 20000
neuron_count = 1000
region_count = 3
dopamine_baseline = 0.5
dopamine_scale = 2.0
curiosity_threshold = 0.2
novelty_threshold = 0.3
reward_discount_factor = 0.95
```

## Integration with Existing Code

### 1. Brain.cpp Integration

Replace hardcoded values with configuration calls:

**Before:**
```cpp
timestep = 0.001; // Hardcoded
float ltpWeight = 0.01f; // Hardcoded
float ltdWeight = 0.012f; // Hardcoded
```

**After:**
```cpp
timestep = config->getOr<double>("simulation_timestep", 0.001);
float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
```

### 2. AgentBrain.cpp Integration

Replace magic numbers in AgentBrain constructor and methods:

**Before:**
```cpp
AgentBrain() 
    : dopamineLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , developmentalAge_(0.0)
    , plasticityModifier_(1.0f)
    , curiosityEnabled_(true)
    , sensoryNoveltyDecay_(0.99f)
```

**After:**
```cpp
AgentBrain() 
    : dopamineLevel_(config->getOr<float>("dopamine_level", 0.0f))
    , noveltyLevel_(config->getOr<float>("novelty_level", 0.0f))
    , curiosityLevel_(config->getOr<float>("curiosity_level", 0.0f))
    , predictionError_(config->getOr<float>("prediction_error", 0.0f))
    , expectedReward_(config->getOr<float>("expected_reward", 0.0f))
    , developmentalAge_(0.0)
    , plasticityModifier_(config->getOr<float>("plasticity_modifier", 1.0f))
    , curiosityEnabled_(config->getOr<bool>("curiosity_enabled", true))
    , sensoryNoveltyDecay_(config->getOr<float>("sensory_novelty_decay", 0.99f))
```

## Configuration Parameter Categories

### 1. Simulation Parameters
- `simulation_timestep`: Simulation timestep in seconds
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Connection probability for random connectivity
- `replay_interval`: Memory replay interval
- `consolidation_interval`: Memory consolidation interval

### 2. Neural Parameters
- `membrane_potential_threshold`: Threshold for spike generation
- `resting_potential`: Resting membrane potential
- `inhibitory_strength`: Inhibitory synaptic strength
- `synaptic_gain`: Synaptic gain factor

### 3. Plasticity Parameters
- `stdp_ltp_weight`: STDP Long-Term Potentiation weight
- `stdp_ltd_weight`: STDP Long-Term Depression weight
- `stdp_time_constant`: STDP time constant
- `hebbian_learning_rate`: Hebbian learning rate

### 4. Memory Parameters
- `working_memory_capacity`: Working memory capacity
- `episodic_memory_limit`: Episodic memory storage limit
- `working_memory_decay_rate`: Working memory decay rate
- `episodic_memory_decay_rate`: Episodic memory decay rate

### 5. Development Parameters
- `development_enabled`: Enable developmental processes
- `plasticity_modifier_rate`: Plasticity modifier rate
- `maturation_rate`: Maturation rate

### 6. Neuromodulation Parameters
- `dopamine_baseline`: Baseline dopamine level
- `curiosity_threshold`: Curiosity activation threshold
- `novelty_threshold`: Novelty detection threshold
- `reward_discount_factor`: Reward discount factor

### 7. General Parameters
- `random_seed`: Random seed for reproducible experiments
- `log_level`: Logging level
- `visualization_enabled`: Enable visualization

### 8. AgentBrain Parameters
- `dopamine_level`: Initial dopamine level
- `novelty_level`: Initial novelty level
- `curiosity_level`: Initial curiosity level
- `prediction_error`: Initial prediction error
- `plasticity_modifier`: Plasticity modifier factor

## Testing the Configuration System

### Run Tests
```bash
# Run configuration system tests
cd tests
./test_config_system
```

### Test Integration
```cpp
#include <cassert>
#include <iostream>
#include "core/Config/Config.hpp"

void testBasicConfiguration() {
    nlm::initializeConfigManager();
    nlm::ConfigManager& config = nlm::getConfigManager();
    
    // Test basic functionality
    assert(config.hasParameter("simulation_timestep"));
    assert(config.getOr<double>("simulation_timestep", 0.0) > 0.0);
    
    // Test parameter groups
    auto simParams = config.getParameterNamesByGroup("simulation");
    assert(!simParams.empty());
    
    std::cout << "All tests passed!" << std::endl;
}
```

## Configuration Validation

### Range Validation
```cpp
// Validate parameters are within ranges
bool timestepValid = configManager.validateParameter("simulation_timestep");
bool neuronCountValid = configManager.validateParameter("neuron_count");
bool connectionProbValid = configManager.validateParameter("connection_probability");
```

### Consistency Checks
```cpp
// Check parameter consistency
if (configManager.getParameter<bool>("development_enabled")) {
    assert(configManager.getParameter<double>("developmental_stage_duration") > 0);
}
```

## Best Practices

### 1. Configuration Management
- Always validate configuration before use
- Use default configuration as starting point
- Document any changes for reproducibility
- Consider parameter ranges and constraints

### 2. Performance Considerations
- Minimize configuration lookups in performance-critical code
- Cache frequently used configuration values
- Use lightweight configuration access methods

### 3. Testing and Validation
- Test all configuration parameters
- Validate parameter ranges
- Test file I/O operations
- Test command line overrides

### 4. Documentation
- Document all parameters with descriptions
- Provide examples for common scenarios
- Document parameter interactions
- Provide migration guides

## Command Line Examples

### Basic Usage
```bash
./nlm --config-file configs/experiment.cfg
```

### Command Line Overrides
```bash
./nlm \
  --config-file configs/base.cfg \
  --simulation_timestep 0.002 \
  --neuron_count 2000 \
  --dopamine_baseline 0.5
```

### Save Configuration
```bash
./nlm --save-config my_config.cfg
```

### Load Configuration
```bash
./nlm --load-config experiment.cfg
```

## Supported Operations

### Read Operations
- `hasParameter(name)`: Check if parameter exists
- `getParameter<T>(name)`: Get parameter value (returns optional)
- `getOr<T>(name, defaultValue)`: Get parameter with default
- `getParameterNamesByGroup(category)`: Get parameters by category
- `validateParameter(name)`: Validate parameter
- `validateAllParameters()`: Validate all parameters

### Write Operations
- `setParameter<T>(name, value, category, description)`: Set parameter
- `resetParameter(name)`: Reset parameter to default
- `resetAllParameters()`: Reset all parameters
- `enableParameter(name)`: Enable parameter
- `disableParameter(name)`: Disable parameter
- `removeParameter(name)`: Remove parameter

### File Operations
- `loadFromFile(filepath)`: Load configuration from file
- `saveToFile(filepath)`: Save configuration to file
- `loadFromCommandLine(argc, argv)`: Load from command line

## Configuration File Format

The configuration system uses a simple key=value format:

```ini
# Parameter category: Comments
simulation_timestep = 0.001
neuron_count = 1000
connection_probability = 0.1

# Another category
membrane_potential_threshold = -55.0
resting_potential = -70.0
```

## Parameter Types

The configuration system supports:
- `float`: Floating point values
- `double`: Double precision floating point values
- `int`: Integer values
- `bool`: Boolean values (true/false)
- `string`: String values (quoted)

## Default Configuration File

A comprehensive default configuration file is available at:
- `configs/default.cfg`: All simulation parameters with reasonable defaults
- `test_config_system.cpp`: Example test file
- `CONFIGURATION.md`: Detailed documentation

## Integration Checklist

### Required Changes
- [x] Replace magic numbers with configuration calls in Brain.cpp
- [x] Replace magic numbers with configuration calls in AgentBrain.cpp
- [x] Update remaining source files to use configuration
- [x] Create comprehensive configuration documentation
- [x] Test configuration system integration
- [x] Provide migration guide for existing code

### Optional Improvements
- [ ] Add JSON/YAML configuration file support
- [ ] Add configuration schema validation
- [ ] Add configuration inheritance
- [ ] Add configuration templates
- [ ] Add runtime configuration updates
- [ ] Add environment-specific configurations

## Conclusion

The NLM configuration system provides a comprehensive, flexible, and extensible framework for managing simulation parameters. It replaces all magic numbers with configurable values, making the neural simulation framework highly configurable for different research scenarios while maintaining backward compatibility.

Key benefits:
- **Configurability**: All parameters can be configured via files, command line, or runtime
- **Backward Compatibility**: Existing code continues to work with minimal changes
- **Validation**: Parameter ranges and constraints are automatically validated
- **Documentation**: Comprehensive documentation and examples
- **Extensibility**: Easy to add new parameters and parameter groups
- **Performance**: Efficient configuration access and management

The configuration system enables researchers to easily configure different experimental scenarios, reproduce results, and explore parameter spaces for optimization and discovery.