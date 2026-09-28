# NLM Python API Reference

Complete reference documentation for the NLM Python bindings and enhanced Python interface.

## Module Overview

The NLM Python API provides two interfaces:

1. **Raw Pybind11 Bindings** (`nlm` module): Direct C++ binding access
2. **Enhanced Python Interface** (`nlm` module): User-friendly wrapper with Pythonic features

Both interfaces are available in the `nlm` module after installation.

## Core Classes

### 1. Config

**Pythonic Interface:** `nlm.Config`
**Raw Bindings:** `nlm.Config`

```python
# Create configuration
config = nlm.Config()
config = nlm.create_default_config()  # Convenience function

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Get configuration values
neuron_count = config.get("brain.neuron_count", 1000)
firing_rate = config.get_float("brain.firing_rate", 0.1)
enabled = config.get_bool("plasticity.stdp.enable", False)

# Configuration properties
print(config.keys)  # Get all configuration keys
print(config.summary)  # Get configuration summary
print(config.has("brain.neuron_count"))  # Check if key exists

# File operations
config.save("config.json")  # Save to file
config.load("config.json")  # Load from file
config.clear()  # Clear all configuration
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `set(key, value, source)` | Set configuration value | key: str, value: Any, source: str | Config |
| `get(key, default)` | Get configuration value | key: str, default: Any | Any |
| `get_int(key, default)` | Get configuration as integer | key: str, default: int | int |
| `get_float(key, default)` | Get configuration as float | key: str, default: float | float |
| `get_bool(key, default)` | Get configuration as boolean | key: str, default: bool | bool |
| `save(filepath)` | Save configuration to file | filepath: str | None |
| `load(filepath)` | Load configuration from file | filepath: str | Config |
| `clear()` | Clear all configuration entries | None | Config |
| `summary()` | Get configuration summary | None | str |

#### Properties

| Property | Description | Type |
|----------|-------------|------|
| `keys` | Get all configuration keys | List[str] |
| `has(key)` | Check if configuration key exists | bool |
| `summary` | Get configuration summary | str |

### 2. Brain

**Pythonic Interface:** `nlm.Brain`
**Raw Bindings:** `nlm.Brain`

```python
# Create brain with configuration
config = nlm.Config()
config.set("brain.neuron_count", 1000)
brain = nlm.Brain(config)
brain = nlm.createBrain(config)  # Convenience function

# Initialize brain
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    brain.step(step, step * 0.1)  # With timestamp

# Brain properties
print(brain.total_neurons)  # Total neuron count
print(brain.total_synapses)  # Total synapse count
print(brain.firing_neurons)  # Currently firing neurons
print(brain.average_firing_rate)  # Average firing rate
print(brain.excitation_inhibition_ratio)  # E/I balance
print(brain.total_spikes)  # Total spike count
print(brain.active_neurons)  # Active neuron count
print(brain.developmental_stage)  # Current developmental stage
print(brain.initialized)  # Whether brain is initialized

# Region management
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
print(brain.get_region_count())  # Number of regions
print(brain.get_region_ids())  # All region IDs

# I/O operations
brain.save("brain_state.bin")  # Save brain state
brain.load("brain_state.bin")  # Load brain state
brain.logStatus()  # Log current status

