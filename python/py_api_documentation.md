PyNLM Python API Documentation

This is the comprehensive Python API documentation for NLM (Neural Learning Machine), providing detailed reference for all classes, methods, and functions available in the Python bindings.

---

# Python API Documentation

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain simulator. This Python API provides access to the full C++ neural simulation framework through intuitive Python interfaces.

The API is organized into several main modules:

- **Core classes**: Config, Brain, AgentBrain, SimpleWorld
- **Sensory classes**: Vision, Audio, InternalSignals, SensoryPercept
- **Motor classes**: Action, MotorCommand
- **World classes**: WorldObject, AgentBody, ActionResult
- **Type enumerations**: NeuronType, SynapseType, DevelopmentalStage, etc.
- **Utility functions**: Factory methods and helper functions

## Installation

```bash
# Install the package
pip install pynlm

# For development, install in editable mode
pip install -e .
```

## Basic Usage Examples

### Hello World: Simple Neural Activity

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent Simulation

```python
import pynlm

# Setup configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation loop
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Decode motor command
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

## Class Reference

### Config

The configuration class manages all NLM system parameters.

```python
class Config:
    def __init__()
    
    def loadFromFile(filepath)
    def loadFromArgs(args)
    def saveToFile(filepath)
    def has(key)
    def get(key)
    def getOr(key, default_value)
    def getKeys()
    def set(key, value, source=ConfigSource.Runtime)
    def remove(key)
    def clear()
    def summary()
```

**Example Usage:**

```python
import pynlm

# Create configuration
config = pynlm.Config()

# Set basic parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.05)
config.set("brain.v_thresh", -50.0)
config.set("brain.v_rest", -70.0)

# Set plasticity parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Set neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Save configuration
config.saveToFile("brain_config.json")

# Load configuration
config2 = pynlm.Config()
config2.loadFromFile("brain_config.json")
```

### Brain

The central brain class implementing spiking neural computation.

```python
class Brain:
    def __init__(config)
    def initialize()
    def step(step_number)
    def step(step_number, timestamp)
    def receiveSensoryInput(input)
    def injectCurrent(neuron, current)
    def injectCurrentToNeurons(type, current)
    def produceAction()
    def reset()
    def save(filepath)
    def load(filepath)
    def addRegion(name="")
    def getRegion(id)
    def getRegionCount()
    def getRegionIds()
    def getRegions()
    def getTotalNeuronCount()
    def getTotalSynapseCount()
    def getActiveNeuronCount()
    def getFiringNeuronCount()
    def getAverageFiringRate()
    def getExcitationInhibitionRatio()
    def getTotalSpikeCount()
    def getPendingSpikeEventCount()
    def getDevelopmentalStage()
    def setDevelopmentalStage(stage)
    def getConfig()
    def logStatus()
    def applyNeuromodulation(signal)
    def updatePlasticity()
    def develop()
    def getWorkingMemory()
    def getEpisodicMemory()
    def getAssociativeMemory()
    def getPredictionSystem()
    def getPlanner()
    def getConceptFormation()
    def getAttention()
    def getDevelopmentSystem()
    def getDopamine()
    def getCuriosity()
    def getNovelty()
    def getPredictionErrorSignal()
    def getRandomGenerator()
    def getLearnedPreference()
    def setLearnedPreference(preference)
    def addInterRegionConnection(source, target, weight=0.0, delay=1)
    def removeInterRegionConnection(source, target)
```

**Example Usage:**

```python
import pynlm

# Create brain with custom configuration
config = pynlm.Config()
config.set("brain.neuron_count", 1500)
config.set("brain.region_count", 3)
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Inject sensory input (example: vision data)
vision = Vision(8, 8, 3)
vision_data = [0.5, 0.3, 0.8] * 27  # 8x8x3 RGB vision
vision.setData(vision_data)
brain.receiveSensoryInput(vision)

# Run simulation with time tracking
for step in range(500):
    brain.step(step, step * 0.1)  # 0.1 seconds per step
    
    # Check statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

# Create neural regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
print(f"Created region: {region_id}")

