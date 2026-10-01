# pynlm - Python Module API Documentation

## Overview

**pynlm** is the Python module for NLM (Neural Learning Machine), an experimental artificial developmental brain system. This module provides Python bindings to the C++ neural simulation framework.

## Module Information

- **Version**: 0.1.0
- **Author**: NLM Authors
- **License**: MIT
- **Dependencies**: Python 3.8+, NumPy (optional)

## Main Classes

### Brain
The central neural simulation brain class that coordinates all neural activity.

#### Creation and Initialization
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize brain (always call first!)
brain.initialize()
```

#### Core Methods

**Simulation Control:**
- `brain.step(step_number)` - Run one simulation step
- `brain.step(step_number, time)` - Run step with timestamp
- `brain.reset()` - Reset brain state

**Input/Output:**
- `brain.receiveSensoryInput(input)` - Inject sensory data into brain
- `brain.produceAction()` - Get motor action based on neural activity
- `brain.save(filepath)` - Save brain state to file
- `brain.load(filepath)` - Load brain state from file

**Statistics:**
- `brain.getTotalNeuronCount()` - Total neurons in brain
- `brain.getTotalSynapseCount()` - Total synapses in brain
- `brain.getFiringNeuronCount()` - Currently firing neurons
- `brain.getActiveNeuronCount()` - Active neurons
- `brain.getAverageFiringRate()` - Average firing rate across all neurons
- `brain.getExcitationInhibitionRatio()` - E/I balance ratio
- `brain.getTotalSpikeCount()` - Total spike count
- `brain.getTotalSpikeCount()` - Total spike count

**State Information:**
- `brain.getDevelopmentalStage()` - Current developmental stage
- `brain.setDevelopmentalStage(stage)` - Set developmental stage
- `brain.getConfig()` - Get configuration object

**Region Management:**
- `brain.addRegion(name)` - Add new neural region
- `brain.getRegion(region_id)` - Get region by ID
- `brain.getRegionIds()` - Get all region IDs
- `brain.getRegions()` - Get all regions

**Logging:**
- `brain.logStatus()` - Log brain status and statistics

### Config
Configuration management class for NLM system.

#### Basic Usage
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Query configuration
if config.has("brain.neuron_count"):
    print(config.get("brain.neuron_count"))

# Get all keys
keys = config.getKeys()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File operations
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Command line arguments
config.loadFromArgs(3, ["nlm", "--neuron_count", "500"])
```

#### Methods

**Query Methods:**
- `config.has(key)` - Check if key exists
- `config.get(key)` - Get value with type conversion
- `config.getOr(key, default)` - Get value with default
- `config.getKeys()` - Get all configuration keys
- `config.summary()` - Get configuration summary string

**Modification Methods:**
- `config.set(key, value, source)` - Set configuration value

**File Operations:**
- `config.loadFromFile(filepath)` - Load from JSON file
- `config.saveToFile(filepath)` - Save to JSON file
- `config.loadFromArgs(argc, argv)` - Load from command line arguments

### AgentBrain
Bridge connecting NLM brain to world environment.

#### Basic Usage
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Enable features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### Core Methods

**Sensory Processing:**
- `agent.processSensoryInput(percept)` - Process sensory percept and inject into brain
- `agent.getSensoryInputSize()` - Get expected sensory input size

**Motor Decoding:**
- `agent.decodeMotorCommand()` - Decode brain activity into motor command

**Neuromodulation:**
- `agent.applyRewardModulation(reward, predicted_reward)` - Apply reward-based neuromodulation

**Development:**
- `agent.updateDevelopment(timestep)` - Update development system
- `agent.getDevelopmentalStage()` - Get current developmental stage

**Statistics:**
- `agent.getCuriosityLevel()` - Get curiosity level (0.0-1.0)
- `agent.getNoveltyLevel()` - Get novelty level (0.0-1.0)
- `agent.getPredictionError()` - Get prediction error
- `agent.getNeuromodulationLevel()` - Get neuromodulation level

