# NLM Python API Reference

**Neural Learning Machine - Python Bindings**

This document provides comprehensive API documentation for the NLM Python bindings, including all bug fixes and improvements implemented in Phase 6.

## Overview

The NLM Python bindings (`pynlm`) provide access to the experimental NLM brain simulation system. The bindings are automatically generated using pybind11 and provide a Python interface to the C++ neural network simulation.

### Key Features

- **Brain Simulation**: Create and simulate virtual brains with LIF neurons
- **Agent-World Interaction**: Connect brains to environments with sensory input and motor output
- **Neuromodulation**: Implement dopamine, curiosity, novelty, and reward systems
- **Development**: Brain development through stages (Initial, CriticalPeriod, Maturation, Adult, Aging)
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Performance**: Optimized with SIMD, memory pools, and parallel processing

### Package Structure

```python
import pynlm

# Core components
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Motor commands
from enum import IntEnum
for cmd in pynlm.MotorCommand:
    print(cmd.name)  # MoveForward, MoveBackward, TurnLeft, etc.
```

## Configuration System

### pynlm.createDefaultConfig()

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.0)
```

**Parameters:**

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

### Configuration Methods

```python
# Query configuration
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Get all keys
keys = config.getKeys()

# Save/Load configuration
config.saveToFile("config.json")
config.loadFromFile("config.json")
```

## Brain Class

### Creation and Initialization

```python
# Create brain with default config
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create brain with custom config
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()
```

### Brain Methods

#### Core Simulation

```python
# Run simulation
brain.step(step_number)          # Step with step number
brain.step(step_number, time)    # Step with timestamp
brain.reset()                     # Reset brain state

# State persistence
brain.save("checkpoint.bin")      # Save brain state
brain.load("checkpoint.bin")      # Load brain state
```

#### Neural Information

```python
# Neuron statistics
brain.getTotalNeuronCount()      # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing neurons
brain.getAverageFiringRate()      # Average firing rate (Hz)
brain.getExcitationInhibitionRatio()  # E/I balance

# Development
brain.getDevelopmentalStage()   # Current stage (string or enum)
brain.getDevelopmentalAge()     # Age in simulation time
brain.getPlasticityModifier()    # Current plasticity factor
```

#### Sensory and Motor

```python
# Sensory processing
brain.receiveSensoryInput(percept)  # Inject sensory data

# Motor output
brain.produceAction()              # Get motor action from neurons
```

#### Regions and Populations

```python
# Region management
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# Population access
for population in region.getPopulations():
    neuron_type = population.getNeuronType()
    for neuron in population.getNeurons():
        # Process neuron
        pass
```

#### Advanced Configuration

```python
# Configure neural groups
brain.configureMotorGroups(8)  # 8 motor commands
brain.configureSensoryGroups({
    'vision': 256,
    'touch': 8,
    'internal': 4,
    'proprioception': 6
})
```

## AgentBrain Class

### Creation and Initialization

```python
# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

### AgentBrain Methods

#### Core Processing

```python
# Sensory processing
agent.processSensoryInput(percept)  # Inject percept into brain

# Motor decoding
agent.decodeMotorCommand()         # Decode action from neural activity
```

#### Neuromodulation

```python
# Apply neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Get neuromodulation levels
agent.getNeuromodulationLevel()    # Overall neuromodulation
agent.getCuriosityLevel()         # Curiosity (0.0-1.0)
agent.getNoveltyLevel()           # Novelty (0.0-1.0)
agent.getPredictionError()        # Prediction error signal
```

#### Development

```python
# Development control
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Enable/disable systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check system status
if agent.isRewardModulationEnabled():
    pass
```

#### Utility Methods

```python
# Input/output sizes
agent.getSensoryInputSize()       # Expected sensory input size
agent.getMotorOutputSize()        # Expected motor output size

# Brain access
agent.getBrain()                 # Get underlying brain object

# Reset
agent.reset()                     # Reset all neuromodulation state
```

## SimpleWorld Class

### Creation and Configuration

```python
# Create world
world = pynlm.createSimpleWorld()

# Configure dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(10.0, 10.0)
```

### World Methods

#### Control

```python
# Simulation control
world.reset()                     # Reset to initial state
world.update(timestep)            # Update simulation

# Agent control
world.applyMotorCommand(action, time)  # Apply motor action
```

#### Sensory Access

```python
# Get sensory percept
percept = world.getSensoryPercept()

# Percept components
vision = percept.getVision()          # Vision data (2D grid)
touch = percept.getTouch()            # Touch/proximity sensors
internal = percept.getInternal()      # Homeostatic signals
proprioception = percept.getProprioception()  # Body position signals
audio = percept.getAudio()            # Audio-like signals (if enabled)

# Utility
all_signals = percept.getAllSignals()  # All signals concatenated
timestamp = percept.getTimestamp()
```

#### State Access

```python
# Agent state
body = world.getAgentBody()
position = body.getPosition()
rotation = body.getRotation()

# Environment
world.isValidPosition(x, y)        # Check if position is valid
world.getWidth(), world.getHeight()  # World dimensions
world.getSimulationTime()         # Current simulation time
world.getMaxEnergy()              # Maximum energy
world.setMaxEnergy(100.0)         # Set maximum energy
```

#### World Objects

```python
# Object management
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Object types
pynlm.WorldObjectType.Empty      # Empty space
pynlm.WorldObjectType.Resource   # Resource object
pynlm.WorldObjectType.Hazard     # Hazard object
pynlm.WorldObjectType.Wall       # Wall object
pynlm.WorldObjectType.Marker     # Marker object
```

## MotorCommand Enum

