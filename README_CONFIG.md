# Configuration Management System
# Enhanced Configuration System for NLM Project

## Overview

This document describes the enhanced configuration management system for the NLM (Neural Learning Machine) project. The system provides a robust, flexible, and type-safe configuration framework that supports multiple file formats, command-line arguments, environment variables, and comprehensive validation.

## Key Features

### 1. Multiple File Format Support
- JSON files (.cfg.json, .cfg.json)
- YAML files (.cfg.yaml, .cfg.yml)
- Plain text files (.cfg) for backward compatibility

### 2. Multi-Layer Configuration Sources
- Default configuration templates
- File-based configuration
- Command-line argument overrides
- Environment variable overrides
- Runtime configuration

### 3. Type-Safe Configuration Access
- Template-based get() methods
- Type-safe getOr() methods with defaults
- Support for primitive types (int, int64_t, double, bool, string)
- Support for collection types (vector<int>, vector<double>, vector<string>)

### 4. Configuration Validation
- Custom validators for each configuration key
- Global validation rules
- Required field validation
- Value range validation
- Type validation

### 5. Nested Configuration Support
- Dot notation for nested configuration keys (e.g., "brain.neuron_count")
- Configuration sections and subsections
- Hierarchical configuration management

### 6. Comprehensive Error Handling
- Detailed validation error messages
- Graceful fallback to defaults
- Configuration file parsing errors

## API Design

### Core Configuration Class
```cpp
class Config {
public:
    // Construction
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Loading from various sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    void loadFromEnv(const std::string& prefix = "NLM_");
    
    // Configuration access
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration setting
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime);
    // ... other type setters
    
    // Nested configuration
    void setNested(const std::vector<std::string>& path,
                  const ConfigValue& value,
                  ConfigSource source = ConfigSource::Runtime);
    bool has(const std::string& key) const;
    bool hasNested(const std::vector<std::string>& path) const;
    
    // Configuration management
    void remove(const std::string& key);
    void clear();
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getSections() const;
    
    // Configuration inspection
    std::string summary() const;
    
    // Validation
    bool validate() const;
    std::string validationErrors() const;
    void addValidation(const std::string& key,
                      std::function<bool(const ConfigValue&)> validator);
    
    // Default management
    void setDefaults(const std::vector<ConfigTemplate>& defaults);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    static std::string determineFileFormat(const std::string& filepath);
};
```

### Configuration Value Types
```cpp
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;
```

### Configuration Source
```cpp
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime
};
```

### Configuration Entry
```cpp
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required = false;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), required(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), required(req) {}
};
```

### Configuration Template
```cpp
struct ConfigTemplate {
    std::string key;
    ConfigValue defaultValue;
    ConfigSource defaultSource = ConfigSource::Default;
    std::string description;
    bool required = false;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigTemplate() : key(), defaultSource(ConfigSource::Default), 
                      required(false) {}
    ConfigTemplate(const std::string& k, const ConfigValue& def,
                   const std::string& desc = "", bool req = false)
        : key(k), defaultValue(def), description(desc), required(req) {}
};
```

### Configuration Section
```cpp
struct ConfigSection {
    std::string name;
    std::map<std::string, ConfigValue> values;
    std::map<std::string, ConfigSection> subsections;
    
    ConfigSection() : name() {}
    ConfigSection(const std::string& n) : name(n) {}
    
    // ... various accessor methods for section values
};
```

## Implementation Details

### File Format Detection
```cpp
static std::string determineFileFormat(const std::string& filepath) {
    if (filepath.size() >= 6 && 
        (filepath.substr(filepath.size() - 6) == ".cfg.json" ||
         filepath.substr(filepath.size() - 5) == ".json")) {
        return "json";
    } else if (filepath.size() >= 7 && 
               (filepath.substr(filepath.size() - 7) == ".cfg.yaml" ||
                filepath.substr(filepath.size() - 6) == ".cfg.yml" ||
                filepath.substr(filepath.size() - 5) == ".yaml" ||
                filepath.substr(filepath.size() - 5) == ".yml")) {
        return "yaml";
    } else if (filepath.size() >= 5 &&
               filepath.substr(filepath.size() - 5) == ".cfg") {
        return "simple";
    }
    // Default to simple format for backward compatibility
    return "simple";
}
```

