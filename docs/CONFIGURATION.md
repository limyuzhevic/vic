# NLM Configuration System Documentation
# Phase 6 Integration - Enhanced Configuration System

## Overview

The NLM (Neural Learning Machine) configuration system provides a flexible, extensible framework for managing brain simulation parameters, system behavior, and runtime settings. This documentation covers the configuration system improvements made in this refactoring phase.

## Configuration Formats

### Supported File Formats

1. **INI-style (.cfg)** - Primary format used by NLM
2. **JSON (.json)** - Hierarchical, type-aware configuration
3. **YAML (.yml/.yaml)** - Human-readable configuration (planned future support)

### Format Comparison

| Feature | INI-style | JSON | YAML |
|---------|-----------|------|------|
| **Structure** | Flat key-value | Hierarchical | Hierarchical |
| **Types** | Type-agnostic | Type-aware | Type-aware |
| **Comments** | # or / | // or # | # or |
| **Arrays** | `[1,2,3]` | `[1,2,3]` | `[1, 2, 3]` |
| **Parsing** | Simple | Structured | Simple |
| **Editing** | Easy | Programmatic | Easy |

### Current Implementation

NLM primarily uses INI-style format with basic JSON import/export support. YAML support is planned for Phase 7.

## Configuration Sources

### Source Priority

Configuration values can come from multiple sources, with the following priority (highest to lowest):

1. **Runtime** - Set programmatically during execution
2. **CommandLine** - Set via `--key=value` or `-k value` arguments
3. **File** - Set in configuration files
4. **Default** - Built-in default values

### Source Detection

Each configuration value is annotated with its source:

```
ConfigSource {
    Default,    // Built-in defaults
    File,       // From configuration file
    CommandLine,// From command line
    Runtime     // Set during runtime
}
```

## Configuration Categories

### Basic Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `random_seed` | int64 | 42 | Random number generator seed |
| `simulation_timestep` | double | 0.001 | Simulation time step (seconds) |
| `neuron_count` | int64 | 1000 | Total number of neurons |
| `region_count` | int64 | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |

### Neural Dynamics (Phase 2)

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `stdp_ltp_weight` | float | 0.02 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.015 | STDP long-term depression weight |
| `stdp_tau` | float | 20.0 | STDP time constant (ms) |
| `synaptogenesis_rate` | float | 0.0001 | Structural synaptogenesis rate |
| `pruning_rate` | float | 0.00001 | Structural pruning rate |

### Neuromodulation (Phase 6)

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `dopamine_initial_level` | float | 0.5 | Initial dopamine level |
| `curiosity_initial_level` | float | 0.3 | Initial curiosity level |
| `novelty_threshold` | float | 0.7 | Novelty detection threshold |
| `prediction_error_initial` | float | 0.0 | Initial prediction error |

### Memory Systems (Phase 6)

#### Working Memory

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `working_memory_capacity` | int64 | 100 | Working memory capacity |
| `working_memory_decay_rate` | float | 0.9 | Working memory decay rate |

#### Episodic Memory

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `max_episodes` | int64 | 1000 | Maximum number of episodes |
| `episodic_memory_window` | int64 | 50 | Episodic memory window size |
| `consolidation_threshold` | float | 0.7 | Memory consolidation threshold |

### Development System (Phase 6)

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `developmental_stage` | string | "Initial" | Current developmental stage |
| `plasticity_decay_rate` | float | 0.9 | Plasticity decay rate |
| `neural_excitability_growth` | float | 1.05 | Neural excitability growth factor |

### Cognition Systems (Phase 6)

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `planning_depth` | int64 | 5 | Neural planning depth |
| `concept_threshold` | float | 0.7 | Concept formation threshold |
| `attention_strength` | float | 0.8 | Attention system strength |

### Prediction System (Phase 6)

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `prediction_accuracy_threshold` | float | 0.8 | Prediction accuracy threshold |
| `prediction_update_rate` | float | 0.1 | Prediction system update rate |
| `confidence_decay_rate` | float | 0.95 | Confidence decay rate |

