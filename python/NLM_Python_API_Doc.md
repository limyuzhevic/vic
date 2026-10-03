# NLM Python API Documentation

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python API. The Python bindings expose the full functionality of the C++ NLM framework, including brain simulation, memory systems, prediction, and advanced experimentation capabilities.

## Overview

The NLM Python API provides access to a brain-inspired spiking neural network simulator that implements:
- Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- Synaptic delays and multiple plasticity mechanisms (STDP, Hebbian learning, structural plasticity)
- Working memory, episodic memory, semantic memory, procedural memory, and associative memory
- Prediction systems for sensory prediction and error computation
- Neuromodulation systems (dopamine, curiosity, novelty detection)
- Development and learning systems
- Integration experiment framework (Phase 6)

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation steps
for step in range(100):
    brain.step(step)

print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
```

### Agent Simulation

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation completed!")
```

## Core Classes

### Config

Configuration management for NLM systems. The Config class provides type-safe access to configuration values with support for strings, integers, floats, booleans, and vector types.

**Key Methods:**
- `__init__()` - Create empty configuration
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save to JSON file
- `has(key)` - Check if key exists
- `get(key)` - Get value (raises if not found)
- `get_int(key)` - Get integer value
- `get_float(key)` - Get float value
- `get_bool(key)` - Get boolean value
- `getOr(key, default)` - Get with default value
- `set(key, value)` - Set value (overloads for different types)
- `items()` - Get all configuration as dictionary
- `getKeys()` - Get all keys
- `summary()` - Get summary string

**Example:**
```python
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuronCount", 1000)
config.set("brain.regionCount", 5)
config.set("brain.connectionProbability", 0.1)

# Get configuration values
neuron_count = config.get_int("brain.neuronCount")
print(f"Neuron count: {neuron_count}")

# Python-style access
config_dict = config.items()
print(config_dict)
```

### Brain

The central neural simulation brain class that implements spiking neural computation with event-driven dynamics and integrated memory, prediction, cognition, and neuromodulation systems.

**Key Methods:**
- `__init__(config)` - Create brain with configuration
- `initialize()` - Initialize brain with configuration
- `step(step)` - Perform simulation step
- `step(step, time)` - Perform simulation step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current to specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to neurons of specific type
- `getSpikeSystem()` - Get spike system
- `getSTDP()` - Get STDP plasticity system
- `getHebbian()` - Get Hebbian plasticity system
- `getStructuralPlasticity()` - Get structural plasticity system
- `getWorkingMemory()` - Get working memory system
- `getEpisodicMemory()` - Get episodic memory system
- `getAssociativeMemory()` - Get associative memory system
- `getPredictionSystem()` - Get prediction system
- `getPlanner()` - Get neural planner
- `getConceptFormation()` - Get concept formation system
- `getAttention()` - Get attentional selection system
- `getDevelopmentSystem()` - Get development system
- `getDopamine()` - Get dopamine neuromodulator
- `getCuriosity()` - Get curiosity neuromodulator
- `getNovelty()` - Get novelty detection system
- `getPredictionErrorSignal()` - Get prediction error signal
- `produceAction()` - Produce motor/action output
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add neural region
- `getRegion(id)` - Get region by ID
- `getRegions()` - Get all regions
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get active neuron count
- `getFiringNeuronCount()` - Get firing neuron count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get excitation/inhibition ratio
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get developmental stage
- `setDevelopmentalStage(stage)` - Set developmental stage
- `getRandomGenerator()` - Get random generator
- `logStatus()` - Log brain status

**Example:**
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Configure the brain
brain.addRegion("main_brain")
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Initial)

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Get statistics
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### SimpleWorld

The 2D world simulation environment where the agent interacts.

**Key Methods:**
- `__init__()` - Create empty world
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world by timestep
- `applyMotorCommand(cmd, time)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position is valid
- `getWidth()`, `getHeight()` - Get world dimensions
- `getMaxEnergy()`, `setMaxEnergy(e)` - Get/set max energy
- `getEnergyDecayRate()`, `setEnergyDecayRate(r)` - Get/set energy decay rate
- `getSimulationTime()` - Get current simulation time
- `setRandomSeed(seed)` - Set random seed

**Example:**
```python
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

# Add objects to world
obj1 = pynlm.WorldObject(x=5, y=5, type=pynlm.WorldObjectType.Resource, value=10)
obj2 = pynlm.WorldObject(x=25, y=25, type=pynlm.WorldObjectType.Hazard, value=-5)

world.addObject(obj1)
world.addObject(obj2)

