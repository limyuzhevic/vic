# NLM Python API Documentation

This document provides comprehensive documentation for the NLM Python bindings (`pynlm`). It covers all available classes, functions, and advanced usage patterns.

## Overview

The NLM (Neural Learning Machine) Python module provides access to the C++ neural simulation framework through pybind11 bindings. It enables you to create, simulate, and experiment with spiking neural networks in Python.

### Quick Start

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
```

## Core Classes

### Brain

The central neural simulation brain class. This is the main interface for neural computation.

#### Creation and Initialization

```python
import pynlm

# Create with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create with custom configuration
brain = pynlm.createBrain(custom_config)
```

#### Simulation Control

```python
# Run one simulation step (with optional timestamp)
brain.step(step_number)
brain.step(step_number, current_time)

# Reset brain to initial state
brain.reset()

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Brain state control
brain.save("checkpoint.bin")  # Save brain state
brain.load("checkpoint.bin")  # Load brain state
```

#### Sensory and Motor Interface

```python
# Inject sensory input into the brain
sensory_input = pynlm.Vision(width=8, height=8, channels=3)
# ... configure sensory data ...
brain.receiveSensoryInput(sensory_input)

# Get motor command from brain activity
brain.produceAction()

# Inject current into specific neurons or types
brain.injectCurrent(neuron_id, current_value)
brain.injectCurrentToNeurons(neuron_type, current_value)
```

#### Memory and Learning Systems

```python
# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Access prediction systems
prediction_system = brain.getPredictionSystem()

# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
```

#### Development and Plasticity

```python
# Control development
brain.develop()

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)

# Get developmental stage
stage = brain.getDevelopmentalStage()

# Access plasticity rules
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural = brain.getStructuralPlasticity()
```

#### Region Management

```python
# Add new brain region
region_id = brain.addRegion("cortex")

# Get region by ID
region = brain.getRegion(region_id)

# Get all region IDs
region_ids = brain.getRegionIds()

# Get region statistics
neuron_count = brain.getTotalNeuronCount()
synapse_count = brain.getTotalSynapseCount()
```

### Config

Configuration class for NLM system parameters.

#### Creation and Basic Operations

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("learning.rate", 0.001)

# Check if configuration key exists
if config.has("brain.neuron_count"):
    print("Neuron count is configured")

# Get all configuration keys
keys = config.getKeys()

# Clear configuration
config.clear()
```

#### Configuration I/O

```python
# Load configuration from file
config.loadFromFile("config.json")

# Save configuration to file
config.saveToFile("config.json")

# Load from command line arguments (advanced usage)
# Note: This is typically used internally
```

### SimpleWorld

Simple 2D world for NLM simulation.

#### Creation and Configuration

```python
import pynlm

# Create simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Reset world to initial state
world.reset()
```

#### World Control

```python
# Run world update
world.update(timestep=0.1)

# Apply motor command
motor_cmd = pynlm.ActionType.MoveForward
world.applyMotorCommand(motor_cmd, world.getSimulationTime())

# Get sensory percept from world
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
```

#### World Properties

```python
# World dimensions
width = world.getWidth()
height = world.getHeight()

# Simulation time
simulation_time = world.getSimulationTime()

# Random seed control
world.setRandomSeed(42)
seed = world.getRandomSeed()

# World energy (if applicable)
max_energy = world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### AgentBrain

Agent brain interface connecting NLM brain to world.

#### Creation and Initialization

```python
import pynlm

# Create agent brain interface
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
agent.initialize(world)
```

#### Sensory Processing

```python
# Process sensory input and inject into brain
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### Motor Decoding

```python
# Decode brain motor activity into motor command
action = agent.decodeMotorCommand()

# Get detailed motor group activities
motor_activities = agent.getMotorGroupActivities()
for activity in motor_activities:
    print(f"Group: {activity.name}, Activity: {activity.activity}")
```

#### Neuromodulation

```python
# Apply reward-based neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Enable/disable neuromodulation subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### Development and Learning

```python
# Update development system
agent.updateDevelopment(timestep)

# Get developmental stage
stage = agent.getDevelopmentalStage()

# Get neuromodulation levels
curiosity_level = agent.getCuriosityLevel()
novelty_level = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
```

#### Statistics and Diagnostics

```python
# Get learned preference trait
preference = agent.getLearnedPreference()

# Get phase 5 diagnostic counters
print(f"Reflex bonus count: {agent.getReflexBonusCount()}")
print(f"Reflex suppress count: {agent.getReflexSuppressCount()}")
print(f"WTA pulse count: {agent.getWtaPulseCount()}")
```

#### Advanced Features

```python
# Phase 6: Text encoding/decoding
text_pattern = pynlm.AgentBrain.encodeText("hello world")

