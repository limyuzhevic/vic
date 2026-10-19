# NLM Configuration System

This document describes the configuration system for NLM (Neural Learning Machine). This is the new enhanced configuration system that supports JSON format with nested objects and arrays.

## Overview

The NLM configuration system provides a flexible, type-safe configuration management system that supports:

- **JSON format**: Full JSON support with nested objects and arrays
- **Backward compatibility**: Maintains support for existing command-line arguments
- **Type conversion**: Automatic type inference for int, float, bool, string, and array types
- **Validation**: Configurable validation with custom validators
- **Error handling**: Detailed error messages and graceful fallbacks
- **Thread safety**: Thread-safe configuration operations
- **Source tracking**: Tracks configuration origin (file, command-line, runtime)

## Configuration File Formats

### JSON Configuration Files

JSON is the primary configuration format for NLM. Configuration files should have a `.json` extension.

**Example: `examples/configs/default.json`**

```json
{
  "version": "2.0",
  "name": "NLM Configuration",
  "brain": {
    "neuron_count": 1000,
    "connection_probability": 0.1,
    "plasticity": {
      "stdp": {
        "learning_rate": 0.001,
        "tau_plus": 20.0
      }
    }
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000
  }
}
```

**Key Features:**

- **Nested objects**: Configuration can be organized into nested structures
- **Arrays**: Support for arrays of values
- **Type safety**: JSON types map to NLM configuration types
- **Comments**: Not supported in standard JSON (use `")]/* comment */` for YAML)

### YAML Configuration Files

YAML format is also supported (optional). Configuration files can have a `.yaml` or `.yml` extension.

**Example: `config.yaml`**

```yaml
# NLM Configuration in YAML format
version: "2.0"
brain:
  neuron_count: 1000
  connection_probability: 0.1
  plasticity:
    stdp:
      learning_rate: 0.001
      tau_plus: 20.0
simulation:
  timestep: 0.001
  max_steps: 10000
```

### Simple Key=Value Files

For backward compatibility, NLM still supports the simple key=value format:

**Example: `configs/default.cfg`**

```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

## Configuration Loading

### Loading from Files

The `Config::loadFromFile()` method automatically detects the file format based on file extension:

1. **`.json`** files: Parsed using nlohmann/json library
2. **`.yaml`/`.yml`** files: Parsed using YAML-CPP (if available)
3. **Other files**: Parsed as simple key=value format

**Example:**

```cpp
auto config = std::make_shared<Config>();
if (config->loadFromFile("config.json")) {
    NLM_LOG_INFO("Configuration loaded successfully");
} else {
    NLM_LOG_WARN("Failed to load configuration, using defaults");
}
```

### Loading from Command Line

Configuration can be loaded from command line arguments:

```bash
./nlm --config=config.json --neuron_count=2000 --connection_probability=0.2 -random_seed 42
```

**Supported formats:**

- `--key=value` (for most types)
- `-key value` (for string values)
- Boolean values: `true`, `false`
- Integer values: `42`, `-5`
- Float values: `3.14`, `-2.5`
- Array values: `[1,2,3]` (JSON array syntax)

## Configuration Value Types

NLM supports the following configuration value types:

### Primitive Types

| Type | JSON Example | NLM Usage |
|------|-------------|-----------|
| `int` | `42` | `config->getOr<int>("brain.neuron_count", 1000)` |
| `int64_t` | `9223372036854775807` | `config->getOr<int64_t>("value", 0)` |
| `double` | `3.14` | `config->getOr<double>("simulation.timestep", 0.001)` |
| `bool` | `true` | `config->getOr<bool>("plasticity.enable", true)` |
| `std::string` | `"hello"` | `config->getOr<std::string>("name", "default")` |

### Array Types

| Type | JSON Example | NLM Usage |
|------|-------------|-----------|
| `std::vector<int>` | `[1, 2, 3]` | `config->getOr<std::vector<int>("list", {})` |
| `std::vector<double>` | `[1.0, 2.5, 3.14]` | `config->getOr<std::vector<double>("weights", {})` |
| `std::vector<std::string>` | `["a", "b", "c"]` | `config->getOr<std::vector<std::string>("tags", {})` |

## Configuration Access

### Getting Values

```cpp
// Get specific type with optional default
int neurons = config->getOr<int>("brain.neuron_count", 1000);
double timestep = config->getOr<double>("simulation.timestep", 0.001);
bool enabled = config->getOr<bool>("plasticity.enable", true);
std::string name = config->getOr<std::string>("brain.name", "Unnamed Brain");

// Check if key exists
if (config->has("brain.neuron_count")) {
    auto neurons = config->get<int>("brain.neuron_count");
}

