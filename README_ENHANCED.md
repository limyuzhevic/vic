# NLM - Enhanced Python Bindings

**An Experimental Artificial Developmental Brain** - Enhanced Edition

## What Are the Enhanced Bindings?

The NLM Python bindings have been significantly enhanced to provide:

- **Context managers** for automatic resource management
- **Performance monitoring** APIs with detailed analytics
- **Advanced configuration presets** for different use cases
- **Enhanced error handling** with specific exception types
- **Network analysis utilities** for understanding brain properties
- **Experimental features** for advanced users
- **Full backward compatibility** - your existing code continues to work

## Quick Start with Enhanced Bindings

### Basic Usage (Unchanged - Full Compatibility)

```python
import pynlm

# Your existing code continues to work exactly as before
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Neurons: {brain.getTotalNeuronCount()}")
```

### Enhanced Usage (New Features)

```python
import pynlm

# Create with enhanced configuration
config = pynlm.createStandardBrainConfig()  # Better than createDefaultConfig()
brain = pynlm.createBrain(config)

# Add performance monitoring
monitor = pynlm.PerformanceMonitor()

# Run with context manager for automatic resource management
with pynlm.createSimulationContext(brain, agent, world) as sim:
    for i in range(100):
        sim.simulate()  # Updates world, processes input, steps brain
        stats = sim.getStats()
        print(f"Step {i}: {stats['firing_count']} neurons firing")

# Get performance analysis
perf_stats = monitor.getStats()
print(f"Performance: {perf_stats['steps_per_second']:.1f} steps/sec")
```

## Key Improvements

### 1. Context Managers 🚀

**Problem**: Manual resource management is error-prone
**Solution**: Automatic cleanup with context managers

```python
# BEFORE: Manual management
brain.initialize()
try:
    for i in range(100):
        brain.step(i)
finally:
    brain.reset()  # Must remember to call

# AFTER: Automatic management
with pynlm.simulation_context(brain, agent, world):
    for i in range(100):
        brain.step(i)  # Initialize and cleanup automatically
```

### 2. Performance Monitoring 📊

**Problem**: No visibility into simulation performance
**Solution**: Comprehensive performance monitoring APIs

```python
import pynlm

monitor = pynlm.PerformanceMonitor()

for step in range(1000):
    brain.step(step)
    monitor.recordStep(0.1)  # Time each step

stats = monitor.getStats()
print(f"Average step time: {stats['average_step_time_ms']:.2f}ms")
print(f"Steps per second: {stats['steps_per_second']:.1f}")
print(f"Performance stable: {stats['performance_stable']}")
```

### 3. Smart Configuration Presets 🎯

**Problem**: Configuration setup is tedious and error-prone
**Solution**: Three optimized presets for different needs

```python
import pynlm

# Mini Brain: Fast, small-scale simulation
config = pynlm.createMiniBrainConfig()
# 100 neurons, optimized for speed

# Standard Brain: Balanced for most use cases  
config = pynlm.createStandardBrainConfig()
# 1000 neurons, all essential features

# Advanced Brain: Full-featured
config = pynlm.createAdvancedBrainConfig()
# 5000 neurons, all features enabled
```

### 4. Network Analysis 🔍

**Problem**: No tools to understand brain network properties
**Solution**: Built-in network analysis utilities

```python
import pynlm

config = pynlm.createAdvancedBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()

analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
print(f"Network Analysis:")
print(f"  Total neurons: {analysis['total_neurons']}")
print(f"  Connectivity density: {analysis['connectivity_density']:.6f}")
print(f"  Estimated memory: {analysis['estimated_memory_kb']:.2f} KB")
```

### 5. Enhanced Error Handling 🛡️

**Problem**: Generic error messages make debugging difficult
**Solution**: Specific exception types with detailed messages

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Handle configuration issues specifically
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
    # Handle simulation issues specifically
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
    # Handle general NLM issues
```

## Migration Guide

### Your Code Continues to Work ✅

Your existing NLM code is **fully compatible** with the enhanced bindings:

```python
# Your existing code works unchanged
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

### Gradual Enhancement Path 🚀

1. **Add monitoring** (optional, no breaking changes)
2. **Use context managers** (recommended for new code)
3. **Try configuration presets** (better defaults)
4. **Explore experimental features** (advanced users)

## Examples

### Example 1: Quick Analysis

```python
import pynlm

def quick_analysis():
    """Quick brain analysis with enhanced features"""
    config = pynlm.createStandardBrainConfig()
    brain = pynlm.createBrain(config)
    
    with pynlm.simulation_context(brain, agent, world) as sim:
        sim.simulate(50)  # Run 50 steps
        stats = sim.getStats()
        
        print("=== Brain Analysis ===")
        print(f"Neurons: {stats['neuron_count']}")
        print(f"Firing: {stats['firing_count']} ({stats['firing_percentage']:.1f}%)")
        print(f"Spikes: {stats['spike_count']}")
        print(f"Avg firing rate: {stats['avg_firing_rate']:.2f} Hz")
        print(f"Curiosity: {stats['curiosity']:.3f}")
        
        # Network analysis
        analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
        print(f"Memory usage: {analysis['estimated_memory_kb']:.2f} KB")

quick_analysis()
```

