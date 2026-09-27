# NLM Python API - Comprehensive Documentation

## Overview

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator written in C++20. The Python bindings (`pynlm`) provide a high-level interface to create virtual brains, simulate them in environments, and study cognitive processes.

This document provides comprehensive coverage of the Python API, from beginner examples to advanced usage patterns.

## Quick Start

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## API Reference

### Core Classes

#### Brain
Represents the neural simulation brain with all cognitive functions.

**Creation:**
```python
brain = pynlm.createBrain(config)
```

**Main Methods:**
- `initialize()` - Initialize the brain
- `step(step_num, time=None)` - Perform a simulation step
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Key Properties:**
- `getTotalNeuronCount()` - Total number of neurons
- `getTotalSynapseCount()` - Total number of synapses
- `getFiringNeuronCount()` - Currently firing neurons
- `getTotalSpikeCount()` - Total spikes generated
- `getAverageFiringRate()` - Average firing rate
- `getDevelopmentalStage()` - Current developmental stage

#### AgentBrain
Connects brain to environment, handling sensory input and motor output.

**Creation:**
```python
agent = pynlm.createAgentBrain(brain)
```

**Key Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get motor command
- `applyRewardModulation(reward, predicted)` - Apply learning

**Enable/Disable Features:**
- `enableRewardModulation(True/False)`
- `enableCuriosity(True/False)`
- `enableDevelopment(True/False)`
- `enableStructuralPlasticity(True/False)`

#### SimpleWorld
Represents a 2D simulation environment for the agent.

**Creation:**
```python
world = pynlm.createSimpleWorld()
```

**Configuration:**
```python
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
```

**Key Methods:**
- `update(dt)` - Update world state
- `getSensoryPercept()` - Get agent's view
- `getAgentBody()` - Get agent state
- `applyMotorCommand(action, time)` - Apply action

#### Configuration
Manages brain configuration and parameters.

**Creation:**
```python
config = pynlm.createDefaultConfig()
```

## Beginner Examples

### 1. Silent Brain
```python
import pynlm

# Create and run a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Silent brain created with {brain.getTotalNeuronCount()} neurons!")
```

### 2. Brain Watching a World
```python
import pynlm

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
    
print(f"Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 3. Complete Agent
```python
import pynlm

# Setup with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent with monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Advanced Examples

### 1. Brain with Memory Systems
```python
import pynlm

# Create brain with memory support
config = pynlm.createDefaultConfig()
config.set("enable_working_memory", True)
config.set("enable_episodic_memory", True)
config.set("enable_associative_memory", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Store and retrieve memories
brain.storeMemory("trial_1", "completed successfully")
brain.storeMemory("trial_2", "failed due to collision")

# Test memory recall
result = brain.recallMemory("trial_1")
print(f"Memory recall: {result}")

# Simulate with memory guidance
for step in range(50):
    # Get memory context
    context = brain.getCurrentContext()
    
    # Simulate decision with memory bias
    brain.step(step, memory_context=context)
```

### 2. Neuromodulation Training
```python
import pynlm

# Create agent with neuromodulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulators
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Training loop with reward and curiosity
for episode in range(10):
    world.reset()
    
    for step in range(200):
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action and get reward
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        # Simulate brain step
        brain.step(step)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Display progress
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"Step {step}: Curiosity={curiosity:.2f}, Novelty={novelty:.2f}")
```

### 3. Prediction and Planning
```python
import pynlm

# Create brain with prediction capabilities
config = pynlm.createDefaultConfig()
config.set("enable_prediction", True)
config.set("prediction_horizon", 10)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world with complex dynamics
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25)

# Create agent with planning
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable planning and prediction
agent.enablePrediction(True)
agent.enablePlanning(True)

# Plan and execute actions
for episode in range(5):
    world.reset()
    
    # Generate action plan
    plan = agent.generatePlan(world.getAgentBody(), target="explore")
    
    for step in range(100):
        # Get current plan action
        action = agent.getCurrentPlanAction()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update perception
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Simulate brain step with prediction
        prediction = brain.getCurrentPrediction()
        brain.step(step, prediction=prediction)
        
        # Update plan based on prediction error
        error = agent.getPredictionError()
        agent.updatePlan(error)
```

### 4. Developmental Brain
```python
import pynlm

# Create brain with development
config = pynlm.createDefaultConfig()
config.set("enable_development", True)
config.set("critical_period_length", 50)
config.set("maturation_rate", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()

# Simulate developmental progression
for stage in range(5):
    print(f"\n=== Developmental Stage {stage} ===")
    
    # Show current stage characteristics
    developmental_stage = brain.getDevelopmentalStage()
    plastic = brain.getPlasticityLevel()
    excitability = brain.getExcitabilityLevel()
    
    print(f"Stage: {developmental_stage}")
    print(f"Plasticity: {plastic:.2f}")
    print(f"Excitability: {excitability:.2f}")
    
    # Run simulation at this stage
    for step in range(30):
        brain.step(stage * 30 + step)
        
        # Show learning rate
        learning_rate = brain.getLearningRate()
        if step % 10 == 0:
            print(f"  Step {step}: Learning rate = {learning_rate:.2f}")
```

## Common Patterns

