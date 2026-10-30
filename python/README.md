# NLM Python Bindings - Advanced Usage Guide

This document provides advanced usage examples and features for the enhanced NLM Python bindings.

## Installation

The NLM Python bindings are installed automatically when building the C++ project with scikit-build.

```bash
# From the project root
pip install -e .
```

## Advanced Usage Examples

### 1. Brain Simulation with Custom Configuration

```python
import pynlm
import numpy as np

# Create a custom configuration
config = pynlm.createDefaultConfig()

# Set custom parameters
config.set("brain.neuron_count", 1000)
config.set("brain.simulation_dt", 0.001)
config.set("brain.max_spikes", 5000)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Advanced Agent Behavior with Callbacks

```python
import pynlm
import numpy as np

def custom_action_selector(agent_brain):
    """Custom action selection logic"""
    # Get current state
    state = agent_brain.getBrain().getStats()
    
    # Custom logic based on brain state
    if state['excitement_ratio'] > 0.8:
        return pynlm.ActionType.Interact
    elif state['total_spikes'] < 100:
        return pynlm.ActionType.MoveForward
    else:
        return pynlm.ActionType.Wait

# Create simulation
brain = pynlm.createBrain()
world = pynlm.createSimpleWorld()
agent_brain = pynlm.createAgentBrain(brain)

# Configure world
world.configure(100.0, 100.0, 10, 10)
world.setAgentStart(50.0, 50.0)

# Run with custom callback
pynlm.runSimulation(brain, world, steps=500, verbose=True)
```

### 3. Data Analysis and Visualization

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

# Run simulation and collect data
brain = pynlm.createBrain()
world = pynlm.createSimpleWorld()
agent_brain = pynlm.createAgentBrain(brain)

# Configure and run
world.configure(100.0, 100.0, 10, 10)
world.setAgentStart(50.0, 50.0)

stats_history = []
for step in range(1000):
    # Get brain stats
    stats = pynlm.getStatistics(brain)
    stats_history.append(stats)
    
    # Run step
    pynlm.runSimulation(brain, world, steps=1)

# Convert to DataFrame for analysis
import pandas as pd
df = pd.DataFrame(stats_history)

# Plot neural activity
plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot(df['total_spikes'], label='Total Spikes')
plt.plot(df['firing_neurons'], label='Firing Neurons')
plt.xlabel('Step')
plt.ylabel('Count')
plt.legend()
plt.title('Neural Activity')

plt.subplot(2, 2, 2)
plt.plot(df['average_firing_rate'])
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')
plt.title('Average Firing Rate')

plt.subplot(2, 2, 3)
plt.plot(df['excitation_inhibition_ratio'])
plt.xlabel('Step')
plt.ylabel('E/I Ratio')
plt.title('Excitation-Inhibition Balance')

plt.subplot(2, 2, 4)
plt.hist(brain.getStats()['spike_times'], bins=50, alpha=0.7)
plt.xlabel('Time (ms)')
plt.ylabel('Frequency')
plt.title('Spike Time Distribution')

plt.tight_layout()
plt.show()
```

### 4. Batch Processing Multiple Brains

```python
import pynlm
import numpy as np

def create_and_run_brain(config_seed, steps=100):
    """Create and run a brain with specific configuration"""
    config = pynlm.createDefaultConfig()
    config.set("brain.seed", config_seed)
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    world.configure(50.0, 50.0, 8, 8)
    world.setAgentStart(25.0, 25.0)
    
    pynlm.runSimulation(brain, world, steps=steps)
    return pynlm.getStatistics(brain)

# Run multiple brains in parallel
import concurrent.futures
import threading

results = []
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    futures = [executor.submit(create_and_run_brain, i, steps=200) 
               for i in range(10)]
    for future in concurrent.futures.as_completed(futures):
        results.append(future.result())

# Analyze results
final_spikes = [r['total_spikes'] for r in results]
print(f"Average spikes: {np.mean(final_spikes):.2f}")
print(f"Spike std: {np.std(final_spikes):.2f}")
```

### 5. Advanced Configuration Management

