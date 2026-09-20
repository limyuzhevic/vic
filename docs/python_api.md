# NLM Python API Reference
# Complete API Documentation for NLM Python Bindings

This document provides comprehensive API documentation for the NLM Python bindings, covering all available classes, functions, and advanced features.

## Overview

The NLM Python API provides access to the complete Neural Learning Machine neural simulation framework through an intuitive Python interface. It includes:

- Brain simulation with real spiking neural dynamics
- World simulation and agent environments
- Advanced neuromodulation and plasticity systems
- Prediction and cognitive systems
- Configuration management
- Advanced simulation utilities

## Core Classes

### Config
Configuration management for NLM systems.

**Constructor:**
```python
config = pynlm.createDefaultConfig()
```

**Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save configuration to file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `set(key, value, source)` - Set configuration value
- `get(key)` - Get configuration value
- `getOr(key, default)` - Get value with default
- `remove(key)` - Remove key
- `clear()` - Clear all entries
- `summary()` - Get configuration summary

**Example:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.002)
config.saveToFile("my_config.json")
```

### Brain
The central neural simulation brain with spiking neurons.

**Constructor:**
```python
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
```

**Methods:**
- `initialize()` - Initialize the brain
- `step(step_number)` - Perform simulation step
- `step(step_number, time)` - Perform step with timestamp
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject current to all neurons of type
- `produceAction()` - Get motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `addRegion(name)` - Add neural region
- `getRegion(id)` - Get region by ID
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getFiringNeuronCount()` - Get currently firing neurons
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I balance
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get developmental stage
- `setDevelopmentalStage(stage)` - Set developmental stage
- `logStatus()` - Log brain status

**Example:**
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### AgentBrain
Interface between brain and world environment.

**Constructor:**
```python
agent = pynlm.createAgentBrain(brain)
```

**Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get current developmental stage
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
- `isRewardModulationEnabled()` - Check if reward modulation enabled
- `isStructuralPlasticityEnabled()` - Check if structural plasticity enabled
- `isDevelopmentEnabled()` - Check if development enabled
- `isCuriosityEnabled()` - Check if curiosity enabled

**Example:**
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
```

### SimpleWorld
Simple 2D world for NLM simulation.

**Constructor:**
```python
world = pynlm.createSimpleWorld()
```

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object
- `isValidPosition(x, y)` - Check if position valid
- `getWidth()`, `getHeight()` - Get world dimensions
- `getMaxEnergy()`, `setMaxEnergy(value)` - Get/set max energy
- `getEnergyDecayRate()`, `setEnergyDecayRate(value)` - Get/set energy decay rate
- `getSimulationTime()` - Get simulation time
- `setRandomSeed(seed)`, `getRandomSeed()` - Get/set random seed

**Example:**
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)
```

### Vision (SensoryInput subclass)
Vision sensory input for visual perception.

**Constructor:**
```python
vision = pynlm.Vision(width, height, channels)
```

**Methods:**
- `setData(data)` - Set vision data
- `getWidth()`, `getHeight()`, `getChannels()` - Get vision dimensions

### Action
Motor command representation.

**Constructor:**
```python
action = pynlm.Action(action_type, parameters)
```

**Methods:**
- `getType()`, `setType(type)` - Get/set action type
- `getParameters()`, `setParameters(params)` - Get/set parameters
- `getName()` - Get action name
- `clone()` - Clone action

### WorldObject
World object representation.

**Constructor:**
```python
obj = pynlm.WorldObject(x, y, object_type, value, radius)
```

**Attributes:**
- `x`, `y` - Position
- `type` - Object type (Resource, Hazard, Wall, etc.)
- `value` - Object value
- `radius` - Object radius
- `active` - Active status

### AgentBody
Agent body state.

