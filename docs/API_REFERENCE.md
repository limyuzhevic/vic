# NLM Python API Reference

## Overview

The NLM Python API provides a comprehensive interface to the NLM (Neural Learning Machine) neural simulation framework. This documentation covers all classes, methods, and advanced features available in the Python bindings.

## Python Exception Classes

NLM provides custom exception classes for better error handling:

```python
# Base exception for all NLM errors
pynlm.RuntimeError("NLM runtime error")

# For invalid arguments or parameters
pynlm.InvalidArgumentError("Invalid argument provided")

# For out-of-bounds access or invalid ranges
pynlm.OutOfRangeError("Value out of valid range")

# For logical errors in the API usage
pynlm.LogicError("Logical error in NLM operation")

# For configuration errors
pynlm.ConfigurationError("Invalid configuration")

# For simulation errors
pynlm.SimulationError("Simulation encountered an error")

# For memory-related errors
pynlm.MemoryError("Memory allocation failed")
```

## Brain Class

### Brain Creation and Initialization

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Brain Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### Brain Input/Output

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Simulation loop with full integration
for step in range(500):
    # Update world
    world.update(0.1)
    
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
```

### Brain Statistics and Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Basic statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Total spike count: {brain.getTotalSpikeCount()}")
print(f"Pending spike events: {brain.getPendingSpikeEventCount()}")

# Developmental information
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Neuromodulation status (if available)
# print(f"Dopamine level: {brain.getDopamine():.3f}")

# Region information
region_ids = brain.getRegionIds()
for region_id in region_ids:
    region = brain.getRegion(region_id)
    print(f"Region {region_id}: {region.getTotalNeuronCount()} neurons, {region.getSynapseCount()} synapses")
```

### Brain State Management

```python
import pynlm
import tempfile
import os

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation steps
for i in range(100):
    brain.step(i)

# Save brain state to file
with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
    checkpoint_file = f.name

if brain.save(checkpoint_file):
    print(f"Brain state saved to {checkpoint_file}")

# Load brain state from file
if brain.load(checkpoint_file):
    print("Brain state loaded successfully")

# Clean up
os.unlink(checkpoint_file)
```

### Brain Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Query configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Get configuration values
if config.has("brain.neuron_count"):
    neuron_count = config.getOr("brain.neuron_count", 1000)
    print(f"Neuron count: {neuron_count}")

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Configuration validation
assert config.has("brain.neuron_count")
assert config.has("brain.synapse_density")
```

## AgentBrain Class

### AgentBrain Creation and Initialization

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent with world
agent.initialize(world)
```

### AgentBrain Subsystem Control

```python
import pynlm

agent = pynlm.createAgentBrain(brain)

# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
print(f"Development enabled: {agent.isDevelopmentEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
```

### AgentBrain Sensory Processing

```python
import pynlm

agent = pynlm.createAgentBrain(brain)

# Create sensory percept
percept = pynlm.SensoryPercept()

# Set vision data (256 values for 16x16 vision)
vision_data = [0.1 * i for i in range(256)]  # Example: 0.0, 0.1, 0.2, ..., 25.5
percept.setVision(vision_data)

# Set other sensory modalities (if available)
# touch_data = [0.0] * 8
# percept.setTouch(touch_data)

# Process sensory input
agent.processSensoryInput(percept)
```

### AgentBrain Motor Decoding

```python
import pynlm

agent = pynlm.createAgentBrain(brain)

# Decode motor command
motor_cmd = agent.decodeMotorCommand()

print(f"Decoded motor command: {motor_cmd}")

# Motor command examples:
# pynlm.MotorCommand.MoveForward
# pynlm.MotorCommand.MoveBackward
# pynlm.MotorCommand.TurnLeft
# pynlm.MotorCommand.TurnRight
# pynlm.MotorCommand.Interact
# pynlm.MotorCommand.Eat
# pynlm.MotorCommand.Drink
# pynlm.MotorCommand.Rest
# pynlm.MotorCommand.Wait
```

### AgentBrain Neuromodulation

```python
import pynlm

agent = pynlm.createAgentBrain(brain)

# Apply reward modulation
agent.applyRewardModulation(reward=1.0, predicted_reward=0.5)

# Get neuromodulation levels
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
neuromodulation = agent.getNeuromodulationLevel()

print(f"Curiosity level: {curiosity:.3f}")
print(f"Novelty level: {novelty:.3f}")
print(f"Prediction error: {prediction_error:.3f}")
print(f"Neuromodulation level: {neuromodulation:.3f}")
```

### AgentBrain Development

```python
import pynlm

agent = pynlm.createAgentBrain(brain)

# Update development
time_step = 0.1
agent.updateDevelopment(time_step)

# Get developmental stage
stage = agent.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Developmental stages:
# pynlm.DevelopmentalStage.Initial
# pynlm.DevelopmentalStage.CriticalPeriod
# pynlm.DevelopmentalStage.Maturation
# pynlm.DevelopmentalStage.Adult
# pynlm.DevelopmentalStage.Aging
```

