# Example 1: Basic Brain Creation and Initialization
# ============================================

## Overview

This example demonstrates the simplest way to create and initialize a brain using the NLM (Neural Learning Machine) Python API. It covers fundamental NLM concepts including configuration management, brain creation, initialization, and basic simulation.

## Learning Objectives

- Understand the basic brain lifecycle (create, initialize, simulate)
- Learn how to customize brain configuration
- Understand basic brain statistics and metrics
- Learn how to run and control simulations
- Experience NLM API usage patterns

## Code Example

```python
#!/usr/bin/env python3
"""
Example 1: Basic Brain Creation and Initialization
====================================================

This example demonstrates the simplest way to create and initialize a brain
using the NLM Python API.
"""

import pynlm
import time

def basic_brain_example():
    """Create a basic brain and run a simple simulation."""
    print("=== Basic Brain Creation Example ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Optionally customize the configuration
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    
    # Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run a simulation
    for step in range(100):
        brain.step(step)
        
        # Log statistics periodically
        if step % 25 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("Simulation completed successfully!")
    return brain

if __name__ == "__main__":
    basic_brain_example()
```

## Key Concepts

### 1. Configuration Management

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Customize configuration parameters
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.15)

# Configuration key patterns:
# - brain.neuron_count: Number of neurons
# - brain.synapse_density: Connection probability
# - plasticity.stdp.learning_rate: Learning rate
# - environment.width: Environment width
```

### 2. Brain Creation and Initialization

```python
import pynlm

# Create brain from configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain with neurons and connections
brain.initialize()

# Brain attributes:
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Synapse count: {brain.getTotalSynapseCount()}")
```

### 3. Simulation Loop

```python
# Run simulation for specified number of steps
for step in range(100):
    brain.step(step)
    
    # Access brain state and statistics
    firing_count = brain.getFiringNeuronCount()
    firing_rate = brain.getAverageFiringRate()
    dev_stage = brain.getDevelopmentalStage()
