# NLM Enhanced Configuration System

This document provides a comprehensive overview of the enhanced configuration system in NLM (熙然).

## Overview

The enhanced configuration system provides:

1. **Comprehensive validation** with type checking, range validation, and dependency validation
2. **User-friendly error messages** with recovery suggestions
3. **Configuration profiles** for different use cases (Research, Simulation, Production, Development, Benchmark)
4. **Environment variable support** for automatic configuration
5. **Atomic backup and restore** for reliability
6. **Configuration help system** with parameter documentation
7. **Performance optimizations** with lazy loading and caching
8. **Extensible schema system** for easy parameter addition

## Configuration Schema

The configuration system uses a schema-based approach where each parameter has:

- **Type**: integer, float, boolean, string, or list type
- **Default value**: Used when parameter is not set
- **Range validation**: Minimum and maximum allowed values
- **Description**: Human-readable explanation of the parameter
- **Category**: Logical grouping of parameters
- **Profile-specific defaults**: Different defaults for different profiles

## Configuration Sources (Priority Order)

1. **Profile defaults**: Base defaults for the selected profile
2. **Environment variables**: From environment variables prefixed with `NLM_`
3. **Configuration file**: From `.cfg`, `.json`, or `.yaml` files
4. **Command-line arguments**: From `--key=value` or `-key value` format
5. **Runtime settings**: From `Config::set()` calls

## Configuration Profiles

### Research Profile
- Optimized for research and experimentation
- High precision settings
- Detailed logging
- Conservative plasticity rates

### Simulation Profile
- Balanced settings for general simulation
- Moderate logging
- Standard plasticity settings
- Good performance vs accuracy tradeoff

### Production Profile
- Optimized for production environments
- Minimal logging
- Aggressive performance settings
- Fast plasticity and development

### Development Profile
- Maximum debugging and logging
- Slow simulation for debugging
- Detailed error reporting
- Safe defaults for experimentation

### Benchmark Profile
- Optimized for performance testing
- Minimal configuration overhead
- Fast simulation settings
- Reduced logging overhead

## Error Handling

The enhanced configuration system provides:

1. **Structured errors**: Specific error types for different failure modes
2. **User-friendly messages**: Clear, actionable error messages
3. **Recovery suggestions**: Automatic suggestions for fixing errors
4. **Detailed debugging**: Full error context for debugging

### Common Error Types

- **InvalidType**: Value type doesn't match parameter type
- **OutOfRange**: Value is outside valid range
- **MissingDependency**: Required dependency is not set
- **ConflictingValues**: Two parameters cannot be used together
- **FileNotFound**: Configuration file doesn't exist
- **ParseError**: Configuration file has syntax errors

## Configuration Help

### Using the Help System

Use the `Config::getHelp()` method to get detailed information about parameters:

```cpp
nlm::Config config;
nlm::ConfigSchema schema;

// Get help for a specific parameter
std::string help = config.getHelp("simulation_timestep", schema);
std::cout << help << std::endl;

// Get help for all parameters
std::string allHelp = config.getAllHelp(schema);
std::cout << allHelp << std::endl;

// Get help for a category
std::string categoryHelp = config.getCategoryHelp("Simulation", schema);
std::cout << categoryHelp << std::endl;
```

### Available Categories

- **General**: Basic simulation settings
- **Neural**: Neural network parameters
- **Plasticity**: Learning and adaptation settings
- **Development**: Growth and maturation settings
- **Neuromodulation**: Reward and motivation settings
- **Environment**: World interaction settings
- **Logging**: Output and debugging settings
- **Visualization**: Display and rendering settings

## Command-Line Interface

The enhanced configuration system provides a comprehensive command-line interface:

### Common Commands

```bash
# Load configuration from file
./program --config=config.cfg

# Set configuration via command line
./program --simulation_timestep=0.01 --neuron_count=1000

# Show help for all parameters
./program --help

# Validate current configuration
./program --validate

# Load configuration from environment variables
./program --load-env
```

### Command-Line Options

| Option | Description | Example |
|--------|-------------|---------|
| `--help` | Show help for all parameters | `--help` |
| `--validate` | Validate current configuration | `--validate` |
| `--load-env` | Load from environment variables | `--load-env` |
| `--profile=<name>` | Load configuration profile | `--profile=research` |
| `--config=<file>` | Load configuration from file | `--config=config.json` |

## Configuration Files

### Simple Format (.cfg)

The default format is a simple key=value format:

```
# NLM Configuration File
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Logging level (DEBUG, INFO, WARNING, ERROR)
log_level = INFO
```

### JSON Format (.json)

JSON format provides structured configuration:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "log_level": "INFO"
}
```

### YAML Format (.yaml)

YAML format provides hierarchical configuration:

```yaml
# NLM Configuration File
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
log_level: INFO
```

## Examples

### Basic Usage

```cpp
#include <nlm/config.h>

int main() {
    nlm::Config config;
    
    // Load from file
    config.loadFromFile("config.cfg");
    
    // Load from environment variables
    config.loadFromEnvironment();
    
    // Set values programmatically
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", 1000);
    
    // Get values
    double timestep = config.get<double>("simulation_timestep").value_or(0.001);
    int neurons = config.get<int>("neuron_count").value_or(1000);
    
    // Validate configuration
    if (!config.validate(schema)) {
        auto errors = config.validateAll(schema);
        for (const auto& error : errors) {
            std::cerr << "Error: " << error.toString() << std::endl;
        }
        return 1;
    }
    
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

## Configuration Schema Definition

### Adding a New Parameter

To add a new parameter to the configuration schema:

```cpp
// Define the parameter
nlm::ConfigParameter param("simulation_timestep", 
                           nlm::ConfigType::Float,
                           "Simulation timestep in seconds",
                           "Simulation");
param.defaultValue(0.001)
    .range(0.0001, 1.0)
    .positive()
    .nonNegative()
    .setProfileDefault("Research", 0.0001)
    .setProfileDefault("Simulation", 0.001)
    .setProfileDefault("Production", 0.01)
    .setProfileDefault("Development", 0.1)
    .setProfileDefault("Benchmark", 0.001);

// Add to schema
nlm::ConfigSchema schema;
schema.addParameter(param);
```

### Parameter Types

The configuration system supports the following types:

- **Integer**: 32-bit integer
- **Integer64**: 64-bit integer
- **Float**: 64-bit floating point
- **Boolean**: true/false
- **String**: Text string
- **StringList**: List of strings
- **IntList**: List of integers
- **FloatList**: List of floats

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

## Backward Compatibility

The enhanced configuration system maintains full backward compatibility:

- All existing configuration files are still valid
- Existing code using `Config` class continues to work
- Command-line interface remains the same
- Default values are preserved for existing parameters

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
./program --help

# Show help for a specific parameter
./program --help=simulation_timestep

# Show help for a category
./program --help=category=simulation
```

## Conclusion

The enhanced configuration system in NLM provides a robust, user-friendly, and extensible way to manage configuration. It maintains full backward compatibility while adding powerful new features for validation, error handling, and usability.

Key benefits:

- **Better user experience**: Intuitive help system and recovery suggestions
- **Improved reliability**: Comprehensive validation and backup/restore
- **Enhanced performance**: Optimized for speed and memory efficiency
- **Easy extension**: Simple to add new parameters and features
- **Full backward compatibility**: Existing code and configurations continue to work

The enhanced configuration system is now ready for production use and provides a solid foundation for future enhancements.