### JSON/YAML Parsing
```cpp
// Parse JSON configuration
void parseJsonObject(const nlohmann::json& obj, 
                    const std::string& prefix,
                    ConfigSource source) {
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        
        if (it.value().is_object()) {
            parseJsonObject(it.value(), key, source);
        } else if (it.value().is_array()) {
            // Handle array values
        } else {
            // Convert to ConfigValue
        }
    }
}
```

### Configuration Validation
```cpp
bool Config::validate() const {
    pImpl->isValid = true;
    pImpl->validationErrors.clear();
    
    // Check for required keys
    for (const auto& entry : pImpl->entries) {
        if (entry.required) {
            requiredKeys.insert(entry.key);
        }
        
        // Run custom validators
        if (entry.validator && !entry.validator(entry.value)) {
            pImpl->isValid = false;
            pImpl->validationErrors += 
                "Validation failed for key: " + entry.key + "\n";
        }
    }
    
    // Check that all required keys are present
    for (const auto& entry : pImpl->entries) {
        if (entry.required && requiredKeys.find(entry.key) == requiredKeys.end()) {
            pImpl->isValid = false;
            pImpl->validationErrors += 
                "Required key missing: " + entry.key + "\n";
        }
    }
    
    return pImpl->isValid;
}
```

### Default Configuration Templates
```cpp
static std::vector<ConfigTemplate> Config::getDefaultTemplates() {
    std::vector<ConfigTemplate> defaults;
    
    // Core configuration defaults
    defaults.emplace_back("random_seed", static_cast<int64_t>(42),
                         "Random seed for reproducible experiments", true);
    defaults.emplace_back("simulation_timestep", 0.001,
                         "Simulation timestep in seconds", true);
    defaults.emplace_back("neuron_count", static_cast<int64_t>(500),
                         "Number of neurons in the brain", true);
    defaults.emplace_back("region_count", static_cast<int64_t>(1),
                         "Number of brain regions", true);
    defaults.emplace_back("connection_probability", 0.15f,
                         "Connection probability for random connectivity", true);
    
    // STDP parameters
    defaults.emplace_back("stdp_ltp_weight", 0.02f, "STDP LTP weight", true);
    defaults.emplace_back("stdp_ltd_weight", 0.015f, "STDP LTD weight", true);
    defaults.emplace_back("stdp_tau", 20.0f, "STDP time constant", true);
    
    // Structural plasticity parameters
    defaults.emplace_back("synaptogenesis_rate", 0.0001f,
                         "Synaptogenesis rate", true);
    defaults.emplace_back("pruning_rate", 0.00001f, "Pruning rate", true);
    
    // Logging defaults
    defaults.emplace_back("log_level", std::string("INFO"), "Log level", true);
    defaults.emplace_back("log_to_file", false, "Log to file", true);
    defaults.emplace_back("log_filename", std::string("nlm.log"),
                         "Log filename", true);
    
    return defaults;
}
```

## Usage Examples

### Basic Usage
```cpp
#include "core/Config/Config.hpp"

int main(int argc, char** argv) {
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Load from file
    config->loadFromFile("configs/default.cfg");
    
    // Load from command line
    config->loadFromArgs(argc, argv);
    
    // Load from environment variables
    config->loadFromEnv("NLM_");
    
    // Access configuration values
    int64_t seed = config->getOr<int64_t>("random_seed", 42);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    int neurons = config->getOr<int64_t>("neuron_count", 500);
    
    // Set configuration values
    config->set("test_value", 123);
    config->set("test_string", std::string("hello"));
    
    // Check if configuration exists
    if (config->has("custom_setting")) {
        // Use the configuration value
    }
    
    // Get configuration summary
    std::cout << config->summary() << std::endl;
    
    // Validate configuration
    if (!config->validate()) {
        std::cout << "Configuration validation errors:" << std::endl;
        std::cout << config->validationErrors() << std::endl;
    }
    
    return 0;
}
```

