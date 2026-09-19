# Enhanced NLM Configuration System - Examples

This module provides comprehensive examples for using the enhanced NLM configuration system.

## Quick Start

```python
from pynlm_enhanced import NLMConfig

# Create enhanced configuration
config = NLMConfig()

# Set basic values
config['brain.neuronCount'] = 1000
config.set('brain.simulationSpeed', 1.5)
config['plasticity.enabled'] = True

print(config)  # Get summary
```

## Basic Configuration Management

```python
from pynlm_enhanced import NLMConfig

# Create configuration
config = NLMConfig()

# Use dictionary-style access
config['brain.neuronCount'] = 1000
config['brain.simulationSpeed'] = 1.5
config['plasticity.enabled'] = True

# Get values
neuron_count = config['brain.neuronCount']
speed = config.get('brain.simulationSpeed', 1.0)  # With default

# Check existence
if 'brain.neuronCount' in config:
    print(f"Neuron count: {config['brain.neuronCount']}")

# Get all keys
for key in config.keys():
    print(f"{key}: {config[key]}")

# Batch operations
keys = ['brain.neuronCount', 'brain.simulationSpeed']
values = config.batch_get(keys)
```

## Configuration Categories

```python
from pynlm_enhanced import NLMConfig, ConfigCategory

config = NLMConfig()

# Set with category
config.set('brain.neuronCount', 1000, category='Brain')
config.set('sensory.vision.width', 640, category='Sensory')
config.set('plasticity.learningRate', 0.01, category='Plasticity')

# Get by category
brain_configs = config.get_by_category('Brain')
all_categories = config.get_categories()

# View all configs by category
configs_by_cat = config.getConfigByCategory()
for category, keys in configs_by_cat.items():
    print(f"{category}: {keys}")
```

## Configuration Builder Pattern

```python
from pynlm_enhanced import NLMConfig

# Create configuration using builder pattern
config = (NLMConfig()
    .createBuilder()
    .set("brain.neuronCount", 1000)
    .set("brain.simulationSpeed", 1.5)
    .set("sensory.vision.enabled", True)
    .set("sensory.audio.enabled", True)
    .set("plasticity.enabled", True)
    .set("development.enabled", True)
    .validate()
    .build())

print(config.summary())
```

## Configuration Templates

```python
from pynlm_enhanced import NLMConfig, ConfigurationTemplate

# Create template
template_config = {
    "brain.neuronCount": 1000,
    "brain.simulationSpeed": "${SPEED}",
    "sensory.vision.enabled": True,
    "sensory.audio.enabled": True,
    "plasticity.enabled": True
}

template = ConfigurationTemplate("research", template_config)

# Render template with variables
variables = {"SPEED": 0.5}
rendered_config = template.render(variables)

# Use rendered config
print(rendered_config['brain.neuronCount'])  # 1000
print(rendered_config['brain.simulationSpeed'])  # 0.5
```

## Validation System

```python
from pynlm_enhanced import NLMConfig, validate_int_range, validate_positive

config = NLMConfig()

# Setup validation
config.enableValidation(True)
config.addValidator("brain.neuronCount", validate_int_range(100, 10000), "Neuron count must be between 100 and 10000")
config.addValidator("brain.simulationSpeed", validate_positive, "Simulation speed must be positive")
config.addValidator("sensory.vision.width", validate_int_range(320, 1920), "Width must be between 320 and 1920")

# Set values (will validate)
config['brain.neuronCount'] = 1000  # OK
config['brain.neuronCount'] = 50   # Will raise exception
try:
    config['brain.neuronCount'] = 50
except Exception as e:
    print(f"Validation error: {e}")

# Validate all
all_valid = config.validateAll()
print(f"All valid: {all_valid}")
```

## Configuration Presets

```python
from pynlm_enhanced import NLMConfig

config = NLMConfig()

# Load built-in presets
config.loadPreset("minimal")
print("Loaded minimal preset:")
print(config.summary())

# Reset and load another preset
config.loadPreset("high_performance")
print("\nLoaded high_performance preset:")
print(config.summary())

# Create and save custom preset
config.savePreset("my_custom_config", "My custom configuration for research")
print("\nCustom preset saved!")

# List all presets
presets = config.listPresets()
print(f"Available presets: {presets}")
```

## Configuration History

```python
from pynlm_enhanced import NLMConfig

config = NLMConfig()

# Enable history tracking
config.enableHistory(True)

# Make some changes
config['brain.neuronCount'] = 1000
config['brain.simulationSpeed'] = 1.5
config['plasticity.enabled'] = True

# Get history
change_history = config.getHistory()
for entry in change_history:
    print(f"{entry.timestamp}: {entry.key} changed from {entry.old_value} to {entry.new_value}")

# Get history for specific key
specific_history = config.getHistory(key='brain.neuronCount')

# Clear history
config.clearHistory()
```

