# NLM Python API Reference - Config Class

## Overview

The Config class provides a flexible configuration system for NLM (Neural Learning Machine). It allows users to set, retrieve, and manage configuration parameters for neural simulation, plasticity mechanisms, neuromodulation, development, and world interaction.

## Key Features

- **Key-value storage**: Simple string-to-value mapping
- **Nested sections**: Organize parameters by category (brain, plasticity, neuromod, etc.)
- **File I/O**: Save/load configurations to/from JSON files
- **Type conversion**: Automatic conversion between Python and C++ types
- **Validation**: Optional validation of configuration values
- **Inheritance**: Support for configuration inheritance and merging
- **Dynamic updates**: Runtime configuration modification

## Class Definition

```python
config = pynlm.createDefaultConfig()
```

## Core Methods

### Configuration Creation

#### `createDefaultConfig()`
```python
config = pynlm.createDefaultConfig()
```

**Description**: Create a default configuration with all standard NLM parameters.

**Parameters**:
- None

**Returns**:
- `config` (Config): Default configuration object

**Example**:
```python
import pynlm

config = pynlm.createDefaultConfig()
print(f"Neuron count: {config.get('brain.neuron_count')}")
```

### Basic Key-Value Operations

#### `config.has(key)`
```python
has_key = config.has(key)
```

**Description**: Check if a configuration key exists.

**Parameters**:
- `key` (str): Configuration key (dot-separated for nested sections)

**Returns**:
- `has_key` (bool): True if key exists, False otherwise

**Example**:
```python
config = pynlm.createDefaultConfig()
print(config.has("brain.neuron_count"))  # True
print(config.has("nonexistent.key"))    # False
```

#### `config.get(key, default=None)`
```python
value = config.get(key, default=None)
```

**Description**: Get configuration value by key. Returns default if key doesn't exist.

**Parameters**:
- `key` (str): Configuration key
- `default` (any): Default value if key doesn't exist (default: None)

**Returns**:
- `value` (any): Configuration value (converted to appropriate Python type)

**Example**:
```python
config = pynlm.createDefaultConfig()

# Get integer value
neuron_count = config.get("brain.neuron_count")
print(neuron_count)  # 1000

# Get float value with default
learning_rate = config.get("plasticity.stdp.learning_rate", 0.001)
print(learning_rate)  # 0.001

# Get string value
region = config.get("brain.region_name", "default")
print(region)  # "default"
```

#### `config.set(key, value)`
```python
config.set(key, value)
```

**Description**: Set configuration value. Creates key if it doesn't exist.

**Parameters**:
- `key` (str): Configuration key
- `value` (any): Value to set (converted to appropriate C++ type)

**Returns**:
- None

**Example**:
```python
config = pynlm.createDefaultConfig()

# Set integer
config.set("brain.neuron_count", 2000)

# Set float
config.set("plasticity.stdp.learning_rate", 0.002)

# Set string
config.set("brain.region_name", "cortex")

# Set boolean
config.set("plasticity.stdp.enable", True)
```

#### `config.getKeys()`
```python
keys = config.getKeys()
```

**Description**: Get all configuration keys.

**Parameters**:
- None

**Returns**:
- `keys` (list[str]): List of all configuration keys

**Example**:
```python
config = pynlm.createDefaultConfig()
keys = config.getKeys()
print(f"Total keys: {len(keys)}")
for key in keys[:5]:  # Show first 5 keys
    print(f"  {key}")
```

### Section Operations

#### `config.getSection(section_name)`
```python
section = config.getSection(section_name)
```

**Description**: Get configuration subsection (if supported).

**Parameters**:
- `section_name` (str): Name of section (e.g., "brain", "plasticity")

**Returns**:
- `section` (Config): Subsection configuration object

**Example**:
```python
config = pynlm.createDefaultConfig()
brain_section = config.getSection("brain")
neuron_count = brain_section.get("neuron_count")
print(neuron_count)  # 1000
```

#### `config.setSection(section_name, section)`
```python
config.setSection(section_name, section)
```

**Description**: Set configuration subsection.

**Parameters**:
- `section_name` (str): Name of section
- `section` (Config): Section configuration object

**Returns**:
- None

**Example**:
```python
import pynlm

# Create section
brain_section = pynlm.createDefaultConfig()
brain_section.set("neuron_count", 2000)
brain_section.set("v_thresh", -45.0)

# Set as subsection
config.setSection("brain", brain_section)
```

### File Operations

#### `config.loadFromFile(filepath)`
```python
config.loadFromFile(filepath)
```

**Description**: Load configuration from JSON file.

**Parameters**:
- `filepath` (str): Path to JSON configuration file

**Returns**:
- `success` (bool): True if load successful, False otherwise

