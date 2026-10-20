# Enhanced Configuration System

## Overview

This document provides comprehensive information about the enhanced configuration system in the NLM project, implementing all requested features while maintaining backward compatibility.

## Key Features

### 1. JSON/YAML Parser
- **Native JSON support** using nlohmann/json (header-only library)
- **YAML support** (placeholder for future enhancement)
- **Legacy format support** for backward compatibility with simple key=value files

### 2. Improved Type System
- **Automatic type inference** from string values
- **Comprehensive type support**: int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>
- **Type conversion** between different formats

### 3. Enhanced Error Handling
- **Detailed error codes** for different failure scenarios
- **ConfigException** class for exception handling
- **ConfigResult** class for operation status
- **Type mismatch detection** and reporting

### 4. Thread Safety
- **Mutex protection** for all public methods
- **Atomic operations** for concurrent access
- **Thread-safe configuration management**

### 5. Advanced CLI Interface
- **Boolean flags**: `--enable-feature`, `--debug`, etc.
- **Key-value pairs**: `--key=value`, `-key value`
- **Subcommands**: `config show`, `config validate`, `config set`
- **Help system**: `--help` for all commands

### 6. Configuration Validation
- **Basic schema validation**
- **Type and range validation**
- **Warning system** for invalid values
- **Automatic validation** on set operations

### 7. Memory Management
- **RAII principles** throughout
- **Smart pointers** for resource management
- **Automatic cleanup** in destructors

## Migration Guide

### Phase 1 → Phase 2 Migration
Use legacy API methods to transition to the new system:

```cpp
// Legacy API (Phase 1 compatibility)
config->legacyLoadFromFile("config.json");
config->legacySet("key", "value", ConfigSource::Runtime);
std::string value = config->legacyGetOr<std::string>("key", "default");
```

### Phase 2 → Phase 3 Migration
Use new enhanced API for better error handling and features:

```cpp
// Enhanced API (Phase 2+)
ConfigResult result = config->loadFromFile("config.json");
if (result.isSuccess()) {
    std::string value = config->getOr<std::string>("key", "default");
} else {
    std::cerr << "Error: " << result.getMessage() << std::endl;
}
```

## API Reference

### Core Configuration Class

#### Constructors/Destructors
```cpp
Config();                    // Default constructor
~Config();                    // Destructor
Config(Config&&) noexcept;    // Move constructor
```

#### File Operations
```cpp
ConfigResult loadFromFile(const std::string& filepath);    // JSON/YAML format
ConfigResult loadFromJson(const std::string& jsonContent);  // JSON string
ConfigResult loadFromYaml(const std::string& yamlContent);  // YAML string
ConfigResult saveToFile(const std::string& filepath) const; // JSON format
```

#### Argument Operations
```cpp
ConfigResult loadFromArgs(int argc, char** argv);    // Enhanced CLI parsing
ConfigResult processCLI(int argc, char** argv, Config& config);  // Advanced CLI
```

#### Get/Set Operations
```cpp
// Get with error handling
ConfigResult get(const std::string& key, T& outValue) const;
T getOr(const std::string& key, const T& defaultValue) const;

// Set with validation
ConfigResult set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
ConfigResult set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
ConfigResult set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
ConfigResult set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
ConfigResult set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
```

#### Query Operations
```cpp
bool has(const std::string& key) const;                 // Check existence
void remove(const std::string& key);                   // Remove key
std::vector<std::string> getKeys() const;              // Get all keys
void clear();                                         // Clear all
std::string summary() const;                          // Get configuration summary
ConfigResult validate() const;                        // Validate configuration
```

### Error Handling

#### Error Codes
```cpp
enum class ConfigErrorCode {
    None,
    FileNotFound,
    FileInvalid,
    ParseError,
    KeyNotFound,
    DuplicateKey,
    TypeMismatch,
    InvalidValue,
    ValidationError,
    OutOfRange,
    ThreadSafetyError,
    MemoryError,
    UnknownError
};
```

#### Exception Class
```cpp
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message, ConfigErrorCode code = ConfigErrorCode::UnknownError);
    ConfigErrorCode getErrorCode() const;
};
```

#### Result Class
```cpp
class ConfigResult {
public:
    ConfigResult();                                           // Default (success)
    ConfigResult(ConfigErrorCode code, const std::string& msg); // Error result
    
    bool isSuccess() const;
    bool isFailure() const;
    ConfigErrorCode getErrorCode() const;
    const std::string& getMessage() const;
};
```

### CLI Parser Class

#### Option Structure
```cpp
struct Option {
    std::string longName;
    std::string shortName;
    std::string description;
    std::string defaultValue;
    bool required;
    bool isFlag;
    std::string type;  // "int", "double", "string", "bool", "file"
    std::function<bool(const std::string&)> validator;
};
```

