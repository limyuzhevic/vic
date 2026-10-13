# NLM Configuration System

## Overview

The NLM (Neural Learning Machine) configuration system provides a robust, flexible, and type-safe way to manage application settings. It supports multiple input formats (simple key=value, JSON, YAML), validation, and schema definition.

## Core Concepts

### Configuration Value Types

The configuration system supports the following value types:

- **Basic Types**: `int`, `int64_t`, `double`, `bool`, `std::string`
- **Collection Types**: `std::vector<int>`, `std::vector<double>`, `std::vector<std::string>`

### Configuration Sources

Each configuration value has a source that indicates its origin:

- **Default**: Built-in default values
- **File**: Loaded from configuration file
- **CommandLine**: Set via command line arguments
- **Runtime**: Set programmatically at runtime
- **Environment**: Set via environment variables

### Configuration Classes

#### Config

The main configuration class (`Config`) provides the following functionality:

- **Loading**: Load from files (JSON/YAML/simple format), command line, or environment variables
- **Setting**: Set values with type safety and source tracking
- **Getting**: Retrieve values with type checking and default values
- **Validation**: Validate configuration against schemas
- **Management**: Remove, clear, and list configuration entries
- **Persistence**: Save configuration to files

#### ConfigSchema

The `ConfigSchema` class defines the structure and validation rules for configuration:

- **Field Definition**: Define fields with names, default values, descriptions, and validators
- **Validation**: Validate configurations against schema rules
- **Documentation**: Generate schema documentation

## Usage Examples

### 1. Basic Configuration

```cpp
#include "core/Config/Config.hpp"

// Create and use configuration
nlm::Config config;

// Set values with type safety
config.set("learning_rate", 0.01f);
config.set("max_iterations", 1000);
config.set("enable_logging", true);
config.set("model_name", "neural_network");

// Get values with defaults
float learningRate = config.getOr<float>("learning_rate", 0.001f);
int iterations = config.getOr<int>("max_iterations", 100);
bool loggingEnabled = config.getOr<bool>("enable_logging", false);

// Check if a value exists
if (config.has("model_name")) {
    std::string modelName = config.get<std::string>("model_name").value();
}
```

### 2. Configuration from File

```cpp
// Save configuration to file (JSON format)
config.saveToFile("config.json", true);

// Load configuration from file (supports JSON, YAML, and simple key=value)
nlm::Config config2;
if (config2.loadFromFile("config.json")) {
    std::cout << "Configuration loaded successfully" << std::endl;
} else {
    std::cout << "Failed to load configuration" << std::endl;
}
```

### 3. Configuration Validation

```cpp
// Create a schema for validation
nlm::ConfigSchema schema;

schema.addField("learning_rate", 0.01f, "Neural network learning rate", [](const nlm::ConfigValue& v) {
    return std::get<float>(v) > 0.0f && std::get<float>(v) < 1.0f;
}, true);

schema.addField("max_iterations", 1000, "Maximum training iterations", nullptr, false);

schema.addField("batch_size", 32, "Training batch size (must be power of 2)", [](const nlm::ConfigValue& v) {
    int size = std::get<int>(v);
    return size > 0 && (size & (size - 1)) == 0;  // Check if power of 2
}, true);

// Validate configuration
nlm::Config config;
config.set("learning_rate", 0.05f);
config.set("max_iterations", 500);
config.set("batch_size", 64);

if (config.validate(schema)) {
    std::cout << "Configuration is valid!" << std::endl;
} else {
    std::cout << "Configuration validation failed!" << std::endl;
}
```

### 4. Command Line Interface

```bash
# Basic usage
./nlm basic --help

# Run basic connectivity test
./nlm basic --config configs/default.cfg

# Run plasticity learning experiment
./nlm plasticity --config experiment.cfg --verbose

# Run STDP verification
./nlm stdp --config stdp_config.cfg

# Run batch experiment
./nlm batch --config batch_config.cfg --iterations 10

# Run simulation
./nlm simulation --config sim_config.cfg --steps 5000

# Save configuration
./nlm config --output custom_config.json --format json

# Run benchmark
./nlm benchmark --steps 10000
```

### 5. Environment Variables

```bash
# Set environment variables
export NLM_LEARNING_RATE=0.01
export NLM_MAX_ITERATIONS=1000
export NLM_ENABLE_LOGGING=true

# Run application
./nlm --verbose
```

## Advanced Configuration

### Using JSON Format

```cpp
// Create JSON configuration
std::string jsonConfig = R"({
    "learning_rate": 0.01,
    "max_iterations": 1000,
    "model": {
        "architecture": "feedforward",
        "layers": [64, 32, 16],
        "activation": "relu"
    },
    "training": {
        "batch_size": 32,
        "optimizer": "adam"
    }
})";

// Parse JSON (requires nlohmann_json library)
nlohmann::json json = nlohmann::json::parse(jsonConfig);

// Convert to Config
nlm::Config config;
// Implementation would include JSON parsing logic
```

### Using YAML Format

```cpp
// Create YAML configuration
yaml-cpp YAML;
YAML["learning_rate"] = 0.01;
YAML["max_iterations"] = 1000;
YAML["model"]["architecture"] = "feedforward";
YAML["model"]["layers"] = std::vector<int>{64, 32, 16};

// Parse YAML (requires yaml-cpp library)
// Implementation would include YAML parsing logic
```

### Configuration Templates

