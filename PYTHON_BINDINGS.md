# Python Bindings API Documentation

## Overview

The NLM Python bindings provide a comprehensive interface to the Neural Learning Machine (NLM) C++ neural simulation framework. The bindings are generated using pybind11 and provide access to all major components of the NLM system including neural computation, memory systems, prediction, neuromodulation, and cognitive capabilities.

## Installation

### Prerequisites
- Python 3.8 or higher
- pybind11 development headers
- Build tools (CMake, compiler)

### Building Python Bindings

The Python bindings are built as part of the main NLM build system:

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
cmake --build .
```

Alternatively, if you already have the NLM C++ library built:

```bash
# Set PYTHONPATH if needed
export PYTHONPATH=/path/to/pynlm.so:$PYTHONPATH
```

## Core API Classes

### nlm.Config
**Configuration for NLM system**

```python
config = nlm.Config()
config.loadFromArgs(['--config', 'configs/default.cfg'])
config.set("neuron_count", 1000)
config.set("region_count", 3)
```

**Methods**:
- `__init__()` - Create empty configuration
- `loadFromFile(filepath: str)` - Load from JSON/YAML file
- `loadFromArgs(args: List[str])` - Load from command line arguments
- `saveToFile(filepath: str)` - Save to JSON file
- `has(key: str)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary

### nlm.Brain
**Central neural simulation brain class**

```python
import nlm

config = nlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 3)

brain = nlm.Brain(config)
if brain.initialize():
    # Simulation loop
    for step in range(100):
        brain.step(step)
    # Process sensory input
    input_data = [0.5, 0.8, 0.3, 0.7]
    sensory_input = nlm.SensoryInput()
    sensory_input.setData(input_data)
    brain.receiveSensoryInput(sensory_input)
    # Produce action
    action = brain.produceAction()
    print(f"Action: {action.getType()}")
```

**Methods**:
- `__init__(config: Config)` - Create brain with configuration
- `initialize()` - Initialize the brain
- `step(currentStep: int)` - Perform simulation step
- `step(currentStep: int, currentTime: float)` - Perform simulation step with timestamp
- `receiveSensoryInput(input: SensoryInput)` - Inject sensory input
- `injectCurrent(neuron: NeuronId, current: float)` - Inject current to specific neuron
- `injectCurrentToNeurons(type: NeuronType, current: float)` - Inject current to all neurons of type
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath: str)` - Save brain state to file
- `load(filepath: str)`があ - Load brain state from file
- `addRegion(name: str = "")` - Add new neural region
- `getRegion(id: RegionId)` - Get region by ID
- `getRegionCount()` - Get number of regions
- `getRegionIds()` - Get all region IDs
- `getRegions()` - Get all regions
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getActiveNeuronCount()` - Get count of active neurons
- `getFiringNeuronCount()` - Get count of currently firing neurons
- `getAverageFiringRate()` - Get average firing rate across all neurons
- `getExcitationInhibitionRatio()` - Get excitation/inhibition balance ratio
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get current developmental stage
- `setDevelopmentalStage(stage: DevelopmentalStage)` - Set developmental stage
- `getConfig()` - Get the configuration
- `logStatus()` - Log brain status

### nlm.AgentBrain
**Agent brain interface connecting NLM brain to world**

```python
brain = nlm.createBrain(config)
agentBrain = nlm.createAgentBrain(brain)
agentBrain.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agentBrain.processSensoryInput(percept)

# Brain simulation
for step in range(50):
    brain.step(step)

# Decode motor command
motorCommand = agentBrain.decodeMotorCommand()
world.applyMotorCommand(motorCommand, time)

# Apply reward modulation
reward = world.computeReward(agent)
agentBrain.applyRewardModulation(reward, predictedReward)
```

