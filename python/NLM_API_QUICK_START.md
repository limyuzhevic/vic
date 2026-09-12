# NLM Python API - Quick Start

This document provides a quick guide to using the enhanced NLM Python API.

## What Changed

The NLM Python API has been significantly enhanced with:

- **Complete Brain System Access**: All memory, cognition, neuromodulation, and plasticity systems are now accessible from Python
- **Helper Methods**: Convenience functions for common operations
- **Enhanced Documentation**: Comprehensive docstrings and examples
- **Pythonic Interface**: Methods designed for easy Python use

## Installation

The NLM Python bindings are included in the main build. You can install them with:

```bash
pip install -e .
```

Or run tests with:

```bash
python test_nlm_api.py
```

## Basic Usage

### 1. Create a Brain

```python
import nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Create brain with configuration
brain = nlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Regions: {brain.getRegionCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
```

### 2. Create World and Agent

```python
# Create world
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create agent brain interface
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
```

### 3. Run Simulation

```python
# Run complete simulation
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process sensory input in brain
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: neurons={brain.getFiringNeuronCount()}, curiosity={agent.getCuriosityLevel():.3f}")
```

## New Enhanced Features

### 1. Brain System Access

```python
# Access all brain systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()
prediction_system = brain.getPredictionSystem()

# Get cognition systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Get plasticity systems
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural_plasticity = brain.getStructuralPlasticity()
```

### 2. Helper Methods

```python
# Get region statistics as dictionary
region_stats = brain.getRegionStatistics()
for region_id, stats in region_stats.items():
    print(f"Region {region_id}: {stats['neuron_count']} neurons, {stats['firing_rate_hz']} Hz firing rate")

# Get top firing neurons
top_neurons = brain.getTopFiringNeurons(region_id, 10)
print(f"Top firing neurons: {len(top_neurons)} neurons")

# Get system state information
memory_stats = brain.getMemoryStats()
neuromod_state = brain.getNeuromodulationState()
plasticity_state = brain.getPlasticityState()

# Get development status
development_stage = brain.getDevelopmentalStage()
is_developing = brain.isDecreasingPlasticity()

# Check if memory replay should occur
if brain.shouldReacquire(step=100):
    print("Memory replay should occur at this step")
```

### 3. Action and Percept Classes

```python
# Create and modify actions
action = nlm.Action(nlm.ActionType.MoveForward)
action.setParameters([0.5, 0.3])
action_dict = nlm.Action.action_to_dict(action)

# Convert to dictionary for storage or serialization
new_action = nlm.Action.action_from_dict(action_dict)

# Percept handling
percept = nlm.SensoryPercept()
percept.setVision([0.1] * 256)
percept.setTouch([0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0])
percept_dict = nlm.SensoryPercept.percept_to_dict(percept)

# Create percept from dictionary
new_percept = nlm.SensoryPercept.percept_from_dict(percept_dict)
```

### 4. Enhanced Configuration

```python
# Python-friendly configuration access
config = nlm.createDefaultConfig()

# Set configuration values
config.set("test_int", 42)
config.set("test_float", 3.14)
config.set("test_bool", True)
config.set("test_string", "hello world")

# Get configuration values with type safety
int_val = config.get_int("test_int")
float_val = config.get_float("test_float")
bool_val = config.get_bool("test_bool")
string_val = config.get("test_string")

# List all configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")
```

### 5. Simulation Patterns

```python
# Pattern 1: Simple brain simulation
def run_brain_simulation(brain, num_steps):
    for step in range(num_steps):
        brain.step(step)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Pattern 2: Complete agent simulation
def run_agent_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward if needed
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)

# Pattern 3: Learning simulation with monitoring
def run_learning_simulation(brain, world, agent, num_steps):
    learning_stats = {
        'total_spikes': [],
        'curiosity_levels': [],
        'action_counts': {},
        'development_stages': []
    }
    
    for step in range(num_steps):
        # ... simulation steps ...
        
        # Collect statistics
        learning_stats['total_spikes'].append(brain.getTotalSpikeCount())
        learning_stats['curiosity_levels'].append(agent.getCuriosityLevel())
        learning_stats['development_stages'].append(brain.getDevelopmentalStage())
        
        # Log periodically
        if step % 100 == 0:
            print(f"Step {step}: spikes={brain.getTotalSpikeCount()}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"dev_stage={brain.getDevelopmentalStage()}")
    
    return learning_stats
```

## Module Information

```python
# Module metadata
print(f"NLM Version: {nlm.__version__}")
print(f"Author: {nlm.__author__}")
print(f"Description: {nlm.__description__}")
```

## Convenience Functions

```python
# Quick test function
brain = nlm.quick_test()

# Complete simulation helper
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

## Advanced Features

For more advanced usage, you can:

1. **Access low-level systems**: Direct access to all neural components
2. **Monitor system state**: Get real-time statistics from all subsystems
3. **Configure extensively**: Set any parameter using configuration system
4. **Save and load states**: Checkpoint your brain for later use
5. **Run complex experiments**: Use the experiment framework

## Next Steps

1. **Read `easy_usage.md`**: For simple beginner examples
2. **Read `HOW_TO_USE.md`**: For comprehensive documentation
3. **Read `docs/ARCHITECTURE.md`**: For technical architecture details
4. **Run tests**: `python test_nlm_api.py` to verify the installation
5. **Experiment**: Try different configurations and simulations

## Common Patterns

### Silent Brain Test
```python
import nlm
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
for i in range(10):
    brain.step(i)
print("Silent brain test done!")
```

### Brain Watching a World
```python
import nlm
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
print(f"Watched world for 30 steps, firing rate: {brain.getAverageFiringRate()} Hz")
```

### Complete Agent with Learning
```python
import nlm
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()
world = nlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
print("Agent simulation complete!")
```

This enhanced API provides everything you need to create sophisticated neural simulations while remaining easy to use for beginners.