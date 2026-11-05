# NLM API Guide

## Overview

NLM API provides Python bindings for the NLM neural learning machine project via pybind11. This guide documents the Python interface for creating and controlling neural simulations programmatically.

## Installation

### System Dependencies
```bash
# Install NLM from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python package
pip install -e .
```

### Python Usage
```python
import pynlm

# Verify installation
print("NLM Python bindings loaded successfully")
```

## Core Classes

### Config

Configuration management for NLM parameters.

#### Constructor
```python
config = Config()
```

#### Methods

##### loadFromFile(filepath)
Load configuration from JSON file.

**Parameters**:
- `filepath` (str): Path to configuration file

**Returns**:
- bool: True if loaded successfully

**Example**:
```python
config.loadFromFile("configs/experimental.cfg")
```

##### loadFromArgs(args)
Load configuration from command-line arguments.

**Parameters**:
- `args` (list): List of command-line arguments

**Returns**:
- bool: True if loaded successfully

**Example**:
```python
config.loadFromArgs(["--neuron_count", "1000", "--plasticity_learning_rate", "0.01"])
```

##### saveToFile(filepath)
Save configuration to JSON file.

**Parameters**:
- `filepath` (str): Path to save configuration

**Returns**:
- bool: True if saved successfully

**Example**:
```python
config.saveToFile("my_config.json")
```

##### has(key)
Check if a configuration key exists.

**Parameters**:
- `key` (str): Configuration key

**Returns**:
- bool: True if key exists

**Example**:
```python
if config.has("neuron_count"):
    print("neuron_count is configured")
```

##### getOr(key, defaultValue)
Get configuration value with default fallback.

**Parameters**:
- `key` (str): Configuration key
- `defaultValue`: Default value if key not found

**Returns**:
- Configuration value of specified type

**Example**:
```python
neuron_count = config.getOr("neuron_count", 1000)
learning_rate = config.getOr("plasticity_learning_rate", 0.01)
```

##### set(key, value, source=Runtime)
Set configuration value.

**Parameters**:
- `key` (str): Configuration key
- `value`: Value to set
- `source` (ConfigSource): Source of configuration

**Example**:
```python
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)
```

##### getKeys()
Get all configuration keys.

**Returns**:
- list: All configuration keys

**Example**:
```python
keys = config.getKeys()
print(f"Configuration keys: {keys}")
```

##### clear()
Clear all configuration entries.

**Example**:
```python
config.clear()
```

##### summary()
Get a summary string of the configuration.

**Returns**:
- str: Configuration summary

**Example**:
```python
summary = config.summary()
print(summary)
```

##### __repr__()
String representation of the configuration.

**Example**:
```python
repr(config)
# Returns: "<Config: random_seed=42, neuron_count=1000, ...>"
```

### Brain

Central neural simulation brain class.

#### Constructor
```python
def createBrain(config: Config) -> Brain:
    """Create a new brain with configuration"""
```

#### Methods

##### initialize()
Initialize the brain with configuration.

**Example**:
```python
brain.initialize()
```

##### step(currentStep)
Perform a simulation step.

**Parameters**:
- `currentStep` (int): Current simulation step

**Example**:
```python
brain.step(step_number)
```

##### step(currentStep, currentTime)
Perform a simulation step with timestamp.

**Parameters**:
- `currentStep` (int): Current simulation step
- `currentTime` (float): Current simulation time

**Example**:
```python
brain.step(step_number, step_number * 0.001)
```

##### receiveSensoryInput(input)
Inject sensory input into the brain.

**Parameters**:
- `input`: SensoryInput object

**Example**:
```python
brain.receiveSensoryInput(percept)
```

##### injectCurrent(neuron, current)
Inject current into a specific neuron.

**Parameters**:
- `neuron` (NeuronId): Target neuron ID
- `current` (float): Current value in nanoamps

**Example**:
```python
brain.injectCurrent(neuron_id, 30.0)
```

##### injectCurrentToNeurons(type, current)
Inject current into all neurons of a specific type.

**Parameters**:
- `type` (NeuronType): Type of neurons to stimulate
- `current` (float): Current value in nanoamps

**Example**:
```python
brain.injectCurrentToNeurons(NeuronType.Sensory, 30.0)
```

##### produceAction()
Produce motor action based on neural activity.

**Returns**:
- Action object

