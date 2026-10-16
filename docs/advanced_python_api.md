# Advanced Python API Documentation

## Overview

This document covers the advanced Pythonic API features available in NLM's enhanced Python bindings. The API follows Python conventions with snake_case naming and provides comprehensive access to all neural systems.

## Key Design Principles

### Pythonic Naming
- All methods and properties use snake_case naming
- Accessors are provided as read-only properties where appropriate
- Clear method names that match their functionality

### Smart Memory Management
- Automatic memory management through shared_ptr
- Reference policies for performance and safety
- Smart reference holders to prevent dangling references

### Comprehensive Error Handling
- Custom exception hierarchy for different error types
- Context-rich error messages
- Proper exception propagation

### Performance Optimizations
- Batch operations for large-scale simulations
- Pythonic interfaces with underlying C++ efficiency
- Optional features to minimize overhead

## Class Documentation

### Config Class

The Config class provides a Pythonic interface to configuration management.

#### Pythonic Accessors

```python
# Property-based access
config = pynlm.createDefaultConfig()

# Type-safe getters with optional returns
value = config.get("brain.neuron_count")  # Returns Optional[int]
count = config.get_int("brain.neuron_count")  # Returns int with default
rate = config.get_float("brain.learning.rate")  # Returns float with default
enabled = config.get_bool("brain.plasticity.enable")  # Returns bool with default
name = config.get_str("brain.region.name")  # Returns str with default

# Type-safe setters
config.set("brain.neuron_count", 5000)
config.set("brain.learning.rate", 0.001)
config.set("brain.plasticity.enable", True)

# Convenience methods
config.loadFromFile("config.json")
config.saveToFile("config.json")
config.has("brain.neuron_count")
config.getKeys()
config.clear()
summary = config.summary()  # Get configuration summary

# Pythonic behavior
len(config)  # Number of configuration keys
str(config)  # Configuration summary
repr(config)  # Detailed representation
```

#### Configuration Patterns

```python
# Pattern 1: Configuration factory
class NeuralConfig:
    @staticmethod
    def get_production_config():
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 10000)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.curiosity.enable", True)
        return config
    
    @staticmethod
    def get_learning_config():
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.2)
        config.set("brain.working_memory.enable", True)
        config.set("brain.episodic_memory.enable", True)
        config.set("neuromod.curiosity.enable", True)
        return config

# Pattern 2: Configuration comparison
def compare_configs(config1, config2):
    """Compare two configurations and show differences."""
    keys1 = set(config1.getKeys())
    keys2 = set(config2.getKeys())
    
    only_in_1 = keys1 - keys2
    only_in_2 = keys2 - keys1
    different = set()
    
    for key in keys1 & keys2:
        if config1.get(key) != config2.get(key):
            different.add(key)
    
    if only_in_1:
        print(f"Only in config1: {sorted(only_in_1)}")
    if only_in_2:
        print(f"Only in config2: {sorted(only_in_2)}")
    if different:
        print(f"Different values: {sorted(different)}")

# Pattern 3: Validation
def validate_config(config):
    """Validate configuration parameters."""
    errors = []
    
    if config.get_int("brain.neuron_count", 0) < 100:
        errors.append("Too few neurons (minimum 100)")
    
    if config.get_int("brain.neuron_count", 0) > 100000:
        errors.append("Too many neurons (maximum 100000)")
    
    if config.get_float("brain.synapse_density", 0) < 0.01:
        errors.append("Synapse density too low")
    
    if config.get_float("brain.synapse_density", 0) > 0.5:
        errors.append("Synapse density too high")
    
    return errors
```

#### Working with Configurations in Complex Applications

