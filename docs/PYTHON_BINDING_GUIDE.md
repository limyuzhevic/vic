# PYTHON_BINDING_GUIDE.md

# NLM Python Binding Guide

## Overview

This guide provides comprehensive information about using NLM's Python bindings. It covers installation, basic usage, advanced features, and API reference for the `pynlm` Python module.

## Module Information

The NLM Python bindings are implemented using `pybind11` and provide access to all major NLM brain simulation functionality from Python. The module is named `pynlm` and is automatically installed when you install the NLM package.

## Installation

### Standard Installation (Recommended)

```bash
# Install from PyPI (stable version)
pip install nlm

# Or install from source (development version)
pip install -e .
```

### Development Installation

```bash
# Install build dependencies first
pip install pybind11 scikit-build-core numpy pytest

# Install in development mode (reinstall on code changes)
pip install -e .

# Verify installation
python -c "import pynlm; print(pynlm.__version__)"
```

### Build from Source

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake
/cmake .. -DPYTHON_EXECUTABLE=$(which python)

# Build Python module
python setup.py build_ext --inplace

# Or use the modern approach
pip install .
```

## Getting Started

### First Program - Brain Simulation

```python
#!/usr/bin/env python3
"""Example: Basic brain simulation with NLM"""

import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally customize configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
print("Running simulation for 1000 steps...")
for step in range(1000):
    brain.step(step)
    
    # Print statistics every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
```

### Complete Agent-World Example

```python
#!/usr/bin/env python3
"""Example: Complete agent with brain and world interaction"""

import pynlm
import numpy as np

def run_complete_simulation(num_steps=500):
    """Run a complete simulation with brain, agent, and world."""
    
    # 1. Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("plasticity.enable_stdp", True)
    config.set("plasticity.enable_hebbian", True)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30,
        height=30,
        visionWidth=12,
        visionHeight=12,
        visionFOV=90
    )
    world.reset()
    world.setAgentStart(15.0, 15.0)  # Center of world
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable key subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("=== NLM Agent Simulation ===")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"Simulation steps: {num_steps}")
    print()
    
    # 6. Run simulation loop
    for step in range(num_steps):
        # Update world physics
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain computation
        brain.step(step)
        
        # Decode motor command from brain
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation based on world state
        reward = 0.0
        if world.getEnergy() > world.getMaxEnergy() * 0.8:
            reward = 0.1  # Small energy reward
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Update developmental state
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev stage: {brain.getDevelopmentalStage().name}")
            print(f"  Agent position: ({world.getAgentX():.1f}, {world.getAgentY():.1f})")
            print()
    
    print("=== Simulation Complete ===")
    return brain, agent, world

if __name__ == "__main__":
    run_complete_simulation(500)
```

## Key Classes and Methods

### Brain Class

```python
# Brain Creation and Configuration
brain = pynlm.createBrain(config)                    # Create from config
brain = pynlm.createBrain(neuron_count=2000)         # Create with parameters

# Initialization
brain.initialize()                                    # Initialize brain
brain.initialize(development_stage='adult')          # Initialize with stage

# Core Simulation
brain.step(step_number)                              # Run simulation step
brain.step(step_number, time)                        # Step with timestamp
brain.reset()                                        # Reset to initial state

# State Management
brain.save("checkpoint.bin")                         # Save brain state
brain.load("checkpoint.bin")                         # Load brain state

# Input/Output
brain.receiveSensoryInput(data)                      # Inject sensory data
action = brain.produceAction()                       # Get motor output

# Statistics and Monitoring
brain.getTotalNeuronCount()                          # Total neurons
brain.getTotalSynapseCount()                         # Total synapses
brain.getFiringNeuronCount()                         # Currently firing
brain.getAverageFiringRate()                         # Average firing rate
brain.getExcitationInhibitionRatio()                # E/I balance
brain.getMemoryUsage()                               # Memory consumption

