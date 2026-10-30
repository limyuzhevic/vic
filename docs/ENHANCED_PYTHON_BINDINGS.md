# NLM Python Bindings - Enhanced Documentation

This document provides comprehensive documentation for the enhanced NLM Python bindings, including new features, examples, and best practices.

## Overview

The NLM (Neural Learning Machine) Python bindings have been significantly enhanced with:

### Key Improvements

1. **Enhanced Configuration System**
   - Advanced configuration validation and schema support
   - Multiple preset configurations (phase2, phase6, learning, development, structural)
   - Configuration validation with detailed error messages
   - Schema-based configuration management

2. **Improved Python API**
   - Comprehensive error handling with custom exception types
   - Convenience factory methods for brain creation
   - Enhanced utility functions for common tasks
   - Statistical analysis tools
   - Machine learning dataset creation utilities

3. **Advanced Features**
   - Reproducible experiments with fixed seeds
   - Complete experiment automation
   - Neural activity analysis
   - Configuration validation and schema management
   - Dictionary serialization for objects

## Quick Start Guide

### Basic Usage

```python
import pynlm

# Create a default brain configuration
config = pynlm.create_example_config()

# Create a brain with configuration
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation steps
for step in range(100):
    brain.step(step, step * 0.001)

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
```

### Using Presets

```python
# Create configuration with preset
learning_config = pynlm.create_preset_config("learning")
development_config = pynlm.create_preset_config("development")
structural_config = pynlm.create_preset_config("structural")

# Use the configuration
brain = pynlm.create_brain_with_validation(learning_config, "learning")
brain.initialize()
```

### Running Experiments

```python
# Run basic connectivity test
brain = pynlm.create_brain(learning_config)
brain.initialize()
pynlm.run_basic_connectivity_test(brain)

# Run complete demo
brain = pynlm.create_brain(learning_config)
brain.initialize()
pynlm.run_complete_demo(brain)

# Run specific experiment type
brain = pynlm.create_brain(learning_config)
brain.initialize()
pynlm.setup_complete_experiment(brain, "plasticity", 1000)
```

### Learning Experiments

```python
# Create and run a learning experiment
experiment = pynlm.create_and_run_learning_experiment(learning_config, 500)
print(f"Learning experiment completed")
print(f"Initial mean weight: {experiment.computeStatistics()}")

# For advanced usage with LearningExperiment class
brain = pynlm.create_brain(learning_config)
brain.initialize()

learning_exp = pynlm.createLearningExperiment(brain, 42)
learning_exp.recordInitialState()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

learning_exp.recordFinalState()
learning_exp.computeStatistics()
```

### Neural Activity Analysis

```python
# Analyze neural activity over time
brain = pynlm.create_brain(learning_config)
brain.initialize()

stats = pynlm.analyze_neural_activity(brain, 1000)
print(f"Firing rate statistics: {stats['firing_rate']}")
print(f"Overall neurons: {stats['overall']['total_neurons']}")
print(f"Overall synapses: {stats['overall']['total_synapses']}")
```

### Configuration Validation

```python
# Validate configuration
config = pynlm.create_example_config()
errors = pynlm.validate_config(config, "learning")

if errors:
    print("Configuration validation failed:")
    for error in errors:
        print(f"  {error.key}: {error.message} ({error.code})")
else:
    print("Configuration is valid")

# Create brain with validation
brain = pynlm.create_brain_with_validation(config, "learning")
```

### Reproducible Experiments

```python
# Create a reproducible experiment with fixed seed
experiment = pynlm.create_reproducible_experiment("phase2", 12345, 1000)
print(f"Reproducible experiment completed")
print(f"Total spikes: {experiment.brain->getTotalSpikeCount()}")
```

### Advanced Configuration Management

