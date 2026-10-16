# NLM Python API Documentation

This document provides comprehensive documentation for the NLM Python API, including examples, best practices, and advanced usage patterns.

## Overview

The NLM (Neural Learning Machine) Python API provides a high-level interface to the NLM C++ neural simulation framework. It enables you to create, configure, and run neural simulations programmatically using Python.

### Key Features

- **Brain Simulation**: Create and run spiking neural networks
- **Memory Integration**: Access working memory, episodic memory, and associative memory
- **Neuromodulation**: Use dopamine, curiosity, and novelty-based learning
- **Development System**: Simulate brain development and maturation
- **Prediction System**: Implement predictive coding and error-based learning
- **Experiment Framework**: Run and analyze complex experiments
- **Visualization Tools**: Visualize neural activity and simulation results

## Installation

```bash
pip install pynlm
```

## Quick Start

### Basic Brain Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Setup

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.1)

# Create complete agent setup (recommended pattern)
brain, agent, world = pynlm.create_complete_agent(
    config,
    worldConfig={
        "width": 20,
        "height": 20,
        "visionWidth": 8,
        "visionHeight": 8
    }
)

print(f"Agent setup complete: {brain.getTotalNeuronCount()} neurons")
```

## Brain Class

The `Brain` class represents the central neural substrate. It contains neurons, synapses, and manages all neural computation.

### Creation and Initialization

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.learning_rate", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

# Check brain status
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synaptoses: {brain.getTotalSynapseCount()}")
print(f"Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### Simulation Control

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation for a specific number of steps
for step in range(1000):
    brain.step(step)
    
    # Log progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
              f"{brain.getAverageFiringRate():.2f} avg firing rate")

# Get final statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Input Processing

```python
import pynlm
from pynlm import Vision, InternalSignals

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create sensory input
vision = Vision(16, 16)  # 16x16 vision field
vision.setData([0.5] * 256)  # Simple pattern

internal_signals = InternalSignals()
for i in range(10):
    internal_signals.addSignal(0.1 * i)

# Inject input into brain
sensory_input = SensoryInput()
sensory_input.setVision(vision)
sensory_input.setInternal(internal_signals)

brain.receiveSensoryInput(sensory_input)

# Run simulation
for step in range(100):
    brain.step(step)
```

### Action Production

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation to generate activity
for step in range(100):
    brain.step(step)

# Get action based on neural activity
action = brain.produceAction()
print(f"Action type: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")
print(f"Action name: {action.getName()}")
```

### Brain State Management

```python
import pynlm
import tempfile
import os

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps to generate activity
for step in range(100):
    brain.step(step)

# Save brain state to file
with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
    checkpoint_path = f.name

brain.save(checkpoint_path)
print(f"Brain state saved to: {checkpoint_path}")

# Load brain state
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.initialize()
new_brain.load(checkpoint_path)

print(f"Loaded brain has {new_brain.getTotalNeuronCount()} neurons")

# Clean up
os.unlink(checkpoint_path)
```

## AgentBrain Class

The `AgentBrain` class provides an interface for connecting the brain to the environment, handling sensory processing and motor decoding.

### Creation and Initialization

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
```

### With World Integration

```python
import pynlm

# Create all components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable subsystems for learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation loop
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Simple reward function
    reward = 0.0
    if world.getAgentBody().x > world.getWidth() * 0.8:
        reward = 1.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)

