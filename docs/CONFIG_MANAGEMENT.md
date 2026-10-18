# Enhanced Configuration Management Documentation

# Overview

This document provides comprehensive information about the enhanced configuration management system in NLM, including JSON/YAML support, validation, and advanced features.

## Key Improvements

### 1. Multi-Format Support

The enhanced Config class provides support for multiple configuration file formats:

```python
import pynlm

# JSON configuration
config = pynlm.createEnhancedConfig("config.json")

# YAML configuration
config = pynlm.createEnhancedConfig("config.yaml")

# Auto-detection based on file extension
config = pynlm.createEnhancedConfig("settings.conf")  # Automatically detected
```

### 2. Smart Format Detection

The system intelligently detects file format based on:

- **File Extension**: `.json`, `.yaml`, `.yml`, `.txt`, `.conf`, etc.
- **File Content**: JSON object/array, YAML key-value pairs, simple key=value format
- **Fallback**: Graceful fallback to simple format for compatibility

### 3. Enhanced Validation

```python
import pynlm

# Type validation
config = pynlm.createEnhancedConfig()
config.set("brain.neuron_count", 1000)  # Valid
config.set("brain.neuron_count", "invalid")  # Invalid - throws ConfigurationError

# Range validation
config.set("brain.v_thresh", -100.0)  # Valid
config.set("brain.v_thresh", 100.0)   # Valid - clamped to -100.0
config.set("brain.v_thresh", -999999.0)  # Valid - not clamped
```

### 4. Error Recovery

```python
import pynlm

# Configuration loading with fallback
try:
    config = pynlm.createEnhancedConfig("production.json")
except pynlm.ConfigurationError as e:
    print(f"Failed to load configuration: {e}")
    # Automatic fallback to defaults
    config = pynlm.createEnhancedConfig()\
        .set("brain.neuron_count", 1000)\
        .set("plasticity.enable", True)\
        .set("neuromodulation.curiosity.enable", True)
    print("Using default configuration")
```

## Configuration File Formats

### JSON Configuration

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "v_thresh": -55.0,
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
      "tau_minus": 20.0
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
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "scale": 1.0
    },
    "novelty": {
      "enable": true,
      "scale": 1.0
    },
    "prediction_error": {
      "enable": true,
      "scale": 1.0
    }
  },
  "memory": {
    "working_memory": {
      "capacity": 1000,
      "decay_rate": 0.01
    },
    "episodic_memory": {
      "max_episodes": 10000,
      "replay_enabled": true
    }
  },
  "performance": {
    "enable_monitoring": true,
    "log_rate_limit": 10000,
    "memory_limit_mb": 1024
  },
  "development": {
    "enable": true,
    "critical_period_duration": 100.0,
    "maturation_rate": 0.01
  }
}
```

### YAML Configuration

```yaml
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.1
  v_thresh: -55.0
  v_rest: -70.0
  v_reset: -75.0
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
    learning_rate: 0.01
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
    scale: 1.0
  novelty:
    enable: true
    scale: 1.0
  prediction_error:
    enable: true
    scale: 1.0

memory:
  working_memory:
    capacity: 1000
    decay_rate: 0.01
  episodic_memory:
    max_episodes: 10000
    replay_enabled: true

performance:
  enable_monitoring: true
  log_rate_limit: 10000
  memory_limit_mb: 1024

development:
  enable: true
  critical_period_duration: 100.0
  maturation_rate: 0.01
```

### Simple Key=Value Configuration

```
# Configuration file format
# Comments start with #
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
brain.v_thresh = -55.0
brain.v_rest = -70.0
brain.v_reset = -75.0
brain.tau_mem = 20.0
brain.tau_ref = 2.0

plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0
plasticity.hebbian.enable = true
plasticity.hebbian.learning_rate = 0.01
plasticity.structural.enable = true
plasticity.structural.synaptogenesis_rate = 0.0001
plasticity.structural.pruning_rate = 0.00001

neuromodulation.dopamine.scale = 1.0
neuromodulation.curiosity.enable = true
neuromodulation.curiosity.scale = 1.0
neuromodulation.novelty.enable = true
neuromodulation.novelty.scale = 1.0
neuromodulation.prediction_error.enable = true
neuromodulation.prediction_error.scale = 1.0

memory.working_memory.capacity = 1000
memory.working_memory.decay_rate = 0.01
memory.episodic_memory.max_episodes = 10000
memory.episodic_memory.replay_enabled = true

performance.enable_monitoring = true
performance.log_rate_limit = 10000
performance.memory_limit_mb = 1024

development.enable = true
development.critical_period_duration = 100.0
development.maturation_rate = 0.01
```

## Configuration Validation

### 1. Type Validation

```python
import pynlm