**Methods**:
- `__init__(brain: Brain)` - Create agent brain interface
- `initialize(world: SimpleWorld)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept: SensoryPercept)` - Process sensory percept
- `decodeMotorCommand()` - Decode brain activity to motor command
- `applyRewardModulation(reward: float, predictedReward: float)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep: float)` - Update development system
- `getDevelopmentalStage()` - Get current developmental state
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent for new episode
- `getBrain()` - Get the underlying brain
- `enableRewardModulation(enable: bool)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable: bool)` - Enable/disable structural plasticity
- `enableDevelopment(enable: bool)` IEP Enabled/disable development
- `enableCuriosity(enable: bool)` - Enable/disable curiosity
- `isRewardModulationEnabled()` - Check if reward modulation enabled
- `isStructuralPlasticityEnabled()` - Check if structural plasticity enabled
- `isDevelopmentEnabled()` - Check if development enabled
- `isCuriosityEnabled()` - Check if curiosity enabled

### nlm.SimpleWorld
**Simple 2D world for NLM simulation**

```python
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
world.setAgentStart(10, 10)

# Update world
for time in range(100):
    world.update(time)
    
    # Get percept
    percept = world.getSensoryPercept()
    
    # Apply agent brain
    motorCommand = agentBrain.decodeMotorCommand()
    world.applyMotorCommand(motorCommand, time)
    
    # Compute reward
    reward = world.computeReward(agent)
```

**Methods**:
- `__init__()` - Create empty world
- `configure(width: int, height: int, visionWidth: int, visionHeight: int)` - Configure world dimensions
- `reset()` - Reset world
- `setAgentStart(x: float, y: float)` - Set agent starting position
- `update(timestep: float)` - Update world state
- `applyMotorCommand(cmd: MotorCommand, currentTime: float)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj: WorldObject)` - Add world object
- `removeObject(x: float, y: float)` - Remove object at position
- `isValidPosition(x: float, y: float)` - Check if position is valid
- `getWidth()` - Get world width
- `getHeight()` - Get world height
- `getMaxEnergy()` - Get max energy
- `setMaxEnergy(e: float)` - Set max energy
- `getEnergyDecayRate()` - Get energy decay rate
- `setEnergyDecayRate(r: float)` - Set energy decay rate
- `getSimulationTime()` - Get simulation time
- `setRandomSeed(seed: int)` - Set random seed
- `getRandomSeed()` - Get random seed

### nlm.Action
**Action representation for motor output**

```python
action = nlm.Action(nlm.ActionType.MoveForward, [0.8, 0.2, 0.5])
print(f"Action type: {action.getType()}")
print(f"Action parameters: {action.getParameters()}")
action_copy = action.clone()
```

**Methods**:
- `__init__()` - Create empty action
- `__init__(type: ActionType)` - Create action with type
- `__init__(type: ActionType, parameters: List[float])` - Create action with type and parameters
- `getType()` - Get action type
- `setType(type: ActionType)` - Set action type
- `getParameters()` - Get action parameters
- `setParameters(params: List[float])` - Set action parameters
- `getName()` - Get action name
- `clone()` - Clone action

### nlm.SensoryInput
**Base class for sensory input**

```python
input = nlm.SensoryInput()
input.setTimestamp(0.0)
data = input.getData()
print(f"Data length: {len(data)}")
print(f"Dimensions: {input.getDimensions()}")
```

**Methods**:
- `getType()` - Get the type of sensory input
- `getData()` - Get the raw data as a vector
- `getDimensions()` - Get the dimensionality
- `getTimestamp()` - Get the timestamp
- `setTimestamp(timestamp: float)` - Set the timestamp

### nlm.Vision
**Vision sensory input**

```python
vision = nlm.Vision(width=10, height=10, channels=3)
data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6] * 25  # 10x10x3 = 300 values
vision.setData(data)
print(f"Width: {vision.getWidth()}")
print(f"Height: {vision.getHeight()}")
print(f"Channels: {vision.getChannels()}")
```

**Methods**:
- `__init__()` - Create empty vision
- `__init__(width: int, height: int, channels: int = 3)` - Create vision with dimensions
- `setData(data: List[float])` - Set data
- `getWidth()` - Get width
- `getHeight()` - Get height
- `getChannels()` - Get channels

### nlm.Audio
**Audio sensory input**

```python
audio = nlm.Audio(sampleRate=44100, numSamples=1000)
audio.setData([0.0, 0.1, 0.2, ...])  # 1000 samples
audio.setSampleRate(48000)
print(f"Sample rate: {audio.getSampleRate()}")
print(f"Num samples: {audio.getNumSamples()}")
```

