# NLM Configuration System Documentation

## Overview

NLM features a sophisticated configuration system that supports JSON, YAML, and custom format parsing with comprehensive validation and type-safe access. The configuration system evolved from a simple key=value format to a comprehensive builder pattern with advanced features.

## Configuration System Architecture

### Core Components

```mermaid
graph TD
    A[Configuration] --> B[DefaultConfiguration]
    A --> C[ConfigurationBuilder]
    A --> D[ConfigurationValidator]
    
    B --> B1[Key-Value Storage]
    B --> B2[Change Notification]
    B --> B3[Source Tracking]
    
    C --> C1[Type-Safe Access]
    C --> C2[Section Management]
    C --> C3[Validation Rules]
    C --> C4[Dependency Management]
    
    D --> D1[Rule-Based Validation]
    D --> D2[Cross-Validation]
    D --> D3[Schema Generation]
    
    A --> E[InputFormat]
    E --> E1[JSON]
    E --> E2[YAML]
    E --> E3[Simple]
end
```

## Configuration Formats

### JSON Configuration

JSON configuration files use standard JSON syntax:

```json
{
  "nlm": {
    "brain": {
      "neuron_count": 1000,
      "synapse_density": 0.1,
      "connection_probability": 0.05,
      "learning_rate": 0.001
    },
    "plasticity": {
      "stdp": {
        "enabled": true,
        "learning_rate": 0.001
      },
      "hebbian": {
        "enabled": true
      }
    },
    "neuromodulation": {
      "curiosity": {
        "enabled": true,
        "weight": 0.5
      }
    }
  },
  "phase6": {
    "enable_checkpointing": true,
    "checkpoint_path": "/tmp/nlm_checkpoints",
    "max_steps": 10000,
    "enable_replay": true
  }
}
```

### YAML Configuration

YAML configuration files use human-readable syntax:

```yaml
nlm:
  brain:
    neuron_count: 1000
    synapse_density: 0.1
    connection_probability: 0.05
    learning_rate: 0.001
  plasticity:
    stdp:
      enabled: true
      learning_rate: 0.001
    hebbian:
      enabled: true
  neuromodulation:
    curiosity:
      enabled: true
      weight: 0.5

phase6:
  enable_checkpointing: true
  checkpoint_path: /tmp/nlm_checkpoints
  max_steps: 10000
  enable_replay: true
```

### Simple Key=Value Format

For backward compatibility, NLM still supports the simple key=value format:

```
# config.txt
nlm.brain.neuron_count = 1000
nlm.brain.synapse_density = 0.1
nlm.plasticity.stdp.enabled = true
phase6.max_steps = 10000
phase6.enable_checkpointing = true
```

## Configuration Builder Pattern

### Creating Configuration

The configuration builder provides a declarative way to define configuration options:

```cpp
#include "nlm/core/Config.hpp"
#include <memory>

// Method 1: Using the factory function
auto config = nlm::createConfiguration();

// Method 2: Using builder pattern
auto config = nlm::createConfiguration();
auto builder = config->getBuilder();

// Add basic configuration options
builder->addInt("nlm.brain.neuron_count", 1000, "Total number of neurons")
       ->addDouble("nlm.brain.synapse_density", 0.1, "Synapse connectivity density")
       ->addDouble("nlm.brain.connection_probability", 0.05, "Connection probability")
       ->addDouble("nlm.brain.learning_rate", 0.001, "Learning rate for plasticity")
       ->addBool("nlm.plasticity.stdp.enabled", true, "Enable Spike-Timing-Dependent Plasticity")
       ->addDouble("nlm.plasticity.stdp.learning_rate", 0.001, "STDP learning rate")
       ->addBool("nlm.plasticity.hebbian.enabled", true, "Enable Hebbian learning")
       ->addBool("nlm.neuromodulation.curiosity.enabled", true, "Enable curiosity-driven exploration")
       ->addDouble("nlm.neuromodulation.curiosity.weight", 0.5, "Weight of curiosity in exploration");

// Add Phase 6 specific configuration
auto phase6Builder = config->getBuilder();
phase6Builder->addInt("phase6.max_steps", 10000, "Maximum simulation steps")
           ->addBool("phase6.enable_checkpointing", true, "Enable checkpoint save/load")
           ->addBool("phase6.enable_replay", true, "Enable replay system")
           ->addBool("phase6.enable_development", true, "Enable development system")
           ->addString("phase6.checkpoint_path", "/tmp/nlm_checkpoints", "Path for checkpoint files");

// Build the configuration
auto finalConfig = builder->build();
auto phase6Config = phase6Builder->build();
```

