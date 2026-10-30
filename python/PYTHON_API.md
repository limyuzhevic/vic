# NLM Python API Reference

This document provides comprehensive reference for the NLM Python API,
including all classes, functions, and utilities available for neural
simulation development.

## Overview

The NLM Python package provides a complete interface to the NLM neural
simulation framework, combining the power of C++ performance with
Python's ease of use and extensibility.

## Module Structure

```
pynlm/
├── __init__.py           # Main package exports
├── core.py               # Core bindings and factory functions
├── enums.py              # Enumerations and utilities
├── types.py              # Type definitions and validation
├── constants.py          # Constants and configuration
├── visualization.py      # Visualization interfaces
├── checkpoint.py         # Checkpoint management
└── bindings.py           # Enhanced Python bindings
```

## Core Classes

### Config

Configuration management for NLM simulations.

**Methods:**

- `loadFromFile(filepath: str) -> bool`
  Load configuration from JSON file

- `loadFromArgs(argc: int, argv: List[str]) -> bool`
  Load configuration from command line arguments

- `saveToFile(filepath: str) -> bool`
  Save configuration to JSON file

- `has(key: str) -> bool`
  Check if configuration key exists

- `getKeys() -> List[str]`
  Get all configuration keys

- `clear() -> None`
  Clear all configuration entries

- `summary() -> str`
  Get configuration summary string

**Factory Methods:**

- `createDefaultConfig() -> Config`
  Create default configuration

**Example:**
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.learning_rate", 0.01)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")
```

### Brain

Central neural simulation brain class with complete neural computation.

**Methods:**

- `initialize() -> None`
  Initialize the brain with configuration

- `step(step: int, time: Optional[float] = None) -> None`
  Perform a simulation step

- `receiveSensoryInput(input) -> None`
  Inject sensory input into the brain

- `injectCurrent(neuron: NeuronId, current: float) -> None`
  Inject current directly into a specific neuron

- `injectCurrentToNeurons(neuronType: NeuronType, current: float) -> None`
  Inject current into all neurons of a specific type

- `produceAction() -> Action`
  Produce motor action based on neural activity

- `reset() -> None`
  Reset brain state

- `save(filepath: str) -> bool`
  Save brain state to file

- `load(filepath: str) -> bool`
  Load brain state from file

- `addRegion(name: str = "") -> RegionId`
  Add a new neural region

- `getRegion(id: RegionId) -> NeuralRegion`
  Get a region by ID

- `getRegionCount() -> int`
  Get the number of regions

- `getRegionIds() -> List[RegionId]`
  Get all region IDs

- `getRegions() -> List[NeuralRegion]`
  Get all regions

- `getTotalNeuronCount() -> int`
  Get total neuron count across all regions

- `getTotalSynapseCount() -> int`
  Get total synapse count across all regions

- `getActiveNeuronCount() -> int`
  Get count of active neurons

- `getFiringNeuronCount() -> int`
  Get count of currently firing neurons

- `getAverageFiringRate() -> float`
  Get average firing rate across all neurons

- `getExcitationInhibitionRatio() -> float`
  Get excitation/inhibition balance ratio

- `getTotalSpikeCount() -> int`
  Get total spike count

- `getDevelopmentalStage() -> DevelopmentalStage`
  Get current developmental stage

- `setDevelopmentalStage(stage: DevelopmentalStage) -> None`
  Set developmental stage

- `getConfig() -> Config`
  Get the configuration

- `logStatus() -> None`
  Log brain status

**Properties:**

- `total_neurons` - Read-only access to total neuron count
- `total_synapses` - Read-only access to total synapse count
- `firing_neurons` - Read-only access to firing neuron count
- `total_spikes` - Read-only access to total spike count
- `average_firing_rate` - Read-only access to average firing rate
- `excitation_inhibition_ratio` - Read-only access to E/I ratio

**Example:**
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get statistics
print(f"Neurons: {brain.total_neurons}")
print(f"Total spikes: {brain.total_spikes}")
print(f"Firing neurons: {brain.firing_neurons}")
```