# Get internal firing rates for text association
internal_rates = agent.getInternalFiringRates()

# Get thought trace for debugging
thought_trace = agent.getThoughtTrace()
```

### Sensory Input Classes

#### Vision

```python
import pynlm

# Create vision sensory input
vision = pynlm.Vision(width=8, height=8, channels=3)
vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8])  # Example data

print(f"Vision width: {vision.getWidth()}")
print(f"Vision height: {vision.getHeight()}")
print(f"Vision channels: {vision.getChannels()}")
```

#### Audio

```python
import pynlm

# Create audio sensory input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)
audio.setData([0.0, 0.1, 0.2, 0.3])  # Example audio samples

print(f"Sample rate: {audio.getSampleRate()}")
print(f"Number of samples: {audio.getNumSamples()}")
```

#### InternalSignals

```python
import pynlm

# Create internal signals (used for neuromodulation)
internal = pynlm.InternalSignals()
internal.addSignal(0.5)
internal.addSignal(0.8)
internal.clearSignals()
```

### Action

Action representation for motor output.

#### Creation and Configuration

```python
import pynlm

# Create action with type only
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create action with type and parameters
action = pynlm.Action(
    pynlm.ActionType.MoveForward,
    [1.0, 2.0, 3.0]  # Movement parameters
)

# Action properties
print(f"Action type: {action.getType()}")
print(f"Action name: {action.getName()}")
print(f"Parameters: {action.getParameters()}")

# Modify action
action.setType(pynlm.ActionType.TurnLeft)
action.setParameters([0.5])

# Clone action
cloned_action = action.clone()
```

### WorldObject

World object representation.

```python
import pynlm

# Create world object
world_obj = pynlm.WorldObject(
    x=5.0,
    y=5.0,
    type=pynlm.WorldObjectType.Resource,
    value=1.0,
    radius=0.5
)

print(f"Position: ({world_obj.x}, {world_obj.y})")
print(f"Type: {world_obj.type}")
print(f"Value: {world_obj.value}")
print(f"Active: {world_obj.active}")
```

## Enumerations

### NeuronType

- `Excitatory` - Excitatory neuron
- `Inhibitory` - Inhibitory neuron  
- `Modulatory` - Modulatory neuron
- `Sensory` - Sensory neuron
- `Motor` - Motor neuron
- `Internal` - Internal neuron

### SynapseType

- `Excitatory` - Excitatory synapse
- `Inhibitory` - Inhibitory synapse
- `Modulatory` - Modulatory synapse
- `Electrical` - Electrical synapse
- `GapJunction` - Gap junction synapse

### DevelopmentalStage

- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period stage
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### ActionType

Action types for motor output:
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

### MotorCommand

Low-level motor commands:
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact
- `Wait` - Wait

### WorldObjectType

- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazard object
- `Wall` - Wall object
- `Marker` - Marker object

## Advanced Usage Patterns

### Complete Agent Simulation

```python
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
        reward = (world.getSensoryPercept().getInternal()[0] 
                 if world.getSensoryPercept().getInternal() 
                 else 0.0)
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

# Run the simulation
run_agent_simulation(1000)
```

### Learning Experiment

```python
import pynlm

def run_learning_experiment(brain, num_steps=500):
    """Run a learning experiment and track synaptic changes."""
    
    # Record initial state
    initial_weights = []
    for region in brain.getRegions():
        for synapse in region.getSynapses():
            initial_weights.append(synapse.getWeight())
    
    initial_mean_weight = sum(initial_weights) / len(initial_weights)
    print(f"Initial mean synaptic weight: {initial_mean_weight:.4f}")
    
    # Run simulation
    for step in range(num_steps):
        brain.step(step)
        
        # Optional: Inject patterned input for learning
        if step % 100 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
    
    # Record final state
    final_weights = []
    for region in brain.getRegions():
        for synapse in region.getSynapses():
            final_weights.append(synapse.getWeight())
    
    final_mean_weight = sum(final_weights) / len(final_weights)
    print(f"Final mean synaptic weight: {final_mean_weight:.4f}")
    print(f"Weight change: {final_mean_weight - initial_mean_weight:.4f}")
    
    # Check if learning occurred
    learning_detected = abs(final_mean_weight - initial_mean_weight) > 0.001
    print(f"Learning detected: {learning_detected}")
    
    return learning_detected

# Example usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

