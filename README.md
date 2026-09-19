# NLM Configuration Manager

A comprehensive configuration management system for the NLM (Neural Learning Machine) project.

## Overview

The NLM Configuration Manager provides advanced configuration management capabilities including:

- **Configuration validation** with schema checking
- **Configuration groups** and hierarchical organization
- **Configuration inheritance** and merging
- **Configuration serialization/deserialization** (JSON, TOML, YAML)
- **Configuration monitoring** and change detection
- **Configuration profiles** and presets
- **Configuration for advanced users** (validation levels, logging, debugging)
- **Configuration commands** and scripting support

## Key Features

### Validation Levels

- **None**: No validation performed
- **Basic**: Basic type checking and required field validation
- **Strict**: Strict type checking, range validation, and pattern matching
- **Schema**: JSON Schema validation with jsonschema package

### Configuration Groups

Organize configuration values into logical groups:
- Create and manage configuration groups
- Organize entries by functionality
- Filter and query by group

### Configuration Profiles

Create reusable configuration profiles:
- Define default values and overrides
- Apply profiles with a single command
- Support for different environments (development, testing, production)

### Configuration Inheritance and Merging

- Set parent-child configuration relationships
- Merge multiple configuration managers
- Control inheritance strategy

### Serialization Support

Save and load configurations in multiple formats:
- JSON (native format)
- YAML (PyYAML required)
- TOML (tomli required)

### Configuration Monitoring

Monitor configuration changes:
- Track changes with callbacks
- Detect value modifications
- Support for real-time monitoring

### Advanced User Features

- Detailed validation rules
- Source tracking (file, command-line, runtime, etc.)
- Logging and debugging support
- Configuration commands for scripting

## Quick Start

### Installation

```bash
pip install -e .
```

### Basic Usage

```python
from nlm import ConfigManager, ValidationLevel

# Create a configuration manager
config = ConfigManager(validation_level=ValidationLevel.STRICT)

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("world.width", 20)

# Get configuration values
print(f"Neurons: {config.get('brain.neuron_count')}")
print(f"World width: {config.get('world.width')}")

# Validate configuration
errors = config.get_validation_errors()
if errors:
    print(f"Validation errors: {errors}")

# Save configuration
config.save_to_file("config.json")

# Load configuration
config2 = ConfigManager()
config2.load_from_file("config.json")
```

### Using Profiles

```python
from nlm import ConfigManager, ValidationLevel, ConfigProfile

# Create a production profile
production = ConfigProfile("production")
production.add_default("brain.neuron_count", 5000)
production.add_default("world.width", 50)
production.with_validation(ValidationLevel.STRICT)

# Create config and apply profile
config = ConfigManager()
config.add_profile("production", production)
config.apply_profile("production")
```

### Configuration Groups

```python
from nlm import ConfigManager, ConfigGroup, ConfigEntry, ConfigSource

# Create a group
brain_group = ConfigGroup("brain", "Brain configuration group")
brain_group.add_entry(
    ConfigEntry("brain.neuron_count", 1000,
               ConfigSource.DEFAULT,
               "Number of neurons",
               "integer",
               True,  # required
               groups=["brain"])
)

# Create config manager and add groups
config = ConfigManager()
config.add_group(brain_group)

# Set value
config.set("brain.neuron_count", 2000)

# Get configuration by group
brain_config = config.get_by_group("brain")
```

## Configuration Validation

The ConfigManager supports multiple validation levels:

### Schema Validation

Define JSON schemas for configuration validation:

```python
schema = {
    "type": "object",
    "properties": {
        "name": {
            "type": "string",
            "minLength": 3,
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

config = ConfigManager(validation_level=ValidationLevel.SCHEMA)
config.set_schema("user", schema)
config.set("name", "Test User")
config.set("age", 25)
```

### Validation Rules

Configuration values can have custom validation rules:

```python
config.set(
    "test.value",
    42,
    description="A test value",
    type_hint="integer",
    required=True,
    validation_rules={
        "minimum": 0,
        "maximum": 100,
        "validator": lambda x: x % 2 == 0  # Must be even
    },
    groups=["test"]
)
```

## Configuration Groups

Configuration groups provide organization and filtering:

### Group Operations

- `add_group(group)`: Add a configuration group
- `get_group(name)`: Get a group by name
- `get_by_group(name)`: Get configuration values by group
- `get_keys(group)`: Get configuration keys filtered by group

### Group Hierarchy

Groups can have subgroups for complex organization:

```python
parent_group = ConfigGroup("parent", "Parent group")
child_group = ConfigGroup("child", "Child group")
parent_group.add_subgroup(child_group)
```

## Configuration Profiles

Profiles are reusable configuration sets:

### Profile Features

- **Defaults**: Default values applied when profile is applied
- **Overrides**: Values that override defaults
- **Validation**: Validation level for the profile
- **Schema**: Schema definition for the profile

### Profile Lifecycle

1. Create profile with `ConfigProfile(name, description)`
2. Add defaults with `add_default(key, value, ...)`
3. Add overrides with `add_override(key, value)`
4. Set validation level with `with_validation(level)`
5. Set schema with `with_schema(schema_dict)`
6. Add profile to config manager with `add_profile(profile)`
7. Apply profile with `apply_profile(name)`