config = pynlm.createEnhancedConfig()

# Valid types
config.set("brain.neuron_count", 1000)  # int
config.set("brain.connection_probability", 0.1)  # double
config.set("brain.description", "Test brain")  # string
config.set("plasticity.enable", True)  # bool
config.set("brain.synapse_list", [1, 2, 3])  # list of ints
config.set("brain.weight_list", [0.1, 0.5, 0.9])  # list of doubles
config.set("brain.name_list", ["A", "B", "C"])  # list of strings

# Invalid types - throw ConfigurationError
try:
    config.set("brain.neuron_count", "invalid")  # Should be int
except pynlm.ConfigurationError as e:
    print(f"Type validation error: {e}")
```

### 2. Range Validation

```python
import pynlm

config = pynlm.createEnhancedConfig()

# Range validation for integer values
config.set("brain.neuron_count", 100)  # Valid
config.set("brain.neuron_count", 10000)  # Valid
config.set("brain.neuron_count", 50)  # Valid

# Range validation for floating-point values
config.set("brain.v_thresh", -100.0)  # Valid
config.set("brain.v_thresh", -55.0)  # Valid
config.set("brain.v_thresh", 100.0)  # Valid (not clamped)
config.set("brain.v_thresh", -999.0)  # Valid (not clamped)

# Range validation for list values
config.set("brain.weight_list", [0.0, 0.5, 1.0])  # Valid
config.set("brain.weight_list", [-0.5, 0.0, 0.5])  # Valid
config.set("brain.weight_list", [0.0, 0.5, 2.0])  # Valid (not clamped)
```

### 3. String Validation

```python
import pynlm

config = pynlm.createEnhancedConfig()

# String validation
config.set("brain.name", "MyBrain")  # Valid
config.set("brain.description", "A test brain for NLM simulation")  # Valid
config.set("brain.tag", "research")  # Valid

# Special characters in strings
config.set("brain.path", "/home/user/brain")  # Valid
config.set("brain.command", "python train.py")  # Valid
config.set("brain.url", "https://example.com")  # Valid
```

## Configuration Chaining

### 1. Builder Pattern

```python
import pynlm

# Chained configuration for better readability
config = (pynlm.createEnhancedConfig()
          .brain("1000", "0.1")
          .plasticity(True, 0.001)
          .neuromodulation(True, 1.0)
          .memory(1000, 0.01))

# Alternative chaining style
config = pynlm.createEnhancedConfig()\
    .set("brain.neuron_count", 1000)\
    .set("brain.connection_probability", 0.1)\
    .set("plasticity.enable", True)\
    .set("plasticity.stdp.learning_rate", 0.001)\
    .set("neuromodulation.curiosity.enable", True)\
    .set("neuromodulation.curiosity.scale", 1.0)\
    .set("memory.capacity", 1000)\
    .set("memory.decay_rate", 0.01)
```

### 2. Preset Configurations

```python
import pynlm

# Preset configurations for common use cases
def createMinimalConfig():
    """Create minimal configuration for basic testing"""
    return (pynlm.createEnhancedConfig()
            .set("brain.neuron_count", 100)
            .set("brain.connection_probability", 0.05)
            .set("plasticity.enable", False)
            .set("neuromodulation.curiosity.enable", False))

def createResearchConfig():
    """Create research configuration with advanced features"""
    return (pynlm.createEnhancedConfig()
            .set("brain.neuron_count", 5000)
            .set("brain.connection_probability", 0.2)
            .set("plasticity.enable", True)
            .set("plasticity.stdp.learning_rate", 0.01)
            .set("plasticity.hebbian.learning_rate", 0.1)
            .set("neuromodulation.curiosity.enable", True)
            .set("neuromodulation.novelty.enable", True)
            .set("neuromodulation.prediction_error.enable", True)
            .set("memory.working_memory.capacity", 5000)
            .set("memory.episodic_memory.max_episodes", 50000)
            .set("development.enable", True)
            .set("performance.enable_monitoring", True))

def createRealTimeConfig():
    """Create real-time configuration for performance"""
    return (pynlm.createEnhancedConfig()
            .set("brain.neuron_count", 2000)
            .set("brain.connection_probability", 0.1)
            .set("plasticity.enable", True)
            .set("plasticity.stdp.learning_rate", 0.005)
            .set("neuromodulation.curiosity.enable", True)
            .set("performance.enable_monitoring", True)
            .set("performance.log_rate_limit", 5000))

# Usage
minimal_config = createMinimalConfig()
research_config = createResearchConfig()
realtime_config = createRealTimeConfig()
```

## Configuration Export and Import

### 1. JSON Export

```python
import pynlm
import json

