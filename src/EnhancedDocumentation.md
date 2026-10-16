# NLM Phase 6 Enhanced Documentation

## Overview

This document describes the improvements made to the NLM (Neural Learning Machine) Phase 6 system, focusing on enhanced configuration management, error handling, and usability improvements.

## Key Improvements

### 1. Configuration System Enhancement

#### JSON/YAML Support
The configuration system now supports both JSON and YAML formats in addition to the legacy key=value format:

```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "random_seed": 42,
    "dopamine_strength": 1.0,
    "curiosity_threshold": 0.5,
    "working_memory_size": 100,
    "episodic_memory_capacity": 1000
}
```

#### Legacy Format Support
The system maintains backward compatibility with the simple key=value format:

```
# Configuration file
neuron_count = 1000
region_count = 1
connection_probability = 0.1
random_seed = 42
```

### 2. Enhanced Error Handling and Validation

#### Configuration Validation
All configuration parameters are validated automatically:

- **neuron_count**: Must be between 1 and 100,000
- **region_count**: Must be between 1 and 1,000
- **connection_probability**: Must be between 0.0 and 1.0
- **random_seed**: Must not be zero

#### Safe Configuration Access
New safe access methods prevent crashes from invalid configurations:

```cpp
bool isValid = config.validateParameter("neuron_count", value);
auto validatedValue = config.getWithValidation("neuron_count", 500);
bool tryGetSuccess = config.tryGet("neuron_count", outValue);
```

### 3. Command Line Interface Improvements

Enhanced command line argument parsing with better error messages:

```bash
./nlm \
    --config=config.json \
    --neuron_count=2000 \
    --connection_probability=0.15 \
    --random_seed=12345
```

### 4. Improved Logging and Diagnostics

The enhanced logging system provides:

- Detailed configuration validation reports
- Error messages with context
- Progress tracking for long operations
- System status reporting

## Usage Examples

### Basic Usage

```cpp
#include "core/Config/Config.hpp"

int main() {
    auto config = std::make_shared<Config>();
    
    // Load from JSON file
    if (config->loadFromFile("config.json")) {
        std::cout << "Configuration loaded successfully!" << std::endl;
    }
    
    // Validate configuration
    if (config->validate()) {
        std::cout << "Configuration is valid!" << std::endl;
    } else {
        std::cout << "Configuration validation failed!" << std::endl;
        auto validationResults = config->validateAllParameters();
        for (const auto& [key, isValid] : validationResults) {
            if (!isValid) {
                std::cout << "  Invalid parameter: " << key << std::endl;
            }
        }
    }
    
    // Safe access to configuration values
    size_t neuronCount = config->getWithValidation("neuron_count", 1000);
    double connectionProb = config->getOr("connection_probability", 0.1);
    
    return 0;
}
```

### Error Handling Example

```cpp
#include "core/Config/Config.hpp"

bool safeConfigurationLoad(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    
    // Try JSON first
    if (config->loadFromFile(filepath)) {
        if (config->validate()) {
            return true;
        } else {
            std::cerr << Config::getErrorMessage("validation", "Invalid configuration parameters") << std::endl;
            return false;
        }
    }
    
    // Fall back to legacy format
    std::cerr << Config::getErrorMessage("file_load", "Failed to parse " + filepath) << std::endl;
    return false;
}
```

## Advanced Features

### 1. Configuration Conversion

The system can convert between different configuration formats:

```cpp
// Convert JSON to legacy format
Config legacyConfig;
if (config.loadFromJsonFile("config.json")) {
    legacyConfig.saveToFile("legacy_config.txt");
}
```

### 2. Configuration Comparison

Compare two configuration objects:

```cpp
bool configsAreEqual(const Config& config1, const Config& config2) {
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    if (keys1.size() != keys2.size()) {
        return false;
    }
    
    for (const auto& key : keys1) {
        if (!config1.has(key) || !config2.has(key)) {
            return false;
        }
        if (config1.getSafe(key) != config2.getSafe(key)) {
            return false;
        }
    }
    
    return true;
}
```

### 3. Configuration Templates

Predefined configuration templates for common scenarios:

```cpp
class ConfigTemplates {
public:
    static Config smallDemo() {
        Config config;
        config.set("neuron_count", 100);
        config.set("region_count", 1);
        config.set("connection_probability", 0.05);
        config.set("random_seed", 42);
        return config;
    }
    
    static Config largeExperiment() {
        Config config;
        config.set("neuron_count", 10000);
        config.set("region_count", 4);
        config.set("connection_probability", 0.15);
        config.set("random_seed", 12345);
        return config;
    }
};
```

