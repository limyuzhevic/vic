# NLM Configuration System Documentation

**Complete reference for the NLM configuration system**

## Overview

The NLM configuration system provides a flexible, hierarchical parameter management framework for controlling all aspects of neural simulation. It enables users to fine-tune brain behavior, plasticity rules, neuromodulation, development processes, and environmental interactions through a unified configuration interface.

## Configuration Architecture

### Configuration System Overview

The NLM configuration system is designed with the following key principles:

#### Hierarchical Structure
- **Global keys**: Top-level configuration parameters
- **Nested keys**: Parameter grouping (e.g., `brain.neuron_count`, `plasticity.stdp.learning_rate`)
- **Type-safe access**: Strong typing for different parameter types
- **Validation**: Built-in validation for parameter values
- **Persistence**: File I/O support for configuration saving/loading

```cpp
// Hierarchical configuration example
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.001,
                "tau_plus": 20.0,
                "tau_minus": 20.0
            },
            "hebbian": {
                "enable": true,
                "learning_rate": 0.0005
            }
        },
        "neuromodulation": {
            "dopamine": {
                "scale": 1.0
            },
            "curiosity": {
                "enable": true,
                "exploration_rate": 0.1
            }
        }
    },
    "development": {
        "system": {
            "enable": true,
            "plasticity_rate": 1.0
        }
    }
}
```

#### Configuration Types

| Type | Syntax | Example | Description |
|------|--------|---------|-------------|
| String | `key = "value"` | `brain.name = "hippocampus"` | Text parameters |
| Integer | `key = value` | `brain.neuron_count = 1000` | Whole number parameters |
| Float | `key = value` | `brain.synapse_density = 0.1` | Decimal parameters |
| Boolean | `key = value` | `plasticity.stdp.enable = true` | True/false parameters |

## Core Configuration Classes

### Config Class (`nlm/core/config.hpp`)

The `Config` class is the primary interface for configuration management in NLM.

#### Config Class Definition

```cpp
class Config {
public:
    // Constructors
    Config();
    Config(const std::string& filename);
    
    // Key-value operations
    bool has(const std::string& key) const;
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    // Configuration key management
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& prefix) const;
    std::vector<std::string> getKeys(const std::string& prefix, const std::string& suffix) const;
    
    // File I/O operations
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    
    // Utility operations
    bool isEmpty() const;
    size_t size() const;
    void clear();
    std::string toString() const;
};
```

#### Config Class Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Config()` | Default constructor | None | `Config` |
| `Config(const std::string& filename)` | Constructor with file loading | `filename: std::string` | `Config` |
| `has(const std::string& key)` | Check if key exists | `key: std::string` | `bool` |
| `set(const std::string& key, const std::string& value)` | Set string value | `key: std::string, value: std::string` | None |
| `set(const std::string& key, int value)` | Set integer value | `key: std::string, value: int` | None |
| `set(const std::string& key, double value)` | Set float value | `key: std::string, value: double` | None |
| `set(const std::string& key, bool value)` | Set boolean value | `key: std::string, value: bool` | None |
| `getString(const std::string& key)` | Get string value | `key: std::string` | `std::string` |
| `getInt(const std::string& key)` | Get integer value | `key: std::string` | `int` |
| `getDouble(const std::string& key)` | Get float value | `key: std::string` | `double` |
| `getBool(const std::string& key)` | Get boolean value | `key: std::string` | `bool` |
| `getKeys()` | Get all configuration keys | None | `std::vector<std::string>` |
| `getKeys(const std::string& prefix)` | Get keys with given prefix | `prefix: std::string` | `std::vector<std::string>` |
| `getKeys(const std::string& prefix, const std::string& suffix)` | Get keys with prefix and suffix | `prefix: std::string, suffix: std::string` | `std::vector<std::string>` |
| `saveToFile(const std::string& filename)` | Save configuration to file | `filename: std::string` | None |
| `loadFromFile(const std::string& filename)` | Load configuration from file | `filename: std::string` | None |
| `isEmpty()` | Check if configuration is empty | None | `bool` |
| `size()` | Get number of configuration entries | None | `size_t` |
| `clear()` | Clear all configuration entries | None | None |
| `toString()` | Convert configuration to string | None | `std::string` |