**Example**:
```python
config = pynlm.createDefaultConfig()

# Load from file
success = config.loadFromFile("configs/my_config.json")
if success:
    print("Configuration loaded successfully")
else:
    print("Failed to load configuration")
```

#### `config.saveToFile(filepath)`
```python
config.saveToFile(filepath)
```

**Description**: Save configuration to JSON file.

**Parameters**:
- `filepath` (str): Path to save JSON configuration file

**Returns**:
- `success` (bool): True if save successful, False otherwise

**Example**:
```python
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.learning_rate", 0.003)

# Save to file
success = config.saveToFile("configs/my_config.json")
if success:
    print("Configuration saved successfully")
```

#### `config.loadFromArgs(argc, argv)`
```python
config.loadFromArgs(argc, argv)
```

**Description**: Load configuration from command line arguments (similar to typical CLI parsers).

**Parameters**:
- `argc` (int): Number of command line arguments
- `argv` (list[str]): Command line argument list

**Returns**:
- `success` (bool): True if parsing successful, False otherwise

**Example**:
```python
import sys

config = pynlm.createDefaultConfig()
sys.argv = ["program", "--brain.neuron_count", "5000", "--plasticity.stdp.enable", "false"]
success = config.loadFromArgs(len(sys.argv), sys.argv)
```

### Configuration Management

#### `config.clear()`
```python
config.clear()
```

**Description**: Clear all configuration entries.

**Parameters**:
- None

**Returns**:
- None

**Example**:
```python
config = pynlm.createDefaultConfig()
print(f"Keys before clear: {len(config.getKeys())}")

config.clear()
print(f"Keys after clear: {len(config.getKeys())}")  # 0
```

#### `config.merge(other_config)`
```python
config.merge(other_config)
```

**Description**: Merge another configuration into this one. Values in other_config override existing values.

**Parameters**:
- `other_config` (Config): Configuration to merge

**Returns**:
- None

**Example**:
```python
import pynlm

# Base configuration
base_config = pynlm.createDefaultConfig()
base_config.set("brain.neuron_count", 1000)

# Override configuration
override_config = pynlm.createDefaultConfig()
override_config.set("brain.neuron_count", 2000)

# Merge (override takes precedence)
base_config.merge(override_config)
print(base_config.get("brain.neuron_count"))  # 2000
```

#### `config.copy()`
```python
config_copy = config.copy()
```

**Description**: Create a copy of this configuration.

**Parameters**:
- None

**Returns**:
- `config_copy` (Config): Copy of configuration

**Example**:
```python
import pynlm

original = pynlm.createDefaultConfig()
original.set("brain.neuron_count", 1000)

copy = original.copy()
print(copy.get("brain.neuron_count"))  # 1000

# Modify copy without affecting original
copy.set("brain.neuron_count", 2000)
print(original.get("brain.neuron_count"))  # 1000 (unchanged)
```

### Specialized Setters

#### `config.set_int(key, value)`
```python
config.set_int(key, value)
```

**Description**: Set integer configuration value with type validation.

**Parameters**:
- `key` (str): Configuration key
- `value` (int): Integer value

**Returns**:
- None

**Example**:
```python
config.set_int("brain.neuron_count", 1500)
```

#### `config.set_float(key, value)`
```python
config.set_float(key, value)
```

**Description**: Set float configuration value with type validation.

**Parameters**:
- `key` (str): Configuration key
- `value` (float): Float value

**Returns**:
- None

**Example**:
```python
config.set_float("plasticity.stdp.learning_rate", 0.0015)
```

#### `config.set_string(key, value)`
```python
config.set_string(key, value)
```

**Description**: Set string configuration value with type validation.

**Parameters**:
- `key` (str): Configuration key
- `value` (str): String value

**Returns**:
- None

**Example**:
```python
config.set_string("brain.region_name", "cortex")
```

#### `config.set_bool(key, value)`
```python
config.set_bool(key, value)
```

**Description**: Set boolean configuration value with type validation.

**Parameters**:
- `key` (str): Configuration key
- `value` (bool): Boolean value

**Returns**:
- None

**Example**:
```python
config.set_bool("plasticity.stdp.enable", True)
```

### Value Queries

#### `config.get_int(key, default=0)`
```python
value = config.get_int(key, default=0)
```

**Description**: Get integer configuration value.

**Parameters**:
- `key` (str): Configuration key
- `default` (int): Default value if key doesn't exist or wrong type

**Returns**:
- `value` (int): Integer value

#### `config.get_float(key, default=0.0)`
```python
value = config.get_float(key, default=0.0)
```

**Description**: Get float configuration value.

**Parameters**:
- `key` (str): Configuration key
- `default` (float): Default value if key doesn't exist or wrong type

**Returns**:
- `value` (float): Float value

#### `config.get_string(key, default="")`
```python
value = config.get_string(key, default="")
```

**Description**: Get string configuration value.

