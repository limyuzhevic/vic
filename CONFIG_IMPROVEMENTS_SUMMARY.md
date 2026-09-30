# Enhanced NLM Python Bindings - Usage Examples

This document provides comprehensive examples for using the enhanced NLM Python bindings.

## Quick Start

```python
from pynlm import Config

# Create a default configuration
config = Config.createDefault()

# Set configuration values
config["learning_rate"] = 0.01
config["batch_size"] = 32
config["model_type"] = "feedforward"

# Get values with type conversion
learning_rate = config.asFloat("learning_rate")
batch_size = config.asInt("batch_size")
model_type = config.asString("model_type")

print(config)
```

## Configuration Creation and Loading

```python
# Create from file
config = Config.fromFile("config.json")

# Create from command line arguments
config = Config.fromArgs(len(sys.argv), sys.argv)

# Create default config and populate
config = Config.createDefault()
config.batchSet({
    "learning_rate": 0.001,
    "batch_size": 64,
    "epochs": 100
}, source="runtime")
```

## Type Conversion Utilities

```python
# Get values as specific types with error handling
try:
    learning_rate = config.asFloat("learning_rate")
    print(f"Learning rate: {learning_rate}")
except KeyError as e:
    print(f"Key not found: {e}")

# Check value types
if config.isInt("batch_size"):
    batch_size = config.asInt("batch_size")
    print(f"Batch size: {batch_size}")

if config.isString("model_type"):
    model_type = config.asString("model_type")
    print(f"Model type: {model_type}")

if config.isVector("weights"):
    weights = config.asFloatVector("weights")
    print(f"Weights length: {len(weights)}")
```

## Validation and Schema Support

```python
# Validate configuration
if config.validate():
    print("Configuration is valid!")
else:
    errors = config.getErrors()
    print(f"Validation errors: {errors}")

# Validate specific key
if config.validateKey("learning_rate"):
    print("Learning rate is valid")

# Add validation schema
from typing import Callable
def positive_float(value: ConfigValue) -> bool:
    return isinstance(value, float) and value > 0

config.addSchema({
    "learning_rate": ConfigValidation("learning_rate", "Learning rate must be positive", 
                                     addRule(positive_float))
})

# Validate with context
context = {"optimizer": "adam"}
if config.validate(context):
    print("Configuration valid with context")
```

## Configuration Comparison and Merging

```python
# Create two configurations
config1 = Config.fromFile("base_config.json")
config2 = Config.fromFile("override_config.json")

# Check if configurations are equal
if config1.equals(config2):
    print("Configurations are identical")

# Merge configurations
config1.merge(config2)
print("Configurations merged")

# Get differences
differences = config1.diff(config2)
for key, (old_val, new_val) in differences.items():
    print(f"{key}: {old_val} -> {new_val}")
```

## Serialization Utilities

```python
# Convert to Python dictionary
config_dict = config.toDict()
print("Configuration as dict:", config_dict)

# Load from Python dictionary
config.fromDict({
    "learning_rate": 0.01,
    "batch_size": 32,
    "model_type": "neural_network"
})

# Convert to JSON
json_str = config.toJson()
with open("config.json", "w") as f:
    f.write(json_str)

# Load from JSON
with open("config.json", "r") as f:
    json_content = f.read()
config.fromJson(json_content)
```

## Python-Style Iteration and Access

```python
# Access by key (supports indexing)
learning_rate = config["learning_rate"]
config["learning_rate"] = 0.001

# Check if key exists
if "learning_rate" in config:
    print("Learning rate is set")

# Get configuration size
print(f"Configuration has {len(config)} entries")

# Iterate over keys
for key in config:
    print(f"Key: {key}, Value: {config[key]}")

# Get key-value pairs
for item in config.items():
    print(f"Key: {item.key}, Value: {item.value}")

# Get all values
values = config.values()
print(f"All values: {values}")
```

## Batch Operations

```python
# Batch set multiple values
config.batchSet({
    "learning_rate": 0.001,
    "batch_size": 64,
    "dropout_rate": 0.2,
    "hidden_size": 128
})

# Batch get multiple values
keys = ["learning_rate", "batch_size", "model_type"]
batch_result = config.batchGet(keys)
print(f"Batch result: {batch_result}")
```

## Context Manager Support

```python
# Use config as context manager
with Config() as config:
    config["learning_rate"] = 0.01
    config["batch_size"] = 32
    print("Training configured")
    # Context automatically cleans up
```

## Configuration Documentation and Help

```python
# Get documentation for specific key
doc = config.getDocumentation("learning_rate")
print(f"Documentation for learning_rate:\n{doc}")

# Get documentation for all keys
all_docs = config.getAllDocumentation()
for key, doc in all_docs.items():
    print(f"=== {key} ===\n{doc}")

# Get help
help_text = config.help()
print(f"Help:\n{help_text}")
```

## Statistics and Diagnostics

```python
# Get configuration statistics
stats = config.getStats()
print(f"Total entries: {stats.totalEntries}")
print(f"Memory usage: {stats.memoryUsage} bytes")
print(f"Type counts: {stats.typeCounts}")
print(f"Average values: {stats.averageValues}")
```

## Exception Handling

