# Python API Reference

This document provides the complete Python API reference for NLM (Neural Learning Machine). The Python bindings are generated using pybind11 and provide access to all NLM functionality from Python.

## Overview

The Python API (`pynlm`) is a wrapper around the C++ NLM framework that provides:

- **Brain simulation**: Create and run neural simulations
- **World interaction**: Connect brains to simulated environments
- **Agent interface**: Bridge between brains and worlds
- **Advanced features**: DNN layers, memory pools, error handling
- **Performance monitoring**: Statistics and metrics

## Quick Start

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

## Main Classes

### Brain Class

The `Brain` class represents the central neural simulation system. It manages neurons, synapses, and all neural computation.

#### Creation and Initialization
```python
import pynlm

# Create with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Create brain directly from config
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation Control
```python
# Run a simulation step
brain.step(step_number)           # Basic step
brain.step(step_number, time)     # Step with timestamp
brain.reset()                     # Reset to initial state

# Save and load brain state
brain.save("checkpoint.bin")      # Save to file
brain.load("checkpoint.bin")      # Load from file
```

#### Neural Input/Output
```python
# Inject sensory input
brain.receiveSensoryInput(sensory_input)

# Produce motor action
action = brain.produceAction()

# Inject current into specific neurons
brain.injectCurrent(neuron_id, current_value)
brain.injectCurrentToNeurons(neuron_type, current_value)
```

#### Brain Regions
```python
# Add a new region
region_id = brain.addRegion("cortex")

# Get regions by ID
region = brain.getRegion(region_id)

# Get all regions
regions = brain.getRegions()

# Get region count
region_count = brain.getRegionCount()

# Get all region IDs
region_ids = brain.getRegionIds()
```

#### Statistics and Metrics
```python
# Neural statistics
brain.getTotalNeuronCount()       # Total neurons in brain
brain.getTotalSynapseCount()      # Total synapses
brain.getActiveNeuronCount()      # Currently active neurons
brain.getFiringNeuronCount()      # Currently firing neurons
brain.getAverageFiringRate()      # Average firing rate (Hz)
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()        # Total spikes generated

# Developmental information
brain.getDevelopmentalStage()    # Current developmental stage
brain.setDevelopmentalStage(stage)  # Set developmental stage
```

#### Configuration Access
```python
# Get brain configuration
config = brain.getConfig()
```

#### Logging and Diagnostics
```python
# Log brain status
brain.logStatus()
```

### AgentBrain Class

The `AgentBrain` class provides the interface between a NLM brain and an external world/environment.

#### Creation and Initialization
```python
import pynlm

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

#### Sensory Processing
```python
# Process sensory input from world
agent.processSensoryInput(percept)
```

#### Motor Decoding
```python
# Get action from brain motor activity
action = agent.decodeMotorCommand()
```

#### Neuromodulation
```python
# Apply reward-based modulation
agent.applyRewardModulation(reward, predicted_reward)
```

#### Development
```python
# Update development system
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()    # Current developmental stage
```

#### Subsystem Control
```python
# Enable/disable various subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
agent.isRewardModulationEnabled()
agent.isStructuralPlasticityEnabled()
agent.isDevelopmentEnabled()
agent.isCuriosityEnabled()
```

#### Neuromodulation Levels
```python
agent.getCuriosityLevel()         # Curiosity-driven exploration (0.0-1.0)
agent.getNoveltyLevel()           # Novelty detection level (0.0-1.0)
agent.getPredictionError()        # Prediction error signal (0.0-1.0)
agent.getNeuromodulationLevel()   # Overall neuromodulation level (0.0-1.0)
```

#### World Object Access
```python
# Get agent position and state
agent.getBrain().getConfig()     # Get underlying brain
```

### SimpleWorld Class

The `SimpleWorld` class represents a simple 2D environment for NLM simulations.

#### Creation and Configuration
```python
import pynlm

# Create a world
world = pynlm.createSimpleWorld()

# Configure world parameters
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

#### World Control
```python
# Reset world to initial state
world.reset()

# Update world by timestep
world.update(timestep)

# Set random seed for reproducibility
world.setRandomSeed(42)
```

#### Agent Interaction
```python
# Set agent starting position
world.setAgentStart(x, y)

# Apply motor command from agent
world.applyMotorCommand(motor_cmd, current_time)
```

#### State Access
```python
# Get sensory percept (what agent sees)
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
```

#### World Objects
```python
# Add a world object (resource, hazard, wall, etc.)
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))