**Example**:
```python
action = brain.produceAction()
print(f"Action: {action.getType()}")
```

##### reset()
Reset brain to initial state.

**Example**:
```python
brain.reset()
```

##### save(filepath)
Save brain state to file.

**Parameters**:
- `filepath` (str): Path to save brain state

**Returns**:
- bool: True if saved successfully

**Example**:
```python
brain.save("brain_state.nml")
```

##### load(filepath)
Load brain state from file.

**Parameters**:
- `filepath` (str): Path to load brain state from

**Returns**:
- bool: True if loaded successfully

**Example**:
```python
brain.load("brain_state.nml")
```

##### addRegion(name="")
Add a new neural region.

**Parameters**:
- `name` (str): Region name (optional)

**Returns**:
- RegionId: ID of the new region

**Example**:
```python
region_id = brain.addRegion("Visual")
```

##### getRegion(id)
Get a region by ID.

**Parameters**:
- `id` (RegionId): Region ID

**Returns**:
- NeuralRegion: The region object

**Example**:
```python
region = brain.getRegion(RegionId(1))
```

##### getRegionCount()
Get the number of regions.

**Returns**:
- int: Number of regions

**Example**:
```python
count = brain.getRegionCount()
print(f"Regions: {count}")
```

##### getRegionIds()
Get all region IDs.

**Returns**:
- list: All region IDs

**Example**:
```python
region_ids = brain.getRegionIds()
```

##### getRegions()
Get all regions.

**Returns**:
- list: All region objects

**Example**:
```python
regions = brain.getRegions()
```

##### getTotalNeuronCount()
Get total neuron count across all regions.

**Returns**:
- int: Total neuron count

**Example**:
```python
total = brain.getTotalNeuronCount()
print(f"Total neurons: {total}")
```

##### getTotalSynapseCount()
Get total synapse count across all regions.

**Returns**:
- int: Total synapse count

**Example**:
```python
total = brain.getTotalSynapseCount()
print(f"Total synapses: {total}")
```

##### getActiveNeuronCount()
Get count of active neurons.

**Returns**:
- int: Active neuron count

**Example**:
```python
active = brain.getActiveNeuronCount()
print(f"Active neurons: {active}")
```

##### getFiringNeuronCount()
Get count of currently firing neurons.

**Returns**:
- int: Firing neuron count

**Example**:
```python
firing = brain.getFiringNeuronCount()
print(f"Firing neurons: {firing}")
```

##### getAverageFiringRate()
Get average firing rate across all neurons.

**Returns**:
- float: Average firing rate in Hz

**Example**:
```python
rate = brain.getAverageFiringRate()
print(f"Average firing rate: {rate} Hz")
```

##### getExcitationInhibitionRatio()
Get excitation/inhibition balance ratio.

**Returns**:
- float: E/I ratio

**Example**:
```python
ei_ratio = brain.getExcitationInhibitionRatio()
print(f"E/I ratio: {ei_ratio}")
```

##### getTotalSpikeCount()
Get total spike count.

**Returns**:
- int: Total spike count

**Example**:
```python
total_spikes = brain.getTotalSpikeCount()
print(f"Total spikes: {total_spikes}")
```

##### getDevelopmentalStage()
Get current developmental stage.

**Returns**:
- DevelopmentalStage: Current stage

**Example**:
```python
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")
```

##### setDevelopmentalStage(stage)
Set developmental stage.

**Parameters**:
- `stage` (DevelopmentalStage): Target stage

**Example**:
```python
brain.setDevelopmentalStage(DevelopmentalStage.Adult)
```

##### getConfig()
Get the configuration.

**Returns**:
- Config: Configuration object

**Example**:
```python
config = brain.getConfig()
```

##### logStatus()
Log brain status.

**Example**:
```python
brain.logStatus()
```

### AgentBrain

Agent brain interface connecting NLM brain to world.

#### Constructor
```python
def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create a new agent brain interface"""
```

#### Methods

##### initialize(world)
Initialize with world environment.

**Parameters**:
- `world`: SimpleWorld instance

**Example**:
```python
agent.initialize(world)
```

##### getSensoryInputSize()
Get expected sensory input size.

**Returns**:
- int: Expected sensory input size

**Example**:
```python
size = agent.getSensoryInputSize()
```

##### getMotorOutputSize()
Get expected motor output size.

