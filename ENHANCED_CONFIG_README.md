# Enhanced Configuration System Documentation

This document provides comprehensive information about the enhanced configuration system in NLM.

## Overview

The enhanced configuration system represents a major upgrade from the original basic configuration system. It provides:

- **Comprehensive validation** with type checking, range validation, and dependency validation
- **User-friendly error messages** with recovery suggestions
- **Configuration profiles** for different use cases
- **Environment variable support** for automatic configuration
- **Atomic backup and restore** for reliability
- **Configuration help system** with parameter documentation
- **Performance optimizations** with lazy loading and caching
- **Extensible schema system** for easy parameter addition

## Key Improvements

### 1. Enhanced Configuration Schema
- **Schema-based validation**: Parameters defined with types, ranges, and validation rules
- **Profile-specific defaults**: Different defaults for research, simulation, production, development, and benchmark
- **Dependency validation**: Parameters can have dependencies and exclusions
- **Smart defaults**: Intelligent suggestions based on context

### 2. Comprehensive Error Handling
- **Structured errors**: Specific error types for different failure modes
- **User-friendly messages**: Clear, actionable error messages
- **Recovery suggestions**: Automatic suggestions for fixing errors
- **Detailed debugging**: Full error context for debugging

### 3. Enhanced Type Safety
- **Compile-time type checking**: Better type safety at compile time
- **Safe type conversions**: Safe conversions with warnings
- **Variant safety**: Better handling of ConfigValue variants
- **Template improvements**: More robust template implementations

### 4. Configuration Help System
- **Built-in help**: Automatic generation of configuration help
- **Parameter descriptions**: Detailed descriptions for each parameter
- **Usage examples**: Examples of how to use different configurations
- **Validation hints**: Hints for parameter validation

### 5. User-Friendly Features
- **Environment-based defaults**: Different defaults for different use cases
- **Profile-based defaults**: Predefined profiles (research, simulation, production)
- **Smart defaults**: Intelligent suggestions based on context
- **User preference storage**: Remember user preferences

### 6. Command-Line Enhancements
- **Better argument parsing**: More robust argument handling
- **Configuration file integration**: Seamless config file + CLI combination
- **Help system**: Comprehensive help for all options
- **Validation on startup**: Early validation of configuration

### 7. Example and Documentation System
- **Example config files**: Complete example configurations
- **Template configurations**: Templates for different use cases
- **Configuration wizards**: Interactive configuration helpers
- **Documentation generator**: Auto-generate documentation from schema

### 8. Performance and Reliability
- **Lazy loading**: Load configuration only when needed
- **Caching**: Cache configuration values for performance
- **Memory efficiency**: Reduce memory footprint
- **Thread safety**: Ensure thread-safe configuration access

### 9. Reliability Improvements
- **Atomic operations**: Atomic configuration updates
- **Consistency guarantees**: Consistent configuration state
- **Recovery mechanisms**: Recovery from corrupted configurations
- **Backup and restore**: Configuration backup and restore capabilities

### 10. Integration and Extensibility
- **Better integration**: Seamless integration with other NLM components
- **Plugin system**: Plugin architecture for configuration extensions
- **Configuration inheritance**: Support for hierarchical configuration
- **Version compatibility**: Backward compatibility support

## Files Created

### Core Configuration Files
- **`src/core/Config/Config.hpp`**: Enhanced original configuration class
- **`src/core/Config/Config.cpp`**: Original implementation (enhanced)
- **`src/core/Config/EnhancedConfig.hpp`**: New enhanced configuration class
- **`src/core/Config/EnhancedConfig.cpp`**: Enhanced configuration implementation

### Schema and Validation Files
- **`src/core/Config/ConfigSchema.hpp`**: Configuration schema definition
- **`src/core/Config/ConfigSchema.cpp`**: Schema implementation
- **`src/core/Config/ConfigError.hpp`**: Error handling system
- **`src/core/Config/ConfigError.cpp`**: Error handling implementation
- **`src/core/Config/ConfigHelp.hpp`**: Configuration help system
- **`src/core/Config/ConfigHelp.cpp`**: Help system implementation

### Example Configuration Files
- **`configs/default.cfg`**: Default configuration (backward compatible)
- **`configs/research.cfg`**: Research profile configuration
- **`configs/production.cfg`**: Production profile configuration
- **`configs/development.cfg`**: Development profile configuration
- **`configs/benchmark.cfg`**: Benchmark profile configuration

