# NLM Environment-Based Configuration

## Overview

The NLM Environment-Based Configuration system provides automatic configuration loading from environment variables, enabling flexible and dynamic configuration management. This system allows users to override configuration values using environment variables, making it ideal for:

- Container orchestration (Docker, Kubernetes)
- Development environments
- Deployment-specific configurations
- CI/CD pipeline configurations
- Multi-environment deployments

## Key Features

- **Automatic Environment Variable Detection**: Automatically detects configuration keys in environment variables
- **Configurable Prefix**: Customizable environment variable prefix for isolation
- **Type Conversion**: Automatic type conversion based on configuration key patterns
- **Validation Integration**: Validates environment variable values against configuration schema
- **Hot-Swapping**: Dynamic configuration updates during runtime
- **Fallback Mechanism**: Graceful handling of missing or invalid environment variables
- **Documentation Generation**: Automatic generation of environment variable documentation

## Environment Variable System

### Environment Variable Format

```bash
# Basic format: NLM_[SECTION]_[KEY]
NLM_SIMULATION_NEURON_COUNT=1000
NLM_SIMULATION_TIMESTEP=0.001
NLM_PLASTICITY_LEARNING_RATE=0.01
NLM_ENVIRONMENT_NAME=GridWorld
NLM_ENVIRONMENT_WIDTH=10
NLM_ENVIRONMENT_HEIGHT=10
```

### Hierarchical Environment Variables

```bash
# Support for hierarchical configuration
NLM_SIMULATION_NEURON_COUNT=1000
NLM_SIMULATION_REGION_COUNT=2
NLM_SIMULATION_CONNECTION_PROBABILITY=0.1
NLM_PLASTICITY_STDP_LTP_WEIGHT=0.01
NLM_PLASTICITY_STDP_LTD_WEIGHT=0.012
NLM_DEVELOPMENT_SYNAPTAGENESIS_RATE=0.001
NLM_DEVELOPMENT_PRUNING_RATE=0.0001
```

### Specialized Configuration Variables

```bash
# Development-specific
NLM_DEBUG_MODE=true
NLM_ENABLE_VALIDATION=true
NLM_LOG_LEVEL=DEBUG
NLM_PERFORMANCE_PROFILE=true

# Performance-specific
NLM_PARALLEL_NEURONS=true
NLM_VECTORIZATION_ENABLED=true
NLM_MEMORY_POOL_SIZE=100000

# Output and logging
NLM_LOG_TO_FILE=true
NLM_LOG_FILENAME=nlm.log
NLM_PERFORMANCE_OUTPUT=performance.json

# Environment configuration
NLM_ENVIRONMENT_TYPE=GridWorld
NLM_ENVIRONMENT_SEED=42
NLM_ENVIRONMENT_RANDOMNESS=0.1
```

## Core Classes

### EnvironmentConfigLoader Class

```cpp
class EnvironmentConfigLoader {
public:
    // Construction and lifecycle
    EnvironmentConfigLoader();
    explicit EnvironmentConfigLoader(const std::string& prefix);
    ~EnvironmentConfigLoader();
    
    // Core loading functions
    bool loadEnvironmentConfig(std::shared_ptr<Config> config);
    bool loadEnvironmentConfig(std::shared_ptr<Config> config, 
                              const std::vector<std::string>& keys);
    bool loadEnvironmentConfig(std::shared_ptr<Config> config, 
                              const ConfigSchema& schema);
    
    // Environment variable management
    void setEnvironmentPrefix(const std::string& prefix);
    std::string getEnvironmentPrefix() const;
    
    // Environment variable inspection
    std::vector<std::string> getEnvironmentVariables() const;
    std::vector<std::string> getMatchingEnvironmentVariables(const std::string& pattern);
    
    // Configuration validation
    bool validateEnvironmentVariables(const std::vector<std::string>& keys);
    std::vector<ValidationError> validateEnvironmentVariables(const ConfigSchema& schema);
    
    // Documentation
    std::string generateEnvironmentDocumentation(const std::vector<std::string>& keys);
    static std::string generateEnvironmentDocumentation(const Config& config);
    
    // State management
    void clearEnvironmentCache();
    void setEnvironmentCacheEnabled(bool enabled);
    bool isEnvironmentCacheEnabled() const;
};
```

### EnvironmentVariableParser Class