### Pattern 1: Run Simulation
```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Statistics
```python
def monitor_brain(brain, interval=10):
    stats = {
        'neurons_firing': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage().value,
        'excitability': brain.getExcitabilityLevel(),
        'plasticity': brain.getPlasticityLevel()
    }
    return stats

# In your simulation loop
for step in range(100):
    brain.step(step)
    
    if step % interval == 0:
        stats = monitor_brain(brain)
        print(f"Step {step}: {stats['neurons_firing']} neurons firing")
```

### Pattern 3: Enable Brain Features
```python
# Enable advanced features for complex simulations
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enablePrediction(True)            # Predict future states
agent.enablePlanning(True)              # Plan action sequences
agent.enableAttention(True)             # Focus on important inputs
```

## Configuration Presets

### Basic Configuration
```python
def create_basic_config():
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    return config
```

### Learning Configuration
```python
def create_learning_config():
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("region_count", 3)
    config.set("connection_probability", 0.15)
    config.set("enable_working_memory", True)
    config.set("enable_episodic_memory", True)
    config.set("enable_reward_modulation", True)
    config.set("enable_curiosity", True)
    return config
```

### Developmental Configuration
```python
def create_development_config():
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300)
    config.set("region_count", 5)
    config.set("connection_probability", 0.2)
    config.set("enable_development", True)
    config.set("critical_period_length", 100)
    config.set("maturation_rate", 0.05)
    config.set("enable_structural_plasticity", True)
    return config
```

## Error Handling

### Common Errors and Solutions

**Error: Brain not initialized**
```python
try:
    brain.step(0)
except RuntimeError as e:
    brain.initialize()
    brain.step(0)  # Now it works
```

**Error: Invalid action**
```python
action = agent.decodeMotorCommand()
if action is None:
    # Handle case where no action is available
    action = create_default_action()
world.applyMotorCommand(action, world.getSimulationTime())
```

**Error: World configuration issue**
```python
try:
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
except ValueError as e:
    print(f"Configuration error: {e}")
    world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
```

### Exception Types

The pynlm module defines the following exceptions:

- `pynlm.RuntimeError` - General runtime errors
- `pynlm.ConfigurationError` - Configuration-related errors
- `pynlm.SimulationError` - Simulation-related errors
- `pynlm.ValidationError` - Input validation errors

## Performance Monitoring

### Get Simulation Statistics
```python
def get_detailed_stats(brain, world, agent):
    return {
        'simulation': {
            'current_step': brain.getCurrentStep(),
            'current_time': brain.getCurrentTime(),
            'world_time': world.getSimulationTime()
        },
        'neurons': {
            'total': brain.getTotalNeuronCount(),
            'active': brain.getActiveNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio()
        },
        'memory': {
            'working_memory_usage': brain.getWorkingMemoryUsage(),
            'episodic_memory_count': brain.getEpisodicMemoryCount()
        },
        'neuromodulation': {
            'dopamine_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel()
        },
        'development': {
            'stage': brain.getDevelopmentalStage().value,
            'age': brain.getDevelopmentalAge(),
            'plasticity': brain.getPlasticityLevel()
        }
    }

# Use in monitoring
stats = get_detailed_stats(brain, world, agent)
print(f"Average firing rate: {stats['neurons']['avg_firing_rate']:.2f} Hz")
```

## Advanced Features

### 1. Custom Actions
```python
class CustomAction:
    def __init__(self, action_type, parameters=None):
        self.type = action_type
        self.parameters = parameters or {}
    
    def get_name(self):
        return f"Custom{self.type}"

# Register custom action
pynlm.registerActionType("EXPLORE", CustomAction("explore", {"radius": 5.0}))

# Use custom action
world.applyCustomAction("EXPLORE", world.getAgentBody())
```

### 2. Event Callbacks
```python
def on_step_completed(brain, step_num):
    if step_num % 100 == 0:
        print(f"Step {step_num}: {brain.getFiringNeuronCount()} neurons firing")

def on_action_taken(action, result):
    if result.success:
        print(f"Successfully performed action: {action}")

# Register callbacks
brain.onStepComplete = on_step_completed
world.onActionComplete = on_action_taken
```

### 3. Parallel Simulation
```python
import threading
from multiprocessing import Pool

def simulate_step(step_num):
    return brain.step(step_num)

# Parallel step execution
with Pool(4) as pool:
    results = pool.map(simulate_step, range(100))
```

## Migration Guide

### From Easy Usage to Full API

If you're currently using the Easy Usage guide, here's how to transition to the full API:

**Easy Usage:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

**Full API with features:**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced features
brain.enableWorkingMemory(True)
brain.enableCuriosity(True)

# More detailed monitoring
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

### Tips for Scaling Up

1. **Start Simple**: Use basic examples first, then add complexity
2. **Monitor Performance**: Use the statistics functions to track simulation
3. **Enable Features Incrementally**: Don't enable all features at once
4. **Save Progress**: Use `save()` and `load()` to preserve brain states
5. **Use Configuration**: Create custom configs for different scenarios

## Next Steps

When you're comfortable with the basic API:

1. **Read** `HOW_TO_USE.md` for detailed technical information
2. **Explore** `docs/ARCHITECTURE.md` to understand system design
3. **Experiment** with different configurations
4. **Create** your own brain architectures and behaviors
5. **Contribute** to the project by adding new features or fixing issues

## License

MIT License - See LICENSE file for details.

## Authors

Research project — See docs for scientific background.