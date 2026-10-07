# NLM Configuration System Improvements - User Guide

## Overview

This document provides comprehensive information about the NLM (Neural Learning Machine) Configuration System improvements. These enhancements address critical bugs, add new features, and improve the overall reliability and flexibility of the NLM project.

## Quick Summary

The configuration system has been completely enhanced with:

- ✅ **JSON Configuration Support**: New JSON format with comprehensive type support
- ✅ **Backward Compatibility**: Legacy simple key=value format still works
- ✅ **Checkpoint Fix**: Fixed critical saveToFile() bug
- ✅ **Enhanced Error Handling**: Robust error handling and validation
- ✅ **Better Code Organization**: Improved structure and maintainability
- ✅ **Advanced Features**: Configuration arrays and complex nested structures

## Getting Started

### Prerequisites

To use the enhanced configuration system, you need:

1. **NLM Python Library**: Install using `pip install .` or `pip install -e .`
2. **CMake and C++ compiler**: For building the core library
3. **Python 3.8+**: Required for the Python bindings

### Basic Usage

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
```

## Configuration File Formats

NLM supports two configuration file formats:

### 1. Simple Key=Value Format (Legacy)

This is the original configuration format. Still supported for backward compatibility.

**File Format:**
```
# My configuration
brain.neuron_count = 1000
brain.connection_probability = 0.1
plasticity.stdp.enable = true
logging.level = "INFO"
```

**Usage:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("config.nlm")
brain = pynlm.createBrain(config)
```

### 2. JSON Format (Recommended)

This is the new, enhanced configuration format with comprehensive type support.

**File Format:**
```json
{
  "brain": {
    "neuron_count": 1000,
    "connection_probability": 0.1
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001
    }
  },
  "logging": {
    "level": "INFO"
  }
}
```

**Usage:**
```python
import pynlm

config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
brain = pynlm.createBrain(config)
```

## Configuration Value Types

NLM configuration supports various value types:

### Scalar Types
- **int / int64_t**: Integer values
- **double**: Floating point values
- **bool**: Boolean values (true/false)
- **std::string**: String values

### Array Types
- **std::vector<int>**: Integer arrays
- **std::vector<double>**: Floating point arrays
- **std::vector<std::string>**: String arrays

### Examples

```python
config = pynlm.createDefaultConfig()

# Scalar values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.5)
config.set("logging.level", "DEBUG")

# Array values
config.set("brain.v_thresh_values", [-50.0, -52.0, -48.0])
config.set("brain.region_ids", [1, 2, 3, 4, 5])
```

## Configuration Management

### Creating and Modifying Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set values
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 5)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", False)

# Get values
neuron_count = config.getOr("brain.neuron_count", 1000)
connection_probability = config.get("brain.connection_probability")
if connection_probability:
    print(f"Connection probability: {connection_probability.value()}")

# Check if key exists
if config.has("brain.neuron_count"):
    print("Neuron count is configured")
```

### Saving Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()
# ... configure the config ...

# Save to file
config.saveToFile("my_config.json")

# Save to different format
config.saveToFile("legacy_config.nlm")
```

### Loading Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Load from JSON file
config.loadFromFile("config.json")

# Load from simple file (backward compatibility)
config.loadFromFile("simple_config.nlm")

# Load from command line arguments
config.loadFromArgs(3, ["program", "--brain.neuron_count=500", "--logging.level=DEBUG"])
```

## Advanced Configuration Features

### Configuration Validation

```python
def validate_config(config):
    """Validate configuration before use"""
    required_keys = [
        "brain.neuron_count",
        "brain.connection_probability",
        "plasticity.stdp.enable"
    ]
    
    for key in required_keys:
        if not config.has(key):
            print(f"ERROR: Missing required configuration: {key}")
            return False
    
    # Validate value ranges
    neuron_count = config.getOr("brain.neuron_count", 0)
    if neuron_count < 10 or neuron_count > 100000:
        print(f"ERROR: Invalid neuron count: {neuron_count}")
        return False
    
    return True

