# NLM Configuration Manager - Quick Start Guide

This guide provides quick instructions for getting started with the NLM Configuration Manager.

## Installation

### Quick Installation

```bash
pip install nlm
```

### Development Installation

```bash
pip install -e .
```

## Quick Start Examples

### 1. Basic Configuration

```python
from nlm import ConfigManager

# Create a configuration manager
config = ConfigManager()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("world.width", 20)
config.set("agent.enabled", True)

# Get configuration values
print(f"Neurons: {config.get('brain.neuron_count')}")
print(f"World width: {config.get('world.width')}")
print(f"Agent enabled: {config.get('agent.enabled')}")
```

### 2. Configuration with Validation

```python
from nlm import ConfigManager, ValidationLevel

# Create config with strict validation
config = ConfigManager(validation_level=ValidationLevel.STRICT)

# Set values (invalid values would raise ValueError)
config.set("test.string", "valid string")
config.set("test.number", 42)
config.set("test.boolean", True)

# Check for validation errors
errors = config.get_validation_errors()
if errors:
    print(f"Validation errors: {errors}")
```

### 3. Configuration Groups

```python
from nlm import ConfigManager, ConfigGroup, ConfigEntry, ConfigSource

# Create configuration groups
brain_group = ConfigGroup("brain", "Brain configuration group")
brain_group.add_entry(
    ConfigEntry("brain.neuron_count", 1000,
               ConfigSource.DEFAULT,
               "Number of neurons",
               "integer",
               True,
               groups=["brain"])
)

world_group = ConfigGroup("world", "World configuration group")
world_group.add_entry(
    ConfigEntry("world.width", 20,
               ConfigSource.DEFAULT,
               "World width",
               "integer",
               True,
               groups=["world"])
)

# Create config manager and add groups
config = ConfigManager()
config.add_group(brain_group)
config.add_group(world_group)

# Set values
config.set("brain.neuron_count", 2000)
config.set("world.width", 30)

# Get configuration by group
brain_config = config.get_by_group("brain")
world_config = config.get_by_group("world")
print(f"Brain config: {brain_config}")
print(f"World config: {world_config}")
```

### 4. Configuration Profiles

```python
from nlm import ConfigManager, ValidationLevel, ConfigProfile

# Create a production profile
production = ConfigProfile("production", "Production configuration")
production.add_default("brain.neuron_count", 5000,
                     description="Production neuron count",
                     type_hint="integer",
                     groups=["brain"])
production.add_default("world.width", 50,
                     description="Production world width",
                     type_hint="integer",
                     groups=["world"])
production.add_override("agent.debug_mode", False)
production.with_validation(ValidationLevel.STRICT)

# Create config and apply profile
config = ConfigManager()
config.add_profile("production", production)
config.apply_profile("production")

print(f"Production neuron count: {config.get('brain.neuron_count')}")
print(f"Production world width: {config.get('world.width')}")
```

### 5. Configuration Serialization

```python
from nlm import ConfigManager
import json

# Create config with values
config = ConfigManager()
config.set("brain.neuron_count", 1000)
config.set("world.width", 20)

# Save to JSON
config.save_to_file("config.json")
print("Saved to config.json")

# Load from JSON
config2 = ConfigManager()
config2.load_from_file("config.json")
print(f"Loaded config: {config2.get_all()}")

# Export to dictionary
exported = config.export_config(["brain.neuron_count", "world.width"])
print(f"Exported: {exported}")

# Load from dictionary
config3 = ConfigManager()
config3.import_config(exported)
print(f"Imported: {config3.get_all()}")
```

### 6. Configuration Monitoring

```python
from nlm import ConfigManager

# Create config with monitoring enabled
config = ConfigManager(enable_monitoring=True)

# Track changes with a callback
change_count = 0

def change_callback(change):
    global change_count
    change_count += 1
    print(f"Change #{change_count}: {change.key} = {change.old_value} -> {change.new_value}")

config.add_change_callback(change_callback)

# Set values (triggers callbacks)
config.set("test.value1", "initial")
config.set("test.value2", 100)
config.set("test.value1", "updated")  # Triggers callback

print(f"Total changes detected: {change_count}")
```

### 7. Configuration Inheritance and Merging