### Documentation Files
- **`docs/EnhancedConfig.md`**: Comprehensive documentation
- **`README.md`**: Project overview
- **`HOW_TO_USE.md`**: Usage instructions

### Example Programs
- **`examples/demo_enhanced_config.cpp`**: Demonstration program

## Building the Enhanced Configuration System

### Prerequisites
- C++20 compiler
- CMake 3.16 or higher
- Standard build tools

### Build Instructions

1. **Create build directory**:
```bash
mkdir build
cd build
```

2. **Configure CMake**:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
```

3. **Build the project**:
```bash
make -j4
```

4. **Run tests**:
```bash
ctest --output-on-failure
```

### Build Options

- **`CMAKE_BUILD_TYPE`**: Set build type (Debug, Release, RelWithDebInfo, MinSizeRel)
- **`NLM_ENABLE_CONFIG_VALIDATION`**: Enable configuration validation (default: ON)
- **`NLM_ENABLE_CONFIG_BACKUPS`**: Enable configuration backups (default: ON)
- **`NLM_ENABLE_CONFIG_HELP`**: Enable configuration help system (default: ON)

## Usage Examples

### Basic Usage

```cpp
#include <nlm/config.h>

int main() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Load configuration from file
    config.loadFromFile("config.cfg");
    
    // Load from command line
    config.loadFromArgs(argc, argv);
    
    // Load from environment variables
    config.loadFromEnvironment();
    
    // Set values programmatically
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", 1000);
    
    // Get values
    double timestep = config.get<double>("simulation_timestep").value_or(0.001);
    int neurons = config.get<int>("neuron_count").value_or(1000);
    
    return 0;
}
```

### Using Profiles

```cpp
#include <nlm/config.h>

int main() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Load research profile
    config.loadFromProfile(nlm::ConfigProfile::Research, schema);
    
    // Modify specific parameters
    config.set("neuron_count", 500);
    
    // Save configuration
    config.saveToFile("research_config.cfg");
    
    return 0;
}
```

### Configuration Validation

```cpp
#include <nlm/config.h>

int main() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Validate configuration
    if (!config.validate(schema)) {
        auto errors = config.validateAll(schema);
        for (const auto& error : errors) {
            std::cerr << "Error: " << error.toString() << std::endl;
            std::cerr << "Suggestion: " << config.getErrorSuggestion(error) << std::endl;
        }
        return 1;
    }
    
    std::cout << "Configuration is valid." << std::endl;
    return 0;
}
```

### Configuration Help

```cpp
#include <nlm/config.h>

int main() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Show help for a specific parameter
    std::string help = config.getHelp("simulation_timestep", schema);
    std::cout << help << std::endl;
    
    // Show help for all parameters
    std::string allHelp = config.getAllHelp(schema);
    std::cout << allHelp << std::endl;
    
    return 0;
}
```

### Configuration Backup and Restore

```cpp
#include <nlm/config.h>

