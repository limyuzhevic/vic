# NLM Python Tutorial

## Overview

This tutorial provides a comprehensive introduction to using NLM's enhanced Python bindings. It's designed for Python developers who want to get started with NLM quickly and efficiently.

## Prerequisites

- Python 3.8 or higher
- Basic understanding of Python programming
- Familiarity with object-oriented concepts

## Installation

```bash
# Install the enhanced NLM Python bindings
pip install pynlm[dev]

# Or install from source
pip install -e .
```

## Tutorial Structure

### 1. Basic Brain Operations
### 2. World and Agent Interactions
### 3. Advanced Brain Analysis
### 4. Experiment Management
### 5. Performance Optimization

---

## 1. Basic Brain Operations

### Creating a Brain

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Print brain information
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain type: {type(brain).__name__}")
```

### Initializing a Brain

```python
# Initialize the brain (ALWAYS call this!)
brain.initialize()

# Check if brain is initialized
print(f"Brain initialized: {brain.getTotalNeuronCount() > 0}")
```

### Basic Brain Operations

```python
# Make brain think for one step
brain.step(0)

# Get brain statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

### Brain Configuration

```python
# Get brain configuration
config = brain.getConfig()
print(f"Random seed: {config.get('random_seed')}")
print(f"Neuron count: {config.get('neuron_count')}")
print(f"Simulation timestep: {config.get('simulation_timestep')}")
```

---

## 2. World and Agent Interactions

### Creating World and Agent

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("World and agent created successfully")
```

### Complete Simulation Loop

```python
# Complete simulation loop
def run_simulation(brain, world, agent, num_steps):
    """Run complete simulation loop."""
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # 3. Brain processes information
        brain.step(step)
        
        # 4. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 5. Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 10 steps
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"Average rate: {brain.getAverageFiringRate():.2f} Hz")

# Run simulation
run_simulation(brain, world, agent, 100)
```

### Sensory Processing

```python
# Example of sensory processing
for step in range(50):
    world.update(0.1)
    
    # Get sensory percept (vision, touch, etc.)
    percept = world.getSensoryPercept()
    
    # Process sensory input (brain sees the world)
    agent.processSensoryInput(percept)
    
    # Brain processes sensory information
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Execute action
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Reward Processing

```python
# Example of reward processing
for step in range(50):
    # ... simulation code ...
    
    # Apply reward based on action outcome
    reward = 1.0 if action.success else 0.0
    predicted_reward = 0.5  # Agent's expectation
    
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Update development
    agent.updateDevelopment(0.1)
```

---

## 3. Advanced Brain Analysis

### Brain Statistics

```python
# Get comprehensive brain statistics
stats = pynlm.getBrainStatistics(brain)

# Access statistics
print("=== Brain Statistics ===")
print(f"Total neurons: {stats['total_neurons']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Development stage: {stats['development_stage']}")
print(f"Excitatory/Inhibitory ratio: {stats['excitatory_inhibition_ratio']}")
print(f"Memory usage: {stats['memory_usage']}")
print(f"CPU usage: {stats['cpu_usage']}")
```

### Activity Heatmap

```python
# Get activity heatmap for visualization
heatmap = pynlm.getBrainActivityHeatmap(brain)

# Use heatmap for analysis
import numpy as np

# Calculate statistics from heatmap
total_activity = np.sum(heatmap)
max_activity = np.max(heatmap)
average_activity = np.mean(heatmap)

print(f"Total activity: {total_activity}")
print(f"Maximum activity: {max_activity}")
print(f"Average activity: {average_activity:.2f}")

# Find active regions
active_regions = np.where(heatmap > 0.5)
print(f"Active regions: {len(active_regions[0])}")
```

### Brain Comparison

```python
# Create second brain for comparison
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

# Compare brain states
similarity = pynlm.compareBrainStates(brain, brain2)

print("=== Brain Comparison ===")
print(f"Overall similarity: {similarity:.2%}")

# Get detailed comparison
detailed_comparison = pynlm.compareBrainStates(
    brain, brain2,
    include_neuron_patterns=True,
    include_synapse_weights=True,
    include_activity_patterns=True
)

print(f"Neuron pattern similarity: {detailed_comparison['neuron_similarity']:.2%}")
print(f"Synapse weight similarity: {detailed_comparison['synapse_similarity']:.2%}")
print(f"Activity pattern similarity: {detailed_comparison['activity_similarity']:.2%}")
print(f"Development stage similarity: {detailed_comparison['development_similarity']:.2%}")
```