# Remove object by position
world.removeObject(x, y)

# Check if position is valid (not a wall)
world.isValidPosition(x, y)
```

#### World Properties
```python
world.getWidth()                 # World width
world.getHeight()                # World height
world.getSimulationTime()        # Current simulation time
world.getMaxEnergy()             # Maximum agent energy
world.setMaxEnergy(100.0)        # Set maximum energy
world.getEnergyDecayRate()       # Energy decay rate
world.setEnergyDecayRate(0.01)   # Set energy decay rate
```

### Config Class

The `Config` class manages NLM configuration parameters and settings.

#### Creation
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()
```

#### Configuration Management
```python
# Check if configuration key exists
if config.has("brain.neuron_count"):
    print("Neuron count is set")

# Get all configuration keys
keys = config.getKeys()

# Set configuration value
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)
```

#### Configuration File I/O
```python
# Load configuration from file
config.loadFromFile("config.json")

# Save configuration to file
config.saveToFile("config.json")
```

### Action Class

The `Action` class represents motor actions that agents can take in the world.

#### Creation
```python
import pynlm

# Create empty action
action = pynlm.Action()

# Create action with type
action = pynlm.Action(pynlm.ActionType.MOVE_FORWARD)

# Create action with type and parameters
action = pynlm.Action(
    pynlm.ActionType.TURN_LEFT,
    [0.5]  # Parameters
)
```

#### Action Properties
```python
# Get action type
action_type = action.getType()

# Set action type
action.setType(pynlm.ActionType.MOVE_FORWARD)

# Get action parameters
parameters = action.getParameters()

# Set action parameters
action.setParameters([0.5, 0.25])

# Get action name (human-readable)
action_name = action.getName()

# Clone action
action_clone = action.clone()
```

### WorldObject Class

The `WorldObject` class represents objects in the world (resources, hazards, walls, etc.).

#### Creation
```python
import pynlm

# Create empty world object
obj = pynlm.WorldObject()

# Create world object with position and type
obj = pynlm.WorldObject(
    x=10.0,
    y=10.0,
    pynlm.WorldObjectType.Resource,
    value=1.0,
    radius=0.5
)
```

#### World Object Properties
```python
# Position
obj.x = 10.0
obj.y = 10.0

# Type (Resource, Hazard, Wall, Water, Obstacle, etc.)
obj.type = pynlm.WorldObjectType.Resource

# Value (for resources)
obj.value = 1.0

# Radius (size)
obj.radius = 0.5

# Active state
obj.active = True
```

### ActionResult Class

The `ActionResult` class represents the result of an agent's action in the world.

#### Creation
```python
import pynlm

# Create empty action result
result = pynlm.ActionResult()

# Create with values
result = pynlm.ActionResult(
    reward=10.0,
    success=True,
    message="Successfully collected resource"
)
```

#### Action Result Properties
```python
# Reward received
result.reward = 10.0

# Success status
result.success = True

# Descriptive message
result.message = "Successfully collected resource"
```

### SensoryPercept Class

The `SensoryPercept` class represents sensory input received by agents from the world.

#### Creation
```python
import pynlm

# Create empty percept
percept = pynlm.SensoryPercept()
```

#### Sensory Signals
```python
# Vision (what agent sees)
vision = percept.getVision()
percept.setVision(vision)

# Touch (physical contact)
touch = percept.getTouch()
percept.setTouch(touch)

# Internal state (internal body sensors)
internal = percept.getInternal()
percept.setInternal(internal)

# Proprioception (body position/orientation)
proprioception = percept.getProprioception()
percept.setProprioception(proprioception)

# Audio (sound input)
audio = percept.getAudio()
percept.setAudio(audio)
```

#### Sensory Dimensions
```python
# Vision dimensions
vision_width = percept.getVisionWidth()
vision_height = percept.getVisionHeight()

# Timestamp
percept.getTimestamp()
percept.setTimestamp(timestamp)

# Get all signals (convenience method)
all_signals = percept.getAllSignals()
```

### AgentBody Class

The `AgentBody` class represents the physical state of an agent in the world.

#### Creation
```python
import pynlm

# Create empty agent body
body = pynlm.AgentBody()
```