### Section Management

Configuration sections help organize related parameters:

```cpp
auto builder = config->getBuilder();

// Begin brain configuration section
builder->beginSection("brain")
    ->addInt("neuron_count", 1000, "Total number of neurons")
    ->addDouble("synapse_density", 0.1, "Synapse connectivity density")
    ->addDouble("connection_probability", 0.05, "Connection probability")
    ->addDouble("learning_rate", 0.001, "Learning rate")
    ->endSection();

// Begin plasticity configuration section
builder->beginSection("plasticity")
    ->beginSection("stdp")
        ->addBool("enabled", true, "Enable STDP")
        ->addDouble("learning_rate", 0.001, "STDP learning rate")
        ->endSection()
    ->beginSection("hebbian")
        ->addBool("enabled", true, "Enable Hebbian learning")
        ->endSection()
    ->endSection();
```

## Configuration Validation

### Built-in Validation Rules

The configuration system includes built-in validation rules for common parameter types:

```cpp
// Type validation
InputValidator::validateInt("param", value, 0, 1000);    // Range validation
InputValidator::validateString("param", value, "^[a-zA-Z0-9_]+$"); // Pattern validation
InputValidator::validateChoice("param", value, {"option1", "option2", "option3"}); // Choice validation
InputValidator::validatePositive("param", value);        // Positive value
InputValidator::validateNonEmpty("param", value);        // Non-empty string

// Neural system specific validation
InputValidator::validateNeuronCount("neuron_count", neuronCount);
InputValidator::validateRegionCount("region_count", regionCount);
InputValidator::validateConnectionProbability("connection_probability", prob);
InputValidator::validateTimestep("timestep", timestep);
```

### Cross-Validation

Cross-validation ensures parameter combinations are valid:

```cpp
// Example: neuron_count and region_count must be consistent
auto crossValidation = [](const ConfigValue& neuronCount, const ConfigValue& regionCount) {
    if (std::get<int64_t>(neuronCount) < std::get<int64_t>(regionCount)) {
        return ValidationResult(false, "Neuron count must be greater than region count");
    }
    return ValidationResult(true);
};

// Add cross-validation rule
validator->addCrossValidation("neuron_count", "region_count", crossValidation);
```

### Validation Configuration

Validation can be configured with different strictness levels:

```cpp
auto validationConfig = std::make_shared<ValidationConfig>();
validationConfig->strictMode = true;                    // Fail on any validation error
validationConfig->warnOnDeprecation = true;             // Warn on deprecated parameters
validationConfig->requireAllFields = false;             // Allow missing optional fields
validationConfig->customRules = customRules;            // Add custom validation rules

config->validate(validationConfig);
```

## Configuration Access

### Type-Safe Access Methods

The configuration system provides type-safe access methods:

```cpp
auto config = createConfiguration();

// Type-safe get with default values
int64_t neuronCount = config->getOrTyped<int64_t>("nlm.brain.neuron_count", 1000);
double learningRate = config->getTyped<double>("nlm.brain.learning_rate");
bool enableSTDP = config->getOrTyped<bool>("nlm.plasticity.stdp.enabled", true);
std::string checkpointPath = config->getOrTyped<std::string>("phase6.checkpoint_path", "/tmp");

// Type-safe get with validation
auto neuronCountOpt = config->getTyped<int64_t>("nlm.brain.neuron_count");
if (neuronCountOpt) {
    // Valid access
    InputValidator::validateNeuronCount("neuron_count", *neuronCountOpt);
} else {
    // Handle missing parameter
}

// Original backward-compatible access (still supported)
int64_t neuronCount2 = config->getOr<int64_t>("nlm.brain.neuron_count", 1000);
double learningRate2 = config->get<double>("nlm.brain.learning_rate");
```

### Section Access

Access configuration values within sections:

