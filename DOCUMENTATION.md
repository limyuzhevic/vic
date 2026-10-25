# NLM Python API Reference

**Version**: 1.0.0  
**Author**: NLM Project  
**Last Updated**: October 25, 2026

---

## Overview

The NLM (Neural Learning Machine) Python API provides a comprehensive interface to the experimental computational brain project. This API exposes the full functionality of the NLM framework, including neural computation, memory systems, neuromodulation, prediction, cognition, and development.

The Python API is built on top of the C++ NLM framework and provides:
- Complete access to all brain systems
- High-level convenience functions for common use cases
- Comprehensive error handling and validation
- Type hints and documentation
- Performance monitoring tools

## Installation

```bash
pip install pynlm
```

Or for development:
```bash
pip install -e .
```

## Quick Start

### Basic Usage

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain()
world = pynlm.createSimpleWorld()

# Initialize
brain.initialize()
world.configure(80, 80, 16, 16)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    brain.step(step, step * 0.001)
    action = agent.decodeMotorCommand()
    # ... process action
```

### High-Level Interface

```python
from pynlm import NLMEnvironment, Agent

# Create environment with learning setup
env = NLMEnvironment(width=100, height=100, vision_width=20, vision_height=20)
env.setup_for_learning(neuron_count=500, enable_curiosity=True)

# Create agent
agent = env.create_agent("curious")

# Run episode
state = env.reset()
for step in range(100):
    action = agent.act(state)
    next_state, reward, done = env.step(action)
    if done:
        break
    state = next_state
```

## Core Classes

### Config
The `Config` class manages NLM configuration parameters.

**Methods:**
- `loadFromFile(filepath)` - Load configuration from file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save configuration to file
- `get(key)` - Get configuration value with type safety
- `getOr(key, default)` - Get value with default
- `set(key, value, source)` - Set configuration value
- `has(key)` - Check if key exists
- `remove(key)` - Remove configuration key
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration
- `summary()` - Get configuration summary

**Example:**
```python
from pynlm import createDefaultConfig

# Create and configure
config = createDefaultConfig()
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.001)
config.set("connection_probability", 0.15)

# Type-safe access
timestep = config.getOr("simulation_timestep", 0.001)
neurons = config.getOr("neuron_count", 100)
```

### Brain
The `Brain` class represents the central neural simulation system.

**Methods:**
- `initialize()` - Initialize brain with configuration
- `step(step, time)` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to neuron type
- `produceAction()` - Generate motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add neural region
- `getRegion(id)` - Get region by ID
- `getRegionCount()` - Get number of regions
- `getRegionIds()` - Get all region IDs
- `getRegions()` - Get all regions
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get active neuron count
- `getFiringNeuronCount()` - Get firing neuron count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I balance
- `getTotalSpikeCount()` - Get total spike count
- `getPendingSpikeEventCount()` - Get pending spike events
- `getDevelopmentalStage()` - Get developmental stage
- `setDevelopmentalStage(stage)` - Set developmental stage
- `getConfig()` - Get configuration
- `getSpikeSystem()` - Get spike system
- `getSTDP()` - Get STDP system
- `getHebbian()` - Get Hebbian system
- `getStructuralPlasticity()` - Get structural plasticity
- `getWorkingMemory()` - Get working memory
- `getEpisodicMemory()` - Get episodic memory
- `getAssociativeMemory()` - Get associative memory
- `getPredictionSystem()` - Get prediction system
- `getPlanner()` - Get neural planner
- `getConceptFormation()` - Get concept formation
- `getAttention()` - Get attentional selection
- `getDevelopmentSystem()` - Get development system
- `getDopamine()` - Get dopamine
- `getCuriosity()` - Get curiosity
- `getNovelty()` - Get novelty
- `getPredictionErrorSignal()` - Get prediction error
- `getRandomGenerator()` - Get random generator
- `addInterRegionConnection(source, target, weight, delay)` - Add inter-region connection
- `removeInterRegionConnection(source, target)` - Remove inter-region connection
- `applyNeuromodulation(signal)` - Apply neuromodulation
- `updatePlasticity()` - Update plasticity rules
- `develop()` - Apply developmental changes
- `logStatus()` - Log brain status

**Example:**
```python
from pynlm import createDefaultConfig, createBrain

config = createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 3)

brain = createBrain(config)
brain.initialize()

print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons")
print(f"Regions: {brain.getRegionCount()}")
print(f"Synaptic connections: {brain.getTotalSynapseCount()}")
```

### AgentBrain
The `AgentBrain` class connects the NLM brain to the world interface.

**Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode brain activity to motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based learning
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get developmental stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent for new episode
- `getBrain()` - Get underlying brain
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity

**Example:**
```python
from pynlm import createBrain, createSimpleWorld, createAgentBrain

brain = createBrain(config)
world = createSimpleWorld()
world.configure(80, 80, 16, 16)

agent_brain = createAgentBrain(brain)
agent_brain.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent_brain.processSensoryInput(percept)

# Get action
action = agent_brain.decodeMotorCommand()

# Apply learning
agent_brain.applyRewardModulation(1.0, 0.8)  # reward, predicted_reward
```

### SimpleWorld
The `SimpleWorld` class represents the simulated environment.

**Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, time)` - Apply motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add object to world
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position is valid
- `getObjectAt(x, y)` - Get object at position
- `getWidth()` - Get world width
- `getHeight()` - Get world height
- `getMaxEnergy()` - Get maximum energy
- `setMaxEnergy(e)` - Set maximum energy
- `getEnergyDecayRate()` - Get energy decay rate
- `setEnergyDecayRate(r)` - Set energy decay rate
- `getSimulationTime()` - Get simulation time
- `setRandomSeed(seed)` - Set random seed
- `getRandomSeed()` - Get random seed

**Example:**
```python
from pynlm import createSimpleWorld

world = createSimpleWorld()
world.configure(100, 100, 32, 32)
world.setAgentStart(50, 50)

# Get current state
percept = world.getSensoryPercept()
agent = world.getAgentBody()

# Update world
world.update(0.001)

# Apply action
result = world.applyMotorCommand(MotorCommand.MoveForward, 0.001)
```

## Sensory Input Types

### Vision
Vision sensory input with configurable dimensions.

**Methods:**
- `__init__(width, height, channels)` - Constructor
- `setData(data)` - Set vision data
- `getWidth()` - Get width
- `getHeight()` - Get height
- `getChannels()` - Get channels

### Audio
Audio sensory input with sample rate and samples.

**Methods:**
- `__init__(sample_rate, num_samples)` - Constructor
- `setData(data)` - Set audio data
- `setSampleRate(sample_rate)` - Set sample rate
- `getSampleRate()` - Get sample rate
- `getNumSamples()` - Get number of samples

### InternalSignals
Internal homeostatic signals.

**Methods:**
- `__init__()` - Constructor
- `addSignal(value)` - Add signal value
- `clearSignals()` - Clear all signals

## Motor Types

### ActionType
Enumeration of action types for motor output.

**Values:**
- `MoveForward`, `MoveBackward`
- `MoveLeft`, `MoveRight`
- `TurnLeft`, `TurnRight`
- `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`
- `Rest`, `Wait`, `Custom`

### MotorCommand
Enumeration of low-level motor commands.

**Values:**
- `MoveForward`, `MoveBackward`
- `TurnLeft`, `TurnRight`
- `LookLeft`, `LookRight`
- `Interact`, `Wait`

### Action
Action representation with type and parameters.

**Methods:**
- `__init__(type)` - Constructor with type
- `__init__(type, parameters)` - Constructor with type and parameters
- `getType()` - Get action type
- `setType(type)` - Set action type
- `getParameters()` - Get action parameters
- `setParameters(params)` - Set action parameters
- `getName()` - Get action name
- `clone()` - Clone action

