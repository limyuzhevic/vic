"""
NLM Python Bindings - Enhanced Documentation

This document provides comprehensive guidance for using the NLM (Neural Learning Machine)
Python bindings. The enhanced bindings provide a more Pythonic and user-friendly interface
to the complex neural simulation framework.

Getting Started
=============

Quick Installation
-------------------

1. Build the NLM project:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

2. Install Python bindings:

```bash
pip install -e .
```

Or build with scikit-build (as specified in pyproject.toml):

```bash
pip install .
```

Basic Usage Examples
====================

Example 1: Creating a Simple Brain
----------------------------------

```python
import pynlm as nlm

# Create a brain with default configuration
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run simulation for a few steps
for step in range(10):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

Example 2: Using Configuration Files
-----------------------------------

```python
import pynlm as nlm

# Create brain from JSON configuration
brain = nlm.createBrainWithConfig("my_config.json")
brain.initialize()

# Create brain from YAML configuration  
brain2 = nlm.createBrainWithConfig("my_config.yaml")
brain2.initialize()

# Quick factory functions
brain3 = nlm.createDefaultBrain(neurons=500, regions=2, connectionProbability=0.2)
brain4 = nlm.createTestBrain()  # Pre-configured for testing
```

Example 3: Working with Agent Brain
------------------------------------

```python
import pynlm as nlm

# Create brain and agent brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
agent_brain = nlm.createAgentBrain(brain)

# Create a simple world
world = nlm.createSimpleWorld()
world.configure(width=32, height=32, visionWidth=16, visionHeight=16)
world.setAgentStart(10, 10)

# Initialize agent
agent_brain.initialize(world)

# Create sensory percept
percept = nlm.SensoryPercept()
# (In a real scenario, this would be populated from world sensing)

# Process sensory input
agent_brain.processSensoryInput(percept)

# Get action
action = agent_brain.decodeMotorCommand()
print(f"Action: {action.getType()}")
```

Example 4: Advanced Configuration Management
--------------------------------------------

```python
import pynlm as nlm
import json

# Create and customize configuration
config = nlm.Config()

# Set values using Pythonic methods
config.set_string("brain_name", "My Neural Network")
config.set_int("neuron_count", 2000)
config.set_float("learning_rate", 0.001)
config.set_bool("enable_curiosity", True)

# Get values with defaults
neuron_count = config.get_or_int("neuron_count", 1000)
learning_rate = config.get_or_float("learning_rate", 0.01)
enable_curiosity = config.get_or_bool("enable_curiosity", False)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = nlm.Config()
config2.loadFromFile("my_config.json")

print(config2.summary())
```

Example 5: Complete Simulation Loop
-----------------------------------

```python
import pynlm as nlm
import time

# Create test brain
brain = nlm.createTestBrain()
brain.initialize()

# Create agent brain and world
agent_brain = nlm.createAgentBrain(brain)
world = nlm.createSimpleWorld()
world.configure(64, 64, 32, 32)
world.setAgentStart(32, 32)
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)

agent_brain.initialize(world)

# Enable neuromodulation systems
agent_brain.enableCuriosity(True)
agent_brain.enableDevelopment(True)

print("Starting simulation...")

# Main simulation loop
for episode in range(10):
    for step in range(100):
        # Get sensory percept from world
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        
        # Get action
        action = agent_brain.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, time.time())
        
        # Get reward
        reward = world.getLastReward()
        predicted_reward = agent_brain.getPredictionError()
        
        # Apply reward modulation
        agent_brain.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent_brain.updateDevelopment(0.1)
        
        # Step brain
        brain.step(step)
    
    # Reset for next episode
    agent_brain.reset()
    brain.reset()
    world.reset()

print("Simulation completed!")
print(f"Final brain state - Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
```

Class Reference
===============

Config Class
------------

The Config class provides enhanced configuration management with support for
JSON and YAML file formats.

**Methods:**

- `__init__()`: Create empty configuration
- `loadFromFile(filepath)`: Load from JSON/YAML file (autodetected)
- `loadFromArgs(argc, argv)`: Load from command line arguments
- `saveToFile(filepath)`: Save to JSON/YAML file (autodetected)
- `has(key)`: Check if key exists
- `get_string(key)`: Get string value or None
- `get_int(key)`: Get integer value or None  
- `get_float(key)`: Get float value or None
- `get_bool(key)`: Get boolean value or None
- `get_or_string(key, default)`: Get value or return default
- `get_or_int(key, default)`: Get integer or return default
- `get_or_float(key, default)`: Get float or return default
- `get_or_bool(key, default)`: Get boolean or return default
- `set(key, value)`: Set configuration value
- `set_string(key, value, source)`: Set string value
- `set_int(key, value, source)`: Set integer value
- `set_float(key, value, source)`: Set float value
- `set_bool(key, value, source)`: Set boolean value
- `getKeys()`: Get all key names
- `clear()`: Clear all configuration
- `summary()`: Get configuration summary string

Brain Class
-----------

The Brain class provides the central neural simulation with integrated systems.

**Methods:**

- `__init__(config)`: Create brain with configuration
- `initialize()`: Initialize brain with configuration
- `step(currentStep, [currentTime])`: Perform simulation step
- `receiveSensoryInput(input)`: Inject sensory input
- `injectCurrent(neuron, current)`: Inject current into specific neuron
- `injectCurrentToNeurons(type, current)`: Inject current to neurons of specific type
- `produceAction()`: Produce motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file
- `addRegion([name])`: Add neural region
- `getRegion(id)`: Get region by ID
- `getRegionCount()`: Get region count
- `getRegionIds()`: Get all region IDs
- `getRegions()`: Get all regions
- `getTotalNeuronCount()`: Get total neuron count
- `getTotalSynapseCount()`: Get total synapse count
- `getActiveNeuronCount()`: Get active neuron count
- `getFiringNeuronCount()`: Get firing neuron count
- `getAverageFiringRate()`: Get average firing rate
- `getExcitationInhibitionRatio()`: Get E/I ratio
- `getTotalSpikeCount()`: Get total spike count
- `getDevelopmentalStage()`: Get developmental stage
- `setDevelopmentalStage(stage)`: Set developmental stage
- `getConfig()`: Get configuration
- `logStatus()`: Log brain status

AgentBrain Class
----------------

The AgentBrain class connects NLM brain to world for sensory-motor interaction.

**Methods:**

- `__init__(brain)`: Create agent brain with underlying brain
- `initialize(world)`: Initialize with world
- `getSensoryInputSize()`: Get expected sensory input size
- `getMotorOutputSize()`: Get expected motor output size
- `processSensoryInput(percept)`: Process sensory percept
- `decodeMotorCommand()`: Decode brain activity to motor command
- `applyRewardModulation(reward, predictedReward)`: Apply reward-based neuromodulation
- `updateDevelopment(timestep)`: Update development system
- `getDevelopmentalStage()`: Get developmental stage
- `getNeuromodulationLevel()`: Get neuromodulation level
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error
- `reset()`: Reset agent for new episode
- `getBrain()`: Get underlying brain
- `enableRewardModulation(enable)`: Enable/disable reward modulation
- `enableStructuralPlasticity(enable)`: Enable/disable structural plasticity
- `enableDevelopment(enable)`: Enable/disable development
- `enableCuriosity(enable)`: Enable/disable curiosity

SensoryPercept Class
--------------------

Represents sensory data packet containing low-level sensory signals.

**Methods:**

- `__init__()`: Create empty percept
- `getVision()`: Get vision data
- `setVision(data)`: Set vision data
- `getVisionWidth()`: Get vision width
- `getVisionHeight()`: Get vision height
- `getTouch()`: Get touch data
- `setTouch(data)`: Set touch data
- `getInternal()`: Get internal signals
- `setInternal(data)`: Set internal signals
- `getProprioception()`: Get proprioception
- `setProprioception(data)`: Set proprioception
- `getAudio()`: Get audio data
- `setAudio(data)`: Set audio data
- `getAllSignals()`: Get all signals concatenated
- `getTimestamp()`: Get timestamp
- `setTimestamp(timestamp)`: Set timestamp

Action Class
------------

Represents motor action for agent output.

**Methods:**

- `__init__([type, parameters])`: Create action
- `getType()`: Get action type
- `setType(type)`: Set action type
- `getParameters()`: Get action parameters
- `setParameters(params)`: Set action parameters
- `getName()`: Get action name
- `clone()`: Create copy of action

SimpleWorld Class
-----------------

Represents simple 2D world for NLM simulation.

**Methods:**

- `__init__()`: Create empty world
- `configure(width, height, visionWidth, visionHeight)`: Configure world dimensions
- `reset()`: Reset world state
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world simulation
- `applyMotorCommand(cmd, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body state
- `addObject(obj)`: Add world object
- `removeObject(x, y)`: Remove object at position
- `isValidPosition(x, y)`: Check if position is valid
- `getWidth()`, `getHeight()`: Get world dimensions
- `getMaxEnergy()`, `setMaxEnergy()`: Get/set max energy
- `getEnergyDecayRate()`, `setEnergyDecayRate()`: Get/set energy decay
- `getSimulationTime()`, `setRandomSeed()`: Get/set random seed

Helper Functions
----------------

- `createDefaultConfig()`: Create default configuration
- `createBrain(config)`: Create brain with configuration
- `createSimpleWorld()`: Create simple world
- `createAgentBrain(brain)`: Create agent brain
- `createBrainWithConfig(filepath)`: Create brain from config file
- `createBrainFromArgs(argc, argv)`: Create brain from command line
- `createDefaultBrain(neurons, regions, connectionProbability)`: Create brain with defaults
- `createTestBrain()`: Create pre-configured test brain
- `createQuickBrain(neurons)`: Quick brain factory

Constants
---------

- `INVALID_NEURON_ID`: Invalid neuron ID
- `INVALID_SYNAPSE_ID`: Invalid synapse ID
- `INVALID_REGION_ID`: Invalid region ID
- `INVALID_POPULATION_ID`: Invalid population ID

Troubleshooting
===============

Common Issues and Solutions
--------------------------

1. **Import Error: ModuleNotFoundError: No module named 'pynlm'**
   - Ensure you've installed the package: `pip install .`
   - Check that you're in the correct directory

2. **Build Errors**
   - Ensure all dependencies are installed
   - Check C++ compiler version (C++20 required)
   - Verify CMake version is 3.16 or higher

3. **Configuration File Errors**
   - Ensure JSON/YAML files are properly formatted
   - Check file extensions match content
   - Use simple key=value format for .txt files

4. **Memory Issues**
   - Large brains may consume significant memory
   - Consider using smaller configurations for testing
   - Monitor system resources during long simulations

5. **Performance Issues**
   - Complex simulations may run slowly
   - Consider reducing neuron count or simulation steps
   - Use test configurations for development

Best Practices
==============

1. **Configuration Management**
   - Use JSON/YAML files for complex configurations
   - Use command line arguments for simple overrides
   - Save configurations after customization

2. **Simulation Design**
   - Start with small brains for testing
   - Use the test brain factory for development
   - Implement proper reset cycles between episodes

3. **Error Handling**
   - Always check return values of initialization methods
   - Handle file I/O errors gracefully
   - Validate configuration parameters before use

4. **Performance Optimization**
   - Profile your application to identify bottlenecks
   - Consider using the enhanced factory functions
   - Monitor memory usage during long simulations

5. **Testing**
   - Use the edge case tests provided
   - Test with various configuration file formats
   - Verify behavior with different brain sizes

Future Enhancements
===================

The enhanced Python bindings provide a foundation for further improvements:

1. **More Advanced Factory Methods**
2. **Additional Convenience Functions**
3. **Better Type Hints and IDE Support**
4. **Interactive Python Notebooks**
5. **Visualization and Plotting Tools**
6. **Parallel Processing Support**
7. **GPU Acceleration Interfaces**

Acknowledgments
===============

This enhanced documentation provides a comprehensive guide to using the NLM Python
bindings. The improvements include better factory methods, enhanced error handling,
and more Pythonic interfaces for improved developer experience.

The NLM project continues to evolve with new features and improvements in each
phase of development. This documentation reflects the current state of Phase 6
Final Integration.
"""