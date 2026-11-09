# NLM Python Bindings - Enhanced Documentation
# Comprehensive Guide to New Features

## Overview

The NLM Python bindings have been enhanced with powerful new features that make it easier to use, more robust, and more powerful for both beginners and advanced users.

## Key Improvements

### 1. Context Managers for Automatic Resource Management

The enhanced bindings introduce context managers that automatically handle resource cleanup:

```python
import pynlm

# Basic setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Legacy way: manual resource management
brain.initialize()
for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
brain.reset()  # Manual cleanup

# Enhanced way: automatic resource management
with pynlm.simulation_context(brain, agent, world, autoReset=True) as sim:
    for i in range(100):
        # Automatic initialization and cleanup
        sim.simulate()
        stats = sim.getStats()
        # Use sim.getBrain(), sim.getAgent(), sim.getWorld() for advanced operations
```

### 2. Performance Monitoring APIs

Track and analyze simulation performance:

```python
import pynlm

# Create monitor
monitor = pynlm.PerformanceMonitor()

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    # Note: In practice, you'd measure elapsed time
    monitor.recordStep(elapsed_time)

# Get performance statistics
stats = monitor.getStats()
print(f"Average step time: {stats['average_step_time_ms']:.2f}ms")
print(f"Steps per second: {stats['steps_per_second']:.1f}")
print(f"Performance stable: {stats['performance_stable']}")

# Reset for next simulation
monitor.reset()
```

### 3. Advanced Configuration Presets

Start with optimized configurations:

```python
import pynlm

# Three preset configurations for different needs

# Mini Brain: Fast, small-scale simulation
config = pynlm.createMiniBrainConfig()
print("Mini brain neuron count:", config.get("brain.neuron_count"))

# Standard Brain: Balanced configuration for most use cases
config = pynlm.createStandardBrainConfig()
print("Standard brain enabled features:", 
      [key for key in config.getKeys() if "enable" in key.lower()])

# Advanced Brain: Full-featured configuration
config = pynlm.createAdvancedBrainConfig()
print("Advanced brain neuron count:", config.get("brain.neuron_count"))
```

### 4. Network Analysis and Visualization Utilities

Analyze brain network properties:

```python
import pynlm

config = pynlm.createAdvancedBrainConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Analyze network properties
analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
print("Network Analysis Results:")
for key, value in analysis.items():
    if isinstance(value, (int, float)):
        if 'memory' in key.lower():
            print(f"  {key}: {value:.2f} KB")
        elif 'rate' in key.lower() or 'time' in key.lower():
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
```

### 5. Enhanced Error Handling

Get specific error types and detailed messages:

```python
import pynlm

try:
    # Configuration operations
    config = pynlm.createDefaultConfig()
    # ... configuration operations
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Handle configuration-specific errors
    
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
    # Handle simulation-specific errors
    
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
    # Handle general NLM errors
```

### 6. Experimental Features (Advanced Users)

Access experimental functionality:

```python
import pynlm

# Access experimental module
experimental = pynlm.experimental

# Synaptic manipulation (experimental)
experimental.SynapticManipulation.strengthenConnections(
    brain, strength_factor=1.5)

# Custom plasticity rules
experimental.CustomPlasticity.applyCustomSTDP(
    brain, custom_pre_rule, custom_post_rule)

# Advanced memory replay
experimental.AdvancedMemory.replayExperience(
    brain, memory_pattern, replay_iterations=10)

# Note: These are experimental and may change
```

### 7. Configuration Validation

Validate and debug configuration issues:

```python
import pynlm

# Create configuration
config = pynlm.createStandardBrainConfig()

# Validate configuration
errors = {}
if advanced_config.validate(errors):
    print("Configuration is valid")
else:
    print("Configuration validation errors:")
    for key, error in errors.items():
        print(f"  {key}: {error}")

# Export configuration
dict_config = advanced_config.exportToDict()
```

## Migration Guide

### From Legacy to Enhanced

The enhanced bindings maintain full backward compatibility while adding new functionality:

#### Gradual Migration Path

1. **Keep existing code working**: Your current NLM code continues to work unchanged
2. **Add monitoring**: Wrap simulation loops with PerformanceMonitor
3. **Add context management**: Use simulation_context for automatic resource management
4. **Use presets**: Replace manual configuration with createStandardBrainConfig()
5. **Explore experimental**: Try experimental features when comfortable