**Parameters**:
- `key` (str): Configuration key
- `default` (str): Default value if key doesn't exist or wrong type

**Returns**:
- `value` (str): String value

#### `config.get_bool(key, default=False)`
```python
value = config.get_bool(key, default=False)
```

**Description**: Get boolean configuration value.

**Parameters**:
- `key` (str): Configuration key
- `default` (bool): Default value if key doesn't exist or wrong type

**Returns**:
- `value` (bool): Boolean value

#### `config.getSectionKeys(section_name)`
```python
keys = config.getSectionKeys(section_name)
```

**Description**: Get keys belonging to a specific section.

**Parameters**:
- `section_name` (str): Section name

**Returns**:
- `keys` (list[str]): List of keys in the section

**Example**:
```python
config = pynlm.createDefaultConfig()
brain_keys = config.getSectionKeys("brain")
print(brain_keys)  # ["neuron_count", "synapse_density", ...]
```

### Validation and Diagnostics

#### `config.validate()`
```python
is_valid = config.validate()
```

**Description**: Validate configuration for correctness and consistency.

**Parameters**:
- None

**Returns**:
- `is_valid` (bool): True if configuration is valid, False otherwise

**Example**:
```python
config = pynlm.createDefaultConfig()
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration has errors")
```

#### `config.getValidationErrors()`
```python
errors = config.getValidationErrors()
```

**Description**: Get validation errors for this configuration.

**Parameters**:
- None

**Returns**:
- `errors` (list[str]): List of validation error messages

**Example**:
```python
config = pynlm.createDefaultConfig()
errors = config.getValidationErrors()
for error in errors:
    print(f"Error: {error}")
```

### Configuration Categories

The configuration system is organized into logical sections:

#### Brain Configuration
- `brain.neuron_count`
- `brain.synapse_density`
- `brain.connection_probability`
- `brain.v_thresh`
- `brain.v_rest`
- `brain.v_reset`
- `brain.tau_mem`
- `brain.tau_ref`

#### Plasticity Configuration
- `plasticity.stdp.enable`
- `plasticity.stdp.learning_rate`
- `plasticity.stdp.tau_plus`
- `plasticity.stdp.tau_minus`
- `plasticity.hebbian.enable`
- `plasticity.hebbian.learning_rate`
- `plasticity.structural.enable`
- `plasticity.structural.synapse_creation_rate`
- `plasticity.structural.synapse_removal_rate`

#### Neuromodulation Configuration
- `neuromod.dopamine.scale`
- `neuromod.curiosity.enable`
- `neuromod.novelty.enable`
- `neuromod.prediction_error.enable`

#### Development Configuration
- `development.initial_plasticity_rate`
- `development.critical_period_duration`
- `development.maturation_rate`

#### World Configuration
- `world.width`
- `world.height`
- `world.visionWidth`
- `world.visionHeight`

## Error Handling

### Configuration Errors

The Config class raises the following exceptions:

#### ConfigurationError
```python
raise ConfigurationError("Invalid configuration: key not found")
```

**Description**: Base class for configuration errors.

#### InvalidKeyError
```python
raise InvalidKeyError("Key 'nonexistent.key' does not exist")
```

**Description**: Key does not exist in configuration.

#### InvalidTypeError
```python
raise InvalidTypeError("Key 'brain.neuron_count' expects integer, got string")
```

**Description**: Value type mismatch for configuration key.

#### ValueRangeError
```python
raise ValueRangeError("Value for 'brain.neuron_count' must be between 1 and 10000, got 20000")
```

**Description**: Configuration value outside valid range.

### File Operation Errors

#### FileNotFoundError
```python
raise FileNotFoundError("Configuration file 'configs/my_config.json' not found")
```

**Description**: Configuration file doesn't exist.

#### ParseError
```python
raise ParseError("Invalid JSON in configuration file")
```

**Description**: Configuration file contains invalid JSON.

## Configuration File Format

