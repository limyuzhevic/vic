# NLM Configuration System Documentation

## Overview

The NLM Configuration System provides a comprehensive framework for managing NLM (Neural Learning Machine) parameters. It supports multiple configuration sources, validation, type safety, and serialization formats.

## Configuration Sources

### 1. Default Configuration
- Built-in default values for all NLM components
- Used when no other configuration is provided
- Stored in source code as `ConfigSource::Default`

### 2. File-Based Configuration
- Supports JSON, YAML, and plain text formats
- File format auto-detection based on content
- Hierarchical key-value pairs

### 3. Command-Line Configuration
- Overrides configuration via command-line arguments
- Format: `--key=value` or `--key value`
- Stored as `ConfigSource::CommandLine`

### 4. Runtime Configuration
- Set programmatically during execution
- Overrides all other sources
- Stored as `ConfigSource::Runtime`

## Configuration Format

### JSON Configuration
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    }
}
```

### YAML Configuration
```yaml
brain:
  neuron_count: 1000
  synapse_density: 0.1
  connection_probability: 0.05
  v_thresh: -50.0
  v_rest: -70.0
  tau_mem: 20.0
  tau_ref: 2.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true

neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
  novelty:
    enable: true
```

### Plain Text Configuration
```
# Brain configuration
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05
brain.v_thresh = -50.0
brain.v_rest = -70.0

# Plasticity configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true

# Neuromodulation configuration
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
```

## Configuration Keys

### Brain Configuration
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for potentiation (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for depression (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Configuration Operations

### Loading Configuration
```cpp
// Load from file
config.loadFromFile("config.json");
config.loadFromFile("config.yaml");
config.loadFromFile("config.txt");

// Load from JSON string
config.loadFromJSON("{\"brain.neuron_count\": 1000}");

// Load from YAML string
config.loadFromYAML("brain:\n  neuron_count: 1000");

// Load from command line arguments
config.loadFromArgs(argc, argv);
```

### Getting and Setting Values
```cpp
// Get values with type safety
auto neuronCount = config.get<int>("brain.neuron_count");
auto synapseDensity = config.get<double>("brain.synapse_density");

// Get values with defaults
auto neurons = config.getOr("brain.neuron_count", 1000);
auto density = config.getOr("brain.synapse_density", 0.1);

// Set values
config.set("brain.neuron_count", 2000);
config.set("brain.synapse_density", 0.2f);
config.set("plasticity.stdp.enable", true);
```

### Configuration Validation
```cpp
// Define validation schema
std::vector<ConfigSchema> schema = {
    ConfigSchema("brain.neuron_count", "int", true, "Total neuron count"),
    ConfigSchema("brain.synapse_density", "float", false, "Connection density", {
        {"min", "0.0"}, {"max", "1.0"}
    }),
    ConfigSchema("plasticity.stdp.enable", "bool", false, "STDP enabled")
};

// Validate configuration
bool isValid = config.validateSchema(schema);
```

### Configuration Statistics
```cpp
// Get configuration statistics
auto stats = config.getStats();
std::cout << "Total entries: " << stats.totalEntries << std::endl;
std::cout << "Default entries: " << stats.sourceCounts[ConfigSource::Default] << std::endl;
```

### Configuration Diffing
```cpp
// Compare two configurations
Config config1, config2;
auto diff = config1.diff(config2);
for (const auto& entry : diff) {
    std::cout << entry.key << ": " << entry.oldValue << " -> " << entry.newValue << std::endl;
}
```

## Configuration Examples

### Example 1: High-Density Network
```json
{
    "brain": {
        "neuron_count": 10000,
        "synapse_density": 0.2,
        "connection_probability": 0.1,
        "plasticity": {
            "stdp": {
                "enable": true,
                "learning_rate": 0.002
            }
        }
    }
}
```

### Example 2: Sparse Network with Learning
```yaml
brain:
  neuron_count: 500
  synapse_density: 0.05
  plasticity:
    stdp:
      enable: true
      learning_rate: 0.001
    hebbian:
      enable: true
    structural:
      enable: true
neuromodulation:
  curiosity:
    enable: true
  novelty:
    enable: true
```

### Example 3: Command Line Configuration
```bash
./nlm --config config.json --brain.neuron_count=2000 \
     --plasticity.stdp.enable=true --neuromod.curiosity.enable=false
```

## File Format Support

### Supported Formats
- **JSON**: Modern, widely supported, human-readable
- **YAML**: Human-friendly, supports comments and aliases
- **Plain Text**: Simple key-value pairs, easy to edit

### File Detection
The configuration system automatically detects file format:
- JSON files: `.json` extension or JSON content
- YAML files: `.yaml` or `.yml` extension or YAML content
- Plain text files: `.txt` extension or plain text content

## Error Handling

### Common Errors
1. **File not found**: Configuration file does not exist
2. **Parse errors**: Invalid format in configuration file
3. **Validation errors**: Configuration values fail schema validation
4. **Type errors**: Attempt to get value with wrong type

### Error Recovery
```cpp
try {
    config.loadFromFile("config.json");
} catch (const ConfigError& e) {
    std::cerr << "Configuration error: " << e.what() << stdendl;
    // Fall back to defaults or handle gracefully
}
```

## Performance Considerations

### Memory Usage
- Configuration entries stored in memory for fast access
- Support for millions of configuration keys
- Memory-efficient variant-based value storage

### Access Patterns
- Frequently accessed keys should use common keys
- Batch operations for multiple key access
- Consider caching frequently used configurations

## Integration with Other Components

### Command Line Interface
Configuration values can be set via:
```bash
./nlm --brain.neuron_count=5000 --plasticity.stdp.enable=true
```

### Testing Framework
Configuration can be loaded for tests:
```cpp
// Load test-specific configuration
config.loadFromFile("tests/test_config.json");

// Validate test configuration
bool valid = config.validateSchema(testSchema);
```

### Performance Benchmarks
Configuration affects benchmark parameters:
```cpp
// Configure benchmark parameters
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", false);

// Run performance benchmark
runBenchmark(config);
```

## Migration Guide

### From v0.x to v1.0
- New hierarchical key structure
- Added YAML support
- Improved error handling and validation
- Enhanced configuration statistics

## Troubleshooting

### Common Issues
1. **Configuration not applied**: Check source priority (Runtime > CommandLine > File > Default)
2. **Type mismatch**: Ensure correct type when accessing values
3. **File format issues**: Verify file extension and content
4. **Validation failures**: Check schema requirements and constraints

### Help Commands
```bash
# Show configuration help
./nlm --help

# Validate configuration
./nlm --config config.json --validate

# Show configuration summary
./nlm --config config.json --verbose
```

## Future Enhancements

1. **Configuration Encryption**: Secure storage of sensitive configuration values
2. **Distributed Configuration**: Support for remote configuration management
3. **Dynamic Configuration**: Runtime configuration updates
4. **Configuration Templates**: Reusable configuration templates for common use cases