## Configuration Organization

### Configuration Key Patterns

The NLM configuration system uses a hierarchical key structure with the following patterns:

#### Brain Configuration

```cpp
namespace BrainConfigKeys {
    static constexpr const char* NEURON_COUNT = "brain.neuron_count";
    static constexpr const char* SYNAPSE_DENSITY = "brain.synapse_density";
    static constexpr const char* CONNECTION_PROBABILITY = "brain.connection_probability";
    static constexpr const char* INITIAL_WEIGHT_MEAN = "brain.initial_weight_mean";
    static constexpr const char* INITIAL_WEIGHT_STD = "brain.initial_weight_std";
    static constexpr const char* V_THRESH = "brain.v_thresh";
    static constexpr const char* V_REST = "brain.v_rest";
    static constexpr const char* V_RESET = "brain.v_reset";
    static constexpr const char* TAU_MEM = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char*TAU_REF = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_ref";
}
```

#### Plasticity Configuration

```cpp
namespace PlasticityConfigKeys {
    static constexpr const char* STDP_ENABLE = "plasticity.stdp.enable";
    static constexpr const char* STDP_LEARNING_RATE = "plasticity.stdp.learning_rate";
    static constexpr const char* STDP_TAU_PLUS = "plasticity.stdp.tau_plus";
    static constexpr const char* STDP_TAU_MINUS = "plasticity.stdp.tau_minus";
    static constexpr const char* HEBBIAN_ENABLE = "plasticity.hebbian.enable";
    static constexpr const char* HEBBIAN_LEARNING_RATE = "plasticity.hebbian.learning_rate";
    static constexpr const char* STRUCTURAL_ENABLE = "plasticity.structural.enable";
    static constexpr const char* STRUCTURAL_RATE = "plasticity.structural.rate";
    static constexpr const char* STRUCTURAL_MAX_DENSITY = "plasticity.structural.max_density";
}
```

#### Neuromodulation Configuration

```cpp
namespace NeuromodConfigKeys {
    static constexpr const char* DOPAMINE_SCALE = "neuromod.dopamine.scale";
    static constexpr const char* CURIOUSITY_ENABLE = "neuromod.curiosity.enable";
    static constexpr const char* CURIOUSITY_EXPLORATION_RATE = "neuromod.curiosity.exploration_rate";
    static constexpr const char* NOVELTY_ENABLE = "neuromod.novelty.enable";
    static constexpr const char* NOVELTY_THRESHOLD = "neuromod.novelty.threshold";
    static constexpr const char* NOVELTY_SENSITIVITY = "neuromod.novelty.sensitivity";
    static constexpr const char* PREDICTION_ERROR_SCALE = "neuromod.prediction_error.scale";
    static constexpr const char* PREDICTION_ERROR_SENSITIVITY = "neuromod.prediction_error.sensitivity";
}
```

#### Development Configuration

```cpp
namespace DevelopmentConfigKeys {
    static constexpr const char* SYSTEM_ENABLE = "development.system.enable";
    static constexpr const char* PLASTICITY_RATE = "development.plasticity_rate";
    static constexpr const char* CRITICAL_PERIOD = "development.critical_period";
    static constexpr const char* STAGE_DURATION_INITIAL = "development.stage_duration.initial";
    static constexpr const char* STAGE_DURATION_CRITICAL = "development.stage_duration.critical";
    static constexpr const char* STAGE_DURATION_MATURATION = "development.stage_duration.maturation";
    static constexpr const char* STAGE_DURATION_ADULT = "development.stage_duration.adult";
    static constexpr const char* STAGE_DURATION_AGING = "development.stage_duration.aging";
    static constexpr const char* SYNAPTOGENESIS_RATE = "development.synaptogenesis.rate";
    static constexpr const char* PRUNING_THRESHOLD = "development.pruning.threshold";
    static constexpr const char* MATURATION_FACTOR = "development.maturation.factor";
}
```