**Configuration:**
- `agent.enableRewardModulation(enable)` - Enable/disable reward modulation
- `agent.enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `agent.enableDevelopment(enable)` - Enable/disable development
- `agent.enableCuriosity(enable)` - Enable/disable curiosity

### SimpleWorld
Simple 2D world for NLM simulation.

#### Basic Usage
```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Update world
world.update(timestep=0.1)
```

#### Core Methods

**World Control:**
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `world.reset()` - Reset to initial state
- `world.update(timestep)` - Update world state
- `world.setAgentStart(x, y)` - Set agent starting position
- `world.setRandomSeed(seed)` - Set random seed for reproducibility

**Agent Interaction:**
- `world.applyMotorCommand(cmd, current_time)` - Apply motor command to agent
- `world.getSensoryPercept()` - Get current sensory percept
- `world.getAgentBody()` - Get agent body state

**World Objects:**
- `world.addObject(obj)` - Add world object
- `world.removeObject(x, y)` - Remove object at position
- `world.getObjectAt(x, y)` - Get object at position
- `world.isValidPosition(x, y)` - Check if position is valid

**Properties:**
- `world.getWidth()` - Get world width
- `world.getHeight()` - Get world height
- `world.getMaxEnergy()` - Get max energy
- `world.setMaxEnergy(e)` - Set max energy
- `world.getEnergyDecayRate()` - Get energy decay rate
- `world.getSimulationTime()` - Get simulation time

### WorldObject
World object representation.

#### Creation
```python
import pynlm

# Create empty world object
obj = pynlm.WorldObject()

# Create with specific properties
obj = pynlm.WorldObject(
    x=5.0, y=5.0,
    type=pynlm.WorldObjectType.Resource,
    value=10.0,
    radius=0.5
)
```

#### Properties
- `obj.x, obj.y` - Position coordinates
- `obj.radius` - Radius
- `obj.type` - Object type (Resource, Hazard, Wall, etc.)
- `obj.value` - Value (reward or damage)
- `obj.active` - Whether object is active

### Action
Action representation for motor output.

#### Creation
```python
import pynlm

# Create empty action
action = pynlm.Action()

# Create with type
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create with type and parameters
action = pynlm.Action(
    pynlm.ActionType.Custom,
    parameters=[0.5, 0.8]
)
```

#### Properties
- `action.type` - Action type
- `action.parameters` - Action parameters

### SensoryPercept
Sensory percept data container.

#### Access Methods
- `percept.getVision()` - Get vision data
- `percept.getVisionWidth()` - Get vision width
- `percept.getVisionHeight()` - Get vision height
- `percept.getTouch()` - Get touch data
- `percept.getInternal()` - Get internal signals
- `percept.getProprioception()` - Get proprioception data
- `percept.getAudio()` - Get audio data
- `percept.getAllSignals()` - Get all sensory signals

### AgentBody
Agent body state.

#### Properties
- `body.x, body.y` - Position
- `body.orientation` - Orientation in radians
- `body.velocityX, body.velocityY` - Linear velocity
- `body.angularVelocity` - Angular velocity
- `body.energy` - Energy level
- `body.health` - Health
- `body.age` - Age in simulation time
- `body.isMoving` - Whether moving
- `body.isTurning` - Whether turning

### ActionResult
Result from applying motor command to world.

#### Properties
- `result.reward` - Reward value
- `result.success` - Whether action was successful
- `result.message` - Result message

## Enumerations

### NeuronType
- `NeuronType.Excitatory`
- `NeuronType.Inhibitory`
- `NeuronType.Modulatory`
- `NeuronType.Sensory`
- `NeuronType.Motor`
- `NeuronType.Internal`

### SynapseType
- `SynapseType.Excitatory`
- `SynapseType.Inhibitory`
- `SynapseType.Modulatory`
- `SynapseType.Electrical`
- `SynapseType.GapJunction`

### DevelopmentalStage
- `DevelopmentalStage.Initial`
- `DevelopmentalStage.CriticalPeriod`
- `DevelopmentalStage.Maturation`
- `DevelopmentalStage.Adult`
- `DevelopmentalStage.Aging`

### FiringState
- `FiringState.Resting`
- `FiringState.Active`
- `FiringState.Refractory`
- `FiringState.Inhibited`

### ActionType
- `ActionType.MoveForward`
- `ActionType.MoveBackward`
- `ActionType.MoveLeft`
- `ActionType.MoveRight`
- `ActionType.TurnLeft`
- `ActionType.TurnRight`
- `ActionType.Look`
- `ActionType.LookUp`
- `ActionType.LookDown`
- `ActionType.Interact`
- `ActionType.Eat`
- `ActionType.Drink`
- `ActionType.Rest`
- `ActionType.Wait`
- `ActionType.Custom`

### MotorCommand
- `MotorCommand.MoveForward`
- `MotorCommand.MoveBackward`
- `MotorCommand.TurnLeft`
- `MotorCommand.TurnRight`
- `MotorCommand.LookLeft`
- `MotorCommand.LookRight`
- `MotorCommand.Interact`
- `MotorCommand.Wait`

### WorldObjectType
- `WorldObjectType.Empty`
- `WorldObjectType.Resource`
- `WorldObjectType.Hazard`
- `WorldObjectType.Wall`
- `WorldObjectType.Marker`

### ID Types
- `NeuronId` - Unique identifier for neurons
- `SynapseId` - Unique identifier for synapses
- `RegionId` - Unique identifier for brain regions
- `PopulationId` - Unique identifier for neuron populations

## Module Attributes

### pynlm.__version__
Python module version string.

### pynlm.INVALID_NEURON_ID
Invalid neuron ID sentinel value.

### pynlm.INVALID_SYNAPSE_ID
Invalid synapse ID sentinel value.

### pynlm.INVALID_REGION_ID
Invalid region ID sentinel value.

### pynlm.INVALID_POPULATION_ID
Invalid population ID sentinel value.

## Utility Functions

### pynlm.createDefaultConfig()
Create a default configuration object.

### pynlm.createBrain(config)
Create a new brain with configuration.

### pynlm.createSimpleWorld()
Create a new simple world.

### pynlm.createAgentBrain(brain)
Create a new agent brain interface.

## Example Complete Application

```python
import pynlm
import time

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
    
    print("Simulation complete!")