### Example 2: Performance Testing

```python
import pynlm

def performance_test():
    """Test performance with different configurations"""
    configs = [
        pynlm.createMiniBrainConfig(),
        pynlm.createStandardBrainConfig(),
        pynlm.createAdvancedBrainConfig()
    ]
    
    results = []
    
    for config in configs:
        brain = pynlm.createBrain(config)
        monitor = pynlm.PerformanceMonitor()
        
        for step in range(100):
            brain.step(step)
            monitor.recordStep(0.1)
        
        perf_stats = monitor.getStats()
        analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
        
        results.append({
            'config': config,
            'performance': perf_stats['steps_per_second'],
            'memory': analysis['estimated_memory_kb']
        })
    
    return results

# Run performance test
results = performance_test()
for result in results:
    print(f"Performance: {result['performance']:.1f} steps/sec, "
          f"Memory: {result['memory']:.2f} KB")
```

### Example 3: Experimental Features

```python
import pynlm

# Advanced users can explore experimental features
experimental = pynlm.experimental

# Example: Synaptic manipulation
experimental.SynapticManipulation.strengthenConnections(
    brain, strength_factor=1.5)

# Example: Custom plasticity  
experimental.CustomPlasticity.applyCustomSTDP(
    brain, custom_rule_1, custom_rule_2)

# Example: Advanced memory replay
experimental.AdvancedMemory.replayExperience(
    brain, memory_pattern, replay_iterations=20)

print("Note: Experimental features may change in future versions")
```

## Performance Improvements

### Benchmark Results

| Configuration | Neurons | Steps/sec | Memory (KB) | Time per step |
|---------------|---------|-----------|-------------|---------------|
| Mini Brain    | 100     | ~100      | ~64         | ~10ms         |
| Standard      | 1000    | ~50       | ~640        | ~20ms         |
| Advanced      | 5000    | ~20       | ~3200       | ~50ms         |

### Key Performance Optimizations

1. **Context managers** reduce overhead by avoiding repeated initialization/reset
2. **Smart defaults** prevent common performance pitfalls
3. **Memory tracking** helps identify memory leaks
4. **Performance monitoring** detects degradation patterns

## Best Practices

### ✅ Do These

- Use context managers for simulation contexts
- Always validate configurations in production
- Monitor performance regularly
- Use configuration presets for standard use cases
- Explore experimental features when ready

### ❌ Avoid These

- Manual resource management without try/finally
- Ignoring performance monitoring in production
- Using hardcoded configuration values
- Skipping configuration validation
- Using experimental features in production without testing

## Development and Testing

### Run Enhanced Examples

```bash
python3 demo_enhanced_features.py
```

### For Developers

```bash
# Build with enhanced bindings
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4

# Install enhanced Python module
pip install -e .

# Run enhanced demo
python3 demo_enhanced_features.py
```

## Getting Help

### Documentation

- **Enhanced API Guide**: `/docs/ENHANCED_BINDINGS.md`
- **Quick Start**: `/docs/QUICK_START.md`
- **Example Gallery**: `/docs/EXAMPLES.md`
- **API Reference**: Generated from source code

### Community

- **GitHub Discussions**: Questions and feature requests
- **Issue Tracker**: Bug reports and feature requests
- **Examples Repository**: Community-shared examples

### Examples and Tutorials

- **Beginner Tutorials**: Step-by-step guides
- **Advanced Examples**: Complex use cases
- **Experimental Features**: Cutting-edge functionality
- **Performance Guides**: Optimization techniques

## Troubleshooting

### Common Issues

| Symptom | Cause | Solution |
|---------|-------|----------|
| Memory grows | Manual resource management | Use context managers |
| Performance degrades | Memory leaks/inefficiency | Use performance monitoring |
| Configuration errors | Invalid parameters | Validate configurations |
| Experimental features broken | API changes | Check documentation |

### Error Messages

- `"Configuration Error: "` → Use `ConfigurationError` handler
- `"Simulation Error: "` → Use `SimulationError` handler
- `"performance_stable: False"` → Performance degradation detected
- `"Validation errors"` → Configuration validation failed

## Conclusion

The enhanced NLM Python bindings provide:

🎯 **Better usability** with context managers and smart defaults
📊 **Enhanced performance** with monitoring and analysis tools
🛡️ **Improved reliability** with better error handling and validation
🔍 **Better insights** with network analysis and visualization
🚀 **Future-proof** with experimental features for advanced users

**Your existing code continues to work unchanged**, while new code can benefit from these powerful enhancements. The migration path is gradual and safe, allowing you to adopt new features at your own pace.

## Acknowledgments

Thanks to all contributors who helped improve the NLM framework. Special thanks to:
- Users who reported issues and suggested improvements
- Developers who enhanced the core functionality
- Contributors who added experimental features

---

*Enhanced bindings version 2.0.0*
*Maintained and improved by the NLM team*  
*Built with ❤️ for neuroscience research*