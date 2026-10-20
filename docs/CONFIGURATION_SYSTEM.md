# NLM Enhanced Configuration System

## Overview

The NLM (Neural Learning Machine) configuration system has been significantly enhanced to provide:

- **JSON/YAML Support**: Native support for both JSON and YAML configuration files
- **Schema Validation**: Configuration schema definition and validation
- **Command-Line Integration**: Enhanced CLI argument parsing with validation
- **Structured Configuration**: Type-safe configuration value management
- **Backward Compatibility**: Support for existing key=value format

## Configuration Value Types

The configuration system supports multiple value types:

```cpp
using ConfigValue = std::variant<
    int,                    // Integer values (int32_t)
    int64_t,                // 64-bit integer values
    double,                 // Floating-point values (IEEE 754)
    bool,                   // Boolean values
    std::string,           // String values
    std::vector<int>,      // Integer arrays
    std::vector<double>,   // Double arrays
    std::vector<std::string>  // String arrays
>;
```

## Configuration Source

Configuration values can come from different sources with different priorities:

```cpp
enum class ConfigSource {
    Default,      // Default values set by the application
    File,         // Loaded from configuration file
    CommandLine,  // Set via command line arguments
    Runtime       // Set dynamically at runtime
};
```

## Configuration Entry Structure

Each configuration entry consists of:

```cpp
struct ConfigEntry {
    std::string key;              // Configuration key (case-insensitive)
    ConfigValue value;            // Stored value
    ConfigSource source;          // Source of this value
    std::string description;      // Human-readable description
};
```

## Schema-Based Validation

### Schema Definition

The configuration system supports schema validation to ensure valid configurations:

```cpp
struct ConfigSchemaEntry {
    std::string key;                                    // Configuration key
    ConfigValue defaultValue;                          // Default value
    std::string description;                           // Human-readable description
    std::function<bool(const ConfigValue&)> validator;  // Custom validator
    bool required;                                      // Whether key is required
    std::string type;                                   // Expected type
};
```

### Validation Examples

```cpp
// Example schema definition
std::vector<ConfigSchemaEntry> schema = {
    {"neuron_count", 1000, "Number of neurons in the brain",
     Config::validateInt, true, "int"},
    {"region_count", 2, "Number of brain regions",
     Config::validateInt, false, "int"},
    {"connection_probability", 0.1, "Connection probability",
     Config::validateDouble, false, "double"},
    {"random_seed", 42, "Random seed for reproducible experiments",
     Config::validateInt, true, "int64"},
    {"simulation_timestep", 0.001, "Simulation timestep in seconds",
     Config::validateDouble, true, "double"},
    {"visualization_enabled", false, "Enable visualization",
     Config::validateBool, false, "bool"}
};
```

### Custom Validators

The system provides built-in validators and allows custom validation functions:

```cpp
// Built-in validators
static bool validateString(const ConfigValue& value);
static bool validateInt(const ConfigValue& value);
static bool validateDouble(const ConfigValue& value);
static bool validateBool(const ConfigValue& value);
static bool validateIntArray(const ConfigValue& value);
static bool validateDoubleArray(const ConfigValue& value);
static bool validateStringArray(const ConfigValue& value);
static bool isValidValue(const ConfigValue& value, const std::string& type);
```

## File Format Support

### JSON Format

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "visualization_enabled": false,
    "log_level": "INFO",
    "log_to_file": false
}
```

### YAML Format

```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.1
random_seed: 42
simulation_timestep: 0.001
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
stdp_tau: 20.0
visualization_enabled: false
log_level: INFO
log_to_file: false
```

### Key=Value Format (Backward Compatible)

```ini
# Example key=value configuration
neuron_count = 1000
region_count = 2
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
visualization_enabled = false
log_level = INFO
log_to_file = false
```

## Command-Line Interface

### Argument Parsing

```bash
# Basic usage
./nlm --config config.json --verbose

# Override configuration values
./nlm --neuron_count=2000 --connection_probability=0.2 --stdp_tau=30.0

# Key=value format for custom settings
./nlm --neuron_count=2000 --region_count=1 --stdp_ltp_weight=0.03

# Shorthand format
./nlm -n 2000 -r 1 -s 30.0