## Migration Guide

### From Legacy System

If you're migrating from the legacy system (Phase 2), follow these steps:

1. **Update Configuration Files**: Convert legacy key=value files to JSON format
2. **Update Command Line Arguments**: Use `--key=value` format instead of old style
3. **Add Error Handling**: Use the new safe access methods
4. **Validation**: Add configuration validation to catch errors early

### Example Migration

```bash
# Legacy configuration file (old format)
neuron_count = 500
region_count = 1
connection_probability = 0.15
random_seed = 42

# New JSON format
{
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "random_seed": 42
}
```

```bash
# Legacy command line (old format)
./nlm --neuron_count 500 --region_count 1

# New command line format
./nlm --neuron_count=500 --region_count=1
```

## Testing and Validation

### Configuration Testing

```bash
// Test configuration validation
./nlm --config=test_config.json --validate-only

// Test configuration conversion
./nlm --config=input.json --convert-output=legacy.txt

// Test configuration comparison
./nlm --config1=config1.json --config2=config2.json --compare
```

### Error Handling Testing

```bash
// Test error handling with invalid configuration
./nlm --config=invalid.json --test-errors

// Test graceful degradation
./nlm --config=missing.json --fallback-to-default
```

## Performance Considerations

### Memory Usage
- Configuration objects are lightweight
- JSON parsing is memory efficient
- Validation uses minimal additional memory

### Performance Tips
1. **Cache Configuration**: Load configuration once and reuse
2. **Batch Validation**: Validate all parameters at once
3. **Lazy Loading**: Load configuration only when needed

## System Integration

### Integration with Main Application

```cpp
// Enhanced main application with configuration validation
int main(int argc, char** argv) {
    // Initialize configuration with validation
    auto config = std::make_shared<Config>();
    
    // Try to load from file with fallback
    bool configLoaded = Config::parseConfigFileSafely(
        "configs/default.cfg", *config
    );
    
    if (!configLoaded) {
        std::cerr << "Using default configuration" << std::endl;
        // Set up default values
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1);
    }
    
    // Load command line arguments
    Config::parseArgsSafely(argc, argv, *config);
    
    // Validate final configuration
    if (!config->validate()) {
        std::cerr << "Configuration validation failed!" << std::endl;
        return 1;
    }
    
    // Initialize brain with validated configuration
    auto brain = std::make_shared<Brain>(config);
    // ... rest of initialization
}
```

### Integration with Experiments

```cpp
// Enhanced experiment configuration with validation
class ExperimentConfig {
public:
    static Phase6Config createFromConfig(const Config& config) {
        Phase6Config expConfig;
        
        // Get validated configuration values
        expConfig.neuronCount = config.getWithValidation("neuron_count", 1000);
        expConfig.regionCount = config.getWithValidation("region_count", 1);
        expConfig.connectionProbability = config.getWithValidation("connection_probability", 0.1);
        
        // Set experiment parameters
        expConfig.maxSteps = 10000;
        expConfig.enableCheckpointing = true;
        expConfig.enableReplay = true;
        expConfig.enableDevelopment = true;
        
        return expConfig;
    }
};
```

## Future Enhancements

### Planned Features

1. **YAML Support**: Full YAML 1.2 support using external libraries
2. **Configuration Inheritance**: Support for configuration templates and inheritance
3. **Dynamic Configuration**: Runtime configuration modification
4. **Configuration History**: Track configuration changes over time
5. **Web Configuration**: HTTP/REST API for configuration management

### Performance Optimizations

1. **Configuration Caching**: Persistent configuration caching
2. **Parallel Configuration Loading**: Concurrent configuration loading
3. **Configuration Compression**: Compressed configuration files
4. **Configuration Encryption**: Secure configuration storage

## References

1. **JSON for Modern C++**: https://github.com/nlohmann/json
2. **Tencent RapidJSON**: https://github.com/Tencent/rapidjson
3. **SIMDJSON**: https://github.com/simdjson/simdjson
4. **YAML-CPP**: https://github.com/yaml-cpp/yaml-cpp

## License

This documentation is licensed under the MIT License. The NLM project is licensed under the MIT License.

## Copyright

Copyright © 2026 NLM Project. All rights reserved.