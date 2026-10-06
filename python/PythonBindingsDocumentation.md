# NLM Python Bindings Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings. These bindings provide Python access to the full NLM neural simulation framework, enabling Python users to create, simulate, and interact with artificial neural networks.

## Installation and Building

### Prerequisites
- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher
- pybind11 (automatically installed as dependency)

### Building with pip

1. Install the package in development mode:
```bash
pip install -e .
```

2. Alternatively, build from source:
```bash
mkdir build
cd build
cmake ..
make -j4
```

### Building Python Bindings Only

For building only the Python bindings:
```bash
mkdir build_python
cd build_python
cmake -DPYBIND11_PYTHON_VERSION=3.8 ..
make -j4
```

## Quick Start Guide

### Basic Brain Simulation

```python
import pynlm as nlm
import time

# Create a default configuration
config = nlm.createDefaultConfig()

# Set basic parameters
config.set("neuron_count", 100)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

# Create a brain with the configuration
brain = nlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation steps
for step in range(100):
    brain.step(step)
    # Get current stats
    print(f"Step {step}: Neurons={brain.getTotalNeuronCount()}, "
          f"Firing={brain.getFiringNeuronCount()}, "
          f"Rate={brain.getAverageFiringRate():.2f} Hz")

# Produce an action
action = brain.produceAction()
print(f"Action: {action.getName()}")

# Save brain state
brain.save("my_brain.nlm")

print("Simulation completed successfully!")
```

### Advanced Agent Simulation

```python
import pynlm as nlm

# Create all components
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

# Configure world
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)

# Initialize system
brain.initialize()
agent.initialize(world)

# Create agent body
agent_body = world.getAgentBody()
agent_body.x = 10.0
agent_body.y = 10.0
agent_body.orientation = 0.0

# Run simulation loop
for step in range(500):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step)
    
    # Get motor command
    cmd = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(cmd, step)
    
    # Get reward (every 10 steps)
    if step % 10 == 0:
        reward = world.computeReward(agent_body)
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
    
    # Log status every 50 steps
    if step % 50 == 0:
        brain.logStatus()

print("Agent simulation completed!")
```

## Python API Reference

### Core Classes

#### NeuronId, SynapseId, RegionId, PopulationId
Unique identifiers for NLM components.

```python
# Create IDs
neuron_id = NeuronId(42)
synapse_id = SynapseId(100)
region_id = RegionId(1)
population_id = PopulationId(5)

# Access ID values
print(neuron_id.value)  # 42
print(neuron_id.index())  # 42
```

#### Config
Configuration management for NLM systems.

```python
config = Config()
config.set("neuron_count", 100)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

# Get configuration values
neuron_count = config.getOr("neuron_count", 1000)
has_debug = config.has("debug_mode")

# Save/load configuration
config.saveToFile("config.json")
config.loadFromFile("config.json")

# Configuration summary
print(config.summary())
```

#### Vision, Audio, InternalSignals
Sensory input classes.

```python
# Vision input
vision = Vision(width=20, height=20, channels=3)
vision.setData(pixel_data)  # List of floats

# Audio input  
audio = Audio(sampleRate=44100, numSamples=1024)

# Internal signals
internal = InternalSignals()
internal.addSignal(0.5)
internal.addSignal(-0.3)
```

#### Action
Motor output representation.

```python
# Create action from type
action = Action(ActionType.MoveForward)

# Create action with parameters
action = Action(ActionType.Custom, [0.1, 0.5, 0.9])

# Action properties
print(f"Type: {action.getType()}")
print(f"Parameters: {action.getParameters()}")
print(f"Name: {action.getName()}")
```

#### WorldObject
World object representation.

```python
# Create world object
wall = WorldObject(x=5.0, y=5.0, type=WorldObjectType.Wall, radius=0.5)

# Access properties
print(f"Position: ({wall.x}, {wall.y})")
print(f"Type: {wall.type}")
print(f"Radius: {wall.radius}")
```

#### AgentBody
Agent state representation.

```python
agent = AgentBody()
agent.x = 10.0
agent.y = 10.0
agent.orientation = 0.0
agent.velocityX = 1.0
agent.velocityY = 0.5
agent.energy = 100.0
agent.health = 1.0
agent.age = 0
agent.isMoving = True
agent.lastActionTime = time.time()

agent.reset()  # Reset to initial state
```

