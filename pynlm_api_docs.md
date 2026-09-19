# PYNLM - Comprehensive Python API Documentation

## Overview

Pynlm is the Python bindings module for NLM (熙然), a computational brain simulation framework. It provides Python access to the full C++ neural simulation capabilities including Leaky Integrate-and-Fire (LIF) neurons, synaptic plasticity, memory systems, and embodied interaction with simulated worlds.

## Installation

### Standard Installation

```bash
pip install pynlm
```

### Development Installation

```bash
pip install -e .
```

### Building from Source

```bash
# Clone the repository
cd /path/to/nlm

# Build with Python bindings
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python module
pip install pynlm
```

## Getting Started

### Quick Installation Test

```python
import pynlm

print("Pynlm version:", getattr(pynlm, '__version__', 'N/A'))

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Initialize and run a simple simulation
brain.initialize()
for i in range(10):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes fired")
```

## Core Classes

### Brain

The Brain class represents a complete artificial neural network simulation.

#### Creation and Initialization

```python
def createBrain(config=None):
    """Create a new brain instance.
    
    Args:
        config: Configuration object. If None, creates default config.
        
    Returns:
        Brain: New brain instance.
    """

# Using default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Customize configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.connection_probability", 0.15)
brain = pynlm.createBrain(config)
```

#### Lifecycle Management

```python
# Initialize brain (must be called before any simulation steps)
brain.initialize()

# Run simulation loop
for step in range(num_steps):
    brain.step(step)
    # ... do something with brain activity

# Reset brain for new simulation
brain.reset()

# Save/restore brain state
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

#### Neural Processing

```python
# Inject sensory input
brain.receiveSensoryInput(vision_input)
brain.injectCurrent(neuron_id, current_value)
brain.injectCurrentToNeurons(neuron_type, current_value)

# Get neural output
action = brain.produceAction()
```

#### Statistical Queries

```python
# Network properties
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
brain.getExcitationInhibitionRatio()

# Activity metrics
brain.getTotalSpikeCount()
brain.getActiveNeuronCount()
brain.getDevelopmentalStage()

# Regions and organization
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
brain.getRegionIds()
brain.getRegions()
brain.getRegionCount()
```

### Config

Configuration management for brain parameters and simulation settings.

#### Configuration Management

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Check if key exists
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Get all keys
keys = config.getKeys()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)

# File operations
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Command line arguments
config.loadFromArgs()

# Configuration summary
print(config.summary())
```

### AgentBrain

The AgentBrain class provides the interface between the NLM brain and an external world.

#### Creation and Initialization

```python
# Create agent brain
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Initialize with world
world = pynlm.createSimpleWorld()
agent.initialize(world)
```

#### Core Operations

```python
# Sensory processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Motor control
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())

# Learning and adaptation
reward = 1.0
predicted_reward = 0.5
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()
```

#### Neuromodulation Subsystems

```python
# Enable/disable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Query neuromodulation levels
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### SimpleWorld

The SimpleWorld class simulates a 2D environment where agents interact.

#### World Setup

```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Initialize world
world.reset()
world.setAgentStart(10.0, 10.0)
```

#### World Operations

```python
# Simulation control
world.update(timestep=0.1)
world.setRandomSeed(42)

# Object management
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.isValidPosition(x, y)
```

### SensoryInput Classes

#### Vision

```python
# Create vision input
vision = pynlm.Vision(width=8, height=8, channels=3)

# Set/get vision data
vision.setData(pixel_values_list)
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
```

#### Audio

```python
# Create audio input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Set/get audio data
audio.setData(samples_list)
audio.setSampleRate(22050)
```

#### InternalSignals

```python
# Create internal signals
internal = pynlm.InternalSignals()

# Add signals
internal.addSignal(0.5)
internal.addSignal(-0.3)
internal.clearSignals()
```

### Action Classes

#### Action Creation

```python
# Create action with type
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create action with type and parameters
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.7, 0.3])