# Save brain state
brain.save("brain_checkpoint.bin")
```

### AgentBrain

Interface connecting NLM brain to world simulation.

```python
class AgentBrain:
    def __init__(brain)
    def initialize(world)
    def getSensoryInputSize()
    def getMotorOutputSize()
    def processSensoryInput(percept)
    def decodeMotorCommand()
    def applyRewardModulation(reward, predicted_reward)
    def updateDevelopment(timestep)
    def getDevelopmentalStage()
    def getNeuromodulationLevel()
    def getCuriosityLevel()
    def getNoveltyLevel()
    def getPredictionError()
    def reset()
    def getBrain()
    def enableRewardModulation(enable)
    def enableStructuralPlasticity(enable)
    def enableDevelopment(enable)
    def enableCuriosity(enable)
    def isRewardModulationEnabled()
    def isStructuralPlasticityEnabled()
    def isDevelopmentEnabled()
    def isCuriosityEnabled()
    def getMotorGroupActivities()
    def observeOtherAgent(observed, before_vision, after_vision)
    def getInternalFiringRates()
    def getPlannerOverrideCount()
    def getPlannerAgreementCount()
    def getPlannerDecisionCount()
    def getConceptBiasCount()
    def getSelfBiasCount()
    def getSocialBiasCount()
    def getConceptCount()
    def getSelfModelConfidence()
    def getSocialObservationCount()
    def getThoughtTrace()
    def getLearnedPreference()
    def setLearnedPreference(preference)
    def getReflexBonusCount()
    def getReflexSuppressCount()
    def getWtaPulseCount()
```

**Example Usage:**

```python
import pynlm

# Create complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(7.5, 7.5)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run agent simulation
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Agent makes decision
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation (example reward)
    reward = 1.0 if step % 10 == 0 else 0.0
    agent.applyRewardModulation(reward, 0.5)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 200 == 0:
        stage = agent.getDevelopmentalStage()
        curiosity = agent.getCuriosityLevel()
        print(f"Step {step}: {stage}, Curiosity: {curiosity:.3f}")

# Get agent statistics
thought_trace = agent.getThoughtTrace()
preference = agent.getLearnedPreference()
print(f"Thought trace: {thought_trace}")
print(f"Personality preference: {preference:.3f}")
```

### SimpleWorld

Simple 2D world for NLM simulation.

```python
class SimpleWorld:
    def __init__()
    def configure(width, height, visionWidth, visionHeight)
    def reset()
    def setAgentStart(x, y)
    def update(timestep)
    def applyMotorCommand(cmd, current_time)
    def getSensoryPercept()
    def getAgentBody()
    def addObject(obj)
    def removeObject(x, y)
    def isValidPosition(x, y)
    def getWidth()
    def getHeight()
    def getMaxEnergy()
    def setMaxEnergy(e)
    def getEnergyDecayRate()
    def setEnergyDecayRate(r)
    def getSimulationTime()
    def setRandomSeed(seed)
    def getRandomSeed()
```

**Example Usage:**

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
world.setRandomSeed(42)

# Reset world
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Create agent and brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run world simulation
for step in range(500):
    world.update(0.1)  # Update by 0.1 seconds
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())

# Get world statistics
body = world.getAgentBody()
print(f"Agent position: ({body.x}, {body.y})")
print(f"Agent energy: {body.energy}")
print(f"Agent health: {body.health}")
print(f"Simulation time: {world.getSimulationTime()}")
```

### Vision

Visual sensory input class.

```python
class Vision(SensoryInput):
    def __init__(width, height, channels=3)
    def setData(data)
    def getWidth()
    def getHeight()
    def getChannels()
```

### Audio

Audio sensory input class.

```python
class Audio(SensoryInput):
    def __init__(sample_rate, num_samples)
    def setData(data)
    def setSampleRate(sample_rate)
    def getSampleRate()
    def getNumSamples()
```

### InternalSignals

Base class for internal signals.

```python
class InternalSignals(SensoryInput):
    def __init__()
    def addSignal(value)
    def clearSignals()
```

### Action

Action representation for motor output.

```python
class Action:
    def __init__()
    def __init__(type)
    def __init__(type, parameters)
    def getType()
    def setType(type)
    def getParameters()
    def setParameters(params)
    def getName()
    def clone()
```

### WorldObject

World object representation.

```python
class WorldObject:
    def __init__()
    def __init__(x, y, type, value=0.1, radius=0.5)
```

Properties:
- x, y: Position coordinates
- type: WorldObjectType enum
- value: Object value
- radius: Object radius
- active: Whether object is active

### AgentBody

Agent body state.