```cpp
class EnvironmentVariableParser {
public:
    // Construction and lifecycle
    EnvironmentVariableParser();
    ~EnvironmentVariableParser();
    
    // Parsing functions
    ConfigValue parseEnvironmentVariable(const std::string& key,
                                        const std::string& value);
    std::map<std::string, ConfigValue> parseEnvironmentVariables(const std::vector<std::string>& keys);
    std::map<std::string, ConfigValue> parseEnvironmentVariables(const std::vector<std::string>& keys,
                                                                const ConfigSchema& schema);
    
    // Type conversion
    template<typename T>
    T convertToType(const std::string& value);
    
    // Pattern matching
    bool matchesKeyPattern(const std::string& key, const std::string& pattern);
    std::vector<std::string> findMatchingEnvironmentVariables(const std::string& pattern);
    
    // Validation
    bool validateEnvironmentVariable(const std::string& key, 
                                     const std::string& value,
                                     const TypeInfo& typeInfo);
    std::vector<ValidationError> validateEnvironmentVariables(const std::map<std::string, std::string>& envVars,
                                                              const ConfigSchema& schema);
};
```

### EnvironmentConfigMonitor Class

```cpp
class EnvironmentConfigMonitor {
public:
    // Construction and lifecycle
    EnvironmentConfigMonitor();
    explicit EnvironmentConfigMonitor(std::shared_ptr<Config> config);
    ~EnvironmentConfigMonitor();
    
    // Monitoring functions
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Callback management
    void addEnvironmentChangeCallback(const EnvironmentChangeCallback& callback);
    void removeEnvironmentChangeCallback(const EnvironmentChangeCallback& callback);
    void clearEnvironmentChangeCallbacks();
    
    // Monitoring state
    std::vector<EnvironmentChange> getRecentChanges() const;
    size_t getChangeCount() const;
    
    // Configuration management
    void updateConfigFromEnvironment();
    bool hasEnvironmentConfigChanged() const;
    
    // State management
    void saveState();
    void restoreState();
    void clearHistory();
};
```

## Environment Variable System Implementation

### Environment Variable Prefix Management

```cpp
// Environment variable prefix configuration
class EnvironmentPrefixManager {
public:
    static const std::string DEFAULT_PREFIX;
    
    // Set global environment prefix
    static void setGlobalPrefix(const std::string& prefix);
    
    // Get global environment prefix
    static std::string getGlobalPrefix();
    
    // Convert configuration key to environment variable name
    static std::string configKeyToEnvVar(const std::string& key);
    
    // Convert environment variable name to configuration key
    static std::string envVarToConfigKey(const std::string& envVar);
};
```

### Environment Variable Type Conversion

```cpp
class EnvironmentVariableTypeConverter {
public:
    // Integer conversion
    static int convertToInt(const std::string& value);
    static int64_t convertToInt64(const std::string& value);
    
    // Floating-point conversion
    static float convertToFloat(const std::string& value);
    static double convertToDouble(const std::string& value);
    
    // Boolean conversion
    static bool convertToBool(const std::string& value);
    
    // String conversion
    static std::string convertToString(const std::string& value);
    
    // Vector conversion
    static std::vector<int> convertToVectorInt(const std::string& value);
    static std::vector<double> convertToVectorDouble(const std::string& value);
    static std::vector<std::string> convertToVectorString(const std::string& value);
    
    // Helper functions
    static bool isInteger(const std::string& value);
    static bool isFloat(const std::string& value);
    static bool isBool(const std::string& value);
    static bool isString(const std::string& value);
};
```

## Environment Variable Loading Strategies

### Automatic Loading

```cpp
// Automatic loading of all environment variables
bool loadAllEnvironmentVariables(std::shared_ptr<Config> config);

// Load only matching environment variables
bool loadMatchingEnvironmentVariables(std::shared_ptr<Config> config,
                                      const std::string& pattern);

// Load environment variables with schema validation
bool loadValidatedEnvironmentVariables(std::shared_ptr<Config> config,
                                       const ConfigSchema& schema);
```

### Selective Loading

```cpp
// Load specific configuration keys from environment
bool loadConfigKeysFromEnvironment(std::shared_ptr<Config> config,
                                   const std::vector<std::string>& keys);

// Load environment variables with priority
bool loadEnvironmentVariablesWithPriority(std::shared_ptr<Config> config,
                                         const std::map<std::string, int>& priorities);
```

