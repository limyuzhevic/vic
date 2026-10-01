# NLM Python API Reference

## Overview

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator written in C++20, with Python bindings provided by `pynlm`. This API reference documents all Python classes, functions, and methods available for interacting with the neural simulation framework.

## Installation

```bash
pip install pynlm
```

Or for development:
```bash
pip install -e .
```

## Core Classes

### Configuration

The `Config` class manages all simulation parameters:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set custom parameters
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -50.0)

# Save configuration
config.saveToFile("config.json")

# Load configuration  
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

print(f"Neuron count: {config.get('brain.neuron_count')}")
```

**Methods:**
- `set(key, value)` - Set a configuration value
- `get(key)` - Get a configuration value
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration
- `summary()` - Get configuration summary

### Brain

The `Brain` class represents the neural simulation engine:

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

# Save/load state
brain.save("checkpoint.bin")
# ... later ...
brain.load("checkpoint.bin")

# Add regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

**Key Methods:**
- `initialize()` - Start simulation
- `step(step)` - Run one simulation step
- `reset()` - Reset brain state
- Various getters for neural statistics
- `receiveSensoryInput(input)` - Inject sensory data
- `produceAction()` - Generate motor output

### AgentBrain

The `AgentBrain` class bridges the brain and the simulation environment:

```python
import pynlm

# Setup complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Complete simulation loop
for step in range(100):
    world.update(0.1)  # Update environment
    percept = world.getSensoryPercept()  # What agent sees
    agent.processSensoryInput(percept)  # Tell brain
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Act
    
    # Get brain stats
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

**Key Methods:**
- `initialize(world)` - Connect to environment
- `processSensoryInput(percept)` - Feed sensory data to brain
- `decodeMotorCommand()` - Convert brain activity to action
- `applyRewardModulation(reward, predicted)` - Learn from outcomes
- `updateDevelopment(timestep)` - Manage developmental stages
- Subsystem control: `enableRewardModulation()`, etc.

### SimpleWorld

The `SimpleWorld` represents the simulation environment:

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=30, 
    height=30, 
    visionWidth=8, 
    visionHeight=8
)
world.reset()
world.setAgentStart(15.0, 15.0)

# Add objects to environment
resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(resource)

# Simulation loop
for step in range(50):
    world.update(0.1)  # Advance time
    
    # Get what agent perceives
    percept = world.getSensoryPercept()
    print(f"Vision width: {percept.getVisionWidth()}")
    print(f"Touch sensors: {len(percept.getTouch())}")
    
    # Get agent state
    body = world.getAgentBody()
    print(f"Agent position: ({body.x:.1f}, {body.y:.1f})")
    print(f"Agent energy: {body.energy:.1f}")
```

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Set environment size
- `reset()` - Reset to initial state
- `update(timestep)` - Advance simulation
- `applyMotorCommand(action, time)` - Execute action
- `getSensoryPercept()` - Get agent's perceptions
- `getAgentBody()` - Get agent's physical state

### Sensory Processing

The `SensoryPercept` class contains all sensory information:

```python
import pynlm

percept = pynlm.SensoryPercept()

# Vision
vision = percept.getVision()
vision_data = vision.getData()
print(f"Vision size: {vision.getWidth()}x{vision.getHeight()}")

# Internal signals (reward, curiosity, etc.)
internal = percept.getInternal()
if internal:
    reward = internal[0]
    curiosity = internal[1]
    
# Audio
audio = percept.getAudio()
sample_rate = audio.getSampleRate()
num_samples = audio.getNumSamples()

# Timestamp
print(f"Percept timestamp: {percept.getTimestamp():.3f}")
```

### Action

The `Action` class represents motor commands:

```python
import pynlm

# Create actions in different ways
action1 = pynlm.Action()  # Empty action
action2 = pynlm.Action(pynlm.ActionType.MoveForward)  # By type
action3 = pynlm.Action(pynlm.ActionType.Eat, [0.5, 0.8, 1.2])  # With parameters

# Access action properties
print(f"Action type: {action2.getType()}")
print(f"Action name: {action2.getName()}")
print(f"Parameters: {action3.getParameters()}")

# Clone action
cloned = action2.clone()
```

## Enumerations

### NeuronType
```python
pynlm.NeuronType.Excitatory      # Regular excitatory neuron
pynlm.NeuronType.Inhibitory      # Inhibitory neuron
pynlm.NeuronType.Sensory         # Sensory neuron
pynlm.NeuronType.Motor           # Motor neuron
pynlm.NeuronType.Modulatory      # Neuromodulatory neuron
pynlm.NeuronType.Internal        # Internal state neuron
```

### ActionType
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Custom
```

### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial         # Newborn
pynlm.DevelopmentalStage.CriticalPeriod  # Sensitive learning period
pynlm.DevelopmentalStage.Maturation       # Adult properties developing
pynlm.DevelopmentalStage.Adult           # Full maturity
pynlm.DevelopmentalStage.Aging           # Age-related changes
```

## Utility Functions

```python
# Create default instances
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Special values
invalid_neuron_id = pynlm.INVALID_NEURON_ID
invalid_synapse_id = pynlm.INVALID_SYNAPSE_ID
invalid_region_id = pynlm.INVALID_REGION_ID
invalid_population_id = pynlm.INVALID_POPULATION_ID
```

## Advanced Usage Examples

### 1. Custom Agent with Learning

```python
def train_agent(config, world, num_episodes=100):
    """Train an agent in the given environment."""
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    episode_rewards = []
    
    for episode in range(num_episodes):
        total_reward = 0
        
        for step in range(200):  # Max steps per episode
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processes input
            brain.step(step)
            
            # Generate and execute action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward signal
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            predicted = agent.getPredictionError()
            
            agent.applyRewardModulation(reward, predicted)
            agent.updateDevelopment(0.1)
            
            total_reward += reward
            
            if world.getAgentBody().health <= 0:
                break
        
        episode_rewards.append(total_reward)
        world.reset()  # Reset for next episode
        
        if episode % 10 == 0:
            avg_reward = sum(episode_rewards[-10:]) / 10
            print(f"Episode {episode}: Average reward = {avg_reward:.2f}")
    
    return episode_rewards, brain, agent