#### Example Migration

```python
# BEFORE (legacy)
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

# AFTER (enhanced, but backward compatible)
import pynlm

# Option 1: Legacy style (still works)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)

# Option 2: Enhanced with monitoring
config = pynlm.createStandardBrainConfig()
brain = pynlm.createBrain(config)

monitor = pynlm.PerformanceMonitor()
for i in range(100):
    brain.step(i)
    # monitor.recordStep(time.time() - start_time)

# Option 3: Fully enhanced
config = pynlm.createStandardBrainConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

with pynlm.simulation_context(brain, agent, world) as sim:
    for i in range(100):
        sim.simulate()
        analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
```

## Advanced Usage Patterns

### 1. Batch Processing with Context Managers

```python
import pynlm

def run_batch_experiment(num_experiments, steps_per_experiment):
    """Run multiple experiments with proper resource management"""
    results = []
    
    for exp in range(num_experiments):
        # Create fresh setup for each experiment
        config = pynlm.createStandardBrainConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Use context manager for automatic cleanup
        with pynlm.simulation_context(brain, agent, world) as sim:
            monitor = pynlm.PerformanceMonitor()
            
            for step in range(steps_per_experiment):
                sim.simulate()
                monitor.recordStep(0.1)  # Simulate time
                
            stats = sim.getStats()
            perf_stats = monitor.getStats()
            
            results.append({
                'experiment': exp,
                'final_stats': stats,
                'performance': perf_stats
            })
    
    return results

# Run batch experiments
experiments = run_batch_experiment(5, 50)
for result in experiments:
    print(f"Experiment {result['experiment']} completed")
    print(f"  Neurons firing: {result['final_stats']['firing_count']}")
    print(f"  Average firing rate: {result['final_stats']['avg_firing_rate']:.2f} Hz")
```

### 2. Configuration Experiments

```python
import pynlm

def test_configuration_combinations():
    """Test different configuration combinations"""
    configurations = [
        pynlm.createMiniBrainConfig(),
        pynlm.createStandardBrainConfig(),
        pynlm.createAdvancedBrainConfig()
    ]
    
    results = []
    
    for i, config in enumerate(configurations):
        print(f"\nTesting configuration {i+1}: {config}")
        
        errors = {}
        if hasattr(config, 'validate'):
            config.validate(errors)
        
        if errors:
            print(f"  Validation errors: {errors}")
            continue
            
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test performance
        monitor = pynlm.PerformanceMonitor()
        for step in range(50):
            brain.step(step)
            monitor.recordStep(0.1)
            
        perf_stats = monitor.getStats()
        analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
        
        result = {
            'config_type': i+1,
            'neuron_count': brain.getTotalNeuronCount(),
            'performance': perf_stats['steps_per_second'],
            'memory_usage': analysis.get('estimated_memory_kb', 0),
            'analysis': analysis
        }
        
        results.append(result)
        print(f"  Success: {result}")
    
    return results

# Run configuration tests
config_results = test_configuration_combinations()
```

### 3. Performance Profiling

```python
import pynlm
import time

def profile_performance_scaling():
    """Profile performance with different brain sizes"""
    sizes = ['mini', 'standard', 'advanced']
    steps = 100
    
    print("Performance Scaling Profile")
    print("=" * 50)
    
    for size in sizes:
        print(f"\n{size.title()} Brain ({steps} steps):")
        
        # Select appropriate configuration
        if size == 'mini':
            config = pynlm.createMiniBrainConfig()
        elif size == 'standard':
            config = pynlm.createStandardBrainConfig()
        else:
            config = pynlm.createAdvancedBrainConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Profile execution time
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
            
        elapsed = time.time() - start_time
        
        # Profile with enhanced monitor
        monitor = pynlm.PerformanceMonitor()
        for step in range(steps):
            brain.step(step)
            monitor.recordStep(elapsed / steps)
            
        perf_stats = monitor.getStats()
        
        print(f"  Real time: {elapsed:.2f}s")
        print(f"  CPU time: {perf_stats['total_time']:.2f}s")
        print(f"  Steps/sec: {perf_stats['steps_per_second']:.1f}")
        print(f"  Avg step time: {perf_stats['average_step_time_ms']:.2f}ms")
        
        # Network analysis
        analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
        print(f"  Memory estimate: {analysis.get('estimated_memory_kb', 0):.2f} KB")

# Run performance profiling
profile_performance_scaling()
```