```cpp
// Access brain section values
if (config->has("brain")) {
    auto brainSection = config->getSection("brain");
    int64_t neuronCount = brainSection->getOr<int64_t>("neuron_count", 1000);
    double synapseDensity = brainSection->getOr<double>("synapse_density", 0.1);
}

// Nested section access
if (config->has("nlm.plasticity.stdp")) {
    auto stdpSection = config->getSection("nlm.plasticity.stdp");
    bool enabled = stdpSection->getOr<bool>("enabled", true);
    double learningRate = stdpSection->getOr<double>("learning_rate", 0.001);
}
```

## Configuration File I/O

### Loading Configuration

```cpp
auto config = nlm::createConfiguration();

// Load from JSON file
if (!config->loadFromFile("config.json")) {
    std::cerr << "Failed to load JSON configuration" << std::endl;
    return 1;
}

// Load from YAML file
if (!config->loadFromFile("config.yaml")) {
    std::cerr << "Failed to load YAML configuration" << std::endl;
    return 1;
}

// Load from simple format
if (!config->loadFromFile("config.txt")) {
    std::cerr << "Failed to load simple configuration" << std::endl;
    return 1;
}

// Load from string
std::string jsonStr = R"({"nlm": {"brain": {"neuron_count": 1000}}})";
if (!config->loadFromJSON(jsonStr)) {
    std::cerr << "Failed to parse JSON string" << std::endl;
    return 1;
}

std::string yamlStr = "nlm:\n  brain:\n    neuron_count: 1000";
if (!config->loadFromYAML(yamlStr)) {
    std::cerr << "Failed to parse YAML string" << std::endl;
    return 1;
}
```

### Saving Configuration

```cpp
auto config = createConfiguration();

// Save to JSON file
if (!config->saveToFile("output.json")) {
    std::cerr << "Failed to save JSON configuration" << std::endl;
    return 1;
}

// Save to YAML file
if (!config->saveToFile("output.yaml")) {
    std::cerr << "Failed to save YAML configuration" << std::endl;
    return 1;
}

// Get JSON string
std::string jsonStr;
if (config->saveToJSON(jsonStr)) {
    std::cout << "JSON Configuration:" << std::endl;
    std::cout << jsonStr << std::endl;
}

// Get YAML string
std::string yamlStr;
if (config->saveToYAML(yamlStr)) {
    std::cout << "YAML Configuration:" << std::endl;
    std::cout << yamlStr << std::endl;
}
```

## Configuration Change Notifications

### Adding Change Callbacks

```cpp
auto config = createConfiguration();

// Add lambda callback
config->addChangeCallback([](const ConfigChange& change) {
    std::cout << "Configuration changed: " << change.key 
              << " from " << change.oldValue 
              << " to " << change.newValue 
              << " (source: " << static_cast<int>(change.source) << ")" 
              << " at " << change.timestamp << std::endl;
});

// Add function callback
void onConfigChange(const ConfigChange& change) {
    std::cout << "Configuration updated: " << change.key << std::endl;
    // React to configuration change
}

config->addChangeCallback(onConfigChange);

// Remove callback (if you have a reference)
config->removeChangeCallback(onConfigChange);
```

### Configuration Change Tracking

```cpp
// Example demonstrating configuration change tracking
auto config = createConfiguration();
config->set("nlm.brain.neuron_count", int64_t(500));
config->set("nlm.brain.learning_rate", 0.01);

// The callbacks will be triggered for each change
// ConfigChange objects contain:
// - key: Parameter name
// - oldValue: Previous value
// - newValue: New value  
// - source: ConfigSource enum (Default, File, CommandLine, etc.)
// - timestamp: When the change occurred
```

## Phase 6 Configuration

### Phase 6 Specific Parameters

Phase 6 introduces dedicated configuration options for integration:

```cpp
// Phase 6 configuration builder
auto phase6Config = createConfiguration();
auto phase6Builder = phase6Config->getBuilder();

// Integration configuration
phase6Builder->addInt("max_steps", 10000, "Maximum simulation steps")
           ->addBool("enable_checkpointing", true, "Enable checkpoint save/load")
           ->addBool("enable_replay", true, "Enable replay system")
           ->addBool("enable_development", true, "Enable development system")
           ->addBool("enable_memory_systems", true, "Enable memory system integration")
           ->addBool("enable_prediction_system", true, "Enable prediction system integration")
           ->addBool("enable_cognitive_systems", true, "Enable cognitive system integration")
           ->addString("checkpoint_path", "/tmp/nlm_checkpoints", "Path for checkpoint files")
           ->addDouble("checkpoint_interval", 1000.0, "Steps between checkpoints")
           ->addInt("replay_window_size", 1000, "Number of steps to replay");

// Build Phase 6 configuration
auto finalConfig = phase6Builder->build();
```