**Methods**:
- `__init__()` - Create empty audio
- `__init__(sampleRate: int, numSamples: int)` - Create audio with specs
- `setData(data: List[float])` - Set data
- `setSampleRate(sampleRate: int)` - Set sample rate
- `getSampleRate()` - Get sample rate
- `getNumSamples()` - Get number of samples

### nlm.InternalSignals
**Internal signals sensory input**

```python
internal = nlm.InternalSignals()
internal.addSignal(0.5)
internal.addSignal(-0.3)
internal.addSignal(1.0)
print(f"Signal count: {internal.getDimensions()}")
signals = internal.getData()
print(f"Signals: {signals}")
internal.clearSignals()
```

**Methods**:
- `__init__()` - Create empty internal signals
- `addSignal(value: float)` - Add signal value
- `clearSignals()` - Clear all signals

### nlm.WorldObject
**World object representation**

```python
obj = nlm.WorldObject(x=5.0, y=5.0, type=nlm.WorldObjectType.Resource, value=10.0, radius=0.5)
print(f"Position: ({obj.x}, {obj.y})")
print(f"Type: {obj.type}")
print(f"Value: {obj.value}")
print(f"Active: {obj.active}")
```

**Attributes**:
- `x: float` - X position
- `y: float` - Y position
- `radius: float` - Object radius
- `type: WorldObjectType` - Object type
- `value: float` - Object value
- `active: bool` - Object active state

### nlm.AgentBody
**Agent body state**

```python
body = nlm.AgentBody()
body.x = 10.0
body.y = 10.0
body.orientation = 45.0
body.velocityX = 1.0
body.velocityY = 0.5
body.angularVelocity = 0.1
body.energy = 100.0
body.health = 1.0
body.age = 0
body.isMoving = True
body.isTurning = False
body.lastActionTime = 0.0

body.reset()
```

**Attributes**:
- `x: float` - X position
- `y: float` - Y position
- `orientation: float` - Orientation in degrees
- `velocityX: float` - X velocity
- `velocityY: float` - Y velocity
- `angularVelocity: float` - Angular velocity
- `energy: float` - Energy level
- `health: float` - Health level
- `age: int` - Age in steps
- `isMoving: bool` - Is moving
- `isTurning: bool` - Is turning
- `lastActionTime: float` - Last action time

### nlm.ActionResult
**Action result from world**

```python
result = nlm.ActionResult(reward=5.0, success=True, message="Reached goal")
print(f"Reward: {result.reward}")
print(f"Success: {result.success}")
print(f"Message: {result.message}")
```

**Attributes**:
- `reward: float` - Reward received
- `success: bool` - If action was successful
- `message: str` - Result message

### nlm.SensoryPercept
**Sensory percept data**

```python
percept = nlm.SensoryPercept()
percept.setVision(vision)
percept.setTouch([0.1, 0.2, 0.3, 0.4])
percept.setInternal(internal)
percept.setProprioception([0.5, 0.6])
percept.setAudio(audio)
print(f"Vision available: {percept.getVisionWidth() > 0}")
print(f"Touch signals: {len(percept.getTouch())}")
print(f"Internal signals: {len(percept.getInternal())}")
```

**Methods**:
- `__init__()` - Create empty percept
- `getVision()` - Get vision
- `setVision(vision: Vision)` - Set vision
- `getVisionWidth()` - Get vision width
- `getVisionHeight()` - Get vision height
- `getTouch()` - Get touch signals
- `setTouch(touch: List[float])` - Set touch signals
- `getInternal()` - Get internal signals
- `setInternal(internal: InternalSignals)` - Set internal signals
- `getProprioception()` - Get proprioception signals
- `setProprioception(proprioception: List[float])` - Set proprioception signals
- `getAudio()` - Get audio
- `setAudio(audio: Audio)` - Set audio
- `getAllSignals()` - Get all signals
- `getTimestamp()` - Get timestamp
- `setTimestamp(timestamp: float)` - Set timestamp

## Helper Functions

### nlm.createDefaultConfig()
```python
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 3)
```

### nlm.createBrain(config: Config)
```python
brain = nlm.createBrain(config)
```