```

### 2. Experiment Runner

```python
class NLMExperiment:
    """Run NLM experiments with automatic recording and analysis."""
    
    def __init__(self, config):
        self.config = config
        self.results = []
    
    def run(self, world, num_steps=1000):
        """Run an experiment and return results."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        episode_data = {
            'rewards': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': []
        }
        
        for step in range(num_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            episode_data['rewards'].append(
                percept.getInternal()[0] if percept.getInternal() else 0.0
            )
            episode_data['firing_rates'].append(
                brain.getAverageFiringRate()
            )
            episode_data['curiosity_levels'].append(agent.getCuriosityLevel())
            episode_data['novelty_levels'].append(agent.getNoveltyLevel())
            
            if world.getAgentBody().health <= 0:
                break
        
        self.results.append(episode_data)
        return episode_data
    
    def analyze(self):
        """Analyze recorded experimental results."""
        if not self.results:
            return None
        
        analysis = {
            'total_episodes': len(self.results),
            'avg_final_reward': sum(
                ep['rewards'][-1] for ep in self.results
            ) / len(self.results),
            'avg_max_firing_rate': sum(
                max(ep['firing_rates']) for ep in self.results
            ) / len(self.results),
            'avg_curiosity': sum(
                ep['curiosity_levels'][-1] for ep in self.results
            ) / len(self.results)
        }
        
        return analysis
```

### 3. Checkpoint Manager

```python
class CheckpointManager:
    """Manage brain checkpoints for experiment reproduction."""
    
    def __init__(self, config):
        self.config = config
        self.checkpoints = {}
    
    def save_checkpoint(self, brain, agent, world, name):
        """Save current state."""
        checkpoint = {
            'config': self.config,
            'brain_state': brain.save(),
            'agent_state': agent.getBrain(),  # Save brain state
            'world_config': (world.getWidth(), world.getHeight()),
            'timestamp': world.getSimulationTime()
        }
        self.checkpoints[name] = checkpoint
        print(f"Checkpoint '{name}' saved.")
    
    def load_checkpoint(self, brain, name):
        """Load state from checkpoint."""
        if name not in self.checkpoints:
            raise ValueError(f"Checkpoint '{name}' not found.")
        
        checkpoint = self.checkpoints[name]
        brain.load(checkpoint['brain_state'])
        print(f"Checkpoint '{name}' loaded.")
    
    def compare_checkpoints(self, name1, name2):
        """Compare two checkpoints."""
        if name1 not in self.checkpoints or name2 not in self.checkpoints:
            raise ValueError("One or both checkpoints not found.")
        
        cp1 = self.checkpoints[name1]
        cp2 = self.checkpoints[name2]
        
        comparison = {
            'time_difference': abs(cp1['timestamp'] - cp2['timestamp']),
            'same_config': cp1['config'] == cp2['config']
        }
        
        return comparison
```

### 4. Visualization Helper

```python
def plot_brain_dynamics(brain, steps=None, filename=None):
    """Plot brain dynamics over time."""
    try:
        import matplotlib.pyplot as plt
        import numpy as np
        
        if steps is None:
            steps = brain.getTotalSpikeCount()
        
        # Create time series data
        time_points = np.arange(steps)
        
        # Get firing rate data (simplified - would need custom monitoring)
        firing_rates = []
        for step in range(steps):
            brain.step(step)
            firing_rates.append(brain.getAverageFiringRate())
        
        # Plot
        plt.figure(figsize=(12, 8))
        
        plt.subplot(2, 2, 1)
        plt.plot(time_points, firing_rates)
        plt.title('Average Firing Rate')
        plt.xlabel('Time Step')
        plt.ylabel('Firing Rate')
        
        plt.subplot(2, 2, 2)
        plt.hist(firing_rates, bins=50)
        plt.title('Firing Rate Distribution')
        plt.xlabel('Firing Rate')
        plt.ylabel('Frequency')
        
        plt.subplot(2, 2, 3)
        plt.plot(brain.getTotalSynapseCount(), brain.getFiringNeuronCount(), 'o-')
        plt.title('Active Synapses vs Firing Neurons')
        plt.xlabel('Total Synapses')
        plt.ylabel('Firing Neurons')
        
        plt.tight_layout()
        
        if filename:
            plt.savefig(filename)
            print(f"Plot saved to {filename}")
        
        plt.show()
        
    except ImportError:
        print("Matplotlib not installed. Skipping visualization.")
    except Exception as e:
        print(f"Error plotting: {e}")
```

## Error Handling and Best Practices

### Common Errors and Solutions

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)  # Will fail if brain not initialized
        
except RuntimeError as e:
    print(f"Runtime error: {e}")
    brain.reset()  # Try resetting
    brain.initialize()  # Reinitialize
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Best Practices

1. **Always initialize the brain before stepping:**
   ```python
   brain = pynlm.createBrain(config)
   brain.initialize()  # Never skip this!
   ```

2. **Use reset() between episodes:**
   ```python
   brain.reset()
   agent.reset()
   world.reset()
   ```

3. **Enable learning subsystems before training:**
   ```python
   agent.enableRewardModulation(True)
   agent.enableCuriosity(True)
   agent.enableStructuralPlasticity(True)
   agent.enableDevelopment(True)
   ```

4. **Monitor for termination conditions:**
   ```python
   while step < max_steps and agent.getBrain().getHealth() > 0:
       # Simulation loop
   ```

5. **Use checkpoints for reproducibility:**
   ```python
   checkpoint_manager = CheckpointManager(config)
   checkpoint_manager.save_checkpoint(brain, agent, world, "pre_training")
   # ... training ...
   checkpoint_manager.save_checkpoint(brain, agent, world, "post_training")
   ```

## Performance Tips

1. **Use batch processing:**
   ```python
   # Process multiple steps at once
   for step in range(100):
       brain.step(step)
   ```

2. **Reuse configurations:**
   ```python
   # Create once, reuse many times
   config = pynlm.createDefaultConfig()
   for episode in range(num_episodes):
       brain = pynlm.createBrain(config)
       brain.initialize()
       # ... run episode ...
   ```

3. **Monitor memory usage:**
   ```python
   import psutil
   import os
   
   process = psutil.Process(os.getpid())
   print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.1f} MB")
   ```

## Migration Guide

### From C++ to Python

**C++:**
```cpp
// C++ usage
Config config;
Brain brain(config);
brain.initialize();
brain.step(step);
```

**Python:**
```python
# Python usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
```

### Key Differences

1. **Object Creation:** Python uses factory functions (`createBrain()`) vs. constructors
2. **Error Handling:** Python uses exceptions instead of error codes
3. **Memory Management:** Python has automatic garbage collection
4. **Type Checking:** Python provides more runtime type checking

## Next Steps

1. **Read `HOW_TO_USE.md`** - Detailed installation and usage instructions
2. **Explore `docs/ARCHITECTURE.md`** - Deep dive into system architecture
3. **Try the examples** - Use the provided examples to get started
4. **Join the community** - Learn from other users and contribute to the project

## Acknowledgements

This Python API is built on the foundation of:
- The NLM C++20 neural simulation framework
- The pybind11 Python binding library
- The scikit-build-core build system

Special thanks to the NLM research team for creating this groundbreaking brain simulation framework.