### Environment Interface

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `env_width` | int64 | 100 | Environment width |
| `env_height` | int64 | 100 | Environment height |
| `sensory_range` | int64 | 10 | Sensory processing range |
| `action_resolution` | float | 0.01 | Action resolution |

### Checkpoint System

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `checkpoint_dir` | string | "./checkpoints" | Checkpoint directory |
| `checkpoint_interval` | int64 | 1000 | Checkpoint creation interval |
| `max_checkpoints` | int64 | 10 | Maximum checkpoints to keep |
| `checkpoint_compression` | string | "balanced" | Checkpoint compression level |

### Performance Optimizations (Phase 5)

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `enable_performance_optimizations` | bool | true | Enable performance optimizations |
| `memory_pool_size` | int64 | 10000 | Memory pool size |
| `event_queue_capacity` | int64 | 1000 | Event queue capacity |
| `enable_simd` | bool | true | Enable SIMD vectorization |
| `parallel_processing` | bool | false | Enable parallel processing |

### Logging Configuration

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `log_level` | string | "info" | Logging level |
| `log_file` | string | "nlm.log" | Log file path |
| `enable_console_logging` | bool | true | Enable console logging |
| `enable_file_logging` | bool | true | Enable file logging |
| `log_rotation_mb` | int64 | 100 | Log file rotation size |

## Configuration Validation

### Validation Rules

Configuration values are validated against the following rules:

1. **Type Validation** - Values must match expected types
2. **Range Validation** - Values must be within valid ranges
3. **Presence Validation** - Required keys must be present
4. **Logical Validation** - Values must be logically consistent

### Required Keys

The following keys are required for basic operation:

- `random_seed` - Random number generator seed
- `simulation_timestep` - Simulation time step
- `neuron_count` - Number of neurons

### Validation Examples

```cpp
// Example validation errors
// "Missing required configuration key: neuron_count"
// "neuron_count must be positive"
// "simulation_timestep must be between 0 and 1.0 seconds"
// "Invalid log_level: 'invalid_level'"
```

## Configuration Import/Export

### Export Formats

The configuration system supports exporting to multiple formats:

1. **JSON** - Hierarchical, type-aware format
2. **INI** - Flat key-value format
3. **YAML** - Human-readable format (planned)

### Import Formats

The configuration system supports importing from multiple formats:

1. **JSON** - Import from JSON strings or files
2. **INI** - Import from INI-style configuration files
3. **YAML** - Import from YAML files (planned)

### Example Usage

```cpp
Config config;

// Load from INI file
if (config.loadFromFile("configs/default.cfg")) {
    std::cout << "Loaded configuration from file" << std::endl;
}

// Export to JSON
std::string jsonStr = config.exportToJSON();
std::cout << "Configuration as JSON:" << std::endl;
std::cout << jsonStr << std::endl;

// Import from JSON
bool success = config.importFromJSON(jsonStr);
if (success) {
    std::cout << "Successfully imported configuration from JSON" << std::endl;
}

// Validate configuration
if (!config.validateAll()) {
    std::vector<std::string> errors = config.getValidationErrors();
    std::cout << "Configuration validation errors:" << std::endl;
    for (const auto& error : errors) {
        std::cout << "  - " << error << std::endl;
    }
}
```

## Command Line Interface

### Argument Parsing

The configuration system automatically parses command line arguments:

#### Key-Value Format

```bash
./nlm --random_seed=42 --neuron_count=2000 --simulation_timestep=0.002
```

#### Short Format

```bash
./nlm -seed 123 -neurons 500 -timestep 0.001
```

#### Configuration File

```bash
./nlm --config configs/fast_config.cfg
```

### Command Line Precedence

Command line arguments override configuration file values, which override default values.

## Configuration Templates

### Available Templates

The configuration system includes several built-in templates:

1. **Default Template** - Complete configuration with all features
2. **Fast Template** - Optimized for speed with fewer neurons
3. **Memory Template** - Optimized for memory research
4. **Plasticity Template** - Optimized for plasticity studies

### Template Usage