# Input/output methods
brain.receiveSensoryInput(sensory_input)  # Inject sensory input
action = brain.produceAction()  # Produce motor action
brain.injectCurrent(neuron, current)  # Inject current into neuron
brain.injectCurrentToNeurons(neuron_type, current)  # Inject to neuron type
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize brain | None | Brain |
| `step(step)` | Perform simulation step | step: int | Brain |
| `step(step, time)` | Perform step with timestamp | step: int, time: float | Brain |
| `reset()` | Reset brain state | None | Brain |
| `receiveSensoryInput(input)` | Inject sensory input | input: SensoryInput | Brain |
| `produceAction()` | Produce motor action | None | Action |
| `injectCurrent(neuron, current)` | Inject current to neuron | neuron: Any, current: float | Brain |
| `injectCurrentToNeurons(type, current)` | Inject to neuron type | type: NeuronType, current: float | Brain |
| `addRegion(name)` | Add neural region | name: str | RegionId |
| `getRegion(id)` | Get region by ID | id: RegionId | Region |
| `getRegionCount()` | Get region count | None | int |
| `getRegionIds()` | Get all region IDs | None | List[RegionId] |
| `getRegions()` | Get all regions | None | List[Region] |
| `getTotalNeuronCount()` | Get total neuron count | None | int |
| `getTotalSynapseCount()` | Get total synapse count | None | int |
| `getActiveNeuronCount()` | Get active neuron count | None | int |
| `getFiringNeuronCount()` | Get firing neuron count | None | int |
| `getAverageFiringRate()` | Get average firing rate | None | float |
| `getExcitationInhibitionRatio()` | Get E/I ratio | None | float |
| `getTotalSpikeCount()` | Get total spike count | None | int |
| `getDevelopmentalStage()` | Get developmental stage | None | DevelopmentalStage |
| `setDevelopmentalStage(stage)` | Set developmental stage | stage: DevelopmentalStage | None |
| `getConfig()` | Get configuration | None | Config |
| `logStatus()` | Log brain status | None | None |
| `save(filepath)` | Save brain state | filepath: str | None |
| `load(filepath)` | Load brain state | filepath: str | Brain |

#### Properties

| Property | Description | Type |
|----------|-------------|------|
| `initialized` | Whether brain is initialized | bool |
| `total_neurons` | Total neuron count | int |
| `total_synapses` | Total synapse count | int |
| `firing_neurons` | Currently firing neurons | int |
| `average_firing_rate` | Average firing rate | float |
| `excitation_inhibition_ratio` | E/I balance ratio | float |
| `total_spikes` | Total spike count | int |
| `active_neurons` | Active neuron count | int |
| `developmental_stage` | Current developmental stage | str |

### 3. SimpleWorld

**Pythonic Interface:** `nlm.SimpleWorld`
**Raw Bindings:** `nlm.SimpleWorld`

```python
# Create and configure world
world = nlm.SimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)

# Set agent start position
world.setAgentStart(15.0, 15.0)

# Reset world
world.reset()

# Update world
world.update(timestep=0.1)

# Apply motor command
result = world.applyMotorCommand(action, world.simulation_time)

# Get sensory input
percept = world.getSensoryPercept()

# Get agent body
body = world.getAgentBody()

# World management
world.addObject(world_object)  # Add world object
world.removeObject(x, y)  # Remove object
world.isValidPosition(x, y)  # Check position validity

# Configuration properties
print(world.width)  # World width
print(world.height)  # World height
print(world.vision_width)  # Vision width
print(world.vision_height)  # Vision height
print(world.max_energy)  # Maximum energy
print(world.energy_decay_rate)  # Energy decay rate
print(world.simulation_time)  # Current simulation time
print(world.random_seed)  # Random seed
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `configure(width, height, vision_width, vision_height)` | Configure world | width: int, height: int, vision_width: int, vision_height: int | SimpleWorld |
| `reset()` | Reset world | None | SimpleWorld |
| `setAgentStart(x, y)` | Set agent start position | x: float, y: float | SimpleWorld |
| `update(timestep)` | Update world | timestep: float | SimpleWorld |
| `applyMotorCommand(cmd, current_time)` | Apply motor command | cmd: MotorCommand, current_time: float | ActionResult |
| `getSensoryPercept()` | Get sensory percept | None | SensoryPercept |
| `getAgentBody()` | Get agent body | None | AgentBody |
| `addObject(obj)` | Add object to world | obj: WorldObject | SimpleWorld |
| `removeObject(x, y)` | Remove object from world | x: float, y: float | SimpleWorld |
| `isValidPosition(x, y)` | Check position validity | x: float, y: float | bool |

#### Properties

| Property | Description | Type |
|----------|-------------|------|
| `width` | World width | int |
| `height` | World height | int |
| `vision_width` | Vision width | int |
| `vision_height` | Vision height | int |
| `max_energy` | Maximum energy | float |
| `energy_decay_rate` | Energy decay rate | float |
| `simulation_time` | Current simulation time | float |
| `random_seed` | Random seed | int |
| `agent_body` | Agent body (convenience property) | AgentBody |
| `percept` | Current sensory percept (convenience property) | SensoryPercept |

### 4. AgentBrain

**Pythonic Interface:** `nlm.AgentBrain`
**Raw Bindings:** `nlm.AgentBrain`

```python
# Create agent brain interface
brain = nlm.Brain(config)
agent = nlm.AgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Process sensory input
agent.processSensoryInput(percept)

