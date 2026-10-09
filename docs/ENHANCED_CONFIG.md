# Enhanced Configuration System Documentation

## Overview

The NLM configuration system has been enhanced to support multiple file formats (JSON, YAML, and the original key=value) with advanced validation, schema definition, and automatic format detection.

## New Features

### 1. Multi-Format Support

The configuration system now supports three formats:

#### JSON Format (`.json`, `.jsonc`, `.json5`)
- Native support with nlohmann::json if available
- Fallback simple JSON parser if library not found
- Support for complex data structures: arrays, nested objects, strings, numbers, booleans
- Examples: `{"key": "value", "number": 42, "array": [1, 2, 3]}`

#### YAML Format (`.yaml`, `.yml`)
- Native support with yaml-cpp if available
- Fallback simple YAML parser if library not found
- Supports YAML 1.2 syntax with anchors and aliases
- Examples:
  ```yaml
  key: value
  number: 42
  array:
    - item1
    - item2
    ```

#### Simple Key=Value Format (`.cfg`, `.ini`, `.properties`)
- Original format, enhanced with complex value types
- Support for quoted strings, arrays in brackets
- Examples:
  ```
  key = value
  number = 42
  array = [1, 2, 3]
  ```

### 2. Automatic Format Detection

The system automatically detects format from:

#### File Extension
- `.json`, `.jsonc`, `.json5` → JSON
- `.yaml`, `.yml` → YAML
- `.cfg`, `.ini`, `.properties` → Simple Key=Value
- Others → Unknown

#### Content Analysis
- JSON: starts with `{` or contains valid JSON structure
- YAML: contains `:` or `---` (document start)
- Simple: contains `=` signs

### 3. Enhanced Value Types

#### Basic Types
- `int`, `int64_t`: Integers
- `double`: Floating point numbers
- `bool`: Boolean values (true/false)
- `std::string`: String values

#### Collection Types
- `std::vector<int>`: Integer arrays
- `std::vector<double>`: Double arrays
- `std::vector<std::string>`: String arrays
- `std::vector<std::vector<int>>`: 2D integer matrices

#### Parsing Examples
```cpp
// Complex values can be specified in configuration files
# Example values
integer_array = [1, 2, 3, 4, 5]
string_array = ["option1", "option2", "option3"]
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
```

### 4. Configuration Validation

#### Schema-Based Validation
- Define validation rules for configuration fields
- Support for required fields, ranges, custom validators
- Validation errors collected and retrievable

#### ValidationRule Structure
```cpp
struct ValidationRule {
    std::string fieldName;           // Field to validate
    std::function<bool(const ConfigValue&)> validator; // Validation function
    std::string errorMessage;        // Error message on failure
    bool required = false;           // Must field be present?
    std::string defaultValue;        // Default value if missing
};
```

#### Usage Example
```cpp
nlm::ConfigSchema schema;
schema.addRule(nlm::ValidationRule{
    "random_seed",
    [](const nlm::ConfigValue& val) {
        return std::get<int>(val) >= 0 && std::get<int>(val) <= 100000;
    },
    "Random seed must be between 0 and 100000",
    true,
    "42"
});
```

### 5. Configuration Schema Definition

#### ConfigSchema Class
Manages validation rules and provides validation services.

#### ConfigValidator Class
Performs validation against a schema with detailed error reporting.

### 6. Advanced Command Line Options

#### Basic Options
- `--key=value`: Set configuration value
- `--file=path`: Load configuration from file

#### Format-Specific Options
- `--json content`: Load inline JSON configuration
- `--yaml content`: Load inline YAML configuration

#### Validation Options
- `--validate`: Validate current configuration against schema
- `--help`: Show help text with all available options
- `--version`: Show version information

#### Example Usage
```bash
# Load from file
./program --file=config.json

# Set command line values
./program --random_seed=42 --simulation_timestep=0.001

# Load inline JSON
./program --json '{"key": "value", "number": 42}'

# Validate configuration
./program --validate
```

### 7. File Operations

#### Loading
```cpp
// Auto-detect format from file extension
config.loadFromFile("config.json");

// Specify format explicitly
config.loadFromFile("config.yaml", nlm::ConfigFormat::YAML);

// Load from string content
config.loadFromString(jsonString, nlm::ConfigFormat::JSON);
```