### AgentBrain

Agent brain interface connecting NLM brain to world.

**Methods:**

- `initialize(world: SimpleWorld) -> None`
  Initialize with world

- `getSensoryInputSize() -> size_t`
  Get expected sensory input size

- `getMotorOutputSize() -> size_t`
  Get expected motor output size

- `processSensoryInput(percept: SensoryPercept) -> None`
  Process sensory percept and inject into brain

- `decodeMotorCommand() -> MotorCommand`
  Decode brain motor activity into motor command

- `applyRewardModulation(reward: float, predictedReward: float) -> None`
  Apply reward-based neuromodulation

- `updateDevelopment(timestep: float) -> None`
  Update development system

- `getDevelopmentalStage() -> DevelopmentalStage`
  Get current developmental stage

- `getNeuromodulationLevel() -> float`
  Get current neuromodulation level

- `getCuriosityLevel() -> float`
  Get curiosity level

- `getNoveltyLevel() -> float`
  Get novelty level

- `getPredictionError() -> float`
  Get prediction error

- `reset() -> None`
  Reset agent for new episode

- `getBrain() -> Brain`
  Get the underlying brain

- `enableRewardModulation(enable: bool) -> None`
  Enable or disable reward modulation

- `enableStructuralPlasticity(enable: bool) -> None`
  Enable or disable structural plasticity

- `enableDevelopment(enable: bool) -> None`
  Enable or disable development

- `enableCuriosity(enable: bool) -> None`
  Enable or disable curiosity

- `isRewardModulationEnabled() -> bool`
  Check if reward modulation is enabled

- `isStructuralPlasticityEnabled() -> bool`
  Check if structural plasticity is enabled

- `isDevelopmentEnabled() -> bool`
  Check if development is enabled

- `isCuriosityEnabled() -> bool`
  Check if curiosity is enabled

**Properties:**

- `curiosity_level` - Read-only access to curiosity level
- `novelty_level` - Read-only access to novelty level
- `prediction_error` - Read-only access to prediction error
- `neuromodulation_level` - Read-only access to neuromodulation level
- `developmental_stage` - Read-only access to developmental stage

**Example:**
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print(f"Agent initialized: curiosity={agent.curiosity_level:.3f}")
```

### SimpleWorld

Simple 2D world for NLM simulation.

**Methods:**

- `configure(width: float, height: float, visionWidth: int, visionHeight: int) -> None`
  Configure world dimensions

- `reset() -> None`
  Reset world to initial state

- `setAgentStart(x: float, y: float) -> None`
  Set agent starting position

- `update(timestep: float) -> None`
  Update world state (call each simulation step)

- `applyMotorCommand(cmd: MotorCommand, currentTime: float) -> ActionResult`
  Apply motor command to agent

- `getSensoryPercept() -> SensoryPercept`
  Get current sensory percept for the agent

- `getAgentBody() -> AgentBody`
  Get agent body state

- `addObject(obj: WorldObject) -> None`
  Add object to world

- `removeObject(x: float, y: float) -> None`
  Remove object at position

- `isValidPosition(x: float, y: float) -> bool`
  Check if position is valid (not wall)

- `getObjectAt(x: float, y: float) -> WorldObject`
  Get object at position (if any)

- `getWidth() -> float`
  Get world width

- `getHeight() -> float`
  Get world height

- `getMaxEnergy() -> float`
  Get maximum energy

- `setMaxEnergy(e: float) -> None`
  Set maximum energy

- `getEnergyDecayRate() -> float`
  Get energy decay rate

- `setEnergyDecayRate(r: float) -> None`
  Set energy decay rate

- `getSimulationTime() -> float`
  Get current simulation time

- `setRandomSeed(seed: uint64_t) -> None`
  Set random seed for reproducibility

- `getRandomSeed() -> uint64_t`
  Get random seed

**Properties:**

- `width` - Read-only access to world width
- `height` - Read-only access to world height
- `simulation_time` - Read-only access to simulation time
- `max_energy` - Read-only access to maximum energy
- `energy_decay_rate` - Read-only access to energy decay rate

**Example:**
```python
import pynlm