config = pynlm.createEnhancedConfig()\
    .set("brain.neuron_count", 1000)\
    .set("brain.connection_probability", 0.1)\
    .set("plasticity.enable", True)

# Export to JSON
json_str = config.toJSON()
print(json_str)

# Save to file
config.save("exported_config.json")

# Load from file
loaded_config = pynlm.createEnhancedConfig("exported_config.json")
```

### 2. YAML Export

```python
import pynlm
import yaml

config = pynlm.createEnhancedConfig()\
    .set("brain.neuron_count", 1000)\
    .set("plasticity.enable", True)

# Export to YAML (requires yaml library)
try:
    yaml_str = config.toYAML()
    print(yaml_str)
except ImportError:
    print("PyYAML not installed, cannot export to YAML")

# Note: YAML export requires additional dependencies
```

### 3. Configuration Comparison

```python
import pynlm
import difflib

def compareConfigurations(config1, config2):
    """Compare two configurations"""
    keys1 = config1.getKeys()
    keys2 = config2.getKeys()
    
    print("Keys in config1:", keys1)
    print("Keys in config2:", keys2)
    
    # Find differences
    diff = difflib.unified_diff(
        sorted(keys1),
        sorted(keys2),
        fromfile='config1',
        tofile='config2',
        lineterm=''
    )
    
    differences = list(diff)
    if differences:
        print("Differences:")
        for line in differences:
            print(line)
    else:
        print("Configurations are identical")

# Usage
config1 = pynlm.createEnhancedConfig()\
    .set("brain.neuron_count", 1000)\
    .set("plasticity.enable", True)

config2 = pynlm.createEnhancedConfig()\
    .set("brain.neuron_count", 1000)\
    .set("plasticity.enable", False)

compareConfigurations(config1, config2)
```

## Configuration Validation Examples

### 1. Complete Configuration Example

```python
import pynlm
import json

# Create complete configuration
complete_config = pynlm.createEnhancedConfig()\
    .set("brain.neuron_count", 2000)\
    .set("brain.region_count", 1)\
    .set("brain.connection_probability", 0.15)\
    .set("brain.v_thresh", -55.0)\
    .set("brain.v_rest", -70.0)\
    .set("brain.v_reset", -75.0)\
    .set("brain.tau_mem", 20.0)\
    .set("brain.tau_ref", 2.0)\
    .set("plasticity.stdp.enable", True)\
    .set("plasticity.stdp.learning_rate", 0.01)\
    .set("plasticity.stdp.tau_plus", 20.0)\
    .set("plasticity.stdp.tau_minus", 20.0)\
    .set("plasticity.hebbian.enable", True)\
    .set("plasticity.hebbian.learning_rate", 0.05)\
    .set("plasticity.structural.enable", True)\
    .set("plasticity.structural.synaptogenesis_rate", 0.0002)\
    .set("plasticity.structural.pruning_rate", 0.00002)\
    .set("neuromodulation.dopamine.scale", 1.0)\
    .set("neuromodulation.curiosity.enable", True)\
    .set("neuromodulation.curiosity.scale", 1.0)\
    .set("neuromodulation.novelty.enable", True)\
    .set("neuromodulation.novelty.scale", 1.0)\
    .set("neuromodulation.prediction_error.enable", True)\
    .set("neuromodulation.prediction_error.scale", 1.0)\
    .set("memory.working_memory.capacity", 2000)\
    .set("memory.working_memory.decay_rate", 0.01)\
    .set("memory.episodic_memory.max_episodes", 20000)\
    .set("memory.episodic_memory.replay_enabled", True)\
    .set("performance.enable_monitoring", True)\
    .set("performance.log_rate_limit", 20000)\
    .set("development.enable", True)\
    .set("development.critical_period_duration", 200.0)\
    .set("development.maturation_rate", 0.02)

# Validate configuration
required_keys = [
    "brain.neuron_count",
    "brain.connection_probability",
    "plasticity.enable",
    "neuromodulation.curiosity.enable",
    "performance.enable_monitoring"
]

for key in required_keys:
    if not complete_config.has(key):
        raise pynlm.ConfigurationError(f"Missing required configuration key: {key}")

print("Configuration is valid")

# Export configuration
config_dict = {
    "brain.neuron_count": complete_config.getOr<int>("brain.neuron_count", 0),
    "brain.connection_probability": complete_config.getOr<double>("brain.connection_probability", 0.0),
    "plasticity.enable": complete_config.getOr<bool>("plasticity.enable", false),
    "neuromodulation.curiosity.enable": complete_config.getOr<bool>("neuromodulation.curiosity.enable", false)
}

print("Configuration as dictionary:", config_dict)
```

### 2. Configuration File Generator

```python
import pynlm
import json
import os

