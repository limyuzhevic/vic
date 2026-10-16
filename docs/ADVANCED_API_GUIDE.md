# Advanced NLM Python API Documentation

This document provides comprehensive documentation for the advanced features of the NLM Python API, including new commands, configuration options, and example usage patterns.

## Overview

The NLM Python API has been enhanced with advanced features that provide:

1. **Advanced Configuration Management**: Multiple predefined configuration profiles for different use cases
2. **Batch Processing**: Run multiple simulations in parallel or sequentially
3. **Performance Monitoring**: Built-in tools to monitor and optimize performance
4. **Memory Management**: Memory profiling and analysis tools
5. **Checkpoint Management**: Advanced checkpoint system for simulation recovery
6. **Configuration Comparison**: Compare performance characteristics of different configurations
7. **Development-Focused Features**: Specialized configurations for developmental simulations
8. **Performance-Optimized Features**: Configurations tuned for performance testing

## New Advanced API Functions

### 1. Configuration Factory Functions

#### `createDevelopmentConfig(plasticity_modifier=1.5)`
Creates a development-focused configuration with high plasticity and exploration behavior.

**Parameters:**
- `plasticity_modifier`: Multiplier for plasticity rates (default: 1.5)

**Returns:** `Config` object with development-focused settings

**Use Case:** Simulating brain development and learning processes

#### `createOptimizedConfig(neuronCount=1000, connectionProb=0.1)`
Creates a performance-optimized configuration with minimal plasticity.

**Parameters:**
- `neuronCount`: Number of neurons in the brain (default: 1000)
- `connectionProb`: Connection probability between neurons (default: 0.1)

**Returns:** `Config` object optimized for performance testing

**Use Case:** Performance testing and benchmarking

#### `createQuickTestConfig()`
Creates a minimal configuration for rapid testing and prototyping.

**Returns:** `Config` object with minimal settings for quick testing

**Use Case:** Rapid prototyping and debugging

### 2. Batch Processing Functions

#### `runBatchSimulation(configs, steps, parallel=False)`
Runs multiple simulations with different configurations.

**Parameters:**
- `configs`: List of `Config` objects to test
- `steps`: List of step counts for each configuration
- `parallel`: Whether to run simulations in parallel (default: False)

**Returns:** List of `Phase6IntegrationResult` objects

**Example:**
```python
configs = [createDevelopmentConfig(), createOptimizedConfig()]
steps = [500, 200]
results = runBatchSimulation(configs, steps, parallel=False)
```

### 3. Configuration Comparison

#### `compareConfigs(configs)`
Compares multiple configurations and returns performance metrics.

**Parameters:**
- `configs`: List of `Config` objects to compare

**Returns:** Dictionary with performance metrics for each configuration

**Example:**
```python
configs = [createQuickTestConfig(), createDevelopmentConfig()]
comparison = compareConfigs(configs)
print(f"Quick test neurons: {comparison['Config_0']['total_neurons']}")
print(f"Development neurons: {comparison['Config_1']['total_neurons']}")
```

### 4. Advanced Tools

#### `createCheckpointManager(basePath, maxCheckpoints=10)`
Creates a checkpoint manager for advanced checkpointing.

**Parameters:**
- `basePath`: Base directory for checkpoint files
- `maxCheckpoints`: Maximum number of checkpoints to retain (default: 10)

**Returns:** Checkpoint manager object

#### `createMemoryProfiler()`
Creates a memory profiler for performance analysis.

**Returns:** Memory profiler object

#### `createPerformanceMonitor()`
Creates a performance monitor for simulation monitoring.

**Returns:** Performance monitor object

## Configuration Profiles

### Development Configuration

The `createDevelopmentConfig()` function creates a configuration optimized for developmental simulations:

**Key Features:**
- High plasticity rates
- Curiosity-driven exploration
- Novelty detection enabled
- Developmental stages enabled
- Focus on learning and adaptation

**Typical Use Cases:**
- Simulating brain development
- Learning from experience
- Studying curiosity and exploration behavior
- Developmental plasticity studies

### Performance Configuration

The `createOptimizedConfig()` function creates a configuration optimized for performance testing:

**Key Features:**
- Minimal Hebbian plasticity (disabled)
- Minimal structural plasticity (disabled)
- Curiosity disabled
- Focused on computational efficiency

