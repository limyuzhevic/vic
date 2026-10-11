# Tutorial 2: Configuration Presets and Best Practices

## Overview
This tutorial covers the new configuration preset functions and best practices for setting up NLM simulations.

## Why Configuration Presets Matter
Different simulation scenarios require different brain configurations. Instead of manually setting every parameter, NLM provides several predefined configuration presets:

- **Development Config**: Optimized for learning and development
- **Standard Config**: Balanced configuration for typical use cases
- **High Performance Config**: Optimized for speed and efficiency
- **Minimal Config**: Simple setup for basic testing

## Configuration Presets Explained

### 1. Development Configuration (`createDevelopmentConfig()`)

**Purpose**: Create a brain optimized for learning and adaptation

**Features**:
- Full plasticity support (STDP + Hebbian + Structural)
- Neuromodulation enabled (dopamine, curiosity)
- Advanced learning mechanisms
- Suitable for complex learning scenarios

**Use Case**: Training agents in dynamic environments where adaptation is crucial

```python
import pynlm

# Create development-optimized brain
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Standard Configuration (`createStandardConfig()`)

**Purpose**: Balanced configuration for typical simulations

**Features**:
- Basic plasticity (STDP + Hebbian)
- Curiosity-enabled exploration
- Good performance and learning balance
- Simpler parameter setup

**Use Case**: General-purpose simulations where you want solid learning without complexity

```python
import pynlm

# Create standard configuration
config = pynlm.createStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### 3. High Performance Configuration (`createHighPerformanceConfig()`)

**Purpose**: Optimized for speed and efficiency

**Features**:
- Full plasticity support
- Event-driven simulation
- Vectorization enabled
- Performance optimizations

**Use Case**: Large-scale simulations where speed is critical

```python
import pynlm

# Create high-performance configuration
config = pynlm.createHighPerformanceConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

### 4. Minimal Configuration (`createMinimalConfig()`)

**Purpose**: Basic setup for testing and simple scenarios

**Features**:
- No plasticity or neuromodulation
- Basic neural computation
- Fast initialization
- Minimal memory footprint

**Use Case**: Quick testing, prototyping, or when you don't need learning

```python
import pynlm

# Create minimal configuration
config = pynlm.createMinimalConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

## Choosing the Right Configuration

### Decision Matrix

| Simulation Goal | Recommended Config | Why |
|----------------|-------------------|-----|
| **Learning from experience** | `createDevelopmentConfig()` | Full learning capabilities |
| **Basic behavior testing** | `createMinimalConfig()` | Fast and simple |
| **Medium-scale learning** | `createStandardConfig()` | Balanced approach |
| **Large-scale simulations** | `createHighPerformanceConfig()` | Optimized for speed |

### Configuration Examples

#### Example 1: Learning Simulation
```python
# For training a learning agent
import pynlm

config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable additional learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

#### Example 2: Quick Test
```python
# For rapid prototyping
import pynlm

config = pynlm.createMinimalConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Just test basic brain function - no learning needed
for step in range(50):
    brain.step(step)
```

#### Example 3: Performance Testing
```python
# For benchmarking
import pynlm

config = pynlm.createHighPerformanceConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Profile performance
import time
start = time.time()

for step in range(1000):
    brain.step(step)

end = time.time()
print(f"Performance: {1000/(end-start):.0f} steps/second")
```

## Configuration Validation

The new validation system helps you identify configuration issues before they cause problems:

```python
import pynlm

# Create configuration
config = pynlm.createDevelopmentConfig()

# Validate configuration
errors, warnings = nlm.validate_config(config)

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")

if warnings:
    print("Configuration warnings:")
    for warning in warnings:
        print(f"  - {warning}")
```

## Advanced Configuration Customization

You can also customize configurations after creation:

```python
import pynlm

# Start with standard config
config = pynlm.createStandardConfig()

# Customize specific parameters
# Note: The exact parameter names depend on your NLM version
# config.set("brain.neuron_count", 2000)
# config.set("plasticity.stdp.learning_rate", 0.01)

# Validate again after changes
errors, warnings = nlm.validate_config(config)
```

## Best Practices

### 1. Always Validate Configurations
```python
def setup_brain(config):
    errors, warnings = nlm.validate_config(config)
    
    if errors:
        raise ValueError(f"Invalid configuration: {errors}")
    
    if warnings:
        print("Configuration warnings:")
        for warning in warnings:
            print(f"  - {warning}")
    
    return config
```

### 2. Use Appropriate Presets
```python
# Pick the right preset for your use case
if learning_required:
    config = pynlm.createDevelopmentConfig()
elif quick_testing:
    config = pynlm.createMinimalConfig()
elif balanced_needs:
    config = pynlm.createStandardConfig()
elif performance_critical:
    config = pynlm.createHighPerformanceConfig()
```

### 3. Profile Before Optimizing
```python
import pynlm

# Start with standard config for profiling
config = pynlm.createStandardConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Profile performance
profile = pynlm.profile_brain_simulation(brain, 1000)
print(f"Average step time: {profile['average_step_time_ms']:.2f} ms")
print(f"Steps per second: {profile['steps_per_second']:.0f}")

# If performance is poor, try high-performance config
if profile['average_step_time_ms'] > 10:  # Adjust threshold as needed
    config = pynlm.createHighPerformanceConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Upgraded to high-performance configuration")
```

### 4. Memory Management
```python
import pynlm

# For long-running simulations
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Periodically clean up memory
if simulation_step % 1000 == 0:
    nlm.cleanup_memory()
    print("Memory cleaned up")
```

## Troubleshooting Common Issues

### Issue: Configuration Not Working as Expected
**Solution**: Validate your configuration and check for errors/warnings

```python
config = pynlm.createDevelopmentConfig()
errors, warnings = nlm.validate_config(config)
if errors:
    print("Fix configuration errors first")
```

### Issue: Poor Performance
**Solution**: Profile and consider high-performance configuration

```python
profile = nlm.profile_brain_simulation(brain, 1000)
if profile['average_step_time_ms'] > threshold:
    # Upgrade to high-performance config
    config = pynlm.createHighPerformanceConfig()
```

### Issue: Memory Issues
**Solution**: Use minimal config or cleanup memory

```python
# For memory-constrained environments
config = pynlm.createMinimalConfig()
# Or periodically clean up
nlm.cleanup_memory()
```

## Key Takeaways

1. **Use presets for quick setup**: Choose the right preset based on your simulation goals
2. **Always validate configurations**: Catch issues before they cause problems
3. **Profile before optimizing**: Measure performance before making changes
4. **Balance features and performance**: Don't pay for features you don't need
5. **Customize when necessary**: Modify presets for specific use cases
6. **Monitor memory usage**: Clean up periodically for long simulations

## Next Steps

1. **Tutorial 3: Advanced Usage** - Learn about monitoring and callbacks
2. **Tutorial 4: Custom Environments** - Create specialized world setups
3. **Tutorial 5: Performance Optimization** - Advanced optimization techniques

## Additional Resources

- [NLM Documentation](https://nlm.readthedocs.io)
- [Configuration Reference](docs/ARCHITECTURE.md)
- [Performance Guide](docs/PERFORMANCE.md)

The configuration system is designed to make it easy to get started while still providing flexibility for advanced users. Start with presets and customize as needed!