# NLM Configuration System Documentation

## Overview

The NLM (Neural Learning Machine) configuration system provides a flexible, extensible framework for managing brain simulation parameters. It supports multiple configuration formats, validation, presets, and comprehensive error handling.

## Configuration Formats

### 1. Legacy Key=Value Format
```
# Simple key=value pairs
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
```

### 2. JSON Format
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "synaptogenesis_rate": 0.001,
    "pruning_rate": 0.0001,
    "enable_development": true,
    "enable_checkpointing": true,
    "enable_replay": true
}
```

## Configuration File Structure

### Configuration Files
- **`configs/default.cfg`** - Default configuration for standard usage
- **`configs/`** - Directory containing all configuration files

### File Format Notes
1. **Comments**: Lines starting with `#` or `/` are ignored
2. **Values**: Strings can be quoted with double or single quotes
3. **Arrays**: Supported as `[value1, value2, value3]`
4. **Types**: Automatic type detection (int, double, bool, string, arrays)

## Configuration System Components

### Config Class
**Location**: `src/core/Config/Config.hpp`

**Main Interface**: The `Config` class provides the primary interface for:
- Loading configuration from files and command line
- Setting and getting configuration values
- Validating configuration against schemas
- Saving configuration to files

### ConfigEntry Structure
```cpp
struct ConfigEntry {
    std::string key;          // Configuration key
    ConfigValue value;         // Configuration value (variant type)
    ConfigSource source;       // Configuration source (Default, File, CommandLine, Runtime)
    std::string description;  // Description of the configuration option
};
```

### ConfigSource Enum
```cpp
enum class ConfigSource {
    Default,       // Default values from the code
    File,          // Values loaded from configuration file
    CommandLine,   // Values from command line arguments
    Runtime        // Values set at runtime
};
```

### ConfigValue Variant
The configuration system supports multiple value types:
- Primitive: `int`, `int64_t`, `double`, `bool`
- String: `std::string`
- Collections: `std::vector<int>`, `std::vector<double>`, `std::vector<std::string>`

## Configuration Validation

### Built-in Validation
The system provides comprehensive validation for common configuration parameters:

#### Required Keys
- `random_seed` - Random seed for reproducible experiments
- `neuron_count` - Number of neurons in the brain (must be > 0)
- `simulation_timestep` - Simulation timestep in seconds (must be > 0)
- `region_count` - Number of brain regions (must be > 0)

#### Type Validation
- Ensures correct data types for each configuration key
- Validates that numeric values are within acceptable ranges
- Checks string formats where appropriate

#### Range Validation
- `neuron_count`: Must be positive, warning for very large values (>100,000)
- `simulation_timestep`: Must be positive, warning for large values (>0.1s)
- `connection_probability`: Must be between 0.0 and 1.0

### Schema Validation
The configuration system supports custom validation schemas:

```cpp
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage(const std::string& key, const ConfigValue& value) const = 0;
};
```

### Configuration Validation Methods

#### Manual Validation
```cpp
Config config;
if (config.loadFromFile("config.json")) {
    if (config.validate()) {
        // Configuration is valid
        int neurons = config.getOr<int64_t>("neuron_count", 1000);
    } else {
        // Configuration has validation errors
        auto errors = config.getValidationErrors();
    }
}
```

#### Automatic Validation
Configuration values are automatically validated when loaded, and warnings are issued for potential issues:

```cpp
// Examples of automatic validation warnings:
// - "Large neuron_count may impact performance: 5000"
// - "Large simulation timestep may affect accuracy: 0.15s"
// - "Invalid connection_probability: must be between 0.0 and 1.0 (got 1.2)"
```

## Configuration Presets

### Available Presets

#### `fast_test`
Optimized for quick testing and development:
```cpp
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 100,                    // Reduced for fast test
    "region_count": 1,
    "connection_probability": 0.15
}
```

#### `standard`
Standard configuration for normal usage:
```cpp
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
}
```

