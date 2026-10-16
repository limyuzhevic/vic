# NLM Python Bindings - Enhanced API Documentation

## Overview

This document provides comprehensive documentation for the enhanced Python bindings of the NLM (Neural Learning Machine) framework. The bindings have been significantly improved with Pythonic APIs, better error handling, visualization tools, and advanced features.

## Module Structure

### Core Classes

#### Config
Configuration management for NLM system parameters.

**Key Methods:**
- `createDefaultConfig()` - Create default configuration
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `set(key, value, source)` - Set configuration value
- `get(key)` - Get configuration value
- `has(key)` - Check if key exists
- `getOr(key, default)` - Get with default fallback (NEW)
- `setIfMissing(key, value, source)` - Set only if key doesn't exist (NEW)
- `update(dict, source)` - Update multiple values (NEW)
- `getSection(prefix)` - Get configuration section (NEW)

**Pythonic Features:**
- Safe access with `getOr()` method
- Conditional setting with `setIfMissing()`
- Batch updates with `update()`
- Section-based configuration management

#### Brain
Central neural simulation brain with spiking dynamics.

**Key Methods:**
- `initialize()` - Initialize brain with configuration
- `step(step, time=None)` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current to specific neuron
- `produceAction()` - Generate motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `addRegion(name)` - Add neural region

**Enhanced Methods:**
- `stepMany(start, count)` - Run multiple steps efficiently (NEW)
- `runEpisode(steps)` - Run complete episode (NEW)
- `createRegion(name)` - Alias for `addRegion` (NEW)
- `getNeuronStats()` - Get brain statistics as dictionary (NEW)
- `compare(other)` - Compare with another brain (NEW)
- `clone()` - Create brain copy (NEW)

**Pythonic Features:**
- Efficient batch processing with `stepMany()`
- Comprehensive statistics with `getNeuronStats()`
- Brain comparison and cloning capabilities

#### AgentBrain
Interface connecting NLM brain to world environment.

**Key Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode motor command from brain
- `applyRewardModulation(reward, predicted)` - Apply reward-based learning
- `updateDevelopment(timestep)` - Update development system

**Enhanced Methods:**
- `runEpisode(world, steps)` - Run complete episode with world (NEW)
- `processSequence(percepts)` - Process multiple percepts (NEW)
- `getAgentStats()` - Get agent statistics as dictionary (NEW)
- `isHealthy()` - Check if agent is in healthy state (NEW)

**Pythonic Features:**
- Complete episode management
- Multiple percept processing
- Comprehensive agent state tracking