// Get all keys
std::vector<std::string> keys = config->getKeys();
```

### Setting Values

```cpp
// Set with specific type and source
config->set("brain.neuron_count", 2000, ConfigSource::Runtime);
config->set("simulation.timestep", 0.0001, ConfigSource::CommandLine);
config->set("plasticity.enable", true, ConfigSource::File);
config->set("brain.name", std::string("MyBrain"), ConfigSource::Runtime);
```

## Configuration Validation

### Built-in Validation

The configuration system includes built-in validation for common configuration patterns:

```cpp
// Check for required brain parameters
if (!config->validate()) {
    auto warnings = config->getValidationWarnings();
    for (const auto& warning : warnings) {
        NLM_LOG_WARN("Validation warning: " + warning);
    }
}
```

### Custom Validators

Users can implement custom validators:

```cpp
class BrainValidator : public ConfigValidator {
public:
    bool validate(const ConfigEntry& entry) const override {
        if (entry.key == "brain.neuron_count") {
            // Validate neuron count
            if (auto val = std::get_if<int>(&entry.value)) {
                return *val > 0 && *val <= 100000;
            }
        }
        return true;
    }
    
    std::string getErrorMessage(const ConfigEntry& entry) const override {
        if (entry.key == "brain.neuron_count") {
            return "Neuron count must be between 1 and 100000";
        }
        return "Invalid value for " + entry.key;
    }
};

// Use custom validator
auto validator = std::make_unique<BrainValidator>();
config->setValidator(std::move(validator));
```

## Configuration Saving

### Saving to JSON

```cpp
if (config->saveToFile("config_output.json")) {
    NLM_LOG_INFO("Configuration saved to JSON file");
}
```

The saved JSON file will maintain the nested structure of the configuration:

```json
{
  "brain": {
    "neuron_count": 1000,
    "connection_probability": 0.1
  },
  "simulation": {
    "timestep": 0.001
  }
}
```

## Configuration Sources and Priority

Configuration values come from different sources with different priorities:

| Source | Priority | Description |
|--------|----------|-------------|
| **File** | 1 (Highest) | Configuration files (JSON/YAML/key=value) |
| **Command Line** | 2 | Command line arguments |
| **Runtime** | 3 | Programmatically set values |
| **Default** | 4 (Lowest) | Hard-coded default values |

**Priority Rules:**

- File settings override defaults
- Command-line settings override file settings
- Runtime settings override command-line settings
- Defaults are used only when no other source provides a value

## Examples and Templates

### Complete Configuration Example

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity": {
      "stdp": {
        "learning_rate": 0.001,
        "tau_plus": 20.0,
        "tau_minus": 20.0
      }
    }
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000,
    "enable_logging": true
  }
}
```

### Command Line Usage

```bash
# Load from JSON file
./nlm --config=config.json

# Override specific values
./nlm --config=config.json --brain.neuron_count=2000 --simulation.max_steps=5000

# Use simple key=value format
./nlm --config=config.cfg

# Combine command line with defaults
./nlm --brain.neuron_count=500 --connection_probability=0.05
```

## Troubleshooting

### Common Issues

**Issue**: "Failed to open config file"
**Solution**: Ensure the file exists and has proper permissions

**Issue**: "JSON parse error"
**Solution**: Check file syntax - use JSON validators or pretty-print the file

**Issue**: "Type conversion error"
**Solution**: Verify that the value matches the expected type

**Issue**: "Validation failed"
**Solution**: Check validation warnings and fix configuration issues

### Error Messages

The configuration system provides detailed error messages for:

- **File not found**: "Failed to open config file: filename"
- **JSON parse error**: "JSON parse error in filename: description"
- **Type conversion error**: "Failed to parse JSON key 'key': description"
- **Validation error**: Custom error messages from validators

## Migration Guide

### From Phase 1 to Phase 2

**Phase 1 (key=value format)**:

```cfg
# configs/default.cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
```

**Phase 2 (JSON format)**:

```json
{
  "random_seed": 42,
  "simulation": {
    "timestep": 0.001
  },
  "brain": {
    "neuron_count": 1000,
    "region_count": 2
  }
}
```

**Migration steps:**

1. Convert your configuration files to JSON format
2. Update your code to use the new configuration loading methods
3. Take advantage of the new nested structure for better organization
4. Add validation rules for your specific use case

## Performance Considerations

- **JSON parsing**: Efficient parsing with nlohmann/json
- **Memory usage**: Configuration is loaded once and stored in memory
- **Type safety**: Compile-time type checking for configuration values
- **Validation**: Custom validation allows for early error detection
- **Thread safety**: Safe for use in multi-threaded environments

## Future Enhancements

Potential future improvements:

- **Schema validation**: JSON Schema support for configuration validation
- **Configuration inheritance**: Support for configuration templates
- **Environment variables**: Support for environment variable overrides
- **Configuration hot-reloading**: Ability to reload configuration without restart
- **Configuration encryption**: Support for encrypted configuration files

## References

- [nlohmann/json documentation](https://github.com/nlohmann/json)
- [YAML-CPP documentation](https://github.com/jbeder/yaml-cpp)
- [Configuration Management Best Practices](https://en.wikipedia.org/wiki/Configuration_management)

---

*This document describes the enhanced JSON/YAML configuration system for NLM Phase 2.*