**Typical Use Cases:**
- Benchmarking and performance testing
- Large-scale simulations
- Resource-constrained environments

### Quick Test Configuration

The `createQuickTestConfig()` function creates a minimal configuration for rapid testing:

**Key Features:**
- Minimal neuron count (50)
- Disabled plasticity subsystems
- Disabled neuromodulation
- Fastest possible initialization

**Typical Use Cases:**
- Rapid prototyping
- Debugging and testing
- Small-scale demonstrations

## Example Workflows

### 1. Performance Optimization Study

```python
import pynlm
import time

print("=== Performance Optimization Study ===")

# Create configurations for testing
configs = [
    pynlm.createQuickTestConfig(),
    pynlm.createOptimizedConfig(neuronCount=500, connectionProb=0.08),
    pynlm.createOptimizedConfig(neuronCount=1000, connectionProb=0.12),
    pynlm.createDevelopmentConfig()
]

# Measure performance
print("Testing performance with different configurations:")
start_time = time.time()

for i, config in enumerate(configs):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(200):
        brain.step(step)
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"  Config {i}: {config.getOr('brain.neuron_count', 0)} neurons, "
          f"{duration:.3f}s, firing rate: {brain.getAverageFiringRate():.4f}")

print("Performance study completed!")
```

### 2. Development vs Performance Comparison

```python
import pynlm

print("=== Development vs Performance Comparison ===")

# Create two configurations for comparison
dev_config = pynlm.createDevelopmentConfig()
perf_config = pynlm.createOptimizedConfig(neuronCount=800)

# Compare configurations
comparison = pynlm.compareConfigs([dev_config, perf_config])

print("Configuration Comparison Results:")
for config_name, metrics in comparison.items():
    print(f"\n{config_name}:")
    print(f"  Neurons: {metrics['total_neurons']}")
    print(f"  Synapses: {metrics['total_synapses']}")
    print(f"  Firing Rate: {metrics['firing_rate']:.4f}")
    print(f"  Total Spikes: {metrics['total_spikes']}")
    print(f"  E/I Ratio: {metrics['excitatory_inhibition_ratio']:.4f}")

# Determine which configuration is better for the task
if comparison['Config_0']['firing_rate'] > comparison['Config_1']['firing_rate']:
    print("\nDevelopment configuration produces higher firing rates - better for learning")
else:
    print("\nPerformance configuration produces higher firing rates - better for stability")
```

### 3. Advanced Agent Simulation

```python
import pynlm

print("=== Advanced Agent Simulation ===")

# Create development-focused agent
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Running advanced agent simulation with full subsystems...")
print("Enabled: Reward Modulation, Structural Plasticity, Development, Curiosity")

# Run simulation
for step in range(300):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"  Step {step}: Firing: {brain.getFiringNeuronCount()}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}")

print("Advanced simulation completed!")
print(f"Final stats - Neurons: {brain.getTotalNeuronCount()}, "
      f"Firing Rate: {brain.getAverageFiringRate():.4f}, "
      f"Development Stage: {brain.getDevelopmentalStage()}")
```

### 4. Batch Processing for Parameter Studies

```python
import pynlm

print("=== Parameter Study with Batch Processing ===")

# Study the effect of different connection probabilities
base_config = pynlm.createOptimizedConfig(neuronCount=500)
connection_probs = [0.05, 0.1, 0.15, 0.2]
steps = [100] * len(connection_probs)

configs = []
for prob in connection_probs:
    config = pynlm.createOptimizedConfig(neuronCount=500, connectionProb=prob)
    configs.append(config)

print(f"Running {len(configs)} simulations to study connection probability effects...")
results = pynlm.runBatchSimulation(configs, steps, parallel=False)

print("\nParameter Study Results:")
for i, (prob, result) in enumerate(zip(connection_probs, results)):
    print(f"  Connection prob {prob:.2f}: Reward = {result.totalReward:.2f}, "
          f"Firing Rate = {result.avgFiringRate:.4f}")
```

## Error Handling and Best Practices

### Common Issues and Solutions

1. **Configuration Errors**
   - **Issue:** Invalid configuration parameters
   - **Solution:** Use factory functions like `createOptimizedConfig()` for validated configurations

