# Configuration Examples for Enhanced NLM

This guide shows how to use the new enhanced configuration system in NLM.

## Simple Configuration (Simple Format)

For backward compatibility, you can still use the simple key=value format:

```bash
# config.cfg
c neuron_count = 1000
connection_probability = 0.1
plasticity.stdp.enable = true
neuromodulation.curiosity.enable = false
learning_rate = 0.01
```

To load this configuration:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("config.cfg")
brain = pynlm.createBrain(config)
```

## Advanced Configuration (JSON Format)

The new enhanced configuration system uses JSON for better structured configuration:

```json
{
  "brain": {
    "neuron_count": 2048,
    "region_count": 4,
    "connection_probability": 0.15,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "v_reset": -75.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0
  },
  
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0,
      "weight_range": [-0.1, 0.1]
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.01
    }
  },
  
  "neuromodulation": {
    "dopamine": {
      "baseline": 0.0,
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "sensitivity": 0.5
    }
  }
}
```

To load this configuration:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("enhanced_config.json")
brain = pynlm.createBrain(config)
```

## Command Line Configuration

You can also configure NLM from command line arguments:

```bash
./nlm --neuron_count=512 --connection_probability=0.2 --enable_curiosity=true
```

Or using short format:

```bash
./nlm -neuron_count 256 -plasticity_learning_rate 0.05
```

## Configuration Validation

The configuration system includes validation support:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromodulation.curiosity.enable", False)

# Check if a key exists
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.getOr('brain.neuron_count', 0)}")

# Get all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Get configuration summary
print(config.summary())
```

## Performance Configuration

You can configure performance-related settings:

```json
{
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000,
    "checkpoint_interval": %> 1000,
    "random_seed": 42
  },
  
  "advanced_features": {
    "parallel_processing": {
      "enable": true,
      "thread_count": 4,
      "load_balance": true
    }
  }
}
```

## Environment Configuration

Configuration for the simulated world:

```json
{
  "environment": {
    "width": 20,
    "height": 20,
    "vision_width": 8,
    "vision_height": 8,
    "max_energy": 100.0,
    "energy_decay_rate": 0.1
  }
}
```

## Example: Complete Enhanced Configuration

Here's an example of a complete enhanced configuration with all features:

```json
{
  "brain": {
    "neuron_count": 2048,
    "region_count": 4,
    "connection_probability": 0.15
  },
  
  "plasticity": {
    "stdp": { "enable": true, "learning_rate": 0.001 },
    "hebbian": { "enable": true, "learning_rate": 0.01 },
    "structural": { "enable": true, "synaptogenesis_rate": 0.0001 }
  },
  
  "neuromodulation": {
    "curiosity": { "enable": true, "sensitivity": 0.5 },
    "novelty": { "enable": true, "threshold": 0.7 }
  },
  === Complete enhanced_config.json > /dev/null

```

## Configuration File Locations

Configuration files can be located in:

- `configs/enhanced_config.json` - Enhanced JSON configuration
- `configs/default.cfg` - Default simple format configuration
- Any user-specified path via command line: `--config=/path/to/config.json`

## Configuration Migration

If you have existing simple format configuration files, you can convert them to JSON:

```python
import pynlm
import json

# Load old config
old_config = pynlm.createDefaultConfig()
old_config.loadFromFile("old_config.cfg")

# Convert to JSON structure
json_config = {}
for key in old_config.getKeys():
    value = old_config.get(key)
    # Convert key format
    json_key = key.replace('.', '_')
    json_config[json_key] = value

# Save as JSON
with open("new_config.json", "w") as f:
    json.dump(json_config, f, indent=2)
```

## Configuration Best Practices

1. **Use JSON for complex configurations**: JSON supports nested structures and is easier to read and edit
2. **Validate your configuration**: Ensure values are within expected ranges
3. **Use environment-specific configs**: Separate development, testing, and production configurations
4. **Enable logging**: Set log_level appropriately for debugging
5. **Test your configuration**: Run simulations to verify configuration takes effect

## Configuration Examples

### Example 1: Simple Brain

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 2: Advanced Brain with Neuromodulation

```python
import pynlm

config = pynlm.createDefaultConfig()

# Brain settings
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.2)

# Plasticity settings
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation settings
config.set("neuromodulation.curiosity.enable", True)
config.set("neuromodulation.novelty.enable", True)
config.set("neuromodulation.dopamine.scale", 1.5)

# Development settings
config.set("development.enable", True)
config.set("development.initial_stage", "CriticalPeriod")

brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 3: World Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Environment settings
config.set("environment.width", 30)
config.set("environment.height", 30)
config.set("environment.vision_width", 10)
config.set("environment.vision_height", 10)
config.set("environment.max_energy", 200.0)