print(f"Simulation complete. Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
```

### Advanced Agent Configuration

```python
import pynlm

# Create configurable agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Configure subsystem parameters
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(False)  # Disable structural plasticity
agent.enableDevelopment(True)  # Enable neural development
agent.enableCuriosity(True)  # Enable curiosity-driven exploration

# Check subsystem status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
print(f"Development enabled: {agent.isDevelopmentEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
```

## SimpleWorld Class

The `SimpleWorld` class provides a 2D simulation environment for testing agents.

### Basic World Setup

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=30,
    height=30,
    visionWidth=8,
    visionHeight=8
)

# Reset to initial state
world.reset()

# Set agent starting position
world.setAgentStart(15.0, 15.0)
```

### World Interaction

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Get agent position and orientation
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent orientation: {body.orientation}")

# Move agent
import pynlm
from pynlm import MotorCommand

move_forward = MotorCommand(MotorCommand::MoveForward)
world.applyMotorCommand(move_forward, world.getSimulationTime())

# Get sensory input
percept = world.getSensoryPercept()
vision = percept.getVision()
print(f"Vision data: {len(vision)} values")

# Get world statistics
print(f"World width: {world.getWidth()}")
print(f"World height: {world.getHeight()}")
print(f"Simulation time: {world.getSimulationTime()}")
```

## Configuration Management

The `Config` class provides a flexible configuration system for all NLM components.

### Basic Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set individual values
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.1)
config.set("simulation_timestep", 0.001)
config.set("plasticity.stdp.learning_rate", 0.001)

# Get values
neuron_count = config.get("brain.neuron_count", 1000)
sim_timestep = config.get("simulation_timestep", 0.001)
```

### Advanced Configuration with Pythonic Interface

```python
import pynlm

# Create configuration using Pythonic setters
config = pynlm.createDefaultConfig()

# Use keyword arguments for multiple values
config.set(
    brain_neuron_count=2000,
    brain_connection_probability=0.05,
    simulation_timestep=0.0005,
    plasticity_stdp_learning_rate=0.01,
    neuromod_dopamine_scale=1.5,
    neuromod_curiosity_enable=True,
    neuromod_novelty_enable=True
)

# Validate configuration
is_valid, errors = config.validate_config()
if not is_valid:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid")

# Get all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Save configuration to file
config.saveToFile("my_config.txt")
```

### Configuration Examples

```python
import pynlm

# Example 1: High-performance configuration
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 5000)
config1.set("brain.connection_probability", 0.15)
config1.set("simulation_timestep", 0.0001)
config1.set("plasticity.stdp.learning_rate", 0.005)

# Example 2: Learning-focused configuration
config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 1000)
config2.set("brain.connection_probability", 0.2)
config2.set("plasticity.stdp.learning_rate", 0.01)
config2.set("plasticity.hebbian.enable", True)
config2.set("neuromod.curiosity.enable", True)
config2.set("neuromod.novelty.enable", True)

# Example 3: Developmental configuration
config3 = pynlm.createDefaultConfig()
config3.set("brain.neuron_count", 2000)
config3.set("development_synaptogenesis_rate", 0.002)
config3.set("development_pruning_rate", 0.0005)
config3.set("neuromod.dopamine_baseline", 0.1)
```

## Simulation Examples

### Example 1: Silent Brain Test

```python
import pynlm

def silent_brain_test():
    """Test basic brain functionality without external input."""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(10):
        brain.step(i)
    
    print("Silent brain test complete!")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    return brain

brain = silent_brain_test()
```

### Example 2: Brain-World Interaction

```python
import pynlm

def brain_world_interaction():
    """Demonstrate brain interacting with a world."""
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        
    print("Watched world for 30 steps")
    print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    return brain, world

brain, world = brain_world_interaction()
```

### Example 3: Complete Agent Simulation

```python
import pynlm

def run_complete_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with all learning subsystems."""
    
    # 1. Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # 2. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 3. Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reward based on position
        body = world.getAgentBody()
        reward = 0.0
        if body.x > world.getWidth() * 0.8:
            reward = 1.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation complete!")
    return brain, agent, world

brain, agent, world = run_complete_agent_simulation(500)
```

## Best Practices

### 1. Use Factory Functions for Common Patterns

```python
# Instead of creating components individually, use factory functions
brain, agent, world = pynlm.create_complete_agent(
    config,
    worldConfig={"width": 20, "height": 20, "visionWidth": 8, "visionHeight": 8}
)
```

### 2. Enable Appropriate Learning Subsystems

```python
# Enable subsystems based on your experimental needs
agent.enableRewardModulation(True)      # For value-based learning
agent.enableCuriosity(True)            # For intrinsic motivation
agent.enableDevelopment(True)          # For neural development
agent.enableStructuralPlasticity(True)  # For structural changes
```

### 3. Use Configuration Validation

```python
# Always validate your configuration before running
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

is_valid, errors = config.validate_config()
if not is_valid:
    raise ValueError(f"Invalid configuration: {errors}")
```

### 4. Handle Errors Gracefully

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Your simulation code here
    
except Exception as e:
    print(f"Error: {e}")
    # Clean up resources
    if 'brain' in locals():
        brain.reset()
```

### 5. Use Simulation Checkpoints

```python
import tempfile
import os

# Save checkpoints for long simulations
with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
    checkpoint_path = f.name

brain.save(checkpoint_path)

# Later, resume from checkpoint
if os.path.exists(checkpoint_path):
    new_brain = pynlm.createBrain(config)
    new_brain.initialize()
    new_brain.load(checkpoint_path)
```

## Advanced Usage

### Custom Agent Behavior

```python
import pynlm

class CustomAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.agent.enableRewardModulation(True)
    
    def run_episode(self, max_steps=100):
        for step in range(max_steps):
            # Custom sensing logic
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Custom thinking logic
            self.brain.step(step)
            
            # Custom action selection
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Custom reward function
            reward = self.calculate_reward()
            self.agent.applyRewardModulation(reward, 0.0)
    
    def calculate_reward(self):
        # Custom reward calculation
        body = self.world.getAgentBody()
        return 1.0 if body.x > self.world.getWidth() * 0.8 else 0.0

# Use custom agent
custom_agent = CustomAgent(brain, world)
custom_agent.run_episode(200)
```

### Experiment Management

```python
import pynlm
from pynlm import Phase6Config, Phase6IntegratedExperiment

# Create experiment configuration
config = Phase6Config()
config.neuronCount = 500
config.maxSteps = 2000
config.enableCheckpointing = True
config.enableReplay = True
config.enableDevelopment = True

# Run integration experiment
experiment = Phase6IntegratedExperiment()
result = experiment.run(config)

# Analyze results
print(f"Total reward: {result.totalReward}")
print(f"Average firing rate: {result.avgFiringRate}")
print(f"Episodes stored: {result.memoryEpisodesStored}")
print(f"Dopamine level: {result.dopamineLevel}")

# Check integration status
print(f"Working Memory integrated: {result.memoryWorkingMemoryIntegrated}")
print(f"Episodic Memory integrated: {result.memoryEpisodicMemoryIntegrated}")
print(f"Neuromodulation integrated: {result.neuromodulationIntegrated}")
```

### Performance Monitoring

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

start_time = time.time()
for step in range(1000):
    brain.step(step)
    
    # Monitor performance
    if step % 100 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step}: {elapsed:.2f}s elapsed, "
              f"{brain.getAverageFiringRate():.2f} avg firing rate")

elapsed = time.time() - start_time
print(f"Completed 1000 steps in {elapsed:.2f}s")
print(f"Performance: {1000/elapsed:.0f} steps/second")
```

## API Reference

### Classes

#### Brain
- `createBrain(config)` - Create a new brain
- `initialize()` - Initialize the brain
- `step(step)` - Perform a simulation step
- `step(step, time)` - Perform a simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input
- `produceAction()` - Get motor action from neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

#### AgentBrain
- `createAgentBrain(brain)` - Create a new agent brain
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command from brain activity
- `applyRewardModulation(reward, predictedReward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development system

#### SimpleWorld
- `createSimpleWorld()` - Create a new simple world
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, time)` - Apply motor command

#### Config
- `createDefaultConfig()` - Create default configuration
- `set(key, value, source)` - Set configuration value
- `get(key, default=None)` - Get configuration value
- `has(key)` - Check if configuration key exists
- `validate_config()` - Validate configuration parameters
- `saveToFile(filepath)` - Save configuration to file

### Enumerations

- `NeuronType` - Neuron types (Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal)
- `ActionType` - Action types
- `MotorCommand` - Motor commands
- `DevelopmentalStage` - Developmental stages
- `FiringState` - Neuron firing states
- `WorldObjectType` - World object types

### Functions

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain
- `createSimpleWorld()` - Create simple world
- `createAgentBrain(brain)` - Create agent brain

## Troubleshooting

### Common Issues and Solutions

**Issue**: Brain initialization fails
```python
# Solution:
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Ensure reasonable neuron count
brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Brain initialization failed")
```

**Issue**: Simulation is not progressing
```python
# Solution:
# Check that brain.step() is being called
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)  # Make sure to call this
```

**Issue**: Agent is not responding to sensory input
```python
# Solution:
# Ensure sensory processing is connected
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

percept = world.getSensoryPercept()
agent.processSensoryInput(percept)  # Process input before brain.step()
```

**Issue**: Configuration validation errors
```python
# Solution:
config = pynlm.createDefaultConfig()
is_valid, errors = config.validate_config()
if not is_valid:
    print("Fix these errors:", errors)
```

### Getting Help

For more information:
- Check the NLM documentation at https://nlm.readthedocs.io
- Visit the NLM GitHub repository
- Join the NLM community forums

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Authors

NLM Research Project - https://nlm-project.org

## Version

Python API v0.1.0
