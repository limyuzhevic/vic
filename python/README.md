# NLM Python Bindings: Enhanced User Guide

## Overview

This document provides comprehensive guidance for using the enhanced NLM (Neural Learning Machine) Python bindings (version 2.0.0).

The enhanced bindings provide a user-friendly API with convenience functions, better error handling, batch processing capabilities, and advanced analysis tools for both beginners and power users.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Installation](#installation)
3. [Basic Usage](#basic-usage)
4. [Advanced Features](#advanced-features)
5. [Batch Processing](#batch-processing)
6. [Error Handling](#error-handling)
7. [Performance Analysis](#performance-analysis)
8. [Configuration Management](#configuration-management)
9. [Export and Analysis](#export-and-analysis)
10. [Example Scripts](#example-scripts)
11. [API Reference](#api-reference)
12. [Migration Guide](#migration-guide)

## Quick Start

### Installation

```bash
pip install pynlm numpy
```

### Basic Usage

```python
from nlm import createDefaultConfig, createBrain, quickSimulation

# Create configuration and brain
config = createDefaultConfig()
brain = createBrain(config)

# Run a quick simulation
spike_count = quickSimulation(brain, steps=100, input_strength=25.0)
print(f"Simulation completed with {spike_count} spikes")
```

## Installation

### Prerequisites

- Python 3.8 or higher
- numpy (optional, for array operations)

### Installation Commands

```bash
# Install the enhanced NLM Python bindings
pip install pynlm numpy

# Install from source
pip install -e .
```

### Build from Source

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
pip install .
```

## Basic Usage

### Creating Configurations

The enhanced bindings provide factory methods for common configuration scenarios:

```python
from nlm import (
    createDefaultConfig,
    createLearningConfig,
    createMinimalConfig,
    createConfigFromEnv
)

# Create default configuration (Phase 2 optimized)
config = createDefaultConfig()

# Create configuration optimized for learning
learning_config = createLearningConfig(learning_rate=0.05, connection_probability=0.2)

# Create minimal configuration
minimal_config = createMinimalConfig(neuron_count=100)

# Create configuration from environment variables
env_config = createConfigFromEnv()
```

### Creating Brains

```python
from nlm import createBrain, createAgentBrain, createWorld

# Create brain with configuration
brain = createBrain(config)

# Create world
world = createWorld(20, 20, 16, 16)

# Create agent brain interface
agent = createAgentBrain(brain)

# Initialize agent with world
agent.initialize(world)
```

### Running Simulations

```python
from nlm import quickSimulation, runBasicTest, analyzeLearning

# Quick simulation with optional input
spike_count = quickSimulation(brain, steps=100, input_strength=25.0)

# Run basic connectivity test
basic_spikes = runBasicTest(brain)

# Analyze learning process
learning_result = analyzeLearning(brain, steps=500)
learning, weight_change, initial_synapses, final_synapses = learning_result
```

### Getting Brain Statistics

```python
from nlm import getBrainStats, compareBrains

# Get detailed statistics
stats = getBrainStats(brain)
print(f"Neurons: {stats['total_neurons']}")
print(f"Synapses: {stats['total_synapses']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")

# Compare multiple brains
comparison = compareBrains([brain1, brain2, brain3], ["brain1", "brain2", "brain3"])
```

## Advanced Features

### Text Processing (Phase 6)

```python
from nlm import encodeText, createTextPercept, createSensoryPercept

# Encode text into neural patterns
pattern = encodeText("hello world")
print(f"Encoded pattern: {pattern}")

# Create percept with encoded text
vision = [0.5] * 256
touch = [0.0] * 8
text_percept = createTextPercept("test message", vision, touch)
```

### Agent Brain Interface

```python
from nlm import createAgentBrain

# Create agent with full brain access
agent = createAgentBrain(brain)

# Access agent capabilities
agent.initialize(world)
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()

# Get neuromodulation levels
print(f"Dopamine: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
```

### Configuration Templating

```python
from nlm import createFromTemplate

# Create brains from predefined templates
basic_brain = createFromTemplate("basic")
sensorimotor_brain = createFromTemplate("sensorimotor")
cognitive_brain = createFromTemplate("cognitive")
```

## Batch Processing

### Batch Training

```python
from nlm import batchTrain

# Create multiple brains
brains = []
for i in range(5):
    config = createDefaultConfig()
    config.set("neuron_count", 100 + i * 50)
    brain = createBrain(config)
    brains.append(brain)

# Train all brains with rewards
rewards = [10.0, -5.0, 15.0, 0.0, 8.0]
batchTrain(brains, rewards, steps=100)
```

### Batch Analysis

```python
from nlm import getBrainStats, compareBrains

# Create multiple brains for comparison
brains = []
labels = ["control", "treatment1", "treatment2", "treatment3"]

# ... create brains with different configurations

# Compare all brains in one operation
comparison = compareBrains(brains, labels)

# Export results for each brain
for i, (label, stats) in enumerate(comparison.items()):
    json_export = exportBrainState(brains[i], "json")
    csv_export = exportBrainState(brains[i], "csv")
    print(f"{label}: JSON={len(json_export)} chars, CSV={len(csv_export)} chars")
```

## Error Handling

The enhanced bindings provide comprehensive error handling with specific exception types:

```python
from nlm import createLearningConfig, validateConfig
import sys

try:
    # Create configuration
    config = createLearningConfig(0.05, 0.2)
    
    # Validate configuration
    result = validateConfig(config)
    print(f"Validation result: {result}")
    
except ConfigurationError as e:
    print(f"Configuration error: {e}")
    sys.exit(1)
except NLMError as e:
    print(f"NLM error: {e}")
    sys.exit(1)
except Exception as e:
    print(f"Unexpected error: {e}")
    sys.exit(1)
```

### Common Error Scenarios

1. **Invalid Configuration**: Missing required parameters or invalid values
2. **Brain Initialization Errors**: Failed to initialize brain with given configuration
3. **Simulation Errors**: Problems during simulation execution
4. **Export Errors**: Invalid export formats or file access issues

## Performance Analysis

### Benchmarking

```python
from nlm import runBenchmarks

# Run performance benchmarks
benchmarks = runBenchmarks()

print("Performance Benchmarks:")
for metric, value in benchmarks.items():
    if isinstance(value, float):
        print(f"  {metric}: {value:.3f}")
    else:
        print(f"  {metric}: {value}")
```

### Performance Testing

```python
from nlm import quickSimulation, getBrainStats

# Test performance with different configurations
brain_sizes = [100, 500, 1000, 2000]
steps_list = [10, 50, 100]

for size in brain_sizes:
    config = createMinimalConfig(size)
    brain = createBrain(config)
    
    for steps in steps_list:
        import time
        start_time = time.time()
        
        quickSimulation(brain, steps, 10.0)
        
        elapsed = time.time() - start_time
        stats = getBrainStats(brain)
        
        print(f"Size {size:4d}, Steps {steps:3d}: {elapsed:.3f}s, {stats['total_spikes']} spikes")
```

## Configuration Management

### Environment Variables

The enhanced bindings support configuration via environment variables:

```bash
export NLM_NEURON_COUNT=800
export NLM_REGION_COUNT=4
export NLM_RANDOM_SEED=12345
export NLM_LOG_LEVEL=INFO
```

```python
from nlm import createConfigFromEnv

# Create configuration from environment
config = createConfigFromEnv()
print(f"Loaded config: {config.getOr('neuron_count', 0)} neurons")
```

### Configuration Validation

```python
from nlm import validateConfig, createDefaultConfig

# Create and validate configuration
config = createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 2)

# Validate configuration
validation_result = validateConfig(config)
print(f"Validation result: {validation_result}")
```

## Export and Analysis

### Brain State Export

```python
from nlm import exportBrainState

# Export in different formats
brain = createBrain(createDefaultConfig())
quickSimulation(brain, 50, 20.0)

# JSON export
json_output = exportBrainState(brain, "json")
print(f"JSON export: {len(json_output)} characters")

# CSV export
csv_output = exportBrainState(brain, "csv")
print(f"CSV export: {len(csv_output)} characters")

# Custom analysis
analysis = {}
for i in range(5):
    config = createLearningConfig(learning_rate=0.01 * (i + 1), connection_probability=0.1)
    brain = createBrain(config)
    quickSimulation(brain, 20, 5.0)
    
    analysis[f"brain_{i}"] = {
        "neuron_count": config.getOr("neuron_count", 0),
        "spike_count": getBrainStats(brain)["total_spikes"],
        "firing_rate": getBrainStats(brain)["average_firing_rate"],
        "json_size": len(exportBrainState(brain, "json")),
        "csv_size": len(exportBrainState(brain, "csv"))
    }

print("Analysis results:")
for name, data in analysis.items():
    print(f"  {name}: {data}")
```

## Example Scripts

### Basic Example

The `examples/basic_example.py` file demonstrates basic usage with step-by-step examples.

### Advanced Example

The `examples/advanced_example.py` file demonstrates advanced features including:

- Batch processing with multiple configurations
- Performance benchmarking and analysis
- Learning curve tracking
- Text processing and association learning
- Error handling and recovery
- Environment integration
- Advanced features and power user capabilities
- Comprehensive pipeline examples

### Running Examples

```bash
# Run basic examples
python3 python/examples/basic_example.py

# Run advanced examples
python3 python/examples/advanced_example.py
```

## API Reference

### Core Classes

#### Config
- `createDefaultConfig()` - Create default configuration
- `createLearningConfig()` - Create learning-optimized configuration
- `createMinimalConfig()` - Create minimal configuration
- `createConfigFromEnv()` - Create configuration from environment variables

#### Brain
- `createBrain(config)` - Create brain with configuration
- `quickSimulation(brain, steps, input_strength)` - Quick simulation
- `runBasicTest(brain)` - Run basic connectivity test
- `analyzeLearning(brain, steps)` - Analyze learning process
- `getBrainStats(brain)` - Get brain statistics
- `compareBrains(brains, labels)` - Compare multiple brains

#### Agent
- `createAgentBrain(brain)` - Create agent brain interface
- `createWorld()` - Create world instance
- `createSensoryPercept()` - Create sensory percept

#### Export
- `exportBrainState(brain, format)` - Export brain state in specified format

### Special Functions

- `encodeText(text)` - Encode text into neural pattern (Phase 6)
- `createTextPercept()` - Create percept with text encoding
- `createFromTemplate(name)` - Create brain from predefined template
- `validateConfig(config)` - Validate configuration
- `runBenchmarks()` - Run performance benchmarks
- `enableAdvancedFeatures()` - Enable advanced features
- `getVersion()` - Get version information
- `listAvailableBrains()` - List available brain types

### Exception Classes

- `NLMError` - Base NLM exception
- `ConfigurationError` - Configuration-related errors
- `BrainInitializationError` - Brain initialization errors
- `SimulationError` - Simulation errors

## Migration Guide

### From v1.0 to v2.0

#### Breaking Changes

1. **Module Structure**: Enhanced bindings now use the same module name (`pynlm`) but with improved API
2. **Configuration Methods**: Added new convenience methods for configuration
3. **Error Handling**: New exception hierarchy with more specific error types
4. **Factory Methods**: New factory methods for common use cases

#### Migration Examples

**Old v1.0 code**:
```python
# Old way (v1.0)
from pynlm import Config, Brain

config = Config()
config.set("neuron_count", 100)
config.set("region_count", 1)

brain = Brain(config)
brain.initialize()
brain.step(0)
```

**New v2.0 code**:
```python
# New way (v2.0)
from nlm import createDefaultConfig, createBrain, quickSimulation

# Enhanced with factory methods and better defaults
config = createDefaultConfig()
config.set("neuron_count", 100)

brain = createBrain(config)
quickSimulation(brain, 100, 25.0)
```

**Benefits of v2.0**:

1. **Better Defaults**: Phase 2 optimized defaults
2. **Convenience Methods**: Factory methods for common scenarios
3. **Improved Error Handling**: Specific exception types
4. **Enhanced Documentation**: Comprehensive examples and tutorials
5. **Advanced Features**: Batch processing, performance analysis, text processing
6. **Environment Support**: Configuration from environment variables
7. **Export Capabilities**: Multiple export formats (JSON, CSV)
8. **Comparison Tools**: Compare multiple brains easily

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "ModuleNotFoundError: No module named 'nlm'"

**Solution**: Ensure you're using the enhanced bindings:
```bash
pip install pynlm
# OR
pip install -e .
```

#### Issue 2: "ConfigurationError: Configuration validation failed"

**Solution**: Check configuration requirements:
```python
from nlm import createLearningConfig, validateConfig

try:
    config = createLearningConfig(learning_rate=0.05, connection_probability=0.2)
    result = validateConfig(config)
    print(f"Validation: {result}")
except Exception as e:
    print(f"Configuration error: {e}")
```

#### Issue 3: "BrainInitializationError: Failed to initialize brain"

**Solution**: Check configuration values:
```python
from nlm import createLearningConfig

# Ensure valid configuration
config = createLearningConfig(learning_rate=0.01, connection_probability=0.1)
# Add required parameters
config.set("neuron_count", 100)
config.set("region_count", 1)
```

#### Issue 4: Export Errors

**Solution**: Check supported formats:
```python
from nlm import exportBrainState

# Supported formats: "json", "csv"
try:
    output = exportBrainState(brain, "json")  # or "csv"
except Exception as e:
    print(f"Export error: {e}")
    # Try alternative format
    output = exportBrainState(brain, "csv")
```

## Support and Documentation

### Getting Help

- **Documentation**: See this file for comprehensive API reference
- **Examples**: Run `python3 python/examples/basic_example.py` and `python3 python/examples/advanced_example.py`
- **Issues**: Report issues on GitHub
- **Community**: Join the NLM discussion forum

### Updates and Improvements

This enhanced version (v2.0.0) includes:

- **Enhanced Python Bindings**: User-friendly API with convenience functions
- **Better Error Handling**: Specific exception types and comprehensive error messages
- **Batch Processing**: Tools for processing multiple brains efficiently
- **Performance Analysis**: Benchmarking and performance testing utilities
- **Text Processing**: Phase 6 text encoding and association learning
- **Configuration Management**: Environment variable support and validation
- **Export Capabilities**: Multiple export formats for analysis
- **Comprehensive Documentation**: Examples, tutorials, and API reference

## License

MIT License

See `LICENSE` file for details.

## Copyright

Copyright 2024 NLM Authors

---

This enhanced documentation provides comprehensive guidance for using the NLM Python bindings. The enhanced bindings are designed to provide an excellent user experience for both beginners and advanced users, with extensive documentation, examples, and support for common use cases.