# Create simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Get world statistics
print(f"World size: {world.width}x{world.height}")
print(f"Max energy: {world.max_energy}")
```

### Action

Action representation for motor output.

**Methods:**

- `getType() -> ActionType`
  Get action type

- `setType(type: ActionType) -> None`
  Set action type

- `getParameters() -> List[float]`
  Get action parameters

- `setParameters(params: List[float]) -> None`
  Set action parameters

- `getName() -> str`
  Get action name

- `clone() -> Action`
  Clone action

**Example:**
```python
import pynlm

# Create action
action = pynlm.Action(pynlm.ActionType.MoveForward)

print(f"Action type: {action.getType()}")
print(f"Action name: {action.getName()}")

# Clone action
action2 = action.clone()
```

### WorldObject

World object representation.

**Properties:**

- `x` - Object x-coordinate
- `y` - Object y-coordinate
- `radius` - Object radius
- `type` - Object type
- `value` - Object value (reward or damage)
- `active` - Object activity state

**Example:**
```python
import pynlm

# Create world object
obj = pynlm.WorldObject(
    x=5.0,
    y=5.0,
    obj_type=pynlm.WorldObjectType.Resource,
    value=10.0,
    radius=0.5
)

print(f"Object at ({obj.x}, {obj.y}) is a {obj.type}")
```

### ActionResult

Action result from world.

**Properties:**

- `reward` - Reward received from action
- `success` - Whether action was successful
- `message` - Optional message about action result

**Example:**
```python
import pynlm

# Create action result
result = pynlm.ActionResult(
    reward=5.0,
    success=True,
    message="Successfully collected resource"
)

print(f"Action reward: {result.reward}")
print(f"Action success: {result.success}")
```

## Enumerations

### NeuronType

Neuron type enumeration:

- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons
- `Modulatory` - Modulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal neurons

### ActionType

Action type enumeration:

- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait
- `Custom` - Custom action

### WorldObjectType

World object type enumeration:

- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazardous object
- `Wall` - Wall object
- `Marker` - Marker object

### Other Enumerations

See `enums.py` for complete enumeration of:
- `SynapseType`
- `DevelopmentalStage`
- `FiringState`
- `MotorCommand`

## Identifier Classes

### NeuronId, SynapseId, RegionId, PopulationId

Strongly-typed identifiers for neurons, synapses, regions, and populations.

**Common Methods:**

- `value` - Get identifier value
- `index() -> int` - Get index
- `__eq__()` - Equality comparison
- `__ne__()` - Inequality comparison
- `__hash__() -> int` - Hash for dictionaries
- `__repr__() -> str` - String representation
- `__str__() -> str` - Human-readable representation

**Example:**
```python
import pynlm

# Create identifiers
neuron_id = pynlm.NeuronId(42)
synapse_id = pynlm.SynapseId(100)
region_id = pynlm.RegionId(1)
population_id = pynlm.PopulationId(2)

