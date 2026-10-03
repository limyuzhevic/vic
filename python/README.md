# NLM - Enhanced Python API Documentation

This document provides comprehensive documentation for the enhanced NLM Python API.

## Overview

The NLM (Neural Learning Machine) Python bindings provide a Python interface to the NLM C++ neural simulation framework. This enhanced version provides improved Pythonic API, comprehensive documentation, and additional convenience methods for easier usage and advanced functionality.

## Installation

### Standard Installation

```bash
pip install pynlm
```

### Development Installation

```bash
# Clone the repository
cd /path/to/nlm

# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the Python bindings
pip install .

# Or install in development mode
pip install -e .
```

### Build from Source

```bash
# Navigate to project directory
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Build Python bindings specifically
pip install .
```

## Basic Usage

### Quick Start

The simplest way to get started with NLM:

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
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Complete Agent Example

A complete example showing an agent interacting with a world:

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
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
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

## API Reference

### Core Classes

#### Brain
The central neural computation engine.

**Key Methods:**
- `createBrain(config)` - Create a new brain with configuration
- `initialize()` - Initialize the brain with configuration
- `step(step, time)` - Perform a simulation step
- `receiveSensoryInput(input)` - Inject sensory input into the brain
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Properties:**
- `total_neuron_count` - Total neuron count across all regions
- `total_synapse_count` - Total synapse count across all regions
- `firing_neuron_count` - Count of currently firing neurons
- `active_neuron_count` - Count of active neurons
- `average_firing_rate` - Average firing rate across all neurons
- `excitation_inhibition_ratio` - E/I balance ratio
- `total_spike_count` - Total spike count
- `pending_spike_event_count` - Count of pending spike events
- `developmental_stage` - Current developmental stage

#### Config
Configuration management for neural simulation parameters.

**Key Methods:**
- `has(key)` - Check if a configuration key exists
- `get(key)` - Get configuration value (type-specific variants available)
- `set(key, value)` - Set configuration value
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries

**Type-specific Get Methods:**
- `getInt(key)` - Get as int
- `getFloat(key)` - Get as float
- `getBool(key)` - Get as bool
- `getString(key)` - Get as string
- `getList(key)` - Get as list

#### AgentBrain
Interface between NLM brain and world environment.

**Key Methods:**
- `initialize(world)` - Initialize with world environment
- `processSensoryInput(percept)` - Process sensory percept and inject into brain
- `decodeMotorCommand()` - Decode brain motor activity into motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep)` - Update development system
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableStructuralPlasticity(bool)` - Enable/disable structural plasticity
- `enableDevelopment(bool)` - Enable/disable development
- `enableCuriosity(bool)` - Enable/disable curiosity-driven exploration

**Properties:**
- `curiosity_level` - Current curiosity level
- `novelty_level` - Current novelty level
- `prediction_error` - Current prediction error
- `neuromodulation_level` - Current neuromodulation level
- `developmental_stage` - Current developmental stage

#### SimpleWorld
2D environment for NLM simulation.

**Key Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world to initial state
- `setAgentStart(x, y)` - Set agent's starting position
- `update(timestep)` - Update world physics for one timestep
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add an object to the world
- `removeObject(x, y)` - Remove object at given position
- `isValidPosition(x, y)` - Check if position is valid

**Properties:**
- `width` - World width in units
- `height` - World height in units
- `vision_width` - Agent vision width
- `vision_height` - Agent vision height
- `agent_x` - Agent's x position
- `agent_y` - Agent's y position
- `agent_orientation` - Agent's orientation in radians

### Sensory Input Classes

#### Vision
Represents visual input from the environment.

**Key Properties:**
- `width` - Image width
- `height` - Image height
- `channels` - Number of color channels (typically 3 for RGB)
- `data` - Array of pixel values (0-1 range)

#### Audio
Represents audio input with sample rate and amplitude data.

**Key Properties:**
- `sample_rate` - Sample rate in Hz
- `num_samples` - Number of samples
- `data` - Array of amplitude values (-1 to 1 range)

#### InternalSignals
Represents internal brain state signals such as hunger, thirst, energy levels.

**Key Properties:**
- `signals` - Array of internal signal values

### Action Classes

#### Action
Represents a motor command or behavioral action.

**Key Properties:**
- `type` - Action type (enum)
- `parameters` - Action parameters as array of floats
- `name` - Human-readable action name

**Conversions:**
- `to_dict()` - Convert to Python dictionary
- `from_dict(dict)` - Create from Python dictionary

#### WorldObject
Represents an object in the simulated world.

**Key Properties:**
- `x` - X position
- `y` - Y position
- `type` - Object type (Resource, Hazard, Wall, etc.)
- `value` - Object value (for resources, energy, etc.)
- `radius` - Object radius
- `active` - Whether object is active

**Methods:**
- `distance_to(x, y)` - Calculate distance to given position
- `is_within_range(x, y, range)` - Check if within range of position

### Enumerations

#### NeuronType
- `Excitatory` - Increases firing probability
- `Inhibitory` - Decreases firing probability
- `Modulatory` - Modulates other neurons' behavior
- `Sensory` - Processes sensory input
- `Motor` - Generates motor output
- `Internal` - For internal computations

#### DevelopmentalStage
- `Initial` - Initial developmental state
- `CriticalPeriod` - Enhanced plasticity period
- `Maturation` - Neural maturation phase
- `Adult` - Adult stable state
- `Aging` - Age-related changes