### AgentBrain Reset

```python
import pynlm

agent = pynlm.createAgentBrain(brain)

# Reset agent for new episode
agent.reset()
print("Agent reset completed")

# Reset clears internal state:
# - Neuromodulation signals
# - Novelty tracking
# - Curiosity levels
# - Prediction error
# - Developmental age
# - Previous sensory state
```

## SimpleWorld Class

### SimpleWorld Creation and Configuration

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
```

### World Control

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)

# Set random seed for reproducible simulations
world.setRandomSeed(42)
```

### World State Access

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Get sensory percept from world
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent orientation: {body.orientation}")
print(f"Agent velocity: ({body.velocityX}, {body.velocityY})")
print(f"Agent energy: {body.energy}")
print(f"Agent health: {body.health}")
```

### World Object Management

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Add world objects (resources, hazards, etc.)
resource = pynlm.WorldObject(x=5.0, y=5.0, type=pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(resource)

hazard = pynlm.WorldObject(x=15.0, y=15.0, type=pynlm.WorldObjectType.Hazard, value=0.5)
world.addObject(hazard)

wall = pynlm.WorldObject(x=10.0, y=20.0, type=pynlm.WorldObjectType.Wall, radius=0.5)
world.addObject(wall)

# Remove objects
world.removeObject(x=5.0, y=5.0)

# Check if position is valid
is_valid = world.isValidPosition(10.0, 10.0)
```

### World Properties

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# World dimensions
print(f"Width: {world.getWidth()}")
print(f"Height: {world.getHeight()}")
print(f"Vision width: {world.getVisionWidth()}")
print(f"Vision height: {world.getVisionHeight()}")

# Simulation time
print(f"Simulation time: {world.getSimulationTime()}")

# Maximum energy
print(f"Max energy: {world.getMaxEnergy()}")
world.setMaxEnergy(100.0)
```

## Config Class

### Config Creation and Modification

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.05)
config.set("brain.initial_weight_mean", 0.5)
config.set("brain.initial_weight_std", 0.1)

# Get configuration values
if config.has("brain.neuron_count"):
    neuron_count = config.getOr("brain.neuron_count", 1000)
    print(f"Neuron count: {neuron_count}")

# Configuration validation
assert config.has("brain.neuron_count")
assert config.has("brain.synapse_density")

# Get all keys
keys = config.getKeys()
print(f"All configuration keys: {keys}")
```

### Config File I/O

```python
import pynlm
import tempfile
import os

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Save to JSON file
with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
    json_file = f.name

config.saveToFile(json_file)
print(f"Configuration saved to {json_file}")

# Load from JSON file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile(json_file)
print("Configuration loaded from JSON")

# Clean up
os.unlink(json_file)
```

## Enumerations

### Neuron Types

```python
import pynlm

# Neuron types
print(f"Excitatory: {pynlm.NeuronType.Excitatory}")
print(f"Inhibitory: {pynlm.NeuronType.Inhibitory}")
print(f"Modulatory: {pynlm.NeuronType.Modulatory}")
print(f"Sensory: {pynlm.NeuronType.Sensory}")
print(f"Motor: {pynlm.NeuronType.Motor}")
print(f"Internal: {pynlm.NeuronType.Internal}")
```

### Developmental Stages

```python
import pynlm

# Developmental stages
print(f"Initial: {pynlm.DevelopmentalStage.Initial}")
print(f"CriticalPeriod: {pynlm.DevelopmentalStage.CriticalPeriod}")
print(f"Maturation: {pynlm.DevelopmentalStage.Maturation}")
print(f"Adult: {pynlm.DevelopmentalStage.Adult}")
print(f"Aging: {pynlm.DevelopmentalStage.Aging}")
```

### Action Types

```python
import pynlm

# Action types
print(f"MoveForward: {pynlm.ActionType.MoveForward}")
print(f"MoveBackward: {pynlm.ActionType.MoveBackward}")
print(f"MoveLeft: {pynlm.ActionType.MoveLeft}")
print(f"MoveRight: {pynlm.ActionType.MoveRight}")
print(f"TurnLeft: {pynlm.ActionType.TurnLeft}")
print(f"TurnRight: {pynlm.ActionType.TurnRight}")
print(f"Look: {pynlm.ActionType.Look}")
print(f"LookUp: {pynlm.ActionType.LookUp}")
print(f"LookDown: {pynlm.ActionType.LookDown}")
print(f"Interact: {pynlm.ActionType.Interact}")
print(f"Eat: {pynlm.ActionType.Eat}")
print(f"Drink: {pynlm.ActionType.Drink}
print(f"Rest: {pynlm.ActionType.Rest}")
print(f"Wait: {pynlm.ActionType.Wait}")
print(f"Custom: {pynlm.ActionType.Custom}")
```

### World Object Types