# Run simulation
for step in range(100):
    world.update(0.1)
    
    # Get agent's view
    percept = world.getSensoryPercept()
    
    # Process percept...
```

### AgentBrain

The agent brain interface that connects the NLM brain to the world, handling sensory transduction and motor decoding.

**Key Methods:**
- `__init__(brain)` - Create agent brain with brain instance
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get developmental stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent
- `getBrain()` - Get underlying brain
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity

**Example:**
```python
# Create complete agent system
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Monitor agent state
    if step % 50 == 0:
        print(f"Step {step}: Curiosity {agent.getCuriosityLevel():.2f}, Novelty {agent.getNoveltyLevel():.2f}")
```

### Action

Motor action representation for motor output from the brain.

**Key Methods:**
- `__init__()` - Create empty action
- `__init__(type)` - Create action with type
- `__init__(type, parameters)` - Create action with type and parameters
- `getType()` - Get action type
- `setType(type)` - Set action type
- `getParameters()` - Get action parameters
- `setParameters(params)` - Set action parameters
- `getName()` - Get action name
- `clone()` - Clone action

### WorldObject

World object representation for the environment.

**Key Methods:**
- `__init__()` - Create empty object
- `__init__(x, y, type, value, radius)` - Create with position and properties
- Attributes: `x`, `y`, `radius`, `type`, `value`, `active`

### SensoryPercept

Sensory percept data container for agent perception.

**Key Methods:**
- `__init__()` - Create empty percept
- `getVision()`, `setVision(vision)` - Vision data
- `getTouch()`, `setTouch(touch)` - Touch data
- `getInternal()`, `setInternal(internal)` - Internal signals
- `getProprioception()`, `setProprioception(proprioception)` - Proprioception
- `getAudio()`, `setAudio(audio)` - Audio data
- `getAllSignals()` - Get all signals
- `getTimestamp()`, `setTimestamp(timestamp)` - Timestamp

### ActionResult

Action result from the world perspective.

**Key Methods:**
- `__init__()` - Create empty result
- `__init__(reward, success, message)` - Create with values
- Attributes: `reward`, `success`, `message`
- `to_dict()` - Convert to Python dictionary

## Advanced Features

### Memory Systems

The NLM framework includes comprehensive memory systems that are accessible through the Brain class:

- **Working Memory**: Transient active information storage
- **Episodic Memory**: Experience and event storage
- **Semantic Memory**: Gradually acquired knowledge
- **Associative Memory**: Pattern associations between neurons

**Access via Brain class:**
```python
brain = pynlm.createBrain(config)

# Get memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
semantic_memory = brain.getSemanticMemory()  # Phase 2 only
associative_memory = brain.getAssociativeMemory()
```

### Prediction System

The prediction system for predicting future sensory states and computing prediction errors.

**Access via Brain class:**
```python
brain = pynlm.createBrain(config)
prediction_system = brain.getPredictionSystem()
```

### Neuromodulation Systems

Integrated neuromodulation systems that affect learning and behavior:

**Access via Brain class:**
```python
brain = pynlm.createBrain(config)

# Get neuromodulators
dopamine = brain.getDopamine()  # Reward and reinforcement
curiosity = brain.getCuriosity()  # Exploration motivation
novelty = brain.getNovelty()  # Novelty detection
prediction_error = brain.getPredictionErrorSignal()  # Prediction error
```

### Development System

The development system that models brain maturation and changing plasticity rates.

**Access via Brain class:**
```python
brain = pynlm.createBrain(config)
development_system = brain.getDevelopmentSystem()
```

### Cognition Systems

Advanced cognitive processing systems:

**Access via Brain class:**
```python
brain = pynlm.createBrain(config)

# Get cognition systems
planner = brain.getPlanner()  # Action planning
concept_formation = brain.getConceptFormation()  # Pattern discovery
attention = brain.getAttention()  # Attentional selection
```

### Experiment and Metrics

The experiment system for recording and analyzing simulation data.

**Key Classes:**
- **Experiment**: Records data from a single experimental run
- **Metrics**: Statistical analysis of recorded metrics
- **Phase6IntegratedExperiment**: Complete integration test of all brain systems

**Example:**
```python
# Create experiment
experiment = pynlm.Experiment("Integration Test")

# Record configuration
experiment.setConfigValue("brain.neuronCount", "1000")
experiment.setConfigValue("world.size", "20")

# Run experiment with metrics
metrics = experiment.getMetrics()

# Record experiment data
experiment.recordStep(step, brain.getTotalNeuronCount(), 
                     brain.getTotalSynapseCount(),
                     brain.getFiringNeuronCount(),
                     brain.getAverageFiringRate())