```cpp
// Define configuration template
nlm::ConfigSchema createExperimentTemplate() {
    nlm::ConfigSchema schema;
    
    // Core parameters
    schema.addField("experiment_name", "default_experiment", "Name of the experiment", nullptr, true);
    schema.addField("random_seed", 42, "Random seed for reproducibility", nullptr, false);
    schema.addField("simulation_timestep", 0.001, "Simulation timestep", nullptr, false);
    
    // Neural network parameters
    schema.addField("neuron_count", 1000, "Number of neurons", [](const nlm::ConfigValue& v) {
        return std::get<int64_t>(v) > 0;
    }, false);
    
    // Plasticity parameters
    schema.addField("plasticity_enabled", true, "Enable plasticity", nullptr, false);
    schema.addField("plasticity_rate", 0.01f, "Plasticity learning rate", [](const nlm::ConfigValue& v) {
        return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f;
    }, false);
    
    // Learning parameters
    schema.addField("training_epochs", 100, "Number of training epochs", [](const nlm::ConfigValue& v) {
        return std::get<int>(v) > 0;
    }, false);
    
    return schema;
}
```

## Configuration Files

### Example Configuration Files

#### configs/default.cfg (Simple Format)

```
# NLM Configuration File
# This is the default configuration for Phase 2: Real Neural Computation

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

#### configs/experiment.json (JSON Format)

```json
{
    "experiment": {
        "name": "plasticity_learning",
        "description": "Test synaptic plasticity and learning capabilities",
        "parameters": {
            "neuron_count": 500,
            "region_count": 1,
            "simulation_timestep": 0.001,
            "plasticity_enabled": true,
            "stdp_enabled": true,
            "hebbian_enabled": true
        },
        "training": {
            "epochs": 100,
            "batch_size": 32,
            "learning_rate": 0.01
        },
        "output": {
            "save_results": true,
            "output_directory": "results",
            "save_visualizations": false
        }
    },
    "validation": {
        "required_fields": ["neuron_count", "region_count"],
        "ranges": {
            "neuron_count": {"min": 10, "max": 10000},
            "learning_rate": {"min": 0.0, "max": 1.0}
        }
    }
}
```

## Configuration Best Practices

### 1. Use Type Safety

Always use typed getters to ensure you get the correct type:

```cpp
// Good practice
float rate = config.getOr<float>("learning_rate", 0.01f);

// Bad practice - type confusion
float rate = config.getOr<double>("learning_rate", 0.01f); // Potential precision loss
```

### 2. Validate Configuration

Always validate configuration against schemas:

```cpp
if (!config.validate(schema)) {
    std::cerr << "Invalid configuration!" << std::endl;
    return 1;
}
```

### 3. Provide Meaningful Default Values

Define meaningful defaults for all optional parameters:

```cpp
schema.addField("learning_rate", 0.001f, "Neural network learning rate", nullptr, false);
schema.addField("max_iterations", 1000, "Maximum training iterations", nullptr, false);
schema.addField("batch_size", 32, "Training batch size", nullptr, false);
```

### 4. Use Environment Variables for Override

Use environment variables for overriding configuration in different environments:

```bash
# Development
export NLM_ENV=development
export NLM_LOG_LEVEL=DEBUG

# Production
export NLM_ENV=production
export NLM_LOG_LEVEL=ERROR
```

### 5. Handle Errors Gracefully

Always handle configuration errors gracefully:

```cpp
if (!config.loadFromFile("config.json")) {
    std::cerr << "Warning: Could not load configuration file. Using defaults." << std::endl;
    // Set default values...
}
```

## Performance Considerations

### 1. Configuration Caching

Cache frequently accessed configuration values:

```cpp
const float& getLearningRate() {
    static float rate = config.getOr<float>("learning_rate", 0.01f);
    return rate;
}
```

### 2. Lazy Loading

Load configuration files lazily when needed:

```cpp
// Instead of loading at startup
// config = std::make_shared<Config>(); config->loadFromFile("config.json");

// Load on first use
std::shared_ptr<Config> getConfig() {
    static std::shared_ptr<Config> config;
    if (!config) {
        config = std::make_shared<Config>();
        config->loadFromFile("config.json");
    }
    return config;
}
```

### 3. Configuration Validation Optimization

Cache validation results when configuration doesn't change:

```cpp
bool isValid() {
    static bool valid = validate(schema);
    return valid;
}
```

## Migration Guide

### From Simple Config to Enhanced Config

If you were using the simple config system before, the migration is straightforward:

```cpp
// Old way (simple key=value only)
// config->set("learning_rate", 0.01f);
// config->loadFromFile("config.cfg");

// New way (with validation and JSON/YAML support)
nlm::Config config;
config.set("learning_rate", 0.01f);
config.loadFromFile("config.json");  // Supports both formats
```

The new config system maintains backward compatibility with the simple key=value format while adding powerful new features.

## Conclusion

The enhanced configuration system provides a robust, type-safe, and extensible way to manage NLM configuration. It supports multiple input formats, validation, and schema definition, making it suitable for both simple applications and complex scientific computing scenarios.

Key benefits:

- **Type Safety**: Compile-time type checking for all configuration values
- **Flexibility**: Support for multiple input formats (simple, JSON, YAML)
- **Validation**: Schema-based validation with custom validators
- **Performance**: Efficient operations for large-scale configurations
- **Extensibility**: Easy to add new configuration types and validation rules
- **Documentation**: Automatic schema documentation generation

The configuration system is a fundamental component of the NLM framework, enabling users to easily configure and customize neural network simulations and experiments.