#### Saving
```cpp
// Save in specific format
config.saveToFile("output.json", nlm::ConfigFormat::JSON);

// Save with auto-detected format
config.saveToFile("output.yaml"); // Detects YAML from extension
```

### 8. Advanced APIs

#### Format Detection
```cpp
// Detect format from file extension
auto format = nlm::ConfigFormatDetector::detectFromExtension("config.json");

// Detect format from content
auto format = nlm::ConfigFormatDetector::detectFromContent(content);
```

#### Schema Management
```cpp
// Set schema
config.setSchema(schema);

// Validate
bool isValid = config.validate();

// Get validation errors
auto errors = config.getValidationErrors();
```

#### Helper Functions
```cpp
// Get complex types
auto intList = config.getIntList("key_name");
auto stringList = config.getStringList("key_name");
auto matrix = config.getIntMatrix("key_name");

// Get keys
auto keys = config.getKeys();
```

## Example Configuration Files

### JSON Example (`config.json`)
```json
{
    "simulation_settings": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "max_simulation_steps": 10000
    },
    "brain_settings": {
        "neuron_count": 1000,
        "plasticity": {
            "learning_rate": 0.01
        }
    },
    "array_examples": {
        "integer_array": [1, 2, 3, 4, 5],
        "string_array": ["option1", "option2", "option3"]
    }
}
```

### YAML Example (`config.yaml`)
```yaml
# NLM Configuration File
simulation_settings:
  random_seed: 42
  simulation_timestep: 0.001
  max_simulation_steps: 10000

brain_settings:
  neuron_count: 1000
  plasticity:
    learning_rate: 0.01

array_examples:
  integer_array:
    - 1
    - 2
    - 3
    - 4
    - 5
  string_array:
    - option1
    - option2
    - option3
```

### Simple Key=Value Example (`default.cfg`)
```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
max_simulation_steps = 10000

# Brain settings
neuron_count = 1000
plasticity_learning_rate = 0.01

# Arrays
integer_array = [1, 2, 3, 4, 5]
string_array = ["option1", "option2", "option3"]
```

## Integration Guide

### 1. Including the Enhanced Config System
```cpp
#include "core/Config/Config.hpp"
```

### 2. Basic Usage
```cpp
nlm::Config config;

// Load from file (auto-detects format)
config.loadFromFile("config.json");

// Get values
int seed = config.getOr<int>("simulation_settings.random_seed", 42);
std::string envName = config.getOr<std::string>("environment_settings.name", "GridWorld");

// Set values
config.set("custom_key", "custom_value", nlm::ConfigSource::Runtime);
```

### 3. Schema Integration
```cpp
nlm::Config config;
nlm::ConfigSchema schema;

// Define schema
schema.addRule(nlm::ValidationRule{
    "random_seed",
    [](const nlm::ConfigValue& val) {
        return std::get<int>(val) > 0;
    },
    "Random seed must be positive",
    true
});

// Apply schema
config.setSchema(schema);

// Validate after changes
config.set("random_seed", 42);
assert(config.validate());  // Will pass
```

### 4. Command Line Integration
```cpp
int main(int argc, char** argv) {
    nlm::Config config;
    
    // Parse advanced command line arguments
    config.loadFromAdvancedArgs(argc, argv);
    
    // Load additional configuration file if specified
    if (config.has("file")) {
        config.loadFromFile(config.getOr<std::string>("file", ""));
    }
    
    // Validate if requested
    if (argc > 1 && std::string(argv[1]) == "--validate") {
        if (config.validate()) {
            std::cout << "Configuration is valid!" << std::endl;
        } else {
            std::cout << "Validation errors:" << std::endl;
            for (const auto& error : config.getValidationErrors()) {
                std::cout << "  - " << error << std::endl;
            }
        }
    }
    
    return 0;
}
```

## Migration Guide

### From Original Simple Format to Enhanced Format

#### 1. Upgrade Configuration Files
Replace simple key=value format with JSON or YAML for better:

#### 2. Update Code
```cpp
// Before (simple format only)
config.loadFromFile("config.cfg");
int value = config.getOr<int>("key", 0);

// After (multi-format)
config.loadFromFile("config.json");  // Auto-detects JSON
int value = config.getOr<int>("key", 0);
```

#### 3. Add Validation
```cpp
// Before (no validation)
config.set("param", value);

// After (with validation)
nlm::ConfigSchema schema;
schema.addRule(nlm::ValidationRule{
    "param",
    [](const nlm::ConfigValue& val) { return std::get<int>(val) >= 0; },
    "Parameter must be non-negative"
});
config.setSchema(schema);
config.set("param", value);
assert(config.validate());
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Format Detection Fails
**Problem**: Config file not loaded
**Solution**: Check file extension or provide explicit format
```cpp
config.loadFromFile("config.unknown", nlm::ConfigFormat::JSON);
```

#### 2. Validation Fails
**Problem**: Configuration doesn't meet schema requirements
**Solution**: Check validation errors and fix configuration
```cpp
if (!config.validate()) {
    for (const auto& error : config.getValidationErrors()) {
        std::cout << "Error: " << error << std::endl;
    }
}
```

#### 3. Complex Value Parsing Fails
**Problem**: Arrays or nested structures not parsed correctly
**Solution**: Check format and ensure correct syntax
```json
// Correct JSON
{
    "array": [1, 2, 3],
    "nested": {
        "key": "value"
    }
}
```

### Performance Considerations

- **Large Configuration Files**: Use JSON or YAML for better performance with large files
- **Validation**: Only validate when needed (e.g., before important operations)
- **Schema Caching**: Reuse schema objects across multiple configuration instances

## API Reference

### Core Classes

#### Config
- `loadFromFile(const std::string& filepath, ConfigFormat format = Unknown)`
- `loadFromString(const std::string& content, ConfigFormat format)`
- `saveToFile(const std::string& filepath, ConfigFormat format = Unknown) const`
- `get<T>(const std::string& key) const`
- `getOr<T>(const std::string& key, const T& defaultValue) const`
- `set(const std::string& key, const ConfigValue& value, ConfigSource source = Runtime)`
- `has(const std::string& key) const`
- `setSchema(const ConfigSchema& schema)`
- `validate() const`
- `getValidationErrors() const`

#### ConfigSchema
- `addRule(const ValidationRule& rule)`
- `validate(const ConfigEntry& entry) const`
- `validateAll(const std::vector<ConfigEntry>& entries) const`
- `getErrors() const`

#### ConfigFormatDetector
- `detectFromExtension(const std::string& filepath)`
- `detectFromContent(const std::string& content)`
- `formatToString(ConfigFormat format)`

#### ConfigParser
- `parse(const std::string& content, std::vector<ConfigEntry>& entries)`
- `getFormatName() const`
- `getSupportedExtensions() const`

### Enumerations

#### ConfigFormat
- `Unknown`: Unknown format
- `SimpleKeyValue`: Simple key=value format
- `JSON`: JSON format
- `YAML`: YAML format

#### ConfigSource
- `Default`: Default configuration
- `File`: Configuration from file
- `CommandLine`: Configuration from command line
- `Runtime`: Configuration set at runtime

## Testing

Run the enhanced configuration tests:

```bash
./nlm_test config_tests  # Run all configuration tests
```

Or use the testing framework:

```cpp
void test_config::runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testComplexValueTypes();
    testFormatDetection();
    testSchemaValidation();
    testFileFormatSupport();
    testAdvancedCommandLine();
    testHelpText();
}
```

## Future Enhancements

1. **Expression Evaluation**: Support for expressions in configuration values
2. **Hot Reloading**: Automatic reloading of configuration files
3. **Distributed Configuration**: Support for distributed configuration management
4. **Configuration Encryption**: Secure storage of sensitive configuration values
5. **Configuration History**: Track changes to configuration over time

## Support and Community

For issues, questions, or contributions:
- Check the documentation for troubleshooting
- Report bugs at the project issue tracker
- Contribute examples and additional parsers
- Help maintain compatibility with existing configurations

## License

The enhanced configuration system is part of the NLM project.
See the LICENSE file for details.
