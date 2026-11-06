# NLM Configuration Documentation

This document provides comprehensive documentation for the enhanced configuration system in NLM (Neural Learning Machine).

## Overview

The NLM configuration system has been significantly enhanced to support multiple file formats, advanced validation, and better integration with Python bindings. This documentation covers:

- Configuration file formats (JSON, YAML, Simple)
- Configuration loading and saving
- Configuration validation
- Command line configuration
- Python API documentation
- Configuration examples and best practices

## Configuration File Formats

### JSON Format (.json)

JSON is the recommended format for NLM configuration due to:

- **Structured data**: Supports nested objects and arrays
- **Human-readable**: Easy to read and edit
- **Type safety**: Strong typing for configuration values
- **Validation**: Easy to implement validation rules

#### Example JSON Configuration

```json
{
  "version": "3.0",
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
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001
    }
  },
  
  "neuromodulation": {
    "dopamine": {
      "baseline": 0.0,
      "scale": 1.0,
      "learning_rate": 0.1
    },
    "curiosity": {
      "enable": true,
      "sensitivity": 0.5,
      "decay_rate": 0.99
    },
    "novelty": {
      "enable": true,
      "threshold": 0.7,
      "detection_window": 100
    }
  },
  
  "development": {
    "enable": true,
    "initial_stage": "Initial",
    "plasticity_decay_rate": 0.995,
    "critical_period_start": 100,
    "critical_period_end": 500
  },
  
  "memory": {
    "working_memory": {
      "capacity": 100,
      "decay_rate": 0.9
    },
    "episodic_memory": {
      "max_episodes": 1000,
      "replay_probability": 0.3,
      "consolidation_threshold": 0.8
    }
  },
  
  "simulation": {
    "timestep": 0.001,
    "max_steps": 10000,
    "checkpoint_interval": 1000,
    "random_seed": 42
  },
  
  "environment": {
    "width": 20,
    "height": 20,
    "vision_width": 8,
    "vision_height": 8,
    "max_energy": 100.0,
    "energy_decay_rate": 0.1
  },
  
  "logging": {
    "level": "INFO",
    "log_to_file": true,
    "log_filename": "nlm.log",
    "console_colors": true
  },
  
  "advanced_features": {
    "prediction_system": {
      "enable": true,
      "prediction_horizon": 10,
      "confidence_threshold": 0.7
    },
    "cognition_system": {
      "enable": true,
      "planning_depth": 5,
      "attention_span": 50
    },
    "parallel_processing": {
      "enable": true,
      "thread_count": 4,
      "load_balance": true
    }
  }
}
```

### Simple Format (.cfg)

The simple format is maintained for backward compatibility:

#### Example Simple Configuration

```cfg
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.0
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### YAML Format (.yaml/.yml)

YAML is supported but currently falls back to simple format:

#### Example YAML Configuration

```yaml
# NLM Configuration File
# YAML format configuration

version: "3.0"
environment: "development"

brain:
  neuron_count: 2048
  region_count: 4
  connection_probability: 0.15
  v_thresh: -50.0
  v_rest: -70.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true
    learning_rate: 0.01

neuromodulation:
  curiosity:
    enable: true
    sensitivity: 0.5
```

## Configuration Loading

### File Loading

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Load from JSON file
config.loadFromFile("configs/enhanced_config.json")

# Load from simple format file
config.loadFromFile("configs/default.cfg")

# Load from specific path
config.loadFromFile("/path/to/custom_config.json")
```

### Command Line Loading

```python
# Command line arguments (passed to main function)
./nlm --neuron_count=512 --connection_probability=0.2 --enable_curiosity=true

# Short format
./nlm -neuron_count 256 -plasticity_learning_rate 0.05

# Mixed format
./nlm --neuron_count=1024 -c 0.3 --enable_stdp
```

## Configuration Values

### Primitive Types

Configuration values can be:

- **Integers** (`int64_t`): `42`, `-5`
- **Floats** (`double`): `3.14`, `-2.5`
- **Booleans** (`bool`): `true`, `false`
- **Strings** (`std::string`): `"example"`, `"path/to/file"`

