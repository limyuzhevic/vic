# Python API Documentation for NLM (Neural Learning Machine)

## Overview

This document provides comprehensive documentation for the NLM Python API, which allows you to interact with the NLM neural simulation framework programmatically.

## Installation

The NLM Python package is built as a native extension module that wraps the C++ neural simulation framework. To install it:

```bash
pip install pynlm
```

The package uses scikit-build with pybind11 for the bindings, so it will compile the native extensions during installation.

## Basic Usage

### Creating a Brain with Configuration

```python
import pynlm as nlm

# Create a default configuration
config = nlm.createDefaultConfig()

# Configure basic parameters
config.set("neuron_count", 500)
config.set("region_count", 1)
config.set("connection_probability", 0.15)
config.set("random_seed", 42)

# Create a brain with the configuration
brain = nlm.createBrain(config)

# Initialize the brain
brain.initialize()
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
print(f"Brain initialized with {brain.getTotalSynapseCount()} synapses")
```

### Simple Neural Simulation

```python
import pynlm as nlm
import time

# Create a brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

print("Starting simple neural simulation...")

# Inject sensory input to stimulate activity
# This simulates receiving visual input
for i in range(10):
    brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 50.0)
    brain.step(i, i * 0.001)

print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Working with the World Interface

```python
import pynlm as nlm

# Create a simple world
world = nlm.createSimpleWorld()
world.configure(width=20.0, height=20.0, visionWidth=16, visionHeight=16)
world.setRandomSeed(42)

# Set agent starting position
world.setAgentStart(5.0, 5.0)

# Get sensory percept (vision)
percept = world.getSensoryPercept()
print(f"Vision size: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
print(f"Vision data length: {len(percept.getVision())}")

# Get agent body state
agent = world.getAgentBody()
print(f"Agent position: ({agent.x}, {agent.y})")
print(f"Agent orientation: {agent.orientation} radians")
```

### Complete Agent-Based Simulation

```python
import pynlm as nlm
import random

# Create simulation components
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()

# Configure the world
world.configure(30.0, 30.0, 20, 20)
world.setAgentStart(5.0, 5.0)

# Create agent brain interface
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Enable all systems
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)

print("Starting agent simulation...")

# Run simulation steps
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)

    # Run brain dynamics
    for neural_step in range(10):
        brain.step(step * 10 + neural_step, step * 0.1 + neural_step * 0.001)

    # Produce motor action
    motor_command = agent_brain.decodeMotorCommand()
    result = world.applyMotorCommand(motor_command, world.getSimulationTime())

    # Apply reward modulation
    agent_brain.applyRewardModulation(result.reward, 0.0)

    # Update development
    agent_brain.updateDevelopment(0.1)

    # Print progress
    if step % 20 == 0:
        print(f"Step {step}: Action={motor_command}, Reward={result.reward:.2f}, "
              f"Dopamine={agent_brain.getNeuromodulationLevel():.3f}, "
              f"Curiosity={agent_brain.getCuriosityLevel():.3f}, "
              f"Energy={world.getAgentBody().energy:.2f}")

print(f"Simulation complete. Final energy: {world.getAgentBody().energy:.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Developmental stage: {agent_brain.getDevelopmentalStage()}")
```

## Advanced Features

### Working Memory and Episodic Memory

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

print("Memory systems available:")
print(f"  Working Memory: {'Available' if working_memory else 'Not Available'}")
print(f"  Episodic Memory: {'Available' if episodic_memory else 'Not Available'}")
print(f"  Associative Memory: {'Available' if associative_memory else 'Not Available'}")
```

### Neuromodulation Systems

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
agent_brain = nlm.createAgentBrain(brain)

# Access neuromodulation components
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

print("Neuromodulation levels:")
print(f"  Dopamine: {dopamine.getLevel():.3f}")
print(f"  Curiosity: {curiosity.getLevel():.3f}")
print(f"  Novelty: {novelty.getLevel():.3f}")
print(f"  Prediction Error: {prediction_error.getSignal():.3f}")
```

### Plasticity Control

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Access plasticity systems
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural = brain.getStructuralPlasticity()

print("Plasticity systems:")
print(f"  STDP: {'Enabled' if stdp else 'Not Available'}")
if stdp:
    print(f"    LTP Weight: {stdp.getLTPWeight():.4f}")
    print(f"    LTD Weight: {stdp.getLTDWeight():.4f}")

print(f"  Hebbian: {'Enabled' if hebbian else 'Not Available'}")
print(f"  Structural Plasticity: {'Enabled' if structural else 'Not Available'}")
if structural:
    print(f"    Synaptogenesis Rate: {structural.getSynaptogenesisRate():.6f}")
    print(f"    Pruning Rate: {structural.getPruningRate():.6f}")
```

### Concept Formation and Cognition

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
agent_brain = nlm.createAgentBrain(brain)