## World Objects

### WorldObject
Representation of objects in the world.

**Attributes:**
- `x, y` - Position
- `radius` - Radius
- `type` - Object type
- `value` - Reward value or damage
- `active` - Whether object is active

### AgentBody
Agent physical state representation.

**Attributes:**
- `x, y` - Position
- `orientation` - Orientation angle
- `velocityX, velocityY` - Velocity
- `angularVelocity` - Angular velocity
- `energy` - Current energy
- `health` - Current health
- `age` - Age in simulation steps
- `isMoving` - Whether agent is moving
- `isTurning` - Whether agent is turning
- `lastActionTime` - Time of last action

### ActionResult
Result of applying a motor command.

**Attributes:**
- `reward` - Reward received
- `success` - Whether action succeeded
- `message` - Optional message

### SensoryPercept
Sensory data packet received by agent.

**Attributes:**
- `vision` - Vision data (2D grid)
- `touch` - Touch/proximity data
- `internal` - Internal state signals
- `proprioception` - Body position/velocity signals
- `audio` - Audio signals (if enabled)
- `timestamp` - Time of percept

## Factory Functions

### createDefaultConfig()
Creates a default configuration.

**Returns:** Configured Config instance

### createBrain(config)
Creates a new brain with configuration.

**Args:**
- `config` - Configuration for brain

**Returns:** Configured Brain instance

### createSimpleWorld()
Creates a new simple world.

**Returns:** Configured SimpleWorld instance

### createAgentBrain(brain)
Creates a new agent brain interface.

**Args:**
- `brain` - Brain to interface with

**Returns:** Configured AgentBrain instance

## Type Definitions

### Core Types
- `NeuronId` - Unique neuron identifier
- `SynapseId` - Unique synapse identifier  
- `RegionId` - Unique region identifier
- `PopulationId` - Unique population identifier

### Enumerations
- `NeuronType` - Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- `SynapseType` - Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- `DevelopmentalStage` - Initial, CriticalPeriod, Maturation, Adult, Aging
- `FiringState` - Resting, Active, Refractory, Inhibited
- `ActionType` - Various movement and interaction actions
- `MotorCommand` - Low-level motor commands
- `WorldObjectType` - Empty, Resource, Hazard, Wall, Marker

### Measurement Types
- `MembranePotential` - Membrane potential (float)
- `MembraneCurrent` - Membrane current (float)
- `FiringRate` - Firing rate (float)
- `Delay` - Time delay (int)
- `Timestamp` - Simulation time (double)
- `SimulationStep` - Simulation step (size_t)

## Advanced API

### High-Level Interfaces

#### NLMEnvironment
High-level interface for managing NLM simulations.

Provides:
- Environment configuration
- Agent management
- Episode management
- State management
- Reward calculation
- Episode termination detection

#### Agent
High-level agent interface with action selection and learning.

Provides:
- Action selection based on state
- Reward-based learning
- Development tracking
- Curiosity-driven exploration

#### Experiment
Experiment management system for systematic testing.

Provides:
- Multiple agent type comparison
- Episode tracking
- Metrics calculation
- Results analysis
- Visualization
- Result persistence

### Utility Functions

#### Performance Monitoring
```python
from pynlm import monitor_performance

metrics = monitor_performance(brain, duration_steps=100, sampling_interval=10)
```

#### Configuration Validation
```python
from pynlm import validate_configuration

warnings = validate_configuration(config)
```

#### Safe Simulation
```python
from pynlm import safe_step

results = safe_step(brain, world, max_steps=1000, timestep=0.001)
```

#### Quick Start
```python
from pynlm import quick_simulation

results = quick_simulation(width=80, height=80, episodes=5)
```

#### Test Helpers
```python
from pynlm import create_test_brain, create_test_world

brain = create_test_brain()
world = create_test_world()
```

## Error Handling