```python
import pynlm

# World object types
print(f"Empty: {pynlm.WorldObjectType.Empty}")
print(f"Resource: {pynlm.WorldObjectType.Resource}")
print(f"Hazard: {pynlm.WorldObjectType.Hazard}")
print(f"Wall: {pynlm.WorldObjectType.Wall}")
print(f"Marker: {pynlm.WorldObjectType.Marker}")
```

### Motor Command

```python
import pynlm

# Motor commands (low-level)
print(f"MoveForward: {pynlm.MotorCommand.MoveForward}")
print(f"MoveBackward: {pynlm.MotorCommand.MoveBackward}")
print(f"TurnLeft: {pynlm.MotorCommand.TurnLeft}")
print(f"TurnRight: {pynlm.MotorCommand.TurnRight}")
print(f"LookLeft: {pynlm.MotorCommand.LookLeft}")
print(f"LookRight: {pynlm.MotorCommand.LookRight}")
print(f"Interact: {pynlm.MotorCommand.Interact}")
print(f"Wait: {pynlm.MotorCommand.Wait}")
```

## Error Handling Examples

### Try-Catch Examples

```python
import pynlm

try:
    # Create brain and initialize
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain created and initialized successfully")
    
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument error: {e}")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
    
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Parameter Validation Examples

```python
import pynlm

# Validate configuration parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Check if parameters are within valid ranges
neuron_count = config.getOr("brain.neuron_count", 1000)
if neuron_count < 100 or neuron_count > 10000:
    print(f"Warning: Unusual neuron count: {neuron_count}")

synapse_density = config.getOr("brain.synapse_density", 0.1)
if synapse_density < 0.0 or synapse_density > 1.0:
    print(f"Warning: Invalid synapse density: {synapse_density}")
```

## Performance Tips

### Configuration Optimization

```python
# Optimal configuration for different use cases

# For research simulations (high accuracy)
research_config = pynlm.createDefaultConfig()
research_config.set("brain.neuron_count", 5000)
research_config.set("brain.synapse_density", 0.2)
research_config.set("plasticity.stdp.learning_rate", 0.001)

# For real-time applications (low latency)
realtime_config = pynlm.createDefaultConfig()
realtime_config.set("brain.neuron_count", 500)
realtime_config.set("brain.synapse_density", 0.05)
realtime_config.set("simulation.timestep", 0.0001)

# For educational demonstrations (small scale)
ed_config = pynlm.createDefaultConfig()
ed_config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.01)
ed_config.set("plasticity.stdp.learning_rate", 0.01)
```

### Simulation Optimization

```python
import pynlm

# Optimize simulation performance
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Set appropriate simulation timestep
simulation_timestep = 0.001  # 1 ms

# Run simulation
for step in range(1000):
    brain.step(step)
```

## Migration Guide

### From Version 0.0.x to 0.1.0

The following changes affect the API between versions:

1. **Enhanced Error Handling**: Added comprehensive exception classes
2. **Improved Documentation**: Added extensive docstrings with examples
3. **New Features**: Added performance monitoring and advanced configuration options
4. **Bug Fixes**: Fixed various issues in the bindings

### Migration Examples

```python
# OLD CODE (version 0.0.x)
brain = pynlm.createBrain(config)
brain.initialize()

# NEW CODE (version 0.1.0) with enhanced error handling
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    print("Brain created successfully")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Handle configuration error
    
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
    # Handle simulation error
```

## Advanced Usage

### Event-Driven Simulation

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Event-driven simulation with callbacks
class SimulationRunner:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.step_count = 0
    
    def run_step(self):
        # Update world
        self.world.update(0.1)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Run brain step
        self.brain.step(self.step_count)
        
        # Decode motor command
        motor_cmd = self.agent.decodeMotorCommand()
        
        # Apply motor command
        self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
        
        # Apply neuromodulation
        reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
        self.agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        self.agent.updateDevelopment(0.1)
        
        self.step_count += 1
        
        return self.step_count

# Run simulation
runner = SimulationRunner(brain, agent, world)
for _ in range(100):
    runner.run_step()
```

### Advanced Configuration

```python
import pynlm

# Advanced configuration with nested parameters
config = pynlm.createDefaultConfig()

# Brain configuration
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)
config.set("brain.initial_weight_mean", 0.5)
config.set("brain.initial_weight_std", 0.1)
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)
config.set("brain.v_reset", -75.0)
config.set("brain.tau_mem", 20.0)
config.set("brain.tau_ref", 2.0)

# Plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

print("Advanced configuration set successfully")
```

### Memory Management

```python
import pynlm
import gc

# Enable garbage collection
gc.enable()

# Create brain and run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation (may allocate memory)
for i in range(1000):
    brain.step(i)

# Force garbage collection
gc.collect()
print("Garbage collection completed")

# Monitor memory usage (if supported)
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

## License and Attribution

This documentation is part of the NLM (熙然) neural simulation framework.

```
MIT License

Copyright (c) 2024 NLM Authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
```

---

For more information, visit: https://github.com/nlm-project/nlm

This documentation was last updated on 2024-10-11.
