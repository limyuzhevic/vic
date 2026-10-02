# NLM Advanced Configuration - Complete Guide

This document describes the new advanced configuration features and improvements made to the NLM (Neural Learning Machine) framework.

## Overview

The NLM framework has been significantly improved with enhanced configuration options, factory methods, and better error handling. These improvements make it easier for users to:

1. Create brains with preset configurations
2. Build custom configurations using a fluent API
3. Validate configurations for correctness
4. Use brains with callback-based initialization
5. Work with advanced Python APIs

## New Factory Methods

### BrainFactory

The `BrainFactory` class provides static methods to create brains with predefined configurations:

```python
import pynlm

# Create a default brain
brain = pynlm.createDefaultBrain("default")

# Create a minimal brain (faster, fewer features)
brain = pynlm.createMinimalBrain()

# Create an experimental brain (enhanced features)
brain = pynlm.createExperimentalBrain()

# Create a research-grade brain (maximum features)
brain = pynlm.createResearchBrain()
```

#### Available Configuration Types

| Configuration | Neuron Count | Regions | Neuromodulation | Development | Use Case |
|---------------|--------------|---------|-----------------|-------------|----------|
| "default" | 1,000 | 4 | Enabled | Enabled | General purpose |
| "minimal" | 100 | 1 | Disabled | Disabled | Performance testing |
| "experimental" | 5,000 | 8 | Enabled | Enabled | Research experiments |
| "research" | 20,000 | 16 | Enabled | Enabled | Scientific research |

### BrainConfigBuilder

The `BrainConfigBuilder` provides a fluent API for creating custom brain configurations:

```python
config = pynlm.BrainConfigBuilder() \
    .set_neuron_count(2000) \
    .set_region_count(8) \
    .set_connection_probability(0.15f) \
    .set_timestep(0.002) \
    .set_random_seed(12345) \
    .enable_neuromodulation(True) \
    .enable_development(True) \
    .enable_curiosity(True) \
    .enable_structural_plasticity(True) \
    .enable_memory(True) \
    .enable_prediction(True) \
    .enable_cognition(True) \
    .build()

brain = pynlm.createBrain(config)
```

#### Available Builder Methods

| Method | Type | Default | Description |
|--------|------|---------|-------------|
| `setNeuronCount()` | int | 1000 | Number of neurons (1-1,000,000) |
| `setRegionCount()` | int | 4 | Number of brain regions (1-100) |
| `setConnectionProbability()` | float | 0.1 | Connection probability (0.0-1.0) |
| `setTimestep()` | double | 0.001 | Simulation timestep (0.0-0.1 seconds) |
| `setRandomSeed()` | int | 0 | Random seed for reproducibility |
| `enableNeuromodulation()` | bool | True | Enable neuromodulation system |
| `enableDevelopment()` | bool | True | Enable developmental plasticity |
| `enableCuriosity()` | bool | True | Enable curiosity-driven exploration |
| `enableStructuralPlasticity()` | bool | True | Enable synaptogenesis/pruning |
| `enableMemory()` | bool | True | Enable memory systems |
| `enablePrediction()` | bool | True | Enable prediction system |
| `enableCognition()` | bool | True | Enable cognitive functions |

## Configuration Validation

### validateConfig()

The `validateConfig()` function checks configuration for errors before creating a brain:

```python
config = brain.getConfig()
if pynlm.validateConfig(config):
    print("✓ Configuration is valid")
else:
    print("✗ Configuration has errors")
```

#### Validation Checks

- Configuration is not null
- Required keys exist (neuron_count, region_count)
- Value ranges are correct:
  - Neuron count: 1-1,000,000
  - Region count: 1-100
  - Connection probability: 0.0-1.0
  - Timestep: 0.0-0.1 seconds

## Brain Creation from File

Create brains from configuration files:

```python
# Save a configuration to file
brain.save("my_brain.bin")

# Create brain from saved state
brain = pynlm.createBrainFromFile("my_brain.bin")

# Create from JSON configuration file
brain = pynlm.createBrainFromFile("config.json")
```

## Callback-Based Initialization

Create brains with custom initialization callbacks:

```python
def custom_init(brain):
    print(f"Initializing brain with {brain.getTotalNeuronCount()} neurons")
    # Add custom initialization logic
    # Enable specific features
    # Load custom configurations

brain = pynlm.createBrainWithCallback(custom_init)
```

## Error Handling and Validation

The new implementation includes comprehensive error handling:

### Common Exceptions

- `ValueError`: Invalid parameter values
- `RuntimeError`: Configuration file loading failures
- `TypeError`: Null reference errors
- `std::invalid_argument`: Invalid configuration parameters

### Error Recovery

The system includes error recovery mechanisms:

1. **Graceful Degradation**: If features fail to initialize, the brain continues with available features
2. **Default Fallback**: Uses safe defaults when configuration is invalid
3. **Detailed Error Messages**: Clear information about what went wrong

## Performance Considerations

### Memory Optimization

- **Minimal Configuration**: Reduced memory usage for performance testing
- **Lazy Initialization**: Features initialize only when needed
- **Memory Pooling**: Internal memory management for better performance

### Simulation Speed