### JSON Structure
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0005
        },
        "structural": {
            "enable": true,
            "synapse_creation_rate": 0.001,
            "synapse_removal_rate": 0.001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        },
        "novelty": {
            "enable": true
        }
    },
    "development": {
        "initial_plasticity_rate": 1.0,
        "critical_period_duration": 1000.0,
        "maturation_rate": 0.001
    },
    "world": {
        "width": 20,
        "height": 20,
        "visionWidth": 8,
        "visionHeight": 8
    }
}
```

### Configuration File Examples

#### Minimal Configuration
```json
{
    "brain": {
        "neuron_count": 500
    }
}
```

#### Learning-Focused Configuration
```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.003
        },
        "structural": {
            "enable": true,
            "synapse_creation_rate": 0.002
        }
    },
    "neuromod": {
        "curiosity": {
            "enable": true,
            "strength": 1.5
        }
    }
}
```

#### High-Performance Configuration
```json
{
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.05,
        "v_thresh": -45.0,
        "tau_mem": 15.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        },
        "hebbian": {
            "enable": false
        },
        "structural": {
            "enable": false
        }
    },
    "development": {
        "initial_plasticity_rate": 0.5,
        "critical_period_duration": 500.0
    }
}
```

## Performance Considerations

### Memory Usage
- Each configuration entry: ~50-100 bytes
- Total memory: O(number of keys)
- Nested sections: Additional overhead for section objects

### Access Time
- `has()`, `get()`, `set()`: O(1) average case
- `getKeys()`: O(n) where n = number of keys
- `getSection()`: O(1) if section exists, O(n) otherwise

### Best Practices
1. **Cache frequently accessed values**: Use `get()` with appropriate types for repeated access
2. **Section organization**: Group related parameters in sections
3. **Validation**: Validate configuration before use
4. **File management**: Use consistent file paths and naming conventions
5. **Inheritance**: Use `merge()` for configuration inheritance

## Compatibility Notes

### Forward Compatibility
- New configuration keys added as optional parameters
- Default behavior preserved for existing keys
- Type conversion improved for new data types

### Backward Compatibility
- Renamed some keys for consistency (e.g., `plasticity.stdp.enable` instead of `stdp.enable`)
- Added validation for required keys in new versions
- Improved error messages for invalid configurations

### Platform Considerations
- Cross-platform file path handling
- JSON encoding compatible with all platforms
- No platform-specific dependencies

## Example Usage Patterns

### Basic Configuration
```python
import pynlm

# Create and use configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Configuration from File
```python
import pynlm
import os

# Load configuration from file
config = pynlm.createDefaultConfig()
if os.path.exists("configs/experiment1.json"):
    config.loadFromFile("configs/experiment1.json")

# Modify loaded configuration
config.set("plasticity.stdp.learning_rate", 0.002)

# Use configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Configuration Validation
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate before use
if config.validate():
    # All good
    brain = pynlm.createBrain(config)
    brain.initialize()
else:
    # Fix errors
    errors = config.getValidationErrors()
    for error in errors:
        print(f"Fix: {error}")
    
    # Retry validation
    if config.validate():
        brain = pynlm.createBrain(config)
        brain.initialize()
```

### Configuration Templates
```python
def create_research_configuration(research_id):
    """Create configuration for research experiment"""
    base_config = pynlm.createDefaultConfig()
    
    if research_id == "learning_speed":
        # Fast learning experiment
        base_config.set("plasticity.stdp.learning_rate", 0.01)
        base_config.set("brain.neuron_count", 1000)
        
    elif research_id == "memory_capacity":
        # Memory capacity experiment
        base_config.set("plasticity.structural.enable", True)
        base_config.set("brain.neuron_count", 3000)
        
    elif research_id == "development":
        # Development experiment
        base_config.set("development.initial_plasticity_rate", 2.0)
        base_config.set("development.critical_period_duration", 2000.0)
    
    return base_config

# Use configuration template
experiment_config = create_research_configuration("learning_speed")
brain = pynlm.createBrain(experiment_config)
brain.initialize()
```

### Configuration Factory
```python
class ConfigFactory:
    """Factory for creating common configuration presets"""
    
    @staticmethod
    def create_default():
        """Create default configuration"""
        return pynlm.createDefaultConfig()
    
    @staticmethod
    def create_minimal():
        """Create minimal configuration with only essential parameters"""
        config = pynlm.createDefaultConfig()
        config.clear()
        config.set("brain.neuron_count", 100)
        return config
    
    @staticmethod
    def create_research(neuron_count=1000, learning_rate=0.001):
        """Create research configuration with specified parameters"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("plasticity.stdp.learning_rate", learning_rate)
        return config
    
    @staticmethod
    def create_production(performance_mode="balanced"):
        """Create production configuration optimized for performance"""
        config = pynlm.createDefaultConfig()
        
        if performance_mode == "fast":
            config.set("brain.neuron_count", 500)
            config.set("plasticity.stdp.enable", False)
            config.set("plasticity.hebbian.enable", False)
            
        elif performance_mode == "memory":
            config.set("brain.neuron_count", 2000)
            config.set("plasticity.structural.enable", True)
            
        elif performance_mode == "balanced":
            # Use defaults
            pass
        
        return config
```

## Conclusion

The Config class provides a flexible, type-safe configuration system essential for managing NLM neural simulation parameters. Its organized structure, validation capabilities, and file I/O support make it suitable for everything from simple testing to complex research experiments.

For more information, see:
- `docs/USAGE/BASIC.md` - Basic usage examples
- `docs/USAGE/INTERMEDIATE.md` - Intermediate usage patterns
- `docs/USAGE/ADVANCED.md` - Advanced features and customization
- `examples/` - Complete working examples
- `configs/` - Example configuration files