## Configuration Factory Functions

### Factory Functions

The NLM configuration system provides factory functions for creating commonly used configuration presets.

```cpp
namespace NLM {
    // Factory functions for configuration creation
    Config createDefaultConfig();                    // Standard configuration for general use
    Config createAdvancedConfig();                   // Advanced configuration with all features enabled
    Config createSimpleConfig();                     // Simple configuration for beginners
    Config createExperimentConfig();                  // Optimized configuration for experimental use
    Config createPerformanceConfig();                 // Optimized for performance
    Config createResearchConfig();                    // Optimized for research
    
    // Configuration validation
    bool validateConfig(const Config& config);         // Validate configuration
    std::vector<std::string> getValidationErrors(const Config& config); // Get validation errors
}
```

#### Factory Function Descriptions

| Function | Description | Typical Use Case |
|----------|-------------|------------------|
| `createDefaultConfig()` | Standard configuration with balanced settings | General purpose simulation |
| `createAdvancedConfig()` | Advanced configuration with all features | Research and development |
| `createSimpleConfig()` | Minimal configuration for beginners | Quick testing and learning |
| `createExperimentConfig()` | Optimized for experimental control | Scientific experiments |
| `createPerformanceConfig()` | Optimized for performance | Large-scale simulations |
| `createResearchConfig()` | Optimized for research accuracy | Research and analysis |

#### Factory Function Examples

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>

void demonstrateFactoryFunctions() {
    // Default configuration (general purpose)
    auto default_config = NLM::createDefaultConfig();
    default_config.set("brain.neuron_count", 1000);
    
    // Advanced configuration (research)
    auto advanced_config = NLM::createAdvancedConfig();
    advanced_config.set("brain.neuron_count", 5000);
    advanced_config.set("plasticity.stdp.learning_rate", 0.002);
    advanced_config.set("neuromod.curiosity.exploration_rate", 0.2);
    
    // Simple configuration (beginners)
    auto simple_config = NLM::createSimpleConfig();
    simple_config.set("brain.neuron_count", 100);
    
    // Experiment configuration (scientific)
    auto experiment_config = NLM::createExperimentConfig();
    experiment_config.set("brain.neuron_count", 2000);
    experiment_config.set("development.system.enable", true);
    experiment_config.set("development.critical_period", 50.0);
}
```

## Configuration File Formats

### Supported File Formats

The NLM configuration system supports multiple file formats for configuration persistence:

#### JSON Format

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
```

#### XML Format

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Config>
    <brain>
        <neuron_count>1000</neuron_count>
        <synapse_density>0.1</synapse_density>
        <v_thresh>-50.0</v_thresh>
        <v_rest>-70.0</v_rest>
    </brain>
    <plasticity>
        <stdp>
            <enable>true</enable>
            <learning_rate>0.001</learning_rate>
        </stdp>
    </plasticity>
