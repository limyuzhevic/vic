# NLM (Neural Learning Machine) - Enhanced Python Bindings

**Version:** 2.0.0-enhanced  
**License:** MIT  
**Author:** NLM Research Project  

## Overview

NLM (Neural Learning Machine) is an experimental computational brain project that implements a neural system beginning in a primitive developmental state and acquiring increasingly complex abilities through interaction with an environment.

This enhanced version provides a Pythonic interface to the C++ neural simulation framework with:

- **Pythonic APIs**: Intuitive method names and parameter handling
- **Context Managers**: Automatic resource management and simulation helpers  
- **Enhanced Configuration**: JSON schema validation with presets
- **Advanced Simulation Tools**: Batch running, episode management, and statistical analysis
- **Comprehensive Documentation**: Examples, API reference, and tutorials

## Key Features

### 🚀 Enhanced Python APIs

- **Context Managers**: `pynlm.agent_simulation()` and `pynlm.world_simulation()` for automatic resource management
- **Pythonic Method Chaining**: Fluent interface for sequential operations
- **Enhanced Configuration**: `pynlm.EnhancedConfig` with JSON schema validation
- **Simulation Utilities**: `pynlm.SimulationUtils` for batch processing and statistical analysis

### 🛠️ Advanced Configuration System

- **JSON Schema Validation**: Comprehensive schema validation with error messages
- **Presets**: Three preset configurations (`basic`, `research`, `real_time`)
- **Configuration Validation**: Automatic validation with detailed error reporting
- **Configuration Diffing**: Compare and merge configurations easily

### 🧪 Advanced Simulation Tools

- **Batch Running**: Run multiple episodes for statistical analysis
- **Context Managers**: Resource management and simulation helpers
- **Progress Tracking**: Built-in progress reporting and checkpointing
- **Statistics Collection**: Comprehensive simulation statistics

### 📚 Comprehensive Documentation

- **Examples**: 8 complete example scripts covering all features
- **API Reference**: Comprehensive documentation of all classes and methods
- **Tutorials**: Step-by-step guides for beginners and advanced users
- **Best Practices**: Guidelines for effective usage

## Quick Start

### Installation

The enhanced Python bindings come pre-installed with the NLM framework. You can use them directly:

```bash
# Run examples
python examples.py

# Import and use in your own code
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)
```

### Basic Usage

```python
import pynlm

# Create brain with enhanced configuration
with pynlm.agent_simulation(preset="research") as sim:
    # Run simulation
    sim.runSteps(100)
    
    # Get statistics
    stats = sim.getStatistics()
    print(f"Neurons: {stats['neurons']}")
    print(f"Firing neurons: {stats['firing_neurons']}")
    
    # Format statistics
    print(sim.formatStatistics("table"))
```

### Advanced Usage

```python
import pynlm

# Create enhanced configuration
config = pynlm.EnhancedConfig.createFromPreset("research")

# Validate configuration
if config.isValid():
    print("Configuration is valid!")
else:
    print("Configuration errors:")
    for error in config.getValidationErrors():
        print(f"  - {error}")

# Create brain and run simulation
brain = pynlm.createBrain(config.getConfig())
brain.initialize()

# Run using context manager
with pynlm.agent_simulation("advanced", brain, world, agent) as sim:
    sim.runSteps(100).runSteps(100).runSteps(100)
```

## Key Classes and Methods

### Core Classes

#### `pynlm.createBrain(config)`
Creates a new brain with the specified configuration.

#### `pynlm.createSimpleWorld()`
Creates a new simple 2D world for NLM simulation.

#### `pynlm.createAgentBrain(brain)`
Creates a new agent brain interface connecting NLM brain to world.

#### `pynlm.createDefaultConfig()`
Creates a default configuration.

### Enhanced Configuration

#### `pynlm.EnhancedConfig.createFromPreset(preset_name)`
Creates configuration from preset (`basic`, `research`, `real_time`).

#### `pynlm.EnhancedConfig.createFromJSON(json_str)`
Creates configuration from JSON string.

#### `pynlm.EnhancedConfig.createFromFile(filepath)`
Creates configuration from file.

#### `pynlm.ConfigValidator.loadSchemaFromFile(filepath)`
Loads schema from JSON file.

#### `pynlm.ConfigValidator.loadSchemaFromString(json_str)`
Loads schema from JSON string.

### Context Managers

#### `pynlm.agentSimulation(preset, brain, world, agent)`
Creates an enhanced agent simulation context.

#### `pynlm.agentSimulation(preset)`
Creates an enhanced agent simulation context with preset configuration.