### Real-time Monitoring

```cpp
// Real-time environment variable monitoring
class RealTimeEnvironmentMonitor {
public:
    void startMonitoring(const std::vector<std::string>& keys);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Callback for environment variable changes
    void onEnvironmentVariableChange(const std::string& key, const std::string& value);
};
```

## Configuration Templates

### Development Environment

```bash
# Environment variables for development
NLM_DEBUG_MODE=true
NLM_ENABLE_VALIDATION=true
NLM_LOG_LEVEL=DEBUG
NLM_PERFORMANCE_PROFILE=true
NLM_SIMULATION_NEURON_COUNT=100
NLM_SIMULATION_TIMESTEP=0.001
NLM_ENVIRONMENT_NAME=GridWorld
NLM_ENVIRONMENT_WIDTH=5
NLM_ENVIRONMENT_HEIGHT=5
```

### Testing Environment

```bash
# Environment variables for testing
NLM_DEBUG_MODE=false
NLM_ENABLE_VALIDATION=false
NLM_LOG_LEVEL=WARNING
NLM_PERFORMANCE_PROFILE=false
NLM_SIMULATION_NEURON_COUNT=50
NLM_SIMULATION_TIMESTEP=0.01
NLM_PLASTICITY_LEARNING_RATE=0.1
NLM_ENVIRONMENT_NAME=GridWorld
NLM_ENVIRONMENT_WIDTH=3
NLM_ENVIRONMENT_HEIGHT=3
```

### Production Environment

```bash
# Environment variables for production
NLM_DEBUG_MODE=false
NLM_ENABLE_VALIDATION=true
NLM_LOG_LEVEL=INFO
NLM_PERFORMANCE_PROFILE=true
NLM_SIMULATION_NEURON_COUNT=10000
NLM_SIMULATION_TIMESTEP=0.001
NLM_PLASTICITY_LEARNING_RATE=0.01
NLM_ENVIRONMENT_NAME=GridWorld
NLM_ENVIRONMENT_WIDTH=20
NLM_ENVIRONMENT_HEIGHT=20
```

## Integration with Configuration System

### Environment Variable Loader Integration

```cpp
// Integrate environment variable loading with existing configuration system
class ConfigWithEnvironment {
public:
    ConfigWithEnvironment() {
        loader_ = std::make_shared<EnvironmentConfigLoader>("NLM_");
        config_ = std::make_shared<Config>();
        monitor_ = std::make_shared<EnvironmentConfigMonitor>(config_);
    }
    
    bool loadFromEnvironment(const std::vector<std::string>& keys) {
        return loader_->loadEnvironmentConfig(config_, keys);
    }
    
    bool loadValidatedFromEnvironment(const ConfigSchema& schema) {
        return loader_->loadEnvironmentVariables(config_, schema);
    }
    
    void startMonitoring() {
        monitor_->startMonitoring();
    }
    
    std::shared_ptr<Config> getConfig() {
        return config_;
    }
    
private:
    std::shared_ptr<EnvironmentConfigLoader> loader_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<EnvironmentConfigMonitor> monitor_;
};
```

### Configuration Management with Environment Variables

```cpp
// Enhanced configuration management with environment variables
class EnhancedConfigManager {
public:
    EnhancedConfigManager() {
        environmentLoader_ = std::make_shared<EnvironmentConfigLoader>();
    }
    
    bool loadConfiguration(const std::string& configFile, 
                          const std::vector<std::string>& envKeys = {}) {
        // Load from file
        if (!config_->loadFromFile(configFile)) {
            return false;
        }
        
        // Override with environment variables
        if (!envKeys.empty()) {
            if (!environmentLoader_->loadEnvironmentConfig(config_, envKeys)) {
                return false;
            }
        }
        
        return true;
    }
    
    bool validateConfiguration(const ConfigSchema& schema) {
        return environmentLoader_->validateEnvironmentVariables(schema);
    }
    
    std::shared_ptr<Config> getConfig() {
        return config_;
    }
    
    void startEnvironmentMonitoring(const std::vector<std::string>& keys) {
        environmentMonitor_->startMonitoring(keys);
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<EnvironmentConfigLoader> environmentLoader_;
    std::shared_ptr<EnvironmentConfigMonitor> environmentMonitor_;
};
```