#### `high_density`
High-density configuration for research and performance testing:
```cpp
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 5000,                   // High density
    "region_count": 4,
    "connection_probability": 0.2           // Higher connectivity
}
```

### Using Presets
```cpp
Config config;
config.applyPreset("fast_test");      // Apply fast test preset
config.applyPreset("standard");       // Apply standard preset
config.applyPreset("high_density");   // Apply high density preset
```

## Configuration Management

### Loading Configuration
```cpp
Config config;

// Load from JSON file
bool success = config.loadFromFile("config.json");

// Load from command line
config.loadFromArgs(argc, argv);

// Override with command line
config.set("neuron_count", 2000, ConfigSource::CommandLine);
```

### Saving Configuration
```cpp
// Save configuration to file
bool saved = config.saveToFile("output_config.cfg");

// Get configuration summary
std::string summary = config.summary();
```

### Getting and Setting Values
```cpp
// Get values with type conversion
int seed = config.getOr<int64_t>("random_seed", 42);
double timestep = config->getOr<double>("simulation_timestep", 0.001);
bool enableDev = config->getOr<bool>("enable_development", false);

// Set values with source tracking
config->set("neuron_count", 1500, ConfigSource::File);
config->set("enable_development", true, ConfigSource::CommandLine);
```

### Configuration Queries
```cpp
// Check if key exists
bool hasKey = config->has("neuron_count");

// Get all keys
std::vector<std::string> keys = config->getKeys();

// Get all entries
std::vector<ConfigEntry> entries = config->getAllEntries();
```

## Error Handling

### Error Reporting
The configuration system provides comprehensive error reporting:

#### Configuration Errors
- **Invalid key names**: Keys with invalid characters
- **Type mismatches**: Incorrect value types for keys
- **Range violations**: Values outside acceptable ranges
- **Missing required keys**: Keys that are required but not provided

#### Warnings
- **Performance warnings**: Large values that may impact performance
- **Accuracy warnings**: Values that may affect simulation accuracy
- **Deprecated usage**: Use of deprecated configuration options

### Error Recovery
The system provides multiple recovery mechanisms:

1. **Graceful degradation**: Falls back to default values when invalid values are provided
2. **Selective validation**: Allows configuration to be used even with some warnings
3. **Source prioritization**: Command line overrides file overrides defaults

## Integration with NLM

### Integration with Brain Class
The `Config` class is integrated with the main `Brain` class:

```cpp
// Create brain with configuration
auto brain = std::make_shared<Brain>(config);
brain->initialize();
```

### Configuration in Main Application
```cpp
int main(int argc, char** argv) {
    // Load and validate configuration
    auto config = loadConfiguration(argc, argv);
    
    if (!config || !config->validate()) {
        std::cerr << "ERROR: Invalid configuration!" << std::endl;
        return 1;
    }
    
    // Initialize brain with validated configuration
    std::shared_ptr<Brain> brain;
    initializeBrain(config, brain);
    
    // Run simulation
    runPhase2Demo(brain);
}
```

## Configuration Examples

### Example 1: Simple Fast Test Configuration
```bash
./nlm --config configs/fast_test.cfg
```

### Example 2: Command Line Overrides
```bash
./nlm \
    --config configs/default.cfg \
    --neuron_count=2000 \
    --enable_development=true \
    --stdp_ltp_weight=0.05
```

### Example 3: JSON Configuration
```bash
cat > my_config.json << EOF
{
    "random_seed": 12345,
    "simulation_timestep": 0.002,
    "neuron_count": 3000,
    "region_count": 3,
    "connection_probability": 0.12,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.028,
    "enable_development": true,
    "enable_checkpointing": true,
    "enable_replay": true,
    "visualization_enabled": true
}
EOF

./nlm --config my_config.json
```

## Configuration File Locations

### Standard Locations
1. **Current directory**: `./configs/default.cfg`
2. **User directory**: `~/.nlm/config.cfg`
3. **Command line**: `--config <filepath>`

### Configuration Search Order
1. Command line arguments (highest priority)
2. Configuration file
3. Default values (lowest priority)