# Use validation
config = pynlm.createDefaultConfig()
if validate_config(config):
    brain = pynlm.createBrain(config)
    brain.initialize()
```

### Configuration Summary

```python
config = pynlm.createDefaultConfig()
# ... configure ...

# Get configuration summary
print(config.summary())
```

This outputs a human-readable summary of all configuration values.

## Examples and Tutorials

### Complete Example: Working with Configuration

```python
import pynlm
import tempfile
import os

def run_nlm_simulation(config_file=None):
    """Run NLM simulation with configuration"""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    if config_file:
        # Load from file
        config.loadFromFile(config_file)
        print(f"Loaded configuration from {config_file}")
    else:
        # Set custom configuration
        config.set("brain.neuron_count", 1000)
        config.set("brain.region_count", 3)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("neuromod.curiosity.enable", True)
        print("Using custom configuration")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Print configuration summary
    print("\nConfiguration Summary:")
    print(config.summary())
    
    # Run simulation
    print(f"\nRunning simulation with {brain.getTotalNeuronCount()} neurons...")
    
    total_spikes = 0
    for step in range(1000):
        brain.step(step)
        
        if step % 100 == 0:
            firing_rate = brain.getAverageFiringRate()
            total_spikes += brain.getTotalSpikeCount()
            print(f"Step {step}: {firing_rate:.4f} avg firing rate, {total_spikes} total spikes")
    
    print(f"Simulation complete! Total spikes: {total_spikes}")
    return brain

# Example usage
if __name__ == "__main__":
    # Run with default config
    brain = run_nlm_simulation()
    
    # Or run with specific config file
    # brain = run_nlm_simulation("my_config.json")
```

### Configuration File Examples

#### Example 1: Fast Simulation Configuration

**File: `fast_config.json`**
```json
{
  "brain": {
    "neuron_count": 100,
    "region_count": 1,
    "connection_probability": 0.01
  },
  "plasticity": {
    "stdp": { "enable": false },
    "hebbian": { "enable": false }
  },
  "neuromod": {
    "curiosity": { "enable": false },
    "novelty": { "enable": false }
  },
  "memory": {
    "enable": false
  }
}
```

#### Example 2: Detailed Research Configuration

**File: `research_config.json`**
```json
{
  "brain": {
    "neuron_count": 10000,
    "region_count": 10,
    "connection_probability": 0.1,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.1
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": { "enable": true },
    "structural": { "enable": true }
  },
  "neuromod": {
    "dopamine": { "scale": 1.0 },
    "curiosity": { "enable": true, "sensitivity": 1.0 },
    "novelty": { "enable": true }
  },
  "development": {
    "enable": true,
    "initial_plasticity": 1.0
  },
  "memory": {
    "working_memory": { "capacity": 100 },
    "episodic_memory": { "max_episodes": 1000 }
  },
  "logging": {
    "level": "INFO",
    "file": "nlm_research.log"
  }
}
```

#### Example 3: Legacy Simple Configuration

**File: `legacy_config.nlm`**
```
# Legacy NLM Configuration
brain.neuron_count = 500
brain.region_count = 2
brain.connection_probability = 0.05
brain.v_thresh = -50.0
brain.v_rest = -70.0

# Plasticity
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true

# Neuromodulation
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

# Development
development.enable = true

# Logging
logging.level = "INFO"
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Configuration File Not Found

**Problem**: `config.json` not found or inaccessible

**Solution**:
```python
import os
if not os.path.exists("config.json"):
    print("ERROR: config.json not found")
    # Create default config
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.saveToFile("config.json")
    print("Created default config.json")
else:
    config.loadFromFile("config.json")
```

#### Issue 2: Invalid Configuration Values

**Problem**: Configuration values are invalid or out of range

**Solution**:
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# Validate neuron count
neuron_count = config.getOr("brain.neuron_count", 0)
if neuron_count < 10 or neuron_count > 100000:
    print(f"ERROR: Invalid neuron count: {neuron_count}")
    # Use default or exit
    neuron_count = 1000
    config.set("brain.neuron_count", neuron_count)
