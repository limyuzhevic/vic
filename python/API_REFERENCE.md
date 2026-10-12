# NLM Python Bindings API Reference

## Overview

The NLM Python bindings provide a high-level interface to the neural computation system. The bindings enable researchers and developers to create and simulate neural brains, implement agents, and interact with simulated environments using Python.

## Core Classes

### `pynlm.Config`

The `Config` class manages brain configuration parameters using a hierarchical key-value structure.

#### Class Methods

- **`createDefaultConfig()`** - Create a default configuration with standard parameters
- **`Config()`** - Create an empty configuration object

#### Instance Methods

- **`has(key: str) -> bool`** - Check if configuration contains a specific key
- **`set(key: str, value: Any)`** - Set a configuration parameter
- **`get(key: str) -> Any`** - Get a configuration parameter value
- **`getKeys() -> List[str]`** - Get all configuration keys
- **`loadFromFile(filename: str)`** - Load configuration from JSON file
- **`saveToFile(filename: str)`** - Save configuration to JSON file

#### Example

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.enabled", True)

# Check if key exists
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

### `pynlm.Brain`

The `Brain` class represents a neural brain that performs spiking neural computation using Leaky Integrate-and-Fire (LIF) neurons.

#### Constructor

- **`Brain(config: Config)`** - Create a brain with specified configuration

#### Initialization Methods

- **`initialize()`** - Initialize the brain and create all neurons and synapses
- **`reset()`** - Reset brain to initial state

#### Simulation Methods

- **`step(step_number: int)`** - Perform one simulation step
- **`step(step_number: int, time: float)`** - Perform simulation step with timestamp

#### Input/Output Methods

- **`receiveSensoryInput(sensory_input)`** - Inject sensory data into the brain
- **`produceAction() -> Action`** - Get motor action from brain activity

#### Analysis Methods

- **`getTotalNeuronCount() -> int`** - Get total number of neurons
- **`getTotalSynapseCount() -> int`** - Get total number of synapses
- **`getFiringNeuronCount() -> int`** - Get currently firing neurons
- **`getAverageFiringRate() -> float`** - Get average firing rate
- **`getExcitationInhibitionRatio() -> float`** - Get E/I balance
- **`getTotalSpikeCount() -> int`** - Get total spike count
- **`getBrainSize() -> int`** - Get brain memory footprint

#### State Management

- **`save(filename: str)`** - Save brain state to binary file
- **`load(filename: str)`** - Load brain state from binary file

#### Development Methods

- **`getDevelopmentalStage() -> DevelopmentalStage`** - Get current developmental stage
- **`setDevelopmentalStage(stage: DevelopmentalStage)`** - Set developmental stage

#### Example

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)

# Create and initialize brain
brain = pynlm.Brain(config)
brain.initialize()

# Simulation loop
for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")

# Save brain state
brain.save("brain_state.bin")
```

### `pynlm.AgentBrain`

The `AgentBrain` class provides the interface between the neural brain and the environment, implementing sensory processing and motor decoding.

#### Constructor

- **`AgentBrain(brain: Brain)`** - Create agent-brain interface

#### Initialization Methods

- **`initialize(world: SimpleWorld)`** - Initialize agent with world interface

#### Sensory Processing

- **`processSensoryInput(percept)`** - Process sensory input from world

#### Motor Control

- **`decodeMotorCommand() -> Action`** - Decode motor command from brain activity

#### Neuromodulation

- **`enableRewardModulation(enable: bool)`** - Enable/disable reward modulation
- **`applyRewardModulation(reward: float, predicted_reward: float)`** - Apply reward signal

#### Development

- **`enableDevelopment(enable: bool)`** - Enable/disable developmental processes
- **`updateDevelopment(timestep: float)`** - Update developmental state

#### Curiosity and Novelty

- **`enableCuriosity(enable: bool)`** - Enable curiosity-driven exploration
- **`enableNovelty(enable: bool)`** - Enable novelty detection
- **`getCuriosityLevel() -> float`** - Get current curiosity level
- **`getNoveltyLevel() -> float`** - Get current novelty level

#### Statistics

- **`getNeuromodulationLevel() -> float`** - Get current neuromodulation level
- **`getPredictionError() -> float`** - Get prediction error

#### Example

```python
import pynlm

# Setup brain
config = pynlm.createDefaultConfig()
brain = pynlm.Brain(config)
brain.initialize()

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create and initialize agent
agent = pynlm.AgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Step brain
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress
    if step % 20 == 0:
        print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