### nlm.createSimpleWorld()
```python
world = nlm.createSimpleWorld()
```

### nlm.createAgentBrain(brain: Brain)
```python
agentBrain = nlm.createAgentBrain(brain)
```

## Neuron Type Enumeration

```python
# Neuron types
NeuronType.Excitatory    # Standard excitatory neuron
NeuronType.Inhibitory    # Standard inhibitory neuron
NeuronType.Modulatory     # Neuromodulatory neuron
NeuronType.Sensory       # Sensory processing neuron
NeuronType.Motor         # Motor output neuron
NeuronType.Internal      # Internal processing neuron
```

## Synapse Type Enumeration

```python
# Synapse types
SynapseType.Excitatory   # Standard excitatory synapse
SynapseType.Inhibitory   # Standard inhibitory synapse
SynapseType.Modulatory    # Neuromodulatory synapse
SynapseType.Electrical    # Electrical synapse (gap junction)
SynapseType.GapJunction  # Gap junction synapse
```

## Action Type Enumeration

```python
# Action types
ActionType.MoveForward
ActionType.MoveBackward
ActionType.MoveLeft
ActionType.MoveRight
ActionType.TurnLeft
ActionType.TurnRight
ActionType.Look
ActionType.LookUp
ActionType.LookDown
ActionType.Interact
ActionType.Eat
ActionType.Drink
ActionType.Rest
ActionType.Wait
ActionType.Custom
```

## Motor Command Enumeration

```python
# Motor commands
MotorCommand.MoveForward
MotorCommand.MoveBackward
MotorCommand.TurnLeft
MotorCommand.TurnRight
MotorCommand.LookLeft
MotorCommand.LookRight
MotorCommand.Interact
MotorCommand.Wait
```

## World Object Type Enumeration

```python
# World object types
WorldObjectType.Empty     # Empty space
WorldObjectType.Resource  # Resource node
WorldObjectType.Hazard    # Hazard/danger
WorldObjectType.Wall      # Wall/obstacle
WorldObjectType.Marker    # Marker/landmark
```

## Developmental Stage Enumeration

```python
# Developmental stages
DevelopmentalStage.Initial      # Initial developmental state
DevelopmentalStage.CriticalPeriod  # Critical period for learning
DevelopmentalStage.Maturation    # Maturation/development phase
DevelopmentalStage.Adult        # Adult/stable phase
DevelopmentalStage.Aging        # Aging phase
```

## Firing State Enumeration

```python
# Neuron firing states
FiringState.Resting    # Not firing, at rest
FiringState.Active     # Currently firing
FiringState.Refractory  # In refractory period
FiringState.Inhibited  # Inhibited by external factors
```

## Useful Examples

### Basic Brain Simulation
```python
import nlm

# Create brain configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 2)

# Initialize brain
brain = nlm.Brain(config)
if not brain.initialize():
    print("Failed to initialize brain!")
    exit(1)

# Run simulation
for step in range(100):
    brain.step(step)
    
    # Add some sensory input every 10 steps
    if step % 10 == 0:
        input_data = [0.5, 0.8, 0.3] * 20  # Create 60 values for 20 neurons
        sensory_input = nlm.SensoryInput()
        sensory_input.setData(input_data)
        brain.receiveSensoryInput(sensory_input)

# Print statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Simulation
```python
import nlm

# Create brain and world
config = nlm.Config()
brain = nlm.Brain(config)
world = nlm.SimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=5, visionHeight=5)
world.setAgentStart(10, 10)

# Create agent brain
agentBrain = nlm.AgentBrain(brain)
agentBrain.initialize(world)

# Initialize brain
if not brain.initialize():
    print("Failed to initialize brain!")
    exit(1)