# Get action
action = agent.decodeMotorCommand()

# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(timestep)

# Get neuromodulation levels
print(agent.getCuriosityLevel())  # Curiosity level
print(agent.getNoveltyLevel())  # Novelty level
print(agent.getPredictionError())  # Prediction error
print(agent.getNeuromodulationLevel())  # Neuromodulation level
print(agent.getDevelopmentalStage())  # Developmental stage

# Check subsystem status
print(agent.isRewardModulationEnabled())  # Reward modulation enabled
print(agent.isCuriosityEnabled())  # Curiosity enabled
print(agent.isDevelopmentEnabled())  # Development enabled
print(agent.isStructuralPlasticityEnabled())  # Structural plasticity enabled

# Reset agent
agent.reset()
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize(world)` | Initialize with world | world: SimpleWorld | AgentBrain |
| `processSensoryInput(percept)` | Process sensory input | percept: SensoryPercept | AgentBrain |
| `decodeMotorCommand()` | Decode motor command | None | MotorCommand |
| `applyRewardModulation(reward, predicted_reward)` | Apply reward modulation | reward: float, predicted_reward: float | AgentBrain |
| `updateDevelopment(timestep)` | Update development | timestep: float | AgentBrain |
| `getDevelopmentalStage()` | Get developmental stage | None | str |
| `getNeuromodulationLevel()` | Get neuromodulation level | None | float |
| `getCuriosityLevel()` | Get curiosity level | None | float |
| `getNoveltyLevel()` | Get novelty level | None | float |
| `getPredictionError()` | Get prediction error | None | float |
| `reset()` | Reset agent | None | AgentBrain |
| `enableRewardModulation(enable)` | Enable reward modulation | enable: bool | AgentBrain |
| `enableStructuralPlasticity(enable)` | Enable structural plasticity | enable: bool | AgentBrain |
| `enableDevelopment(enable)` | Enable development | enable: bool | AgentBrain |
| `enableCuriosity(enable)` | Enable curiosity | enable: bool | AgentBrain |

#### Properties

| Property | Description | Type |
|----------|-------------|------|
| `is_reward_modulation_enabled` | Reward modulation enabled | bool |
| `is_structural_plasticity_enabled` | Structural plasticity enabled | bool |
| `is_development_enabled` | Development enabled | bool |
| `is_curiosity_enabled` | Curiosity enabled | bool |
| `brain` | Get underlying brain | Brain |

### 5. SensoryInput

**Pythonic Interface:** `nlm.SensoryInput`
**Raw Bindings:** `nlm.SensoryInput`

```python
# Create sensory input
vision = nlm.Vision(width=10, height=10, channels=3)
audio = nlm.Audio(sampleRate=44100, numSamples=1024)
internal = nlm.InternalSignals()

# Set data
vision.setData([0.1, 0.2, 0.3, 0.4, 0.5])
audio.setData([0.01, 0.02, 0.03])
internal.addSignal(0.7)