```python
# Access configuration metadata
config = pynlm.create_example_config()
print(f"Available keys: {config.getKeys()}")

for key in config.getKeys():
    print(f"{key}:")
    print(f"  Type: {config.getValueType(key)}")
    print(f"  Description: {config.getDescription(key)}")
    print(f"  Source: {config.getSource(key)}")
    value = config.get(key)
    if value:
        print(f"  Value: {value.value}")
    print()

# Apply configuration overrides
overrides = {"neuron_count": "2000", "learning_rate": "0.05"}
advanced_config = pynlm.create_advanced_config("phase2", overrides)
```

### Serialization

```python
# Convert objects to dictionaries
brain = pynlm.create_brain(learning_config)
brain.initialize()

brain_dict = brain.to_dict()
print(f"Brain dictionary: {brain_dict}")

# Create object from dictionary
new_brain = pynlm.Brain.from_dict(brain_dict)
```

### Statistical Analysis

```python
# Compare two brains
brain1 = pynlm.create_brain(learning_config)
brain2 = pynlm.create_brain(development_config)

comparison = pynlm.compareBrains(brain1, brain2)
print(f"Neuron count difference: {comparison['neuron_count_diff']}")
print(f"Spike count difference: {comparison['spike_count_diff']}")
```

## Advanced Features

### Configuration Validation

The configuration system includes comprehensive validation:

```python
# Create configuration with validation
config = pynlm.create_preset_config("learning")

# Validate configuration
validation_errors = config.validate("learning")
if validation_errors:
    for error in validation_errors:
        print(f"Validation error: {error.message}")

# Check if configuration is valid
if config.isValid("learning"):
    print("Configuration is valid")
else:
    print("Configuration has validation errors")
```

### Schema Management

```python
# Access configuration schemas
config = pynlm.create_example_config()
schemas = config.getSchemaNames()

for schema_name in schemas:
    schema = config.getSchema(schema_name)
    print(f"Schema '{schema_name}':")
    print(f"  Key: {schema.key}")
    print(f"  Type: {schema.type}")
    print(f"  Required: {schema.required}")
    print(f"  Description: {schema.description}")
```

### Exception Handling

```python
import pynlm

try:
    config = pynlm.create_example_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # This will raise an exception if something goes wrong
    print("Brain initialized successfully")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e.what()}")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization error: {e.what()}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e.what()}")
except pynlm.NLMError as e:
    print(f"NLM error: {e.what()}")
```

### Machine Learning Dataset Creation

```python
# Create a training dataset
brain = pynlm.create_brain(learning_config)
dataset = pynlm.create_training_dataset(brain, 10)

print(f"Created dataset with {len(dataset)} episodes")
for i, episode in enumerate(dataset):
    print(f"Episode {i}: {episode}")
```

## Best Practices

### 1. Use Presets Wisely

```python
# Use appropriate presets for different use cases
if you_need_learning:
    config = pynlm.create_preset_config("learning")
elif you_need_development:
    config = pynlm.create_preset_config("development")
elif you_need_structural:
    config = pynlm.create_preset_config("structural")
else:
    config = pynlm.create_preset_config("phase2")
```

### 2. Validate Before Use

```python
# Always validate your configuration before using it
config = pynlm.create_preset_config("learning")
if not config.isValid("learning"):
    errors = config.validate("learning")
    # Handle errors appropriately
    raise ValueError("Invalid configuration")

brain = pynlm.create_brain_with_validation(config, "learning")
```

### 3. Use Reproducible Seeds

```python
# Use fixed seeds for reproducible experiments
experiment = pynlm.create_reproducible_experiment("learning", 42, 1000)
```

### 4. Handle Errors Gracefully

```python
try:
    brain = pynlm.create_brain_with_validation(config, "learning")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e.what()}")
    # Fix configuration and retry
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization error: {e.what()}")
    # Check brain state and try again
```

### 5. Use Statistical Analysis

