# Python Bindings Documentation for NLM

This document provides comprehensive documentation for the NLM Python bindings (`pynlm`). It includes API reference, examples, and best practices for using the neural simulation framework in Python.

## Overview

The NLM Python bindings provide access to the full NLM (Neural Learning Machine) neural simulation framework. NLM implements a brain-inspired spiking neural network simulator with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

### Key Features

- **Realistic neuron models**: Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics
- **Multiple plasticity rules**: STDP, Hebbian learning, and structural plasticity
- **Memory systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Cognition**: Neural planning, concept formation, attentional selection
- **Full ecosystem**: Brain, world, agent integration

## Installation

```bash
# Install from source
pip install pynlm

# Or install in development mode
pip install -e .

# Build requirements (if installing from source)
pip install scikit-build-core pybind11 pytest numpy
```

## Basic Usage

### Quick Start

Here's the simplest way to create and run a brain simulation:

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Simulation

A full simulation with brain, world, and agent:

```python
import pynlm

# Create complete agent system
brain, world, agent = pynlm.create_agent_world()

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation loop
for step in range(500):
    world.update(0.1)  # Update world physics
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Brain processes sensory input
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Brain decides action
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

## API Reference

### Core Classes

#### Config
Configuration management for NLM parameters:

```python
# Create default config
config = pynlm.createDefaultConfig()

# Set parameters
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Load from file
config.loadFromFile("config.json")

# Save to file
config.saveToFile("my_config.json")

# Get values
if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count")

# Get with default
vision_width = config.getOr("world.vision_width", 64)
```

#### Brain
The central neural simulation engine:

```python
# Create brain with config
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simple simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Regions and regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Developmental stages
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
stage = brain.getDevelopmentalStage()

# Save and load state
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

#### SimpleWorld
The simulation environment:

```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(width=200, height=200, visionWidth=16, visionHeight=16)

# Set agent starting position
world.setAgentStart(100.0, 100.0)

# Reset world
world.reset()

# Get agent state
body = world.getAgentBody()
print(f"Position: ({body.x}, {body.y})")
print(f"Velocity: ({body.velocityX}, {body.velocityY})")

# Apply actions
action = pynlm.MotorCommand.MoveForward
world.applyMotorCommand(action, world.getSimulationTime())
```

#### AgentBrain
Interface between brain and world:

```python
# Create agent brain
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get motor command
action = agent.decodeMotorCommand()

# Apply reward-based learning
reward = 1.0  # Example reward
agent.applyRewardModulation(reward, predicted_reward=0.8)

# Update development
agent.updateDevelopment(0.1)

# Enable features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

#### SensoryInput Classes

##### Vision
```python
# Create vision data
vision_data = [0.1, 0.5, 0.8, ...]  # Flattened RGB values
vision = pynlm.vision_data(64, 64, vision_data)

# Create default vision
vision = pynlm.create_vision(128, 128, 3)
```

##### Audio
```python
# Create audio data
audio_data = [0.0, 0.1, 0.0, ...]  # Audio samples
audio = pynlm.create_audio(44100, len(audio_data))
```

##### InternalSignals
```python
# Create internal signals
internal = pynlm.create_internal_signals()
internal.addSignal(0.5)  # Add signal value
```

### Helper Functions

#### Context Managers

The main classes support context managers for safe resource management:

```python
# Using Brain as context manager
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for i in range(100):
        brain.step(i)
    print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")

# Using Config as context manager
with pynlm.createDefaultConfig() as config:
    config.set("brain.neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
```

#### Convenience Functions

```python
# Run a complete episode with automatic state management
result = pynlm.run_episode(world, brain, agent, steps=100)
spike_count, firing_rate, curiosity = result

# Create simulation with statistics
firing_rates, spike_counts, avg_rate, total_spikes = pynlm.run_simulation(brain, 1000)

# Load configuration
config = pynlm.load_config("saved_config.json")

# Create complete system
brain, world, agent = pynlm.create_agent_world()
```

#### Advanced Functions

```python
# Create brain with research defaults
config = pynlm.create_default_config()
brain = pynlm.create_brain()

# Create vision with validation
vision = pynlm.vision_data(32, 32, [0.1] * 32 * 32 * 3)
```

## Advanced Usage

### Learning and Plasticity

```python
import pynlm

# Create brain with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable plasticity
if hasattr(brain, 'enablePlasticity'):
    brain.enablePlasticity(True)

# Run learning simulation
for step in range(2000):
    # Inject current to simulate input
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
    
    brain.step(step)
    
    if step % 200 == 0:
        print(f"Step {step}: {brain.getAverageFiringRate():.2f} Hz")
        
# Check if learning occurred
total_spikes_before = brain.getTotalSpikeCount()
print(f"Total learning spikes: {total_spikes_before}")
```

### Multiple Regions

```python
import pynlm

# Create brain with multiple regions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add different regions
motor_region = brain.addRegion("motor_cortex")
sensory_region = brain.addRegion("sensory_cortex")

# Get regions
motor = brain.getRegion(motor_region)
sensory = brain.getRegion(sensory_region)

# Work with specific regions
print(f"Motor region neuron count: {motor.getNeuronCount()}")
print(f"Sensory region neuron count: {sensory.getNeuronCount()}")
```

### Development Stages

```python
import pynlm
import time

brain, world, agent = pynlm.create_agent_world()
agent.enableDevelopment(True)

# Simulate development over time
for episode in range(10):
    print(f"\nEpisode {episode + 1}:")
    print(f"  Initial stage: {agent.getDevelopmentalStage()}")
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1)  # Advance development
    
    print(f"  Final stage: {agent.getDevelopmentalStage()}")
    print(f"  Plasticity modifier: {agent.getNeuromodulationLevel():.3f}")