Properties:
- x, y: Position
- orientation: Orientation angle
- velocityX, velocityY: Linear velocities
- angularVelocity: Angular velocity
- energy: Current energy
- health: Health status
- age: Age
- isMoving: Whether moving
- isTurning: Whether turning
- lastActionTime: Time of last action

### ActionResult

Result of applying an action.

```python
class ActionResult:
    def __init__(reward, success, message="")
```

Properties:
- reward: Reward value
- success: Whether action succeeded
- message: Status message

### SensoryPercept

Complete sensory percept.

```python
class SensoryPercept:
    def __init__()
    def getVision()
    def setVision(vision)
    def getVisionWidth()
    def getVisionHeight()
    def getTouch()
    def setTouch(touch)
    def getInternal()
    def setInternal(internal)
    def getProprioception()
    def setProprioception(proprioception)
    def getAudio()
    def setAudio(audio)
    def getAllSignals()
    def getTimestamp()
    def setTimestamp(timestamp)
```

## Enumeration Types

### NeuronType
- NeuronType.Excitatory: Excitatory neuron
- NeuronType.Inhibitory: Inhibitory neuron
- NeuronType.Modulatory: Modulatory neuron
- NeuronType.Sensory: Sensory neuron
- NeuronType.Motor: Motor neuron
- NeuronType.Internal: Internal neuron

### SynapseType
- SynapseType.Excitatory: Excitatory synapse
- SynapseType.Inhibitory: Inhibitory synapse
- SynapseType.Modulatory: Modulatory synapse
- SynapseType.Electrical: Electrical synapse
- SynapseType.GapJunction: Gap junction

### DevelopmentalStage
- DevelopmentalStage.Initial: Initial stage
- DevelopmentalStage.CriticalPeriod: Critical period
- DevelopmentalStage.Maturation: Maturation stage
- DevelopmentalStage.Adult: Adult stage
- DevelopmentalStage.Aging: Aging stage

### FiringState
- FiringState.Resting: Resting neuron
- FiringState.Active: Active neuron
- FiringState.Refractory: Refractory neuron
- FiringState.Inhibited: Inhibited neuron

### ActionType
- ActionType.MoveForward: Move forward
- ActionType.MoveBackward: Move backward
- ActionType.MoveLeft: Move left
- ActionType.MoveRight: Move right
- ActionType.TurnLeft: Turn left
- ActionType.TurnRight: Turn right
- ActionType.Look: Look
- ActionType.LookUp: Look up
- ActionType.LookDown: Look down
- ActionType.Interact: Interact
- ActionType.Eat: Eat
- ActionType.Drink: Drink
- ActionType.Rest: Rest
- ActionType.Wait: Wait
- ActionType.Custom: Custom action

### MotorCommand
- MotorCommand.MoveForward: Move forward
- MotorCommand.MoveBackward: Move backward
- MotorCommand.TurnLeft: Turn left
- MotorCommand.TurnRight: Turn right
- MotorCommand.LookLeft: Look left
- MotorCommand.LookRight: Look right
- MotorCommand.Interact: Interact
- MotorCommand.Wait: Wait

### WorldObjectType
- WorldObjectType.Empty: Empty space
- WorldObjectType.Resource: Resource object
- WorldObjectType.Hazard: Hazard object
- WorldObjectType.Wall: Wall
- WorldObjectType.Marker: Marker

### NeuronId, SynapseId, RegionId, PopulationId
Unique identifiers with value, index, and hash capabilities.

## Utility Functions

### Factory Functions

```python
def createDefaultConfig()
    """Create default configuration"""
    
def createBrain(config)
    """Create brain with configuration"""
    
def createSimpleWorld()
    """Create simple world"""
    
def createAgentBrain(brain)
    """Create agent brain interface"""
```

### Invalid Identifiers

```python
INVALID_NEURON_ID
INVALID_SYNAPSE_ID
INVALID_REGION_ID
INVALID_POPULATION_ID
```

## Error Handling

The Python bindings provide comprehensive error handling:

### Common Exceptions

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except FileNotFoundError as e:
    print(f"File not found: {e}")
except ValueError as e:
    print(f"Invalid value: {e}")
except KeyError as e:
    print(f"Key error: {e}")