## Usage Examples

### Basic Environment Variable Loading

```cpp
#include <nlm/config/EnvironmentConfigLoader.hpp>

int main() {
    // Create environment loader
    auto environmentLoader = std::make_shared<nlm::EnvironmentConfigLoader>("MYAPP_");
    
    // Load configuration from environment variables
    auto config = std::make_shared<nlm::Config>();
    std::vector<std::string> keys = {
        "SIMULATION_NEURON_COUNT",
        "SIMULATION_TIMESTEP",
        "PLASTICITY_LEARNING_RATE"
    };
    
    if (environmentLoader->loadEnvironmentConfig(config, keys)) {
        std::cout << "Configuration loaded from environment variables" << std::endl;
        
        // Get configuration values
        int neurons = config->getOr<int64_t>("simulation.neuron_count", 1000);
        double timestep = config->getOr<double>("simulation.timestep", 0.001);
        float learningRate = config->getOr<float>("plasticity.learning_rate", 0.01);
        
        std::cout << "Neuron count: " << neurons << std::endl;
        std::cout << "Timestep: " << timestep << std::endl;
        std::cout << "Learning rate: " << learningRate << std::endl;
        
    } else {
        std::cerr << "Failed to load configuration from environment variables" << std::endl;
    }
    
    return 0;
}
```

### Environment Variable Monitoring

```cpp
#include <nlm/config/EnvironmentConfigMonitor.hpp>

void environmentChangeCallback(const std::string& key, const std::string& value) {
    std::cout << "Environment variable changed: " << key << " = " << value << std::endl;
    
    // Update configuration
    // ...
}

int main() {
    // Create environment monitor
    auto environmentMonitor = std::make_shared<nlm::EnvironmentConfigMonitor>();
    
    // Add callback
    environmentMonitor->addEnvironmentChangeCallback(environmentChangeCallback);
    
    // Start monitoring
    environmentMonitor->startMonitoring();
    
    // ... application logic ...
    
    // Stop monitoring
    environmentMonitor->stopMonitoring();
    
    return 0;
}
```

### Environment Variable Documentation

```cpp
#include <nlm/config/EnvironmentConfigLoader.hpp>

std::string generateDocumentation(const std::vector<std::string>& keys) {
    auto environmentLoader = std::make_shared<nlm::EnvironmentConfigLoader>("NLM_");
    return environmentLoader->generateEnvironmentDocumentation(keys);
}

int main() {
    // Generate documentation
    std::vector<std::string> keys = {
        "SIMULATION_NEURON_COUNT",
        "SIMULATION_TIMESTEP",
        "PLASTICITY_LEARNING_RATE",
        "ENVIRONMENT_NAME",
        "ENVIRONMENT_WIDTH",
        "ENVIRONMENT_HEIGHT"
    };
    
    std::string documentation = generateDocumentation(keys);
    
    std::cout << "Environment Variable Documentation:" << std::endl;
    std::cout << documentation << std::endl;
    
    return 0;
}
```

## Performance Considerations

### Efficient Environment Variable Loading

```cpp
// Cache environment variables for performance
class CachedEnvironmentLoader : public EnvironmentConfigLoader {
public:
    bool loadEnvironmentConfig(std::shared_ptr<Config> config) override {
        // Check cache first
        if (isCacheEnabled() && hasCachedEnvironmentVariables()) {
            applyCachedEnvironmentVariables(config);
            return true;
        }
        
        // Load from environment
        bool success = EnvironmentConfigLoader::loadEnvironmentConfig(config);
        
        // Cache result
        if (success) {
            cacheEnvironmentVariables();
        }
        
        return success;
    }
};
```

### Memory-Efficient Parsing

```cpp
// Efficient type conversion
class MemoryEfficientEnvironmentParser : public EnvironmentVariableParser {
public:
    std::map<std::string, ConfigValue> parseEnvironmentVariables(const std::vector<std::string>& keys,
                                                                const ConfigSchema& schema) override {
        std::map<std::string, ConfigValue> result;
        result.reserve(keys.size());
        
        for (const auto& key : keys) {
            const char* envValue = std::getenv(convertToEnvVar(key).c_str());
            if (envValue != nullptr) {
                ConfigValue value = parseEnvironmentVariable(key, envValue);
                result[key] = value;
            }
        }
        
        return result;
    }
};
```

