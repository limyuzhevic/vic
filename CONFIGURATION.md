# Configuration Management System for NLM

## Overview

This document describes the configuration management system for the NLM (Neural Learning Machine) project. It provides standardized patterns for configuration management, validation, and error handling across all components.

## Configuration Architecture

### Configuration Types

1. **System Configuration** - Core brain parameters and simulation settings
2. **Component Configuration** - Specific to individual components (memory, neuromodulation, etc.)
3. **Runtime Configuration** - Dynamic settings that can change during execution
4. **Environment Configuration** - File paths, logging levels, output destinations

### Configuration Pattern

```cpp
// Example of standardized configuration pattern
struct ComponentConfig {
    // Core configuration parameters
    int64_t neuronCount = 1000;
    float connectionProbability = 0.1f;
    double simulationTimestep = 0.001;
    
    // Component-specific parameters
    float plasticityLTPWeight = 0.01f;
    float plasticityLTDWeight = 0.012f;
    float stdpTau = 20.0f;
    
    // Validation functions
    bool validate() const {
        return neuronCount > 0 && connectionProbability >= 0.0f && 
               simulationTimestep > 0.0f && plasticityLTPWeight >= 0.0f;
    }
};
```

## Configuration Files

### Core Configuration Files

1. **configs/default.cfg** - Default configuration settings
2. **configs/development.cfg** - Development environment settings
3. **configs/production.cfg** - Production environment settings
4. **configs/benchmark.cfg** - Performance testing settings

### Configuration File Format

```ini
# NLM Configuration File Format

# Core Simulation Parameters
[simulation]
neuron_count = 1000
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42

# Neural Dynamics
[neural]
neuron_count = 1000
connection_probability = 0.1

# Plasticity Parameters
[plasticity]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory Systems
[memory]
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
associative_memory_strength_threshold = 0.3

# Neuromodulation
[neuromodulation]
dopamine_strength = 1.0
curiosity_threshold = 0.3
novelty_decay_rate = 0.99

# Development
[development]
enable_development = true
developmental_stage = initial
plasticity_modifier_rate = 0.1

# Logging
[logging]
level = info
file_path = ./logs/nlm.log
console_output = true
max_file_size = 10485760

# Performance
[performance]
checkpoint_interval = 1000
replay_interval = 100
consolidation_interval = 10000
```

## Configuration Validation

### Validation Framework

```cpp
class ConfigValidator {
public:
    static bool validate(const Config& config, std::vector<std::string>& errors);
    static bool validateMemoryConfig(const MemoryConfig& config);
    static bool validateNeuralConfig(const NeuralConfig& config);
    static bool validatePlasticityConfig(const PlasticityConfig& config);
    static std::string getValidationErrorMessage(ConfigError error);
};

enum class ConfigError {
    NONE,
    INVALID_VALUE,
    MISSING_REQUIRED_PARAM,
    OUT_OF_RANGE,
    CONFLICTING_PARAMS,
    CORRUPTED_FILE
};
```

### Validation Rules

1. **Range Validation** - Parameters must be within valid ranges
2. **Dependency Validation** - Parameters must satisfy dependencies
3. **Uniqueness Validation** - IDs must be unique
4. **Format Validation** - Parameters must follow expected formats
5. **Consistency Validation** - Parameters must be internally consistent

## Configuration Loading

### Configuration Loading Patterns

```cpp
class ConfigLoader {
public:
    static std::shared_ptr<Config> loadFromFile(const std::string& path);
    static std::shared_ptr<Config> loadFromCommandLine(int argc, char** argv);
    static std::shared_ptr<Config> loadFromEnvironment();
    static std::shared_ptr<Config> mergeConfigs(const std::vector<std::shared_ptr<Config>>& configs);
    
    // Configuration sources
    enum class Source {
        DEFAULT,
        FILE,
        COMMAND_LINE,
        ENVIRONMENT
    };
};
```

### Configuration Priority

1. **Environment Variables** - Highest priority
2. **Command Line Arguments** - High priority
3. **Configuration File** - Medium priority
4. **Default Values** - Lowest priority