#### Subcommand Structure
```cpp
struct Subcommand {
    std::string name;
    std::string description;
    std::vector<Option> options;
    std::function<void(const std::map<std::string, ConfigValue>&)> handler;
};
```

#### Parser Methods
```cpp
void addOption(const std::string& longName, const std::string& shortName,
               const std::string& description, const std::string& defaultValue = "",
               bool required = false, bool isFlag = false, const std::string& type = "string",
               std::function<bool(const std::string&)> validator = nullptr);

void addSubcommand(const std::string& name, const std::string& description);

ConfigResult parse(int argc, char** argv, std::map<std::string, ConfigValue>& outArgs) const;
void printHelp() const;
```

## Backward Compatibility

### Legacy API Methods
```cpp
// All existing code continues to work
bool legacyLoadFromFile(const std::string& filepath);
bool legacyLoadFromArgs(int argc, char** argv);
bool legacySaveToFile(const std::string& filepath) const;

std::optional<T> legacyGet(const std::string& key) const;
T legacyGetOr(const std::string& key, const T& defaultValue) const;

void legacySet(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void legacySet(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void legacySet(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void legacySet(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void legacySet(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

bool legacyHas(const std::string& key) const;
void legacyRemove(const std::string& key);
std::vector<std::string> legacyGetKeys() const;
void legacyClear();
std::string legacySummary() const;
```

## Examples

### Basic Usage
```cpp
#include "core/Config/Config.hpp"

int main() {
    nlm::Config config;
    
    // Set configuration values
    config.legacySet("random_seed", 42);
    config.legacySet("simulation_timestep", 0.001);
    config.legacySet("neuron_count", 1000);
    
    // Get configuration values
    int seed = config.legacyGetOr<int>("random_seed", 0);
    double timestep = config.legacyGetOr<double>("simulation_timestep", 0.001);
    
    // Load from JSON file
    ConfigResult result = config.legacyLoadFromFile("config.json");
    if (result.isSuccess()) {
        std::cout << "Configuration loaded successfully" << std::endl;
    }
    
    return 0;
}
```

### Enhanced CLI Usage
```bash
# Help system
./nlm --help

# Boolean flags
./nlm --enable-feature --debug

# Key-value pairs
./nlm --config-file config.json --neuron-count 1000

# Subcommands
./nlm config show
./nlm config validate
./nlm config set key value
```

### JSON Configuration
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "visualization_enabled": false,
    "log_level": "INFO",
    "max_simulation_steps": 10000
}
```

### Thread-Safe Configuration
```cpp
#include <thread>
#include <vector>

void worker(nlm::Config& config, int id) {
    config.legacySet("worker_id", id);
    config.legacySet("status", "active");
    
    std::string status = config.legacyGetOr<std::string>("status", "unknown");
    int workerId = config.legacyGetOr<int>("worker_id", -1);
}

int main() {
    nlm::Config config;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker, std::ref(config), i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

## Build Instructions

### CMake Build
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Options
- `-DBUILD_WITH_NLOHMANN_JSON=ON/OFF`: Enable/disable JSON support (default: ON)
- `-DCMAKE_BUILD_TYPE=Debug/Release`: Build type

### Install
```bash
cmake --install . --prefix /usr/local
```

## Testing

### Running Tests
```bash
# Run all tests
./test_runner

# Run specific test
./test_runner --filter config
```

### Test Files
- `tests/test_config.cpp`: Basic configuration tests
- `test_enhanced_config.cpp`: Enhanced features test

## Troubleshooting

### Common Issues

1. **JSON parsing errors**: Ensure JSON syntax is correct
2. **File not found**: Check file paths and permissions
3. **Type mismatches**: Use the correct type for configuration values
4. **Thread safety issues**: Ensure all configuration operations are thread-safe
5. **Memory errors**: Check for proper resource cleanup

### Error Messages

- `FileNotFound`: Configuration file cannot be opened
- `ParseError`: Invalid JSON/YAML format
- `TypeMismatch`: Value type doesn't match expected type
- `DuplicateKey`: Key already exists with higher priority source
- `OutOfRange`: Value outside valid range
- `ValidationError`: Configuration validation failed

## Future Enhancements

1. **YAML support**: Native YAML parsing using a YAML library
2. **Advanced validation**: Schema validation with JSON Schema
3. **Configuration inheritance**: Support for configuration inheritance
4. **Hot-reloading**: Dynamic configuration reloading
5. **Environment variables**: Support for environment variable configuration

## Conclusion

The enhanced configuration system provides a robust, thread-safe, and feature-rich solution for managing configuration in the NLM project. It maintains full backward compatibility while offering powerful new features for modern software development.