#### SimpleWorld
Simple 2D simulation environment for NLM agents.

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `update(timestep)` - Update world by timestep
- `applyMotorCommand(cmd, time)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept from world
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position

**Enhanced Methods:**
- `runEpisode(initial_objects)` - Start episode with initial objects (NEW)
- `addResource(x, y, value)` - Add resource (convenience) (NEW)
- `addHazard(x, y, value)` - Add hazard (convenience) (NEW)
- `addWall(x, y, radius)` - Add wall (convenience) (NEW)
- `getWorldStats()` - Get world statistics (NEW)
- `getOccupancy()` - Get agent position information (NEW)

**Pythonic Features:**
- Convenience methods for common world objects
- Episode-based world management
- Comprehensive world statistics

#### Action
Motor action representation for agent output.

**Key Methods:**
- `__init__(type=None, parameters=None)` - Constructor
- `getType()` - Get action type
- `setType(type)` - Set action type
- `getParameters()` - Get action parameters
- `setParameters(params)` - Set action parameters
- `getName()` - Get action name
- `clone()` - Create action copy

**Enhanced Methods:**
- `fromDict(dict)` - Create action from dictionary (NEW)
- `toDict()` - Convert action to dictionary (NEW)
- `isValid()` - Check if action is valid (NEW)
- `withParameters(params)` - Create copy with new parameters (NEW)
- `create(type, params)` - Static factory method (NEW)

**Pythonic Features:**
- Dictionary-based action creation
- Action validation and parameter management
- Static factory for convenient action creation

#### WorldObject
Representation of objects in the simulation world.

**Key Methods:**
- `__init__(x=0, y=0, type=Empty, value=0, radius=0.5)` - Constructor
- Properties: `x`, `y`, `type`, `value`, `radius`, `active`

**Enhanced Methods:**
- `toDict()` - Convert to dictionary (NEW)
- `fromDict(dict)` - Update from dictionary (NEW)
- `isColliding(x, y)` - Check collision with position (NEW)
- `distanceTo(other)` - Calculate distance to another object (NEW)
- `isResource()` - Check if resource (NEW)
- `isHazard()` - Check if hazard (NEW)
- `getTypeName()` - Get human-readable type name (NEW)

**Pythonic Features:**
- Dictionary serialization
- Collision detection and distance calculations
- Object type checking convenience methods

#### AgentBody
Physical state of the agent in the world.

**Key Methods:**
- `__init__()` - Constructor
- Properties: `x`, `y`, `orientation`, `velocityX`, `velocityY`, `angularVelocity`, `energy`, `health`, `age`, `isMoving`, `isTurning`, `lastActionTime`
- `reset()` - Reset body state

**Enhanced Methods:**
- `toDict()` - Convert to dictionary (NEW)
- `fromDict(dict)` - Update from dictionary (NEW)
- `isAlive()` - Check if agent is alive (NEW)
- `moveTo(x, y, orientation)` - Move agent to position (NEW)

**Pythonic Features:**
- Dictionary serialization
- Health and status checking
- Movement convenience methods

#### SensoryPercept
Sensory data received by the agent.

**Key Methods:**
- `getVision()` - Get vision data
- `setVision(vision)` - Set vision data
- `getTouch()` - Get touch data
- `setTouch(touch)` - Set touch data
- `getInternal()` - Get internal signals
- `setInternal(internal)` - Set internal signals
- `getProprioception()` - Get proprioception
- `setProprioception(proprioception)` - Set proprioception
- `getAudio()` - Get audio data
- `setAudio(audio)` - Set audio data

**Enhanced Methods:**
- `toDict()` - Convert to dictionary (NEW)
- `fromDict(dict)` - Update from dictionary (NEW)
- `getCombinedSignal()` - Get first signal (convenience) (NEW)
- `addSignal(signal)` - Add signal (NEW)

**Pythonic Features:**
- Dictionary serialization
- Combined signal access
- Signal management convenience methods

#### ActionResult
Result of an action executed in the world.

**Key Methods:**
- `__init__(reward=0, success=False, message="")` - Constructor
- Properties: `reward`, `success`, `message`

**Enhanced Methods:**
- `toDict()` - Convert to dictionary (NEW)
- `fromDict(dict)` - Update from dictionary (NEW)
- `isSuccessful()` - Check if action was successful (NEW)
- `withMessage(message)` - Create copy with new message (NEW)

**Pythonic Features:**
- Dictionary serialization
- Success checking
- Message convenience methods

### Helper Functions and Utilities

#### Module-level Functions

**Configuration Operations:**
- `createDefaultConfig()` - Create default configuration
- `loadConfig(filepath)` - Load configuration from file
- `saveConfig(config, filepath)` - Save configuration to file

**Brain Operations:**
- `createBrain(config)` - Create brain with configuration
- `saveBrain(brain, filepath)` - Save brain state
- `loadBrain(filepath)` - Load brain state

**World Operations:**
- `createSimpleWorld()` - Create simple world
- `saveWorld(world, filepath)` - Save world state
- `loadWorld(filepath)` - Load world state

**Agent Operations:**
- `createAgentBrain(brain)` - Create agent brain interface

**Batch Processing:**
- `runBatchSimulation(brains, steps)` - Run multiple brains
- `runTrainingEpisode(agent, world, max_steps, max_reward)` - Run training episode
- `benchmarkBrains(brains, steps)` - Benchmark brain performance

**Multi-Agent Support:**
- `createMultiAgentSystem(configs, world)` - Create multi-agent system
- `runMultiAgentEpisode(agents, worlds, steps)` - Run multi-agent episode

**Visualization and Debugging:**
- `visualizeBrain(brain)` - Human-readable brain visualization
- `visualizeWorld(world)` - Human-readable world visualization
- `visualizeAgent(agent)` - Human-readable agent visualization
- `debugStep(brain, step, time)` - Debug simulation step
- `debugAgentStep(agent, world, step)` - Debug agent step
- `profileSimulation(brain, steps)` - Profile simulation performance
- `profileAgentEpisode(agent, world, steps)` - Profile agent episode

#### Exception Classes

- `NLMError` - Base NLM error
- `ConfigError` - Configuration-specific error
- `BrainError` - Brain-specific error
- `WorldError` - World-specific error
- `AgentError` - Agent-specific error

#### Constants

- `INVALID_NEURON_ID` - Invalid neuron ID
- `INVALID_SYNAPSE_ID` - Invalid synapse ID
- `INVALID_REGION_ID` - Invalid region ID
- `INVALID_POPULATION_ID` - Invalid population ID

### Enumerations

#### NeuronType
- `Excitatory` (0) - Excitatory neuron
- `Inhibitory` (1) - Inhibitory neuron
- `Modulatory` (2) - Modulatory neuron
- `Sensory` (3) - Sensory neuron
- `Motor` (4) - Motor neuron
- `Internal` (5) - Internal neuron

#### SynapseType
- `Excitatory` (0) - Excitatory synapse
- `Inhibitory` (1) - Inhibitory synapse
- `Modulatory` (2) - Modulatory synapse
- `Electrical` (3) - Electrical synapse
- `GapJunction` (4) - Gap junction

#### ActionType
- `MoveForward` (0) - Move forward
- `MoveBackward` (1) - Move backward
- `MoveLeft` (2) - Move left
- `MoveRight` (3) - Move right
- `TurnLeft` (4) - Turn left
- `TurnRight` (5) - Turn right
- `Look` (6) - Look
- `LookUp` (7) - Look up
- `LookDown` (8) - Look down
- `Interact` (9) - Interact
- `Eat` (10) - Eat
- `Drink` (11) - Drink
- `Rest` (12) - Rest
- `Wait` (13) - Wait
- `Custom` (14) - Custom action

#### WorldObjectType
- `Empty` (0) - Empty space
- `Resource` (1) - Resource object
- `Hazard` (2) - Hazardous object
- `Wall` (3) - Wall object
- `Marker` (4) - Marker object

#### DevelopmentalStage
- `Initial` (0) - Initial development stage
- `CriticalPeriod` (1) - Critical period
- `Maturation` (2) - Maturation stage
- `Adult` (3) - Adult stage
- `Aging` (4) - Aging stage

#### FiringState
- `Resting` (0) - Neuron is resting
- `Active` (1) - Neuron is active
- `Refractory` (2) - Neuron is refractory
- `Inhibited` (3) - Neuron is inhibited

#### MotorCommand
- `MoveForward` (0) - Move forward
- `MoveBackward` (1) - Move backward
- `TurnLeft` (2) - Turn left
- `TurnRight` (3) - Turn right
- `LookLeft` (4) - Look left
- `LookRight` (5) - Look right
- `Interact` (6) - Interact
- `Wait` (7) - Wait

### Usage Examples

#### Basic Usage
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Pythonic Configuration
```python
config = pynlm.createDefaultConfig()