# Subsystems Access
brain.getWorkingMemory()                             # Access working memory
brain.getEpisodicMemory()                            # Access episodic memory
brain.getAssociativeMemory()                         # Access associative memory
brain.getNeuromodulators()                           # Get neuromodulator levels
brain.getPredictionSystem()                          # Access prediction system
brain.getDevelopmentSystem()                         # Access development system

# Configuration Access
brain.getConfig()                                    # Get configuration object
brain.setConfigValue(key, value)                     # Set config value
```

### Config Class

```python
# Configuration Creation and Access
config = pynlm.createDefaultConfig()                  # Create default config
config = pynlm.createConfig()                         # Create empty config

# Query Configuration
config.has("brain.neuron_count")                     # Check if key exists
config.getKeys()                                     # Get all keys
config.getValue(key)                                 # Get value

# Modify Configuration
config.set("brain.neuron_count", 2000)               # Set value
config.set("plasticity.enable_stdp", True)          # Enable STDP
config.set("neuromod.curiosity.scale", 2.0)          # Set neuromodulator scale

# Configuration File I/O
config.loadFromFile("config.json")                   # Load from JSON
config.saveToFile("config.json")                     # Save to JSON
config.exportToDict()                                # Export to Python dict
config.importFromDict(d)                             # Import from dict

# Preset Configurations
config = pynlm.createSmallConfig()                    # Minimal configuration
config = pynlm.createStandardConfig()                 # Standard settings
config = pynlm.createLargeConfig()                    # High-performance config
```

### AgentBrain Class

```python
# Agent Creation and Management
agent = pynlm.createAgentBrain(brain)                 # Create agent from brain
agent = pynlm.createAgentBrain(brain, config)        # Create with custom config

# Initialization
agent.initialize(world)                              # Initialize with world
agent.initialize(world, enable_developmental=True)   # Initialize with options

# Core Agent Functionality
agent.processSensoryInput(percept)                   # Process sensory data
agent.decodeMotorCommand()                          # Decode motor action
agent.applyRewardModulation(reward, predicted)       # Apply reward signal
agent.updateDevelopment(timestep)                    # Update development

# Neuromodulation Control
agent.enableRewardModulation(True/False)            # Enable/disable reward
agent.enableCuriosity(True/False)                    # Enable/disable curiosity
agent.enableNovelty(True/False)                      # Enable/disable novelty
agent.enableStructuralPlasticity(True/False)         # Enable/disable structural plasticity

# State and Statistics
agent.getCuriosityLevel()                            # Current curiosity level
agent.getNoveltyLevel()                              # Current novelty level
agent.getPredictionError()                           # Prediction error signal
agent.getMemoryUsage()                               # Memory consumption
agent.getActiveSubsystems()                           # List enabled subsystems

# Development
agent.getDevelopmentalStage()                       # Current developmental stage
agent.isInCriticalPeriod()                           # Check if in critical period
agent.getPlasticityRate()                            # Current plasticity rate
```

### SimpleWorld Class

```python
# World Creation
world = pynlm.createSimpleWorld()                    # Create basic world
world = pynlm.createOpenWorld()                       # Create open-world environment
world = pynlm.createGridWorld(width, height)         # Create grid-based world

# Configuration
world.configure(width=20, height=20)                 # Basic configuration
world.configure(width=20, height=20, visionWidth=8)  # With vision
world.configure(width=20, height=20, maxEnergy=1000) # With energy

# Control and Simulation
world.reset()                                        # Reset world state
world.update(timestep)                               # Update physics
world.step(action, timestep)                        # Take action

# Agent Management
world.setAgentStart(x, y)                           # Set initial position
world.getAgentX(), world.getAgentY()                 # Get current position
world.applyMotorCommand(action, time)                # Apply action

# Sensory and Perception
percept = world.getSensoryPercept()                   # Get sensory data
vision = world.getVision()                           # Get visual input
body = world.getAgentBody()                         # Get agent body
energy = world.getEnergy()                           # Get current energy

