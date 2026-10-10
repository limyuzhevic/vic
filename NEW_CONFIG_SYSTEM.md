# NLM Configuration System - New Implementation

## Overview
This is a complete refactoring of the NLM (Neural Learning Machine) configuration system, replacing the simple key=value format with a full-featured JSON/YAML-based system with schema validation, type safety, and backward compatibility.

## Key Improvements

### 1. JSON and YAML Support
- Full JSON support using nlohmann/json library
- Full YAML support using yaml-cpp library
- Automatic format detection when loading files
- Bidirectional saving (JSON ↔ YAML)

### 2. Configuration Schema Validation
- Schema-based validation with error reporting
- Support for required/optional fields
- Type validation with custom validators
- Category-based organization (System, Neural, Plasticity, Memory, etc.)

### 3. Enhanced Type Safety
- Extended ConfigValue types including vectors of int, int64_t, double, bool, and string
- Automatic type conversion and validation
- Type-specific formatters and validators

### 4. Configuration Categories and Organization
- 11 predefined categories: System, Neural, Simulation, Plasticity, Memory, Learning, Development, Visualization, Environment, Logging, Performance
- Nested configuration groups for hierarchical organization
- Support for subgroups within groups

### 5. Backward Compatibility
- Maintained the existing Config class interface
- Kept all original methods: `get()`, `set()`, `loadFromFile()`, `loadFromArgs()`, etc.
- Support for the original key=value format for legacy configs
- All existing code continues to work without changes

### 6. Advanced Features
- Default configuration management
- Configuration inheritance (merging multiple configs)
- Detailed error reporting with validation results
- Configuration summaries grouped by category
- Schema saving/loading functionality

## Technical Implementation

### ConfigValue Structure
```cpp
typedef std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>
> ConfigValue;
```

### Configuration Sources
- `Default`: Built-in default values
- `File`: Loaded from JSON/YAML/config files
- `CommandLine`: From command-line arguments
- `Runtime`: Set programmatically at runtime
- `Inherited`: From configuration inheritance

### Configuration Categories
- **System**: Random seeds, simulation parameters
- **Neural**: Neuron counts, connectivity, regions
- **Plasticity**: STDP parameters, structural plasticity
- **Memory**: Working memory, episodic memory
- **Learning**: Neuromodulation, reward systems
- **Development**: Age-dependent parameters
- **Visualization**: Display settings
- **Environment**: World settings
- **Logging**: Log level, output settings
- **Performance**: Optimization parameters

## Files Created/Modified

### Core Files
1. **src/core/Config/Config.hpp** - New header file with enhanced Config class and schema support
2. **src/core/Config/Config.cpp** - Complete implementation (906 lines vs original 215)

### Configuration Files
3. **configs/config.json** - Sample JSON configuration file
4. **configs/config.yaml** - Sample YAML configuration file
5. **configs/SAMPLES.md** - Documentation with sample formats

### Test Files
6. **tests/test_config.cpp** - Enhanced test suite with 10 new test cases
7. **test_config_system.cpp** - Standalone test program

### Updated Files
8. **src/main.cpp** - Updated to use `loadFromFileAuto()` for format detection
9. **CMakeLists.txt** - Reference (not modified for this implementation)

## Testing

The new configuration system includes comprehensive tests:

1. **Basic Functionality**: Creation, setting, getting, removing keys
2. **Type Support**: int, int64_t, double, bool, string, arrays
3. **JSON Loading**: Parse and validate JSON configuration files
4. **YAML Loading**: Parse and validate YAML configuration files
5. **Schema Integration**: Schema validation with error reporting
6. **File Operations**: Save and load configuration files
7. **Configuration Inheritance**: Merge multiple configuration sources
8. **Backward Compatibility**: Legacy key=value format support

## Build Requirements

### Dependencies
- nlohmann/json library (installed via `nlohmann-json3-dev`)
- yaml-cpp library (installed via `libyaml-cpp-dev`)

### Compilation
```bash
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Required libraries
find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)

target_link_libraries(nlm_core PUBLIC nlohmann_json::nlohmann_json yaml-cpp)
```

## Migration Guide

### Existing Code
No changes required! The existing Config interface remains exactly the same:

```cpp
// This still works exactly as before
auto config = std::make_shared<Config>();
config->loadFromFile("configs/default.cfg");
config->set("random_seed", 42, ConfigSource::Default);
int seed = config->getOr<int>("random_seed", 42);
```

### New Features
To use new features, you can:

```cpp
// Load from JSON/YAML with format detection
config->loadFromFileAuto("config.json"); // Works with .json, .yaml, .yml
config->loadFromJSON(jsonString);         // Direct JSON parsing
config->loadFromYAML(yamlString);         // Direct YAML parsing

// Use schema validation
config->setSchema(schema);               // Apply schema for validation
config->applyDefaults();                  // Apply default values
config->merge(otherConfig);              // Configuration inheritance
```

## Example Usage

### Creating a JSON Configuration
```json
{
  "nlm": {
    "version": "2.0",
    "system": {
      "random_seed": 42,
      "simulation_timestep": 0.001
    },
    "neural": {
      "neuron_count": 500,
      "connection_probability": 0.15
    }
  }
}
```

### Creating a YAML Configuration
yaml: '2.1'
nlm:
  version: '2.0'
  system:
    random_seed: 42
    simulation_timestep: 0.001
  neural:
    neuron_count: 500
    connection_probability: 0.15

### Using the Configuration System
```cpp
#include "src/core/Config/Config.hpp"

int main() {
    auto config = std::make_shared<Config>();
    
    // Load with automatic format detection
    if (config->loadFromFile("configs/config.json")) {
        std::cout << "Config loaded successfully!" << std::endl;
    }
    
    // Access values
    int64_t seed = config->getOr<int64_t>("random_seed", 42);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Override with command line
    config->loadFromArgs(argc, argv);
    
    // Apply defaults and validation
    config->applyDefaults();
    
    // Get summary
    std::cout << config->summary() << std::endl;
    
    return 0;
}
```

## Benefits

1. **Type Safety**: Full type checking and conversion
2. **Schema Validation**: Catch errors early with detailed reporting
3. **Organization**: Logical grouping by category
4. **Flexibility**: Support for multiple formats and inheritance
5. **Backward Compatibility**: No breaking changes
6. **Extensibility**: Easy to add new schema validators
7. **Developer Experience**: Better error messages and summaries

## Future Enhancements

- **Schema Evolution**: Versioned configuration schemas
- **Remote Configuration**: Loading from HTTP/HTTPS endpoints
- **Encryption**: Support for encrypted configuration files
- **Hot Reload**: Real-time configuration updates
- **Advanced Validators**: Regex, range, format validation

## Conclusion

The new configuration system provides a robust, type-safe, and extensible foundation for the NLM project while maintaining full backward compatibility with existing code. The system supports JSON, YAML, and legacy formats, with comprehensive validation and error reporting to ensure configuration integrity.