## Configuration Error Handling

### Error Handling Patterns

```cpp
class ConfigErrorHandler {
public:
    static void handleError(ConfigError error, const std::string& message);
    static std::string getErrorMessage(ConfigError error);
    static bool isFatalError(ConfigError error);
    static void logError(const std::string& component, ConfigError error, const std::string& details);
};
```

### Common Configuration Errors

1. **InvalidFormatError** - Configuration file has incorrect format
2. **MissingParameterError** - Required parameter is missing
3. **InvalidValueError** - Parameter value is invalid
4. **ConflictError** - Parameters have conflicting values
5. **DeprecatedParameterError** - Parameter is deprecated
6. **CircularDependencyError** - Configuration has circular dependencies

## Configuration API

### Core Configuration Interface

```cpp
class Config {
public:
    // Get configuration value
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int64_t value);
    void set(const std::string& key, double value);
    void set(const std::string& key, float value);
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration sources
    bool hasSource(Source source) const;
    Source getSource(const std::string& key) const;
    
    // Configuration modification
    void merge(const Config& other);
    void clear();
    
    // Configuration persistence
    bool save(const std::string& path) const;
    bool load(const std::string& path);
    
    // Configuration introspection
    std::vector<std::string> getKeys() const;
    std::string getValueType(const std::string& key) const;
    bool hasKey(const std::string& key) const;
    
protected:
    // Internal configuration storage
    std::unordered_map<std::string, ConfigValue> values_;
    std::unordered_map<std::string, Source> sources_;
    std::vector<std::string> validationErrors_;
};
```

### Configuration Value Types

```cpp
class ConfigValue {
public:
    enum class Type {
        STRING,
        INT64,
        DOUBLE,
        FLOAT,
        BOOL,
        VECTOR_STRING,
        VECTOR_INT64,
        VECTOR_DOUBLE,
        VECTOR_FLOAT
    };
    
    ConfigValue(const std::string& value);
    ConfigValue(int64_t value);
    ConfigValue(double value);
    ConfigValue(float value);
    ConfigValue(bool value);
    ConfigValue(const std::vector<std::string>& value);
    ConfigValue(const std::vector<int64_t>& value);
    ConfigValue(const std::vector<double>& value);
    ConfigValue(const std::vector<float>& value);
    
    Type getType() const { return type_; }
    const std::string& getString() const;
    int64_t getInt64() const;
    double getDouble() const;
    float getFloat() const;
    bool getBool() const;
    const std::vector<std::string>& getStringVector() const;
    const std::vector<int64_t>& getInt64Vector() const;
    const std::vector<double>& getDoubleVector() const;
    const std::vector<float>& getFloatVector() const;
    
    std::string toString() const;
    
private:
    Type type_;
    union Value {
        std::string str;
        int64_t i64;
        double dbl;
        float flt;
        bool b;
        std::vector<std::string> strVec;
        std::vector<int64_t> i64Vec;
        std::vector<double> dblVec;
        std::vector<float> fltVec;
        
        Value() : str("") {}
        ~Value() {}
    } value_;
};
```

## Configuration Examples

### Example 1: Simple Configuration

```cpp
// main.cpp
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    auto config = std::make_shared<Config>();
    
    // Set configuration values
    config->set("neuron_count", 1000LL);
    config->set("simulation_timestep", 0.001);
    config->set("connection_probability", 0.1f);
    
    // Get configuration values
    int64_t neurons = config->getOr<int64_t>("neuron_count", 500);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    float probability = config->getOr<float>("connection_probability", 0.1f);
    
    // Validate configuration
    if (!config->validate()) {
        auto errors = config->getValidationErrors();
        for (const auto& error : errors) {
            std::cerr << "Configuration error: " << error << std::endl;
        }
        return 1;
    }
    
    // Use configuration
    std::cout << "Configuration: " << neurons << " neurons, " 
              << timestep << " timestep, " 
              << probability << " connection probability" << std::endl;
    
    return 0;
}
```

### Example 2: Configuration File Loading

