# NLM - Enhanced Usage Guide

## Overview

This document describes the enhanced NLM (Neural Learning Machine) Python API with advanced features for power users and researchers.

## Quick Start - Enhanced Features

### 1. Advanced Brain Creation with Context Manager

```python
import pynlm

# Using context manager for automatic cleanup
with pynlm.createPythonBrain() as brain:
    brain.initialize()
    for step in range(1000):
        brain.step(step)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
# Brain is automatically cleaned up here
```

### 2. Enhanced Configuration

```python
import pynlm

# Create configuration from dictionary
config = pynlm.PythonConfig()
config.loadFromDict({
    "neuron_count": 1000,
    "timestep": 0.001,
    "connection_probability": 0.1,
    "enable_development": True,
    "enable_curiosity": True
})

brain = pynlm.createPythonBrain(config)
brain.initialize()
```

### 3. Batch Simulation

```python
import pynlm

# Run multiple simulations
batch_config = {
    "neuron_count": 500,
    "timestep": 0.001,
    "experiment_type": "plasticity"
}

results = pynlm.runBatchSimulation(batch_config, num_simulations=10, 
                                   steps_per_simulation=1000, 
                                   experiment_type="plasticity")

print(f"Completed {len(results)} simulations")
for result in results:
    print(f"  Simulation {result['simulation_id']}: {result['final_spike_count']} spikes")
```

### 4. Brain Comparison and Analysis

```python
import pynlm

# Compare multiple brain simulations
brain1 = pynlm.createPythonBrain()
brain2 = pynlm.createPythonBrain()
brain3 = pynlm.createPythonBrain()

comparison = pynlm.compareBrains([brain1, brain2, brain3])

print(f"Average spike count: {comparison['avg_spike_count']:.2f}")
print(f"Best brain: {comparison['best_brain_id']}")
print(f"Performance score: {comparison['best_score']:.4f}")
```

### 5. Performance Profiling

```python
import pynlm

# Profile brain execution
brain = pynlm.createPythonBrain()
brain.initialize()

benchmark = pynlm.benchmarkBrain(brain, iterations=100)
print(f"Average execution time: {benchmark['average_time_ms']:.2f} ms")
print(f"Operations per second: {benchmark['operations_per_second']:.2f}")
```

### 6. Advanced Spike Analysis

```python
import pynlm

# Analyze spike train
spike_counts = [10, 15, 20, 25, 30, 35, 40, 45, 50, 55]
analysis = pynlm.analyzeSpikeTrain(spike_counts, time_window=10.0)

print(f"Mean spikes per sample: {analysis['mean_spikes']:.2f}")
print(f"Standard deviation: {analysis['std_deviation']:.2f}")
print(f"Burst rate: {analysis['burst_rate']:.2f} bursts/second")
```

## Advanced Features

### Enhanced Configuration Management

```python
import pynlm

# Create configuration template
template = pynlm.getDefaultConfigTemplate()
print("Default configuration:", template)

# Merge configurations
base_config = {"neuron_count": 500, "timestep": 0.001}
extended_config = {"enable_development": True, "enable_curiosity": True}
merged = pynlm.mergeConfigs(base_config, extended_config)
print("Merged configuration:", merged)

# Validate configuration
validation = pynlm.validateConfig({"neuron_count": 1000, "timestep": 0.001})
if validation["valid"]:
    print("Configuration is valid!")
else:
    print("Configuration errors:", validation["errors"])
    print("Configuration warnings:", validation["warnings"])
```

### Research Export

```python
import pynlm

# Export brain data for research
brain = pynlm.createPythonBrain()
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Export data
result = pynlm.exportBrainData(brain, "brain_research_data.txt")
print(result)
```

### Advanced Debugging

```python
import pynlm

# Debug brain state
brain = pynlm.createPythonBrain()
brain.initialize()
debug_info = pynlm.debugBrainState(brain)
print("Debug information:", debug_info)
```

## Context Managers

### Brain Context Manager

```python
import pynlm

# Automatic brain cleanup
with pynlm.createPythonBrain() as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
    
# Brain automatically reset and cleaned up
print("Brain context manager completed")
```

### World Context Manager

```python
import pynlm

# Automatic world cleanup
with pynlm.createPythonWorld() as world:
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Simulation code here
    for step in range(100):
        world.update(0.1)
    
# World automatically reset and cleaned up
print("World context manager completed")
```

## Performance Optimization

### Profile and Optimize

```python
import pynlm

# Profile and optimize brain performance
brain = pynlm.createPythonBrain()

# Get optimization suggestions
optimization_params = {
    "neuron_reduction": 0.8,  # Reduce neurons by 20%
    "timestep_adjustment": 0.002,  # Increase timestep
    "plasticity_disabled": True  # Disable plasticity for speed
}

optimization = pynlm.optimizeForPerformance(brain, optimization_params)
print("Optimization applied:", optimization["applied_optimizations"])
print("Estimated speedup:", optimization["estimated_speedup"])
```