#### `pynlm.worldSimulation(world)`
Creates a world simulation context.

### Simulation Utilities

#### `pynlm.SimulationUtils.runAgentEpisode(max_steps, preset)`
Runs a complete agent simulation episode.

#### `pynlm.SimulationUtils.batchRunEpisodes(num_episodes, steps_per_episode, preset)`
Runs multiple episodes for statistical analysis.

#### `pynlm.SimulationUtils.createAgentSetup(preset)`
Creates a complete agent simulation setup.

### Validation Utilities

#### `pynlm.validateConfig(config)`
Validates configuration (raises exception if invalid).

#### `pynlm.getConfigValidationErrors(config)`
Gets configuration validation errors.

## Examples

The `examples.py` file contains comprehensive examples covering all features:

### Example 1: Basic Brain Simulation
Shows the simplest possible example of creating and running a brain.

### Example 2: Brain in a Simple World
Demonstrates creating a complete agent simulation with a world.

### Example 3: Advanced Configuration
Shows the enhanced configuration system with presets and validation.

### Example 4: Batch Simulation
Demonstrates running multiple simulations for statistical analysis.

### Example 5: Context Manager Usage
Shows the enhanced context managers for resource management.

### Example 6: Configuration Validation
Demonstrates the configuration validation system.

### Example 7: Complete Agent Example
A comprehensive agent simulation example using all major features.

### Example 8: Basic Import and Usage
Shows the basic import and usage patterns.

To run all examples:

```bash
python examples.py
```

To run a specific example:

```bash
python examples.py --example 1  # Example 1: Basic Brain Simulation
```

## Configuration Presets

The enhanced configuration system provides three presets:

### Basic Preset
- Minimal configuration for quick testing
- 500 neurons, 1 region
- 0.1 connection probability
- 0.001 simulation timestep

### Research Preset
- Comprehensive configuration for scientific experiments
- 2000 neurons, 2 regions
- 0.05 connection probability
- 0.0005 simulation timestep
- Full development and neuromodulation support

### Real-time Preset
- Optimized for real-time applications
- 100 neurons, 1 region
- 0.2 connection probability
- 0.01 simulation timestep
- Minimal logging

## Configuration Schema

The configuration system uses JSON schema validation. The schema is available in:

- `configs/nlm_schema.json` - Complete JSON schema
- `docs/CONFIGURATION.md` - Detailed configuration documentation

### Schema Validation

```python
import pynlm

# Create configuration
config = pynlm.EnhancedConfig.createFromPreset("research")

# Validate configuration
if config.isValid():
    print("✓ Configuration is valid")
else:
    print("✗ Configuration errors:")
    for error in config.getValidationErrors():
        print(f"  - {error}")
```

### Configuration Validation Examples

```python
# Invalid: Negative neurons
config = pynlm.EnhancedConfig.createFromPreset("basic")
config.set("neuron_count", -1)
# Raises exception: Configuration validation failed

# Valid: Correct configuration
config = pynlm.EnhancedConfig.createFromPreset("basic")
config.set("neuron_count", 500)
# No exception: Configuration is valid
```

## Performance and Efficiency

### Memory Management

Context managers ensure proper cleanup of resources:

```python
# Resources are automatically managed
with pynlm.agent_simulation(preset="research") as sim:
    sim.runSteps(100)

# No need to manually delete objects
```

### Batch Processing

For scientific experiments, use batch running:

```python
# Run 50 episodes for statistical analysis
results = pynlm.batchRunEpisodes(50, 200)

# Analyze results
total_rewards = [r["total_reward"] for r in results]
avg_rewards = [r["avg_reward_per_step"] for r in results]
```

### Configuration Management

```python
# Create configuration from preset
config1 = pynlm.EnhancedConfig.createFromPreset("research")

# Create custom configuration
config2 = pynlm.EnhancedConfig.createFromPreset("basic")
config2.set("neuron_count", 1000)

# Merge configurations
config1.merge(config2)
```

## Best Practices

### 1. Use Context Managers

Always use context managers for simulation management:

```python
with pynlm.agent_simulation(preset="research") as sim:
    sim.runSteps(100)
```

### 2. Validate Configurations

Validate configurations before running simulations:

```python
config = pynlm.EnhancedConfig.createFromPreset("research")
if config.isValid():
    print("Configuration is valid!")
else:
    print("Configuration has errors")
```

### 3. Use Presets for Consistency

Use presets for consistent experimental setups:

```python
config = pynlm.EnhancedConfig.createFromPreset("research")
```