</Config>
```

#### Binary Format

```cpp
// Binary format uses efficient serialization
struct ConfigBinary {
    uint32_t version;
    uint32_t entry_count;
    struct Entry {
        uint16_t key_length;
        char key[KEY_LENGTH_MAX];
        ValueType type;
        union Value {
            char string_value[STRING_LENGTH_MAX];
            int32_t int_value;
            double double_value;
            bool bool_value;
        } value;
    } entries[ENTRY_COUNT_MAX];
};
```

### File I/O Examples

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <fstream>

void demonstrateFileIO() {
    // Create configuration
    auto config = NLM::createDefaultConfig();
    config.set("brain.neuron_count", 1000);
    config.set("plasticity.stdp.enable", true);
    config.set("neuromod.dopamine.scale", 1.0);
    
    // Save to JSON file
    config.saveToFile("brain_config.json");
    
    // Load from JSON file
    auto loaded_config = NLM::createDefaultConfig();
    loaded_config.loadFromFile("brain_config.json");
    
    // Save to XML file
    config.saveToFile("brain_config.xml");
    
    // Load from XML file
    auto loaded_xml_config = NLM::createDefaultConfig();
    loaded_xml_config.loadFromFile("brain_config.xml");
    
    std::cout << "Configuration loaded successfully!" << std::endl;
    std::cout << "Neurons: " << loaded_config.getInt("brain.neuron_count") << std::endl;
    std::cout << "STDP enabled: " << (loaded_config.getBool("plasticity.stdp.enable") ? "yes" : "no") << std::endl;
}
```

## Configuration Validation

### Validation Functions

The NLM configuration system provides validation functions to ensure configuration correctness.

```cpp
namespace NLM {
    // Configuration validation
    bool validateConfig(const Config& config);
    std::vector<std::string> getValidationErrors(const Config& config);
    
    // Parameter validation
    bool validateParameter(const std::string& key, const std::string& value);
    bool validateParameter(const std::string& key, int value);
    bool validateParameter(const std::string& key, double value);
    bool validateParameter(const std::string& key, bool value);
}
```

#### Validation Rules

| Validation Rule | Description | Error Message |
|----------------|-------------|---------------|
| Required Keys | Check if all required keys are present | "Missing required key: {key}" |
| Type Mismatch | Check if value type matches expected type | "Type mismatch for key {key}: expected {expected_type}, got {actual_type}" |
| Value Range | Check if value is within valid range | "Value out of range for key {key}: {value} not in [{min}, {max}]" |
| Dependency Check | Check if dependent parameters are consistent | "Dependency violation for key {key}: {dependency_key} must be {required_value}" |

#### Validation Example

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>