```python
# Analyze brain activity for insights
brain = pynlm.create_brain(learning_config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Analyze activity
stats = pynlm.analyze_neural_activity(brain, 1000)
print(f"Average firing rate: {stats['firing_rate']['mean']}")
print(f"Firing rate stddev: {stats['firing_rate']['stddev']}")
```

## API Reference

### Core Classes

#### Config
- `create_default_config()` - Create default configuration
- `create_preset_config(preset)` - Create configuration with preset
- `validate_config(config, preset)` - Validate configuration
- `apply_preset(base_config, preset_name)` - Apply preset to configuration

#### Brain
- `create_brain(config)` - Create brain with configuration
- `create_brain_with_validation(config, preset_name)` - Create brain with validation
- `create_preset_brain(preset)` - Create brain with preset configuration

#### AgentBrain
- `create_agent_brain(brain)` - Create agent brain interface
- `initialize(world)` - Initialize with world

#### SimpleWorld
- `create_simple_world()` - Create simple world
- `configure(width, height, vision_width, vision_height)` - Configure world

#### Utility Functions
- `run_basic_connectivity_test(brain)` - Run connectivity test
- `run_complete_demo(brain)` - Run complete demo
- `setup_complete_experiment(brain, experiment_type, steps)` - Setup complete experiment
- `analyze_neural_activity(brain, steps)` - Analyze neural activity
- `create_training_dataset(brain, episodes)` - Create training dataset
- `create_reproducible_experiment(preset, seed, steps)` - Create reproducible experiment

### Exception Types

- `NLMError` - Base exception for all NLM errors
- `ConfigurationError` - Configuration-related errors
- `BrainInitializationError` - Brain initialization errors
- `SimulationError` - Simulation errors

### Module Attributes

- `__version__` - Version of the bindings
- `__description__` - Description of the bindings
- `__available_presets__` - Available preset names
- `__available_types__` - Available configuration types
- `__validation_errors__` - Available validation error codes

## Troubleshooting

### Common Issues

**Issue: Configuration validation fails**
```python
# Solution: Check validation errors
config = pynlm.create_preset_config("learning")
errors = config.validate("learning")
for error in errors:
    print(f"Error: {error.message}")
```

**Issue: Brain initialization fails**
```python
# Solution: Check brain configuration
config = pynlm.create_preset_config("learning")
try:
    brain = pynlm.create_brain_with_validation(config, "learning")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization failed: {e.what()}")
```

**Issue: Simulation runs too slowly**
```python
# Solution: Use smaller configurations
config = pynlm.create_preset_config("small")
brain = pynlm.create_brain(config)
```

**Issue: Neural activity analysis fails**
```python
# Solution: Check brain state before analysis
brain = pynlm.create_brain(learning_config)
brain.initialize()
stats = pynlm.analyze_neural_activity(brain, 100)
```

### Getting Help

For more information and examples:

1. **Documentation**: See this document for comprehensive examples
2. **Source Code**: Examine the Python bindings source code
3. **Repository**: Visit the NLM repository for more examples
4. **Community**: Join the NLM community for support

## Conclusion

The enhanced NLM Python bindings provide a comprehensive set of tools for neural network simulation and analysis. With improved configuration management, error handling, and convenience functions, users can quickly set up and run complex neural experiments.

Key benefits:

- **Simplified setup** with preset configurations
- **Robust error handling** with detailed error messages
- **Advanced analysis tools** for neural activity
- **Reproducible experiments** with fixed seeds
- **Comprehensive documentation** with examples
- **Flexible configuration** with validation

These enhancements make the NLM Python bindings more accessible to users of all levels, from beginners to advanced researchers, while providing powerful tools for complex neural simulation tasks.

## References

- [NLM Project Documentation](https://github.com/nlm-project/nlm)
- [NLM Python Bindings](https://pypi.org/project/pynlm/)
- [NLM Scientific Papers](https://scholar.google.com)

---

*Last updated: October 30, 2026*
*NLM Project Version 2.0.0-enhanced*