#### Agent Body Properties
```python
# Position
body.x = 10.0
body.y = 10.0

# Orientation (angle in radians)
body.orientation = 0.0

# Velocity
body.velocityX = 0.0
body.velocityY = 0.0

# Angular velocity
body.angularVelocity = 0.0

# Energy
body.energy = 100.0
body.health = 1.0

# Age
body.age = 0.0

# State flags
body.isMoving = False
body.isTurning = False

# Last action time
body.lastActionTime = 0.0

# Reset agent to initial state
body.reset()
```

## Enumerations

### NeuronType
```python
import pynlm

# Neuron types
pynlm.NeuronType.Excitatory    # Excitatory neurons
pynlm.NeuronType.Inhibitory    # Inhibitory neurons
pynlm.NeuronType.Modulatory    # Neuromodulatory neurons
pynlm.NeuronType.Sensory       # Sensory neurons
pynlm.NeuronType.Motor         # Motor neurons
pynlm.NeuronType.Internal      # Internal neurons
```

### SynapseType
```python
# Synapse types
pynlm.SynapseType.Excitatory    # Excitatory synapses
pynlm.SynapseType.Inhibitory    # Inhibitory synapses
pynlm.SynapseType.Modulatory    # Modulatory synapses
pynlm.SynapseType.Electrical    # Electrical synapses
pynlm.SynapseType.GapJunction  # Gap junctions
```

### DevelopmentalStage
```python
# Developmental stages
pynlm.DevelopmentalStage.Initial      # Embryonic stage
pynlm.DevelopmentalStage.CriticalPeriod  # Critical learning period
pynlm.DevelopmentalStage.Maturation   # Maturation stage
pynlm.DevelopmentalStage.Adult        # Adult stage
pynlm.DevelopmentalStage.Aging        # Aging stage
```

### ActionType
```python
# Action types
pynlm.ActionType.MOVE_FORWARD      # Move forward
pynlm.ActionType.MOVE_BACKWARD     # Move backward
pynlm.ActionType.TURN_LEFT         # Turn left
pynlm.ActionType.TURN_RIGHT        # Turn right
pynlm.ActionType.STAY              # Stay in place
pynlm.ActionType.GRAB              # Grab object
pynlm.ActionType.RELEASE           # Release object
pynlm.ActionType.NONE              # No action
```

### WorldObjectType
```python
# World object types
pynlm.WorldObjectType.AGENT       # Agent position
pynlm.WorldObjectType.FOOD        # Food resource
pynlm.WorldObjectType.WALL        # Wall/hindrance
pynlm.WorldObjectType.WATER       # Water
pynlm.WorldObjectType.OBSTACLE     # Obstacle
```

## Error Handling

The Python API includes comprehensive error handling with custom exception types:

### Exception Types
```python
import pynlm

try:
    # Code that may raise exceptions
    brain.initialize()
    
except pynlm.NLMException as e:
    print(f"NLM Error: {e}")
    
except pynlm.SimulationError as e:
    print(f"Simulation Error: {e}")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration Error: {e}")
    
except pynlm.MemoryError as e:
    print(f"Memory Error: {e}")
    
except pynlm.InvalidOperationError as e:
    print(f"Invalid Operation Error: {e}")
```

### Error Handling Functions
```python
import pynlm

# Set custom error handler
def my_error_handler(error):
    print(f"Custom error handler: {error}")

pynlm.set_error_handler(my_error_handler)

# Get last error
error_msg = pynlm.get_last_error()

# Clear error
pynlm.clear_error()
```

## Advanced Python Features

### DNN Layers (Phase 6+ Features)
```python
import pynlm
import numpy as np

# Create DNN layer
dnn_layer = pynlm.DNNLayer(
    input_size=10,
    output_size=5,
    activation=pynlm.NeuronType.Excitatory,
    dropout_rate=0.1
)

# Forward pass
input_data = np.random.randn(10)
output = dnn_layer.forward(input_data)

# Set/get weights
weights = dnn_layer.get_weights()
dnn_layer.set_weights(weights)

# Set/get biases
biases = dnn_layer.get_biases()
dnn_layer.set_biases(biases)

# Training mode
dnn_layer.set_training_mode(True)
is_training = dnn_layer.is_training()

# Architecture summary
summary = dnn_layer.get_architecture_summary()
```