learning_result = run_learning_experiment(brain, 500)
```

### Pattern Generation and Analysis

```python
import pynlm

def analyze_firing_patterns(brain, num_steps=1000):
    """Analyze firing patterns over time."""
    
    spike_counts = []
    firing_rates = []
    
    for step in range(num_steps):
        brain.step(step)
        
        spike_counts.append(brain.getTotalSpikeCount())
        firing_rates.append(brain.getFiringNeuronCount())
    
    # Calculate statistics
    avg_spikes = sum(spike_counts) / len(spike_counts)
    avg_firing = sum(firing_rates) / len(firing_rates)
    max_spikes = max(spike_counts)
    
    print(f"Average spikes per step: {avg_spikes:.1f}")
    print(f"Average firing rate: {avg_firing:.1f} neurons")
    print(f"Maximum spikes in single step: {max_spikes}")
    
    # Simple pattern detection (basic example)
    increasing = sum(1 for i in range(1, len(spike_counts)) 
                    if spike_counts[i] > spike_counts[i-1]) / (len(spike_counts) - 1)
    
    print(f"Trend (increasing steps): {increasing:.2%}")
    
    return {
        'spike_counts': spike_counts,
        'firing_rates': firing_rates,
        'avg_spikes': avg_spikes,
        'avg_firing': avg_firing,
        'max_spikes': max_spikes,
        'trend': increasing
    }

# Example usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

results = analyze_firing_patterns(brain, 500)
```

## Error Handling

### Common Errors and Solutions

1. **Brain not initialized**
   ```python
   # Error: Brain should be initialized before calling step()
   brain.initialize()  # Always call this first!
   ```

2. **Invalid neuron ID**
   ```python
   # Error: Invalid neuron ID
   # Solution: Always validate neuron IDs from brain regions
   for region in brain.getRegions():
       for neuron in region.getAllNeurons():
           valid_neuron_ids.add(neuron.getId())
   ```

3. **Invalid world coordinates**
   ```python
   # Error: Invalid world position
   if world.isValidPosition(x, y):
       world.setAgentStart(x, y)
   else:
       print("Invalid position! Try different coordinates.")
   ```

### Best Practices

1. **Always initialize components before use**
   ```python
   brain.initialize()
   world.reset()
   agent.initialize(world)
   ```

2. **Handle configuration loading errors**
   ```python
   config = pynlm.createDefaultConfig()
   if not config.loadFromFile("config.json"):
       print("Using default configuration (file not found or invalid)")
   ```

3. **Validate sensory input before processing**
   ```python
   percept = world.getSensoryPercept()
   if percept.getVisionWidth() > 0:  # Check if data exists
       agent.processSensoryInput(percept)
   ```

## Performance Tips

1. **Reuse objects when possible**
   ```python
   # Reuse action objects instead of creating new ones
   action = pynlm.Action()
   for step in range(1000):
       world.applyMotorCommand(action, world.getSimulationTime())
   ```

2. **Batch operations**
   ```python
   # Configure world once
   world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
   
   # Then run many steps
   for step in range(10000):
       world.update(0.1)
   ```

3. **Monitor simulation state**
   ```python
   # Print progress periodically
   if step % 100 == 0:
       print(f"Step {step}, Firing: {brain.getFiringNeuronCount()}, "
             f"Spikes: {brain.getTotalSpikeCount()}")
   ```

## Migration Guide

### From v0.1.0 to v1.0.0

1. **Method name changes**
   - `brain.getStatus()` → `brain.logStatus()`
   - `brain.getWeight()` → `brain.getExcitationInhibitionRatio()`

2. **New configuration parameters**
   - Added `neuromod.dopamine.scale` for dopamine modulation
   - Added `neuromod.curiosity.enable` for curiosity control

3. **New classes added**
   - `pynlm.Novelty` class for novelty detection
   - Enhanced `pynlm.AgentBrain` with advanced cognitive features

4. **API improvements**
   - Better error messages and validation
   - More Pythonic method signatures
   - Enhanced documentation

## Further Reading

- [NLM Documentation](https://nlm.readthedocs.io) - Comprehensive project documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and results

## Example Scripts

Several example scripts are available in the repository:

1. **`examples/learning_experiment.py`** - Demonstrates synaptic plasticity
2. **`examples/agent_world.py`** - Complete agent simulation
3. **`examples/pattern_analysis.py`** - Firing pattern analysis
4. **`examples/configuration_examples.py`** - Configuration examples

## Acknowledgments

This documentation is based on the NLM (Neural Learning Machine) project and its contributors.

For technical support, please refer to the project's GitHub repository and issue tracker.