## Testing and Validation

### Unit Testing

```cpp
#include <nlm/config/EnvironmentConfigLoader.hpp>
#include <gtest/gtest.h>

TEST(EnvironmentConfigLoaderTest, BasicLoading) {
    auto loader = std::make_shared<nlm::EnvironmentConfigLoader>("TEST_");
    
    // Set environment variable
    setenv("TEST_NEURON_COUNT", "1000", 1);
    
    auto config = std::make_shared<nlm::Config>();
    std::vector<std::string> keys = {"NEURON_COUNT"};
    
    EXPECT_TRUE(loader->loadEnvironmentConfig(config, keys));
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 1000LL);
    
    // Clean up
    unsetenv("TEST_NEURON_COUNT");
}

TEST(EnvironmentConfigLoaderTest, TypeConversion) {
    auto loader = std::make_shared<nlm::EnvironmentConfigLoader>("TEST_");
    
    // Set environment variable
    setenv("TEST_FLOAT_VALUE", "3.14", 1);
    setenv("TEST_BOOL_VALUE", "true", 1);
    setenv("TEST_INT_VALUE", "42", 1);
    
    auto config = std::make_shared<nlm::Config>();
    std::vector<std::string> keys = {"FLOAT_VALUE", "BOOL_VALUE", "INT_VALUE"};
    
    EXPECT_TRUE(loader->loadEnvironmentConfig(config, keys));
    EXPECT_DOUBLE_EQ(config->getOr<double>("float_value", 0.0), 3.14);
    EXPECT_TRUE(config->getOr<bool>("bool_value", false));
    EXPECT_EQ(config->getOr<int>("int_value", 0), 42);
    
    // Clean up
    unsetenv("TEST_FLOAT_VALUE");
    unsetenv("TEST_BOOL_VALUE");
    unsetenv("TEST_INT_VALUE");
}
```

### Integration Testing

```bash
// Set environment variables for testing
export NLM_SIMULATION_NEURON_COUNT=100
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_PLASTICITY_LEARNING_RATE=0.01

// Run tests
./test_runner --test environment_config

// Clean up environment
unset NLM_SIMULATION_NEURON_COUNT
unset NLM_SIMULATION_TIMESTEP
unset NLM_PLASTICITY_LEARNING_RATE
```

## Migration Guide

### From Earlier Versions

Environment variable system changes:

- **Version 0.1.0**: Basic environment variable support with simple prefix
- **Version 0.2.0**: Automatic environment variable detection
- **Version 0.3.0**: Type conversion and validation
- **Version 0.4.0**: Real-time monitoring and change detection
- **Version 0.5.0**: Configuration templates and documentation generation
- **Version 0.6.0**: Enhanced integration and performance optimization

### API Changes

- **Environment variable format**: Changed from simple key=value to hierarchical format
- **Type conversion**: Enhanced type conversion with validation
- **Monitoring**: Added real-time environment variable monitoring
- **Documentation**: Added automatic documentation generation
- **Integration**: Enhanced integration with existing configuration system
- **Error handling**: Improved error handling and reporting
- **Performance**: Added caching and optimization for better performance

## References

1. [Environment Variables in C++](https://en.cppreference.com/w/cpp/utility/environment)
2. [Configuration Management](https://en.wikipedia.org/wiki/Configuration_management)
3. [Environment Variable Usage](https://en.wikipedia.org/wiki/Environment_variable)
4. [Type Conversion](https://en.cppreference.com/w/cpp/string/basic_string/stoi)

## See Also

- [Config API](config_api.md) - Configuration system
- [Logger API](logging_api.md) - Logging system
- [Brain API](brain_api.md) - Brain integration
- [Memory API](memory_api.md) - Memory system
- [Neuromodulation API](neuromodulation_api.md) - Neuromodulation system
- [Cognition API](cognition_api.md) - Cognitive system
- [Development API](development_api.md) - Development system
- [Sensory API](sensory_api.md) - Sensory system
- [Motor API](motor_api.md) - Motor system
- [Visualization API](visualization_api.md) - Visualization system
- [Experiments API](experiments_api.md) - Experimental system
- [Agent API](agent_api.md) - Agent system

**Last updated:** October 18, 2026
**Version:** 0.6.0