**Returns**:
- int: Expected motor output size

**Example**:
```python
size = agent.getMotorOutputSize()
```

##### processSensoryInput(percept)
Process sensory percept and inject into brain.

**Parameters**:
- `percept`: SensoryPercept object

**Example**:
```python
agent.processSensoryInput(percept)
```

##### decodeMotorCommand()
Decode brain motor activity into motor command.

**Returns**:
- Action object

**Example**:
```python
action = agent.decodeMotorCommand()
```

##### applyRewardModulation(reward, predictedReward)
Apply reward-based neuromodulation.

**Parameters**:
- `reward` (float): Actual reward received
- `predictedReward` (float): Predicted reward

**Example**:
```python
agent.applyRewardModulation(reward, predicted_reward)
```

##### updateDevelopment(timestep)
Update development system.

**Parameters**:
- `timestep` (float): Time step for development

**Example**:
```python
agent.updateDevelopment(0.1)
```

##### getDevelopmentalStage()
Get current developmental stage.

**Returns**:
- DevelopmentalStage: Current stage

**Example**:
```python
stage = agent.getDevelopmentalStage()
```

##### getNeuromodulationLevel()
Get current neuromodulation level.

**Returns**:
- float: Current neuromodulation level

**Example**:
```python
level = agent.getNeuromodulationLevel()
```

##### getCuriosityLevel()
Get curiosity level.

**Returns**:
- float: Current curiosity level

**Example**:
```python
level = agent.getCuriosityLevel()
```

##### getNoveltyLevel()
Get novelty level.

**Returns**:
- float: Current novelty level

**Example**:
```python
level = agent.getNoveltyLevel()
```

##### getPredictionError()
Get prediction error.

**Returns**:
- float: Current prediction error

**Example**:
```python
error = agent.getPredictionError()
```

##### reset()
Reset agent for new episode.

**Example**:
```python
agent.reset()
```

##### getBrain()
Get the underlying brain.

**Returns**:
- Brain: The brain object

**Example**:
```python
brain = agent.getBrain()
```

##### enableRewardModulation(enable)
Enable or disable reward modulation.

**Parameters**:
- `enable` (bool): Enable or disable

**Example**:
```python
agent.enableRewardModulation(True)
```

##### enableStructuralPlasticity(enable)
Enable or disable structural plasticity.

**Parameters**:
- `enable` (bool): Enable or disable

**Example**:
```python
agent.enableStructuralPlasticity(True)
```

##### enableDevelopment(enable)
Enable or disable development.

**Parameters**:
- `enable` (bool): Enable or disable

**Example**:
```python
agent.enableDevelopment(True)
```

##### enableCuriosity(enable)
Enable or disable curiosity.

**Parameters**:
- `enable` (bool): Enable or disable

**Example**:
```python
agent.enableCuriosity(True)
```

##### isRewardModulationEnabled()
Check if reward modulation is enabled.

**Returns**:
- bool: True if enabled

**Example**:
```python
if agent.isRewardModulationEnabled():
    print("Reward modulation is enabled")
```

##### isStructuralPlasticityEnabled()
Check if structural plasticity is enabled.

**Returns**:
- bool: True if enabled

**Example**:
```python
if agent.isStructuralPlasticityEnabled():
    print("Structural plasticity is enabled")
```

##### isDevelopmentEnabled()
Check if development is enabled.

**Returns**:
- bool: True if enabled

**Example**:
```python
if agent.isDevelopmentEnabled():
    print("Development is enabled")
```

##### isCuriosityEnabled()
Check if curiosity is enabled.

**Returns**:
- bool: True if enabled

**Example**:
```python
if agent.isCuriosityEnabled():
    print("Curiosity is enabled")
```

### SimpleWorld

Simple 2D world for NLM simulation.

#### Constructor
```python
def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world"""
```

#### Methods

##### configure(width, height, visionWidth, visionHeight)
Configure world dimensions.

**Parameters**:
- `width` (int): World width in cells
- `height` (int): World height in cells
- `visionWidth` (int): Vision width in cells
- `visionHeight` (int): Vision height in cells

**Example**:
```python
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
```

##### reset()
Reset the world to initial state.

**Example**:
```python
world.reset()
```

##### setAgentStart(x, y)
Set agent starting position.

**Parameters**:
- `x` (float): X coordinate
- `y` (float): Y coordinate