```python
from nlm import ConfigManager

# Create parent config
parent = ConfigManager()
parent.set("brain.neuron_count", 1000)
parent.set("world.width", 20)

# Create child config
child = ConfigManager()
child.set_parent(parent)
child.inherit_from_parents()

# Child can access parent's values
print(f"Child neuron count: {child.get('brain.neuron_count')}")  # 1000
print(f"Child world width: {child.get('world.width')}")        # 20

# Override parent's value
child.set("brain.neuron_count", 2000)
print(f"Child overridden neuron count: {child.get('brain.neuron_count')}")  # 2000

# Merge configurations
config1 = ConfigManager()
config1.set("brain.neuron_count", 1000)

config2 = ConfigManager()
config2.set("world.width", 20)

config1.merge_config(config2, strategy="overwrite")

print(f"Merged config includes both values")
```

### 8. Configuration Validation

```python
from nlm import ConfigManager, ValidationLevel

# Define a schema
schema = {
    "type": "object",
    "properties": {
        "name": {
            "type": "string",
            "minLength": 3,
            "maxLength": 50,
            "required": True
        },
        "age": {
            "type": "integer",
            "minimum": 0,
            "maximum": 150
        }
    },
    "additionalProperties": False
}

# Create config with schema validation
config = ConfigManager(validation_level=ValidationLevel.SCHEMA)
config.set_schema("user", schema)

# Set valid values
config.set("name", "Test User")
config.set("age", 25)

# Set invalid value (would raise ValueError in strict mode)
try:
    config.set("age", -5)
except ValueError as e:
    print(f"Validation error: {e}")
```

## Command Line Usage

The NLM Configuration Manager provides a command-line interface:

### Available Commands

```bash
# View configuration
nlm-config view

# Set configuration
nlm-config set brain.neuron_count 1000

# Get configuration
nlm-config get brain.neuron_count

# Load from file
nlm-config load config.json

# Save to file
nlm-config save config.json

# Apply profile
nlm-config apply production

# List profiles
nlm-config list-profiles

# Remove profile
nlm-config remove-profile production
```

### Examples

```bash
# Create a configuration file
nlm-config set brain.neuron_count 1000
nlm-config set world.width 20
nlm-config set agent.enabled true

# Save to file
nlm-config save config.json

# Load from file
nlm-config load config.json

# Apply production profile
nlm-config apply production
```

## Testing

### Run Tests

```bash
python test_config_manager.py
```

### Test Categories

- **Basic Configuration**: Test basic configuration operations
- **Validation**: Test configuration validation
- **Profiles**: Test configuration profiles
- **Groups**: Test configuration groups
- **Serialization**: Test configuration serialization
- **Monitoring**: Test configuration monitoring
- **Schema Validation**: Test schema validation
- **Copy and Merge**: Test copying and merging
- **Export Import**: Test export and import

## Common Issues and Solutions

### Issue: "ModuleNotFoundError: No module named 'nlm'"

**Solution**: Install the nlm module:
```bash
pip install -e .
```

### Issue: Validation Errors

**Solution**: Check for validation errors:
```python
config = ConfigManager(validation_level=ValidationLevel.STRICT)
errors = config.get_validation_errors()
if errors:
    print(f"Validation errors: {errors}")
```

### Issue: YAML or TOML Support

**Solution**: Install optional dependencies:
```bash
pip install PyYAML
pip install tomli
pip install jsonschema
```

### Issue: Configuration Not Saving

**Solution**: Check file permissions:
```bash
chmod 644 config.json
```

## Frequently Asked Questions

### Q: What validation levels are available?

A: The available validation levels are:
- `NONE`: No validation
- `BASIC`: Basic type checking and required field validation
- `STRICT`: Strict type checking, range validation, and pattern matching
- `SCHEMA`: JSON Schema validation

### Q: Can I use multiple profiles?

A: Yes, you can apply multiple profiles to a configuration manager. The last applied profile's settings will take precedence.

### Q: How do I get configuration values by group?

A: Use the `get_by_group()` method:
```python
brain_config = config.get_by_group("brain")
```

### Q: Can I monitor configuration changes?

A: Yes, enable monitoring with `enable_monitoring=True` and add callbacks with `add_change_callback()`.

### Q: What file formats are supported?

A: The ConfigManager supports JSON, YAML, and TOML formats.

## References

### Documentation

- README.md: Main documentation
- docs/IMPLEMENTATION.md: Implementation details
- docs/ARCHITECTURE.md: System architecture

### Code Examples

- examples_advanced.py: Advanced usage examples
- test_config_manager.py: Comprehensive tests

### API Reference

- nlm.config.ConfigManager: Main configuration manager
- nlm.config.ConfigProfile: Configuration profiles
- nlm.config.ConfigGroup: Configuration groups
- nlm.config.ValidationLevel: Validation levels
- nlm.config.ConfigSource: Configuration sources

## Support

For issues, please refer to the NLM project documentation or GitHub issues.

## License

MIT License

## Version

0.1.0