#### ActionResult
Result of applying an action to the world.

```python
result = ActionResult(reward=10.0, success=True, message="Successfully moved")
print(f"Reward: {result.reward}")
print(f"Success: {result.success}")
print(f"Message: {result.message}")
```

#### SensoryPercept
Combined sensory input.

```python
percept = SensoryPercept()
vision_data = percept.getVision()
touch_data = percept.getTouch()
internal_data = percept.getInternal()
audio_data = percept.getAudio()
```

#### SimpleWorld
2D simulation world.

```python
world = SimpleWorld()
world.configure(width=30, height=30, visionWidth=15, visionHeight=15)

# World control
world.reset()
world.setAgentStart(15.0, 15.0)
world.setRandomSeed(42)

# Simulation control
world.update(timestep=0.001)
world.applyMotorCommand(motor_command, current_time)

# World properties
print(f"Width: {world.getWidth()}, Height: {world.getHeight()}")
print(f"Simulation time: {world.getSimulationTime()}")
```

#### Brain
Central neural simulation brain class.

```python
brain = Brain(config)
brain.initialize()

# Simulation control
brain.step(step_number)
brain.step(step_number, current_time)

# Input processing
brain.receiveSensoryInput(percept)
brain.injectCurrent(neuron_id, current_value)
brain.injectCurrentToNeurons(neuron_type, current_value)

# Action production
action = brain.produceAction()

# State management
brain.reset()
brain.save("brain_state.nlm")
brain.load("brain_state.nlm")

# Brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

#### AgentBrain
Agent brain interface connecting NLM to world.

```python
agent_brain = AgentBrain(brain)
agent_brain.initialize(world)

# Processing
decoded_cmd = agent_brain.decodeMotorCommand()
agent_brain.processSensoryInput(percept)
agent_brain.applyRewardModulation(reward, predicted_reward)
agent_brain.updateDevelopment(timestep)

# State and settings
agent_brain.reset()
development_stage = agent_brain.getDevelopmentalStage()
neuromod_level = agent_brain.getNeuromodulationLevel()
curiosity_level = agent_brain.getCuriosityLevel()
novelty_level = agent_brain.getNoveltyLevel()
prediction_error = agent_brain.getPredictionError()

# Feature control
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)
```

### Enumeration Types

#### NeuronType
```python
if neuron_type == NeuronType.Excitatory:
    print("Excites other neurons")
elif neuron_type == NeuronType.Inhibitory:
    print("Inhibits other neurons")
```

#### SynapseType
```python
if synapse_type == SynapseType.GapJunction:
    print("Electrical connection")
```

#### DevelopmentalStage
```python
development_stage = DevelopmentalStage.Maturation
if development_stage == DevelopmentalStage.CriticalPeriod:
    print("High plasticity period")
```

#### FiringState
```python
if firing_state == FiringState.Refractory:
    print("Neuron is refractory")
```

#### ActionType
```python
action_type = ActionType.MoveForward
if action_type in [ActionType.MoveForward, ActionType.MoveBackward]:
    print("Movement action")
```

#### MotorCommand
```python
motor_cmd = MotorCommand.TurnLeft
print(f"Motor command: {motor_cmd}")
```

#### WorldObjectType
```python
obj_type = WorldObjectType.Resource
if obj_type == WorldObjectType.Hazard:
    print("Avoid this!")
```

### Global Functions and Constants

```python
# Create default objects
default_config = createDefaultConfig()
default_brain = createBrain(default_config)
default_world = createSimpleWorld()
default_agent = createAgentBrain(default_brain)

# Invalid ID constants
invalid_neuron_id = INVALID_NEURON_ID
invalid_synapse_id = INVALID_SYNAPSE_ID
invalid_region_id = INVALID_REGION_ID
invalid_population_id = INVALID_POPULATION_ID
```

## Advanced Usage Examples

### 1. Brain Development Simulation

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 3)

brain = nlm.createBrain(config)
brain.initialize()

# Simulate brain development
for step in range(1000):
    brain.step(step)
    
    # Update development system
    if step % 100 == 0:
        development_stage = brain.getDevelopmentalStage()
        print(f"Development stage: {development_stage}")
        
        # Modify development parameters based on stage
        if development_stage == nlm.DevelopmentalStage.CriticalPeriod:
            # Increase plasticity during critical period
            pass
    
    # Check for developmental transitions
    if step % 500 == 0:
        brain.setDevelopmentalStage(nlm.DevelopmentalStage.Maturation)

print("Development simulation completed")
```