```python
import pynlm
import json
import os

def save_experiment_config(experiment_name, parameters):
    """Save experiment configuration to file"""
    config = pynlm.createDefaultConfig()
    
    # Apply experiment parameters
    for key, value in parameters.items():
        config.set(key, value)
    
    # Save to file
    filepath = f"experiments/{experiment_name}_config.json"
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    config.saveToFile(filepath)
    
    return config

def load_experiment_config(experiment_name):
    """Load experiment configuration from file"""
    filepath = f"experiments/{experiment_name}_config.json"
    if os.path.exists(filepath):
        config = pynlm.createDefaultConfig()
        config.loadFromFile(filepath)
        return config
    else:
        raise FileNotFoundError(f"Experiment config not found: {experiment_name}")

# Example experiment
experiment_params = {
    "brain.neuron_count": 2000,
    "brain.simulation_dt": 0.0005,
    "brain.max_spikes": 10000,
    "development.stage": "Adult"
}

config = save_experiment_config("experiment_1", experiment_params)
brain = pynlm.createBrain(config)
```

### 6. Neural Network Analysis

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

def analyze_neural_connectivity(brain):
    """Analyze connectivity patterns in the brain"""
    stats = brain.getStats()
    
    # Get all regions
    regions = brain.getRegions()
    region_ids = brain.getRegionIds()
    
    connectivity_data = {}
    
    for i, region_id in enumerate(region_ids):
        region = brain.getRegion(region_id)
        neurons = region.getNeurons()
        
        # Count neurons by type
        neuron_types = {}
        for neuron in neurons:
            neuron_type = neuron.getType()
            neuron_types[neuron_type] = neuron_types.get(neuron_type, 0) + 1
        
        # Calculate average firing rate
        total_rate = 0.0
        active_count = 0
        for neuron in neurons:
            if neuron.isFiring():
                total_rate += neuron.getFiringRate()
                active_count += 1
        
        avg_rate = total_rate / max(active_count, 1)
        
        connectivity_data[region_id.value] = {
            'neuron_count': len(neurons),
            'neuron_types': neuron_types,
            'average_firing_rate': avg_rate,
            'active_percentage': (active_count / len(neurons)) * 100 if neurons else 0
        }
    
    return connectivity_data

# Example usage
brain = pynlm.createBrain()
brain.initialize()

connectivity = analyze_neural_connectivity(brain)

# Plot connectivity heatmap
plt.figure(figsize=(10, 8))
region_ids = sorted(connectivity.keys())
neuron_counts = [connectivity[rid]['neuron_count'] for rid in region_ids]

plt.subplot(2, 1, 1)
plt.bar(range(len(region_ids)), neuron_counts)
plt.xlabel('Region ID')
plt.ylabel('Neuron Count')
plt.title('Neuron Distribution Across Regions')
plt.xticks(range(len(region_ids)), region_ids)

plt.subplot(2, 1, 2)
for i, rid in enumerate(region_ids):
    types = connectivity[rid]['neuron_types']
    plt.bar(i, sum(types.values()), alpha=0.7, label=f'Region {rid}')

plt.xlabel('Region ID')
plt.ylabel('Total Neurons')
plt.title('Region Summary')
plt.legend()

plt.tight_layout()
plt.show()
```

### 7. Reinforcement Learning Integration

```python
import pynlm
import numpy as np
from typing import Callable

class NeuralReinforcementLearner:
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        self.brain = brain
        self.world = world
        self.agent_brain = pynlm.createAgentBrain(brain)
        self.reward_history = []
        self.prediction_history = []
        
    def run_learning_episode(self, max_steps: int = 1000, 
                           reward_function: Callable = None) -> dict:
        """Run a learning episode with reward collection"""
        self.agent_brain.initialize(self.world)
        self.world.reset()
        
        episode_data = {
            'rewards': [],
            'predictions': [],
            'actions': [],
            'steps': []
        }
        
        for step in range(max_steps):
            # Get current state
            percept = self.world.getSensoryPercept()
            
            # Process input and produce action
            self.agent_brain.processSensoryInput(percept)
            action = self.agent_brain.decodeMotorCommand()
            
            # Apply action and get reward
            action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate reward using custom function or default
            if reward_function:
                reward = reward_function(self.agent_brain, action_result)
            else:
                reward = action_result.reward
            
            # Apply reward modulation
            predicted = self.agent_brain.getPredictionError()
            self.agent_brain.applyRewardModulation(reward, predicted)
            
            # Update development
            self.agent_brain.updateDevelopment(1.0)
            
            # Record data
            episode_data['rewards'].append(reward)
            episode_data['predictions'].append(predicted)
            episode_data['actions'].append(action)
            episode_data['steps'].append(step)
            
            # Update world
            self.world.update(1.0)
            
            # Check for termination
            if self.world.getAgentBody().energy <= 0.0:
                break
        
        return episode_data
    
    def train(self, num_episodes: int, max_steps_per_episode: int = 1000):
        """Train the agent over multiple episodes"""
        all_episodes = []
        
        for episode in range(num_episodes):
            episode_data = self.run_learning_episode(
                max_steps=max_steps_per_episode
            )
            all_episodes.append(episode_data)
            
            if episode % 10 == 0:
                avg_reward = np.mean([d['rewards'] for d in all_episodes])
                print(f"Episode {episode}: Average reward = {avg_reward:.3f}")
        
        return all_episodes