```

### Error Handling

```python
import pynlm

# Safe configuration loading
try:
    config = pynlm.load_config("my_config.json")
    print("Configuration loaded successfully")
except (IOError, ValueError) as e:
    print(f"Error loading config: {e}")
    config = pynlm.createDefaultConfig()

# Safe simulation with error handling
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(1000):
        brain.step(i)
        
except RuntimeError as e:
    print(f"Simulation error: {e}")
    brain.reset()
    
finally:
    print("Simulation ended")
```

## Best Practices

### 1. Use Context Managers

```python
# Recommended: Use context managers for automatic cleanup
with pynlm.createBrain(config) as brain:
    brain.initialize()
    run_simulation(brain)

# Recommended: Proper error handling
try:
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        run_simulation(brain)
except RuntimeError as e:
    print(f"Simulation failed: {e}")
```

### 2. Enable Learning Features

```python
# Enable all learning features for realistic behavior
brain.enableRewardModulation(True)
brain.enableCuriosity(True)
brain.enableDevelopment(True)
brain.enableStructuralPlasticity(True)
```

### 3. Monitor Performance

```python
import time

brain, world, agent = pynlm.create_agent_world()

start_time = time.time()
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Performance: {100 / (end_time - start_time):.1f} steps/second")
```

### 4. Use Helper Functions

```python
# Use helper functions for common patterns
# Instead of manual setup, use convenience functions
brain, world, agent = pynlm.create_agent_world()

# Or run complete episode with automatic state management
results = pynlm.run_episode(world, brain, agent, steps=200)
spike_count, firing_rate, curiosity = results
```

## Common Patterns

### Pattern 1: Simple Sensorimotor Loop

```python
def run_simple_loop(brain, world, agent, steps):
    """Run a basic sensorimotor loop."""
    for step in range(steps):
        # Perceive
        percept = world.getSensoryPercept()
        
        # Process
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Act
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn (if enabled)
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, agent.getPredictionError())
    
    return brain.getTotalSpikeCount()
```

### Pattern 2: Goal-Directed Behavior

```python
def run_goal_directed(brain, world, agent, target_position, max_steps):
    """Run simulation toward a target."""
    agent_target_x, agent_target_y = target_position
    
    for step in range(max_steps):
        # Get current position
        body = world.getAgentBody()
        distance = ((body.x - agent_target_x)**2 + (body.y - agent_target_y)**2)**0.5
        
        # Stop if reached target
        if distance < 2.0:
            break
        
        # Update world and get percept
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Provide reward based on progress
        progress_reward = 1.0 / (distance + 1.0)
        agent.applyRewardModulation(progress_reward, 0.0)
    
    return distance, brain.getTotalSpikeCount()
```

## Troubleshooting

### Common Issues

**Issue: Brain doesn't produce spikes**

```python
# Check if neurons are initialized properly
brain.initialize()
brain.logStatus()  # Check brain status

# Verify configuration
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Connection probability: {config.getOr('brain.connection_probability', 0.0)}")
```

**Issue: Agent doesn't move**

```python
# Check if motor neurons are active
print(f"Motor neuron count: {brain.getMotorNeuronCount()}")

# Verify action decoding
action = agent.decodeMotorCommand()
print(f"Decoded action: {action}")

# Check world state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
```

**Issue: Memory usage is high**

```python
# Use smaller brain for testing
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.05)

# Run with smaller simulation
pynlm.run_simulation(brain, 100)
```

### Getting Help

If you encounter issues:

1. **Check documentation**: Review this document and the existing documentation files
2. **Check examples**: Look at the example scripts in the repository
3. **Check configuration**: Verify your configuration settings
4. **Check logs**: Use `brain.logStatus()` to debug issues
5. **Check errors**: Review error messages and exception handling

## Migration Guide

### From C++ to Python

If you're migrating from C++ NLM usage:

**C++ Example:**
```cpp
// C++
Brain brain(config);
brain.initialize();
for (int i = 0; i < 100; ++i) {
    brain.step(i);
}
```

**Python Equivalent:**
```python
# Python
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
```

### From Version 5 to Version 6

Version 6 added context managers and helper functions. The API remains largely compatible.

```python
# Old way (still works)
brain = pynlm.createBrain(config)
brain.initialize()

# New recommended way
with pynlm.createBrain(config) as brain:
    brain.initialize()
```

## References

1. **NLM Project Documentation**: Available in the `docs/` directory
2. **Phase 6 Integration**: See `README.md` for integration testing procedures
3. **Scientific Background**: Available in `docs/SCIENCE.md`
4. **Architecture**: Available in `docs/ARCHITECTURE.md`

## Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)
- [Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Experiments](docs/EXPERIMENTS.md)

## Acknowledgments

The NLM Python bindings are based on the original C++ implementation and provide a Python interface to the neural simulation framework.

---

*This documentation is continuously updated with new features and examples. For the latest information, refer to the official documentation.*