int main() {
    nlm::Config config;
    
    // Create backup
    config.createBackup("before_experiment", "Backup before starting experiment");
    
    // Load configuration
    config.loadFromFile("experiment.cfg");
    
    // Modify configuration
    config.set("simulation_timestep", 0.0001);
    
    // Restore from backup if needed
    config.restoreBackup("before_experiment");
    
    return 0;
}
```

## Configuration Parameters

### Common Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds |
| `neuron_count` | int | 1000 | Number of neurons in the brain |
| `log_level` | string | INFO | Logging level (DEBUG, INFO, WARNING, ERROR) |
| `visualization_enabled` | bool | false | Enable/disable visualization |

### Parameter Categories

- **General**: Basic simulation settings
- **Neural**: Neural network parameters
- **Plasticity**: Learning and adaptation settings
- **Development**: Growth and maturation settings
- **Neuromodulation**: Reward and motivation settings
- **Environment**: World interaction settings
- **Logging**: Output and debugging settings
- **Visualization**: Display and rendering settings

## Configuration Profiles

### Research Profile
- **Description**: Optimized for research and experimentation
- **Key Features**: High precision settings, detailed logging, conservative plasticity rates
- **Best For**: Scientific research, experimentation, debugging

### Simulation Profile
- **Description**: Balanced settings for general simulation
- **Key Features**: Moderate logging, standard plasticity settings, good performance vs accuracy tradeoff
- **Best For**: General simulation, production testing

### Production Profile
- **Description**: Optimized for production environments
- **Key Features**: Minimal logging, aggressive performance settings, fast plasticity and development
- **Best For**: Production deployment, high-performance requirements

### Development Profile
- **Description**: Optimized for development and debugging
- **Key Features**: Maximum debugging and logging, slow simulation for debugging, safe defaults for experimentation
- **Best For**: Development, debugging, testing

### Benchmark Profile
- **Description**: Optimized for performance testing
- **Key Features**: Minimal configuration overhead, fast simulation settings, reduced logging overhead
- **Best For**: Performance benchmarking, stress testing

## Backward Compatibility

The enhanced configuration system maintains full backward compatibility:

- **File Format**: Old `.cfg` files continue to work
- **API**: Existing `Config` class interface unchanged
- **Default Values**: All existing default values preserved
- **Command Line**: Existing command-line interface unchanged
- **Profiles**: New profiles are additive, not breaking

## Migration Guide

### From Old to New Configuration

If you're migrating from an older version of NLM, you may need to:

1. **Update configuration files**: Old format files are still supported, but new features require the new format
2. **Update code**: No code changes are required for basic usage
3. **Update profiles**: New profiles are available for different use cases

### Example Migration

```cpp
// Old configuration loading
nlm::Config config;
config.loadFromFile("config.cfg");
config.loadFromArgs(argc, argv);

// New configuration loading (same code works)
nlm::Config config;
nlm::ConfigSchema schema;
config.loadFromFile("config.cfg");  // Still works
config.loadFromArgs(argc, argv);    // Still works

// New features (additional code)
config.loadFromEnvironment();       // Load from environment
config.loadFromProfile(nlm::ConfigProfile::Research, schema);  // Load profile
```

## Troubleshooting

### Common Issues

1. **Configuration validation fails**: Check that all required parameters are set and have valid values
2. **File not found**: Ensure the configuration file exists and is readable
3. **Type mismatch**: Verify that the configuration value type matches the parameter type
4. **Range violation**: Check that the value is within the valid range
5. **Dependency missing**: Ensure that all dependencies are set before using a parameter

### Error Messages

The enhanced configuration system provides detailed error messages:

```
Error[1]: Invalid value for parameter 'simulation_timestep' (Key: 'simulation_timestep' - Suggestion: Try a value within the valid range)
Error[2]: Missing required dependency 'random_seed' (Key: 'neuron_count' - Suggestion: Set dependency 'random_seed' first)
Error[3]: Parameter 'simulation_timestep' conflicts with 'environment_width' (Key: 'simulation_timestep' - Suggestion: Remove one of the conflicting parameters)
```

### Getting Help

If you're having trouble with configuration, use the help system:

```bash
# Show help for all parameters
./demo --help

# Show help for a specific parameter
./demo --help=simulation_timestep

# Show help for a category
./demo --help=category=simulation
```

## Performance Considerations

1. **Lazy Loading**: Configuration is loaded only when needed
2. **Caching**: Frequently accessed values are cached
3. **Memory Efficiency**: Efficient storage and access patterns
4. **Thread Safety**: All operations are thread-safe

## Reliability Features

1. **Atomic Operations**: All configuration updates are atomic
2. **Consistency**: Configuration state is always consistent
3. **Recovery**: Automatic backup and restore capabilities
4. **Validation**: Comprehensive validation of all parameters

## Testing

### Running Tests

```bash
# Run all tests
cmake --build build --target test

# Run specific tests
cmake --build build --target test_test_config
```

### Test Coverage

The enhanced configuration system includes tests for:

- Basic configuration operations
- Configuration validation
- Configuration help system
- Configuration profiles
- Configuration backup and restore
- Command-line argument parsing
- Environment variable loading
- Error handling

## Conclusion

The enhanced configuration system represents a significant improvement over the original basic configuration system. It provides:

- **Better user experience**: Intuitive help system and recovery suggestions
- **Improved reliability**: Comprehensive validation and backup/restore
- **Enhanced performance**: Optimized for speed and memory efficiency
- **Easy extension**: Simple to add new parameters and features
- **Full backward compatibility**: Existing code and configurations continue to work

The enhanced configuration system is now ready for production use and provides a solid foundation for future enhancements.