# World Objects
world.addObject(object)                              # Add object
world.removeObject(x, y)                            # Remove object
world.getObject(x, y)                               # Get object
world.isValidPosition(x, y)                         # Check position validity

# World Properties
world.getWidth(), world.getHeight()                 # World dimensions
world.getWidthUnits(), world.getHeightUnits()        # Physical dimensions
world.getSimulationTime()                           # Current simulation time
world.getMaxEnergy()                                # Maximum energy
world.setMaxEnergy(value)                           # Set maximum energy
```

### Enumerations

```python
# Neuron Types
pynlm.NeuronType.Excitatory     # Excitatory neuron
pynlm.NeuronType.Inhibitory    # Inhibitory neuron
pynlm.NeuronType.Sensory       # Sensory neuron
pynlm.NeuronType.Motor         # Motor neuron
pynlm.NeuronType.Modulatory    # Neuromodulatory neuron
pynlm.NeuronType.Internal     # Internal neuron

# Developmental Stages
pynlm.DevelopmentalStage.Initial          # Initial developmental stage
pynlm.DevelopmentalStage.CriticalPeriod  # Critical period for learning
pynlm.DevelopmentalStage.Maturation      # Maturation phase
pynlm.DevelopmentalStage.Adult          # Adult stage
pynlm.DevelopmentalStage.Aging          # Aging stage

# Action Types
pynlm.ActionType.MoveForward    # Move forward
pynlm.ActionType.TurnLeft       # Turn left
pynlm.ActionType.TurnRight      # Turn right
pynlm.ActionType.MoveBackward   # Move backward
pynlm.ActionType.Interact       # Interact with object
pynlm.ActionType.Eat            # Consume resource
pynlm.ActionType.Sleep          # Rest/consolidate

