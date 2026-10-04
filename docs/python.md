# NLM Python API Documentation

This document describes the Python API for the NLM (Neural Learning Machine) library.
Wave 5 Enhanced Version with comprehensive documentation, simplified API, and modern Python features.

## Quick Start

The NLM Python bindings provide access to a complete neural learning system through an intuitive Python interface. This enhanced version (Wave 5) includes comprehensive documentation, simplified beginner-friendly APIs, and modern Python features.

### Installation

```bash
pip install pynlm
```

The package includes the compiled C++ bindings with all NLM functionality.

### Basic Usage

```python
import pynlm

# Create and initialize a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Core Classes

### Brain

The main neural simulation brain class representing the computational engine of the NLM system.

#### Constructor
```python
Brain(config=None)
```

#### Methods
- `initialize()`: Initialize the brain with configuration
- `step(currentStep, currentTime=None)`: Perform a simulation step
- `receiveSensoryInput(input)`: Inject sensory data into the brain
- `injectCurrent(neuron, current)`: Inject current into a specific neuron
- `injectCurrentToNeurons(type, current)`: Inject current into all neurons of a specific type
- `produceAction()`: Generate motor command based on neural activity
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file
- `addRegion(name="")`: Add a new neural region
- `getRegion(id)`: Get a region by ID
- `getRegionCount()`: Get the number of regions
- `getRegionIds()`: Get all region IDs
- `getRegions()`: Get all regions
- `getTotalNeuronCount()`: Get total neuron count
- `getTotalSynapseCount()`: Get total synapse count
- `getActiveNeuronCount()`: Get count of active neurons
- `getFiringNeuronCount()`: Get count of currently firing neurons
- `getAverageFiringRate()`: Get average firing rate
- `getExcitationInhibitionRatio()`: Get network balance ratio
- `getTotalSpikeCount()`: Get total spike count
- `getDevelopmentalStage()`: Get current developmental stage
- `setDevelopmentalStage(stage)`: Set developmental stage
- `getConfig()`: Get the brain's configuration
- `logStatus()`: Log brain status

#### Properties
- `config`: Configuration object for the brain

### AgentBrain

Bridge between NLM brain and world, handling sensory transduction and motor decoding.

#### Constructor
```python
AgentBrain(brain)
```

#### Methods
- `initialize(world)`: Initialize with world reference
- `getSensoryInputSize()`: Get expected sensory input size
- `getMotorOutputSize()`: Get expected motor output size
- `processSensoryInput(percept)`: Process sensory percept
- `decodeMotorCommand()`: Decode neural activity to action
- `applyRewardModulation(reward, predictedReward)`: Apply reward-based learning
- `updateDevelopment(timestep)`: Update development system
- `getDevelopmentalStage()`: Get developmental stage
- `getNeuromodulationLevel()`: Get neuromodulation level
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error
- `reset()`: Reset agent
- `getBrain()`: Get underlying brain

#### Configuration Methods
- `enableRewardModulation(enable)`: Enable/disable reward modulation
- `enableStructuralPlasticity(enable)`: Enable/disable structural plasticity
- `enableDevelopment(enable)`: Enable/disable development
- `enableCuriosity(enable)`: Enable/disable curiosity
- `isRewardModulationEnabled()`: Check if reward modulation enabled
- `isStructuralPlasticityEnabled()`: Check if structural plasticity enabled
- `isDevelopmentEnabled()`: Check if development enabled
- `isCuriosityEnabled()`: Check if curiosity enabled

### SimpleWorld

Simple 2D world for NLM simulation.

#### Constructor
```python
SimpleWorld()
```

#### Configuration
- `configure(width, height, visionWidth, visionHeight)`: Set world dimensions
- `setAgentStart(x, y)`: Set agent starting position

#### Core Methods
- `reset()`: Reset world to initial state
- `update(timestep)`: Update world physics
- `applyMotorCommand(cmd, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get current perception
- `getAgentBody()`: Get agent state

#### Object Management
- `addObject(obj)`: Add object to world
- `removeObject(x, y)`: Remove object at position
- `isValidPosition(x, y)`: Check if position accessible

#### Properties
- `width`: World width
- `height`: World height
- `maxEnergy`: Maximum energy level
- `energyDecayRate`: Energy decay rate
- `simulationTime`: Current simulation time

### Config

Configuration system for NLM parameters.

#### Constructor
```python
Config()
```

#### Methods
- `loadFromFile(filepath)`: Load from JSON file
- `loadFromArgs(argc, argv)`: Load from command line
- `saveToFile(filepath)`: Save to JSON file
- `has(key)`: Check if key exists
- `getKeys()`: Get all configuration keys
- `clear()`: Clear all entries
- `summary()`: Get configuration summary

#### Configuration Categories
- `brain`: Neural network parameters
- `world`: Environment settings
- `agent`: Agent behavior
- `learning`: Learning algorithms
- `development`: Developmental settings

### SensoryInput Classes

Base class for all sensory input types.

#### Vision
- `Vision(width, height, channels=3)`: Create vision input
- `getWidth()`, `getHeight()`, `getChannels()`: Get dimensions
- `setData(data)`: Set pixel data

#### Audio
- `Audio(sampleRate, numSamples)`: Create audio input
- `getSampleRate()`, `getNumSamples()`: Get audio properties
- `setData(data)`, `setSampleRate(rate)`: Set audio data

#### InternalSignals
- `InternalSignals()`: Create internal signals
- `addSignal(value)`: Add signal
- `clearSignals()`: Clear all signals

### Action and Related Classes

#### Action
- `Action(type)`: Create action with type
- `Action(type, parameters)`: Create action with parameters
- `getType()`, `getParameters()`, `getName()`: Get action properties
- `setType(type)`, `setParameters(params)`: Set action properties
- `clone()`: Create copy of action

#### WorldObject
- `WorldObject(x, y, type, value=0.0, radius=0.5)`: Create world object

#### AgentBody
- Physical state of agent (position, velocity, energy, health, etc.)

#### ActionResult
- `ActionResult(reward, success, message="")`: Create action result

### Context Manager: BrainSimulator

```python
with pynlm.BrainSimulator() as simulator:
    brain = simulator.brain
    brain.initialize()
    simulator.run_steps(100)
```

Context manager for automatic brain initialization and cleanup.

## Simplified Beginner API

The Wave 5 bindings include a simplified beginner-friendly API that makes it easier to get started with NLM.

### Beginner Functions

```python
import pynlm

# Create brain with defaults
brain = pynlm.beginner_createBrain()

# Create world with defaults
world = pynlm.beginner_createWorld()

# Run complete simulation with logging
total_spikes = pynlm.beginner_runSimulation(brain, steps=1000, verbose=True)

# Create default config
config = pynlm.beginner_createDefaultConfig()
```

### Benefits of Beginner API

- No need to understand configuration details
- Automatic initialization
- Built-in error handling
- Helpful logging and progress reporting
- Consistent, predictable behavior

## Modern Python Features

### Type Conversion Utilities

```python
# Convert between Python types and NLM types
neuron_id = pynlm.asNeuronId(12345)
synapse_id = pynlm.asSynapseId(67890)
```

### Constants

```python
# Invalid identifiers for error handling
invalid_neuron_id = pynlm.INVALID_NEURON_ID
invalid_synapse_id = pynlm.INVALID_SYNAPSE_ID
invalid_region_id = pynlm.INVALID_REGION_ID
invalid_population_id = pynlm.INVALID_POPULATION_ID
```

### Version and Metadata

```python
# Access package information
version = pynlm.__version__
author = pynlm.__author__
description = pynlm.__description__
```

## Enumerations

### NeuronType
- `Excitatory`: Neurons that excite other neurons
- `Inhibitory`: Neurons that inhibit other neurons
- `Modulatory`: Neurons that modulate network state
- `Sensory`: Neurons that process sensory input
- `Motor`: Neurons that produce motor output
- `Internal`: Neurons for internal processing

### SynapseType
- `Excitatory`: Synapses that increase firing probability
- `Inhibitory`: Synapses that decrease firing probability
- `Modulatory`: Synapses that affect network modulation
- `Electrical`: Direct electrical connections
- `GapJunction`: Specialized electrical coupling

### DevelopmentalStage
- `Initial`: Newborn, high plasticity
- `CriticalPeriod`: Sensitive learning phase
- `Maturation`: Stabilization and refinement
- `Adult`: Optimized performance
- `Aging`: Decline in plasticity

### ActionType
- `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`
- `TurnLeft`, `TurnRight`
- `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`, `Rest`
- `Wait`, `Custom`

### MotorCommand
- `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`
- `LookLeft`, `LookRight`, `Interact`, `Wait`

### WorldObjectType
- `Empty`: No object
- `Resource`: Consumable resource
- `Hazard`: Dangerous object
- `Wall`: Impassable barrier
- `Marker`: Special location

## Error Handling

Wave 5 includes improved error handling with Pythonic messages.

### Validation Errors

```python
world = pynlm.safeCreateWorld(3.0, 3.0, 8, 8)  # Will raise ValueError
```

Error messages include suggestions for correct usage.

### Type Errors

```python
brain = pynlm.beginner_createBrain()
if brain is None:
    raise RuntimeError("Brain creation failed")
```

## Examples Directory

Complete examples are available in the `examples/` directory:

### 01_basic_brain.py
Basic brain creation and simulation (see [Beginner Examples](examples/beginner.md))

### 02_agent_simulation.py
Complete agent simulation with world interaction (see [Intermediate Examples](examples/intermediate.md))

### 03_advanced_features.py
Advanced features and all learning mechanisms (see [Advanced Examples](examples/advanced.md))

## Performance Tips

### For Large Brains

```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()
```

### For Fast Simulation

```python
# Use bulk operations where possible
# Configure world for optimal performance
world = pynlm.createWorld(50, 50, 20, 20)
world.setRandomSeed(12345)  # Reproducible simulations

# Use efficient sensor processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

### Memory Management

```python
# Let context managers handle cleanup
with pynlm.BrainSimulator() as simulator:
    simulator.run_steps(1000)
    # Automatic reset and cleanup
```

## Advanced Usage

### Custom Configuration

```python
config = pynlm.createDefaultConfig()

# Configure brain development
config.set("development.initial_plasticity", 2.0)
config.set("development.critical_period_length", 1000)
config.set("development.adult_plasticity_modifier", 0.05)

# Configure world physics
config.set("world.width", 100.0)
config.set("world.height", 50.0)
config.set("world.gravity", -9.8)
config.set("world.friction", 0.1)

brain = pynlm.createBrain(config)
```

### Complex Agent Behavior

```python
# Create agent with all features
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Enable learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complex simulation
for step in range(5000):
    world.update(0.05)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.05)
    agent.applyRewardModulation(reward, agent.getPredictionError())
    agent.updateDevelopment(0.05)