### Integration Configuration Guidelines

Phase 6 configuration should focus on integration rather than adding new features:

1. **Memory Systems**: Enable, disable, or configure memory integration
2. **Prediction System**: Configure temporal and causal prediction
3. **Cognitive Systems**: Enable attention, planning, self-model, social learning
4. **Neuromodulation**: Configure dopamine, curiosity, novelty effects
5. **Development**: Configure developmental stages and maturation
6. **Persistence**: Configure checkpoint save/load and replay

## Configuration Examples

### Complete Example

```cpp
#include "nlm/core/Config.hpp"
#include <iostream>

int main() {
    try {
        // Create configuration
        auto config = nlm::createConfiguration();
        
        // Load from file with error handling
        if (!config->loadFromFile("config.json")) {
            std::cerr << "Failed to load configuration" << std::endl;
            return 1;
        }
        
        // Add configuration using builder
        auto builder = config->getBuilder();
        builder->addInt("nlm.brain.neuron_count", 1000, "Number of neurons")
               ->addDouble("nlm.brain.learning_rate", 0.001, "Learning rate")
               ->addBool("nlm.plasticity.enabled", true, "Enable plasticity")
               ->addBool("phase6.enable_checkpointing", true, "Enable checkpoints");
        
        // Build configuration
        auto finalConfig = builder->build();
        
        // Use configuration
        int64_t neurons = finalConfig->getOrTyped<int64_t>("nlm.brain.neuron_count", 1000);
        double learningRate = finalConfig->getTyped<double>("nlm.brain.learning_rate");
        
        std::cout << "Configuration loaded successfully:" << std::endl;
        std::cout << "  Neurons: " << neurons << std::endl;
        std::cout << "  Learning Rate: " << learningRate << std::endl;
        
    } catch (const nlm::ConfigError& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### JSON File Example

```json
{
  "nlm": {
    "brain": {
      "neuron_count": 10000,
      "synapse_density": 0.05,
      "connection_probability": 0.01,
      "learning_rate": 0.0005,
      "plasticity": {
        "stdp": {
          "enabled": true,
          "learning_rate": 0.0005
        },
        "hebbian": {
          "enabled": true
        }
      }
    },
    "neuromodulation": {
      "curiosity": {
        "enabled": true,
        "weight": 0.3
      },
      "novelty": {
        "enabled": true
      }
    }
  },
  "phase6": {
    "enable_checkpointing": true,
    "checkpoint_path": "/tmp/nlm_checkpoints",
    "max_steps": 50000,
    "enable_replay": true,
    "enable_memory_systems": true,
    "enable_prediction_system": true,
    "enable_cognitive_systems": true
  }
}
```

## Configuration Best Practices

### 1. Use Configuration Builder for New Code

```cpp
// ✅ Recommended: Use builder pattern for new configurations
auto config = createConfiguration();
auto builder = config->getBuilder();
builder->addInt("param1", 100, "Description")
       ->addBool("param2", true, "Description");
auto finalConfig = builder->build();
```

```cpp
// ❌ Not recommended: Direct parameter setting
auto config = createConfiguration();
config->set("param1", 100);
config->set("param2", true);
```

### 2. Validate Critical Parameters

```cpp
// Always validate critical configuration parameters
if (config->has("nlm.brain.neuron_count")) {
    int64_t neuronCount = config->getTyped<int64_t>("nlm.brain.neuron_count");
    InputValidator::validateNeuronCount("neuron_count", neuronCount);
}
```

### 3. Use Change Callbacks for Reactive Configuration

```cpp
// Use change callbacks for runtime configuration updates
config->addChangeCallback([](const ConfigChange& change) {
    if (change.key == "nlm.brain.learning_rate") {
        // Update learning rate in neural systems
        updateLearningRate(std::get<double>(change.newValue));
    }
});
```

### 4. Save Configuration for Reproducibility

```cpp
// Save configuration for experiment reproducibility
config->saveToFile("experiment_config.json");