# Validation only
./nlm --validate-only --config custom.cfg

# JSON logging
./nlm --json-logging --config config.yaml
```

### Supported Command-Line Options

```
-h, --help                    Show help message and exit
--version                     Show version information and exit
-v, --verbose                 Enable verbose output
--json-logging               Enable JSON logging format
--validate-only              Validate configuration and exit
-c, --config FILE            Specify configuration file (default: configs/default.cfg)
--steps N                    Set maximum simulation steps
--time-limit SECONDS        Set time limit in seconds
--key=value                  Set configuration key-value pair
-k value                     Set configuration key-value pair (shorthand)
```

### Configuration Override Priority

1. **Command-line arguments** (highest priority)
2. **Configuration file** (medium priority)
3. **Default values** (lowest priority)

### Configuration Validation

The system validates configuration values against schema and basic constraints:

```cpp
// Validate configuration
bool validateConfiguration(const Config& config);

// Common validation rules:
- neuron_count must be between 1 and 100000
- region_count must be between 1 and 1000
- connection_probability must be between 0.0 and 1.0
- simulation_timestep must be between 0.0001 and 0.1
- Required keys must be present
```

## API Reference

### Class: nlm::Config

#### Constructors
```cpp
Config();                           // Default constructor
Config(Config&&) noexcept;           // Move constructor
```

#### Destructor
```cpp
~Config();                         // Destructor
```

#### Assignment Operators
```cpp
Config& operator=(const Config&) = delete;  // Copy assignment deleted
Config& operator=(Config&&) noexcept;       // Move assignment
```

#### Loading Methods
```cpp
bool loadFromFile(const std::string& filepath);           // Load from file (JSON/YAML/key=value)
bool loadFromArgs(int argc, char** argv);                // Load from command line arguments
bool loadFromJSON(const std::string& jsonContent);         // Load from JSON string
bool loadFromYAML(const std::string& yamlContent);         // Load from YAML string
```

#### Saving Methods
```cpp
bool saveToFile(const std::string& filepath) const;        // Save to file (JSON format)
```

#### Value Access Methods
```cpp
template<typename T>
std::optional<T> get(const std::string& key) const;      // Get value by key (returns optional)

template<typename T>
T getOr(const std::string& key, const T& defaultValue) const;  // Get value with default

void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
```

#### Query Methods
```cpp
bool has(const std::string& key) const;                    // Check if key exists
void remove(const std::string& key);                      // Remove key
std::vector<std::string> getKeys() const;                 // Get all keys
void clear();                                           // Clear all configuration
std::string summary() const;                             // Get configuration summary
```

#### Schema Management
```cpp
void setSchema(const std::vector<ConfigSchemaEntry>& schema);  // Set configuration schema
void clearSchema();                                          // Clear schema
std::vector<ConfigValidationError> validate() const;        // Validate against schema
```

#### Static Validation Helpers
```cpp
static bool isValidValue(const ConfigValue& value, const std::string& type);
static bool validateString(const ConfigValue& value);
static bool validateInt(const ConfigValue& value);
static bool validateDouble(const ConfigValue& value);
static bool validateBool(const ConfigValue& value);
static bool validateIntArray(const ConfigValue& value);
static bool validateDoubleArray(const ConfigValue& value);
static bool validateStringArray(const ConfigValue& value);
```

## Error Handling

### Configuration Validation Errors

The validation system reports errors in a structured way:

```cpp
struct ConfigValidationError {
    std::string key;     // The key that failed validation
    std::string message; // Error message describing the issue
};
```

### Common Error Scenarios

```
ERROR: Missing required configuration key: neuron_count
ERROR: Expected type 'int', got 'string' (key: simulation_timestep)
ERROR: Failed custom validation (key: connection_probability)
ERROR: neuron_count must be between 1 and 100000
ERROR: connection_probability must be between 0.0 and 1.0
```

## Performance Considerations

### Memory Usage

- Configuration entries are stored in a vector for fast access
- Default schema validation is performed after loading
- Memory usage is proportional to number of configuration entries

### Loading Performance

- JSON parsing uses streaming for large files
- YAML parsing is optimized for key-value pairs
- Command-line parsing is linear in number of arguments
- File format detection is O(n) in file size

### Validation Performance

- Schema validation is O(n) where n is number of schema entries
- Type checking is O(1) per entry
- Custom validators are called per entry
- Validation fails fast on first error

## Integration with Other Components

### Brain Initialization

The configuration system integrates with brain initialization:

```cpp
auto config = std::make_shared<Config>();
config->loadFromFile("configs/default.cfg");
config->loadFromArgs(argc, argv);