# Access cognitive systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

print("Cognitive systems:")
print(f"  Neural Planner: {'Available' if planner else 'Not Available'}")
print(f"  Concept Formation: {'Available' if concept_formation else 'Not Available'}")
print(f"  Attention System: {'Available' if attention else 'Not Available'}")
```

### Prediction and Learning

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
agent_brain = nlm.createAgentBrain(brain)

# Access prediction system
prediction = brain.getPredictionSystem()

if prediction:
    print("Prediction system status:")
    print(f"  Prediction error: {agent_brain.getPredictionError():.3f}")
    print(f"  Prediction confidence: {prediction.getConfidence():.3f}")
    print(f"  Prediction active: {prediction.isActive()}")
```

### Developmental Stages

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

print("Developmental stages:")
for stage in nlm.DevelopmentalStage:
    print(f"  {stage.name}: {stage.value}")

# Demonstrate developmental progression
print("\nSimulating development...")
brain.setDevelopmentalStage(nlm.DevelopmentalStage.Initial)
print(f"Stage 1 - Initial: Plasticity modifier = {brain.getDevelopmentSystem().getPlasticityModifier():.2f}")

brain.setDevelopmentalStage(nlm.DevelopmentalStage.CriticalPeriod)
print(f"Stage 2 - Critical Period: Plasticity modifier = {brain.getDevelopmentSystem().getPlasticityModifier():.2f}")

brain.setDevelopmentalStage(nlm.DevelopmentalStage.Maturation)
print(f"Stage 3 - Maturation: Plasticity modifier = {brain.getDevelopmentSystem().getPlasticityModifier():.2f}")

brain.setDevelopmentalStage(nlm.DevelopmentalStage.Adult)
print(f"Stage 4 - Adult: Plasticity modifier = {brain.getDevelopmentSystem().getPlasticityModifier():.2f}")
```

## Configuration Management

### Creating and Saving Configurations

```python
import pynlm as nlm
import json

# Create configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 3)
config.set("connection_probability", 0.2)
config.set("random_seed", 123)

# Save to file
config.saveToFile("my_config.json")
print("Configuration saved to 'my_config.json'")

# Load from file
loaded_config = nlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")
print("Configuration loaded successfully")

# Get configuration summary
print(f"Loaded config: {loaded_config.summary()}")
```

### Working with Action Types

```python
import pynlm as nlm

# Create different action types
action_types = [
    nlm.ActionType.MoveForward,
    nlm.ActionType.MoveBackward,
    nlm.ActionType.TurnLeft,
    nlm.ActionType.TurnRight,
    nlm.ActionType.Interact,
    nlm.ActionType.Look,
    nlm.ActionType.LookUp,
    nlm.ActionType.LookDown,
    nlm.ActionType.Eat,
    nlm.ActionType.Drink,
    nlm.ActionType.Rest,
    nlm.ActionType.Wait,
    nlm.ActionType.Custom
]

print("Available action types:")
for action_type in action_types:
    print(f"  {action_type.name}: {action_type.value}")

# Create specific actions
action1 = nlm.Action(nlm.ActionType.MoveForward)
print(f"\nCreated action: {action1.getName()} (type: {action1.getType()})")

# Action with parameters
action2 = nlm.Action(nlm.ActionType.Custom, [0.5, 0.8, 1.2])
print(f"Created action with parameters: {action2.getParameters()}")
```

## Error Handling

```python
import pynlm as nlm

# Configuration validation
config = nlm.createDefaultConfig()
try:
    config.set("invalid_key", "some_value")
    print("This should not happen - config set accepted invalid key")
except Exception as e:
    print(f"Configuration validation works: {type(e).__name__}: {e}")

# Brain initialization
try:
    brain = nlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully")
except Exception as e:
    print(f"Brain initialization failed: {type(e).__name__}: {e}")
```

## Performance and Monitoring

### Monitoring Brain State

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

print("Brain performance metrics:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Active neurons: {brain.getActiveNeuronCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"  Excitation/inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

### Long-running Simulation Example

```python
import pynlm as nlm
import time

config = nlm.createDefaultConfig()
world = nlm.createSimpleWorld()
world.configure(50.0, 50.0, 32, 32)
world.setAgentStart(10.0, 10.0)

brain = nlm.createBrain(config)
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

print("Starting long-running simulation...")
start_time = time.time()

for step in range(500):
    # Process sensory input
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)

    # Run neural dynamics
    for i in range(20):
        brain.step(step * 20 + i, step * 0.05 + i * 0.001)

    # Generate action
    action = agent_brain.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())

    # Apply learning
    agent_brain.applyRewardModulation(result.reward, 0.0)

    # Update development
    agent_brain.updateDevelopment(0.05)

    # Log periodically
    if step % 100 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step} ({elapsed:.1f}s): "
              f"Action={action}, Reward={result.reward:.2f}, "
              f"Energy={world.getAgentBody().energy:.2f}, "
              f"Spikes={brain.getTotalSpikeCount()}")