void demonstrateValidation() {
    // Create configuration
    auto config = NLM::createDefaultConfig();
    config.set("brain.neuron_count", 1000);
    config.set("plasticity.stdp.enable", true);
    
    // Validate configuration
    if (NLM::validateConfig(config)) {
        std::cout << "Configuration is valid!" << std::endl;
    } else {
        std::cout << "Configuration validation failed:" << std::endl;
        auto errors = NLM::getValidationErrors(config);
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Validate specific parameter
    if (!NLM::validateParameter("brain.neuron_count", 0)) {
        std::cout << "Invalid neuron count: cannot be zero" << std::endl;
    }
}
```

## Configuration Templates

### Configuration Templates

The NLM configuration system provides templates for common configuration scenarios.

#### Brain Configuration Templates

```cpp
namespace NLM {
    // Brain configuration templates
    Config brainTemplateBeginner();      // For beginners
    Config brainTemplateResearch();       // For research
    Config brainTemplatePerformance();   // For performance
    Config brainTemplateDevelopment();   // For development
    
    // Plasticity configuration templates
    Config plasticityTemplateSTDP();     // For STDP learning
    Config plasticityTemplateHebbian();  // For Hebbian learning
    Config plasticityTemplateHybrid();   // For hybrid learning
    
    // Neuromodulation configuration templates
    Config neuromodTemplateBasic();      // Basic neuromodulation
    Config neuromodTemplateAdvanced();   // Advanced neuromodulation
    
    // Development configuration templates
    Config developmentTemplateStandard(); // Standard development
    Config developmentTemplateCritical(); // Critical period focus
}
```

#### Template Examples

```cpp
void demonstrateTemplates() {
    // Brain template for beginners
    auto beginner_brain = NLM::brainTemplateBeginner();
    beginner_brain.set("brain.neuron_count", 100);
    
    // Brain template for research
    auto research_brain = NLM::brainTemplateResearch();
    research_brain.set("brain.neuron_count", 5000);
    research_brain.set("brain.synapse_density", 0.15);
    
    // Plasticity template for STDP
    auto stdp_plasticity = NLM::plasticityTemplateSTDP();
    stdp_plasticity.set("plasticity.stdp.learning_rate", 0.001);
    stdp_plasticity.set("plasticity.stdp.tau_plus", 20.0);
    
    // Neuromodulation template for advanced
    auto advanced_neuromod = NLM::neuromodTemplateAdvanced();
    advanced_neuromod.set("neuromod.curiosity.exploration_rate", 0.2);
    advanced_neuromod.set("neuromod.novelty.threshold", 0.3);
}
```

## Configuration Best Practices

### Recommended Practices

#### Naming Conventions

1. **Use consistent naming**: Follow the `category.parameter` pattern
2. **Descriptive names**: Use descriptive parameter names
3. **Case sensitivity**: Configuration keys are case-sensitive
4. **Avoid spaces**: Don't use spaces in parameter keys

#### Configuration Management

1. **Use factory functions**: Start with factory functions for common configurations
2. **Validate configurations**: Always validate configurations before use
3. **Save configurations**: Save important configurations to files
4. **Use environment variables**: Consider environment variables for configuration
5. **Document configurations**: Document configuration choices

#### Performance Considerations

1. **Minimize configuration changes**: Reduce runtime configuration changes
2. **Use efficient data types**: Choose appropriate data types for parameters
3. **Cache configurations**: Cache frequently used configurations
4. **Use light configurations**: Start with simple configurations for debugging

### Common Configuration Patterns

#### Pattern 1: Standard Configuration
```cpp
auto config = NLM::createDefaultConfig();
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);
```

#### Pattern 2: Research Configuration
```cpp
auto config = NLM::createAdvancedConfig();
config.set("brain.neuron_count", 5000);
config.set("brain.synapse_density", 0.15);
config.set("plasticity.stdp.learning_rate", 0.002);
config.set("neuromod.curiosity.exploration_rate", 0.2);
config.set("development.system.enable", true);
config.set("development.critical_period", 75.0);
```

#### Pattern 3: Performance Configuration
```cpp
auto config = NLM::createPerformanceConfig();
config.set("brain.neuron_count", 2000);
config.set("brain.synapse_density", 0.05);
config.set("plasticity.stdp.enable", false);
config.set("development.system.enable", false);
```

## Configuration Migration

### Configuration Migration Guide

This guide covers migration from older NLM configuration versions.

#### Version Migration

| From Version | To Version | Changes |
|--------------|------------|---------|
| v0.1.x | v0.2.0 | New key naming, new factory functions |
| v0.2.0 | v0.3.0 | New configuration validation, new templates |
| v0.3.0 | v0.4.0 | New configuration file formats, enhanced factory functions |

#### Migration Examples

```cpp
void migrateFromV0_1_ToV0_2() {
    // v0.1.x configuration
    auto old_config;
    old_config.set("neuron_count", 1000);                    // Old key
    old_config.set("stdp_learning_rate", 0.001);             // Old key
    
    // Migrate to v0.2.0
    auto new_config = NLM::createDefaultConfig();
    if (old_config.has("neuron_count")) {
        new_config.set("brain.neuron_count", old_config.getInt("neuron_count"));
    }
    if (old_config.has("stdp_learning_rate")) {
        new_config.set("plasticity.stdp.learning_rate", old_config.getDouble("stdp_learning_rate"));
    }
}
```

## Configuration Utilities

### Utility Functions

The NLM configuration system provides utility functions for common configuration tasks.

```cpp
namespace NLM {
    // Configuration comparison
    bool compareConfigs(const Config& config1, const Config& config2);
    
    // Configuration merging
    Config mergeConfigs(const Config& config1, const Config& config2);
    Config mergeConfigs(const std::vector<Config>& configs);
    