## Best Practices

### 1. Use Context Managers

Always use context managers for simulation contexts to ensure proper resource cleanup:

```python
# Recommended
with pynlm.simulation_context(brain, agent, world) as sim:
    for i in range(100):
        sim.simulate()

# Avoid (if you must, ensure cleanup)
brain.initialize()
try:
    for i in range(100):
        brain.step(i)
finally:
    brain.reset()
```

### 2. Monitor Performance

Always monitor performance in production code:

```python
# Production pattern
monitor = pynlm.PerformanceMonitor()

for step in range(1000):
    brain.step(step)
    monitor.recordStep(calculate_elapsed_time())

# Analyze and log performance
stats = monitor.getStats()
if not stats['performance_stable']:
    print("Warning: Performance instability detected")
```

### 3. Validate Configurations

Always validate configurations before use:

```python
# Production pattern
config = pynlm.createStandardBrainConfig()
errors = {}
if not config.validate(errors):
    print("Configuration errors:", errors)
    sys.exit(1)

# Use validated configuration
brain = pynlm.createBrain(config)
```

### 4. Gradual Migration

Migrate gradually to avoid breaking changes:

```python
# Step 1: Add monitoring
monitor = pynlm.PerformanceMonitor()
for i in range(100):
    brain.step(i)
    monitor.recordStep(0.1)

# Step 2: Add context management (if needed)
# Step 3: Use presets
# Step 4: Explore experimental features
```

## Common Issues and Solutions

### Issue 1: Memory Usage Growing

**Problem**: Memory usage increases during simulation

**Solution**: Use context managers and monitor memory usage:

```python
# Before (potential memory leak)
while running:
    brain.step(step)
    # ... simulation code

# After (proper resource management)
with pynlm.simulation_context(brain, agent, world) as sim:
    for step in range(1000):
        sim.simulate()
        analysis = pynlm.VisualizationHelper.analyzeNetwork(brain)
        print(f"Memory: {analysis.get('estimated_memory_kb', 0):.2f} KB")
```

### Issue 2: Performance Degradation

**Problem**: Simulation slows down over time

**Solution**: Monitor and analyze performance:

```python
# Monitor performance and detect issues
monitor = pynlm.PerformanceMonitor()
performance_stable = True

for step in range(1000):
    brain.step(step)
    monitor.recordStep(calculate_elapsed_time())
    
    # Check for performance issues every 100 steps
    if (step + 1) % 100 == 0:
        stats = monitor.getStats()
        if not stats['performance_stable']:
            performance_stable = False
            print(f"Performance issue detected at step {step}")

if not performance_stable:
    print("Performance optimization needed")
```

### Issue 3: Configuration Errors

**Problem**: Invalid configuration causes runtime errors

**Solution**: Validate configurations:

```python
# Validate before use
config = pynlm.createAdvancedBrainConfig()
errors = {}

if not config.validate(errors):
    print("Fix configuration errors:", errors)
    sys.exit(1)

# Safe to use
brain = pynlm.createBrain(config)
```

## Troubleshooting

### Common Error Messages

- "Configuration Error: " - Use ConfigurationError handler
- "Simulation Error: " - Use SimulationError handler  
- "performance_stable: False" - Performance degradation detected
- "Validation errors" - Configuration validation failed

### Getting Help

1. **Documentation**: Read the enhanced documentation
2. **Examples**: Run demo_enhanced_features.py
3. **Community**: Join the NLM community for support
4. **Experimental**: Try experimental features for advanced usage
5. **Performance**: Use performance monitoring to diagnose issues

## Conclusion

The enhanced NLM Python bindings provide:

- **Better resource management** through context managers
- **Enhanced performance monitoring** with detailed analytics
- **Simplified configuration** with preset configurations
- **Advanced error handling** with specific exception types
- **Network analysis tools** for understanding brain properties
- **Experimental features** for advanced users
- **Full backward compatibility** for easy migration
- **Comprehensive documentation** with examples

These improvements make the NLM framework more accessible, robust, and powerful while maintaining compatibility with existing code.