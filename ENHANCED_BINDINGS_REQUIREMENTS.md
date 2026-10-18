# Enhanced NLM Python Bindings - Requirements

This file documents the requirements for using the enhanced pynlm library.

## Basic Requirements

```python
import pynlm

# Basic usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

## Enhanced Features Requirements

### 1. Standard Library Dependencies

The enhanced bindings require no additional Python packages beyond the standard library.

### 2. C++ Dependencies

The enhanced bindings extend the original C++ dependencies:

```cpp
# Original dependencies (from pyproject.toml)
requires = ["scikit-build-core>=0.5.0", "pybind11>=2.11.0"]

# Enhanced dependencies (added for improved features)
requires = ["scikit-build-core>=0.5.0", "pybind11>=2.11.0", "numpy>=1.20", "pytest>=7.0"]
```

### 3. Build Requirements

```bash
# Install build dependencies
pip install scikit-build-core pybind11 numpy pytest

# Build and install enhanced bindings
pip install -e .

# Or install specific build
pip install build
python -m build
```

## Installation Instructions

### 1. From Source (Recommended for Development)

```bash
# Clone the repository
cd /path/to/nlm

# Install build dependencies
pip install scikit-build-core pybind11 numpy pytest

# Build and install in development mode
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

### 2. For Production

```bash
# Install with scikit-build-core
pip install scikit-build-core pybind11 numpy
pip install .
```

### 3. Quick Test

```python
# Test basic functionality
import pynlm

# Create and test
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Test enhanced features
brain.initialize()
stats = brain.getStatistics()
print(f"Brain stats: {stats}")

# Test context managers
with pynlm.brainContext(brain) as ctx:
    print("Context manager active")
```

## Usage Examples

### Basic Usage

```python
import pynlm

# Simple brain simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Enhanced Configuration

```python
import pynlm

# Create configuration with type safety
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 2000)
config.setDouble("brain.synapse_density", 0.15)
config.setBool("plasticity.stdp.enable", True)

# Convert to/from JSON
config_dict = pynlm.configToJson(config)
config2 = pynlm.configFromJson(config_dict)

# Save and load
config.saveToFile("my_config.json")
loaded_config = pynlm.loadConfigFromFile("my_config.json")
```

### Context Managers

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Use context manager for safe resource management
with pynlm.brainContext(brain) as brain_ctx:
    brain_ctx.initialize()
    print("Brain is safely managed")
```

### Advanced Operations

```python
import pynlm

# Create neural network
brain = pynlm.createNetwork(5000, 10)

# Connect regions
pynlm.connectRegions(brain, "region0", "region1", 0.5)
pynlm.connectRegions(brain, "region1", "region2", 0.3)

# Analyze brain
analysis = pynlm.analyzeBrain(brain)
print(f"Network stability: {analysis['network_stability']:.4f}")

# Get comprehensive statistics
stats = brain.getStatistics()
print(f"Neuron count: {stats['total_neurons']}")
print(f"Synapse count: {stats['total_synapses']}")
```

### Error Handling

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Test error handling
try:
    brain.injectCurrentToNeuronType("INVALID_TYPE", 1.0)
except pynlm.NLMException as e:
    print(f"Caught NLM exception: {e}")

try:
    pynlm.loadConfigFromFile("nonexistent_file.json")
except pynlm.ConfigurationError as e:
    print(f"Caught configuration error: {e}")
```

### Standard Library Integration

```python
import pynlm

# Use with Python collections
config = pynlm.createDefaultConfig()

# List operations
settings = [
    ("brain.neuron_count", 1500),
    ("brain.synapse_density", 0.12),
    ("learning.rate", 0.001),
]

for key, value in settings:
    config.set(key, value)

# Dictionary operations
updates = {
    "brain.v_thresh": -50.0,
    "brain.v_rest": -65.0,
}

for key, value in updates.items():
    config.set(key, value)

# Generator operations
keys = config.getKeys()
sorted_keys = sorted(config.getKeys())
print(f"Processed {len(list(keys))} configuration keys")
```

### Simulation Utilities

```python
import pynlm

# Create and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run complete simulation
total_spikes = pynlm.runSimulation(brain, 100)
print(f"Simulation complete: {total_spikes} total spikes")

# Reset brain
pynlm.resetBrain(brain)
```

### Data Persistence

```python
import pynlm

# Create and train brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run training
for i in range(200):
    brain.step(i)

# Save brain state
pynlm.saveBrainState(brain, "trained_brain.bin")

# Load brain state
new_brain = pynlm.createBrain(config)
pynlm.loadBrainState(new_brain, "trained_brain.bin")

# Clean up
import os
os.remove("trained_brain.bin")
```

## Testing and Development

### Running Tests

```bash
# Run the enhanced bindings test suite
python tests/test_enhanced_bindings.py