- **Configurable Timestep**: Adjust simulation speed vs. accuracy tradeoff
- **Region-based Processing**: Parallel processing across brain regions
- **Optimized Connectivity**: Sparse matrix operations for large networks

## Advanced Usage Examples

### Multi-Stage Training

```python
# Stage 1: Learn with minimal brain
minimal_brain = pynlm.createMinimalBrain()
minimal_brain.initialize()
for step in range(1000):
    minimal_brain.step(step)

# Stage 2: Enhance with experimental brain
experimental_brain = pynlm.createExperimentalBrain()
experimental_brain.initialize()
for step in range(5000):
    experimental_brain.step(step)

# Stage 3: Research with full capabilities
research_brain = pynlm.createResearchBrain()
research_brain.initialize()
for step in range(20000):
    research_brain.step(step)
```

### Reproducible Research

```python
# Set random seed for reproducible results
config = pynlm.BrainConfigBuilder() \
    .set_random_seed(42) \
    .set_neuron_count(5000) \
    .build()

brain = pynlm.createBrain(config)
brain.initialize()

# Run same experiment multiple times
for run in range(10):
    # Reset brain state
    brain.reset()
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
    
    # Record results
    results.append({
        'run': run,
        'total_reward': get_reward(brain),
        'firing_rate': brain.getAverageFiringRate()
    })
```

### Experiment Control

```python
# Create configuration for specific experiment
experiment_config = pynlm.BrainConfigBuilder() \
    .set_neuron_count(3000) \
    .set_region_count(6) \
    .enable_neuromodulation(True) \
    .enable_curiosity(True) \
    .enable_structural_plasticity(True) \
    .enable_memory(True) \
    .enable_prediction(True) \
    .enable_cognition(True) \
    .build()

# Save experiment configuration
experiment_config.saveToFile("experiment_config.json")

# Load for different experimental conditions
for condition in ['baseline', 'reward', 'curiosity', 'novelty']:
    config = pynlm.BrainConfigBuilder().applyTo(
        pynlm.BrainConfigBuilder.defaultConfig()
    )
    # Modify configuration based on condition
    brain = pynlm.createBrain(config)
    brain.initialize()
    run_experiment(brain, condition)
```

## Migration Guide

### From Simple to Advanced Usage

**Before (Simple API)**:
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

**After (Advanced API)**:
```python
# Use factory for common cases
brain = pynlm.createDefaultBrain("default")
brain.initialize()

# Or use builder for custom configurations
config = pynlm.BrainConfigBuilder() \
    .set_neuron_count(2000) \
    .set_region_count(8) \
    .enable_curiosity(True) \
    .build()
brain = pynlm.createBrain(config)
brain.initialize()
```

### Backward Compatibility

The new API maintains full backward compatibility:

1. **Existing Code**: All existing code continues to work unchanged
2. **New Features**: Additional features are available through new methods
3. **Error Handling**: Better error messages for debugging
4. **Validation**: Automatic validation prevents common mistakes

## Performance Benchmarks

| Configuration | Neurons | Memory | Speed | Use Case |
|---------------|---------|--------|-------|----------|
| Minimal | 100 | Low | Fast | Testing |
| Default | 1,000 | Medium | Medium | General |
| Experimental | 5,000 | High | Slow | Research |
| Research | 20,000 | Very High | Very Slow | Large-scale studies |

## Getting Started

### Quick Start

1. **Install**: `pip install pynlm`
2. **Basic Usage**:
   ```python
   import pynlm
   brain = pynlm.createDefaultBrain()
   brain.initialize()
   for step in range(100):
       brain.step(step)
   ```
3. **Advanced Usage**:
   ```python
   config = pynlm.BrainConfigBuilder() \
       .set_neuron_count(2000) \
       .enable_curiosity(True) \
       .build()
   brain = pynlm.createBrain(config)
   ```

### Examples Directory

The `examples/` directory contains comprehensive examples:

- `advanced_brain_config.py`: Complete examples for all new features
- `migration_guide.py`: Migration examples from old to new API
- `performance_benchmarks.py`: Performance comparison examples

## Technical Details

### Implementation Notes

1. **Thread Safety**: All factory methods are thread-safe
2. **Memory Management**: Automatic cleanup of smart pointers
3. **Exception Safety**: No resource leaks in exception scenarios
4. **Configuration Validation**: Input validation prevents runtime errors

### Extension Points

The new architecture supports:

1. **Custom Configuration Builders**: Create custom builder implementations
2. **Brain Factory Subclasses**: Extend factory for domain-specific brains
3. **Plugin System**: Add new brain types without modifying core code
4. **Configuration Templates**: Save and load experiment configurations

## Conclusion

These improvements significantly enhance the NLM framework by:

1. **Simplifying Common Use Cases**: Factory methods for typical scenarios
2. **Empowering Advanced Users**: Builder pattern for complex configurations
3. **Improving Reliability**: Comprehensive validation and error handling
4. **Enhancing Developer Experience**: Better APIs and documentation
5. **Supporting Research**: Advanced features for scientific experiments

The new features maintain full backward compatibility while providing powerful new capabilities for both simple and complex use cases.
