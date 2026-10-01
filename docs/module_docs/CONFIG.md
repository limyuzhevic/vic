# Core Module Documentation

## Config Module

The Config module provides a comprehensive configuration management system for the Neural Learning Machine (NLM). It offers a type-safe key-value store for application parameters and settings with support for multiple configuration sources and proper precedence handling.

### Key Features

- **Multiple Configuration Sources**: Default, File, CommandLine, and Runtime sources
- **Type Safety**: Template-based type checking for configuration values
- **JSON/YAML Support**: File loading for standard configuration formats
- **Priority System**: Proper precedence handling when multiple sources conflict
- **Error Handling**: Comprehensive error handling for file I/O and parsing failures

### Usage Examples

#### Basic Configuration
```cpp
#include "Config.hpp"

void example() {
    Config config;
    
    // Set configuration values
    config.set("network.timestep", 0.001);
    config.set("network.neuron_count", 1000);
    config.set("learning.rate", 0.01);
    config.set("simulation.duration", 10.0);
    
    // Get configuration values
    auto timestep = config.get<double>("network.timestep");
    int neuronCount = config.get<int>("network.neuron_count");
    
    // Use values with defaults
    double learningRate = config.getOr("learning.rate", 0.001);
}
```

#### File Loading
```cpp
bool loadConfig(const std::string& filepath) {
    Config config;
    
    if (!config.loadFromFile(filepath)) {
        std::cerr << "Failed to load configuration from: " << filepath << std::endl;
        return false;
    }
    
    return true;
}
```

#### Command Line Arguments
```cpp
void processCommandLine(int argc, char** argv) {
    Config config;
    
    if (config.loadFromArgs(argc, argv)) {
        // Process configuration
        auto logLevel = config.get<std::string>("logging.level");
    }
}
```

#### Configuration Summary
```cpp
void printConfigSummary(const Config& config) {
    std::cout << config.summary() << std::endl;
}
```

### API Reference

#### Class: `Config`

##### Constructors
- `Config()`: Default constructor creating an empty configuration
- `Config(Config&&) noexcept`: Move constructor
- `Config(const Config&) = delete`: Copy constructor (deleted)

##### Destructor
- `~Config()`: Destructor

##### Assignment Operators
- `Config& operator=(const Config&) = delete`: Copy assignment (deleted)
- `Config& operator=(Config&&) noexcept`: Move assignment

##### Configuration Loading
- `bool loadFromFile(const std::string& filepath)`: Load configuration from file
- `bool loadFromArgs(int argc, char** argv)`: Load configuration from command line

##### Configuration Saving
- `bool saveToFile(const std::string& filepath) const`: Save configuration to file

##### Configuration Access
- `template<typename T> std::optional<T> get(const std::string& key) const`: Get value of specific type
- `template<typename T> T getOr(const std::string& key, const T& defaultValue) const`: Get value with default
- `bool has(const std::string& key) const`: Check if key exists
- `std::vector<std::string> getKeys() const`: Get all keys
- `void clear()`: Clear all configuration

##### Configuration Modification
- `void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime)`: Set value with type deduction
- `void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime)`: Set string value
- `void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime)`: Set integer value
- `void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime)`: Set floating-point value
- `void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime)`: Set boolean value
- `void remove(const std::string& key)`: Remove configuration key

##### Information
- `std::string summary() const`: Get human-readable configuration summary

#### Enum: `ConfigSource`

- `Default`: Built-in configuration values
- `File`: Values loaded from configuration file
- `CommandLine`: Values set via command line arguments
- `Runtime`: Values set at runtime (programmatically)

#### Type Alias: `ConfigValue`

A variant type that can hold:
- Basic types: `int`, `int64_t`, `double`, `bool`
- String types: `std::string`
- Container types: `std::vector<int>`, `std::vector<double>`, `std::vector<std::string>`

#### Struct: `ConfigEntry`

Represents a single configuration parameter with metadata:
- `key`: Configuration key/parameter name
- `value`: Configuration value (ConfigValue)
- `source`: How the value was obtained
- `description`: Human-readable description of the parameter

### Error Handling

#### Exception Hierarchy

The Config module defines an exception hierarchy for configuration-related errors:

##### Base Class: `ConfigExceptions::ConfigError`
- Base exception for all configuration errors
- Constructor: `ConfigError(const std::string& message)`

##### Derived Classes:
- `ConfigExceptions::FileLoadError`: Thrown when a configuration file cannot be loaded
- `ConfigExceptions::ParseError`: Thrown when configuration parsing fails
- `ConfigExceptions::MissingValueError`: Thrown when a required configuration value is missing

### Configuration Best Practices

1. **Use Descriptive Keys**: Use dotted notation (e.g., `network.learning_rate`)
2. **Provide Defaults**: Always provide sensible defaults for critical parameters
3. **Error Handling**: Handle configuration loading failures gracefully
4. **Type Safety**: Use templates to ensure type safety when accessing configuration values
5. **Documentation**: Add descriptions for configuration parameters

### Performance Considerations

- Configuration values are cached for fast access
- File I/O is buffered for large configuration files
- Type checking is performed at compile time for critical paths
- Memory usage is optimized for large numbers of configuration entries

### Integration with Other Modules

The Config module integrates with many other NLM modules:

- **Random Module**: Configuration of random number generators and seeds
- **Logger Module**: Configuration of logging levels and output destinations
- **SimulationClock Module**: Configuration of simulation parameters
- **NeuralNetwork Modules**: Configuration of network topology and parameters

### Migration Guide

#### From Version 0.x to Version 1.0

1. **Configuration File Format**: Changed from simple key=value format to support JSON/YAML
2. **Type System**: Added stronger type checking and support for vector types
3. **Error Handling**: Completely rewritten error handling system
4. **Configuration Sources**: Enhanced command-line argument parsing

## Related Documentation

- **NLM Overview**: General introduction to the Neural Learning Machine
- **API Reference**: Complete API documentation for all classes and methods
- **Usage Examples**: Practical examples of configuration usage
- **Error Handling**: Detailed information about configuration-related errors