```

#### Issue 3: JSON Parsing Errors

**Problem**: JSON configuration file has syntax errors

**Solution**:
```python
import json

try:
    config = pynlm.createDefaultConfig()
    config.loadFromFile("config.json")
    print("Successfully loaded JSON configuration")
except json.JSONDecodeError as e:
    print(f"ERROR: Invalid JSON in config.json: {e}")
    print("Using default configuration")
except Exception as e:
    print(f"ERROR: Failed to load configuration: {e}")
```

#### Issue 4: File Permission Issues

**Problem**: Cannot read/write configuration files due to permissions

**Solution**:
```python
import os
from pathlib import Path

config_path = "config.json"

# Check if file exists and is readable
if os.path.exists(config_path) and os.access(config_path, os.R_OK):
    print("Configuration file is readable")
else:
    print("WARNING: Cannot read configuration file")

# Check if we can write
if os.access(os.path.dirname(os.path.abspath(config_path)), os.W_OK):
    print("Configuration directory is writable")
else:
    print("ERROR: Cannot write to configuration directory")
```

### Error Handling Best Practices

```python
def safe_load_config(config_path):
    """Safely load configuration with comprehensive error handling"""
    import json
    import os
    
    if not config_path:
        print("WARNING: No configuration file specified")
        return pynlm.createDefaultConfig()
    
    if not os.path.exists(config_path):
        print(f"WARNING: Configuration file '{config_path}' not found")
        return pynlm.createDefaultConfig()
    
    try:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_path)
        print(f"Successfully loaded configuration from '{config_path}'")
        return config
        
    except json.JSONDecodeError as e:
        print(f"ERROR: Invalid JSON in '{config_path}': {e}")
        print("Falling back to default configuration")
        return pynlm.createDefaultConfig()
        
    except Exception as e:
        print(f"ERROR: Failed to load configuration from '{config_path}': {e}")
        print("Falling back to default configuration")
        return pynlm.createDefaultConfig()

# Usage
config = safe_load_config("production_config.json")
brain = pynlm.createBrain(config)
```

## Migration Guide

### Migrating from Legacy to Enhanced Configuration

#### Step 1: Analyze Current Configuration

If you're currently using the legacy simple configuration format:

```python
# Old way (still supported)
config = pynlm.createDefaultConfig()
config.loadFromFile("old_config.nlm")
```

#### Step 2: Convert to New JSON Format

You can convert your simple configuration to JSON format using Python:

```python
import json