### Complex Types

Configuration supports complex types:

- **String arrays**: `["item1", "item2", "item3"]`
- **Integer arrays**: `[1, 2, 3, 4, 5]`
- **Float arrays**: `[1.5, 2.7, 3.14]`

### Type Conversion

The configuration system automatically converts between compatible types:

- `int64_t` ↔ `double` ↔ `float`
- String representation of numbers is automatically converted

## Configuration Validation

### Validation Rules

Configuration validation ensures values are within expected ranges:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Define validation function
def validate_neuron_count(cfg):
    neurons = cfg.getOr("brain.neuron_count", 0)
    return neurons > 0 and neurons <= 100000

# Add validation rule
config.addValidationRule("neuron_count", validate_neuron_count, "Neuron count must be between 1 and 100,000")

# Validate configuration
is_valid = config.validateConfig()
if not is_valid:
    print("Configuration validation failed")
```

### Built-in Validation

The configuration system includes built-in validation for common settings:

- **Neuron count**: Must be positive and within reasonable limits
- **Connection probability**: Must be between 0.0 and 1.0
- **Learning rates**: Must be non-negative
- **Time constants**: Must be positive
- **Memory capacities**: Must be positive

## Configuration Saving

### Saving Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Save to JSON (recommended)
config.saveToFile("configs/enhanced_config.json")

# Save to simple format (backward compatibility)
config.saveToFile("configs/default.cfg")

# Save to YAML (experimental)
config.saveToFile("configs/config.yaml")
```

### Configuration Summary

The configuration system provides a summary method for debugging:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

print(config.summary())
# Output:
# Configuration (2 entries):
#   brain.neuron_count = [1000] (1)
#   plasticity.stdp.enable = [true] (4)
```

## Python API Documentation

### Configuration Class

```python
class Config:
    def __init__(self):
        """Create a default configuration"""
        pass
    
    def loadFromFile(self, filepath: str) -> bool:
        """Load configuration from file (JSON, YAML, or simple format)"""
        pass
    
    def loadFromArgs(self, args: List[str]) -> bool:
        """Load configuration from command line arguments"""
        pass
    
    def saveToFile(self, filepath: str) -> bool:
        """Save configuration to file (JSON, YAML, or simple format)"""
        pass
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists"""
        pass
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys"""
        pass
    
    def get(self, key: str) -> Optional[Any]:
        """Get configuration value (returns None if key doesn't exist)"""
        pass
    
    def getOr(self, key: str, default: Any) -> Any:
        """Get configuration value with default fallback"""
        pass
    
    def set(self, key: str, value: Any, source: str = "Runtime") -> None:
        """Set configuration value"""
        pass
    
    def remove(self, key: str) -> None:
        """Remove configuration key"""
        pass
    
    def clear(self) -> None:
        """Clear all configuration entries"""
        pass
    
    def summary(self) -> str:
        """Get configuration summary as string"""
        pass
    
    def addValidationRule(self, name: str, validator: callable, description: str) -> None:
        """Add configuration validation rule"""
        pass
    
    def validateConfig(self) -> bool:
        """Validate configuration against rules"""
        pass
```

### Helper Functions

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Create simple world
world = pynlm.createSimpleWorld()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
```

## Configuration Examples

### Example 1: Basic Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set basic brain parameters
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.1)

# Enable basic plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

# Initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 2: Advanced Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Advanced brain configuration
config.set("brain.neuron_count", 2048)
config.set("brain.region_count", 4)
config.set("brain.connection_probability", 0.15)

# Enhanced plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.01)
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0001)

# Neuromodulation
config.set("neuromodulation.curiosity.enable", True)
config.set("neuromodulation.novelty.enable", True)
config.set("neuromodulation.dopamine.scale", 1.0)

# Development
config.set("development.enable", True)
config.set("development.initial_stage", "CriticalPeriod")

# Memory systems
config.set("memory.working_memory.capacity", 100)
config.set("memory.episodic_memory.max_episodes", 1000)