# Run simulation loop
for time in range(200):
    # Get sensory percept from world
    percept = world.getSensoryPercept()
    
    # Process sensory input through agent brain
    agentBrain.processSensoryInput(percept)
    
    # Run brain simulation
    brain.step(time)
    
    # Decode motor command
    motorCommand = agentBrain.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(motorCommand, time)
    
    # Compute reward
    reward = world.computeReward(agentBrain.getAgentBody())
    
    # Apply reward modulation
    agentBrain.applyRewardModulation(reward, 0.0)  # Assuming no predicted reward
    
    # Update development
    agentBrain.updateDevelopment(0.001)
    
    # Log progress every 20 steps
    if time % 20 == 0:
        print(f"Time {time}: Position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"  Energy: {world.getAgentBody().energy:.1f}")
        print(f"  Reward: {reward:.2f}")
        print(f"  Developmental stage: {agentBrain.getDevelopmentalStage()}")
```

### Saving and Loading Brain State
```python
import nlm

# Create and initialize brain
config = nlm.Config()
config.set("neuron_count", 1000)
brain = nlm.Brain(config)
if not brain.initialize():
    print("Failed to initialize brain!")
    exit(1)

# Run some simulation to generate interesting state
for step in range(50):
    brain.step(step)

# Save brain state
if brain.save("/tmp/brain_checkpoint.bin"):
    print("Brain saved successfully")
else:
    print("Failed to save brain")

# Create new brain and load state
brain2 = nlm.Brain(config)
if not brain2.initialize():
    print("Failed to initialize brain2!")
    exit(1)

if brain2.load("/tmp/brain_checkpoint.bin"):
    print("Brain loaded successfully")
    print(f"Loaded neuron count: {brain2.getTotalNeuronCount()}")
else:
    print("Failed to load brain")
```

### Working Memory and Memory Systems
```python
import nlm

# Create brain
config = nlm.Config()
brain = nlm.Brain(config)

if brain.initialize():
    # Get working memory
    workingMemory = brain.getWorkingMemory()
    if workingMemory:
        print(f"Working memory capacity: {workingMemory.getMemoryActivity():.2f}")
    
    # Get episodic memory
    episodicMemory = brain.getEpisodicMemory()
    if episodicMemory:
        recentEpisodes = episodicMemory.getRecentEpisodes(5)
        print(f"Recent episodes: {len(recentEpisodes)}")
    
    # Get associative memory
    associativeMemory = brain.getAssociativeMemory()
    if associativeMemory:
        print(f"Associative memory ready for pattern matching")
```

## Error Handling

The Python bindings raise the following exceptions:

### nlm.RuntimeError
General runtime error for unexpected conditions

### ValueError
Invalid parameter values (e.g., out of range)

### TypeError
Invalid parameter types

```python
try:
    # This will raise ValueError
    brain.step(-1)
except ValueError as e:
    print(f"Error: {e}")
```

## Performance Considerations

### Large Network Simulation
For large networks (1000+ neurons), consider:
1. **Minimize Python overhead**: Use native C++ functions for performance-critical operations
2. **Batch processing**: Process multiple steps in batches when possible
3. **Memory management**: Monitor memory usage for large simulations

### Real-time Applications
For real-time applications:
1. **Fixed timestep**: Use consistent timesteps for predictable performance
2. **Asynchronous processing**: Separate simulation from visualization/analysis
3. **Event-driven**: Focus on important events rather than all steps

### Memory Usage
Memory usage is primarily determined by:
- **Brain size**: Neuron count and connection density
- **Simulation duration**: Longer simulations accumulate more state
- **Checkpoints**: Saved checkpoints consume additional disk space

## Troubleshooting

### "Failed to initialize brain"
Check that configuration parameters are valid:
- `neuron_count` should be > 0
- `region_count` should be > 0
- Other parameters should be within valid ranges

### "ModuleNotFoundError: No module named 'pynlm'"
Ensure Python bindings are built and in Python path:
```bash
export PYTHONPATH=/path/to/pynlm.so:$PYTHONPATH
python -c "import nlm; print(nlm.__version__)"
```

### Memory errors in large simulations
Reduce memory usage by:
- Using fewer neurons
- Reducing connection density
- Using smaller checkpoints

### Performance issues
Optimize by:
- Using native functions instead of Python wrappers
- Minimizing memory allocations
- Using efficient data structures

## License

MIT License
Copyright (c) 2026 NLM Project

## Version

NLM Python Bindings v0.1.0
NLM Core Library v0.1.0

## Acknowledgements

This Python bindings implementation was generated using pybind11 and follows the NLM core library architecture. The implementation maintains compatibility with the existing NLM C++ API while providing a Python-friendly interface for neural simulation research and applications.