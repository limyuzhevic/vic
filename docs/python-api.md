# NLM Python API Documentation

## Overview

NLM (Neural Learning Machine) provides Python bindings via the `pynlm` package, which gives access to the full C++ neural simulation framework. The Python API is built using pybind11 and provides a high-level interface to brain simulation, learning, and interaction capabilities.

### Key Features

- **Simple brain creation and management**
- **Neural computation with learning**
- **World simulation for agent-environment interaction**
- **Complete control over simulation parameters**
- **Access to brain statistics and state**
- **Integrated neuromodulation and development systems**

## Installation

```bash
pip install pynlm
```

Or for development mode:
```bash
pip install -e .
```

## Basic Usage

### Import and Version

```python
import pynlm

print(f"NLM Python API version: {pynlm.__version__}")
```

### Creating a Brain

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize brain (required before use)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getTotalSynapseCount()} synapses")
```

## Core Classes

### Config Class

The `Config` class manages brain simulation parameters. It supports file-based configuration, command-line arguments, and programmatic modification.

#### Creation and Basic Operations

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Check if key exists
if config.has("brain.neuron_count"):
    print("Neuron count is configured")

# Get all keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Clear configuration
config.clear()

# Get summary string
print(config.summary())
```

#### Loading and Saving Configuration

```python
# Load from JSON file
config.loadFromFile("my_config.json")

# Save to JSON file  
config.saveToFile("my_config.json")

# Load from command line arguments (Python equivalent)
# Note: loadFromArgs is typically called from C++ main()
```

#### Accessing Configuration Values

```python
# Access configuration values using getOr() for defaults
neuron_count = config.getOr("brain.neuron_count", 1000)
simulation_timestep = config.getOr("simulation_timestep", 0.001)
plasticity_rate = config.getOr("plasticity.learning_rate", 0.01)

# The following methods are available on config objects:
# getOr<T>(key, default) - Get value with default
# has(key) - Check if key exists
# getKeys() - Get all configuration keys
# clear() - Clear all configuration
# summary() - Get configuration summary as string
```

#### Setting Configuration Values

```python
# Set simple values
config.set("brain.neuron_count", 1000)
config.set("simulation_timestep", 0.001)
config.set("plasticity.enable", True)

# Set nested values
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)
config.set("brain.v_reset", -75.0)
config.set("brain.tau_mem", 20.0)
config.set("brain.tau_ref", 2.0)

# Set plasticity parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.01)
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.001)
config.set("plasticity.structural.pruning_rate", 0.0001)

# Set neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
```

### Brain Class

The `Brain` class represents the central neural simulation engine. It manages neurons, synapses, plasticity, and the simulation loop.

#### Brain Creation and Initialization

```python
# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain (required before use)
brain.initialize()

# Check if initialization was successful
if not brain.initialize():
    print("Failed to initialize brain")
    return
```

#### Simulation Control

```python
# Reset brain to initial state
brain.reset()

# Save brain state to file
brain.save("checkpoint1.bin")

# Load brain state from file
brain.load("checkpoint1.bin")

# Run simulation loop
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Input and Output Processing

```python
# Inject current into specific neuron
brain.injectCurrent(neuron_id, current_value)

# Inject current into all neurons of a specific type
brain.injectCurrentToNeurons(neuron_type, current_value)

# Receive sensory input from world
brain.receiveSensoryInput(sensory_input)

# Produce motor action based on current brain state
action = brain.produceAction()
```

#### Region Management

```python
# Add a new neural region
region_id = brain.addRegion("cortex")

# Get region by ID
region = brain.getRegion(region_id)

# Get all region IDs
region_ids = brain.getRegionIds()

# Get all regions
regions = brain.getRegions()

# Get number of regions
region_count = brain.getRegionCount()
```

#### Statistics and Monitoring

```python
# Get basic statistics
total_neurons = brain.getTotalNeuronCount()
total_synapses = brain.getTotalSynapseCount()
active_neurons = brain.getActiveNeuronCount()
firing_neurons = brain.getFiringNeuronCount()
average_firing_rate = brain.getAverageFiringRate()
e_excitatory_inhibition_ratio = brain.getExcitationInhibitionRatio()
total_spikes = brain.getTotalSpikeCount()

# Get developmental stage
development_stage = brain.getDevelopmentalStage()

# Log brain status
brain.logStatus()

# Get configuration
config = brain.getConfig()
```

#### Time-based Simulation

```python
# Run simulation step with timestamp
brain.step(step_number, timestamp)