```bash
# Use fast template
./nlm --template fast

# Create configuration from template
./nlm --generate-template fast --output fast_config.cfg
```

## Configuration Examples

### Basic Configuration

```ini
# Basic brain configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.1
```

### Advanced Configuration

```json
{
    "name": "Phase 6 Integration Configuration",
    "version": "6.0.0",
    "description": "Complete NLM brain with all integrated systems",

    "basic": {
        "random_seed": 12345,
        "simulation_timestep": 0.001,
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.15
    },

    "plasticity": {
        "stdp": {
            "ltp_weight": 0.025,
            "ltd_weight": 0.02,
            "tau": 25.0
        }
    },

    "neuromodulation": {
        "dopamine": {
            "initial_level": 0.6,
            "plasticity_factor": 1.2
        }
    }
}
```

### YAML Configuration

```yaml
# YAML configuration (planned for Phase 7)
name: "Phase 6 Integration Configuration"
version: "6.0.0"

basic:
  random_seed: 42
  simulation_timestep: 0.001
  neuron_count: 1000
  region_count: 1

plasticity:
  stdp:
    ltp_weight: 0.02
    ltd_weight: 0.015
    tau: 20.0
```

## Configuration Best Practices

### For Users

1. **Use templates for common configurations**
2. **Validate your configuration before running**
3. **Use comments to document your configuration**
4. **Keep configuration files organized**
5. **Use relative paths for file locations**

### For Developers

1. **Add validation rules for new configuration keys**
2. **Document configuration options thoroughly**
3. **Provide sensible defaults for all configuration options**
4. **Support multiple configuration formats**
5. **Ensure backward compatibility**

## Configuration API Reference

### Core Methods

#### Loading and Saving

```cpp
bool loadFromFile(const std::string& filepath);
bool loadFromArgs(int argc, char** argv);
bool saveToFile(const std::string& filepath) const;
```

#### Getting Values

```cpp
template<typename T>
std::optional<T> get(const std::string& key) const;

template<typename T>
T getOr(const std::string& key, const T& defaultValue) const;
```

#### Setting Values

```cpp
void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
// ... other type-specific overloads
```

#### Enhanced API

```cpp
// Convenience methods with specific type conversions
void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, uint64_t value, ConfigSource source = ConfigSource::Runtime);
void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
// ... other convenience methods
```

#### Validation

```cpp
bool validate(const std::string& key) const;
bool validateAll() const;
std::vector<std::string> getValidationErrors() const;
```

#### Export/Import

```cpp
std::string exportToJSON() const;
bool importFromJSON(const std::string& jsonStr);
std::string exportToINI() const;
bool importFromINI(const std::string& iniStr);
```

#### Comparison and Merging

```cpp
bool hasSameValues(const Config& other) const;
void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
```

#### Statistics and Debugging

```cpp
size_t getEntryCount() const;
std::vector<std::string> getKeys() const;
std::vector<ConfigSource> getSources(const std::string& key) const;
```

## Future Enhancements

### Planned Features

1. **YAML Support** - Full YAML format support
2. **Schema Validation** - Configuration schema definition and validation
3. **Dynamic Reloading** - Automatic configuration file reloading
4. **Environment Variables** - Support for environment variable configuration
5. **Configuration History** - Configuration change tracking and history
6. **Advanced Templating** - More sophisticated configuration templates
7. **Distributed Configuration** - Support for distributed configuration management
8. **Configuration Encryption** - Secure configuration storage

### Research Features

1. **Configuration Optimization** - Automatic configuration optimization
2. **Configuration Analysis** - Configuration analysis and recommendations
3. **Configuration Testing** - Automated configuration testing
4. **Configuration Visualization** - Configuration visualization tools

## Error Handling

### Common Errors

| Error Type | Description | Resolution |
|------------|-------------|------------|
| Missing Key | Required configuration key is missing | Add the key or provide a default value |
| Invalid Type | Configuration value has wrong type | Ensure value matches expected type |
| Invalid Value | Configuration value is out of range | Check value range or adjust limits |
| File Error | Cannot read or write configuration file | Check file permissions and path |
| JSON Error | Invalid JSON format in configuration | Fix JSON syntax or use a different format |