# Safe access with defaults
learning_rate = config.getOr("learning.rate", 0.001)

# Set only if not present
config.setIfMissing("plasticity.stdp.enable", True)

# Update multiple values
config.update({
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.15
})
```

#### Enhanced Brain Operations
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Efficient batch processing
brain.stepMany(0, 100)

# Get comprehensive statistics
stats = brain.getNeuronStats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing: {stats['firing_neurons']}")

# Run episode
brain.runEpisode(50)
```

#### World with Convenience Methods
```python
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

# Add objects with convenience methods
world.addResource(5, 5, 100.0)
world.addHazard(15, 15, 50.0)
world.addWall(10, 10, 1.0)

# Start episode with initial objects
world.runEpisode({
    "treasure1": pynlm.WorldObject(3, 3, pynlm.WorldObjectType.Resource, 75.0)
})
```

#### Action Management
```python
# Create action using factory method
action = pynlm.Action.create(pynlm.ActionType.Eat, [1.0])

# Create from dictionary
action2 = pynlm.Action()
action2.fromDict({
    "type": pynlm.ActionType.MoveForward,
    "parameters": [0.8, 0.2]
})

# Convert back to dictionary
action_info = action.toDict()
```

#### Serialization
```python
# Save brain state
pynlm.saveBrain(brain, "my_brain.bin")

# Load brain state
loaded_brain = pynlm.loadBrain("my_brain.bin")

# Save configuration
pynlm.saveConfig(config, "my_config.json")

# Load configuration
loaded_config = pynlm.loadConfig("my_config.json")
```

