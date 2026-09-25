# NLM Configuration System Examples

This document provides examples of the enhanced NLM configuration system, including preset profiles and various file formats.

## Preset Profiles

### Basic Profile (basic)

Beginner-friendly configuration for simple experiments and getting started with NLM.

**Basic Configuration (basic.cfg - Plain Text Format):**
```
# NLM Configuration - Basic Preset
# Beginner-friendly configuration for simple experiments

# Random seed for reproducible experiments
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.05
log_level = INFO
max_simulation_steps = 1000
```

**Basic Configuration (basic.json):**
```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 100,
  "region_count": 1,
  "connection_probability": 0.05,
  "log_level": "INFO",
  "max_simulation_steps": 1000,
  "_description": "Random seed for reproducible experiments",
  "_description_simulation_timestep": "Simulation timestep in seconds",
  "_description_neuron_count": "Number of neurons in the brain",
  "_description_region_count": "Number of brain regions",
  "_description_connection_probability": "Connection probability for random connectivity",
  "_description_log_level": "Logging level",
  "_description_max_simulation_steps": "Maximum simulation steps"
}
```

### Advanced Profile (advanced)

Advanced configuration for complex experiments with optimization and monitoring capabilities.

**Advanced Configuration (advanced.yml):**
```yaml
# NLM Configuration - Advanced Preset
# Advanced configuration for complex experiments with optimization and monitoring

random_seed: 12345
simulation_timestep: 0.0005
neuron_count: 1000
region_count: 4
connection_probability: 0.1
plasticity_learning_rate: 0.02
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.022
stdp_time_constant: 20.0
development_synaptogenesis_rate: 0.002
development_pruning_rate: 0.0002
dopamine_baseline: 0.1
reward_discount_factor: 0.99
environment_name: "GridWorld"
log_level: "DEBUG"
log_to_file: true
log_filename: "nlm_advanced.log"
max_simulation_steps: 50000
simulation_time_limit: 0.0
visualization_enabled: true
visualization_update_rate: 30.0
# _description comments can be added for each key
```

**Advanced Configuration (advanced.json):**
```json
{
  "random_seed": 12345,
  "simulation_timestep": 0.0005,
  "neuron_count": 1000,
  "region_count": 4,
  "connection_probability": 0.1,
  "plasticity_learning_rate": 0.02,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.022,
  "stdp_time_constant": 20.0,
  "development_synaptogenesis_rate": 0.002,
  "development_pruning_rate": 0.0002,
  "dopamine_baseline": 0.1,
  "reward_discount_factor": 0.99,
  "environment_name": "GridWorld",
  "log_level": "DEBUG",
  "log_to_file": true,
  "log_filename": "nlm_advanced.log",
  "max_simulation_steps": 50000,
  "simulation_time_limit": 0.0,
  "visualization_enabled": true,
  "visualization_update_rate": 30.0
}
```

### Research Profile (research)

Research-grade configuration for scientific studies with comprehensive parameter control and reproducibility.

**Research Configuration (research.cfg):**
```
# NLM Configuration - Research Preset
# Research-grade configuration for scientific studies with comprehensive parameter control and reproducibility

random_seed = 2024
simulation_timestep = 0.0001
neuron_count = 5000
region_count = 10
connection_probability = 0.15
plasticity_learning_rate = 0.015
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.018
stdp_time_constant = 15.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.00005
dopamine_baseline = 0.05
reward_discount_factor = 0.995
environment_name = ComplexEnvironment
log_level = TRACE
log_to_file = true
log_filename = nlm_research_$(date + %Y%m%d_%H%M%S).log
max_simulation_steps = 100000
simulation_time_limit = 3600.0
visualization_enabled = true
visualization_update_rate = 60.0
checkpoint_interval = 5000
replay_interval = 1000
enable_development = true
enable_curiosity = true
enable_novelty = true
```

**Research Configuration (research.json):**
```json
{
  "random_seed": 2024,
  "simulation_timestep": 0.0001,
  "neuron_count": 5000,
  "region_count": 10,
  "connection_probability": 0.15,
  "plasticity_learning_rate": 0.015,
  "stdp_ltp_weight": 0.015,
  "stdp_ltd_weight": 0.018,
  "stdp_time_constant": 15.0,
  "development_synaptogenesis_rate": 0.001,
  "development_pruning_rate": 0.00005,
  "dopamine_baseline": 0.05,
  "reward_discount_factor": 0.995,
  "environment_name": "ComplexEnvironment",
  "log_level": "TRACE",
  "log_to_file": true,
  "log_filename": "nlm_research_$(date + %Y%m%d_%H%M%S).log",
  "max_simulation_steps": 100000,
  "simulation_time_limit": 3600.0,
  "visualization_enabled": true,
  "visualization_update_rate": 60.0,
  "checkpoint_interval": 5000,
  "replay_interval": 1000,
  "enable_development": true,
  "enable_curiosity": true,
  "enable_novelty": true
}
```