**Attributes:**
- `x`, `y` - Position
- `orientation` - Orientation
- `velocityX`, `velocityY` - Linear velocity
- `angularVelocity` - Angular velocity
- `energy` - Energy level
- `health` - Health
- `age` - Age
- `isMoving`, `isTurning` - State flags
- `lastActionTime` - Last action time

### ActionResult
Action result from world interaction.

**Constructor:**
```python
result = pynlm.ActionResult(reward, success, message)
```

**Attributes:**
- `reward` - Reward value
- `success` - Success flag
- `message` - Message string

### SensoryPercept
Sensory percept data combining all modalities.

**Methods:**
- `getVision()`, `setVision(vision)` - Get/set vision
- `getTouch()`, `setTouch(touch)` - Get/set touch
- `getInternal()`, `setInternal(internal)` - Get/set internal signals
- `getProprioception()`, `setProprioception(proprioception)` - Get/set proprioception
- `getAudio()`, `setAudio(audio)` - Get/set audio
- `getAllSignals()` - Get all signals
- `getTimestamp()`, `setTimestamp(timestamp)` - Get/set timestamp

## Enumeration Types

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
- `GapJunction` - Gap junction

### DevelopmentalStage
- `Initial` - Initial stage
- `CriticalPeriod` - Critical period
- `Maturation` - Maturation stage
- `Adult` - Adult stage
- `Aging` - Aging stage

### FiringState
- `Resting` - Resting state
- `Active` - Active state
- `Refractory` - Refractory state
- `Inhibited` - Inhibited state

### ActionType
- `MoveForward`, `MoveBackward` - Movement actions
- `TurnLeft`, `TurnRight` - Turning actions
- `Look`, `LookUp`, `LookDown` - Looking actions
- `Interact` - Interaction action
- `Eat`, `Drink` - Consumption actions
- `Rest`, `Wait` - Resting actions
- `Custom` - Custom action

### MotorCommand
- `MoveForward`, `MoveBackward` - Movement commands
- `TurnLeft`, `TurnRight` - Turning commands
- `LookLeft`, `LookRight` - Looking commands
- `Interact` - Interaction command
- `Wait` - Wait command

### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazard object
- `Wall` - Wall object
- `Marker` - Marker object

## Advanced API Functions

### Configuration Utilities

**createDefaultConfigFromFile(filepath)**
Create configuration from JSON file.

**createConfigFromPythonDict(config_dict)**
Create configuration from Python dictionary.

**saveBrainToFile(brain, filepath)**
Save brain state to file.

**loadBrainFromFile(brain, filepath)**
Load brain state from file.

**exportBrainState(brain)**
Export brain state as string.

**getSimulationInfo(brain)**
Get detailed simulation information.

### Simulation Utilities

**runAdvancedSimulation(brain, world, agent, numSteps)**
Run advanced simulation with agent in world.
Returns tuple: (neuron_count, total_spikes, final_energy)

**createComplexWorld(width, height, visionWidth, visionHeight, maxEnergy, energyDecayRate)**
Create complex world with objects.

**runLearningExperiment(brain, world, agent, numSteps)**
Run comprehensive learning experiment with all advanced features.
Returns tuple: (avg_reward, avg_curiosity, firing_rate, total_spikes)

**benchmarkPerformance(brain, durationSteps)**
Benchmark brain performance (steps/second).
Returns tuple: (steps_per_second, total_spikes, average_firing_rate)

**createSimulationCheckpoint(prefix, maxCheckpoints)**
Create simulation checkpoint system.

**exportBrainStats(brain)**
Export detailed brain statistics.

## Usage Examples

### Basic Simulation
```python
import pynlm

# Create brain and configure
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Complete Agent Simulation
```python
import pynlm

# Setup complete agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply development
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