#### Batch Processing
```python
# Create multiple brains
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(5)]
for b in brains:
    b.initialize()

# Run batch simulation
results = pynlm.runBatchSimulation(brains, 50)

# Benchmark performance
benchmark = pynlm.benchmarkBrains(brains, 100)
```

#### Multi-Agent System
```python
# Create multiple agents
configs = [pynlm.createDefaultConfig() for _ in range(3)]
world = pynlm.createSimpleWorld()

# Create multi-agent system
multi_agent = pynlm.createMultiAgentSystem(configs, world)
agents = multi_agent["agents"]

# Run multi-agent episode
pynlm.runMultiAgentEpisode(agents, [world, pynlm.createSimpleWorld(), pynlm.createSimpleWorld()], 30)
```

#### Visualization and Debugging
```python
# Get visualizations
brain_viz = pynlm.visualizeBrain(brain)
world_viz = pynlm.visualizeWorld(world)
agent_viz = pynlm.visualizeAgent(agent)

# Debug single step
debug_info = pynlm.debugStep(brain, 0, 0.0)

# Profile simulation
profile = pynlm.profileSimulation(brain, 200)
```

### Migration Guide

#### From Old API to New API

**Configuration Access:**
```python
# Old
value = config.get("learning.rate")
if value:
    learning_rate = value
else:
    learning_rate = 0.001

# New
learning_rate = config.getOr("learning.rate", 0.001)
```

**Brain Operations:**
```python
# Old
for i in range(10):
    brain.step(i)

# New
brain.runEpisode(10)
```

**World Operations:**
```python
# Old
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(10, 10, 5, 5)

# New
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(10, 10, 5, 5)
```

**Action Creation:**
```python
# Old
action = pynlm.Action()
action.setType(pynlm.ActionType.Eat)
action.setParameters([1.0])

# New
action = pynlm.Action.create(pynlm.ActionType.Eat, [1.0])
```

### Best Practices

1. **Use Pythonic APIs:** Prefer the new convenience methods for cleaner code.
2. **Handle Exceptions:** Use try-catch blocks for error handling.
3. **Use Dictionary Operations:** Leverage dictionary-based methods for configuration and object management.
4. **Profile Performance:** Use `profileSimulation()` and `benchmarkBrains()` for performance analysis.
5. **Debug Effectively:** Use `debugStep()` and `debugAgentStep()` for detailed debugging.
6. **Serialize State:** Use `save/load` functions for preserving simulation state.
7. **Batch Operations:** Use batch processing methods for multiple brain/world operations.
8. **Multi-Agent Systems:** Use `createMultiAgentSystem()` for complex agent scenarios.

### Performance Tips

1. **Batch Processing:** Use `stepMany()` for multiple simulation steps.
2. **Efficient Serialization:** Use `save/load` functions for state preservation.
3. **Memory Management:** Use `clone()` for creating brain copies.
4. **Parallel Processing:** Use `runBatchSimulation()` for multiple brains.
5. **Profiling:** Use `benchmarkBrains()` for performance optimization.

### Troubleshooting

**Common Issues:**
- `brain.initialize()` not called before `brain.step()` → Always call `initialize()` first
- Configuration key not found → Use `getOr(key, default)` for safe access
- World collision → Check `isValidPosition()` before applying actions
- Performance issues → Use `profileSimulation()` for optimization

**Error Handling:**
```python
try:
    brain.initialize()
    brain.step(0)
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Future Enhancements

The Python bindings continue to evolve with:

1. **Additional Convenience Methods:** More methods added based on user feedback
2. **Performance Optimizations:** Continued optimization of batch operations
3. **New Visualization Tools:** Enhanced debugging and profiling capabilities
4. **Integration Features:** Better support for multi-agent systems
5. **Serialization Formats:** Support for additional serialization formats
6. **Configuration Wizards:** Guided configuration setup
7. **Template Systems:** Predefined configurations for common scenarios
8. **Testing Frameworks:** Comprehensive testing and validation tools

The enhanced Python bindings provide a robust, Pythonic interface to the NLM neural simulation framework, making it accessible to both beginners and advanced users while maintaining high performance and flexibility.