## Advanced Configuration Features

### Custom Validators
```cpp
class CustomValidator : public ConfigValidator {
public:
    bool validate(const std::string& key, const ConfigValue& value) const override {
        if (key == "neuron_count") {
            auto neurons = std::get<int64_t>(value);
            return neurons > 0 && neurons <= 1000000;
        }
        return true;
    }
    
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        return "neuron_count must be between 1 and 1,000,000";
    }
};
```

### Configuration Monitoring
```cpp
// Monitor configuration changes
config->setValidator(std::make_shared<CustomValidator>());
```

## Configuration Debugging

### Debug Commands
```bash
# Show configuration summary
./nlm --debug

# Validate configuration only
./nlm --validate-only

# Save configuration to file
./nlm --save-config output.cfg

# Load configuration from file
./nlm --load-config my_config.json
```

### Debug Information
The configuration system provides detailed debug information:

1. **Configuration sources**: Shows where each value came from
2. **Validation results**: Lists all warnings and errors
3. **Performance metrics**: Estimates computational requirements
4. **Dependency analysis**: Checks for inconsistent configuration

## Migration Guide

### From Legacy to New Configuration System

#### Old Format (Legacy Key=Value)
```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
```

#### New Format (JSON)
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
}
```

### Migration Steps
1. **Convert configuration files**: Convert existing key=value files to JSON
2. **Update code**: Update code that reads configuration to use new API
3. **Test thoroughly**: Test configuration loading and validation
4. **Monitor performance**: Monitor any performance impacts

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Configuration not loading
**Symptom**: `Failed to load configuration`

**Solution**:
- Check file permissions: `chmod 644 config.json`
- Verify file format: Ensure valid JSON or key=value format
- Check file location: Ensure file is in expected location

#### Issue 2: Invalid configuration values
**Symptom**: Validation errors or warnings

**Solution**:
- Check configuration types and ranges
- Use presets for standard configurations
- Override invalid values via command line

#### Issue 3: Performance issues
**Symptom**: Long loading times or high memory usage

**Solution**:
- Use `fast_test` preset for quick testing
- Reduce `neuron_count` for performance testing
- Monitor configuration size and complexity

#### Issue 4: Integration problems
**Symptom**: Brain initialization failures

**Solution**:
- Validate configuration before brain initialization
- Use `high_density` preset for large simulations
- Check for conflicting configuration options

### Getting Help

#### Debug Mode
Enable debug mode for detailed configuration information:
```bash
./nlm --debug --config configs/default.cfg
```

#### Configuration Wizard
Use the configuration wizard for guided setup:
```bash
./nlm --wizard
```

#### Support
For configuration issues, check the documentation:
- Configuration System Guide
- NLM User Manual
- Source code comments and examples

## Future Enhancements

### Planned Features
1. **YAML support**: Add YAML format support
2. **Environment variables**: Support for environment variable configuration
3. **Configuration inheritance**: Support for configuration file inheritance
4. **Hot configuration**: Support for runtime configuration changes
5. **Configuration validation schemas**: JSON Schema support for advanced validation

### Performance Improvements
1. **Lazy loading**: Load configuration values only when needed
2. **Caching**: Cache frequently used configuration values
3. **Compression**: Support for compressed configuration files
4. **Incremental updates**: Support for incremental configuration updates

## Conclusion

The NLM configuration system provides a comprehensive, flexible framework for managing brain simulation parameters. It supports multiple configuration formats, validation, presets, and comprehensive error handling. The system is designed to be:

- **Flexible**: Support for multiple configuration formats and custom validation
- **Robust**: Comprehensive validation and error recovery
- **Performant**: Optimized for both development and production use
- **User-friendly**: Clear documentation and easy-to-use API
- **Extensible**: Easy to add new configuration options and validation rules

The configuration system is a key component of the NLM framework, enabling users to easily configure and manage complex brain simulations while ensuring robustness and reliability.

---

*Document Version: 1.0*
*Last Updated: 2026-10-17*
*NLM Configuration Team*