#### ActionType
Comprehensive action types including movement, interaction, consumption, etc.

#### WorldObjectType
- `Empty` - Empty space
- `Resource` - Consumable resource
- `Hazard` - Harmful object
- `Wall` - Immovable barrier
- `Marker` - Location marker

## Advanced Features

### Configuration Management

Create custom configurations for specific simulation scenarios:

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.05)
config.set("brain.v_thresh", -45.0)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Memory Management

Access and monitor memory systems:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get memory system access
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

print(f"Working memory active patterns: {working_memory.getActivePatternCount()}")
print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
```

### Plasticity Control

Control learning mechanisms:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access plasticity systems
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural = brain.getStructuralPlasticity()

print(f"STDP learning rate: {stdp.getLearningRate()}")
print(f"Hebbian enabled: {hebbian.isEnabled()}")
print(f"Structural plasticity enabled: {structural.isEnabled()}")
```

### Neuromodulation

Monitor and control neuromodulators:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Get neuromodulation levels
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
neuromod_level = agent.getNeuromodulationLevel()

print(f"Curiosity: {curiosity:.3f}")
print(f"Novelty: {novelty:.3f}")
print(f"Prediction Error: {prediction_error:.3f}")
print(f"Neuromodulation Level: {neuromod_level:.3f}")
```

## Helper Functions

### Quick Functions

The module provides several helper functions for common operations:

```python
import pynlm

# Create default config with custom parameters
config = pynlm.createDefaultConfig(
    neuron_count=2000,
    connection_probability=0.05
)

# Run complete episode
pynlm.run_simulation(brain, world, agent, 1000)
```

### Documentation Functions

Access built-in documentation:

```python
import pynlm

# Get basic documentation
doc = pynlm.getDocumentation()
print(doc["brain"])

# Get examples
examples = pynlm.getExamples()
print(f"Number of examples: {len(examples)}")

# Get quick start guide
quickstart = pynlm.getQuickStart()
print(quickstart["basic_usage"])
```

## Error Handling

### Common Exceptions

The module defines several custom exceptions:

- `pynlm.RuntimeError` - General runtime errors
- `pynlm.InvalidArgument` - Invalid argument errors

### Error Handling Example

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Attempt invalid operation
    invalid_value = config.getInt("nonexistent_key")
    
except pynlm.InvalidArgument as e:
    print(f"Invalid argument: {e}")
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
```

## Performance Tips

### Simulation Optimization

1. **Reuse Configurations**: Create configuration objects once and reuse them
2. **Batch Operations**: Use vector operations when possible
3. **Memory Management**: Monitor memory usage during long simulations
4. **Checkpointing**: Use checkpointing for long-running simulations

### Memory Monitoring

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

start_time = time.time()
start_memory = brain.getTotalNeuronCount()

for step in range(10000):
    brain.step(step)
    
    if step % 1000 == 0:
        current_memory = brain.getTotalNeuronCount()
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

## Troubleshooting

### Common Issues

1. **Import Error: No module named 'pynlm'**
   - Ensure the package is properly installed
   - Try: `pip install --force-reinstall .`

2. **Segmentation Fault**
   - Ensure `initialize()` is called before `step()`
   - Check that `reset()` is called before re-running simulation
   - Verify all required dependencies are installed

3. **Configuration Errors**
   - Check that configuration keys are spelled correctly
   - Use `config.has(key)` to check existence before accessing
   - Ensure configuration values are of correct type

4. **World Simulation Issues**
   - Verify that `world.update()` is being called
   - Check that agent is properly initialized with world
   - Ensure valid positions for actions

### Debugging Tips

```python
import pynlm

# Create minimal test setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Test basic functionality
brain.step(0)
print(f"Step completed. Firing neurons: {brain.getFiringNeuronCount()}")

# Test configuration
config.set("test.value", 42)
if config.has("test.value"):
    value = config.getInt("test.value")
    print(f"Configuration value: {value}")
```

## Migration Guide

### From Version 0.1.0 to Enhanced 0.1.0

**New Features Added:**
- Enhanced error handling with custom exceptions
- Pythonic property access and convenience methods
- Improved documentation and examples
- Helper functions for common operations
- Dictionary conversion methods
- Enhanced configuration management

**Breaking Changes:**
- None - This is a feature enhancement release

**New API Highlights:**
- `Brain.to_dict()` and `AgentBrain.to_dict()` for state inspection
- `Config.getInt()`, `Config.getFloat()`, etc. for type-safe access
- `SensoryPercept.to_dict()` and `SensoryPercept.from_dict()`
- `Action.to_dict()` and `Action.from_dict()`
- `run_simulation()` helper function
- Enhanced enumerations with descriptions
- Better string representations for IDs

## Further Reading

For more advanced usage and technical details:

1. **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** - System architecture overview
2. **[SCIENCE.md](docs/SCIENCE.md)** - Scientific background and principles
3. **[EXPERIMENTS.md](docs/EXPERIMENTS.md)** - Experiment descriptions and examples
4. **[LIMITATIONS.md](docs/LIMITATIONS.md)** - Scientific limitations and considerations
5. **[PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md)** - Phase 6 integration audit

## Support

For issues and questions:
- Check the documentation above
- Review example code snippets
- Consult the GitHub repository for updates
- Report bugs through appropriate channels

Enjoy using NLM to explore neural computation and artificial intelligence!