```

### 4. Statistics Collection

```python
# Basic brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage().name}")
print(f"Plasticity rate: {brain.getSynapticPlasticityRate():.3f}")
```

## Expected Output

```
=== Basic Brain Creation Example ===
Created brain with 500 neurons
Total synapses: 75000
Step 0: 50 firing neurons
Step 25: 60 firing neurons
Step 50: 55 firing neurons
Step 75: 58 firing neurons
Simulation completed successfully!
```

## Practical Applications

### Quick Testing

```python
def quick_test():
    """Quick test of NLM basic functionality."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run short simulation
    for step in range(10):
        brain.step(step)
    
    print(f"Quick test passed: {brain.getTotalNeuronCount()} neurons")
    return brain
```

### Custom Configuration

```python
def custom_configuration(neuron_count, synapse_density, steps):
    """Create brain with custom configuration."""
    config = pynlm.createDefaultConfig()
    
    # Apply custom parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", synapse_density)
    
    # Optional: Customize other parameters
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("environment.width", 10)
    config.set("environment.height", 10)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        if step % 50 == 0:
            print(f"Progress: Step {step}/{steps}")
    
    return brain
```

## Error Handling

```python
import pynlm

def safe_brain_creation(config_file=None):
    """Safely create a brain with error handling."""
    try:
        config = pynlm.createDefaultConfig()
        
        # Validate configuration
        if config_file:
            config.load(config_file)
        
        # Check critical parameters
        neuron_count = config.get("brain.neuron_count", 0)
        if neuron_count <= 0:
            raise ValueError("Neuron count must be positive")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        return None
    except pynlm.InitializationError as e:
        print(f"Initialization error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None
```

## Performance Considerations

### Memory Usage

- **Small brains (100-500 neurons)**: Minimal memory usage, fast execution
- **Medium brains (1000-5000 neurons)**: Moderate memory usage, good for testing
- **Large brains (5000+ neurons)**: High memory usage, use for research

### Simulation Speed

```python
def benchmark_performance(neuron_counts):
    """Benchmark performance for different neuron counts."""
    import time
    
    results = {}
    
    for neuron_count in neuron_counts:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        elapsed = end_time - start_time
        steps_per_second = 100 / elapsed
        
        results[neuron_count] = {
            'steps_per_second': steps_per_second,
            'elapsed_time': elapsed,
            'neuron_count': neuron_count
        }
        
        print(f"Neuron count: {neuron_count:4d}, Speed: {steps_per_second:6.1f} steps/sec")
    
    return results
```

## Extensions and Customization

### 1. Different Simulation Parameters

```python
def simulation_variations():
    """Test different simulation parameters."""
    variations = [
        {'neuron_count': 100, 'steps': 50},
        {'neuron_count': 500, 'steps': 100},
        {'neuron_count': 1000, 'steps': 200},
        {'neuron_count': 2000, 'steps': 300}
    ]
    
    for var in variations:
        print(f"\nTesting: {var['neuron_count']} neurons, {var['steps']} steps")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", var['neuron_count'])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(var['steps']):
            brain.step(step)
        
        print(f"  Result: {brain.getAverageFiringRate():.3f} Hz firing rate")
```

### 2. Custom Statistics Collection

```python
def advanced_statistics(brain, steps):
    """Collect advanced statistics during simulation."""
    stats = {
        'firing_rate_history': [],
        'neuron_counts': [],
        'synapse_counts': [],
        'development_stages': [],
        'plasticity_rates': []
    }
    
    for step in range(steps):
        brain.step(step)
        
        # Collect statistics
        stats['firing_rate_history'].append(brain.getAverageFiringRate())
        stats['neuron_counts'].append(brain.getTotalNeuronCount())
        stats['synapse_counts'].append(brain.getTotalSynapseCount())
        stats['development_stages'].append(brain.getDevelopmentalStage().value)
        stats['plasticity_rates'].append(brain.getSynapticPlasticityRate())
        
        # Log progress
        if step % 50 == 0:
            print(f"Step {step}: {stats['firing_rate_history'][-1]:.3f} Hz")
    
    return stats
```

### 3. Visualization Setup

```python
def setup_visualization(brain, steps):
    """Setup for visualization (example structure)."""
    # This would integrate with visualization libraries
    # For now, just return data for plotting
    
    time_series = []
    for step in range(steps):
        brain.step(step)
        time_series.append({
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'neuron_count': brain.getTotalNeuronCount(),
            'development_stage': brain.getDevelopmentalStage().value
        })
    
    return time_series
```

## Comparison with Other Examples

### Example 1 vs Example 2

| Feature | Example 1 | Example 2 |
|---------|-----------|-----------|
| **Complexity** | Basic | Advanced |
| **Components** | Brain only | Brain + Agent + World |
| **Learning** | Basic simulation | Complete agent behavior |
| **Time Required** | 1-2 minutes | 3-5 minutes |

### Example 1 vs Example 5

| Feature | Example 1 | Example 5 |
|---------|-----------|-----------|
| **Focus** | Basic functionality | Performance benchmarking |
| **Metrics** | Basic statistics | Advanced performance metrics |
| **Complexity** | Simple | Advanced |
| **Use Case** | Learning NLM basics | Optimizing performance |

## Troubleshooting

### Common Issues

1. **Brain Creation Fails**
   - **Problem**: Brain initialization fails
   - **Solution**: Check configuration parameters, use valid neuron counts
   - **Command**: Ensure config has positive neuron count

2. **Memory Issues**
   - **Problem**: High memory usage with large brain
   - **Solution**: Use smaller configurations or increase available memory
   - **Command**: Use lightweight configuration for testing

3. **Simulation Too Slow**
   - **Problem**: Simulation takes too long
   - **Solution**: Reduce neuron count or simulation steps
   - **Command**: Use smaller configuration for quick testing

### Debugging Tips

```python
import pynlm
import time
import psutil

def debug_brain_creation():
    """Debug brain creation issues."""
    print("=== Brain Creation Debug ===")
    
    # Check available memory
    memory = psutil.virtual_memory()
    print(f"Available memory: {memory.available / 1024 / 1024:.1f} MB")
    
    # Try with different configurations
    configs = [
        {'neuron_count': 100},
        {'neuron_count': 500},
        {'neuron_count': 1000}
    ]
    
    for i, params in enumerate(configs):
        print(f"\nAttempt {i+1}: {params['neuron_count']} neurons")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params['neuron_count'])
        
        try:
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            print(f"  Success: {brain.getTotalNeuronCount()} neurons")
            return brain
            
        except Exception as e:
            print(f"  Failed: {e}")
    
    return None
```

## Conclusion

Example 1 provides a foundation for understanding NLM basics. By working through this example, you can:

1. **Understand Core Concepts**: Brain creation, configuration, simulation
2. **Learn API Usage**: pynlm.createDefaultConfig(), pynlm.createBrain()
3. **Practice Skills**: Parameter setting, statistics collection
4. **Explore Extensions**: Custom configurations, performance tuning
5. **Build Foundation**: Prepare for more complex examples

This example serves as an entry point to the NLM ecosystem, introducing key concepts and patterns that are used throughout the examples and real-world NLM applications.

---

*This example is part of the NLM (Neural Learning Machine) examples suite.
For additional examples, documentation, and support, visit the NLM project.*