# Run examples
python examples/enhanced_examples.py
```

### Test Suite Coverage

The enhanced bindings include comprehensive tests for:

1. **Basic functionality**: Brain creation, initialization, simulation
2. **Configuration management**: Type-safe settings, JSON conversion
3. **Context managers**: Safe resource management
4. **Convenience methods**: Enhanced API features
5. **Error handling**: Python-specific exceptions
6. **Advanced operations**: Neural networks, connectivity
7. **Standard library integration**: Collections, generators
8. **Simulation utilities**: Episode management, statistics
9. **Data persistence**: Save/load functionality
10. **Module constants**: Easy access to enumerations
11. **Advanced utilities**: Pattern matching, random generation
12. **Comprehensive episode**: Full agent simulation

### Development Guidelines

#### Adding New Features

1. **Type Safety**: Use the enhanced type-safe configuration methods
2. **Error Handling**: Use Python-specific exceptions for better debugging
3. **Documentation**: Include comprehensive docstrings
4. **Context Management**: Use context managers for resource safety
5. **Standard Library**: Integrate with Python collections and generators

#### Best Practices

1. **Use context managers** for brain and world management
2. **Leverage type-safe configuration** to prevent errors
3. **Handle exceptions** gracefully with appropriate error types
4. **Use convenience methods** for common operations
5. **Utilize standard library** patterns for data processing
6. **Test thoroughly** with the provided test suite

#### Performance Considerations

1. **Reuse configurations** instead of creating new ones
2. **Use batch operations** for multiple settings
3. **Leverage generators** for memory efficiency
4. **Profile with cProfile** for optimization
5. **Monitor memory usage** for long simulations

## Version Compatibility

### Supported Python Versions

- Python 3.8+
- Python 3.9+
- Python 3.10+
- Python 3.11+
- Python 3.12+

### C++ Compiler Requirements

- GCC 10+ (Recommended)
- Clang 12+ (Recommended)
- MSVC 2019+ (Windows)

### CMake Requirements

- CMake 3.16+

## Troubleshooting

### Common Issues

#### "ImportError: No module named 'pynlm'"

```bash
pip install --force-reinstall .
```

#### "Segmentation fault"

```python
# Ensure proper initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this before stepping
```

#### Configuration Loading Issues

```python
# Use file paths that exist
config = pynlm.loadConfigFromFile("config.json")
# or create default config
config = pynlm.createDefaultConfig()
```

#### Memory Issues

```python
# Use context managers for cleanup
with pynlm.brainContext(brain) as ctx:
    # Your code here
    pass  # Context manager cleans up automatically
```

### Performance Optimization

1. **Use batch operations** for multiple configuration settings
2. **Reuse brain instances** instead of creating new ones
3. **Use efficient data structures** for large-scale simulations
4. **Profile with cProfile** to identify bottlenecks
5. **Monitor memory usage** for long-running simulations

### Debugging

1. **Enable verbose logging** by calling `brain.logStatus()`
2. **Use Python debuggers** like pdb or ipdb
3. **Check exception details** for specific error messages
4. **Use print statements** for quick debugging
5. **Consult documentation** for parameter meanings

## Migration from Original Bindings

### Breaking Changes

1. **Enhanced API**: Some methods have been enhanced with additional parameters
2. **New exceptions**: Python-specific exceptions have been added
3. **Convenience methods**: New methods provide easier access to functionality
4. **Type safety**: Configuration setting is now more type-safe

### Migration Examples

#### From Original to Enhanced Configuration

```python
# Original
config.set("neuron_count", 1000)

# Enhanced
config.setInt("brain.neuron_count", 1000)
```

#### From Original to Enhanced Error Handling

```python
# Original
// Some C++ code

# Enhanced
try:
    brain.injectCurrentToNeuronType("INVALID_TYPE", 1.0)
except pynlm.NLMException as e:
    print(f"Error: {e}")
```

#### From Original to Enhanced Context Management

```python
# Original
brain = pynlm.createBrain(config)
brain.initialize()
# ... do work ...
# No automatic cleanup

# Enhanced
with pynlm.brainContext(brain) as brain_ctx:
    brain_ctx.initialize()
    # ... do work ...
    # Automatic cleanup
```

### What's New

The enhanced bindings provide these additional features:

1. **Comprehensive documentation** with examples
2. **Pythonic API** with context managers and properties
3. **Type-safe configuration** methods
4. **Enhanced error handling** with specific exceptions
5. **Convenience methods** for common operations
6. **Standard library integration** for data processing
7. **Advanced simulation utilities** for complex scenarios
8. **Data persistence** enhancements
9. **Comprehensive test suite** for validation
10. **Example applications** for demonstration

These enhancements make the NLM Python bindings more powerful, easier to use, and more robust while maintaining full backward compatibility.

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, Pythonic interface to the NLM neural simulation framework. They include:

- **Enhanced documentation** and type safety
- **Pythonic API** features for better usability
- **Advanced functionality** for complex simulations
- **Robust error handling** with specific exceptions
- **Standard library integration** for modern Python development
- **Comprehensive testing** for reliability
- **Practical examples** for learning and development

These improvements make the NLM bindings more accessible to Python developers while maintaining full compatibility with existing code.