## Configuration File Formats

### Plain Text Format (.cfg)

The original key=value format with optional comments and quotes.

```
# Configuration comment
key = value
another_key = "quoted value"
number_key = 42
float_key = 3.14
boolean_key = true
```

### JSON Format (.json)

Modern configuration format with rich type support and metadata.

```json
{
  "key": "value",
  "number": 42,
  "float": 3.14,
  "boolean": true,
  "array": [1, 2, 3],
  "object": {"nested": "value"},
  "description_key": "Description of this configuration value"
}
```

### YAML Format (.yml, .yaml)

Human-readable configuration format.

```yaml
key: value
number: 42
float: 3.14
boolean: true
array:
  - 1
  - 2
  - 3
object:
  nested: value
# Comments are supported in YAML
```

## Configuration System Features

### 1. Validation

Configuration values can be validated against rules:

```cpp
config.addValidationRule({
    "random_seed",
    [](const ConfigValue& val) {
        if (std::holds_alternative<int>(val)) {
            return std::get<int>(val) >= 0;
        }
        return false;
    },
    "Random seed must be a non-negative integer"
});
```

### 2. Inheritance and Merging

Configurations can inherit from templates and merge with other configurations:

```cpp
// Load a base configuration
Config baseConfig;
baseConfig.loadFromFile("base.cfg");

// Create a derived configuration
Config derivedConfig;
derivedConfig.loadFromTemplate(baseTemplate);

// Merge another configuration (overwrite conflicting keys)
derivedConfig.mergeWith(overrideConfig, true);
```

### 3. Command Line Interface

Configuration can be loaded from command line arguments:

```bash
./nlm_simulation \
  --config configs/advanced.json \
  --neuron_count=2000 \
  --simulation_timestep=0.001 \
  --enable_development=true
```

### 4. Runtime Configuration

Configuration can be modified at runtime:

```cpp
// Set a value at runtime
config.set("neuron_count", 1500, ConfigSource::Runtime);

// Get a value
auto neuronCount = config.get<int>("neuron_count");

// Check if a value exists
bool hasNeuronCount = config.has("neuron_count");
```

## Using Preset Profiles

### Loading and Applying Presets

```cpp
#include "Config.hpp"

int main() {
    nlm::Config config;
    
    // Get available preset names
    auto presets = nlm::Config::getAvailablePresets();
    
    // Apply a preset
    config.applyPreset("basic");  // Load basic configuration
    
    // Apply advanced preset
    config.applyPreset("advanced");
    
    // Apply research preset
    config.applyPreset("research");
    
    // Display configuration summary
    std::cout << config.summary() << std::endl;
    
    return 0;
}
```

### Saving Presets

```cpp
#include "Config.hpp"

int main() {
    nlm::Config config;
    
    // Apply a preset
    config.applyPreset("basic");
    
    // Save configuration to file
    config.saveToFile("my_basic_config.json", nlm::ConfigFileFormat::JSON);
    
    return 0;
}
```

## Configuration Options

The configuration system supports various options that control its behavior:

```cpp
nlm::ConfigOptions options;
options.validateOnSet = true;        // Validate on every set operation
options.allowInheritance = true;     // Allow template inheritance
options.preferredFormat = nlm::ConfigFileFormat::JSON;  // Preferred file format
options.mergeOnLoad = false;         // Merge loaded configurations
options.defaultFileExtension = ".json";  // Default file extension

Config config;
config.setOptions(options);
```

## Error Handling

The configuration system provides detailed error messages:

```cpp
try {
    config.loadFromFile("invalid.json");
} catch (const nlm::ConfigValidationError& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
}
```

## Complete Example

```cpp
#include "Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    nlm::Config config;
    
    // Set some basic values
    config.set("random_seed", 42);
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    
    // Apply a preset
    config.applyPreset("advanced");
    
    // Override specific values
    config.set("neuron_count", 1500, nlm::ConfigSource::Runtime);
    
    // Validate configuration
    if (!config.isValid()) {
        auto errors = config.getValidationErrors();
        std::cerr << "Configuration invalid:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  - " << error << std::endl;
        }
        return 1;
    }
    
    // Display configuration
    std::cout << config.summary() << std::endl;
    
    // Save configuration
    config.saveToFile("final_config.json", nlm::ConfigFileFormat::JSON);
    
    return 0;
}
```

## Getting Started Guide

1. **Choose a preset**: Use `config.applyPreset("basic")`, `config.applyPreset("advanced")`, or `config.applyPreset("research")`.

2. **Modify configuration**: Use `config.set(key, value)` to modify values.

3. **Validate configuration**: Use `config.validate()` to check for errors.

4. **Save configuration**: Use `config.saveToFile(filepath)` to save to a file.

5. **Load configuration**: Use `config.loadFromFile(filepath)` to load from a file.

The enhanced NLM configuration system provides a powerful, flexible, and user-friendly way to configure neural learning machine simulations for researchers, developers, and beginners alike.