```

## Configuration

The NLM framework supports extensive configuration through the Config class. Configuration keys follow a hierarchical naming convention:

### Brain Configuration
- `brain.neuronCount` - Number of neurons
- `brain.regionCount` - Number of neural regions
- `brain.connectionProbability` - Probability of connections
- `brain.plasticity.stdp LTP` - STDP LTP weight
- `brain.plasticity.stdp LTD` - STDP LTD weight
- `brain.development.initialPlasticity` - Initial plasticity rate
- `brain.neuromodulation.dopamineSensitivity` - Dopamine sensitivity

### World Configuration
- `world.size` - World dimensions
- `world.vision.width` - Vision width
- `world.vision.height` - Vision height
- `world.maxEnergy` - Maximum energy
- `world.energyDecayRate` - Energy decay rate

### Memory Configuration
- `memory.working.capacity` - Working memory capacity
- `memory.episodic.maxEpisodes` - Maximum episodic memory entries
- `memory.semantic.maxFacts` - Maximum semantic facts

### Agent Configuration
- `agent.sensory.visionWidth` - Vision sensor width
- `agent.sensory.visionHeight` - Vision sensor height
- `agent.motor.actions` - Available motor actions

## Error Handling

The NLM Python API provides comprehensive error handling with custom exceptions:

- **NLMException**: Base exception for all NLM errors
- **RuntimeError**: For runtime errors
- **LogicError**: For logic errors

**Error handling example:**
```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    
    # This will raise NLMException if key not found
    neuron_count = config.get_int("brain.neuronCount")
    
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Type Enumerations

The NLM framework uses several enumeration types for different concepts:

### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Modulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

### SynapseType
- `Excitatory` - Excitatory synapses
- `Inhibitory` - Inhibitory synapses
- `Modulatory` - Modulatory synapses
- `Electrical` - Electrical synapses
- `GapJunction` - Gap junctions

### DevelopmentalStage
- `Initial` - Initial development stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### ActionType
- `MoveForward`, `MoveBackward` - Movement actions
- `TurnLeft`, `TurnRight` - Rotation actions
- `Look`, `LookUp`, `LookDown` - Looking actions
- `Interact` - Interaction actions
- `Eat`, `Drink` - Consumption actions
- `Rest`, `Wait` - Rest actions

### MotorCommand
- `MoveForward`, `MoveBackward` - Basic movement
- `TurnLeft`, `TurnRight` - Rotation
- `LookLeft`, `LookRight` - Pan sensor
- `Interact` - Interaction
- `Wait` - No movement

### WorldObjectType
- `Empty` - Empty space
- `Resource` - Positive reward object
- `Hazard` - Negative reward object
- `Wall` - Boundary wall
- `Marker` - Navigation aid

## Performance Tips

### Memory Management
- Reuse Config objects when possible
- Clear working memory periodically for long simulations
- Use checkpoints for long-running experiments

### Simulation Performance
- Use appropriate timestep sizes
- Monitor memory usage during long simulations
- Consider parallel processing for large brain models

### Python Integration
- Batch sensory input processing when possible
- Use numpy arrays for large sensor data
- Minimize attribute access in tight loops

## Migration Guide

### From Easy Usage to Full API

The `easy_usage.md` provides simplified patterns for beginners. The full `pynlm` module provides more control and access to advanced features:

1. **Simple Pattern**: Use `easy_usage.md` examples for basic simulations
2. **Advanced Pattern**: Use `pynlm` module for complex configurations
3. **Production Pattern**: Combine both for scalable applications

### Version Compatibility

- NLM v0.1.0: Initial Python API release
- All changes are backward compatible
- New features are additive

## Troubleshooting

### Common Issues

**"Brain isn't doing anything"**
- Ensure `brain.initialize()` is called before simulation steps
- Check that sensory input is properly connected
- Verify that neurons have sufficient input

**"Agent isn't moving"**
- Ensure `world.applyMotorCommand()` is called
- Check that `world.update()` is being called
- Verify that motor neurons are receiving input

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - ensure sensory input is connected
- Check that the agent is properly initialized

**Configuration errors**
- Ensure configuration keys are correct
- Check data types match (int vs float vs string)
- Use `config.has(key)` to check key existence

### Debugging

**Enable verbose logging:**
```python
brain.logStatus()
```

**Check configuration:**
```python
config = pynlm.createDefaultConfig()
print(config.summary())
```

**Monitor simulation progress:**
```python
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: Status OK")
```

## License

MIT License. See `LICENSE` file for details.

## Authors

NLM Research Project - See `README.md` for scientific background and documentation.