### Nested Configuration
```cpp
// Set nested configuration using dot notation
config->set("brain.region1.neuron_count", 1000);
config->set("brain.region1.connection_probability", 0.2f);
config->set("brain.region2.neuron_count", 500);

// Access nested configuration
auto region1Neurons = config->get<int64_t>("brain.region1.neuron_count");
auto region2ConnProb = config->get<double>("brain.region2.connection_probability");

// Check nested configuration
if (config->has("brain.region1.neuron_count")) {
    // ...
}
```

### File Format Examples

#### JSON Configuration (.cfg.json)
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "brain": {
        "region1": {
            "neuron_count": 1000,
            "connection_probability": 0.2
        },
        "region2": {
            "neuron_count": 500
        }
    },
    "stdp": {
        "ltp_weight": 0.02,
        "ltd_weight": 0.015,
        "tau": 20.0
    }
}
```

#### YAML Configuration (.cfg.yaml)
```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 500
region_count: 1
connection_probability: 0.15

brain:
  region1:
    neuron_count: 1000
    connection_probability: 0.2
  region2:
    neuron_count: 500

stdp:
  ltp_weight: 0.02
  ltd_weight: 0.015
  tau: 20.0
```

#### Simple Configuration (.cfg)
```cfg
# NLM Configuration File
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 500

# Number of brain regions
region_count = 1

# Connection probability for random connectivity
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Brain regions
brain.region1.neuron_count = 1000
brain.region1.connection_probability = 0.2
brain.region2.neuron_count = 500
```

### Python Bindings
```python
import pynlm

# Create configuration
config = pynlm.Config()

# Load from file
config.loadFromFile("configs/default.cfg")

# Access configuration values
seed = config.getOr("random_seed", 42)
timestep = config.getOr("simulation_timestep", 0.001)

# Set configuration values
config.set("test_value", 123)
config.set("test_string", "hello")

# Check if configuration exists
if config.has("custom_setting"):
    # ...

# Get configuration summary
print(config.summary())
```

## Benefits

1. **Flexibility**: Supports multiple file formats and configuration sources
2. **Type Safety**: Template-based access with compile-time type checking
3. **Validation**: Comprehensive validation with detailed error messages
4. **Backward Compatibility**: Supports existing simple configuration files
5. **Extensibility**: Easy to add new configuration options and validation rules
6. **Nested Configuration**: Support for hierarchical configuration organization
7. **Environment Integration**: Seamless integration with environment variables
8. **Developer-Friendly**: Intuitive API with clear documentation

## Migration Guide

### From Simple Configuration to Enhanced System

#### Old Configuration (.cfg)
```cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
```

#### New Configuration (.cfg.json)
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000
}
```

### Adding New Configuration Options

#### Step 1: Add Configuration Template
```cpp
defaults.emplace_back("new_option", std::string("default_value"),
                     "Description of the new option", false);
```

#### Step 2: Add Validation Rule
```cpp
config->addValidation("new_option", 
    [](const ConfigValue& val) {
        if (std::holds_alternative<std::string>(val)) {
            const auto& str = std::get<std::string>(val);
            return str.length() >= 3 && str.length() <= 100;
        }
        return false;
    });
```

## Conclusion

The enhanced configuration management system provides a robust, flexible, and type-safe foundation for configuring the NLM project. It supports multiple file formats, comprehensive validation, nested configuration, and seamless integration with command-line arguments and environment variables. The system is designed to be backward compatible while offering powerful new features for complex configuration management.

The implementation maintains the simplicity of the original configuration system while adding advanced features that enable more sophisticated configuration management. The template-based type-safe API ensures compile-time type checking while maintaining runtime flexibility.