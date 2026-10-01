# NLM Config API Documentation

## Overview

The Config class provides a unified interface for managing configuration settings in the NLM (Neural Learning Machine) system. It supports multiple configuration sources and value types.

## Configuration Value Types

The Config class supports the following value types:

- **int**: Integer values
- **int64_t**: 64-bit integer values  
- **double**: Floating point values
- **bool**: Boolean values (true/false)
- **std::string**: String values
- **std::vector<int>**: Integer arrays
- **std::vector<double>**: Double arrays
- **std::vector<std::string>**: String arrays

## Configuration Sources

Configuration values can come from different sources with different priorities:

- **ConfigSource::Default**: Default configuration values
- **ConfigSource::File**: Values loaded from configuration files
- **ConfigSource::CommandLine**: Values from command line arguments
- **ConfigSource::Runtime**: Values set at runtime

## Core Methods

### Constructor/Destructor

```cpp
Config();                    // Create empty configuration
~Config();                   // Clean up configuration
```

### Loading Configuration

```cpp
bool loadFromFile(const std::string& filepath);
// Load configuration from file (supports JSON, YAML, and simple key=value formats)

bool loadFromArgs(int argc, char** argv);
// Load configuration from command line arguments (--key=value, -key value)
```

### Saving Configuration

```cpp
bool saveToFile(const std::string& filepath) const;
// Save configuration to file in simple key=value format
```

### Getting Values

```cpp
template<typename T>
std::optional<T> get(const std::string& key) const;
// Get value with type T, returns std::nullopt if key doesn't exist

template<typename T>
T getOr(const std::string& key, const T& defaultValue) const;
// Get value with fallback to default value
```

### Setting Values

```cpp
void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
// Set value with specific source
```

### Configuration Management

```cpp
bool has(const std::string& key) const;
// Check if key exists

void remove(const std::string& key);
// Remove key and its value

std::vector<std::string> getKeys() const;
// Get all configuration keys

void clear();
// Clear all configuration values

std::string summary() const;
// Get configuration summary as string
```

## File Format Support

### Simple Key=Value Format (Default)

The config file uses a simple key=value format with the following features:

```ini
# Configuration comments
key = value
another_key = "string value"
boolean_key = true
number_key = 42.5
```

### JSON Format

JSON files are supported with full object and array support:

```json
{
    "key": "value",
    "number": 42,
    "boolean": true,
    "array": [1, 2, 3],
    "nested": {
        "inner_key": "inner_value"
    }
}
```

### YAML Format

YAML files are supported (basic implementation):

```yaml
key: value
number: 42
boolean: true
array:
  - 1
  - 2
  - 3
nested:
  inner_key: inner_value
```

## Example Usage

### Basic Configuration

```cpp
#include "core/Config/Config.hpp"

int main() {
    nlm::Config config;
    
    // Set configuration values
    config.set("learning_rate", 0.01);
    config.set("neuron_count", 1000);
    config.set("enable_plasticity", true);
    config.set("model_name", "default_model");
    
    // Get configuration values
    double learningRate = config.getOr<double>("learning_rate", 0.001);
    int neuronCount = config.getOr<int>("neuron_count", 500);
    bool plasticityEnabled = config.getOr<bool>("enable_plasticity", false);
    
    // Save to file
    config.saveToFile("config.json");
    
    // Load from file
    nlm::Config loadedConfig;
    loadedConfig.loadFromFile("config.json");
    
    return 0;
}
```

### Command Line Usage

```bash
./nlm --learning_rate=0.05 --neuron_count=2000 --enable_plasticity
./nlm -learning_rate 0.1 -neuron_count 1000
```

## Error Handling

- `loadFromFile()` returns false if file cannot be opened
- `get()` returns std::nullopt if key doesn't exist
- Invalid type conversions are handled gracefully

## Performance Considerations

- Configuration values are cached in memory for fast access
- Configuration source is tracked for priority resolution
- The JSON/YAML parser is lazy-loaded to reduce startup time

## Thread Safety

The Config class is not thread-safe. Access from multiple threads requires external synchronization.

## Migration Guide

### From Simple Key=Value to JSON

To migrate from simple key=value format to JSON:

1. Create a JSON file with the same key-value pairs
2. Update loadFromFile calls to use .json extension
3. The parser automatically detects JSON format

Example migration:

```ini
# Before (simple format)
learning_rate = 0.01
neuron_count = 1000
```

```json
// After (JSON format)
{
    "learning_rate": 0.01,
    "neuron_count": 1000
}
```

## Best Practices

1. **Use consistent key naming**: Use snake_case for configuration keys
2. **Document configuration**: Add comments describing what each setting does
3. **Use appropriate types**: Choose the most specific type for each value
4. **Provide sensible defaults**: Set default values for critical configuration options
5. **Version configuration**: Include version information in configuration files
6. **Test configuration loading**: Ensure configuration parsing works correctly in all formats