## Export/Import

```python
from pynlm_enhanced import NLMConfig

config = NLMConfig()

# Set up some configuration
config['brain.neuronCount'] = 1000
config['brain.simulationSpeed'] = 1.5
config['sensory.vision.width'] = 640
config['sensory.vision.height'] = 480

# Export to JSON
config.exportToFile("config.json", format="json")

# Import from JSON
new_config = NLMConfig()
new_config.importFromFile("config.json", format="json")

# Verify import
for key in new_config.keys():
    print(f"{key}: {new_config[key]}")

# Export to INI format (simple format)
config.exportToFile("config.ini", format="ini")
```

## Configuration Comparison and Merging

```python
from pynlm_enhanced import NLMConfig

# Create two different configurations
config1 = NLMConfig()
config1['brain.neuronCount'] = 1000
config1['brain.simulationSpeed'] = 1.0
config1['sensory.vision.enabled'] = True

config2 = NLMConfig()
config2['brain.neuronCount'] = 1000
config2['brain.simulationSpeed'] = 2.0
config2['sensory.audio.enabled'] = True
config2['plasticity.enabled'] = True

# Compare configurations
diff = config1.compare(config2)
print("Differences:")
for key, result in diff.items():
    if result == "added":
        print(f"  {key}: Added")
    elif result == "removed":
        print(f"  {key}: Removed")
    elif result == "modified":
        print(f"  {key}: Modified")

# Merge configurations
merged_config = config1.merge(config2, strategy="replace")
print(f"\nMerged configuration has {len(merged_config.keys())} keys")

# Merge with different strategy
merged_union = config1.merge(config2, strategy="merge")
print(f"Merged (union) configuration has {len(merged_union.keys())} keys")
```

## Type Inference

```python
from pynlm_enhanced import auto_detect_config_type

# Test various values
print(f"Type of 42: {auto_detect_config_type(42)}")
print(f"Type of 3.14: {auto_detect_config_type(3.14)}")
print(f"Type of 'hello': {auto_detect_config_type('hello')}")
print(f"Type of True: {auto_detect_config_type(True)}")
print(f"Type of [1, 2, 3]: {auto_detect_config_type([1, 2, 3])}")
print(f"Type of [1.0, 2.5, 3.14]: {auto_detect_config_type([1.0, 2.5, 3.14])}")
print(f"Type of ['a', 'b', 'c']: {auto_detect_config_type(['a', 'b', 'c'])}")
print(f"Type of mixed list: {auto_detect_config_type([1, 'a', True])}")
```

## Global Configuration Registry

```python
from pynlm_enhanced import registerConfig, getConfig, listConfigs

# Create and register configurations
config1 = NLMConfig()
config1['brain.neuronCount'] = 1000
config1['brain.simulationSpeed'] = 1.0

registerConfig("research_config", config1)

# Get registered configuration
loaded_config = getConfig("research_config")
print(f"Loaded config with {len(loaded_config.keys())} keys")

# List all registered configurations
all_configs = listConfigs()
print(f"Available configurations: {all_configs}")
```

## Validation Schema

```python
from pynlm_enhanced import NLMConfig, ValidationSchema

# Create validation schema
schema = ValidationSchema()

# Add rules
schema.addRule("brain.neuronCount", lambda v: v >= 100 and v <= 10000,
              "Neuron count must be between 100 and 10000")
schema.addRule("brain.simulationSpeed", lambda v: v > 0,
              "Simulation speed must be positive")
schema.addRule("sensory.vision.width", lambda v: v % 2 == 0,
              "Vision width must be even")

# Use schema with configuration
config = NLMConfig()

# Validate configuration
all_valid = True
for key in config.keys():
    if not schema.validate(key, config.getRawValue(key)):
        all_valid = False
        print(f"Validation failed for {key}: {schema.getErrorMessage(key)}")

print(f"All valid: {all_valid}")
```

## Advanced Usage Examples