# Set action properties
action.setType(pynlm.ActionType.TurnLeft)
action.setParameters([1.0, 0.5])
```

#### Action Properties

```python
action.getType()                    # Get action type
action.getParameters()            # Get action parameters
action.getName()                   # Get descriptive name
action.clone()                     # Create a copy
```

### Enumerations

#### NeuronType

```python
# Neuron types
excitatory = pynlm.NeuronType.Excitatory      # Standard excitatory neuron
inhibitory = pynlm.NeuronType.Inhibitory      # Standard inhibitory neuron
modulatory = pynlm.NeuronType.Modulatory      # Neuromodulatory neuron
sensory = pynlm.NeuronType.Sensory          # Sensory input neuron
motor = pynlm.NeuronType.Motor            # Motor output neuron
internal = pynlm.NeuronType.Internal      # Internal brain neuron
```

#### SynapseType

```python
# Synapse types
excitatory = pynlm.SynapseType.Excitatory     # Standard excitatory synapse
inhibitory = pynlm.SynapseType.Inhibitory     # Standard inhibitory synapse
modulatory = pynlm.SynapseType.Modulatory     # Neuromodulatory synapse
electrical = pynlm.SynapseType.Electrical    # Gap junction synapse
gap_junction = pynlm.SynapseType.GapJunction # Electrical coupling
```

#### DevelopmentalStage

```python
# Developmental stages
initial = pynlm.DevelopmentalStage.Initial          # Embryonic stage
critical_period = pynlm.DevelopmentalStage.CriticalPeriod # Sensitive learning period
maturation = pynlm.DevelopmentalStage.Maturation     # Physical development
adult = pynlm.DevelopmentalStage.Adult          # Full maturity
aging = pynlm.DevelopmentalStage.Aging          # Decline phase
```

#### ActionType

```python
# Action types for agent behavior
move_forward = pynlm.ActionType.MoveForward
move_backward = pynlm.ActionType.MoveBackward
turn_left = pynlm.ActionType.TurnLeft
turn_right = pynlm.ActionType.TurnRight
look = pynlm.ActionType.Look
interact = pynlm.ActionType.Interact
eat = pynlm.ActionType.Eat
drink = pynlm.ActionType.Drink
rest = pynlm.ActionType.Rest
wait = pynlm.ActionType.Wait
custom = pynlm.ActionType.Custom
```

#### WorldObjectType

```python
# World object types
empty = pynlm.WorldObjectType.Empty        # No object
resource = pynlm.WorldObjectType.Resource   # Valuable resource
hazard = pynlm.WorldObjectType.Hazard      # Dangerous object
wall = pynlm.WorldObjectType.Wall         # Impassable barrier
marker = pynlm.WorldObjectType.Marker     # Reference point
```

## Configuration Parameters

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Advanced Usage Patterns

### Pattern 1: Complete Agent Simulation

```python
"""Complete agent simulation with learning and development."""

import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return {
        'total_reward': agent.getNeuromodulationLevel(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel()
    }

# Run the simulation
results = run_agent_simulation(1000)
print("Simulation complete:", results)
```

### Pattern 2: Learning Experiment

```python
"""Learning experiment with STDP and Hebbian plasticity."""

import pynlm
import numpy as np

def run_learning_experiment(config_path=None):
    """Run a learning experiment demonstrating synaptic changes."""
    
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    if config_path:
        config.loadFromFile(config_path)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable specific plasticity
    if hasattr(brain, 'enablePlasticity'):
        brain.enablePlasticity(
            hebbian=True,
            stdp=True,
            reward_modulated=True
        )
    
    # Record initial state
    initial_spikes = brain.getTotalSpikeCount()
    initial_weights = get_synaptic_weights(brain)
    
    # Apply learning stimuli
    learning_cycles = 100
    for cycle in range(learning_cycles):
        # Correlated pre-post activity (simulating STDP)
        brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 10.0)
        brain.step(cycle)
        
        # Apply reward to modulate learning
        if cycle % 20 == 0:
            # Use a mock neuromodulator if available
            if hasattr(brain, 'applyReward'):
                brain.applyReward(1.0)
    
    # Record final state
    final_spikes = brain.getTotalSpikeCount()
    final_weights = get_synaptic_weights(brain)
    
    # Compute learning statistics
    learning_stats = {
        'total_spikes': final_spikes - initial_spikes,
        'weight_changes': compute_weight_changes(initial_weights, final_weights),
        'learning_efficiency': (final_spikes - initial_spikes) / learning_cycles
    }
    
    return learning_stats

def get_synaptic_weights(brain):
    """Extract synaptic weights from brain for analysis."""
    # Implementation depends on brain API
    # This is a placeholder for the actual implementation
    return []

def compute_weight_changes(initial, final):
    """Compute weight changes across the learning period."""
    if not initial or not final:
        return []
    
    return [final[i] - initial[i] for i in range(min(len(initial), len(final)))]

# Run learning experiment
stats = run_learning_experiment()
print("Learning experiment results:", stats)
```

### Pattern 3: Multi-Agent World Simulation

```python
"""Multi-agent simulation in a shared world environment."""

import pynlm
import threading
import time