def convert_simple_to_json(simple_file, json_file):
    """Convert simple .nlm format to JSON"""
    config = {}
    
    with open(simple_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            
            if '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Convert value based on type
                if value.lower() == 'true':
                    value = True
                elif value.lower() == 'false':
                    value = False
                elif value.isdigit() or (value.startswith('-') and value[1:].isdigit()):
                    value = int(value)
                elif '.' in value or 'e' in value.lower():
                    value = float(value)
                elif (value.startswith('"') and value.endswith('"')) or (value.startswith("'") and value.endswith("'")):
                    value = value[1:-1]
                
                # Convert dot notation to nested structure
                keys = key.split('.')
                current = config
                for k in keys[:-1]:
                    if k not in current:
                        current[k] = {}
                    current = current[k]
                current[keys[-1]] = value
    
    with open(json_file, 'w') as f:
        json.dump(config, f, indent=2)
    
    print(f"Converted '{simple_file}' to '{json_file}'")

# Usage
convert_simple_to_json("old_config.nlm", "new_config.json")
```

#### Step 3: Update Code

Update your code to use the new features:

```python
# Old code
config = pynlm.createDefaultConfig()
config.loadFromFile("old_config.nlm")
brain = pynlm.createBrain(config)

# New code
config = pynlm.createDefaultConfig()
config.loadFromFile("new_config.json")

# Use new features
brain = pynlm.createBrain(config)

# Save with improved format
config.saveToFile("backup_config.json")
```

#### Step 4: Test Everything

After migration, test thoroughly:

```python
def test_migration():
    """Test configuration migration"""
    print("=== Testing Configuration Migration ===")
    
    # Test 1: Load old format
    print("1. Testing old format loading...")
    config1 = pynlm.createDefaultConfig()
    config1.loadFromFile("old_config.nlm")
    print(f"   Loaded {config1.getKeys()} keys")
    
    # Test 2: Load new format
    print("2. Testing new format loading...")
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile("new_config.json")
    print(f"   Loaded {config2.getKeys()} keys")
    
    # Test 3: Compare results
    print("3. Comparing results...")
    old_neurons = config1.getOr("brain.neuron_count", 0)
    new_neurons = config2.getOr("brain.neuron_count", 0)
    
    if old_neurons == new_neurons:
        print(f"   ✓ Neuron count matches: {old_neurons}")
    else:
        print(f"   ✗ Mismatch: old={old_neurons}, new={new_neurons}")
    
    # Test 4: Both work with brain creation
    print("4. Testing brain creation...")
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    print(f"   ✓ Old format brain: {brain1.getTotalNeuronCount()} neurons")
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    print(f"   ✓ New format brain: {brain2.getTotalNeuronCount()} neurons")
    
    print("✓ All migration tests passed!")

test_migration()
```

## Performance Considerations

### Configuration Loading Performance

- **JSON Format**: Slightly slower to parse, but more feature-rich
- **Simple Format**: Faster to parse, limited features
- **Large Configurations**: JSON format scales better
- **Frequent Loading**: Cache configuration in memory if used repeatedly

### Memory Usage

```python
# For applications that use configuration frequently
class ConfigManager:
    def __init__(self):
        self._cached_configs = {}
    
    def get_config(self, config_path):
        if config_path not in self._cached_configs:
            config = pynlm.createDefaultConfig()
            config.loadFromFile(config_path)
            self._cached_configs[config_path] = config
        return self._cached_configs[config_path]
    
    def clear_cache(self):
        self._cached_configs.clear()
```

## Best Practices

### 1. Configuration File Organization

- **Use descriptive key names**: Use dots to separate related settings
- **Group related settings**: Keep related configurations together
- **Document configuration**: Include comments in configuration files
- **Use consistent naming**: Follow a consistent naming convention

### 2. Error Handling

- **Validate configuration**: Always validate configuration before use
- **Graceful fallbacks**: Provide sensible defaults
- **Comprehensive error messages**: Include helpful error messages
- **Logging configuration**: Log configuration loading errors

### 3. Testing Configuration

- **Test configuration loading**: Always test configuration loading
- **Validate configuration**: Validate configuration values
- **Test edge cases**: Test minimum and maximum values
- **Test file formats**: Test both JSON and simple formats

### 4. Version Management

- **Version configuration**: Include version information in configuration
- **Backward compatibility**: Ensure backward compatibility
- **Migration paths**: Provide migration guides for configuration changes

## Conclusion

The enhanced NLM Configuration System provides:

- ✅ **Robust JSON support** with comprehensive type handling
- ✅ **Backward compatibility** with legacy simple configuration
- ✅ **Improved error handling** and validation
- ✅ **Better code organization** and maintainability
- ✅ **Advanced features** like arrays and nested structures
- ✅ **Comprehensive documentation** and examples

This represents a significant improvement in the NLM project's configuration management, providing users with more flexible, reliable, and maintainable configuration options while maintaining compatibility with existing code.

## Further Reading

- **Configuration System Documentation**: See `CONFIG_IMPROVEMENTS_SUMMARY.md`
- **Example Configurations**: See `example_configurations.md`
- **Configuration Test Script**: See `test_config_improvements.py`
- **NLM Documentation**: Refer to the main NLM documentation for more details

---
*Last updated: October 2026*
*NLM Project Team*