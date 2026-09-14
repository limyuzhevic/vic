# Python API Reference Guide

This document provides complete API documentation for the NLM Python bindings.

## Module Overview

The `pynlm` module provides Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework. It enables Python users to create and control artificial brains with spiking neural networks, memory systems, and learning capabilities.

## Importing and Basic Setup

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create a brain
brain = pynlm.createBrain(config)
brain.initialize()
```

## Core Classes

### Config
Configuration management for NLM systems.

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("neuron_count", 1000)
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)

# Get configuration values
neuron_count = config.getOr("neuron_count", 1000)
log_level = config.getOr("log_level", "INFO")

# Check if key exists
if config.has("neuron_count"):
    print(f"Neuron count: {config.get('neuron_count')}")

# Save configuration to file
config.saveToFile("my_config.cfg")

# Load configuration from file
config.loadFromFile("configs/default.cfg")

# Load from command line arguments
config.loadFromArgs(len(sys.argv), sys.argv)

# Get summary
summary = config.summary()
```

### Brain
The main neural simulation brain class with integrated systems.

#### Construction and Initialization

```python
# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize the brain
if brain.initialize():
    print("Brain initialized successfully")
else:
    print("Failed to initialize brain")
```

#### Core Simulation Methods

```python
# Main simulation step with default timestep (1ms)
brain.step(step_number)

# Main simulation step with explicit timestep
brain.step(step_number, current_time)

# Process sensory input from environment
brain.receiveSensoryInput(sensory_input)

# Inject current into a specific neuron
brain.injectCurrent(neuron_id, current_value)

# Inject current into all neurons of a specific type
brain.injectCurrentToNeurons(neuron_type, current_value)
```

#### Action Production and Control

```python
# Produce motor action based on neural activity
action = brain.produceAction()

# Reset brain to initial state
brain.reset()

# Save brain state to checkpoint file
if brain.save("brain_checkpoint.bin"):
    print("Brain state saved successfully")

# Load brain state from checkpoint file
if brain.load("brain_checkpoint.bin"):
    print("Brain state loaded successfully")
```

#### Region Management

```python
# Add a new region to the brain
region_id = brain.addRegion("MyRegion")
region_id = brain.addRegion("")  # Auto-generate name

# Get region by ID
region = brain.getRegion(region_id)

# Get all regions
regions = brain.getRegions()

# Get region count
num_regions = brain.getRegionCount()

# Get all region IDs
region_ids = brain.getRegionIds()
```

#### Statistics and Monitoring

```python
# Global statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Pending events
print(f"Pending spike events: {brain.getPendingSpikeEventCount()}")

# Development stage
stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {stage}")

# Configuration access
config = brain.getConfig()
summary = config.summary()

# Random generator access
rng = brain.getRandomGenerator()
```

#### Plasticity System Access

```python
# Get STDP (Spike-Timing-Dependent Plasticity) system
stdp = brain.getSTDP()

# Get Hebbian plasticity system
hebbian = brain.getHebbian()

# Get structural plasticity system
structural_plasticity = brain.getStructuralPlasticity()

# Get spike system
spike_system = brain.getSpikeSystem()
```

#### Memory System Access

```python
# Working memory (short-term active information)
working_memory = brain.getWorkingMemory()

# Episodic memory (experience storage)
episodic_memory = brain.getEpisodicMemory()

# Associative memory (pattern associations)
associative_memory = brain.getAssociativeMemory()

# Prediction system
prediction_system = brain.getPredictionSystem()

# Cognition systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

# Development system
development_system = brain.getDevelopmentSystem()

# Neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()
```

#### Logging and Status

```python
# Log brain status
brain.logStatus()
```

### AgentBrain
Bridge connecting NLM brain to world environments.

#### Construction and Initialization

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

#### Sensory Processing

```python
# Process sensory percept and inject into brain
agent.processSensoryInput(percept)

# Get expected sensory input size
sensory_size = agent.getSensoryInputSize()

# Get expected motor output size
motor_size = agent.getMotorOutputSize()
```

#### Motor Control

```python
# Decode brain motor activity into motor command
action = agent.decodeMotorCommand()

# Apply neuromodulation based on reward
agent.applyRewardModulation(reward, predicted_reward)

# Update development system
agent.updateDevelopment(timestep)
```

#### Configuration Controls

```python
# Enable/disable learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check status
if agent.isRewardModulationEnabled():
    print("Reward modulation is active")
```

#### Neuromodulation Access