### 2. Memory System Integration

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Access memory systems (if implemented)
working_memory = brain.getWorkingMemory()
if working_memory:
    # Store sensory experience
    percept = nlm.SensoryPercept()
    working_memory.storeExperience(percept)
    
    # Retrieve recent experiences
    recent_experiences = working_memory.getRecentExperiences(time_window=100)

print("Memory system demonstration completed")
```

### 3. Neuromodulation Simulation

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

agent.initialize(world)

# Simulate neuromodulation over time
for step in range(200):
    # Create random reward
    reward = random.uniform(-1.0, 10.0)
    
    # Apply reward modulation
    predicted_reward = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Check neuromodulation levels
    dopamine = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    print(f"Step {step}: Dopamine={dopamine:.3f}, "
          f"Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")

print("Neuromodulation simulation completed")
```

### 4. Batch Processing

```python
import pynlm as nlm
import numpy as np

# Create multiple brains with different configurations
brains = []
for seed in range(10):
    config = nlm.createDefaultConfig()
    config.set("random_seed", seed)
    config.set("neuron_count", 200)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Run simulation for all brains
for i, brain in enumerate(brains):
    for step in range(100):
        brain.step(step)
    
    # Collect statistics
    firing_rates = brain.getAverageFiringRate()
    print(f"Brain {i}: Final firing rate = {firing_rates:.2f} Hz")

print("Batch processing completed")
```

## Error Handling

The Python bindings include comprehensive error handling with custom exceptions:

```python
import pynlm as nlm

try:
    # Create invalid neuron ID (should fail)
    neuron_id = nlm.NeuronId(-1)
except nlm.InvalidNeuronIdError as e:
    print(f"Invalid neuron ID: {e}")

try:
    # Try to access non-existent region
    region = brain.getRegion(invalid_region_id)
except nlm.InvalidRegionIdError as e:
    print(f"Invalid region ID: {e}")

try:
    # Try to perform operation outside valid range
    brain.step(-1)
except nlm.InvalidParameterError as e:
    print(f"Invalid parameter: {e}")
```

## Performance Tips

1. **Reuse Objects**: Create and reuse Brain, AgentBrain, and World objects when possible
2. **Batch Processing**: Process multiple steps before accessing object properties
3. **Memory Management**: Clean up references when finished to free memory
4. **Configuration**: Load configuration from file once, then modify in memory
5. **Logging**: Use the logging system for debugging, but disable in production

## Troubleshooting

### Common Issues

**Issue**: "ModuleNotFoundError: No module named 'pynlm'"
**Solution**: Install the module with `pip install pynlm` or `pip install -e .`

**Issue**: "ImportError: libpynlm.so: cannot open shared object file"
**Solution**: Ensure the module is in the Python path or reinstall the package

**Issue**: Simulation crashes with invalid neuron ID
**Solution**: Validate all neuron IDs before using them in brain operations

**Issue**: Poor performance
**Solution**: Check for memory leaks, disable logging in production, and optimize simulation parameters

### Getting Help

1. Check the NLM documentation website for comprehensive examples
2. Use the built-in logging system for debugging
3. Create minimal reproduction examples for reporting bugs
4. Consult the API reference for method signatures and parameters

## Migration Guide

### From C++ to Python

If you're migrating from C++ NLM usage, note the following key differences:

1. **Memory Management**: Python handles memory automatically
2. **Error Handling**: Python exceptions instead of return codes
3. **Type System**: More dynamic Python types vs static C++ types
4. **Documentation**: This Python API reference instead of C++ headers

### Key Equivalent Operations

```cpp
// C++
nlm::Brain brain(config);
brain.initialize();
brain.step(step);
brain.produceAction();

// Python
brain = createBrain(config)
brain.initialize()
brain.step(step)
action = brain.produceAction()
```

## License

MIT License

## Copyright

NLM Project 2026

## Acknowledgments

- pybind11 for excellent Python bindings support
- CMake for cross-platform build system
- Contributors to the NLM research project