## Spike Train Analysis

### Burst Detection and Analysis

```python
import pynlm

# Advanced spike train analysis
spike_counts = [5, 8, 12, 18, 25, 35, 45, 55, 65, 75, 80]

# Analyze for bursts and patterns
analysis = pynlm.analyzeSpikeTrain(spike_counts, time_window=5.0)

print("=== Spike Train Analysis ===")
print(f"Total samples: {len(spike_counts)}")
print(f"Time window: {analysis['time_window']} seconds")
print(f"Average spikes per sample: {analysis['mean_spikes']:.2f}")
print(f"Standard deviation: {analysis['std_deviation']:.2f}")
print(f"Coefficient of variation: {analysis['coefficient_of_variation']:.3f}")
print(f"Burst count: {analysis['burst_count']}")
print(f"Burst rate: {analysis['burst_rate']:.2f} bursts/second")
```

## Research Export

### Export Data for Research

```python
import pynlm

# Prepare research data
research_data = {
    "metadata": {
        "experiment_type": "plasticity_learning",
        "neuron_count": 1000,
        "timestep": 0.001,
        "date": "2024-01-01",
        "researcher": "Dr. Smith"
    },
    "results": [
        {
            "simulation_id": 0,
            "final_spike_count": 5420,
            "final_firing_rate": 54.2,
            "e_i_ratio": 0.85,
            "developmental_stage": 3
        },
        # ... more results
    ]
}

# Export for research
export_result = pynlm.exportForResearchPaper(research_data, "Neural Plasticity Study")
print("Export prepared:", export_result)
```

## Development Tools

### Testing and Validation

```python
import pynlm

# Create test configuration for development
test_config = pynlm.createTestConfig(neuron_count=100, timestep=0.001)
print("Test configuration created:", test_config.getKeys())

# Benchmark configuration
benchmark_result = pynlm.benchmarkConfiguration(test_config, iterations=50)
print(f"Benchmark completed: {benchmark_result['total_time_ms']:.2f} ms total")
print(f"Average per iteration: {benchmark_result['average_time_ms']:.2f} ms")
```

### Error Handling

```python
import pynlm

# Set custom error handler
def custom_error_handler(error):
    print(f"Custom error handler: {error}")
    # Implement custom error handling logic

pynlm.setErrorHandler(custom_error_handler)

# Use the brain with custom error handling
brain = pynlm.createPythonBrain()
try:
    brain.initialize()
    brain.step(0)
except Exception as e:
    print(f"Error caught: {e}")
```

## Advanced APIs

### Multi-Region Simulation

```python
import pynlm

# Create multi-region simulation configuration
multi_region_config = pynlm.createMultiRegionSimulation(
    num_regions=3,
    neurons_per_region=333,
    connection_prob=0.1
)

print("Multi-region configuration:", multi_region_config)
```

### Configuration Difference Analysis

```python
import pynlm

# Compare two configurations
config1 = {"neuron_count": 500, "timestep": 0.001, "enable_development": True}
config2 = {"neuron_count": 1000, "timestep": 0.001, "enable_curiosity": True}

differences = pynlm.getConfigDifference(config1, config2)
print("Configuration differences:", differences)
```

## Integration with Existing Features

### Backward Compatibility

```python
import pynlm

# All existing code still works (backward compatibility)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)

print("Backward compatibility maintained")
```

### Enhanced API Usage

```python
import pynlm

# Enhanced API with all new features
brain = pynlm.createPythonBrain()
brain.initialize()

# Use enhanced methods
stats = brain.getNeuralStats()
print("Brain stats:", stats)

spike_stats = brain.getSpikeStats(time_window=5.0)
print("Spike stats:", spike_stats)

# Save brain state
brain.save("enhanced_brain_state.bin")
print("Brain state saved")
```

## Help and Documentation

### Get Help

```python
import pynlm

# Get comprehensive help
documentation = pynlm.getPythonAPIDoc()
print(documentation)

# Get library information
library_info = pynlm.getLibraryInfo()
print("Library info:", library_info)

# Show advanced features
pynlm.showAdvancedFeatures()
```

### About the Enhanced API

```python
import pynlm

# Get information about enhanced API
about_info = pynlm.about()
print(about_info)
```

## Conclusion

The enhanced NLM Python API provides:

1. **Context managers** for automatic resource cleanup
2. **Batch simulation** utilities for comparative analysis
3. **Brain comparison** and analysis tools
4. **Performance profiling** and benchmarking
5. **Configuration validation** and merging
6. **Research export** capabilities
7. **Advanced error handling** and debugging tools
8. **Configuration templates** and utilities
9. **Spike analysis** and burst detection
10. **Performance optimization** tools

All enhancements maintain backward compatibility with existing code while providing powerful new features for advanced users and researchers.

For more information, visit: https://github.com/nlm-project/nlm