```

### `pynlm.SimpleWorld`

The `SimpleWorld` class simulates a 2D environment with objects, providing sensory input and action application.

#### Constructor

- **`createSimpleWorld()`** - Create a simple world instance

#### Configuration

- **`configure(width: int, height: int, visionWidth: int, visionHeight: int)`** - Configure world dimensions and sensory capabilities

#### World Control

- **`reset()`** - Reset world to initial state
- **`update(timestep: float)`** - Update world state
- **`setRandomSeed(seed: int)`** - Set random seed for reproducibility

#### Agent Interaction

- **`setAgentStart(x: float, y: float)`** - Set agent starting position
- **`applyMotorCommand(action: Action, current_time: float)`** - Apply motor command to world

#### State Access

- **`getSensoryPercept() -> SensoryPercept`** - Get current sensory percept
- **`getAgentBody() -> AgentBody`** - Get agent body information

#### World Objects

- **`addObject(object: WorldObject)`** - Add object to world
- **`removeObject(x: float, y: float)`** - Remove object at position
- **`isValidPosition(x: float, y: float) -> bool`** - Check if position is valid

#### Properties

- **`getWidth() -> int`** - Get world width
- **`getHeight() -> int`** - Get world height
- **`getSimulationTime() -> float`** - Get current simulation time
- **`getMaxEnergy() -> float`** - Get maximum energy
- **`setMaxEnergy(value: float)`** - Set maximum energy

#### Example

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setMaxEnergy(100.0)

# Set agent start
world.setAgentStart(10.0, 10.0)

# Reset world
world.reset()

# Run simulation
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    print(f"Vision available: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
    
    # Apply motor command (in complete example)
    # action = agent.decodeMotorCommand()
    # world.applyMotorCommand(action, world.getSimulationTime())
```

### Factory Functions

The Python bindings provide factory functions for creating common NLM components:

#### Core Factory Functions

- **`pynlm.createDefaultConfig()`** - Create default configuration
- **`pynlm.createBrain(config: Config)`** - Create brain with configuration
- **`pynlm.createSimpleWorld()`** - Create simple world
- **`pynlm.createAgentBrain(brain: Brain)`** - Create agent-brain interface

#### Example

```python
import pynlm

# Create all components using factory functions
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

### Enumerations

#### Neuron Types

- **`pynlm.NeuronType.Excitatory`** - Excitatory neuron
- **`pynlm.NeuronType.Inhibitory`** - Inhibitory neuron
- **`pynlm.NeuronType.Modulatory`** - Modulatory neuron
- **`pynlm.NeuronType.Sensory`** - Sensory neuron
- **`pynlm.NeuronType.Motor`** - Motor neuron
- **`pynlm.NeuronType.Internal`** - Internal neuron

#### Synapse Types

- **`pynlm.SynapseType.Excitatory`** - Excitatory synapse
- **`pynlm.SynapseType.Inhibitory`** - Inhibitory synapse
- **`pynlm.SynapseType.Modulatory`** - Modulatory synapse
- **`pynlm.SynapseType.Electrical`** - Electrical synapse
- **`pynlm.SynapseType.GapJunction`** - Gap junction synapse

#### Developmental Stages

- **`pynlm.DevelopmentalStage.Initial`** - Initial stage (high plasticity)
- **`pynlm.DevelopmentalStage.CriticalPeriod`** - Critical period (sensitive learning)
- **`pynlm.DevelopmentalStage.Maturation`** - Maturation (stabilization)
- **`pynlm.DevelopmentalStage.Adult`** - Adult (low plasticity)
- **`pynlm.DevelopmentalStage.Aging`** - Aging (declining function)

#### Firing States

- **`pynlm.FiringState.Resting`** - Not firing
- **`pynlm.FiringState.Depolarizing`** - Building up to threshold
- **`pynlm.FiringState.Firing`** - Currently firing
- **`pynlm.FiringState.Repolarizing`** - Resetting after firing

#### Action Types

- **`pynlm.ActionType.MoveForward`** - Move forward
- **`pynlm.ActionType.MoveBackward`** - Move backward
- **`pynlm.ActionType.TurnLeft`** - Turn left
- **`pynlm.ActionType.TurnRight`** - Turn right
- **`pynlm.ActionType.LookUp`** - Look up
- **`pynlm.ActionType.LookDown`** - Look down
- **`pynlm.ActionType.Attack`** - Attack
- **`pynlm.ActionType.Defend`** - Defend
- **`pynlm.ActionType.Eat`** - Eat
- **`pynlm.ActionType.Drink`** - Drink

#### World Object Types

- **`pynlm.WorldObjectType.Empty`** - Empty space
- **`pynlm.WorldObjectType.Resource`** - Resource (food, water, etc.)
- **`pynlm.WorldObjectType.Obstacle`** - Obstacle
- **`pynlm.WorldObjectType.Predator`** - Predator
- **`pynlm.WorldObjectType.Prey`** - Prey
- **`pynlm.WorldObjectType.Shelter`** - Shelter
- **`pynlm.WorldObjectType.Water`** - Water source
- **`pynlm.WorldObjectType.Food`** - Food source

### Exception Classes

#### `pynlm.ConfigError`

- **Description:** Raised when there's an error in configuration
- **Example:** `raise ConfigError("Invalid configuration parameter")`

#### `pynlm.BrainError`

- **Description:** Raised when there's an error in brain operations
- **Example:** `raise BrainError("Brain not initialized")`

#### `pynlm.WorldError`

- **Description:** Raised when there's an error in world operations
- **Example:** `raise WorldError("Invalid world configuration")`

### Utility Functions

#### `pynlm.print_version()`

Print version information about the NLM Python bindings.

```python
pynlm.print_version()
```

#### Additional Utility Functions

The bindings may include other utility functions for common operations.