2. **Memory Issues**
   - **Issue:** Memory usage too high for large simulations
   - **Solution:** Use `createOptimizedConfig()` for performance-optimized configurations or increase `maxCheckpoints` in checkpoint manager

3. **Performance Issues**
   - **Issue:** Simulations taking too long
   - **Solution:** Use `createQuickTestConfig()` for rapid testing or `createOptimizedConfig()` for performance

### Best Practices

1. **For Development Research:**
   - Use `createDevelopmentConfig()`
   - Enable all plasticity and neuromodulation subsystems
   - Use moderate to high neuron counts

2. **For Performance Testing:**
   - Use `createOptimizedConfig()`
   - Test with different neuron counts
   - Use batch processing for multiple configurations

3. **For Prototyping:**
   - Use `createQuickTestConfig()`
   - Test concepts quickly
   - Scale up to full configurations when ready

4. **For Large-Scale Studies:**
   - Use batch processing with `runBatchSimulation()`
   - Monitor performance with `createPerformanceMonitor()`
   - Use checkpointing for long simulations

## Performance Tips

1. **Memory Management:**
   - Monitor memory usage with `createMemoryProfiler()`
   - Use checkpointing for long simulations
   - Clean up references to large objects when no longer needed

2. **Performance Optimization:**
   - Use `createOptimizedConfig()` for performance-critical applications
   - Disable unnecessary subsystems
   - Use batch processing for multiple related simulations

3. **Simulation Control:**
   - Use appropriate configuration for your use case
   - Monitor simulation progress and adjust parameters as needed
   - Use checkpointing for fault tolerance

## Advanced Features Summary

| Feature | Function | Primary Use Case |
|---------|----------|------------------|
| Development Config | `createDevelopmentConfig()` | Developmental research |
| Performance Config | `createOptimizedConfig()` | Performance testing |
| Quick Test Config | `createQuickTestConfig()` | Prototyping |
| Batch Processing | `runBatchSimulation()` | Parameter studies |
| Comparison | `compareConfigs()` | Configuration analysis |
| Checkpointing | `createCheckpointManager()` | Long simulations |
| Memory Profiling | `createMemoryProfiler()` | Memory analysis |
| Performance Monitoring | `createPerformanceMonitor()` | Performance tracking |

## Getting Started

1. **Install the enhanced Python bindings:**
   ```bash
   pip install -e .
   ```

2. **Run the advanced examples:**
   ```bash
   python examples/advanced_api_examples.py
   ```

3. **Explore configuration options:**
   ```python
   import pynlm
   
   # Create different configuration types
   dev_config = pynlm.createDevelopmentConfig()
   perf_config = pynlm.createOptimizedConfig()
   quick_config = pynlm.createQuickTestConfig()
   ```

4. **Run batch simulations:**
   ```python
   configs = [dev_config, perf_config, quick_config]
   steps = [500, 200, 100]
   results = pynlm.runBatchSimulation(configs, steps)
   ```

## Troubleshooting

### Common Issues

1. **Import Error: No module named 'pynlm'**
   - **Solution:** Ensure the Python bindings are installed correctly
   ```bash
   pip install -e .
   ```

2. **Configuration Error: Invalid parameter**
   - **Solution:** Use factory functions that provide validated configurations
   ```python
   config = pynlm.createOptimizedConfig(neuronCount=1000)
   ```

3. **Memory Error: Not enough memory**
   - **Solution:** Use performance-optimized configurations or reduce simulation size
   ```python
   config = pynlm.createOptimizedConfig(neuronCount=500)
   ```

4. **Performance Error: Simulation taking too long**
   - **Solution:** Use quick test configuration or optimize parameters
   ```python
   config = pynlm.createQuickTestConfig()
   ```

## Conclusion

The enhanced NLM Python API provides a comprehensive set of tools for advanced neural network simulation, including:

1. **Flexible configuration management** with predefined profiles for different use cases
2. **Batch processing** capabilities for parameter studies and comparative analysis
3. **Performance monitoring** tools for optimization and analysis
4. **Memory management** tools for large-scale simulations
5. **Checkpoint management** for fault tolerance and long-running simulations
6. **Configuration comparison** tools for analysis and decision-making

These features make the NLM Python API suitable for a wide range of applications, from rapid prototyping to large-scale research studies, providing users with powerful tools for neural network simulation and analysis.