**Example**:
```python
world.setAgentStart(5.0, 5.0)
```

##### update(timestep)
Update the world by timestep seconds.

**Parameters**:
- `timestep` (float): Time step in seconds

**Example**:
```python
world.update(0.1)
```

##### applyMotorCommand(cmd, currentTime)
Apply motor command to the world.

**Parameters**:
- `cmd` (Action): Motor command
- `currentTime` (float): Current time in simulation

**Example**:
```python
world.applyMotorCommand(action, current_time)
```

##### getSensoryPercept()
Get sensory percept from the world.

**Returns**:
- SensoryPercept: Sensory percept data

**Example**:
```python
percept = world.getSensoryPercept()
```

##### getAgentBody()
Get agent body state.

**Returns**:
- AgentBody: Agent body state

**Example**:
```python
body = world.getAgentBody()
```

##### addObject(obj)
Add object to the world.

**Parameters**:
- `obj` (WorldObject): Object to add

**Example**:
```python
world.addObject(resource_obj)
```

##### removeObject(x, y)
Remove object from world.

**Parameters**:
- `x` (float): X coordinate
- `y` (float): Y coordinate

**Example**:
```python
world.removeObject(3.0, 3.0)
```

##### isValidPosition(x, y)
Check if position is valid.

**Parameters**:
- `x` (float): X coordinate
- `y` (float): Y coordinate

**Returns**:
- bool: True if position is valid

**Example**:
```python
if world.isValidPosition(5.0, 5.0):
    print("Position is valid")
```

##### getWidth()
Get world width.

**Returns**:
- int: World width in cells

**Example**:
```python
width = world.getWidth()
```

##### getHeight()
Get world height.

**Returns**:
- int: World height in cells

**Example**:
```python
height = world.getHeight()
```

##### getMaxEnergy()
Get maximum agent energy.

**Returns**:
- float: Maximum energy

**Example**:
```python
max_energy = world.getMaxEnergy()
```

##### setMaxEnergy(e)
Set maximum agent energy.

**Parameters**:
- `e` (float): Maximum energy

**Example**:
```python
world.setMaxEnergy(100.0)
```

##### getEnergyDecayRate()
Get energy decay rate.

**Returns**:
- float: Energy decay rate

**Example**:
```python
decay_rate = world.getEnergyDecayRate()
```

##### setEnergyDecayRate(r)
Set energy decay rate.

**Parameters**:
- `r` (float): Energy decay rate

**Example**:
```python
world.setEnergyDecayRate(0.01)
```

##### getSimulationTime()
Get current simulation time.

**Returns**:
- float: Current simulation time

**Example**:
```python
time = world.getSimulationTime()
```

##### setRandomSeed(seed)
Set random seed for world.

**Parameters**:
- `seed` (int): Random seed

**Example**:
```python
world.setRandomSeed(42)
```

##### getRandomSeed()
Get random seed for world.

**Returns**:
- int: Random seed

**Example**:
```python
seed = world.getRandomSeed()
```

### SensoryInput Classes

#### Vision

Visual sensory input class.

**Constructor**:
```python
vision = Vision(width, height, channels=3)
```

**Methods**:
- `setData(data)`: Set vision data
- `getWidth()`: Get vision width
- `getHeight()`: Get vision height
- `getChannels()`: Get number of channels

#### Audio

Audio sensory input class.

**Constructor**:
```python
audio = Audio(sampleRate, numSamples)
```

**Methods**:
- `setData(data)`: Set audio data
- `setSampleRate(rate)`: Set sample rate
- `getSampleRate()`: Get sample rate
- `getNumSamples()`: Get number of samples

#### InternalSignals

Internal signals sensory input class.

**Methods**:
- `addSignal(value)`: Add internal signal
- `clearSignals()`: Clear all signals

### Action Classes

#### Action

Action representation for motor output.

**Constructor**:
```python
action = Action(actionType)
action = Action(actionType, parameters)
```

**Methods**:
- `getType()`: Get action type
- `setType(type)`: Set action type
- `getParameters()`: Get action parameters
- `setParameters(params)`: Set action parameters
- `getName()`: Get action name
- `clone()`: Create a copy of action

### WorldObject Classes

#### WorldObject

World object representation.