### Learning Experiment
```python
import pynlm

# Create complex world with objects
world = pynlm.createComplexWorld(width=30, height=30)
world.setAgentStart(15.0, 15.0)

# Create brain and agent
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run learning experiment
avg_reward, avg_curiosity, firing_rate, total_spikes = pynlm.runLearningExperiment(
    brain, world, agent, 1000
)

print(f"Learning Results:")
print(f"  Average Reward: {avg_reward:.3f}")
print(f"  Average Curiosity: {avg_curiosity:.3f}")
print(f"  Average Firing Rate: {firing_rate:.2f} Hz")
print(f"  Total Spikes: {total_spikes}")
```

### Performance Benchmarking
```python
import pynlm

# Create brain for benchmarking
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run benchmark
steps_per_second, total_spikes, avg_firing_rate = pynlm.benchmarkPerformance(
    brain, 10000
)

print(f"Benchmark Results:")
print(f"  Performance: {steps_per_second:.0f} steps/second")
print(f"  Total Spikes: {total_spikes}")
print(f"  Average Firing Rate: {avg_firing_rate:.2f} Hz")
```

### Configuration Management
```python
import pynlm
import json

# Save configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.saveToFile("my_config.json")

# Load configuration
loaded_config = pynlm.createConfigFromFile("my_config.json")

# Create from Python dictionary
config_dict = {
    "brain.neuron_count": 2000,
    "plasticity.hebbian.learning_rate": 0.01,
    "neuromod.dopamine.scale": 1.5
}
dict_config = pynlm.createConfigFromPythonDict(config_dict)
```

## Migration Guide

### From C++ to Python

The NLM Python API provides high-level access to all C++ functionality:

1. **Configuration**: Use dictionaries instead of Config objects
2. **Brain Simulation**: Simplified brain.step() calls
3. **Sensory Input**: Pre-built Vision, Audio, and InternalSignals classes
4. **Action Output**: Unified Action class with type and parameters
5. **Memory Systems**: Abstracted through AgentBrain interface

### From Python to C++

Key differences when moving from Python to C++:

1. **Memory Management**: Manual memory management required in C++
2. **Configuration**: More flexible variant types
3. **Error Handling**: Different exception handling patterns
4. **Performance**: Direct memory access and optimized algorithms

## Performance Considerations

### For High-Performance Applications

1. **Reuse Objects**: Reuse Brain, AgentBrain, and World objects across simulations
2. **Batch Operations**: Use batch operations when possible
3. **Memory Pools**: Leverage NLM's internal memory pools
4. **Configuration Caching**: Cache frequently accessed configuration values

### For Development and Testing

1. **Use Python API**: For rapid prototyping and testing
2. **Leverage High-Level Utilities**: Use provided convenience functions
3. **Detailed Statistics**: Use exportBrainStats() for debugging
4. **Checkpoint System**: Use simulation checkpoints for complex experiments

## Troubleshooting

### Common Issues

1. **Brain Not Firing**: Check that brain.initialize() was called
2. **Agent Not Moving**: Ensure world.applyMotorCommand() is called
3. **Configuration Errors**: Verify JSON format and key names
4. **Memory Issues**: Monitor neuron and synapse counts in large simulations

### Error Messages

- "Brain not initialized": Call brain.initialize() before first step
- "Invalid action type": Use valid ActionType enum values
- "World update error": Check world configuration and boundaries

## Future Enhancements

The NLM Python API continues to evolve with planned features:

1. **Machine Learning Integration**: Neural network training utilities
2. **Visualization Tools**: Real-time brain visualization
3. **Distributed Computing**: Multi-process simulation support
4. **Advanced Learning Algorithms**: Reinforcement learning frameworks
5. **Custom Neurons**: User-defined neuron types and dynamics

## Conclusion

The NLM Python API provides comprehensive access to the Neural Learning Machine framework, combining ease of use with powerful functionality. Whether for research, education, or production simulation, the Python interface offers an intuitive way to explore artificial neural systems with brain-like learning and development capabilities.

For the most recent API documentation and examples, visit the NLM documentation portal at https://nlm.readthedocs.io