# Get configuration timestep for time management
dt = brain.getConfig().getOr("simulation_timestep", 0.001)
```

### AgentBrain Class

The `AgentBrain` class connects the NLM brain to the world, providing interface for sensory processing, motor decoding, and neuromodulation.

#### Creation and Initialization

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

#### Sensory Processing

```python
# Process sensory input from world
agent.processSensoryInput(percept)

# Get expected sensory input size
sensory_input_size = agent.getSensoryInputSize()
```

#### Motor Decoding

```python
# Decode brain activity into motor command
motor_cmd = agent.decodeMotorCommand()

# Get expected motor output size
motor_output_size = agent.getMotorOutputSize()
```

#### Neuromodulation

```python
# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Get neuromodulation level
neuromod_level = agent.getNeuromodulationLevel()

# Enable/disable neuromodulation systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### Development and Learning

```python
# Update development system
agent.updateDevelopment(timestep)

# Get current developmental stage
stage = agent.getDevelopmentalStage()

# Get curiosity level
curiosity = agent.getCuriosityLevel()

# Get novelty level
novelty = agent.getNoveltyLevel()

# Get prediction error
error = agent.getPredictionError()
```

#### Subsystem Status

```python
# Check if subsystems are enabled
is_reward_modulation_enabled = agent.isRewardModulationEnabled()
is_structural_plasticity_enabled = agent.isStructuralPlasticityEnabled()
is_development_enabled = agent.isDevelopmentEnabled()
is_curiosity_enabled = agent.isCuriosityEnabled()
```

### SimpleWorld Class

The `SimpleWorld` class provides a 2D simulation environment for agent interaction.

#### World Creation and Configuration

```python
# Create simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
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
```

#### World Control

```python
# Update world simulation
timestep = 0.1
world.update(timestep)

# Set random seed for reproducibility
world.setRandomSeed(42)
```

#### Agent Interaction

```python
# Apply motor command to world
world.applyMotorCommand(motor_cmd, current_time)

# Get sensory percept from world
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
```

#### World Object Management

```python
# Add world object (resource, hazard, etc.)
world_object = pynlm.WorldObject(
    x=5.0,
    y=5.0,
    type=pynlm.WorldObjectType.Resource,
    value=1.0,
    radius=0.5
)
world.addObject(world_object)

# Remove object
world.removeObject(x=5.0, y=5.0)

# Check if position is valid
is_valid = world.isValidPosition(x=5.0, y=5.0)
```

#### World Properties

```python
# Get world dimensions
width = world.getWidth()
height = world.getHeight()

# Get simulation time
time = world.getSimulationTime()

# Get world energy properties
max_energy = world.getMaxEnergy()
world.setMaxEnergy(100.0)

energy_decay_rate = world.getEnergyDecayRate()
world.setEnergyDecayRate(0.01)
```

### Helper Functions

#### Factory Functions

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Create simple world
world = pynlm.createSimpleWorld()

# Create agent brain
agent = pynlm.createAgentBrain(brain)
```

### Global Constants

```python
# Invalid identifiers (use to check for errors)
invalid_neuron_id = pynlm.INVALID_NEURON_ID
invalid_synapse_id = pynlm.INVALID_SYNAPSE_ID
invalid_region_id = pynlm.INVALID_REGION_ID
invalid_population_id = pynlm.INVALID_POPULATION_ID
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Custom

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker

# Motor commands
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait

# Synapse types
pynlm.SynapseType.Excitatory
pynlm.SynapseType.Inhibitory
pynlm.SynapseType.Modulatory
pynlm.SynapseType.Electrical
pynlm.SynapseType.GapJunction

# Firing states
pynlm.FiringState.Resting
pynlm.FiringState.Active
pynlm.FiringState.Refractory
pynlm.FiringState.Inhibited
```

## Class Summaries

### Config

- **Purpose**: Manages brain simulation configuration
- **Key Methods**: `loadFromFile()`, `saveToFile()`, `set()`, `getOr()`, `has()`, `getKeys()`, `clear()`, `summary()`
- **Typical Usage**: Create, configure, load/save parameters

### Brain

- **Purpose**: Central neural simulation engine
- **Key Methods**: `initialize()`, `reset()`, `step()`, `injectCurrent()`, `receiveSensoryInput()`, `produceAction()`, statistics getters
- **Typical Usage**: Main simulation loop, learning experiments

### AgentBrain

- **Purpose**: Interface connecting brain to world
- **Key Methods**: `initialize()`, `processSensoryInput()`, `decodeMotorCommand()`, neuromodulation methods
- **Typical Usage**: Agent-environment interaction, complete agent control

### SimpleWorld

- **Purpose**: 2D simulation environment
- **Key Methods**: `configure()`, `update()`, `applyMotorCommand()`, `getSensoryPercept()`
- **Typical Usage**: Environment simulation, agent testing

## Error Handling

### Common Errors

1. **Brain not initialized**: Always call `initialize()` before `step()`

```python
brain = pynlm.createBrain(config)
brain.initialize()  # Required!
for step in range(100):
    brain.step(step)  # This will fail if not initialized