```

### Error Prevention

1. **Always initialize before use**: `brain.initialize()` before `brain.step()`
2. **Validate inputs**: Check file paths, parameter ranges
3. **Handle exceptions**: Use try-except blocks for file operations
4. **Check preconditions**: Verify configuration before creating components

## Best Practices

### Performance Optimization

1. **Batch operations**: Process multiple actions efficiently
2. **Memory management**: Use checkpointing for long simulations
3. **Configuration caching**: Load configuration once and reuse
4. **Resource monitoring**: Monitor neuron/synapse counts regularly

### Simulation Best Practices

1. **Use timestamps**: Include time in step calls for temporal accuracy
2. **Initialize components**: Always call `initialize()` before use
3. **Monitor development**: Track developmental stage progression
4. **Use thought traces**: Debug agent behavior with `getThoughtTrace()`
5. **Save progress**: Use checkpointing for long-running simulations

### Configuration Best Practices

1. **Use default configurations**: Start with `createDefaultConfig()`
2. **Validate parameters**: Check ranges and constraints
3. **Save configurations**: Store working configurations for reuse
4. **Document settings**: Keep track of configuration changes
5. **Test configurations**: Validate with small simulations before scaling up

## Migration Guide

### From C++ to Python

**C++:**
```cpp
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 1000);
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();
brain->step(0);
```

**Python:**
```python
import pynlm

config = pynlm.Config()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
```

### From Old API to New API

**Old method signatures:**
- `brain.getRegionCount()` → `brain.getRegionCount()` (same)
- `brain.getTotalNeuronCount()` → `brain.getTotalNeuronCount()` (same)

**New methods added:**
- `brain.getPendingSpikeEventCount()` - Check pending spike events
- `brain.getWorkingMemory()` - Access working memory system
- `brain.getEpisodicMemory()` - Access episodic memory system
- `brain.getAssociativeMemory()` - Access associative memory system
- `agent.getThoughtTrace()` - Get human-readable thought trace
- `agent.getMotorGroupActivities()` - Get motor group activities

## Examples by Complexity

### Simple Examples (Beginner)

```python
# Basic neural activity
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Intermediate Examples

```python
# Agent in world
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

### Advanced Examples

```python
# Development and social learning
import pynlm

# Create developing agent
config = pynlm.createDefaultConfig()
config.set("brain.development.enabled", True)
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

for step in range(2000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    agent.updateDevelopment(0.1)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Social learning
agent.observeOtherAgent(
    pynlm.MotorCommand.MoveForward,
    world.getSensoryPercept().getVision(),
    world.getSensoryPercept().getVision()
)

print(f"Thought trace: {agent.getThoughtTrace()}")
```

## Troubleshooting

### Common Issues

**Issue**: "Brain doesn't fire neurons"

**Solution**: Call `brain.initialize()` before simulation

**Issue**: "Agent doesn't move"

**Solution**: Call `world.update()` and `world.applyMotorCommand()`

**Issue**: "Memory systems not working"

**Solution**: Enable required systems with `agent.enableRewardModulation(True)`

**Issue**: "Too slow simulation"

**Solution**: Configure smaller world/vision dimensions

### Debug Tips

1. **Use thought traces**: `agent.getThoughtTrace()` for behavior insights
2. **Monitor statistics**: Check neuron/synapse counts regularly
3. **Use checkpointing**: Save intermediate states for debugging
4. **Enable logging**: Use `brain.logStatus()` for diagnostics
5. **Check development**: Monitor `agent.getDevelopmentalStage()`

## Future Extensions

The NLM Python API is actively developed. Future improvements may include:

1. **Enhanced world simulation**: 3D environments, physics simulation
2. **Advanced sensory modalities**: Touch, proprioception, olfaction
3. **Multi-agent systems**: Multiple interacting agents
4. **Tool integration**: Robot control, computer vision
5. **Cloud deployment**: Distributed computing capabilities
6. **Advanced learning**: Meta-learning, transfer learning
7. **Visualization tools**: Brain activity visualization

## Contributing

This API documentation is maintained as part of the NLM project. Contributions are welcome for:

1. **Documentation improvements**: Add examples, fix errors
2. **API enhancements**: Add new methods and features
3. **Bug fixes**: Fix documentation and code errors
4. **Testing**: Add comprehensive test suites
5. **Performance**: Optimize bindings and documentation

Please refer to the main NLM documentation for contribution guidelines.

---

*Generated automatically from NLM Phase 6 complete implementation*  
*Version: 6.0*  
*Last Updated: 2026-11-15*  
*License: MIT*