```python
from enum import IntEnum

# Motor commands (value = output neuron index)
MotorCommand.MoveForward = 0      # Move forward
MotorCommand.MoveBackward = 1     # Move backward
MotorCommand.TurnLeft = 2          # Turn left
MotorCommand.TurnRight = 3         # Turn right
MotorCommand.LookLeft = 4          # NEW: Pan sensor left (bug fix)
MotorCommand.LookRight = 5         # NEW: Pan sensor right (bug fix)
MotorCommand.Interact = 6          # Interact with object
MotorCommand.Wait = 7              # Stay in place
```

## DevelopmentalStage Enum

```python
# Brain development stages
DevelopmentalStage.Initial        # Initial development
DevelopmentalStage.CriticalPeriod  # Critical learning period
DevelopmentalStage.Maturation     # Maturation phase
DevelopmentalStage.Adult          # Adult stage
DevelopmentalStage.Aging          # Aging phase
```

## NeuronType Enum

```python
# Neuron types
NeuronType.Excitatory   # Excitatory neurons
NeuronType.Inhibitory   # Inhibitory neurons
NeuronType.Sensory       # Sensory neurons
NeuronType.Motor         # Motor neurons
NeuronType.Modulatory     # Neuromodulatory neurons
NeuronType.Internal      # Internal state neurons
```

## SensoryPercept Class

### Creation and Configuration

```python
# Create percept
percept = pynlm.SensoryPercept()

# Configure components
percept.setVision(vision_data)           # Vision grid (2D flattened)
percept.setTouch(touch_data)             # Touch sensor data
percept.setInternal(internal_data)       # Internal state signals
percept.setProprioception(proprio_data)   # Proprioception data
percept.setAudio(audio_data)             # Audio data (if enabled)
percept.setTimestamp(timestamp)         # Simulation timestamp
```

### Dimension Getters

```python
# Get dimensions
percept.getVisionWidth()           # Vision grid width
percept.getVisionHeight()          # Vision grid height
percept.getTouchSize()             # Number of touch sensors
percept.getInternalSize()          # Number of internal signals
percept.getProprioceptionSize()    # Number of proprioception signals
percept.getAudioSize()             # Number of audio signals

# Get data
vision = percept.getVision()          # Vision data
# ... other getters
```

## WorldObject Class

### Creation

```python
# Create world object
obj = pynlm.WorldObject(
    x=10.0,
    y=20.0,
    type=pynlm.WorldObjectType.Resource,
    value=1.0
)
```

### Attributes

```python
# Object properties
obj.getPosition()          # (x, y) tuple
obj.getType()              # WorldObjectType enum
obj.getValue()             # Object value (e.g., energy, resource amount)
obj.getID()                # Unique object ID
obj.isActive()             # Whether object is active

# Set attributes
obj.setPosition(x, y)
obj.setValue(new_value)
```

## Configuration Types

### ConfigSource Enum

```python
# Configuration sources
ConfigSource.Default     # Default configuration
ConfigSource.User       # User configuration
ConfigSource.Explicit   # Explicitly set value
```

## Error Handling

### Common Exceptions

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # May raise exceptions for:
    # - Invalid configuration values
    # - Null pointer errors (now properly handled)
    # - Memory allocation failures
    # - Invalid state transitions
    
except Exception as e:
    print(f"Error: {e}")
    # Handle gracefully
```

### Error Codes

| Error Code | Description |
|------------|-------------|
| `CONFIG_ERROR` | Invalid configuration value |
| `MEMORY_ERROR` | Memory allocation failed |
| `NULL_POINTER_ERROR` | Attempt to use null pointer (now prevented) |
| `STATE_ERROR` | Invalid state transition |
| `PERCEPT_ERROR` | Invalid sensory percept data |

## Examples

See the `examples/` directory for comprehensive examples:

- `example_1_basic_usage.py` - Basic brain simulation
- `example_2_agent_world.py` - Complete agent-world interaction
- `example_3_learning_curiosity.py` - Curiosity-driven learning
- `example_4_development.py` - Development stages
- `example_5_memory_and_replay.py` - Memory systems
- `example_6_new_motor_commands.py` - New LookLeft/LookRight commands
- `example_7_performance_benchmark.py` - Performance benchmarks
- `example_8_error_handling.py` - Error handling
- `example_9_mixed_control.py` - Advanced mixed control
- `example_10_production_ready.py` - Production-ready agent

## Performance Considerations

### Memory Management

- All neural group vectors are properly initialized in constructor
- Destructor cleans up all neural group vectors (fixes memory leaks)
- Dynamic calculation of input/output sizes reduces memory waste

### Computational Efficiency

- Pre-allocation reduces vector reallocation
- Bulk operations for efficient memory access
- Optimized sensory processing with bounds checking

### Thread Safety

- Neural group access is not thread-safe
- Use synchronization when accessing shared resources

## Migration Guide

### From Older Versions

#### Motor Commands (NEW in Phase 6)

**Added:**
- `LookLeft` (4) - Pan sensor left
- `LookRight` (5) - Pan sensor right

**Use:** These new commands enable environmental exploration

#### Input Validation (NEW in Phase 6)

**Before:** Sparse error handling, potential crashes
**After:** Comprehensive validation, graceful error handling

#### Memory Management (FIXED in Phase 6)

**Before:** Potential memory leaks, uninitialized vectors
**After:** Proper constructor initialization, destructor cleanup

#### Performance Optimizations (NEW in Phase 6)

**Before:** Repeated vector reallocations
**After:** Pre-allocation, bulk operations

## License

MIT License

## Version Information

```python
import pynlm
print(pynlm.__version__)  # "0.1.0"
print(pynlm.__version_info__)  # (major, minor, patch)
```

## Copyright

© 2024-2026 NLM Project Authors

See LICENSE file for details.