```

2. **Import errors**: Ensure pynlm is properly installed

```python
import pynlm  # May fail if not installed
```

3. **Configuration errors**: Handle missing keys gracefully

```python
# Safe access with defaults
value = config.getOr("missing.key", "default_value")
```

### Exception Types

```python
# pynlm.register_exception() registers exceptions
# Common exceptions:
# - RuntimeError: General runtime errors
# - ValueError: Invalid parameter values
# - std::exception: C++ exceptions wrapped by pybind11
```

## Performance Tips

### For Large Simulations

```python
# Use batch operations when possible
brain.reset()  # Resets entire brain
brain.save("checkpoint.bin")  # Saves entire state
brain.load("checkpoint.bin")  # Loads entire state

# Monitor memory usage
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### For Real-time Applications

```python
# Use smaller timesteps for smoother control
dt = config.getOr("simulation_timestep", 0.01)

# Pre-allocate sensory buffers if possible
percept = world.getSensoryPercept()

# Minimize object creation in loops
for step in range(num_steps):
    brain.step(step)
    # Reuse variables when possible
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, time)
```

### Memory Management

```python
# Allow garbage collection to work normally
# No need for explicit memory management in Python

# But avoid circular references
agent = pynlm.createAgentBrain(brain)
# brain still exists, but this doesn't prevent garbage collection

# When done, let variables go out of scope
# agent and brain will be garbage collected
```

## Best Practices

### Code Organization

```python
# Organization by function

def run_simple_simulation(steps=100):
    """Simple simulation without world"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return brain.getTotalSpikeCount()

def run_agent_episode(steps=500):
    """Complete agent with world interaction"""
    # Create all components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run episode
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    return brain, world, agent
```

### Configuration Management

```python
# Centralized configuration
class BrainConfig:
    @staticmethod
    def create_learning_config():
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("plasticity.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        return config
    
    @staticmethod
    def create_control_config():
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        config.set("plasticity.enable", False)  # No learning
        config.set("neuromod.dopamine.scale", 0.5)
        config.set("neuromod.curiosity.enable", False)
        return config

# Usage
learning_config = BrainConfig.create_learning_config()
control_config = BrainConfig.create_control_config()
```

### Error Handling and Validation

```python
# Validate configuration before use
def validate_config(config):
    if not config.has("brain.neuron_count"):
        print("Warning: Neuron count not configured")
        config.set("brain.neuron_count", 100)
    
    if not config.has("simulation_timestep"):
        print("Warning: Timestep not configured")
        config.set("simulation_timestep", 0.01)

# Use try-except for robust error handling
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
except Exception as e:
    print(f"Error during simulation: {e}")
    # Handle error, maybe try recovery or graceful shutdown
```

## Migration Guide

### From C++ to Python

If you're coming from C++, here are key differences:

1. **Memory Management**: Python handles garbage collection automatically
2. **Type Safety**: Python is dynamically typed, C++ is statically typed
3. **Exception Handling**: Python uses try-catch blocks
4. **Configuration**: Python uses JSON files, C++ uses Config objects
5. **Simulation Loop**: Python is event-driven, C++ uses explicit step() calls

### From Other Brain Frameworks

If you're familiar with other brain simulation frameworks:

**PyTorch/Brain frameworks**:
- NLM uses explicit spiking neurons, not gradient-based learning
- NLM supports STDP and Hebbian learning, not backpropagation
- NLM includes world interaction, not just neural networks

**Neuronal modeling tools**:
- NLM implements real LIF neuron dynamics
- NLM includes plasticity mechanisms
- NLM adds cognitive capabilities like working memory and prediction

## Conclusion

The NLM Python API provides comprehensive access to the neural learning machine framework. It balances ease of use with powerful capabilities for research and development in computational brain systems.

Key takeaways:

1. **Always initialize** the brain before use
2. **Understand configuration** for parameter control
3. **Use the agent interface** for world interaction
4. **Monitor statistics** for simulation control
5. **Handle errors gracefully** for robust applications
6. **Follow best practices** for code organization and performance

The NLM Python API enables researchers, developers, and students to explore neural computation, learning, and behavior in a flexible, powerful framework.