# Get data
print(vision.getType())  # Input type
print(vision.getData())  # Raw data as list
print(vision.getDimensions())  # Dimensions
print(vision.getTimestamp())  # Timestamp

# Vision properties
print(vision.width)  # Vision width
print(vision.height)  # Vision height
print(vision.channels)  # Number of channels

# Audio properties
print(audio.getSampleRate())  # Sample rate
print(audio.getNumSamples())  # Number of samples
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Vision(width, height, channels)` | Create vision input | width: int, height: int, channels: int | Vision |
| `Audio(sampleRate, numSamples)` | Create audio input | sampleRate: int, numSamples: int | Audio |
| `InternalSignals()` | Create internal signals | None | InternalSignals |
| `getType()` | Get input type | None | NeuronType |
| `getData()` | Get raw data | None | List[float] |
| `getDimensions()` | Get dimensions | None | int |
| `getTimestamp()` | Get timestamp | None | float |
| `setTimestamp(timestamp)` | Set timestamp | timestamp: float | None |

### 6. Action

**Pythonic Interface:** `nlm.Action`
**Raw Bindings:** `nlm.Action`

```python
# Create action
move_forward = nlm.Action(nlm.ActionType.MoveForward)
custom = nlm.Action(nlm.ActionType.Custom, [0.5, 0.8, 1.0])

# Get action properties
print(move_forward.getType())  # Action type
print(move_forward.getParameters())  # Action parameters
print(move_forward.getName())  # Action name

# Set action properties
move_forward.setType(nlm.ActionType.TurnLeft)
custom.setParameters([0.1, 0.2, 0.3])

# Clone action
cloned = custom.clone()
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Action()` | Create empty action | None | Action |
| `Action(type)` | Create action with type | type: ActionType | Action |
| `Action(type, parameters)` | Create action with parameters | type: ActionType, parameters: List[float] | Action |
| `getType()` | Get action type | None | ActionType |
| `setType(type)` | Set action type | type: ActionType | None |
| `getParameters()` | Get action parameters | None | List[float] |
| `setParameters(params)` | Set action parameters | params: List[float] | None |
| `getName()` | Get action name | None | str |
| `clone()` | Clone action | None | Action |

### 7. AgentBody

**Pythonic Interface:** `nlm.AgentBody`
**Raw Bindings:** `nlm.AgentBody`

```python
# Create agent body
body = nlm.AgentBody()

# Set body properties
body.x = 10.0
body.y = 15.0
body.orientation = 0.0
body.velocityX = 1.0
body.velocityY = 0.5
body.angularVelocity = 0.0
body.energy = 100.0
body.health = 100.0
body.age = 10
body.isMoving = True
body.isTurning = False
body.lastActionTime = time.time()

# Get properties
print(body.x)  # X position
print(body.y)  # Y position
print(body.orientation)  # Orientation
print(body.velocityX)  # Velocity X
print(body.velocityY)  # Velocity Y
print(body.energy)  # Energy
print(body.health)  # Health
print(body.age)  # Age

# Reset body
body.reset()
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `AgentBody()` | Create empty agent body | None | AgentBody |
| `reset()` | Reset body to initial state | None | None |

#### Properties

| Property | Description | Type |
|----------|-------------|------|
| `x` | X position | float |
| `y` | Y position | float |
| `orientation` | Orientation angle | float |
| `velocityX` | Velocity X component | float |
| `velocityY` | Velocity Y component | float |
| `angularVelocity` | Angular velocity | float |
| `energy` | Current energy | float |
| `health` | Health | float |
| `age` | Age | float |
| `isMoving` | Whether moving | bool |
| `isTurning` | Whether turning | bool |
| `lastActionTime` | Last action time | float |

### 8. ActionResult

**Pythonic Interface:** `nlm.ActionResult`
**Raw Bindings:** `nlm.ActionResult`