print(f"Neuron ID: {neuron_id}")
print(f"Synapse ID: {synapse_id}")
print(f"Region ID: {region_id}")
print(f"Population ID: {population_id}")
```

### Invalid ID Sentinels

- `INVALID_NEURON_ID` - Invalid neuron ID
- `INVALID_SYNAPSE_ID` - Invalid synapse ID
- `INVALID_REGION_ID` - Invalid region ID
- `INVALID_POPULATION_ID` - Invalid population ID

## Type Aliases

### Core Types

- `SimulationStep` - Simulation step counter
- `Timestamp` - Simulation time in seconds
- `TimestepDuration` - Time step duration
- `NeuronIndex` - Neuron index in population
- `SynapseIndex` - Synapse index
- `PopulationIndex` - Population index in region
- `RegionIndex` - Region index
- `SynapticWeight` - Synaptic weight type
- `MembranePotential` - Membrane potential type
- `FiringRate` - Firing rate type
- `Delay` - Connection delay in simulation steps

### Configuration Type

- `ConfigValue` - Union of all configuration value types

## Factory Functions

### Core Factories

- `createDefaultConfig() -> Config`
  Create default configuration

- `createBrain(config: Config) -> Brain`
  Create brain with configuration

- `createAgentBrain(brain: Brain) -> AgentBrain`
  Create agent brain interface

- `createSimpleWorld() -> SimpleWorld`
  Create simple world

- `createAgentBody() -> AgentBody`
  Create agent body

### Convenience Functions

- `run_agent_simulation(num_steps: int = 1000, **kwargs) -> None`
  Run complete agent simulation

- `get_system_info() -> Dict[str, Any]`
  Get system information

## Enhanced Factory Functions

### Advanced Factories

- `create_enhanced_config(**kwargs) -> Config`
  Create enhanced configuration with validation

- `create_enhanced_brain(config: Optional[Config] = None, **kwargs) -> Brain`
  Create enhanced brain with common configurations

- `run_enhanced_simulation(num_steps: int = 1000, config: Optional[Config] = None, **kwargs) -> Dict[str, Any]`
  Run enhanced simulation with common patterns

## Exception Classes

### Core Exceptions

- `NLMError` - Base exception for NLM errors
- `ConfigurationError` - Configuration-related errors
- `InitializationError` - Initialization-related errors
- `SimulationError` - Simulation-related errors
- `ValidationError` - Validation-related errors
- `FileError` - File-related errors

**Example:**
```python
import pynlm

try:
    config.loadFromFile("nonexistent.json")