elapsed = time.time() - start_time
print(f"\nSimulation complete in {elapsed:.1f} seconds")
print(f"Final energy: {world.getAgentBody().energy:.2f}")
print(f"Final developmental stage: {agent_brain.getDevelopmentalStage()}")
```

## Saving and Loading Brain States

### Checkpointing for Long Simulations

```python
import pynlm as nlm
import os

# Create and run brain simulation
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

print("Simulating and creating checkpoints...")

# Run for some time
for step in range(100):
    brain.step(step, step * 0.001)

# Save checkpoint
checkpoint_file = "brain_checkpoint.nnet"
if brain.save(checkpoint_file):
    print(f"Checkpoint saved: {checkpoint_file}")
else:
    print("Failed to save checkpoint")

# Continue simulation from checkpoint
brain.reset()
brain.initialize()
if brain.load(checkpoint_file):
    print("Checkpoint loaded successfully")
    print(f"Loaded brain has {brain.getTotalNeuronCount()} neurons")
    print(f"Loaded brain has {brain.getTotalSynapseCount()} synapses")
else:
    print("Failed to load checkpoint")

# Clean up
if os.path.exists(checkpoint_file):
    os.remove(checkpoint_file)
    print("Checkpoint file cleaned up")
```

## Advanced Topics

### Custom Configuration

```python
import pynlm as nlm

# Create a custom configuration with specific settings
config = nlm.createDefaultConfig()

# Environment settings
config.set("environment_width", 100)
config.set("environment_height", 100)
config.set("vision_width", 20)
config.set("vision_height", 20)

# Brain settings
config.set("neuron_count", 2000)
config.set("region_count", 5)
config.set("connection_probability", 0.05)
config.set("random_seed", 42)

# Plasticity settings
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.015)
config.set("stdp_tau", 30.0)
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00001)

# Development settings
config.set("developmental_stage", "Maturation")
config.set("plasticity_modifier", 0.5)

print("Custom configuration created successfully")
print(f"Configuration: {config.summary()}")
```

### Batch Processing

```python
import pynlm as nlm
import time

# Process multiple episodes with the same configuration
template_config = nlm.createDefaultConfig()
config = nlm.createDefaultConfig()

# Modify configuration
config.set("neuron_count", 500)
config.set("region_count", 1)

# Run multiple episodes
num_episodes = 5
episode_rewards = []

print(f"Running {num_episodes} episodes...")

for episode in range(num_episodes):
    print(f"\n--- Episode {episode} ---")
    
    # Create fresh brain for each episode
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run episode
    episode_reward = 0.0
    for step in range(200):
        brain.step(step, step * 0.001)
        episode_reward += 0.1  # Dummy reward
    
    episode_rewards.append(episode_reward)
    print(f"Episode {episode} reward: {episode_reward:.2f}")

# Calculate statistics
avg_reward = sum(episode_rewards) / len(episode_rewards)
max_reward = max(episode_rewards)
min_reward = min(episode_rewards)

print(f"\nEpisode Statistics:")
print(f"  Average reward: {avg_reward:.2f}")
print(f"  Maximum reward: {max_reward:.2f}")
print(f"  Minimum reward: {min_reward:.2f}")
print(f"  Reward variance: {sum((r - avg_reward) ** 2 for r in episode_rewards) / len(episode_rewards):.2f}")
```

## Troubleshooting

### Common Issues and Solutions

1. **Initialization Failures**
   ```python
   try:
       brain.initialize()
   except Exception as e:
       print(f"Initialization failed: {e}")
       # Check if configuration is valid
       print(f"Config has 'neuron_count': {config.has('neuron_count')}")
       print(f"Config has 'region_count': {config.has('region_count')}")
   ```

2. **Memory Issues**
   ```python
   # Monitor memory usage
   brain = nlm.createBrain(config)
   brain.initialize()
   
   # Large brain simulation - ensure enough system memory
   print(f"Brain memory usage: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
   ```

3. **Configuration Errors**
   ```python
   # Validate configuration before using
   config = nlm.createDefaultConfig()
   if not config.has("neuron_count"):
       config.set("neuron_count", 100)
   ```

### Debugging Tips

```python
import pynlm as nlm

# Enable debug logging
config = nlm.createDefaultConfig()
config.set("log_level", "Debug")

brain = nlm.createBrain(config)
brain.initialize()
brain.logStatus()

# Step through simulation one step at a time
for step in range(50):
    brain.step(step, step * 0.001)
    brain.logStatus()  # Log after each step
```

## References

- NLM (Neural Learning Machine) Documentation
- pybind11 Python bindings
- scikit-build build system
- NEURON simulation framework concepts

## License

MIT License
Copyright (c) 2024 NLM Project