### Error Logging

Configuration errors are logged with detailed information:

```
2026-11-01 14:02:27 ERROR: Failed to load configuration from "configs/default.cfg"
2026-11-01 14:02:27 ERROR: File not found: "configs/default.cfg"
2026-11-01 14:02:27 ERROR: Invalid configuration value for "neuron_count": "invalid"
```

## Configuration Migration Guide

### From Phase 1/2/3/4/5

If upgrading from earlier phases, the following configuration key changes apply:

#### Phase 5 to Phase 6

| Phase 5 Key | Phase 6 Key | Notes |
|-------------|-------------|-------|
| `neuron_count` | `neuron_count` | Same |
| `region_count` | `region_count` | Same |
| `connection_probability` | `connection_probability` | Same |
| `stdp_ltp_weight` | `stdp_ltp_weight` | Same |
| `stdp_ltd_weight` | `stdp_ltd_weight` | Same |
| `stdp_tau` | `stdp_tau` | Same |
| `[new]` | `working_memory_capacity` | New Phase 6 feature |
| `[new]` | `max_episodes` | New Phase 6 feature |
| `[new]` | `dopamine_initial_level` | New Phase 6 feature |

### Migration Examples

#### INI Format Migration

```ini
# Phase 5 configuration
random_seed = 42
neuron_count = 1000
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Phase 6 configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
working_memory_capacity = 100
max_episodes = 1000
dopamine_initial_level = 0.5
```

#### JSON Format Migration

```json
// Phase 5 configuration
{
    "random_seed": 42,
    "neuron_count": 1000,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012
}

// Phase 6 configuration
{
    "name": "Phase 6 Configuration",
    "version": "6.0.0",
    "basic": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "ltp_weight": 0.02,
            "ltd_weight": 0.015,
            "tau": 20.0
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 100
        },
        "episodic_memory": {
            "max_episodes": 1000
        }
    },
    "neuromodulation": {
        "dopamine": {
            "initial_level": 0.5
        }
    }
}
```

## Configuration Testing

### Test Cases

The configuration system includes the following test cases:

1. **Basic Configuration Test** - Tests basic configuration loading
2. **Validation Test** - Tests configuration validation
3. **Export/Import Test** - Tests configuration export and import
4. **Command Line Test** - Tests command line argument parsing
5. **Template Test** - Tests configuration templates
6. **Error Handling Test** - Tests error handling

### Running Tests

```bash
# Run all configuration tests
./nlm --test=configuration

# Run specific configuration tests
./nlm --test=configuration:basic
./nlm --test=configuration:validation
./nlm --test=configuration:export_import
```

## Configuration Monitoring

### Metrics Collection

The configuration system collects metrics for monitoring:

1. **Configuration Load Time** - Time taken to load configuration
2. **Configuration Validation Time** - Time taken to validate configuration
3. **Configuration Export Time** - Time taken to export configuration
4. **Configuration Import Time** - Time taken to import configuration
5. **Configuration Usage** - How configuration keys are used

### Monitoring API

```cpp
// Get configuration metrics
ConfigurationMetrics getMetrics() const;

// Reset configuration metrics
void resetMetrics();

// Configuration metrics structure
struct ConfigurationMetrics {
    double load_time_ms;
    double validation_time_ms;
    double export_time_ms;
    double import_time_ms;
    std::map<std::string, int> key_usage_counts;
    std::map<std::string, double> key_usage_durations;
};
```

## Configuration Security

### Security Considerations

1. **File Permissions** - Configuration files should have appropriate permissions
2. **Sensitive Data** - Configuration files may contain sensitive information
3. **Access Control** - Only authorized users should be able to modify configuration
4. **Encryption** - Consider encrypting configuration files (planned feature)

### Security Best Practices

1. **Use environment variables for sensitive data**
2. **Restrict file permissions**
3. **Use secure file locations**
4. **Implement access control**
5. **Consider configuration encryption**