### Performance Metrics

```python
# Get performance metrics
metrics = pynlm.getBrainPerformanceMetrics(brain)

print("=== Performance Metrics ===")
print(f"Computational efficiency: {metrics['computational_efficiency']}")
print(f"Memory efficiency: {metrics['memory_efficiency']}")
print(f"Activity patterns: {metrics['activity_patterns']}")
print(f"Learning rate: {metrics['learning_rate']}")
print(f"Plasticity events: {metrics['plasticity_events']}")
print(f"Response time: {metrics['response_time']:.2f} ms")
```

---

## 4. Experiment Management

### Simple Experiment

```python
# Define experiment function
def run_learning_experiment(brain, world, agent, num_episodes):
    """Run learning experiment."""
    results = []
    
    for episode in range(num_episodes):
        episode_result = {
            'episode': episode,
            'reward': 0,
            'success': False,
            'steps': 0
        }
        
        # Run episode
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            episode_result['reward'] += result['reward']
            episode_result['steps'] += 1
            
            if result['success']:
                episode_result['success'] = True
                break
        
        results.append(episode_result)
        print(f"Episode {episode}: Reward={episode_result['reward']}, "
              f"Success={episode_result['success']}, Steps={episode_result['steps']}")
    
    return results

# Run experiment
results = run_learning_experiment(brain, world, agent, 10)
```

### Experiment with Monitoring

```python
# Run experiment with monitoring
def run_monitored_experiment(brain, world, agent, num_episodes):
    """Run experiment with monitoring."""
    import time
    
    start_time = time.time()
    
    for episode in range(num_episodes):
        episode_start = time.time()
        
        # Run episode
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        episode_time = time.time() - episode_start
        print(f"Episode {episode} completed in {episode_time:.2f}s")
    \n    total_time = time.time() - start_time
    print(f"Experiment completed in {total_time:.2f}s")
    
    # Get monitoring data
    monitoring_data = pynlm.getMonitoringData()
    print(f"Monitoring data: {monitoring_data}")

# Run monitored experiment
run_monitored_experiment(brain, world, agent, 5)
```

### Experiment with Analysis

```python
# Run experiment with analysis
def run_analyzed_experiment(brain, world, agent, num_episodes):
    """Run experiment with analysis."""
    experiment_results = []
    
    for episode in range(num_episodes):
        # Run episode
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect results
        episode_result = {
            'episode': episode,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'active_neurons': brain.getFiringNeuronCount(),
            'development_stage': brain.getDevelopmentalStage(),
            'stats': pynlm.getBrainStatistics(brain),
            'performance': pynlm.getBrainPerformanceMetrics(brain)
        }
        
        experiment_results.append(episode_result)
    
    # Analyze results
    print("=== Experiment Analysis ===")
    print(f"Number of episodes: {len(experiment_results)}")
    print(f"Average firing rate: {np.mean([r['final_firing_rate'] for r in experiment_results]):.2f} Hz")
    print(f"Total spikes: {sum(r['total_spikes'] for r in experiment_results)}")
    print(f"Final development stage: {experiment_results[-1]['development_stage']}")
    
    return experiment_results

# Run analyzed experiment
results = run_analyzed_experiment(brain, world, agent, 10)
```

---

## 5. Performance Optimization

### Memory Optimization

```python
import pynlm
import gc

# Optimize memory usage
def optimize_memory_usage(brain):
    """Optimize memory usage for brain."""
    # Force garbage collection
    gc.collect()
    
    # Clear temporary references
    import weakref
    weak_brain = weakref.ref(brain)
    
    # Process brain
    brain.step(0)
    
    # Clear weak reference
    del weak_brain
    gc.collect()

# Use memory optimization
optimize_memory_usage(brain)
```