## Configuration Serialization

Save and load configurations in multiple formats:

### Save Configuration

```python
# Save as JSON
config.save_to_file("config.json", "json")

# Save as YAML (requires PyYAML)
config.save_to_file("config.yaml", "yaml")

# Save as TOML (requires tomli)
config.save_to_file("config.toml", "toml")
```

### Load Configuration

```python
# Load from JSON
config = ConfigManager()
config.load_from_file("config.json", "json")

# Load from YAML
config.load_from_file("config.yaml", "yaml")

# Load from TOML
config.load_from_file("config.toml", "toml")
```

### Export and Import

```python
# Export to dictionary
exported = config.export_config(["brain.neuron_count", "world.width"])

# Import from dictionary
config2 = ConfigManager()
config2.import_config(exported)
```

## Configuration Monitoring

Monitor configuration changes in real-time:

### Monitoring Features

- **Change Detection**: Detect when values change
- **Callbacks**: Register functions to handle changes
- **Snapshots**: Track configuration state over time
- **Monitoring**: Enable/disable real-time monitoring

### Usage

```python
from nlm import ConfigManager

config = ConfigManager(enable_monitoring=True)

def on_change(change):
    print(f"Configuration changed: {change.key} = {change.new_value}")

config.add_change_callback(on_change)

# Set values (triggers callbacks)
config.set("test.value", "initial")
config.set("test.value", "updated")  # Triggers callback
```

## Configuration Inheritance and Merging

### Inheritance

Set up parent-child configuration relationships:

```python
# Create parent config
parent = ConfigManager()
parent.set("brain.neuron_count", 1000)

# Create child config
child = ConfigManager()
child.set_parent(parent)
child.inherit_from_parents()

# Child can access parent's values
print(child.get("brain.neuron_count"))  # 1000
```

### Merging

Merge multiple configuration managers:

```python
config1 = ConfigManager()
config1.set("brain.neuron_count", 1000)

config2 = ConfigManager()
config2.set("world.width", 20)

# Merge config2 into config1
config1.merge_config(config2, strategy="overwrite")

# Now config1 has both values
print(config1.get("brain.neuron_count"))  # 1000
print(config1.get("world.width"))        # 20
```

## Advanced Configuration Management

### Configuration Commands

The ConfigManager provides a command-line interface for configuration management:

```bash
# View configuration
nlm-config view

# Set configuration
nlm-config set brain.neuron_count 1000

# Load from file
nlm-config load config.json

# Save to file
nlm-config save config.json

# Apply profile
nlm-config apply production
```

### Configuration Logging

Enable detailed logging for debugging:

```python
from nlm import ConfigManager, ValidationLevel

config = ConfigManager(validation_level=ValidationLevel.STRICT, log_changes=True)

# Configuration operations are logged
config.set("test.value", 42)
config.load_from_file("config.json")
```

### Configuration Scripts

Automate configuration tasks:

```python
# Script to generate configuration from templates
script = """
from nlm import ConfigManager

config = ConfigManager()

# Load template
template = config.load_from_file("template.json")

# Override with command-line arguments
import sys
if len(sys.argv) > 1:
    config.set("test.value", int(sys.argv[1]))

# Save configuration
config.save_to_file("generated_config.json")
"""
```

## Error Handling

### Configuration Errors

The ConfigManager raises exceptions for various error conditions:

- **KeyError**: Configuration key not found
- **ValueError**: Validation failed
- **FileNotFoundError**: File not found when loading
- **ImportError**: Required packages not installed

### Validation Errors

Use `get_validation_errors()` to get all validation errors:

```python
config = ConfigManager(validation_level=ValidationLevel.STRICT)
config.set("test.value", "invalid")  # Would raise ValueError

errors = config.get_validation_errors()
for error in errors:
    print(error)
```

## API Reference

### Classes

- **ConfigManager**: Main configuration manager class
- **ConfigProfile**: Configuration profile class
- **ConfigGroup**: Configuration group class
- **ConfigMonitor**: Configuration monitoring class
- **ConfigSchema**: Configuration schema class
- **ConfigEntry**: Configuration entry class
- **ConfigChange**: Configuration change event class

### Enumerations

- **ValidationLevel**: Configuration validation levels
- **ConfigSource**: Configuration source types

### Functions

- **create_default_config()**: Create default configuration
- **create_test_config()**: Create test configuration
- **create_development_config()**: Create development configuration

## Testing

Run the tests to verify the functionality:

```bash
python test_config_manager.py
```

The tests cover:
- Basic configuration operations
- Configuration validation
- Profile management
- Group operations
- Serialization
- Monitoring
- Schema validation
- Copying and merging
- Export and import

## Dependencies

The NLM Configuration Manager requires the following packages:

- Python 3.8+
- PyYAML (optional, for YAML support)
- tomli (optional, for TOML support)
- jsonschema (optional, for schema validation)

Install with:

```bash
pip install nlm
# or with optional dependencies
pip install nlm[yaml,toml,schema]
```

## Installation

### From Source

```bash
pip install -e .
```

### From PyPI

```bash
pip install nlm
```

### Development Installation

```bash
pip install -e .[dev]
```

## License

MIT License

## Author

NLM Team

## Version

0.1.0