```

## Best Practices

1. **Use Beginner API for Learning**: Start with `beginner_*` functions to understand the basics.

2. **Enable Learning Features**: For realistic behavior, enable reward modulation, curiosity, and development.

3. **Use Context Managers**: The `BrainSimulator` context manager ensures proper cleanup.

4. **Validate Inputs**: Use `safeCreateWorld()` for better error messages.

5. **Monitor Performance**: Track spikes, firing rates, and energy for optimization.

6. **Reproducible Simulations**: Set random seeds for consistent results.

7. **Gradual Complexity**: Start simple, then add features incrementally.

## Migration Guide

### From Old API (Wave 4) to New API (Wave 5)

#### Before (Wave 4):
```python
import pynlm

# Complex configuration required
config = pynlm.Config()
config.loadFromFile("config.json")
brain = pynlm.Brain(config)
brain.initialize()

# No documentation or examples
```

#### After (Wave 5):
```python
import pynlm

# Simplified beginner API
brain = pynlm.beginner_createBrain()
brain.initialize()

# Comprehensive documentation and examples
# Modern error handling and Python features
```

## Acknowledgments

This enhanced Python API (Wave 5) includes contributions from:
- The NLM development team
- Python community for best practices
- Documentation generators for improved API docs
- All open-source projects referenced in this documentation

## Version Information

- **Package Version**: `0.5.0-wave5-enhanced`
- **Author**: NLM Project (Wave 5)
- **Description**: Enhanced Python bindings with comprehensive documentation, simplified API, and modern Python features
- **Dependencies**: None (pure Python interface over C++ implementation)

For additional support, visit the [NLM Project Documentation](https://nlm.readthedocs.io) or the [NLM GitHub Repository](https://github.com/nlm-project/nlm).