except pynlm.FileError as e:
    print(f"File error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
```

## Visualization Module

See `visualization.py` for comprehensive visualization capabilities:

- `NLMVisualizer` - Main visualization interface
- `MatplotlibBackend` - Matplotlib-based backend
- `PlotlyBackend` - Plotly-based backend
- `create_visualizer()` - Create visualizer instance
- `visualize_brain()` - Quick brain visualization
- `visualize_simulation()` - Simulation visualization

## Checkpoint Management Module

See `checkpoint.py` for comprehensive checkpoint management:

- `CheckpointManager` - Checkpoint manager class
- `CheckpointInfo` - Checkpoint information class
- `CheckpointStatus` - Checkpoint status enumeration
- `create_checkpoint_manager()` - Create checkpoint manager
- `save_brain_checkpoint()` - Quick checkpoint save
- `load_brain_checkpoint()` - Quick checkpoint load

## Submodules

### `enums.py`

Enumeration utilities with Pythonic features:
- Enhanced enum classes with factory methods
- Convenience functions for common operations
- String conversion utilities
- Type validation functions

### `types.py`

Type definitions and validation:
- Enhanced type classes with validation
- Type checking utilities
- Collection type definitions
- Function type definitions

### `constants.py`

Constants and configuration:
- Compression levels
- Checkpoint sections
- Simulation constants
- Error codes
- Configuration prefixes

## Usage Examples

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Example 2: Complete Agent Simulation

```python
import pynlm

# Create all components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### Example 3: Enhanced Configuration

```python
import pynlm

# Create enhanced configuration with common settings
config = pynlm.create_enhanced_config(
    brain_neuron_count=2000,
    brain_connection_probability=0.05,
    plasticity_learning_rate=0.001,
    neuromod_curiosity_enable=True,
    development_enable=True
)

brain = pynlm.create_enhanced_brain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Example 4: Visualization

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create visualizer
visualizer = pynlm.create_visualizer()

# Run simulation and visualize
for i in range(100):
    brain.step(i)
    visualizer.render_network(brain)
    visualizer.render_activity(brain)

# Save plots
visualizer.save_plots("brain_simulation.png")
```

### Example 5: Checkpoint Management

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for i in range(100):
    brain.step(i)

# Save checkpoint
checkpoint_info = pynlm.save_brain_checkpoint(brain, name="my_simulation")
if checkpoint_info:
    print(f"Checkpoint saved: {checkpoint_info.name}")
    print(f"Checkpoint size: {checkpoint_info.get_formatted_size()}")

# Load checkpoint
loaded_brain = pynlm.load_brain_checkpoint("my_simulation")
if loaded_brain:
    print("Checkpoint loaded successfully")
```

### Example 6: Error Handling

```python
import pynlm

# Example 1: File error handling
try:
    config = pynlm.createDefaultConfig()
    config.loadFromFile("nonexistent.json")
except pynlm.FileError as e:
    print(f"File error: {e}")
    # Handle error appropriately

# Example 2: Configuration validation
try:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", "not_a_number")  # Should raise ValueError
except (pynlm.ConfigurationError, ValueError) as e:
    print(f"Configuration validation error: {e}")

# Example 3: Custom error handler
def custom_error_handler(error: Exception) -> None:
    print(f"Custom error handler: {error}")

brain = pynlm.createDefaultConfig()
brain.enable_error_handler(custom_error_handler)
```

## Migration Guide

### From C++ to Python

If you're migrating from C++ to Python, keep in mind:

1. **Memory Management**: Python's garbage collection handles memory management automatically
2. **Type Safety**: Python provides runtime type checking for better safety
3. **Error Handling**: Python exceptions provide better error reporting
4. **Syntax**: Python syntax is generally more concise than C++

### Key Differences

- **Smart Pointers**: Use Python references instead of C++ smart pointers
- **STL vs Python Collections**: Use Python lists, dicts, and sets
- **Threading**: Python provides built-in threading support
- **File I/O**: Use Python's built-in file I/O operations

## Performance Considerations

### For Large Simulations

1. **Use Context Managers**: Leverage `simulation_context` for proper resource management
2. **Batch Operations**: Perform multiple operations in batches when possible
3. **Memory Management**: Let Python's garbage collector handle memory
4. **Checkpointing**: Use checkpointing for long-running simulations

### For Real-time Applications

1. **Asynchronous Operations**: Use async/await for real-time applications
2. **Batch Processing**: Process data in small batches
3. **Optimization**: Profile and optimize your code for performance
4. **Hardware Acceleration**: Leverage hardware acceleration where available

## Testing

### Python Tests

Run Python tests using:
```bash
pytest tests/
```

### C++ Tests

Run C++ tests using:
```bash
cd build && ctest --output-on-failure
```

## Installation

### From Source

```bash
# Clone repository
cd /path/to/nlm

# Build and install Python bindings
pip install .
```

### From PyPI

```bash
pip install pynlm
```

## Troubleshooting

### Common Issues

1. **Import Error**: Check that all dependencies are installed
2. **Module Not Found**: Check Python path and module structure
3. **Memory Error**: Check available memory and optimize your code
4. **Performance Issues**: Profile your code and optimize bottlenecks

### Getting Help

- **Documentation**: See `HOW_TO_USE.md` for comprehensive documentation
- **Examples**: See `examples/` for additional examples
- **Issues**: Report issues on GitHub
- **Community**: Join the NLM community forums

## Version Information

- **Package Version**: 0.1.0
- **C++ Core Version**: 0.1.0
- **Python API Version**: 0.1.0
- **Last Updated**: October 30, 2026

## License

MIT License - See `LICENSE` file for details.

---

This document provides comprehensive reference for the NLM Python API.
For more detailed information, refer to the specific module documentation
and examples in the repository.