```python
# Get current neuromodulation levels
dopamine = agent.getNeuromodulationLevel()
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()

print(f"Dopamine: {dopamine:.3f}")
print(f"Curiosity: {curiosity:.3f}")
print(f"Novelty: {novelty:.3f}")
print(f"Prediction Error: {prediction_error:.3f}")
```

#### Development Access

```python
# Get current developmental stage
stage = agent.getDevelopmentalStage()
print(f"Current developmental stage: {stage}")
```

#### Reset and Cleanup

```python
# Reset agent for new episode
agent.reset()
```

### SimpleWorld
Simple 2D world for NLM simulation.

#### Construction and Configuration

```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions and sensing
world.configure(width=100, height=100, visionWidth=16, visionHeight=16)

# Set agent start position
world.setAgentStart(50, 50)
```

#### World Interaction

```python
# Update world physics
world.update(timestep)

# Get sensory percept (what the agent sees)
percept = world.getSensoryPercept()

# Get agent body state
agent_body = world.getAgentBody()

# Apply motor command to agent
world.applyMotorCommand(action, current_time)
```

#### World State Queries

```python
# Get world dimensions
width = world.getWidth()
height = world.getHeight()

# Check if position is valid
if world.isValidPosition(x, y):
    print(f"Position ({x}, {y}) is valid")

# Get simulation time
simulation_time = world.getSimulationTime()

# Set random seed for reproducible worlds
world.setRandomSeed(12345)
```

### SensoryInput Classes
Base and derived classes for sensory input data.

#### Vision
Visual sensory input.

```python
# Create vision input
vision = pynlm.Vision(width, height, channels=3)

# Set vision data
vision_data = [[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]]
vision.setData(vision_data)

# Get vision properties
print(f"Vision width: {vision.getWidth()}")
print(f"Vision height: {vision.getHeight()}")
print(f"Vision channels: {vision.getChannels()}")
```

#### Audio
Audio sensory input.

```python
# Create audio input
audio = pynlm.Audio(sample_rate=44100, num_samples=1024)

# Set audio data
audio_data = [0.1, 0.2, 0.3, ...]  # 1024 samples
audio.setData(audio_data)

# Set sample rate
audio.setSampleRate(48000)

# Get audio properties
print(f"Sample rate: {audio.getSampleRate()}")
print(f"Number of samples: {audio.getNumSamples()}")
```

#### InternalSignals
Internal neural signals.

```python
# Create internal signals
internal = pynlm.InternalSignals()

# Add signals
internal.addSignal(0.1)
internal.addSignal(0.5)
internal.addSignal(-0.3)

# Clear signals
internal.clearSignals()
```

### Action
Motor action representation.

```python
# Create action from type
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create action with parameters
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.2, 0.8])

# Get action properties
print(f"Action type: {action.getType()}")
print(f"Action name: {action.getName()}")
print(f"Parameters: {action.getParameters()}")

# Clone action
cloned_action = action.clone()
```

### WorldObject
World object representation.

```python
# Create world object
wall = pynlm.WorldObject(x=10, y=10, type=pynlm.WorldObjectType.Wall, radius=0.5)

# Get object properties
print(f"X: {wall.x}, Y: {wall.y}")
print(f"Type: {wall.type}")
print(f"Radius: {wall.radius}")
print(f"Value: {wall.value}")
print(f"Active: {wall.active}")
```

### AgentBody
Agent body state for movement tracking.

```python
# Create agent body
body = pynlm.AgentBody()

# Set position and orientation
body.x = 50.0
body.y = 50.0
body.orientation = 0.0  # radians

# Set velocity
body.velocityX = 1.0
body.velocityY = 0.5
body.angularVelocity = 0.1

# Set energy and health
body.energy = 100.0
body.health = 100.0
body.age = 0.0

# Get movement state
print(f"Moving: {body.isMoving}")
print(f"Turning: {body.isTurning}")

# Reset body
body.reset()
```

### ActionResult
Result of applying an action to the world.

```python
# Create action result
result = pynlm.ActionResult(reward=10.0, success=True, message="Reached target")

# Get result properties
print(f"Reward: {result.reward}")
print(f"Success: {result.success}")
print(f"Message: {result.message}")
```

### SensoryPercept
Container for all sensory inputs.

```python
# Create percept
percept = pynlm.SensoryPercept()

# Set different sensory modalities
vision = pynlm.Vision(8, 8)
percept.setVision(vision)

# Get percept properties
print(f"Vision width: {percept.getVisionWidth()}")
print(f"Vision height: {percept.getVisionHeight()}")
```

## Enumeration Types

### NeuronType
```python
# Available neuron types
NeuronType.Excitatory
NeuronType.Inhibitory
NeuronType.Modulatory
NeuronType.Sensory
NeuronType.Motor
NeuronType.Internal
```