### Memory Pools (Phase 5+ Features)
```python
import pynlm

# Create memory pool (1MB capacity)
memory_pool = pynlm.MemoryPool(1024 * 1024)

# Allocate memory
ptr = memory_pool.allocate(1024)  # 1KB

# Deallocate memory
memory_pool.deallocate(ptr, 1024)

# Get memory statistics
used = memory_pool.get_used_memory()
free = memory_pool.get_free_memory()
full = memory_pool.is_full()
memory_pool.clear()

# Vector memory allocation
float_ptrs = memory_pool.allocate_vector(100)  # 100 floats
memory_pool.deallocate_vector(float_ptrs, 100)
```

### Performance Monitoring
```python
import pynlm

# Get comprehensive performance statistics
stats = pynlm.get_performance_stats(brain)

print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Excitement/Inhibition ratio: {stats['excitement_inhibition_ratio']:.3f}")

# Get memory pool usage
pool = pynlm.MemoryPool(1024 * 1024)
usage = pynlm.get_memory_usage(pool)

print(f"Total capacity: {usage['total_capacity'] / 1024 / 1024:.2f} MB")
print(f"Used memory: {usage['used_memory'] / 1024 / 1024:.2f} MB")
print(f"Free memory: {usage['free_memory'] / 1024 / 1024:.2f} MB")
print(f"Usage percentage: {usage['usage_percentage']:.1f}%")
```

### Advanced Configuration
```python
import pynlm

# Create brain with DNN configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrainWithDNN(
    config,
    dnn_input_size=20,
    dnn_hidden_size=10,
    dnn_num_layers=3
)

# Configure networks directly
pynlm.configure_dnn_network(brain, 20, 10, 3, "relu")
pynlm.configure_rnn_network(brain, 15, 8, 2, "gru")
pynlm.configure_cnn_network(brain, 3, 16, [3, 3], [2, 2])
```

## Module Attributes

The `pynlm` module includes several useful constants and attributes:

```python
import pynlm

# Invalid ID constants
pynlm.INVALID_NEURON_ID
pynlm.INVALID_SYNAPSE_ID
pynlm.INVALID_REGION_ID
pynlm.INVALID_POPULATION_ID

# Version
print(pynlm.__version__)
```

## Troubleshooting

### Common Issues

**Import Error: No module named 'pynlm'**
- Ensure the Python bindings are installed
- Run `pip install pynlm` or `pip install -e .`

**Segmentation Fault**
- Ensure `brain.initialize()` is called before `brain.step()`
- Check configuration values
- Verify world and agent initialization order

**Configuration Errors**
- Check that required configuration keys exist
- Use `config.has()` to verify key presence
- Use descriptive error messages in try-catch blocks

**Performance Issues**
- Monitor memory usage with `get_memory_usage()`
- Check neuron and synapse counts
- Profile with `get_performance_stats()`

## Best Practices

### Memory Management
```python
# Use memory pools for large allocations
memory_pool = pynlm.MemoryPool(100 * 1024 * 1024)  # 100MB

# Deallocate memory explicitly
ptr = memory_pool.allocate(size)
# ... use ptr ...
memory_pool.deallocate(ptr, size)
```

### Error Handling
```python
import pynlm

# Always handle errors in production code
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Your simulation code here
    
except pynlm.NLMException as e:
    print(f"Simulation failed: {e}")
    # Cleanup resources
    if 'brain' in locals():
        brain.reset()
```

### Configuration Management
```python
# Use meaningful configuration keys
config.set("brain.neuron_count", 1000)
config.set("brain.initial_weight_mean", 0.5)

# Load/save configurations for reproducibility
config.saveToFile("experiment_config.json")
```

### Simulation Control
```python
# Use consistent timestep
brain.initialize()
for step in range(num_steps):
    brain.step(step)
    if step % 100 == 0:
        # Log statistics periodically
        brain.logStatus()
```

## Future Enhancements

The Python API will continue to evolve with:

1. **Enhanced DNN Integration**: More flexible neural network configurations
2. **Advanced Memory Management**: Automatic memory pooling and cleanup
3. **Parallel Processing**: Multi-threaded simulation capabilities
4. **Distributed Computing**: Multi-machine scaling support
5. **Visualization Tools**: Built-in plotting and analysis functions
6. **Cloud Integration**: Easy deployment to cloud platforms

## API Evolution

The Python API follows semantic versioning. Breaking changes will be documented in release notes, and deprecation warnings will be provided for legacy functionality.

---

*For the latest API documentation, visit: https://nlm.readthedocs.io*
*Last updated: 2026-10-09*
*Documentation version: 0.1.0*