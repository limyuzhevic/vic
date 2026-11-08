"""
Basic NLM usage examples for the pynlm package.

This module provides simple examples of how to use the NLM (Neural Learning Machine) 
Python API for basic tasks like brain creation, initialization, and simulation.

## Examples

### 1. Simple Brain Creation and Simulation
Create a brain with default configuration and run a basic simulation.

```python
from pynlm.examples import basic_brain_example

# Run the example
sim_stats = basic_brain_example()
print(f"Total spikes: {sim_stats['total_spikes']}")
print(f"Average firing rate: {sim_stats['avg_firing_rate']:.3f} Hz")
```

### 2. Learning and Plasticity Example
Demonstrate STDP, Hebbian, and structural plasticity mechanisms.

```python
from pynlm.examples import learning_plasticity_example

# Run the learning example
learning_stats = learning_plasticity_example()
print(f"Initial mean weight: {learning_stats['initial_mean_weight']:.4f}")
print(f"Final mean weight: {learning_stats['final_mean_weight']:.4f}")
print(f"Learning detected: {learning_stats['learning_detected']}")
```

### 3. Agent in World Example
Complete agent-world interaction setup with sensory processing and motor output.

```python
from pynlm.examples import agent_world_example

# Run the agent-world example
agent_stats = agent_world_example()
print(f"Total spikes: {agent_stats['total_spikes']}")
print(f"Curiosity level: {agent_stats['curiosity_level']:.3f}")
print(f"Development stage: {agent_stats['development_stage']}")
```

## Usage

### Import Examples

```python
# Import specific examples
from pynlm.examples import basic_brain_example
from pynlm.examples import learning_plasticity_example
from pynlm.examples import agent_world_example

# Import all examples
import pynlm.examples
```

### Run Examples

```python
# Run a specific example
basic_stats = basic_brain_example()

# Run all examples
all_stats = pyn大于extExamples.run_all_examples()
```

### Customize Examples

```python
from pynlm.examples import basic_brain_example

# Customize parameters and run
stats = basic_brain_example(
    neuron_count=2000,
    steps=2000,
    plasticity_enabled=True,
    development_enabled=True
)
```

## API Reference

### basic_brain_example

Creates a simple brain and runs a basic simulation.

#### Parameters
- `neuron_count`: Number of neurons (default: 1000)
- `steps`: Number of simulation steps (default: 1000)
- `plasticity_enabled`: Whether to enable plasticity (default: True)
- `development_enabled`: Whether to enable development (default: False)

#### Returns
- Dictionary with simulation statistics:
  - `total_spikes`: Total number党员 spikes
  - `firing_neurons`: Number of firing neurons at final step
  - `avg_firing_rate`: Average firing rate in Hz
  - `excitation_inhibition_ratio`: E/I balance ratio
  - `development_stage`: Final developmental stage

#### Example Usage
```python
from pynlm.examples import basic_brain_example

stats = basic_brain_example(neuron_count=500, steps=500)
print(f"Simulation complete with {stats['total_spikes']} spikes")
```

### learning_plasticity_example

Demonstrates different plasticity mechanisms and learning.

#### Parameters
- `steps`: Number of simulation steps (default: 2000)
- `show_detailed_stats`: Whether to show detailed learning statistics (default: True)

#### Returns
- Dictionary with learning statistics:
  - `initial_mean_weight`: Initial average synaptic weight
  - `final_mean_weight`: Final average synaptic weight
  - `weight_change`: Absolute weight change
  - `learning_detected`: Whether significant learning occurred
  - `stdp_active`: Whether STDP plasticity is active
  - `hebbian_active`: Whether Hebbian plasticity is active
  - `structural_active`: Whether structural plasticity is active

#### Example Usage
```python
from pynlm.examples import learning_plasticity_example

stats = learning_plasticity_example(steps=3000, show_detailed_stats=True)
print(f"Learning detected: {stats['learning_detected']}")
print(f"Initial weight: {stats['initial_mean_weight']:.4f}")
print(f"Final weight: {stats['final_mean_weight']:.4f}")
```

### agent_world_example

Complete agent-world interaction with sensory processing and motor output.

#### Parameters
- `world_width`: World width (default: 20)
- `world_height`: World height (default: 20)
- `vision_width`: Vision width (default: 8)
- `vision_height`: Vision height (default: 8)
- `steps`: Number of simulation steps (default: 1000)
- `enable_reward`: Whether to enable reward modulation (default: True)
- `enable_curiosity`: Whether to enable curiosity (default: True)

#### Returns
- Dictionary with agent statistics:
  - `total_spikes`: Total spikes in brain
  - `firing_neurons`: Number of firing neurons
  - `curiosity_level`: Current curiosity level
  - `novelty_level`: Current novelty level
  - `development_stage`: Current development stage
  - `neuromodulation_level`: Current neuromodulation level
  - `prediction_error`: Current prediction error

#### Example Usage
```python
from pynlm.examples import agent_world_example

stats = agent_world_example(
    world_width=30,
    world_height=30,
    vision_width=10,
    vision_height=10,
    steps=2000,
    enable_reward=True,
    enable_curiosity=True
)
print(f"Agent simulation complete with {stats['total_spikes']} spikes")
print(f"Curiosity level: {stats['curiosity_level']:.3f}")
print(f"Development stage: {stats['development_stage']}")
```

## Advanced Example Features

### 1. Custom Configuration

```python
from pynlm.examples import basic_brain_example
from pynlm import createDefaultConfig

# Create custom configuration
config = createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("development.enabled", True)
config.set("neuromod.curiosity.enable", True)

# Run with custom configuration (if supported by example)
```

### 2. Error Handling

```python
from pynlm.examples import basic_brain_example, learning_plasticity_example, agent_world_example

def safe_run_example(example_func, **kwargs):
    try:
        return example_func(**kwargs)
    except Exception as e:
        print(f"Error running {example_func.__name__}: {e}")
        return None

# Run examples with error handling
basic_stats = safe_run_example(basic_brain_example, steps=1000)
learning_stats = safe_run_example(learning_plasticity_example, steps=2000)
agent_stats = safe_run_example(agent_world_example, steps=1000)
```

### 3. Performance Monitoring

```python
import time
from pynlm.examples import basic_brain_example

# Monitor performance of examples
def run_with_monitoring(example_func, example_name, **kwargs):
    start_time = time.time()
    result = example_func(**kwargs)
    end_time = time.time()
    
    execution_time = end_time - start_time
    print(f"{example_name} completed in {execution_time:.2f} seconds")
    
    return result, execution_time

# Run with monitoring
basic_stats, basic_time = run_with_monitoring(
    basic_brain_example, "Basic Brain Example", steps=1000
)
```

## Troubleshooting

### Common Issues

1. **Import errors**
   ```
   Error: cannot import 'pynlm.examples'
   
   Solution: Install pynlm properly:
   pip install -e .
   ```

2. **Simulation errors**
   ```
   Error: Brain initialization failed
   
   Solution: Check if examples call brain.initialize():
   Ensure all examples follow the pattern:
   1. Create brain
   2. Call brain.initialize()
   3. Run simulation
   ```

3. **Memory issues**
   ```
   Error: Memory error during simulation
   
   Solution: Reduce parameters:
   - Lower neuron_count
   - Reduce steps
   - Disable development
   ```

### Debug Examples

```python
from pynlm.examples import basic_brain_example

# Debug basic example step by step
print("Running basic brain example step by step...")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
print(f"1. Brain created with {brain.getTotalNeuronCount()} neurons")

brain.initialize()
print(f"2. Brain initialized")

for step in range(10):
    brain.step(step)
    print(f"3. Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print(f"4. Total spikes: {brain.getTotalSpikeCount()}")
```

## Example Variations

### 1. Minimal Example

```python
from pynlm import createBrain, createDefaultConfig

# Minimal example - just create and run one step
config = createDefaultConfig()
brain = createBrain(config)
brain.initialize()
brain.step(0)

print("Minimal example completed")
```

### 2. Extended Example

```python
from pynlm.examples import agent_world_example

# Extended example with custom world settings
def extended_example():
    stats = agent_world_example(
        world_width=50,
        world_height=50,
        vision_width=15,
        vision_height=15,
        steps=3000,
        enable_reward=True,
        enable_curiosity=True
    )
    
    # Add custom analysis
    if stats['total_spikes'] > 10000:
        print("High spike count detected - analysis needed")
    
    return stats

extended_stats = extended_example()
```

### 3. Comparative Example

```python
from pynlm.examples import basic_brain_example, learning_plasticity_example

# Run comparative example
def comparative_example():
    print("Running comparative example...")
    
    # Basic simulation
    basic_stats = basic_brain_example(steps=500)
    
    # Learning simulation
    learning_stats = learning_plasticity_example(steps=500)
    
    # Compare results
    print("\nComparison:")
    print(f"Basic: {basic_stats['total_spikes']} spikes")
    print(f"Learning: {learning_stats['total_spikes']} spikes")
    print(f"Learning detected: {learning_stats['learning_detected']}")
    
    return basic_stats, learning_stats

comparative_results = comparative_example()
```

## Best Practices

### 1. Configuration Management

```python
# Use predefined configurations
from pynlm.examples import basic_brain_example

# Fast configuration (fewer neurons)
fast_stats = basic_brain_example(
    neuron_count=100,
    steps=100,
    development_enabled=False
)

# Detailed configuration (more neurons)
detailed_stats = basic_brain_example(
    neuron_count=2000,
    steps=2000,
    development_enabled=True
)
```

### 2. Error Recovery

```python
from pynlm.examples import basic_brain_example

def robust_example_runner(steps):
    try:
        return basic_brain_example(steps=steps)
    except Exception as e:
        print(f"Error with steps={steps}: {e}")
        print("Retrying with smaller steps...")
        return basic_brain_example(steps=steps//2)

# Run with error recovery
stats = robust_example_runner(2000)
```

### 3. Resource Management

```python
import gc
from pynlm.examples import basic_brain_example

def memory_efficient_example(steps):
    # Create and run example
    stats = basic_brain_example(steps=steps)
    
    # Force garbage collection
    gc.collect()
    
    return stats

# Run with memory management
stats = memory_efficient_example(1000)
```

## Example Index

| Example Function | Description | Difficulty | Time |
|------------------|-------------|------------|------|
| basic_brain_example | Basic brain creation and simulation | Beginner | 5-10 min |
| learning_plasticity_example | Learning and plasticity demonstration | Intermediate | 15-20 min |
| agent_world_example | Complete agent-world interaction | Advanced | 20-30 min |

## Summary

These examples provide a comprehensive introduction to NLM Python API usage, covering:
- Basic brain creation and simulation
- Learning and plasticity mechanisms
- Complete agent-world interaction
- Configuration management
- Error handling and recovery
- Performance monitoring
- Best practices and optimization

Use these examples as a foundation for building your own NLM applications and extend them as needed for your specific requirements.