# Example usage
def simple_reward_function(agent_brain, action_result):
    """Simple reward function based on action result"""
    return action_result.reward

brain = pynlm.createBrain()
world = pynlm.createSimpleWorld()
world.configure(100.0, 100.0, 10, 10)
world.setAgentStart(50.0, 50.0)

learner = NeuralReinforcementLearner(brain, world)
training_data = learner.train(100, max_steps_per_episode=500)
```

## Advanced Features

### 7.1 Batch Operations

The enhanced bindings include batch operations for improved performance:

```python
# Batch neuron operations
brain = pynlm.createBrain()
neurons = [neuron for region in brain.getRegions() 
           for neuron in region.getNeurons()]

# Reset all neurons
for neuron in neurons:
    neuron.reset()

# Inject current to all excitatory neurons
brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 1.0)
```

### 7.2 Context Management

```python
# Safe resource management with context managers
from contextlib import contextmanager

@contextmanager
def brain_simulation(brain, world, steps):
    """Context manager for safe brain simulation"""
    try:
        brain.initialize()
        yield brain, world
    finally:
        brain.reset()
        world.reset()

# Use context manager
with brain_simulation(brain, world, 1000) as (sim_brain, sim_world):
    pynlm.runSimulation(sim_brain, sim_world, steps=1000)
```

### 7.3 Error Handling and Validation

```python
def safe_simulation(brain, world, steps):
    """Safe simulation with comprehensive error handling"""
    try:
        if brain is None:
            raise ValueError("Brain cannot be None")
        
        if world is None:
            raise ValueError("World cannot be None")
        
        if steps <= 0:
            raise ValueError("Steps must be positive")
        
        brain.initialize()
        pynlm.runSimulation(brain, world, steps=steps)
        return True
        
    except Exception as e:
        print(f"Simulation error: {e}")
        return False

# Usage with error handling
if not safe_simulation(brain, world, 1000):
    print("Simulation failed, using fallback")
```

## Performance Optimization Tips

1. **Reuse Objects**: Reuse Brain and World objects across simulations
2. **Batch Operations**: Use batch operations for multiple neurons
3. **Minimize Configuration**: Cache frequently used configurations
4. **Monitor Memory**: Use `getStatistics()` to monitor memory usage
5. **Parallel Processing**: Use multiple threads for independent simulations

## Troubleshooting

### Common Issues and Solutions

1. **Import Error**
   ```python
   # Install with development dependencies
   pip install -e .[dev]
   ```

2. **Memory Issues**
   ```python
   # Monitor memory usage
   stats = pynlm.getStatistics(brain)
   print(f"Active neurons: {stats['active_neurons']}")
   
   # Reduce complexity
   brain.getConfig().set("brain.neuron_count", 1000)
   ```

3. **Simulation Instability**
   ```python
   # Use stable configuration
   config = pynlm.createDefaultConfig()
   config.set("brain.simulation_dt", 0.001)
   config.set("brain.max_spikes", 10000)
   ```

## Migration Guide from Previous Version

The enhanced bindings maintain backward compatibility while adding new features:

- All existing APIs work unchanged
- New convenience methods added for common operations
- Enhanced error handling and validation
- Performance optimizations for batch operations
- New statistical analysis utilities

## Next Steps

1. **Explore Documentation**: Check the [NLM Documentation](https://nlm.readthedocs.io) for comprehensive technical details
2. **Community Resources**: Join the [NLM Discord](https://discord.gg/nlm) for community support
3. **Research Papers**: Review the scientific publications on the NLM architecture
4. **Contribute**: Consider contributing to the project with new features or optimizations

## Version Information

- Python Bindings: 0.1.0
- C++ Core: 0.1.0
- Phase 6 Complete
- License: MIT

For more information, visit the [NLM Project](https://github.com/nlm-project/nlm).