```python
from pynlm import ConfigError, ConfigFileError, ConfigValidationError, ConfigTypeError

try:
    # Attempt to load a non-existent file
    config = Config.fromFile("nonexistent.json")
except ConfigFileError as e:
    print(f"File error: {e}")

# Validation error
try:
    if not config.validate():
        errors = config.getErrors()
        raise ConfigValidationError("\n".join(errors))
except ConfigValidationError as e:
    print(f"Validation error: {e}")

# Type error
try:
    learning_rate = config.asInt("learning_rate")
except ConfigTypeError as e:
    print(f"Type error: {e}")
```

## Complete Example: Neural Network Configuration

```python
from pynlm import Config

def create_neural_network_config(
    learning_rate: float = 0.001,
    batch_size: int = 32,
    hidden_layers: list = None,
    dropout_rate: float = 0.1
) -> Config:
    """Create a neural network configuration with validation."""
    if hidden_layers is None:
        hidden_layers = [128, 64, 32]
    
    # Create configuration
    config = Config.createDefault()
    
    # Add validation rules
    def positive_float(value):
        return isinstance(value, float) and value > 0
    
    def positive_int(value):
        return isinstance(value, int) and value > 0
    
    def non_empty_list(value):
        return isinstance(value, list) and len(value) > 0
    
    config.addSchema({
        "learning_rate": ConfigValidation("learning_rate", "Learning rate must be positive",
                                         addRule(positive_float)),
        "batch_size": ConfigValidation("batch_size", "Batch size must be positive",
                                       addRule(positive_int)),
        "dropout_rate": ConfigValidation("dropout_rate", "Dropout rate must be between 0 and 1",
                                         addRule(lambda v: isinstance(v, float) and 0 <= v <= 1)),
        "hidden_layers": ConfigValidation("hidden_layers", "Hidden layers must be a non-empty list",
                                          addRule(non_empty_list))
    })
    
    # Set values
    config.batchSet({
        "learning_rate": learning_rate,
        "batch_size": batch_size,
        "dropout_rate": dropout_rate,
        "hidden_layers": hidden_layers,
        "model_type": "feedforward",
        "activation": "relu",
        "optimizer": "adam",
        "loss_function": "crossentropy"
    })
    
    # Validate
    if not config.validate():
        errors = config.getErrors()
        raise ValueError(f"Configuration validation failed: {errors}")
    
    return config

# Usage
if __name__ == "__main__":
    config = create_neural_network_config(
        learning_rate=0.01,
        batch_size=64,
        hidden_layers=[256, 128, 64],
        dropout_rate=0.2
    )
    
    print("Neural Network Configuration:")
    print(config)
    
    # Save to file
    config.saveToFile("nn_config.txt")
    
    # Convert to dictionary for JSON serialization
    import json
    config_dict = config.toDict()
    with open("nn_config.json", "w") as f:
        json.dump(config_dict, f, indent=2)
```

## Advanced Usage: Configuration Templates

```python
from pynlm import Config

class ConfigurationTemplate:
    """Configuration templates for common use cases."""
    
    @staticmethod
    def create_default() -> Config:
        """Create a default configuration."""
        config = Config.createDefault()
        config.batchSet({
            "learning_rate": 0.001,
            "batch_size": 32,
            "epochs": 100,
            "model_type": "feedforward",
            "activation": "relu",
            "optimizer": "adam",
            "loss_function": "crossentropy",
            "dropout_rate": 0.1
        })
        return config
    
    @staticmethod
    def create_cnn(width: int = 224, height: int = 224, channels: int = 3) -> Config:
        """Create a CNN configuration."""
        config = Config.createDefault()
        config.batchSet({
            "model_type": "cnn",
            "input_width": width,
            "input_height": height,
            "input_channels": channels,
            "conv_layers": [32, 64, 128],
            "pool_size": 2,
            "kernel_size": 3,
            "batch_size": 16,
            "epochs": 50
        })
        return config
    
    @staticmethod
    def create_rnn(sequence_length: int = 50, hidden_size: int = 128) -> Config:
        """Create an RNN configuration."""
        config = Config.createDefault()
        config.batchSet({
            "model_type": "rnn",
            "sequence_length": sequence_length,
            "hidden_size": hidden_size,
            "num_layers": 2,
            "batch_size": 32,
            "epochs": 80,
            "dropout_rate": 0.2
        })
        return config

# Usage
# Default config
config1 = ConfigurationTemplate.create_default()

# CNN config
config2 = ConfigurationTemplate.create_cnn(256, 256, 1)

# RNN config
config3 = ConfigurationTemplate.create_rnn(100, 256)
```

## Best Practices

1. **Use context managers** for resource management
2. **Validate configurations** before use
3. **Use type conversion utilities** for safe type handling
4. **Implement proper exception handling** for user-friendly error messages
5. **Use batch operations** for multiple configuration changes
6. **Leverage configuration templates** for common use cases
7. **Document your configurations** with comments and help text
8. **Use validation schemas** for complex validation rules
9. **Implement proper serialization** for configuration persistence
10. **Use iteration and access patterns** for Pythonic code

These examples demonstrate the enhanced Python API for NLM, providing powerful configuration management capabilities with full Pythonic idioms and comprehensive error handling.