**Attributes**:
- `x`: X coordinate
- `y`: Y coordinate
- `radius`: Object radius
- `type`: Object type
- `value`: Object value
- `active`: Object active state

### AgentBody

Agent body state representation.

**Attributes**:
- `x`: X coordinate
- `y`: Y coordinate
- `orientation`: Orientation angle
- `velocityX`: X velocity
- `velocityY`: Y velocity
- `angularVelocity`: Angular velocity
- `energy`: Current energy
- `health`: Current health
- `age`: Age in simulation steps
- `isMoving`: Moving state
- `isTurning`: Turning state
- `lastActionTime`: Time since last action

### ActionResult

Action result from world interaction.

**Attributes**:
- `reward`: Reward received
- `success`: Whether action was successful
- `message`: Result message

### SensoryPercept

Sensory percept data.

**Methods**:
- `getVision()`: Get vision data
- `setVision(vision)`: Set vision data
- `getVisionWidth()`: Get vision width
- `getVisionHeight()`: Get vision height
- `getTouch()`: Get touch data
- `setTouch(touch)`: Set touch data
- `getInternal()`: Get internal signals
- `setInternal(internal)`: Set internal signals
- `getProprioception()`: Get proprioception data
- `setProprioception(proprioception)`: Set proprioception data
- `getAudio()`: Get audio data
- `setAudio(audio)`: Set audio data
- `getAllSignals()`: Get all sensory signals
- `getTimestamp()`: Get timestamp
- `setTimestamp(timestamp)`: Set timestamp

### Helper Functions

#### createDefaultConfig()
Create a default configuration.

**Returns**:
- Config: Default configuration

**Example**:
```python
config = pynlm.createDefaultConfig()
```

#### createBrain(config)
Create a new brain with configuration.

**Parameters**:
- `config` (Config): Configuration for brain

**Returns**:
- Brain: New brain instance

**Example**:
```python
brain = pynlm.createBrain(config)
```

#### createSimpleWorld()
Create a new simple world.

**Returns**:
- SimpleWorld: New world instance

**Example**:
```python
world = pynlm.createSimpleWorld()
```

#### createAgentBrain(brain)
Create a new agent brain interface.

**Parameters**:
- `brain` (Brain): Brain to interface with

**Returns**:
- AgentBrain: New agent brain interface

**Example**:
```python
agent = pynlm.createAgentBrain(brain)
```

## Enumerations

### NeuronType
- Excitatory
- Inhibitory
- Modulatory
- Sensory
- Motor
- Internal

### SynapseType
- Excitatory
- Inhibitory
- Modulatory
- Electrical
- GapJunction

### DevelopmentalStage
- Initial
- CriticalPeriod
- Maturation
- Adult
- Aging

### FiringState
- Resting
- Active
- Refractory
- Inhibited

### ActionType
- MoveForward
- MoveBackward
- MoveLeft
- MoveRight
- TurnLeft
- TurnRight
- Look
- LookUp
- LookDown
- Interact
- Eat
- Drink
- Rest
- Wait
- Custom

### MotorCommand
- MoveForward
- MoveBackward
- TurnLeft
- TurnRight
- LookLeft
- LookRight
- Interact
- Wait

### WorldObjectType
- Empty
- Resource
- Hazard
- Wall
- Marker

### ConfigSource
- Default
- File
- CommandLine
- Runtime

## Constants

### Invalid IDs
- `INVALID_NEURON_ID`: Invalid neuron ID
- `INVALID_SYNAPSE_ID`: Invalid synapse ID
- `INVALID_REGION_ID`: Invalid region ID
- `INVALID_POPULATION_ID`: Invalid population ID

## Advanced Usage Patterns

### Complete Simulation Loop
```python
import pynlm

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(1000):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

### Configuration Example
```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("random_seed", 42)
config.set("neuron_count", 500)
config.set("simulation_timestep", 0.001)
config.set("plasticity_learning_rate", 0.01)

# Create and run brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")

# Save brain state
brain.save("my_brain.nml")

# Load and continue
brain.load("my_brain.nml")
```

### Memory System Example
```python
import pynlm

# Create configuration with memory settings
config = pynlm.createDefaultConfig()
config.set("working_memory_capacity", 200.0)
config.set("episodic_memory_max_episodes", 500)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize agent with curiosity
agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Run learning episode
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Think
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = 1.0 if step % 10 ==  ragazzo else 0.0
    agent.applyRewardModulation(reward, 0.5)