The API uses custom exception classes for error handling:

- `pynlm.RuntimeError` - General runtime errors
- `ValueError` - Invalid parameter values
- `TypeError` - Type errors
- `RuntimeError` - Simulation errors

### Error Handling Example
```python
try:
    brain = createBrain(config)
    brain.initialize()
    
    # Simulate potential errors
    if brain.getTotalNeuronCount() == 0:
        raise RuntimeError("Brain has no neurons")
    
    for step in range(100):
        brain.step(step, step * 0.001)
        
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except ValueError as e:
    print(f"Invalid value: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Considerations

### Memory Management
- Objects are reference-counted and automatically garbage collected
- No explicit memory management required
- Consider reusing brain/world instances for long-running simulations

### Simulation Performance
- Use efficient data structures for large-scale simulations
- Monitor performance with `monitor_performance()`
- Consider increasing timestep for faster simulation
- Use `safe_step()` for robust simulation with error checking

### Configuration Optimization
- Cache frequently accessed configuration values
- Use `getOr()` with appropriate defaults
- Consider configuration objects for complex parameter sets

## Advanced Usage

### Multi-Agent Simulations
```python
from pynlm import createBrain, createSimpleWorld, createAgentBrain

# Create shared world
world = createSimpleWorld()
world.configure(200, 200, 40, 40)

# Create multiple brains
brains = [createBrain(config) for _ in range(3)]
for brain in brains:
    brain.initialize()

# Create multiple agents
agents = [createAgentBrain(brain) for brain in brains]
for agent in agents:
    agent.initialize(world)

# Run simulation loop
for step in range(100):
    world.update(0.001)
    
    for agent in agents:
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Learning and Adaptation
```python
from pynlm import NLMEnvironment

# Create environment optimized for learning
env = NLMEnvironment()
env.setup_for_learning(
    neuron_count=1000,
    learning_rate=0.02,
    enable_curiosity=True,
    enable_development=True,
    enable_structural_plasticity=True
)

# Create curious agent
agent = env.create_agent("curious")
agent.enable_learning(True)

# Run learning episodes
for episode in range(50):
    state = env.reset()
    
    for step in range(200):
        # Act based on current state
        action = agent.act(state)
        
        # Execute action and get new state
        next_state, reward, done = env.step(action)
        
        # Store experience for learning
        # (Implementation depends on specific learning algorithm)
        
        if done:
            break
        
        state = next_state
```

### Experimentation Framework
```python
from pynlm import Experiment

# Create experiment comparing agent types
experiment = Experiment("Curiosity Effect")

# Create environment
env = experiment.create_environment("complex")

# Run experiments for different agent types
metrics_curious = experiment.run("curious", max_episodes=100)
metrics_conservative = experiment.run("conservative", max_episodes=100)
metrics_exploratory = experiment.run("exploratory", max_episodes=100)

# Analyze and compare results
analysis = experiment.analyze()
experiment.plot_results()
experiment.save_results("experiment_results.json")
```

## Python Version Support

This API supports Python 3.8 and above:

```python
import sys
print(f"Python version: {sys.version}")
```

## Dependencies

### Required
- Python 3.8 or higher
- Standard Python library

### Optional (for advanced features)
- matplotlib - For plotting experiment results
- numpy - For numerical operations
- pandas - For data analysis

Install optional dependencies:
```bash
pip install matplotlib numpy pandas
```

## Examples Directory

The examples directory contains additional demonstration scripts:

- `nlm_examples.py` - Comprehensive examples of all major features
- `tutorial.md` - Step-by-step tutorial for getting started

## References

For more information about NLM and the underlying C++ implementation:

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [Phase 6 Integration Results](docs/PHASE6_FINAL_RESULTS.md)
- [Architecture Documentation](docs/ARCHITECTURE.md)

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright (c) 2026 NLM Project

---

*This documentation was generated from the NLM Python API source code.*
*For the most up-to-date information, see the source files in the repository.*
