# NLM Configuration System Documentation

## Overview

The NLM (Neural Learning Machine) Configuration System provides a flexible way to manage brain simulation parameters and runtime settings. It supports multiple configuration sources and formats, enabling users to customize behavior through files, command-line arguments, environment variables, and runtime modifications.

## Configuration Sources

### 1. Default Configuration
The system comes with a built-in default configuration that includes:
- `simulation_timestep`: 0.001 (time step for simulation, in seconds)
- `random_seed`: 42 (seed for random number generation)
- `neuron_count`: 500 (initial number of neurons)
- `region_count`: 1 (number of brain regions)
- `connection_probability`: 0.15 (probability of forming connections)
- `max_energy`: 100.0 (maximum energy per agent)
- `energy_decay_rate`: 0.01 (rate at which energy decays)
- `plasticity_rate`: 0.1 (rate at which connections change)

### 2. File Configuration
Configuration can be loaded from files using the `loadFromFile()` method.

**Supported Formats:**
- **JSON**: Modern, structured format with full type support
- **Simple**: Key=value format (legacy support)

**JSON Example:**
```json
{
    "simulation_timestep": 0.001,
    "random_seed": 12345,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.2,
    "max_energy": 200.0,
    "energy_decay_rate": 0.02,
    "plasticity_rate": 0.15,
    "enable_reward_modulation": true,
    "enable_curiosity": false,
    "development_stages": ["Initial", "CriticalPeriod", "Maturation", "Adult"],
    "synapse_weights": [0.5, 0.8, 0.3]
}
```

**Simple Format Example:**
```
# Simulation parameters
simulation_timestep = 0.001
random_seed = 42
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Learning parameters
enable_reward_modulation = true
enable_curiosity = false

# Development
plasticity_rate = 0.1
```

### 3. Command Line Configuration
Load configuration from command-line arguments using the `loadFromArgs()` method.

**Supported Formats:**
- `--key=value`: Set a key-value pair
- `-k value`: Short form of the above
- `--flag`: Boolean flag (sets to true)

**Example Usage:**
```bash
./nlm --simulation_timestep=0.005 --neuron_count=2000 --enable_reward_modulation --enable_curiosity
```

### 4. Environment Configuration
Load configuration from environment variables using the `createFromEnvironment()` static method.

**Supported Variables:**
- `NLM_SIMULATION_TIMESTEP`
- `NLM_RANDOM_SEED`
- `NLM_NEURON_COUNT`
- And others for all configuration keys

**Example:**
```bash
export NLM_SIMULATION_TIMESTEP=0.01
export NLM_NEURON_COUNT=1000
./nlm
```

## Configuration Value Types

The configuration system supports various value types:

### Primitive Types
- **Integer**: `int`, `int64_t`, `uint64_t`
- **Floating Point**: `double`, `float`
- **Boolean**: `bool`
- **String**: `std::string`

### Complex Types
- **Arrays**: Lists of primitive types (`std::vector<int>`, `std::vector<double>`, etc.)
- **Enum values**: Neuron types, synapse types, developmental stages

### Type Inference
When loading from text files, the system automatically infers types:
- `"true"` or `"false"` → boolean
- Numeric strings without decimal points → integer
- Numeric strings with decimal points → double
- Everything else → string

## Configuration API

### Core Methods

#### Creating Configurations
```python
import pynlm

# Create default config
config = pynlm.createDefaultConfig()

# Create from environment
config = pynlm.createFromEnvironment()

# Create from command line
config = pynlm.createFromCommandLine(len(sys.argv), sys.argv)
```

#### Loading Configuration
```python
# Load from file
config.loadFromFile("config.json")

# Load from command line
config.loadFromArgs(len(sys.argv), sys.argv)
```

#### Getting Configuration Values
```python
# Get with type conversion
simulation_timestep = config.get_double("simulation_timestep")
random_seed = config.get_int("random_seed")
enable_curiosity = config.get_bool("enable_curiosity")
string_value = config.get("string_key")

# Get with defaults
default_value = config.getOr("nonexistent_key", 100)  # Returns 100
```

#### Setting Configuration Values
```python
# Set with type conversion
config.set_double("simulation_timestep", 0.01)
config.set_int("random_seed", 12345)
config.set_bool("enable_reward_modulation", True)
config.set("custom_string", "value")
```

#### Querying Configuration
```python
# Check if key exists
if config.has("neuron_count"):
    neurons = config.get_int("neuron_count")

# Get all keys
keys = config.getKeys()

# Clear configuration
config.clear()
```

#### Configuration Management
```python
# Remove specific key
config.remove("temp_key")

# Clear all entries
config.clear()

# Get configuration summary
print(config.summary())

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Validation errors:", config.validationErrors())

# Get statistics
print(f"Total entries: {config.entryCount()}")
print(f"Runtime entries: {config.entryCount(ConfigSource.Runtime)}")
```

### Advanced Configuration

#### Source-Based Operations
```python
# Get keys from specific source
file_keys = config.getKeys(ConfigSource.File)
cmdline_keys = config.getKeys(ConfigSource.CommandLine)

# Get summary from specific source
print(config.summary(ConfigSource.File))

# Filter keys with prefix
brain_region_keys = config.getKeys("region_")
```

#### Configuration Manipulation
```python
# Merge configurations
other_config = pynlm.createDefaultConfig()
config.merge(other_config)

# Clone configuration
config_copy = config.clone(ConfigSource.Runtime)

# Remove all entries from a source
config.clear(ConfigSource.File)
```

### Error Handling

The configuration system provides robust error handling:

- **File Not Found**: Returns `false` for missing files
- **Parse Errors**: Falls back to simple format when JSON parsing fails
- **Type Mismatches**: `get()` methods return `None` for type mismatches
- **Validation Errors**: `validate()` method returns detailed error messages

### Best Practices

1. **Always validate configuration**: Use `validate()` before running simulations
2. **Use defaults appropriately**: Leverage `createDefaultConfig()` for sensible defaults
3. **Set environment variables**: Use environment variables for development and deployment
4. **Document configuration**: Add descriptions to configuration keys for better maintainability
5. **Test configuration loading**: Test configuration loading from all supported sources

## Integration with NLM Brain

The configuration system integrates seamlessly with the NLM brain:

```python
# Create brain with configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()
```

All configuration values are accessible through the brain object and can be modified at runtime.

## Migration Guide

### From Simple Format
If you're migrating from the simple key=value format, your existing configurations will continue to work with the enhanced parser.

### From JSON (Old Format)
If you have existing JSON configurations, they will be automatically parsed with enhanced type support.

### From YAML
The system supports JSON but not YAML. For YAML support, you'll need to convert to JSON first or use an external YAML parser.

## Performance Considerations

- **Memory Usage**: Configuration entries are stored in memory for fast access
- **File I/O**: Configuration files are cached in memory after loading
- **Type Safety**: Template-based type checking ensures type safety at compile time
- **Error Handling**: Graceful degradation when optional features (JSON parsing) are not available

## Future Enhancements

The configuration system is designed to be extensible:

1. **YAML Support**: Add native YAML parsing support
2. **Schema Validation**: Add JSON Schema validation for complex configurations
3. **Watchdog**: File watching for live configuration updates
4. **Distributed Configuration**: Support for distributed configuration management

## References

- [JSON Specification](https://www.json.org/)
- [Python Bindings API](https://github.com/nlm-project/nlm/tree/main/python)
- [NLM Architecture Documentation](https://github.com/nlm-project/nlm/tree/main/docs)