```python
from pynlm_enhanced import NLMConfig, merge_configs, diff_configs

# Example 1: Merge multiple configurations
configs = [
    NLMConfig(),  # Minimal
    NLMConfig()   # Standard
]

configs[0].loadPreset("minimal")
configs[1].loadPreset("research")

merged = merge_configs(*configs, strategy="replace")
print(f"Merged config has {len(merged.keys())} keys")

# Example 2: Complex configuration management
base_config = NLMConfig()
base_config.setupStandard()

# Modify for specific use case
research_config = base_config.createBuilder() \
    .set("brain.neuronCount", 5000) \
    .set("brain.simulationSpeed", 0.5) \
    .set("development.enabled", True) \
    .set("neuromodulation.enabled", True) \
    .set("memory.enabled", True) \
    .build()

# Save as custom preset
research_config.savePreset("research_version_2", "Research configuration with advanced features")

# Example 3: Template-based configuration management
from pynlm_enhanced import ConfigurationTemplate, registerTemplate

# Create template
template_data = {
    "brain.neuronCount": 1000,
    "brain.simulationSpeed": "${SPEED}",
    "sensory.vision.enabled": True,
    "plasticity.enabled": "${PLASTICITY_ENABLED}"
}

registerTemplate("experiment_template", template_data)

# Render for different experiments
experiment1 = getConfigFromTemplate("experiment_template", {"SPEED": 1.0, "PLASTICITY_ENABLED": True})
experiment2 = getConfigFromTemplate("experiment_template", {"SPEED": 0.5, "PLASTICITY_ENABLED": False})

print("Experiment 1 (speed=1.0, plasticity=True):")
print(experiment1.summary())

print("\nExperiment 2 (speed=0.5, plasticity=False):")
print(experiment2.summary())

# Example 4: Configuration validation with batch operations
from pynlm_enhanced import NLMConfig, validateConfig

# Create configuration with batch operations
batch_keys = ["brain.neuronCount", "brain.simulationSpeed", "sensory.vision.width"]
batch_values = [1000, 1.5, 640]

config = NLMConfig()
config.batchSet(batch_keys, batch_values)

# Validate entire configuration
validation_result = validateConfig(config)
print(f"Validation result: {validation_result}")

# Example 5: Configuration migration helper
from pynlm_enhanced import NLMConfig

# Get migration guide
migration_guide = NLMConfig.getMigrationGuide()
print("Migration Guide:")
print(migration_guide)

# Example 6: Factory functions for common configurations
from pynlm_enhanced import getExampleMinimalConfig, getExampleStandardConfig, getExampleHighPerformanceConfig

# Get example configurations
minimal = getExampleMinimalConfig()
standard = getExampleStandardConfig()
high_performance = getExampleHighPerformanceConfig()

print("Minimal config:", minimal.summary())
print("Standard config:", standard.summary())
print("High performance config:", high_performance.summary())

# Example 7: Configuration comparison utilities
from pynlm_enhanced import diff_configs

# Compare configurations using dicts
config1_dict = {"brain.neuronCount": 1000, "brain.simulationSpeed": 1.0}
config2_dict = {"brain.neuronCount": 1000, "brain.simulationSpeed": 2.0, "plasticity.enabled": True}

diff = diff_configs(config1_dict, config2_dict)
print("Dict comparison differences:")
for key, change in diff.items():
    print(f"  {key}: {change}")
```

## Python Integration Examples

```python
from pynlm_enhanced import pynlm

# Access enhanced config through pynlm
config = pynlm.createEnhancedConfig()
config['brain.neuronCount'] = 1000

# Use pybind11's enhanced features
builder = config.createBuilder()
config = builder.build()

# Get configuration category enum
from pynlm_enhanced import ConfigCategory
print("Available categories:")
for category in ConfigCategory:
    print(f"  {category}")

# Configuration source enum
from pynlm_enhanced import ConfigSource
print("\nAvailable sources:")
for source in ConfigSource:
    print(f"  {source}")

# Validation helper functions
from pynlm_enhanced import validate_int_range, validate_positive, validate_string_length

# Use validation helpers
if validate_int_range(500, 100, 1000):
    print("Value 500 is within range 100-1000")

if validate_positive(3.14):
    print("Value 3.14 is positive")

if validate_string_length("hello", 3, 10):
    print("String 'hello' is within length 3-10")

# Create composite validation
from pynlm_enhanced import create_validation_function

is_positive_even = create_validation_function(
    lambda x: x > 0,
    lambda x: x % 2 == 0
)

print(f"\nComposite validation - 4 is positive and even: {is_positive_even(4)}")
print(f"Composite validation - -2 is positive and even: {is_positive_even(-2)}")
print(f"Composite validation - 5 is positive and even: {is_positive_even(5)}")
```

## Error Handling

```python
from pynlm_enhanced import EnhancedConfig, EnhancedRuntimeError, EnhancedInvalidArgumentError

try:
    config = EnhancedConfig()
    
    # Try to set invalid value
    config['brain.neuronCount'] = -1  # This will likely fail validation
    
except EnhancedInvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except EnhancedRuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

This comprehensive examples module demonstrates all the advanced features of the enhanced NLM configuration system, providing clear, practical examples for both beginners and advanced users.