### SynapseType
```python
# Available synapse types
SynapseType.Excitatory
SynapseType.Inhibitory
SynapseType.Modulatory
SynapseType.Electrical
SynapseType.GapJunction
```

### DevelopmentalStage
```python
# Developmental stages
DevelopmentalStage.Initial
DevelopmentalStage.CriticalPeriod
DevelopmentalStage.Maturation
DevelopmentalStage.Adult
DevelopmentalStage.Aging
```

### FiringState
```python
# Neuron firing states
FiringState.Resting
FiringState.Active
FiringState.Refractory
FiringState.Inhibited
```

### ActionType
```python
# Available action types
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

### MotorCommand
```python
# Motor commands for world interaction
MotorCommand.MoveForward
MotorCommand.MoveBackward
MotorCommand.TurnLeft
MotorCommand.TurnRight
MotorCommand.LookLeft
MotorCommand.LookRight
MotorCommand.Interact
MotorCommand.Wait
```

### WorldObjectType
```python
# World object types
WorldObjectType.Empty
WorldObjectType.Resource
WorldObjectType.Hazard
WorldObjectType.Wall
WorldObjectType.Marker
```

### LogLevel
```python
# Log levels (from Logger.hpp)
LogLevel.Trace
LogLevel.Debug
LogLevel.Info
LogLevel.Warning
LogLevel.Error
LogLevel.Fatal
```

## Constants

### Invalid IDs
```python
# Invalid identifier constants
pynlm.INVALID_NEURON_ID
pynlm.INVALID_SYNAPSE_ID
pynlm.INVALID_REGION_ID
pynlm.INVALID_POPULATION_ID
```

## Error Handling

The Python bindings automatically handle C++ exceptions and convert them to Python exceptions.

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    for step in range(100):
        brain.step(step)
except RuntimeError as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Advanced Usage Patterns

### Configuration Templates

```python
def get_training_config():
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("random_seed", 42)
    config.set("connection_probability", 0.15)
    config.set("simulation_timestep", 0.0005)
    return config

def get_reproduction_config():
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("random_seed", 123)  # Different seed
    config.set("connection_probability", 0.05)
    return config
```

### Batch Processing

```python
def run_multiple_simulations(configs, steps):
    results = []
    
    for i, config in enumerate(configs):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        results.append({
            'run': i,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'final_state': brain.save(f"checkpoint_{i}.bin")
        })
    
    return results
```

### Agent Training Loop

```python
def train_agent(agent, world, num_episodes, steps_per_episode):
    for episode in range(num_episodes):
        # Reset world and agent
        world.reset()
        agent.reset()
        
        # Run episode
        for step in range(steps_per_episode):
            world.update(0.1)
            
            # Sense
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Think
            action = agent.decodeMotorCommand()
            
            # Act
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learn
            reward = calculate_reward(world, action)
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted)
        
        # Log episode results
        if episode % 10 == 0:
            print(f"Episode {episode}: "
                  f"Dopamine={agent.getNeuromodulationLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    return agent
```

## Integration with C++ Extensions

The Python bindings expose advanced C++ functionality through the Python interface. Key features include:

- Direct access to brain internal systems (memory, prediction, cognition)
- Fine-grained control over neuron populations and connectivity
- Integration with NLM's complete development lifecycle
- Access to performance-optimized data structures

## Performance Considerations

1. **Memory Management**: Brain objects are managed by Python garbage collection
2. **Performance**: For high-performance applications, consider direct C++ usage
3. **Threading**: NLM supports parallel processing through C++ implementation
4. **Profiling**: Use NLM's built-in profiling tools for performance analysis

## Examples

Complete example files are available in the examples/ directory or can be found in the demo applications:

- `./nlm_phase6_demo` - Full integration demo
- `./nlm` - Basic neural computation demo
- `examples/` - Additional usage examples

## Migration from C++ to Python

If you're migrating from C++ to Python, note these differences:

1. **Memory Management**: Python handles object lifetime automatically
2. **Error Handling**: Python exceptions instead of C++ exceptions
3. **Type System**: More dynamic in Python
4. **Performance**: Python interface adds overhead but improves developer productivity

## Future Enhancements

The Python bindings are actively developed. Future enhancements may include:

- More detailed type annotations
- Jupyter notebook examples
- Performance profiling tools
- Advanced visualization interfaces
- Machine learning integration

## License

The NLM Python bindings are released under the MIT License.

## Acknowledgments

This Python interface is part of the NLM Phase 6 integration effort, representing a complete implementation of brain-like computation in both C++ and Python.