# Environment
config.set("environment.width", 20)
config.set("environment.height", 20)
config.set("environment.vision_width", 8)
config.set("environment.vision_height", 8)

# Simulation
config.set("simulation.timestep", 0.001)
config.set("simulation.max_steps", 10000)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 3: Configuration from File

```python
import pynlm

# Load from JSON file
config = pynlm.createDefaultConfig()
if config.loadFromFile("configs/enhanced_config.json"):
    print("Configuration loaded successfully")
else:
    print("Failed to load configuration")

# Load from simple format
config.loadFromFile("configs/default.cfg")
```

### Example 4: Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()

# Add validation rules
config.addValidationRule("neuron_count", validate_neuron_count, "Invalid neuron count")
config.addValidationRule("connection_probability", validate_probability, "Invalid connection probability")
config.addValidationRule("learning_rate", validate_learning_rate, "Invalid learning rate")

# Validate configuration
if not config.validateConfig():
    print("Configuration validation failed")
else:
    print("Configuration is valid")
```

## Configuration File Formats

### Supported File Extensions

| Extension | Format | Description |
|-----------|--------|-------------|
| `.json` | JSON | Human-readable, structured format |
| `.cfg` | Simple | Simple key=value format |
| `.yaml`, `.yml` | YAML | Human-readable, YAML format |

### File Format Detection

The configuration system automatically detects file format based on extension:

- `.json`: Parsed as JSON using nlohmann/json library
- `.cfg`: Parsed as simple key=value format
- `.yaml`, `.yml`: Parsed as YAML (if library available, otherwise falls back to simple format)

## Configuration Loading Priority

1. **File format detection**: Based on file extension
2. **JSON parsing**: Most feature-rich format
3. **YAML parsing**: If available, otherwise falls back
4. **Simple format**: Backward compatibility

## Configuration Error Handling

### Error Types

1. **File not found**: Configuration file doesn't exist
2. **Permission denied**: Cannot read/write configuration file
3. **Format error**: Invalid JSON/YAML syntax
4. **Validation error**: Configuration values don't meet requirements
5. **Parse error**: Cannot parse configuration value

### Error Recovery

The configuration system includes error recovery mechanisms:

- Graceful fallback to simple format
- Default values for missing configuration
- Partial loading when possible
- Detailed error messages for debugging

## Configuration Migration

### From Simple to JSON

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

### Migration Examples

| Source Format | Target Format | Conversion Method |
|---------------|---------------|-------------------|
| `key=value`   | `{"key": "value"}` | Simple to JSON conversion |
| `key = value` | JSON with nested objects | Complex conversion |
| Multiple files | Single JSON | Merge configuration |

## Configuration Best Practices

### 1. Use JSON for Complex Configurations

- JSON supports nested structures
- Easier to read and edit
- Better for configuration management

### 2. Validate Your Configuration

```python
# Always validate configuration
if not config.validateConfig():
    print("Configuration validation failed")
    # Fix configuration issues
```

### 3. Use Environment-Specific Configurations

```python
# Development configuration
if environment == "development":
    config.set("logging.level", "DEBUG")
    config.set("brain.neuron_count", 100)

# Production configuration
elif environment == "production":
    config.set("logging.level", "INFO")
    config.set("brain.neuron_count", 2048)
```

### 4. Test Your Configuration

```python
# Test configuration loading
config = pynlm.createDefaultConfig()
if config.loadFromFile("config.json"):
    print("Configuration loaded successfully")
    # Run tests with loaded configuration