def generateConfigurationFile(filename, config_dict):
    """Generate configuration file from dictionary"""
    try:
        # Try to save as JSON
        config = pynlm.createEnhancedConfig()
        
        # Set values from dictionary
        for key, value in config_dict.items():
            if isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
            elif isinstance(value, list):
                if all(isinstance(v, int) for v in value):
                    config.set(key, value)
                elif all(isinstance(v, float) for v in value):
                    config.set(key, value)
                elif all(isinstance(v, str) for v in value):
                    config.set(key, value)
        
        # Save to file
        config.save(filename)
        print(f"Configuration saved to {filename}")
        return True
        
    except Exception as e:
        print(f"Failed to generate configuration file: {e}")
        return False

# Configuration dictionary
config_dict = {
    "brain.neuron_count": 1000,
    "brain.connection_probability": 0.1,
    "plasticity.enable": True,
    "plasticity.stdp.learning_rate": 0.001,
    "neuromodulation.curiosity.enable": True,
    "neuromodulation.novelty.scale": 1.0,
    "memory.working_memory.capacity": 1000,
    "performance.enable_monitoring": True
}

# Generate configuration file
generateConfigurationFile("generated_config.json", config_dict)
```

## Configuration Best Practices

### 1. Use Environment Variables

```python
import os
import pynlm

# Configuration from environment variables
config = pynlm.createEnhancedConfig()

# Set values from environment variables
if "NLM_NEURON_COUNT" in os.environ:
    config.set("brain.neuron_count", int(os.environ["NLM_NEURON_COUNT"]))

if "NLM_CONNECTION_PROBABILITY" in os.environ:
    config.set("brain.connection_probability", float(os.environ["NLM_CONNECTION_PROBABILITY"]))

if "NLM_ENABLE_STDP" in os.environ:
    config.set("plasticity.stdp.enable", os.environ["NLM_ENABLE_STDP"].lower() == "true")

if "NLM_ENABLE_CURIOUSITY" in os.environ:
    config.set("neuromodulation.curiosity.enable", os.environ["NLM_ENABLE_CURIOUSITY"].lower() == "true")

print("Configuration from environment variables")
```

### 2. Configuration Validation Script

```python
import pynlm
import sys
import json

def validateConfigurationFile(filepath):
    """Validate configuration file"""
    try:
        config = pynlm.createEnhancedConfig(filepath)
        
        # Required keys
        required_keys = [
            "brain.neuron_count",
            "brain.connection_probability",
            "plasticity.enable"
        ]
        
        missing_keys = []
        for key in required_keys:
            if not config.has(key):
                missing_keys.append(key)
        
        if missing_keys:
            print(f"Error: Missing required configuration keys: {missing_keys}")
            return False
        
        # Validate key values
        if not (100 <= config.getOr<int>("brain.neuron_count", 0) <= 10000):
            print("Error: brain.neuron_count must be between 100 and 10000")
            return False
        
        if not (0.0 <= config.getOr<double>("brain.connection_probability", 0.0) <= 1.0):
            print("Error: brain.connection_probability must be between 0.0 and 1.0")
            return False
        
        if not isinstance(config.getOr<bool>("plasticity.enable", false), bool):
            print("Error: plasticity.enable must be a boolean")
            return False
        
        print("Configuration file is valid")
        return True
        
    except pynlm.ConfigurationError as e:
        print(f"Error: Configuration file is invalid: {e}")
        return False

# Usage
if len(sys.argv) != 2:
    print("Usage: python validate_config.py <config_file>")
    sys.exit(1)

config_file = sys.argv[1]
if not validateConfigurationFile(config_file):
    sys.exit(1)
```

## Conclusion

Enhanced configuration management provides:

1. **Multiple Format Support**: JSON, YAML, and simple key=value formats
2. **Smart Auto-Detection**: Automatic format detection based on file content
3. **Robust Validation**: Comprehensive input validation and error recovery
4. **Flexible Chaining**: Builder pattern for readable configuration
5. **Export/Import**: Easy configuration export and import
6. **Best Practices**: Environment variable support and validation scripts

These improvements make configuration management more robust, user-friendly, and suitable for both beginners and advanced users.

## Additional Resources

### Configuration Examples

- **examples/basic_config.py**: Basic configuration examples
- **examples/advanced_config.py**: Advanced configuration features
- **examples/config_validation.py**: Configuration validation examples
- **examples/config_export.py**: Configuration export examples

### Related Documentation

- **ENHANCED_ERROR_HANDLING.md**: Error handling documentation
- **PERFORMANCE_MONITORING.md**: Performance monitoring documentation
- **DEBUGGING_GUIDE.md**: Debugging and troubleshooting guide

---

*This document covers the enhanced configuration management features and improvements made to NLM. For specific questions or additional information, please refer to the individual documentation files.*