    // Configuration transformation
    Config transformConfig(const Config& config, const std::function<bool(const std::string&)>& filter);
    Config transformConfig(const Config& config, const std::function<void(std::string&)>& transformer);
    
    // Configuration export
    std::string exportConfigToJSON(const Config& config);
    std::string exportConfigToXML(const Config& config);
    std::vector<uint8_t> exportConfigToBinary(const Config& config);
    
    // Configuration import
    Config importConfigFromJSON(const std::string& json_string);
    Config importConfigFromXML(const std::string& xml_string);
    Config importConfigFromBinary(const std::vector<uint8_t>& binary_data);
}
```

#### Utility Examples

```cpp
void demonstrateUtilities() {
    // Create two configurations
    auto config1 = NLM::createDefaultConfig();
    config1.set("brain.neuron_count", 1000);
    
    auto config2 = NLM::createAdvancedConfig();
    config2.set("brain.neuron_count", 2000);
    config2.set("plasticity.stdp.enable", true);
    
    // Compare configurations
    if (NLM::compareConfigs(config1, config2)) {
        std::cout << "Configurations are identical" << std::endl;
    } else {
        std::cout << "Configurations differ" << std::endl;
    }
    
    // Merge configurations
    auto merged_config = NLM::mergeConfigs(config1, config2);
    std::cout << "Merged neurons: " << merged_config.getInt("brain.neuron_count") << std::endl;
    
    // Export to JSON
    std::string json_output = NLM::exportConfigToJSON(config1);
    std::cout << "JSON output: " << json_output << std::endl;
    
    // Import from JSON
    auto imported_config = NLM::importConfigFromJSON(json_output);
}
```

## Troubleshooting

### Common Configuration Issues

#### "Configuration file not found"

**Cause**: Configuration file doesn't exist.

**Solution**:
```cpp
try {
    config.loadFromFile("config.json");
} catch (const std::exception& e) {
    std::cerr << "Configuration file not found: " << e.what() << std::endl;
    // Use default configuration
    config = NLM::createDefaultConfig();
}
```

#### "Invalid configuration value"

**Cause**: Configuration value has wrong type or is out of range.

**Solution**:
```cpp
if (!config.has("brain.neuron_count")) {
    std::cerr << "Missing required parameter: brain.neuron_count" << std::endl;
    config.set("brain.neuron_count", 1000); // Use default
}