// Create brain with configuration
auto brain = std::make_shared<Brain>(config);
brain->initialize();
```

### Logger Integration

The configuration system provides logging configuration:

```cpp
auto logger = std::make_shared<Logger>();
auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
logger->setLevel(LogLevel::Info);
logger->addLogger(consoleLogger);
Logger::setGlobal(logger);
```

## Examples

### Basic Configuration Loading

```cpp
#include "Config.hpp"

int main(int argc, char** argv) {
    nlm::Config config;
    
    // Load from default file
    config.loadFromFile("configs/default.cfg");
    
    // Override with command line
    config.loadFromArgs(argc, argv);
    
    // Access configuration values
    int64_t neuronCount = config.getOr<int64_t>("neuron_count", 1000);
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    
    // Use configuration
    std::cout << "Simulating " << neuronCount << " neurons with timestep " << timestep << std::endl;
    
    return 0;
}
```

### Schema-Based Configuration

```cpp
#include "Config.hpp"

// Define schema
std::vector<nlm::ConfigSchemaEntry> schema = {
    {"neuron_count", 1000, "Number of neurons", nlm::Config::validateInt, true, "int"},
    {"region_count", 2, "Number of regions", nlm::Config::validateInt, false, "int"},
    {"connection_probability", 0.1, "Connection probability", nlm::Config::validateDouble, false, "double"},
    {"random_seed", 42, "Random seed", nlm::Config::validateInt, true, "int64"}
};

int main() {
    nlm::Config config;
    config.setSchema(schema);
    
    // Load and validate configuration
    if (!config.loadFromFile("config.json")) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }
    
    std::vector<nlm::ConfigValidationError> errors = config.validate();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            std::cerr << "Validation error (key: " << error.key << "): " << error.message << std::endl;
        }
        return 1;
    }
    
    std::cout << "Configuration validation passed" << std::endl;
    return 0;
}
```

### Command-Line Configuration

```bash
// Run with command-line overrides
./nlm --neuron_count=5000 --region_count=3 --connection_probability=0.2 --random_seed=123

// Check validation only
./nlm --validate-only --config myconfig.cfg

// Use JSON logging
./nlm --json-logging --config config.json
```

## Migration Guide

### From Key=Value to JSON/YAML

If you have existing key=value configuration files, you can migrate them:

```bash
# Rename or copy to .cfg extension for backward compatibility
cp config.cfg config.json

# Or create YAML version
cat config.cfg | sed 's/ = /: /g' > config.yaml
```

### Upgrading from Old Config Version

1. **Add required schema validation**
2. **Update to use JSON/YAML format**
3. **Add new configuration keys**
4. **Test validation rules**
5. **Update documentation**

## Limitations

1. **Simple JSON/YAML Parser**: The current implementation is a simplified parser. For complex nested structures, use a dedicated library.
2. **No Array Operations**: Configuration arrays are supported but operations are limited.
3. **Performance**: Validation adds overhead, but it's minimal for typical configurations.
4. **Schema Complexity**: Complex validation rules need custom validators.

## Future Enhancements

1. **Full JSON/YAML Support**: Integrate with nlohmann/json for complete JSON support.
2. **Advanced Validation**: Add constraint validation (range, patterns, etc.).
3. **Configuration Inheritance**: Support for configuration inheritance.
4. **Environment Variables**: Support for environment variable configuration.
5. **Configuration Monitoring**: Runtime configuration changes.
6. **Documentation Generation**: Automatic documentation from schema.

## References

- [C++20 Standard](https://isocpp.org/std/standing/2020-wormhole)
- [nlohmann/json Documentation](https://github.com/nlohmann/json)
- [YAML Specification](https://yaml.org/spec/1.2/spec.html)

---
*This documentation is auto-generated from the NLM source code comments and configuration system implementation.*
*Last updated: 2026-10-20*
*Version: 0.1.0*