```python
# Advanced configuration management with version control
class ConfigManager:
    def __init__(self, config_dir="configs/"):
        self.config_dir = config_dir
        self.configs = {}
        self.current_version = "default"
    
    def load_config(self, name, filename=None):
        """Load a configuration from file."""
        if filename is None:
            filename = f"{name}.json"
        
        config = pynlm.createDefaultConfig()
        config.loadFromFile(f"{self.config_dir}/{filename}")
        self.configs[name] = config
        return config
    
    def create_versioned_config(self, base_name, version, **kwargs):
        """Create a versioned configuration based on another."""
        if base_name not in self.configs:
            raise ValueError(f"Base config '{base_name}' not found")
        
        config = self.configs[base_name]
        for key, value in kwargs.items():
            config.set(key, value)
        
        versioned_name = f"{base_name}_v{version}"
        self.configs[versioned_name] = config
        return config
    
    def compare_configs(self, name1, name2):
        """Compare two configurations."""
        if name1 not in self.configs or name2 not in self.configs:
            raise ValueError("One or both configurations not found")
        
        config1 = self.configs[name1]
        config2 = self.configs[name2]
        
        # Implementation similar to earlier pattern
        # ...

# Pattern 4: Dynamic configuration
def create_dynamic_config(base_config, modifications):
    """Create a dynamic configuration from base with modifications."""
    config = pynlm.createDefaultConfig()
    
    # Copy base config if provided
    if base_config:
        for key in base_config.getKeys():
            config.set(key, base_config.get(key))
    
    # Apply modifications
    for key, value in modifications.items():
        config.set(key, value)
    
    return config

# Pattern 5: Configuration templates
CONFIG_TEMPLATES = {
    'minimal': {
        'brain.neuron_count': 100,
        'brain.synapse_density': 0.05
    },
    'balanced': {
        'brain.neuron_count': 1000,
        'brain.synapse_density': 0.1,
        'plasticity.stdp.enable': True,
        'plasticity.hebbian.enable': True
    },
    'complex': {
        'brain.neuron_count': 5000,
        'brain.synapse_density': 0.15,
        'brain.working_memory.enable': True,
        'brain.episodic_memory.enable': True,
        'brain.associative_memory.enable': True,
        'prediction.system.enable': True,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True,
        'development.system.enable': True
    }
}

def get_config_template(name):
    """Get a configuration template."""
    if name not in CONFIG_TEMPLATES:
        raise ValueError(f"Unknown template: {name}")
    
    config = pynlm.createDefaultConfig()
    for key, value in CONFIG_TEMPLATES[name].items():
        config.set(key, value)
    
    return config

# Use templates
minimal_config = get_config_template('minimal')
complex_config = get_config_template('complex')
```

## Custom Exceptions

### Exception Hierarchy

```python
import pynlm

# Exception types
exceptions = {
    'InvalidOperationError': pynlm.InvalidOperationError,
    'ConfigurationError': pynlm.ConfigurationError,
    'NetworkError': pynlm.NetworkError,
    'MemoryError': pynlm.MemoryError
}

# Exception handling with context
class SimulationError(Exception):
    """Custom simulation error with context."""
    def __init__(self, message, step=None, component=None):
        super().__init__(message)
        self.step = step
        self.component = component

# Demonstrate custom exceptions
def demonstrate_exceptions():
    try:
        # Simulate invalid operation
        raise pynlm.InvalidOperationError("Cannot process empty percept")
    except pynlm.InvalidOperationError as e:
        print(f"Invalid operation: {e}")
    
    try:
        # Simulate configuration error
        raise pynlm.ConfigurationError("Missing required parameter: brain.neuron_count")
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
    
    try:
        # Simulate memory error
        raise pynlm.MemoryError("Out of memory in working memory system")
    except pynlm.MemoryError as e:
        print(f"Memory error: {e}")

# Exception context helpers
error_context = pynlm.createErrorContext("simulation", "Failed to process step 100")
print(error_context)  # "Operation: simulation - Details: Failed to process step 100"

# Robust error handling
def robust_operation(operation, *args, **kwargs):
    """Execute operation with comprehensive error handling."""
    try:
        return operation(*args, **kwargs)
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        raise
    except pynlm.InvalidOperationError as e:
        print(f"Invalid operation: {e}")
        return None
    except pynlm.MemoryError as e:
        print(f"Memory error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {type(e).__name__}: {e}")
        return None
```