# Run the simulation
run_agent_simulation(1000)
```

## Advanced Usage

### Configuration Management
```python
import pynlm
import json

# Save configuration to file
config = pynlm.createDefaultConfig()
config.saveToFile("my_config.json")

# Load configuration from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Modify configuration
new_config.set("brain.neuron_count", 2000)
new_config.set("plasticity.stdp.enable", False)
new_config.saveToFile("modified_config.json")
```

### Brain Checkpointing
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Save brain state (checkpoint)
brain.save("checkpoint1.bin")

# Run simulation for a while
for step in range(100):
    brain.step(step)

# Load brain state (resume)
brain.load("checkpoint1.bin")

# Continue from checkpoint
for step in range(100, 200):
    brain.step(step)
```

### World Exploration
```python
import pynlm

# Create agent and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Explore world with curiosity
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action and log curiosity
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}")
```

## Migration Guide

### From Easy Usage to Full API

**Easy Usage Example:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

**Full API Equivalent:**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
```

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Ensure `brain.initialize()` is called before `brain.step()`
- Check that configuration is properly set
- Verify that brain has been initialized with neurons

**"The agent isn't moving"**
- Ensure `world.applyMotorCommand()` is called
- Check that `world.update()` is being called
- Verify that agent has initialized the world

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - ensure sensory input is connected
- Check that agent is processing sensory input

**"Import error: No module named 'pynlm'"**
- Install with: `pip install --force-reinstall .`
- Ensure build dependencies are installed: `pip install pybind11 scikit-build-core numpy`

**"Segmentation faults"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify that all required components are properly initialized

## Performance Tips

### For Large Networks
- Use larger configuration values for more realistic brain sizes
- Consider using the `nlm_core` static library directly for custom integrations
- Monitor memory usage and consider limiting the number of neurons

### For Real-time Applications
- Use smaller timestep values (e.g., 0.01) for smoother updates
- Consider using `world.update(0.01)` for more granular control
- Monitor simulation time with `world.getSimulationTime()`

### For Debugging
- Use `brain.logStatus()` to print detailed brain information
- Monitor developmental stage with `brain.getDevelopmentalStage()`
- Track learning with `agent.getCuriosityLevel()` and `agent.getPredictionError()`

## Future Enhancements

The pynlm Python module is actively developed. Future enhancements may include:

- **Advanced sensory modalities**: Color vision, more complex internal signals
- **Enhanced learning mechanisms**: More sophisticated plasticity rules
- **Advanced world features**: Physics simulation, multiple agents
- **Visualization tools**: Python-based brain visualization
- **Extended configuration**: More fine-grained control over brain parameters
- **Distributed computing**: Support for parallel brain simulation
- **Cloud integration**: Remote brain simulation and monitoring

## See Also

- [NLM Documentation](https://nlm.readthedocs.io) - Full NLM documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [ROADMAP.md](docs/ROADMAP.md) - Development roadmap