// Load configuration in new experiments
auto experimentConfig = createConfiguration();
experimentConfig->loadFromFile("experiment_config.json");
```

### 5. Document Configuration Parameters

```cpp
// Always document configuration parameters
builder->addInt("param_name", default_value, 
               "Description of what this parameter controls and valid range");

// Use clear, descriptive names
builder->addDouble("learning_rate", 0.001, "STDP learning rate (0.0-1.0)");
builder->addInt("max_iterations", 1000, "Maximum training iterations");
```

## Configuration Migration Guide

### From Simple Config to New System

```cpp
// Old way (still supported)
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);

// New way (recommended for new code)
auto config = createConfiguration();
auto builder = config->getBuilder();
builder->addInt("neuron_count", 1000, "Number of neurons");
auto finalConfig = builder->build();
int64_t neuronCount = finalConfig->getOrTyped<int64_t>("neuron_count", 0);
```

## Error Handling

### Configuration Error Types

```cpp
// Configuration errors include:
struct ConfigError : public NLMError {
    enum class Code {
        InvalidParameter = 1000,
        MissingRequiredParameter = 1001,
        TypeMismatch = 1002,
        FileLoadFailed = 1003,
        FileParseFailed = 1004,
        ValidationFailed = 1005,
        SchemaMismatch = 1006
    };
};
```

### Error Handling in Configuration Code

```cpp
auto config = createConfiguration();

try {
    if (!config->loadFromFile("config.json")) {
        throw ConfigError("Failed to load configuration", "loadFromFile", 
                         ConfigError::Code::FileLoadFailed);
    }
    
    auto builder = config->getBuilder();
    builder->addInt("param", 100, "Description");
    // ... more configuration
    
    auto finalConfig = builder->build();
    
    // Validate configuration
    auto validationResult = finalConfig->validate();
    if (!validationResult.valid) {
        throw ConfigError("Configuration validation failed: " + validationResult.errorMessage,
                         "validate", ConfigError::Code::ValidationFailed);
    }
    
} catch (const ConfigError& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    return 1;
}
```

## Performance Considerations

### Thread Safety

All configuration operations are thread-safe:

```cpp
// Configuration operations can be called from multiple threads
std::thread t1([&]() {
    config->set("param1", 100);
});

std::thread t2([&]() {
    config->set("param2", 200);
});

t1.join();
t2.join();
// No race conditions
```

### Memory Management

```cpp
// Configuration is designed for efficient memory usage
// - Lazy validation (only performed when explicitly requested)
// - Change callbacks use weak references to avoid memory leaks
// - Builder pattern avoids unnecessary object creation
```

### Configuration Validation Performance

```cpp
// Validation is cached for performance
auto validationResult1 = config->validate();
// First validation: computes result

auto validationResult2 = config->validate();
// Second validation: uses cached result

// To force re-validation:
auto validationResult3 = config->validate(true);  // Force re-validation
```

## Future Enhancements

The configuration system provides a foundation for future enhancements:

1. **Advanced Validation**: Rule-based validation with complex expressions
2. **Configuration Schema**: JSON Schema generation for configuration files
3. **Environment Variables**: Support for environment variable configuration
4. **Configuration Persistence**: Save/load configuration state
5. **Distributed Configuration**: Support for distributed configuration management
6. **Hot Configuration Reloading**: Reload configuration files without restarting
7. **Configuration Encryption**: Secure configuration management
8. **Audit Logging**: Track configuration changes for compliance

## Conclusion

The NLM configuration system provides a comprehensive, flexible, and extensible approach to managing configuration. It supports multiple formats (JSON, YAML, simple key=value), provides type-safe access, comprehensive validation, change notifications, and Phase 6 integration configuration. The system is fully backward compatible while offering advanced features for new code.

Key benefits:

1. **Extensibility**: Easy to add new configuration options without modifying existing code
2. **Type Safety**: Type-safe configuration access reduces runtime errors
3. **Validation**: Configuration validation prevents invalid parameter combinations
4. **Multiple Formats**: JSON, YAML, and simple formats supported
5. **Change Notifications**: Real-time notification of configuration changes
6. **Builder Pattern**: Clean, declarative configuration definition
7. **Phase 6 Integration**: Dedicated configuration for Phase 6 integration
8. **Backward Compatibility**: Existing code continues to work without changes

The configuration system is production-ready and provides a solid foundation for NLM experiments and research.