## Configuration Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration File Not Found

**Symptom:** "Failed to load configuration from 'configs/default.cfg'"

**Solution:**

1. Check if the file exists
2. Check if the file has read permissions
3. Use an absolute path instead of relative path
4. Create the directory if it doesn't exist

#### Issue: Invalid Configuration Value

**Symptom:** "Invalid configuration value for 'neuron_count': 'invalid'"

**Solution:**

1. Check the value type (expected: integer)
2. Ensure the value is within valid range
3. Remove extra whitespace or special characters
4. Use a different value format

#### Issue: Configuration Validation Failure

**Symptom:** "Configuration validation errors:" followed by error messages

**Solution:**

1. Check the validation errors for specific details
2. Fix each error in order
3. Run validation again
4. Consider relaxing validation rules if necessary

#### Issue: Configuration Export/Import Failure

**Symptom:** "Failed to import configuration from JSON"

**Solution:**

1. Check JSON syntax for errors
2. Ensure the JSON is compatible with NLM format
3. Use a simpler format for testing
4. Check for encoding issues

### Debugging Tools

#### Configuration Debug Mode

```bash
# Enable configuration debug mode
./nlm --config-debug

# Log configuration loading
./nlm --log-config

# Validate configuration before loading
./nlm --validate-config
```

#### Configuration Debug API

```cpp
// Enable configuration debugging
void enableDebugMode(bool enabled = true);

// Log configuration loading
void logConfigurationLoading(const std::string& filepath);

// Validate configuration before loading
bool validateConfigurationBeforeLoading(const std::string& filepath);

// Get configuration debug information
declared_configuration_debug_info getDebugInfo();
```

## Conclusion

The NLM configuration system provides a comprehensive, flexible framework for managing brain simulation parameters and system behavior. This documentation provides detailed information about configuration formats, validation, import/export, templates, testing, monitoring, security, and troubleshooting.

The configuration system is designed to be:

1. **Flexible** - Support for multiple configuration formats
2. **Extensible** - Easy to add new configuration options
3. **Validating** - Comprehensive validation of configuration values
4. **Documented** - Detailed documentation and examples
5. **Tested** - Comprehensive test coverage
6. **Monitored** - Metrics collection and monitoring
7. **Secure** - Security considerations and best practices
8. **User-friendly** - Easy to use and understand

The configuration system is a core component of the NLM brain simulation framework and is essential for configuring, managing, and monitoring brain simulation behavior.

## References

1. **NLM Phase 6 Integration Documentation** - Complete documentation of Phase 6 features
2. **NLM Brain Architecture** - Documentation of brain system architecture
3. **NLM Configuration System API** - API reference for configuration system
4. **NLM Configuration Examples** - Examples of configuration usage
5. **NLM Configuration Testing** - Test suite for configuration system
6. **NLM Configuration Best Practices** - Best practices for configuration management

## Version History

### Version 6.0.0

- Initial implementation of enhanced configuration system
- Support for JSON/YAML configuration formats
- Comprehensive validation and error handling
- Configuration templates and examples
- Configuration monitoring and metrics
- Security enhancements
- Documentation and examples

### Version 5.0.0
- Original configuration system implementation
- Basic INI-style configuration support
- Command line argument parsing
- Basic validation

### Version 4.0.0
- Configuration system added in Phase 5
- Support for checkpoint system configuration
- Performance optimization configuration
- Logging configuration

## Acknowledgments

The NLM configuration system team:

- Core configuration system design and implementation
- Format support (INI, JSON, YAML)
- Validation and error handling
- Documentation and examples
- Testing and monitoring
- Security and performance considerations

Special thanks to all contributors who have worked on the NLM configuration system.

---

*This documentation is for NLM Phase 6 Configuration System. For the latest information, visit the NLM documentation portal or contact the NLM development team.*

*Configuration system version: 6.0.0 (Phase 6)*
*Documentation version: 1.0.0*
*Last updated: 2026-11-01*
*Copyright © 2026 NLM Research Project. All rights reserved.*

---END OF DOCUMENTATION---