```python
# Create action result
result = nlm.ActionResult()
result = nlm.ActionResult(reward=10.0, success=True, message="Success")

# Set properties
result.reward = 15.0
result.success = True
result.message = "Task completed successfully"

# Get properties
print(result.reward)  # Reward value
print(result.success)  # Success flag
print(result.message)  # Message
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `ActionResult()` | Create empty result | None | ActionResult |
| `ActionResult(reward, success, message)` | Create with parameters | reward: float, success: bool, message: str | ActionResult |

#### Properties

| Property | Description | Type |
|----------|-------------|------|
| `reward` | Reward value | float |
| `success` | Success flag | bool |
| `message` | Result message | str |

### 9. SensoryPercept

**Pythonic Interface:** `nlm.SensoryPercept`
**Raw Bindings:** `nlm.SensoryPercept`

```python
# Create sensory percept
percept = nlm.SensoryPercept()

# Get sensory inputs
vision = percept.getVision()
touch = percept.getTouch()
internal = percept.getInternal()
proprioception = percept.getProprioception()
audio = percept.getAudio()

# Set sensory inputs
percept.setVision(vision_data)
percept.setTouch(touch_data)
percept.setInternal(internal_data)
percept.setProprioception(proprioception_data)
percept.setAudio(audio_data)

# Get all signals
all_signals = percept.getAllSignals()
print(percept.getTimestamp())  # Timestamp

# Vision properties
print(percept.getVisionWidth())  # Vision width
print(percept.getVisionHeight())  # Vision height
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `SensoryPercept()` | Create empty percept | None | SensoryPercept |
| `getVision()` | Get vision input | None | Vision |
| `setVision(vision)` | Set vision input | vision: Vision | None |
| `getTouch()` | Get touch input | None | Touch |
| `setTouch(touch)` | Set touch input | touch: Touch | None |
| `getInternal()` | Get internal signals | None | InternalSignals |
| `setInternal(internal)` | Set internal signals | internal: InternalSignals | None |
| `getProprioception()` | Get proprioception | None | Proprioception |
| `setProprioception(proprioception)` | Set proprioception | proprioception: Proprioception | None |
| `getAudio()` | Get audio input | None | Audio |
| `setAudio(audio)` | Set audio input | audio: Audio | None |
| `getAllSignals()` | Get all signals | None | Dict[str, Any] |
| `getTimestamp()` | Get timestamp | None | float |
| `setTimestamp(timestamp)` | Set timestamp | timestamp: float | None |

## Global Functions

### 1. `createDefaultConfig()`

```python
# Create a default configuration
config = nlm.createDefaultConfig()
```

**Returns:** `Config` object

### 2. `createBrain(config)`

```python
# Create a new brain with configuration
brain = nlm.createBrain(config)
```

**Parameters:**
- `config`: Configuration object (`Config` or `Dict[str, Any]`)

**Returns:** `Brain` object

### 3. `createSimpleWorld()`

```python
# Create a new simple world
world = nlm.createSimpleWorld()
```

**Returns:** `SimpleWorld` object

### 4. `createAgentBrain(brain)`

```python
# Create a new agent brain interface
agent = nlm.createAgentBrain(brain)
```

**Parameters:**
- `brain`: Brain object (`Brain`)

**Returns:** `AgentBrain` object

## Enumerations

### 1. NeuronType

- `Excitatory`: Excitatory neuron
- `Inhibitory`: Inhibitory neuron
- `Modulatory`: Modulatory neuron
- `Sensory`: Sensory neuron
- `Motor`: Motor neuron
- `Internal`: Internal neuron

### 2. ActionType

- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `MoveLeft`: Move left
- `MoveRight`: Move right
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `Look`: Look
- `LookUp`: Look up
- `LookDown`: Look down
- `Interact`: Interact with object
- `Eat`: Eat
- `Drink`: Drink
- `Rest`: Rest
- `Wait`: Wait
- `Custom`: Custom action

### 3. MotorCommand

- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `LookLeft`: Look left
- `LookRight`: Look right
- `Interact`: Interact
- `Wait`: Wait

### 4. WorldObjectType

- `Empty`: Empty space
- `Resource`: Resource object
- `Hazard`: Hazard object
- `Wall`: Wall object
- `Marker`: Marker object

### 5. DevelopmentalStage

- `Initial`: Initial developmental stage
- `CriticalPeriod`: Critical period
- `Maturation`: Maturation stage
- `Adult`: Adult stage
- `Aging`: Aging stage

### 6. FiringState

- `Resting`: Resting state
- `Active`: Active state
- `Refractory`: Refractory state
- `Inhibited`: Inhibited state

## Constants

### 1. Invalid IDs

- `nlm.INVALID_NEURON_ID`: Invalid neuron ID
- `nlm.INVALID_SYNAPSE_ID`: Invalid synapse ID
- `nlm.INVALID_REGION_ID`: Invalid region ID
- `nlm.INVALID_POPULATION_ID`: Invalid population ID

## Convenience Functions

### 1. `simulation_session(brain, steps, timestep)`

```python
# Context manager for simulation sessions
from nlm import simulation_session

with simulation_session(brain, steps=1000, timestep=0.1) as session:
    session.run()  # Run all steps
    print(f"Completed {session.steps} steps")
```

### 2. `run_experiment(brain, world, agent, num_steps, timestep)`

```python
# Run a complete agent experiment
from nlm import run_experiment

results = run_experiment(brain, world, agent, num_steps=1000, timestep=0.1)

print(f"Experiment completed with {results['steps']} steps")
print(f"Final spikes: {results['spikes'][-1]}")
```

### 3. `profile_simulation(brain, world, agent, steps, timestep)`

```python
# Profile simulation performance
from nlm import profile_simulation

profile = profile_simulation(brain, world, agent, steps=1000, timestep=0.1)

print(f"Simulation performance:")
print(f"  Total time: {profile['total_time']:.2f}s")
print(f"  Average step time: {profile['average_step_time']*1000:.2f}ms")
print(f"  Steps per second: {profile['steps_per_second']:.0f}")
```

## Example Usage

### Example 1: Simple Brain Simulation

```python
import nlm

# Create and initialize brain
brain = nlm.Brain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.total_spikes} spikes")
```

### Example 2: Complete Agent System

```python
import nlm

# Setup
config = nlm.create_default_config()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.percept)
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.simulation_time)

print("Agent simulation complete!")
```

### Example 3: Configuration with Custom Settings

```python
import nlm

# Create configuration with custom settings
config = nlm.Config()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)

# Create brain
brain = nlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(200):
    brain.step(step)
```

## Type Hints

For IDE support and type checking, the following type hints are available:

```python
# For Python 3.9+
from typing import Optional, List, Dict, Any

# Type aliases
ConfigDict = Dict[str, Any]
NeuronId = int
SynapseId = int
RegionId = int

# Common function signatures
create_brain_config() -> Config
create_brain(config: Optional[ConfigDict] = None) -> Brain
create_simple_world() -> SimpleWorld
create_agent_brain(brain: Brain) -> AgentBrain
```

## Additional Resources

- **Tutorial**: `TUTORIAL.md` - Comprehensive tutorial for beginners
- **Advanced Guide**: `ADVANCED.md` - Advanced topics and techniques
- **Examples**: `EXAMPLES.md` - Collection of practical examples
- **Project Improvement Plan**: `PROJECT_IMPROVEMENT_PLAN.md` - Overall improvement strategy

## Version Information

- **API Version**: 2.0.0
- **Python Compatibility**: 3.8+
- **C++ Standard**: C++20
- **Dependencies**: pybind11 >= 2.11.0, scikit-build-core >= 0.5.0

This API reference provides complete documentation for both the raw pybind11 bindings and the enhanced Python interface, giving users flexibility in how they interact with the NLM framework.