class Agent:
    """Wrapper class for NLM agent in world simulation."""
    
    def __init__(self, brain_config, world):
        self.brain = pynlm.createBrain(brain_config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = world
        self.alive = True
        
        # Initialize
        self.brain.initialize()
        self.agent.initialize(self.world)
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
    
    def run(self, steps=100):
        """Run agent simulation loop."""
        for step in range(steps):
            if not self.alive:
                break
                
            try:
                # Get perception
                percept = self.world.getSensoryPercept()
                
                # Process and act
                self.agent.processSensoryInput(percept)
                self.brain.step(step)
                
                action = self.agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Small delay to make simulation readable
                time.sleep(0.01)
                
            except Exception as e:
                print(f"Agent error: {e}")
                self.alive = False
    
    def get_status(self):
        """Get current agent status."""
        return {
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel()
        }

def run_multi_agent_simulation(num_agents=3, world_size=20):
    """Run multiple agents in a shared world."""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Create agents with different starting positions
    agents = []
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    for i in range(num_agents):
        # Stagger starting positions
        x = (i + 1) * world_size // (num_agents + 1)
        y = world_size // 2
        world.setAgentStart(x, y)
        
        agent = Agent(config, world)
        agents.append(agent)
    
    # Start all agents
    threads = []
    for agent in agents:
        thread = threading.Thread(target=agent.run, args=(100,))
        thread.start()
        threads.append(thread)
    
    # Wait for completion
    for thread in threads:
        thread.join()
    
    # Collect results
    results = [agent.get_status() for agent in agents]
    return results

# Run multi-agent simulation
print("Running multi-agent simulation...")
results = run_multi_agent_simulation(3, 20)
for i, result in enumerate(results):
    print(f"Agent {i}: {result}")
```

### Pattern 4: Batch Training

```python
"""Batch training of multiple brains with shared experiences."""

import pynlm
import concurrent.futures
from multiprocessing import Pool

def train_brain(config_seed):
    """Train a single brain with given random seed."""
    config = pynlm.createDefaultConfig()
    config.set("random_seed", config_seed)
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Training loop
    for step in range(100):
        # Inject random stimuli
        stimulus = (hash(str(config_seed + step)) % 1000) / 1000.0
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, stimulus)
        brain.step(step)
    
    return {
        'seed': config_seed,
        'final_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'excitatory_inhibition_ratio': brain.getExcitationInhibitionRatio()
    }

def batch_train_brains(num_brains=10, max_workers=4):
    """Train multiple brains in parallel."""
    
    # Create seeds for each brain
    seeds = list(range(num_brains))
    
    # Train brains in parallel
    with concurrent.futures.ProcessPoolExecutor(max_workers=max_workers) as executor:
        results = list(executor.map(train_brain, seeds))
    
    # Analyze results
    total_spikes = sum(r['final_spikes'] for r in results)
    avg_firing = sum(r['avg_firing_rate'] for r in results) / num_brains
    
    return {
        'individual_results': results,
        'batch_stats': {
            'total_spikes': total_spikes,
            'average_firing_rate': avg_firing,
            'std_firing_rate': np.std([r['avg_firing_rate'] for r in results])
        }
    }

# Run batch training
print("Running batch training...")
training_results = batch_train_brains(10, 4)
print("Batch training complete:", training_results['batch_stats'])
```

## Performance Tips

### For Large Networks

```python
# Use smaller network for prototyping
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Start small
config.set("brain.connection_probability", 0.05)

# Gradually increase for production
config.set("brain.neuron_count", 10000)
config.set("brain.connection_probability", 0.1)
```

### Memory Management

```python
# Clear large objects to free memory
brain.reset()
world.reset()

# Use explicit cleanup
import gc
gc.collect()
```

### Simulation Speed

```python
# Optimize timestep for speed
# Smaller timestep = more accuracy, slower speed
brain.step(step, time=step * 0.01)  # 10ms timestep

# For faster simulation, use larger timestep
brain.step(step, time=step * 0.1)   # 100ms timestep
```

## Error Handling

### Common Exceptions

```python
try:
    import pynlm
    
    # Create and initialize brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
        
except RuntimeError as e:
    print(f"Runtime error: {e}")
    # Check if initialization was skipped
    
except Exception as e:
    print(f"Unexpected error: {e}")
    # Handle general errors
```

## Migration Guide

### From Previous Versions

This API is designed to be stable and backward-compatible. Key changes:

1. **Configuration API**: Slightly simplified parameter access
2. **Action system**: Enhanced with more action types
3. **World system**: Improved with better object management
4. **Neuromodulation**: Extended with curiosity and novelty systems

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [Architecture Documentation](docs/ARCHITECTURE.md)
- [Science Documentation](docs/SCIENCE.md)

## Version Information

```python
import pynlm

# API version (if available)
print("Pynlm API version:", getattr(pynlm, '__version__', '1.0.0'))

# Build information
print("Build info:", getattr(pynlm, '__build_info__', 'N/A'))
```