```

### 5. Use Descriptive Configuration Keys

```python
# Good practice
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Avoid generic keys
config.set("key1", 1)  # Not recommended
```

### 6. Document Your Configuration

```python
# Add comments to configuration
config.set("brain.neuron_count", 1000, source="Documentation: 1000 neurons provides good balance")
config.set("plasticity.stdp.learning_rate", 0.001, source="Documentation: STDP learning rate")
```

### 7. Use Configuration Templates

```python
# Configuration template
DEFAULT_CONFIG = {
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {"enable": True, "learning_rate": 0.001},
        "hebbian": {"enable": True, "learning_rate": 0.01}
    }
}
```

## Configuration Performance

### Configuration Loading Performance

- **JSON loading**: O(n) where n is number of configuration entries
- **Simple format loading**: O(n * m) where m is average line length
- **Validation**: O(k * n) where k is number of validation rules

### Memory Usage

- **Simple format**: Minimal overhead
- **JSON format**: Higher memory usage but more structured
- **Configuration objects**: Constant overhead per configuration

### Optimization Tips

```python
# Load configuration once
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# Reuse configuration
brain1 = pynlm.createBrain(config)
brain2 = pynlm.createBrain(config)  # Share configuration

# Clear configuration if not needed
config.clear()
```

## Configuration Security

### Secure Configuration Practices

1. **Avoid sensitive data in configuration files**
2. **Use environment variables for secrets**
3. **Restrict file permissions**
4. **Validate configuration values**
5. **Log configuration access**

### Example Secure Configuration

```python
import pynlm
import os

config = pynlm.createDefaultConfig()

# Use environment variables for secrets
config.set("database.password", os.environ.get("DB_PASSWORD", ""))
config.set("api.key", os.environ.get("API_KEY", ""))

# Don't store passwords in configuration files
# Use environment variables or secure key management systems
```

## Configuration Troubleshooting

### Common Issues

1. **File not found error**
2. **Invalid format error**
3. **Permission error**
4. **Validation error**
5. **Performance issues**

### Troubleshooting Steps

1. **Check file paths**: Ensure configuration files are in the correct location
2. **Validate format**: Check JSON/YAML syntax
3. **Check permissions**: Ensure read/write permissions
4. **Validate configuration**: Use built-in validation
5. **Profile performance**: Identify bottlenecks

### Example Troubleshooting Script

```python
import pynlm

def troubleshoot_configuration(filepath):
    config = pynlm.createDefaultConfig()
    
    # Check if file exists
    import os
    if not os.path.exists(filepath):
        print(f"Configuration file not found: {filepath}")
        return False
    
    # Check if file is readable
    if not os.access(filepath, os.R_OK):
        print(f"Configuration file not readable: {filepath}")
        return False
    
    # Try to load configuration
    if not config.loadFromFile(filepath):
        print(f"Failed to load configuration from: {filepath}")
        return False
    
    # Validate configuration
    if not config.validateConfig():
        print("Configuration validation failed")
        return False
    
    print("Configuration troubleshooting completed successfully")
    return True
```

## Configuration Documentation

### Configuration Reference

For detailed configuration reference, see:

- `docs/CONFIGURATION.md` - Complete configuration reference
- `docs/CONFIG_SAMPLES.md` - Configuration samples and examples
- `docs/CONFIG_VALIDATION.md` - Configuration validation rules

### Configuration Examples

For configuration examples, see:

- `examples/configuration.json` - JSON configuration examples
- `examples/configuration.cfg` - Simple configuration examples
- `examples/configuration.yaml` - YAML configuration examples

### Configuration Support

For configuration-related support, visit:

- GitHub: [Configuration Documentation](https://github.com/nlm-project/nlm/tree/main/docs)
- Issues: [Configuration Issues](https://github.com/nlm-project/nlm/issues)
- Forum: [NLM Community Forum](https://community.nlm.org)

## Conclusion

The NLM configuration system provides:

1. **Multiple file formats**: JSON, YAML, and simple format
2. **Advanced validation**: Configuration validation and error handling
3. **Flexible loading**: File, command line, and programmatic loading
4. **Comprehensive documentation**: Detailed documentation and examples
5. **Performance optimization**: Efficient configuration loading and saving

The configuration system is designed to be:

- **Flexible**: Support for multiple formats and loading methods
- **Robust**: Comprehensive error handling and validation
- **Documented**: Detailed documentation and examples
- **Performant**: Efficient configuration management

For more information, refer to the configuration documentation and examples provided with NLM.