# World Object Types
pynlm.WorldObjectType.Empty     # Empty space
pynlm.WorldObjectType.Resource  # Resource object
pynlm.WorldObjectType.Hazard   # Hazardous object
pynlm.WorldObjectType.Wall     # Wall barrier
pynlm.WorldObjectType.Marker   # Navigation marker
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Connection probability |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Weight initialization std dev |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant for LTP (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant for LTD (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.development.modulate_plasticity` | bool | true | Plasticity modulation by development |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.dopamine.baseline` | float | 0.1 | Baseline dopamine level |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.sensitivity` | float | 1.0 | Curiosity sensitivity |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.5 | Novelty detection threshold |
| `neuromod.prediction.error_sensitivity` | float | 1.0 | Prediction error sensitivity |

## Advanced Usage

### Custom Configuration

```python
# Create custom configuration from scratch
import pynlm

config = pynlm.createConfig()

# Configure brain parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.08)
config.set("brain.initial_weight_mean", 0.3)
config.set("brain.initial_weight_std", 0.05)

# Configure plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.sensitivity", 1.2)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.threshold", 0.3)

# Create and use brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Multi-Agent Simulation

```python
# Multi-agent simulation example
import pynlm
import threading
import time

class NLMEnvironment:
    def __init__(self, num_agents=3):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
        self.world.reset()
        
        self.agents = []
        self.brains = []
        
        for i in range(num_agents):
            # Create unique brain for each agent
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1500 + i * 500)  # Different sizes
            config.set("neuromod.curiosity.scale", 1.0 + i * 0.1)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            
            self.brains.append(brain)
            self.agents.append(agent)
            
            # Set different starting positions
            x = 10 + i * 10
            y = 10 + i * 10
            self.world.setAgentStart(x, y)
    
    def run_step(self):
        """Run one simulation step for all agents."""
        self.world.update(0.1)
        
        for i, agent in enumerate(self.agents):
            percept = self.world.getSensoryPercept()
            agent.processSensoryInput(percept)
            self.brains[i].step(i)
            
            motor_cmd = agent.decodeMotorCommand()
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
    
    def run_simulation(self, num_steps):
        """Run complete simulation."""
        for step in range(num_steps):
            self.run_step()
            
            if step % 100 == 0:
                print(f"Step {step}")
                for i, agent in enumerate(self.agents):
                    print(f"  Agent {i}: {self.brains[i].getFiringNeuronCount()} firing neurons, "
                          f"curiosity: {agent.getCuriosityLevel():.3f}")

# Run multi-agent simulation
env = NLMEnvironment(num_agents=3)
env.run_simulation(300)
```

### Visualization and Data Collection

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

# Create and run simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Collect statistics
simulation_steps = 1000
firing_rates = []
curiosity_levels = []
energy_levels = []

for step in range(simulation_steps):
    brain.step(step)
    
    firing_rates.append(brain.getFiringNeuronCount() / brain.getTotalNeuronCount())
    curiosity_levels.append(brain.getCuriosityLevel())
    energy_levels.append(brain.getEnergyLevel())

# Plot statistics
fig, axes = plt.subplots(3, 1, figsize=(12, 10))

# Firing rate
axes[0].plot(firing_rates)
axes[0].set_ylabel('Firing Rate')
axes[0].set_title('Brain Firing Rate Over Time')
axes[0].grid(True)

# Curiosity level
axes[1].plot(curiosity_levels)
axes[1].set_ylabel('Curiosity Level')
axes[1].set_title('Curiosity Over Time')
axes[1].grid(True)

# Energy level
axes[2].plot(energy_levels)
axes[2].set_ylabel('Energy Level')
axes[2].set_xlabel('Simulation Step')
axes[2].set_title('Energy Level Over Time')
axes[2].grid(True)

plt.tight_layout()
plt.show()
```

## Performance Optimization

### Memory Management

```python
# Efficient memory usage
import pynlm

# Use configuration to optimize memory
config = pynlm.createConfig()
config.set("brain.neuron_count", 2000)  # Smaller for memory efficiency
config.set("brain.synapse_density", 0.05)  # Lower connectivity

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor memory usage during simulation
for step in range(500):
    brain.step(step)
    
    # Get current memory usage
    memory_usage = brain.getMemoryUsage()
    print(f"Step {step}: Memory usage: {memory_usage / 1024 / 1024:.2f} MB")
```

### Parallel Processing

```python
# Parallel simulation using multiple processes
import pynlm
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor

def run_simulation_step(step):
    """Run a single simulation step in parallel."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(step)
    
    return {
        'step': step,
        'firing_neurons': brain.getFiringNeuronCount(),
        'activity': brain.getAverageActivity()
    }

# Run multiple simulations in parallel
def parallel_simulation(num_steps=100, num_workers=4):
    with ProcessPoolExecutor(max_workers=num_workers) as executor:
        futures = [executor.submit(run_simulation_step, i) 
                  for i in range(num_steps)]
        
        results = []
        for future in futures:
            results.append(future.result())
    
    return results

# Run parallel simulation
results = parallel_simulation(100)
print(f"Completed {len(results)} parallel simulation steps")
```

## Troubleshooting

### Common Issues

#### 1. Import Errors
```bash
# Check if pynlm is installed correctly
python -c "import pynlm; print(pynlm.__version__)"

# Reinstall if necessary
pip uninstall pynlm
pip install -e .
```

#### 2. Memory Issues
```python
# Monitor memory usage
import pynlm
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")

# Use smaller configuration for memory efficiency
config = pynlm.createConfig()
config.set("brain.neuron_count", 1000)  # Smaller brain
```

#### 3. Performance Issues
```python
# Profile simulation performance
import pynlm
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"1000 steps took {end_time - start_time:.2f} seconds")
print(f"Steps per second: {1000 / (end_time - start_time):.1f}")
```

### Support

For Python-specific issues:
- Check the Python API documentation in `docs/PYTHON_API_REFERENCE.md`
- Review the examples in the `examples/` directory
- Check the project's issue tracker for known issues
- Join the NLM community for support

---

*Last updated: $(date +%Y-%m-%d)*