### Computational Efficiency

```python
def optimize_computation(brain, steps):
    """Optimize computation for brain."""
    # Use efficient data structures
    import numpy as np
    
    # Batch process steps
    for batch_start in range(0, steps, 100):
        batch_end = min(batch_start + 100, steps)
        for step in range(batch_start, batch_end):
            brain.step(step)
    
    return steps

# Optimize computation
optimized_steps = optimize_computation(brain, 1000)
```

### Profiling

```python
def profile_brain_performance(brain, duration):
    """Profile brain performance."""
    import time
    import pstats
    import io
    
    # Start monitoring
    pynlm.startMonitoring(brain, 1.0)
    
    # Run performance test
    start_time = time.time()
    while time.time() - start_time < duration:
        brain.step(int(time.time() - start_time))
    
    # Get monitoring data
    monitoring_data = pynlm.getMonitoringData()
    
    # Stop monitoring
    pynlm.stopMonitoring()
    
    return {
        'duration': duration,
        'steps': duration,
        'monitoring_data': monitoring_data,
        'performance_metrics': pynlm.getBrainPerformanceMetrics(brain)
    }

# Profile brain performance
profile_results = profile_brain_performance(brain, 60)
print(f"Profile completed: {profile_results}")
```

### Error Handling

```python
import pynlm
import logging

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def safe_brain_operation(brain, operation, *args, **kwargs):
    """Perform brain operation with error handling."""
    try:
        result = operation(brain, *args, **kwargs)
        return result
    except pynlm.NLMError as e:
        logger.error(f"NLM operation failed: {e}")
        raise
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        raise pynlm.NLMError(f"Operation failed: {e}")

# Use safe operations
safe_brain_operation(brain, brain.step, 0)
safe_brain_operation(brain, brain.initialize)
```

---

## Quick Reference

### Essential Functions

```python
# Core functions
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

# Get information
brain.getTotalNeuronCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
brain.getTotalSpikeCount()

# World and agent
world = pynlm.createSimpleWorld()
world.update(0.1)
world.getSensoryPercept()
agent = pynlm.createAgentBrain(brain)
agent.processSensoryInput(percept)
agent.decodeMotorCommand()

# Enhanced functions
pynlm.quickStart()
pynlm.getBrainStatistics(brain)
pynlm.getBrainActivityHeatmap(brain)
pynlm.compareBrainStates(brain1, brain2)
pynlm.trainAgentEpisode(agent, brain, world, steps)
```

### Common Patterns

#### Simulation Loop
```python
# Complete simulation loop
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Learning Loop
```python
# Learning with rewards
for step in range(100):
    # ... simulation code ...
    
    # Apply learning
    agent.applyRewardModulation(reward, predicted_reward)
    agent.updateDevelopment(0.1)
```

#### Error Handling
```python
try:
    # Your code here
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.step(0)
except pynlm.NLMError as e:
    print(f"NLM Error: {e}")
except Exception as e:
    print(f"Unexpected Error: {e}")
```

---

## Resources

### Additional Reading

- **Documentation**: `docs/` directory contains detailed documentation
- **Examples**: `examples/` directory contains working examples
- **API Reference**: `API_REFERENCE.md` for complete API documentation
- **CLI Reference**: `CLI_REFERENCE.md` for command-line interface

### Getting Help

- **GitHub Issues**: Report bugs and request features
- **Discussions**: Ask questions and share experiences
- **Documentation**: Read comprehensive guides and tutorials

---

## Conclusion

This tutorial provides a comprehensive introduction to using NLM's enhanced Python bindings. It covers:

1. **Basic operations** for getting started quickly
2. **Advanced features** for complex simulations
3. **Performance optimization** for production use
4. **Error handling** for robust applications

The enhanced Python API makes it easy to create, control, and analyze NLM brains with Pythonic simplicity and power.

**Start your NLM journey today and experience the future of neural simulation!** 🚀

---

## License

This tutorial is part of the NLM project and is licensed under the MIT License. See LICENSE file for details.

## Authors

This tutorial was created by the NLM Development Team to provide comprehensive documentation for the enhanced NLM Python bindings.