### 4. Batch Run for Statistical Analysis

For scientific experiments, use batch running:

```python
results = pynlm.batchRunEpisodes(50, 200)
```

### 5. Save Checkpoints

Save checkpoints during long simulations:

```python
with pynlm.agent_simulation(preset="research") as sim:
    for i in range(0, 1000, 100):
        sim.runSteps(100)
        brain.save(f"checkpoint_{i}.bin")
```

## Advanced Features

### Method Chaining

```python
# Chain method calls for fluent interface
pynlm.agent_simulation(preset="research")
    .runSteps(50)
    .runSteps(50)
    .runSteps(50)
```

### Statistics Formatting

```python
with pynlm.agent_simulation(preset="research") as sim:
    sim.runSteps(100)
    
    # Get statistics in different formats
    table_stats = sim.formatStatistics("table")
    json_stats = sim.formatStatistics("json")
```

### Configuration Diffing

```python
# Compare configurations
config1 = pynlm.EnhancedConfig.createFromPreset("basic")
config2 = pynlm.EnhancedConfig.createFromPreset("research")

diff = config1.diff(config2)
print(f"Differences: {len(diff)} changes")
```

## API Reference

### Class Reference

#### `pynlm.BrainSimulationContext`
Context manager for brain simulations with advanced helpers.

#### `pynlm.WorldSimulationContext`
Context manager for world simulations.

#### `pynlm.EnhancedConfig`
Enhanced configuration class with JSON schema validation.

#### `pynlm.ConfigValidator`
Configuration validator with JSON schema support.

#### `pynlm.SimulationUtils`
Static class with simulation utility functions.

### Method Reference

See the `examples.py` file for comprehensive method documentation and usage examples.

## Migration Guide

### From Basic to Enhanced API

If you're using the basic API and want to upgrade, here's what changes:

#### Old API
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

#### New API
```python
import pynlm

with pynlm.agent_simulation(preset="basic") as sim:
    sim.runSteps(100)
```

### Error Handling Improvements

The enhanced bindings provide better error handling:

```python
# Old: Silent failures
brain.step(step)  # Might fail silently

# New: Descriptive error messages
with pynlm.agent_simulation(preset="basic") as sim:
    sim.runSteps(100)  # Clear error messages if something goes wrong
```

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Configuration Issues

**Configuration validation errors:**
- Check that configuration keys are spelled correctly
- Verify that values are within allowed ranges
- Use `getConfigValidationErrors()` to see detailed errors

## Further Reading

- [`examples.py`](examples.py) - Comprehensive examples
- [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) - System architecture overview
- [`docs/SCIENCE.md`](docs/SCIENCE.md) - Scientific background
- [`docs/EXPERIMENTS.md`](docs/EXPERIMENTS.md) - Experiment descriptions
- [`configs/nlm_schema.json`](configs/nlm_schema.json) - Configuration schema
- [`docs/PYTHON_API.md`](docs/PYTHON_API.md) - Python API documentation

## Getting Help

### Documentation

- **Python API Reference**: See `docs/PYTHON_API.md`
- **Configuration Guide**: See `docs/CONFIGURATION.md`
- **Architecture Overview**: See `docs/ARCHITECTURE.md`
- **Scientific Background**: See `docs/SCIENCE.md`

### Examples

- **Basic Examples**: Run `python examples.py --help`
- **Advanced Examples**: Look at the `examples.py` file
- **Complete Examples**: See the `example_7_complete_agent_example()`

### Community

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: Report issues at https://github.com/nlm-project/nlm/issues

## Acknowledgments

This enhanced version builds on the original NLM Python bindings and adds significant improvements:

- **Enhanced Configuration System**: JSON schema validation with presets
- **Context Managers**: Automatic resource management and simulation helpers
- **Advanced Simulation Tools**: Batch running and statistical analysis
- **Comprehensive Documentation**: Examples and API reference
- **Better Error Handling**: Descriptive error messages and validation

## Changelog

### Version 2.0.0-enhanced

- Added Pythonic context managers
- Added enhanced configuration system with JSON schema validation
- Added advanced simulation helpers and utilities
- Improved error handling and validation
- Enhanced documentation and examples
- Added batch running capabilities
- Added statistical analysis tools
- Added method chaining and fluent interfaces

### Version 1.0.0

- Original Python bindings
- Basic API for NLM access
- Limited configuration options
- Basic error handling

## License

MIT License. See the LICENSE file for details.

---

*This enhanced version provides a more intuitive and powerful Python interface to the NLM framework, making it easier for researchers and developers to work with artificial neural networks.*