# Simulation settings
config.set("simulation.timestep", 0.002)
config.set("simulation.max_steps", 5000)
config.set("simulation.checkpoint_interval", 500)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 4: Performance-Optimized Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# High-performance settings
config.set("brain.neuron_count", 4000)
config.set("brain.connection_probability", 0.12)
config.set("advanced_features.parallel_processing.enable", True)
config.set("advanced_features.parallel_processing.thread_count", 8)
config.set("advanced_features.parallel_processing.load_balance", True)

# Enable all learning mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Advanced memory systems
config.set("memory.working_memory.capacity", 500)
config.set("memory.episodic_memory.max_episodes", 5000)

brain = pynlm.createBrain(config)
brain.initialize()
```

## Configuration Quick Reference

| Configuration Type | File Extension | Example |
|------------------|----------------|---------|
| Simple Format    | `.cfg`         | `key = value` |
| JSON Format      | `.json`        | `{"key": "value"}` |
| YAML Format      | `.yaml/.yml`   | `key: value` |

| Configuration Method | Description |
|---------------------|-------------|
| `loadFromFile()`    | Load from file (JSON, YAML, or simple format) |
| `loadFromArgs()`    | Load from command line arguments |
| `set()`             | Set configuration value |
| `get()`             | Get configuration value |
| `getOr()`           | Get configuration value with default |
| `has()`             | Check if key exists |
| `saveToFile()`      | Save configuration to file |
| `getKeys()`         | Get all configuration keys |
| `summary()`         | Get configuration summary |

## Configuration Troubleshooting

### Common Issues

1. **File not found error**: Ensure the configuration file exists at the specified path
2. **Invalid format error**: Check that JSON/YAML files are properly formatted
3. **Parsing error**: Ensure value formats match the expected types

### Solutions

1. **Test file paths**: Use absolute paths or paths relative to the working directory
2. **Validate configuration**: Use a configuration validator before loading
3. **Check permissions**: Ensure the application has read/write permissions for configuration files

### Debugging

To debug configuration loading:

```python
import pynlm

# Enable logging
import logging
logging.basicConfig(level=logging.DEBUG)

config = pynlm.createDefaultConfig()
if config.loadFromFile("config.json"):
    print("Configuration loaded successfully")
else:
    print("Configuration loading failed")
```

## Configuration Versioning

The configuration system supports versioning:

- **v1.0**: Simple format only (key=value)
- **v2.0**: JSON/YAML format with nested structures
- **v3.0**: Enhanced JSON with validation and advanced features

To specify a configuration version:

```json
{
  "version": "3.0",
  "brain": { ... },
  "plasticity": { ... }
}
```

## Configuration Extensions

The configuration system supports extensions through plugins:

```python
import pynlm

# Load configuration with plugin
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json", plugin="my_plugin")

# Get extended configuration values
extended_values = config.getExtendedConfig()
```

## Configuration Migration Guide

### From v1.x to v2.x

1. Convert `key=value` files to JSON
2. Use nested objects for related settings
3. Add validation rules

### From v2.x to v3.x

1. Add versioning field
2. Use advanced features
3. Implement validation rules

## Configuration Samples

### Sample 1: Development Configuration

```json
{
  "version": "3.0",
  "environment": "development",
  "brain": {
    "neuron_count": 100,
    "connection_probability": 0.1
  },
  "logging": {
    "level": "DEBUG",
    "log_to_file": true
  }
}
```

### Sample 2: Production Configuration

```json
{
  "version": "3. sledování",
  "environment": "production",
  "brain": {
    "neuron_count": 2048,
    "connection_probability": 0.15
  },
  "logging": {
    "level": "INFO",
    "log_to_file": false
  },
  "performance": {
    "enable_parallel_processing": true,
    "thread_count": 4
  }
}
```

### Sample 3: Research Configuration

```json
{
  "version": "3.0",
  "environment": "research",
  "brain": {
    "neuron_count": 4096,
    "connection_probability": 0.18
  },
  "plasticity": {
    "stdp": { "enable": true, "learning_rate": 0.002 },
    "hebbian": { "enable": true, "learning_rate": 0.01 },
    "structural": { "enable": true, "synaptogenesis_rate": 0.0002 }
  },
  "neuromodulation": {
    "curiosity": { "enable": true, "sensitivity": 0.7 },
    "novelty": { "enable": true, "threshold": 0.8 }
  }
}
```

## Configuration Documentation

For detailed documentation on configuration options, see:

- `docs/CONFIGURATION.md` - Complete configuration reference
- `docs/CONFIG_SAMPLES.md` - Configuration samples and examples
- `docs/CONFIG_VALIDATION.md` - Configuration validation rules

## Configuration Support

For configuration-related support, visit:

- GitHub: [Configuration Documentation](https://github.com/nlm-project/nlm/tree/main/docs)
- Issues: [Configuration Issues](https://github.com/nlm-project/nlm/issues)
- Forum: [NLM Community Forum](https://community.nlm.org)