if (config.getInt("brain.neuron_count") <= 0) {
    std::cerr << "Invalid neuron count: must be positive" << std::endl;
    config.set("brain.neuron_count", 1000); // Use default
}
```

#### "Configuration validation failed"

**Cause**: Configuration doesn't meet validation requirements.

**Solution**:
```cpp
if (!NLM::validateConfig(config)) {
    auto errors = NLM::getValidationErrors(config);
    for (const auto& error : errors) {
        std::cerr << "Validation error: " << error << std::endl;
    }
    // Fix configuration
    config.set("brain.neuron_count", 1000);
}
```

#### "Configuration too large"

**Cause**: Configuration exceeds memory limits.

**Solution**:
```cpp
if (config.size() > 1000) {
    std::cerr << "Configuration too large, reducing size" << std::endl;
    // Remove unnecessary keys
    config.clear();
    config.set("brain.neuron_count", 100); // Use simple configuration
}
```

## Version Information

### Current Configuration Version
v0.4.0

### Supported File Formats
- JSON (.json)
- XML (.xml)
- Binary (.bin)

### Dependencies
- None (header-only library)
- Optional: nlohmann/json for JSON support

### Build System
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## References

### Related Documentation
- [NLM Python API](docs/api/python-api.md)
- [NLM C++ API](docs/api/cpp-api.md)
- [System Architecture](docs/architecture/architecture.md)
- [Performance Optimization](docs/best-practices/performance.md)
- [Tutorial: Configuration](docs/tutorial/configuration.md)

### Research Papers
- [Biological Fidelity Documentation](docs/SCIENCE.md)
- [Phase 6 Final Results](docs/PHASE6_FINAL_RESULTS.md)
- [Research Methods](docs/RESEARCH_METHODS.md)

## Changelog

### v0.4.0 (Current)
- Added configuration validation system
- Added configuration templates
- Added utility functions
- Enhanced file I/O support
- Improved documentation

### v0.3.0
- Added factory functions
- Added configuration key constants
- Enhanced validation
- Improved error handling

### v0.2.0
- Initial configuration system
- Basic configuration management
- Simple file I/O

### v0.1.0
- Initial configuration implementation
- Basic key-value storage

## Contact Information

### Issues and Feedback
- **GitHub Repository**: [nlm-project/nlm](https://github.com/nlm-project/nlm)
- **Documentation Issues**: Submit issues through repository
- **Configuration Questions**: Check configuration documentation
- **Technical Support**: Consult system architecture documentation

### Support Channels
- **Documentation Hub**: Current documentation index
- **Quick Start Guide**: [easy_usage.md](easy_usage.md)
- **Technical Details**: [HOW_TO_USE.md](HOW_TO_USE.md)
- **Community Resources**: [SCIENCE.md](docs/SCIENCE.md)

---

*This documentation is continuously updated. Please report any issues or provide feedback to help improve the configuration system documentation.*

**Last Updated**: v0.4.0 - Enhanced configuration system documentation**

---

## Configuration System Quick Reference

### Core Functions

```cpp
// Configuration creation
NLM::Config createDefaultConfig();
NLM::Config createAdvancedConfig();
NLM::Config createSimpleConfig();
NLM::Config createExperimentConfig();

// Configuration operations
bool has(const std::string& key);
void set(const std::string& key, const std::string& value);
void set(const std::string& key, int value);
void set(const std::string& key, double value);
void set(const std::string& key, bool value);

std::string getString(const std::string& key);
int getInt(const std::string& key);
double getDouble(const std::string& key);
bool getBool(const std::string& key);

// Configuration management
std::vector<std::string> getKeys();
std::vector<std::string> getKeys(const std::string& prefix);
void saveToFile(const std::string& filename);
void loadFromFile(const std::string& filename);

// Validation and utilities
bool validateConfig(const Config& config);
std::vector<std::string> getValidationErrors(const Config& config);
Config mergeConfigs(const Config& config1, const Config& config2);
```

### Configuration Key Constants

```cpp
namespace BrainConfigKeys {
    static constexpr const char* NEURON_COUNT = "brain.neuron_count";
    static constexpr const char* SYNAPSE_DENSITY = "brain.synapse_density";
    // ... other keys
}

namespace PlasticityConfigKeys {
    static constexpr const char* STDP_ENABLE = "plasticity.stdp.enable";
    static constexpr const char* STDP_LEARNING_RATE = "plasticity.stdp.learning_rate";
    // ... other keys
}

namespace NeuromodConfigKeys {
    static constexpr const char* DOPAMINE_SCALE = "neuromod.dopamine.scale";
    static constexpr const char* CURIOUSITY_ENABLE = "neuromod.curiosity.enable";
    // ... other keys
}
```

### Configuration File Formats

```cpp
// JSON format
{
    "brain.neuron_count": 1000,
    "plasticity.stdp.enable": true,
    "neuromod.dopamine.scale": 1.0
}

// XML format
<Config>
    <brain>
        <neuron_count>1000</neuron_count>
    </brain>
    <plasticity>
        <stdp>
            <enable>true</enable>
        </stdp>
    </plasticity>
</Config>
```

This comprehensive configuration system documentation provides complete reference for NLM's configuration management, including core classes, factory functions, file I/O, validation, templates, utilities, and troubleshooting. The configuration system is designed for flexibility, ease of use, and performance with comprehensive support for all NLM simulation needs.