```

### Development System Example
```python
import pynlm

# Create configuration with development
config = pynlm.createDefaultConfig()
config.set("development_synaptogenesis_rate", 0.002)
config.set("development_pruning_rate", 0.0001)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize agent with development
agent = pynlm.createAgentBrain(brain)
agent.enableDevelopment(True)

# Run developmental simulation
for step in range(1000):
    # Update development
    agent.updateDevelopment(0.001)
    
    # Get developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: {stage}")
    
    # Run simulation
    brain.step(step)
```

## Error Handling

### Common Exceptions

#### RuntimeError
Raised for runtime errors in NLM operations.

**Example**:
```python
try:
    brain.initialize()
except RuntimeError as e:
    print(f"Error initializing brain: {e}")
```

#### ValueError
Raised for invalid parameter values.

**Example**:
```python
try:
    config.set("neuron_count", -1)
except ValueError as e:
    print(f"Invalid value: {e}")
```

## Performance Tips

### Memory Management
1. **Reuse objects**: Reuse configuration objects when possible
2. **Clear memory**: Use `brain.reset()` for memory cleanup
3. **Monitor memory**: Track memory usage in long simulations

### Simulation Performance
1. **Batch operations**: Process multiple steps efficiently
2. **Optimize configuration**: Use minimal necessary configuration
3. **Profile simulation**: Use profiling tools for optimization

### Configuration Performance
1. **Cache frequently accessed values**: Use `config.getOr()` with defaults
2. **Limit configuration updates**: Minimize frequent configuration changes
3. **Use efficient data structures**: Prefer arrays over lists where appropriate

## Best Practices

### Code Style
1. **Consistent naming**: Use camelCase for method names, snake_case for variables
2. **Error handling**: Always handle potential exceptions
3. **Documentation**: Include docstrings for public APIs
4. **Type checking**: Use type hints for better code clarity

### Simulation Management
1. **Initialize properly**: Always call `initialize()` before using brain
2. **Use appropriate timesteps**: Choose timestep based on simulation requirements
3. **Monitor progress**: Log or print progress for long simulations
4. **Save checkpoints**: Save brain state regularly during long simulations

### Configuration Management
1. **Use defaults**: Start with `createDefaultConfig()` and modify as needed
2. **Validate configuration**: Use `config.validate()` before running
3. **Document configurations**: Keep comments in configuration files
4. **Test configurations**: Test configuration with small simulations first

## Migration Guide

### From Previous Versions

#### Version 1.0 to 2.0
- Configuration format changed from INI to JSON
- API methods renamed (e.g., `step` now accepts time parameter)
- New memory system added
- Development system enhanced

#### Migration Steps
```python
# Old way
config.loadFromIniFile("config.ini")
brain = Brain(config)
brain.initialize()
brain.step(step)

# New way
config.loadFromFile("config.json")
brain = createBrain(config)
brain.initialize()
brain.step(step, step * 0.001)
```

## Testing Examples

### Unit Testing
```python
import pynlm
import unittest

class TestNLM(unittest.TestCase):
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_initialization(self):
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
    
    def test_neuron_count(self):
        self.assertEqual(self.brain.getTotalNeuronCount(), 
                        self.config.getOr("neuron_count", 1000))
    
    def test_simulation_step(self):
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(0)
        self.assertGreaterEqual(self.brain.getTotalSpikeCount(), initial_spikes)
```

### Integration Testing
```python
import pynlm

def test_complete_simulation():
    """Test complete brain-world interaction"""
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Verify results
    assert brain.getTotalNeuronCount() > 0
    assert world.getSimulationTime() > 0
    print("Integration test passed")
```

## API Changes and Deprecations

### Deprecated Methods
1. **Config.loadFromArgs**: Use `loadFromArgs()` instead
2. **Brain.getRegion**: Use `getRegion()` with RegionId

### New Methods (Version 2.0)
1. **Config.saveToFile**: Save configuration to JSON
2. **Config.getOr**: Get configuration with default
3. **Brain.injectCurrentToNeurons**: Inject to all neurons of type
4. **Brain.getExcitationInhibitionRatio**: Get E/I balance
5. **AgentBrain.enableCuriosity**: Enable curiosity system

---
*API guide generated from Python bindings (pynlm)*