```cpp
// config_loader.cpp
#include "core/Config/Config.hpp"
#include <iostream>

std::shared_ptr<Config> loadConfig(const std::string& path) {
    auto config = ConfigLoader::loadFromFile(path);
    if (!config) {
        std::cerr << "Failed to load configuration from " << path << std::endl;
        return nullptr;
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: program <config_file>" << std::endl;
        return 1;
    }
    
    auto config = loadConfig(argv[1]);
    if (!config) {
        return 1;
    }
    
    // Process configuration
    std::cout << "Configuration loaded successfully" << std::endl;
    
    return 0;
}
```

### Example 3: Command Line Configuration

```cpp
// command_line_config.cpp
#include "core/Config/Config.hpp"
#include <iostream>

void printHelp() {
    std::cout << "Usage: program [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neuron_count <n>     Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --simulation_timestep <t>  Simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --connection_probability <p>  Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --config <file>         Configuration file path" << std::endl;
    std::cout << "  --help                  Show help message" << std::endl;
}

int main(int argc, char* argv[]) {
    auto config = std::make_shared<Config>();
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "--config" && i + 1 < argc) {
            auto fileConfig = ConfigLoader::loadFromFile(argv[++i]);
            if (fileConfig) {
                config->merge(*fileConfig);
            }
        } else if (arg.find("--") == 0) {
            // Parse key=value pairs
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                config->set(key, value);
            }
        }
    }
    
    // Process configuration
    int64_t neurons = config->getOr<int64_t>("neuron_count", 1000);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    float probability = config->getOr<float>("connection_probability", 0.1f);
    
    std::cout << "Configuration from command line:" << std::endl;
    std::cout << "  neuron_count: " << neurons << std::endl;
    std::cout << "  simulation_timestep: " << timestep << std::endl;
    std::cout << "  connection_probability: " << probability << std::endl;
    
    return 0;
}
```

## Configuration Best Practices

### 1. Use Constants for Magic Numbers

```cpp
// Good
const double DEFAULT_SIMULATION_TIMESTEP = 0.001;
const float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
const int64_t DEFAULT_NEURON_COUNT = 1000;

// Use in configuration
config->set("simulation_timestep", DEFAULT_SIMULATION_TIMESTEP);
config->set("connection_probability", DEFAULT_CONNECTION_PROBABILITY);
config->set("neuron_count", DEFAULT_NEURON_COUNT);
```

### 2. Validate Configuration Early

```cpp
// Always validate configuration early
if (!config->validate()) {
    auto errors = config->getValidationErrors();
    // Handle validation errors
    return 1;
}
```

### 3. Use Descriptive Configuration Keys

```cpp
// Good
config->set("working_memory_capacity", 100);
config->set("episodic_memory_max_episodes", 1000);
config->set("plasticity_stdp_ltp_weight", 0.01f);

// Bad
config->set("mem_cap", 100);           // Too short
config->set("pl_ltp", 0.01f);         // Too abbreviated
config->set("param123", 1000);       // Not descriptive
```

### 4. Provide Sensible Defaults

```cpp
// Always provide sensible defaults
config->set("neuron_count", 1000);  // Default value
config->set("simulation_timestep", 0.001);  // Default timestep
config->set("connection_probability", 0.1f);  // Default connection probability
```

### 5. Document Configuration Parameters

```cpp
// Document configuration parameters
void documentConfiguration() {
    std::cout << "Configuration Parameters:" << std::endl;
    std::cout << "  neuron_count: Number of neurons in the simulation" << std::endl;
    std::cout << "  simulation_timestep: Simulation time step" << std::endl;
    std::cout << "  connection_probability: Probability of connection between neurons" << std::endl;
    // ... more documentation
}
```

## Conclusion

The configuration management system provides a standardized, robust framework for managing configuration across the NLM project. It includes:

1. **Consistent patterns** for configuration management
2. **Comprehensive validation** of configuration parameters
3. **Flexible loading** from multiple sources
4. **Robust error handling** for configuration issues
5. **Comprehensive documentation** for all configuration parameters
6. **Best practices** for configuration management

This system ensures that configuration is handled consistently across all components of the NLM project, making it easier to maintain, test, and deploy the system.