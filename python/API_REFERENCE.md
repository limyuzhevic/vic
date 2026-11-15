API Reference Guide for NLM Python Bindings

Version: Phase 6 Complete

This guide provides comprehensive documentation for the NLM Python API, including all classes, methods, and examples for both beginners and advanced users.

---

## Table of Contents

1. Overview
2. Core Classes
   - Config
   - Brain
   - AgentBrain
   - SimpleWorld
3. Sensory Input Classes
4. Motor Classes
5. World Classes
6. Type Enumerations
7. Utility Functions
8. Advanced Features
9. Examples by Complexity
10. Performance Considerations
11. Troubleshooting

---

## 1. Overview

NLM (Neural Learning Machine) is a brain-inspired spiking neural network simulator written in C++20. The Python bindings provide access to:

- Real spiking neural computation with event-driven dynamics
- Multiple memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Cognitive systems (prediction, planning, concept formation)
- Developmental processes
- Social learning capabilities
- Full agent-world interaction

---

## 2. Core Classes

### Config

Configuration management for NLM systems.

#### Methods

- `config = pynlm.Config()`
  Create empty configuration

- `config.loadFromFile("config.json")`
  Load configuration from JSON file

- `config.loadFromArgs(["--neuron_count", "1000", "--connection_probability", "0.1"])`
  Load configuration from command-line arguments

- `config.saveToFile("config.json")`
  Save configuration to JSON file

- `config.set("brain.neuron_count", 1000, nlm.ConfigSource.Runtime)`
  Set configuration value with source

- `config.has("brain.neuron_count")`
  Check if configuration key exists

- `config.get("brain.neuron_count")`
  Get configuration value by key

- `config.getOr("brain.missing_key", "default_value")`
  Get value with fallback default

- `config.getKeys()`
  Get all configuration keys

- `config.remove("brain.neuron_count")`
  Remove configuration key

- `config.clear()`
  Clear all configuration entries

- `config.summary()`
  Get configuration summary

- `str(config)`
  String representation

#### Example Configuration

```python
import pynlm

# Create and configure config
config = pynlm.Config()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)

# Save configuration
config.saveToFile("my_brain_config.json")

# Load configuration
config2 = pynlm.Config()
config2.loadFromFile("my_brain_config.json")
```

### Brain

Central neural simulation brain class with integrated systems.

#### Basic Methods

- `brain = pynlm.createBrain(config)`
  Create brain with configuration

- `brain.initialize()`
  Initialize brain with configuration

- `brain.step(step_number)`
  Perform simulation step

- `brain.step(step_number, timestamp)`
  Perform simulation step with timestamp

- `brain.reset()`
  Reset brain state

#### Sensory Input/Output

- `brain.receiveSensoryInput(percept)`
  Inject sensory input into brain

- `brain.injectCurrent(neuron_id, current)`
  Inject current into specific neuron

- `brain.injectCurrentToNeurons(neuron_type, current)`
  Inject current to all neurons of specific type

- `brain.produceAction()`
  Produce motor action based on neural activity

#### State Management

- `brain.save("checkpoint.bin")`
  Save brain state to file

- `brain.load("checkpoint.bin")`
  Load brain state from file

- `brain.addRegion("cortex")`
  Add new neural region

#### Access Methods

- `region = brain.getRegion(region_id)`
  Get region by ID

- `region_count = brain.getRegionCount()`
  Get number of regions

- `region_ids = brain.getRegionIds()`
  Get all region IDs

- `regions = brain.getRegions()`
  Get all regions

#### Statistics

- `neuron_count = brain.getTotalNeuronCount()`
  Get total neuron count

- `synapse_count = brain.getTotalSynapseCount()`
  Get total synapse count

- `active_count = brain.getActiveNeuronCount()`
  Get active neuron count

- `firing_count = brain.getFiringNeuronCount()`
  Get firing neuron count

- `avg_firing_rate = brain.getAverageFiringRate()`
  Get average firing rate

- `e_i_ratio = brain.getExcitationInhibitionRatio()`
  Get excitation/inhibition ratio

- `total_spikes = brain.getTotalSpikeCount()`
  Get total spike count

- `pending_spikes = brain.getPendingSpikeEventCount()`
  Get pending spike events

#### Development

- `stage = brain.getDevelopmentalStage()`
  Get current developmental stage

- `brain.setDevelopmentalStage(stage)`
  Set developmental stage

#### Systems Access

- `working_memory = brain.getWorkingMemory()`
  Get working memory system

- `episodic_memory = brain.getEpisodicMemory()`
  Get episodic memory system

- `associative_memory = brain.getAssociativeMemory()`
  Get associative memory system

- `prediction_system = brain.getPredictionSystem()`
  Get prediction system

- `planner = brain.getPlanner()`
  Get neural planner

- `concept_formation = brain.getConceptFormation()`
  Get concept formation system

- `attention = brain.getAttention()`
  Get attentional selection system

- `development_system = brain.getDevelopmentSystem()`
  Get development system

- `dopamine = brain.getDopamine()`
  Get dopamine system

- `curiosity = brain.getCuriosity()`
  Get curiosity system

- `novelty = brain.getNovelty()`
  Get novelty system

- `prediction_error = brain.getPredictionErrorSignal()`
  Get prediction error signal

#### Configuration and Random

- `config = brain.getConfig()`
  Get brain configuration

- `random = brain.getRandomGenerator()`
  Get random generator

- `preference = brain.getLearnedPreference()`
  Get learned preference trait (personality)

- `brain.setLearnedPreference(preference)`
  Set learned preference trait

#### Development Operations

- `brain.applyNeuromodulation(signal)`
  Apply neuromodulatory signals

- `brain.updatePlasticity()`
  Update plasticity rules

- `brain.develop()`
  Apply developmental changes

#### Diagnostics

- `brain.logStatus()`
  Log brain status

- `brain.addInterRegionConnection(source, target, weight, delay)`
  Add inter-region connection

- `brain.removeInterRegionConnection(source, target)`
  Remove inter-region connection

### AgentBrain

Interface connecting NLM brain to world simulation.

#### Basic Methods

- `agent = pynlm.createAgentBrain(brain)`
  Create agent brain interface

- `agent.initialize(world)`
  Initialize with world

- `agent.processSensoryInput(percept)`
  Process sensory input and inject into brain

- `action = agent.decodeMotorCommand()`
  Decode brain motor activity into motor command

#### Neuromodulation

- `agent.applyRewardModulation(reward, predicted_reward)`
  Apply reward-based neuromodulation

- `agent.updateDevelopment(timestep)`
  Update development system

#### Configuration

- `agent.enableRewardModulation(True/False)`
  Enable/disable reward modulation

- `agent.enableStructuralPlasticity(True/False)`
  Enable/disable structural plasticity

- `agent.enableDevelopment(True/False)`
  Enable/disable development

- `agent.enableCuriosity(True/False)`
  Enable/disable curiosity

#### State Access

- `stage = agent.getDevelopmentalStage()`
  Get current developmental stage

- `neuromod_level = agent.getNeuromodulationLevel()`
  Get current neuromodulation level

- `curiosity_level = agent.getCuriosityLevel()`
  Get curiosity level

- `novelty_level = agent.getNoveltyLevel()`
  Get novelty level

- `prediction_error = agent.getPredictionError()`
  Get prediction error

#### Advanced Agent Features

- `activities = agent.getMotorGroupActivities()`
  Get motor group activities for analysis

- `agent.observeOtherAgent(observed, before_vision, after_vision)`
  Observe another agent's behavior (Phase 6)

- `internal_rates = agent.getInternalFiringRates()`
  Get internal firing rates for recall probe (Phase 6)

- `planner_overrides = agent.getPlannerOverrideCount()`
  Get planner override count

- `thought_trace = agent.getThoughtTrace()`
  Get thought trace (human-readable record)

- `preference = agent.getLearnedPreference()`
  Get learned preference trait

- `agent.setLearnedPreference(preference)`
  Set learned preference trait

- `reflex_bonus_count = agent.getReflexBonusCount()`
  Get vision-gated reflex bonus count

- `reflex_suppress_count = agent.getReflexSuppressCount()`
  Get hazard suppression count

- `wta_pulse_count = agent.getWtaPulseCount()`
  Get winner-take-all pulse count

#### World Interaction

- `input_size = agent.getSensoryInputSize()`
  Get expected sensory input size

- `output_size = agent.getMotorOutputSize()`
  Get expected motor output size

- `agent.reset()`
  Reset agent for new episode

- `brain_ptr = agent.getBrain()`
  Get underlying brain pointer

### SimpleWorld

Simple 2D world for NLM simulation.

#### Configuration

- `world = pynlm.createSimpleWorld()`
  Create simple world

- `world.configure(width, height, visionWidth, visionHeight)`
  Configure world dimensions

#### Control

- `world.reset()`
  Reset world to initial state

- `world.setAgentStart(x, y)`
  Set agent starting position

- `world.update(timestep)`
  Update world by timestep seconds

- `world.applyMotorCommand(cmd, current_time)`
  Apply motor command to world

#### State Access

- `percept = world.getSensoryPercept()`
  Get sensory percept from world

- `body = world.getAgentBody()`
  Get agent body state

#### World Objects

- `world.addObject(object)`
  Add world object

- `world.removeObject(x, y)`
  Remove object at position

- `world.isValidPosition(x, y)`
  Check if position is valid

#### Properties

- `width = world.getWidth()`
  Get world width

- `height = world.getHeight()`
  Get world height

- `max_energy = world.getMaxEnergy()`
  Get maximum energy

- `world.setMaxEnergy(e)`
  Set maximum energy

- `decay_rate = world.getEnergyDecayRate()`
  Get energy decay rate

- `world.setEnergyDecayRate(r)`
  Set energy decay rate

- `time = world.getSimulationTime()`
  Get simulation time

- `world.setRandomSeed(seed)`
  Set random seed

- `seed = world.getRandomSeed()`
  Get random seed

---

## 3. Sensory Input Classes

### Vision

Visual sensory input with configurable dimensions and data.

- `vision = Vision(width, height, channels=3)`
  Create vision input

- `vision.setData([r,g,b,...])`
  Set vision data

- `width = vision.getWidth()`
  Get vision width

- `height = vision.getHeight()`
  Get vision height

- `channels = vision.getChannels()`
  Get number of channels

### Audio

Audio sensory input with sample rate and samples.

- `audio = Audio(sample_rate, num_samples)`
  Create audio input

- `audio.setData([sample1, sample2, ...])`
  Set audio data

- `audio.setSampleRate(rate)`
  Set sample rate

- `sample_rate = audio.getSampleRate()`
  Get sample rate

- `num_samples = audio.getNumSamples()`
  Get number of samples

### InternalSignals

Internal signal processing for cognitive representation.

- `signals = InternalSignals()`
  Create internal signals

- `signals.addSignal(value)`
  Add signal value

- `signals.clearSignals()`
  Clear all signals

---

## 4. Motor Classes

### Action

Action representation for motor output.

- `action = Action()`
  Create empty action

- `action = Action(action_type)`
  Create action with type

- `action = Action(action_type, [param1, param2, ...])`
  Create action with type and parameters

- `type = action.getType()`
  Get action type

- `action.setType(type)`
  Set action type

- `params = action.getParameters()`
  Get action parameters

- `action.setParameters([params...])`
  Set action parameters

- `name = action.getName()`
  Get action name

- `cloned_action = action.clone()`
  Clone action

### MotorCommand

Low-level motor command enumeration.

Values: MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait

---

## 5. World Classes

### WorldObject

Representation of objects in the simulated world.

- `obj = WorldObject(x, y, type, value=0.0, radius=0.5)`
  Create world object

Properties:
- `x, y`: Position coordinates
- `type`: WorldObjectType (Empty, Resource, Hazard, Wall, Marker)
- `value`: Object value
- `radius`: Object radius
- `active`: Whether object is active

### AgentBody

Agent body state representation.

Properties:
- `x, y`: Position
- `orientation`: Orientation angle
- `velocityX, velocityY`: Linear velocities
- `angularVelocity`: Angular velocity
- `energy`: Current energy
- `health`: Health status
- `age`: Age
- `isMoving`: Whether moving
- `isTurning`: Whether turning
- `lastActionTime`: Time of last action

### ActionResult

Result of applying an action to the world.

- `result = ActionResult(reward, success, message="")`
  Create action result

Properties:
- `reward`: Reward value
- `success`: Whether action succeeded
- `message`: Status message

### SensoryPercept

Complete sensory percept containing all modalities.

Properties:
- `vision`: Vision sensory input
- `touch`: Touch sensory input
- `internal`: Internal signals
- `proprioception`: Proprioception
- `audio`: Audio sensory input

Methods:
- `vision = percept.getVision()`
- `percept.setVision(vision)`
- Similar getters/setters for other modalities

---

## 6. Type Enumerations

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
- ActionType.Interact: Interact with object
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

---

## 7. Utility Functions

### Factory Functions

- `config = pynlm.createDefaultConfig()`
  Create default configuration

- `brain = pynlm.createBrain(config)`
  Create brain with configuration

- `world = pynlm.createSimpleWorld()`
  Create simple world

- `agent = pynlm.createAgentBrain(brain)`
  Create agent brain interface

### Invalid Identifiers

- `pynlm.INVALID_NEURON_ID`
- `pynlm.INVALID_SYNAPSE_ID`
- `pynlm.INVALID_REGION_ID`
- `pynlm.INVALID_POPULATION_ID`

---

## 8. Advanced Features

### Development System

The NLM brain develops through multiple stages:

```python
# Set development stage
brain.setDevelopmentalStage(nlm.DevelopmentalStage.CriticalPeriod)

# Update development each step
agent.updateDevelopment(0.1)  # Update each 0.1 seconds

# Monitor development
stage = agent.getDevelopmentalStage()
```

### Neuromodulation

Control learning through neuromodulators:

```python
# Enable neuromodulation systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Monitor neuromodulation levels
reward_level = agent.getNeuromodulationLevel()
curiosity_level = agent.getCuriosityLevel()
novelty_level = agent.getNoveltyLevel()
```

### Memory Systems

Access integrated memory systems:

```python
# Working memory (persistent activity)
working_memory = brain.getWorkingMemory()

# Episodic memory (experience storage)
episodic_memory = brain.getEpisodicMemory()

# Associative memory (pattern associations)
associative_memory = brain.getAssociativeMemory()
```

### Prediction and Cognition

Use prediction and cognitive systems:

```python
# Prediction system for temporal sequences
prediction_system = brain.getPredictionSystem()

# Neural planner for action planning
planner = brain.getPlanner()

# Concept formation for pattern discovery
concept_formation = brain.getConceptFormation()

# Attentional selection
attention = brain.getAttention()
```

### Social Learning (Phase 6)

Observe and learn from other agents:

```python
# Observe another agent's behavior
agent.observeOtherAgent(
    observed_motor_command,
    before_vision_percept,
    after_vision_percept
)

# Monitor social learning
concept_count = agent.getConceptCount()
self_confidence = agent.getSelfModelConfidence()
social_observations = agent.getSocialObservationCount()
```

### Thought Traces (Phase 5)

Debug and understand agent behavior:

```python
# Get detailed thought trace
thought_trace = agent.getThoughtTrace()
print(f"Thought trace: {thought_trace}")

# Monitor cognitive influence counts
planner_overrides = agent.getPlannerOverrideCount()
concept_biases = agent.getConceptBiasCount()
self_biases = agent.getSelfBiasCount()
social_biases = agent.getSocialBiasCount()
```

### Checkpointing and Persistence

Save and restore brain state:

```python
# Save brain state
brain.save("my_checkpoint.bin")

# Load brain state
brain.load("my_checkpoint.bin")

# Save/load entire configuration
config.saveToFile("config.json")
config.loadFromFile("config.json")
```

---

## 9. Examples by Complexity

### Beginner Level (simple neural activity)

```python
import pynlm

# Simple brain with just neurons
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Let the brain 'think'
for i in range(100):
    brain.step(i)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Intermediate Level (agent in world)

```python
import pynlm

# Setup complete agent
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

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Advanced Level (development and social learning)

```python
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

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run development simulation
for step in range(2000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    agent.updateDevelopment(0.1)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 500 == 0:
        stage = agent.getDevelopmentalStage()
        curiosity = agent.getCuriosityLevel()
        preference = agent.getLearnedPreference()
        print(f"Step {step}: Stage={stage}, Curiosity={curiosity:.3f}, Preference={preference:.3f}")

# Social learning phase
print("\nSocial learning phase:")
teacher_action = pynlm.MotorCommand.MoveForward
before_vision = world.getSensoryPercept().getVision()
agent.observeOtherAgent(teacher_action, before_vision, before_vision)

# Check social learning results
concept_count = agent.getConceptCount()
print(f"Concepts learned: {concept_count}")

# Get final statistics
thought_trace = agent.getThoughtTrace()
print(f"Thought trace: {thought_trace}")
print(f"Planner overrides: {agent.getPlannerOverrideCount()}")
```

---

## 10. Performance Considerations

### Memory Management

- **Large simulations**: Use checkpointing to save/restore state
- **Many agents**: Share underlying brain instances
- **Continuous learning**: Monitor for memory leaks

### Simulation Speed

- **Simple world**: Configure smaller vision dimensions
- **Large brains**: Use structural plasticity to limit growth
- **Complex behaviors**: Use neuromodulation to focus on key features

### Best Practices

1. **Initialize before use**: Always call `brain.initialize()` before stepping
2. **Use timestamps**: Include time in step calls for temporal accuracy
3. **Monitor resources**: Check neuron/synapse counts regularly
4. **Batch operations**: For multiple actions, use efficient methods
5. **Error handling**: Use try-except blocks for file operations

### Common Performance Bottlenecks

- **Vision processing**: Large vision buffers can slow down simulations
- **Memory systems**: Episodic memory grows over time
- **Plasticity**: Heavy structural plasticity can impact performance
- **Development**: Continuous development can increase computational load

---

## 11. Troubleshooting

### Common Issues and Solutions

**Issue**: "My brain isn't doing anything"

**Symptoms**: Zero spikes, no activity

**Solutions**:
1. Verify `brain.initialize()` was called
2. Increase simulation steps (2000+ for warm-up)
3. Inject sensory input: `brain.receiveSensoryInput(percept)`
4. Check neuron parameters in config

**Issue**: "Agent isn't moving"

**Symptoms**: Zero motor activity

**Solutions**:
1. Verify `world.update()` is called each step
2. Check `world.applyMotorCommand()` is used
3. Ensure `agent.decodeMotorCommand()` is working
4. Verify world boundaries and obstacles

**Issue**: "Memory systems not working"

**Symptoms**: Zero memory traces

**Solutions**:
1. Enable required systems: `agent.enableRewardModulation(True)`
2. Check system access: `brain.getWorkingMemory()`
3. Verify configuration parameters
4. Monitor development stage

**Issue**: "Too slow simulation"

**Symptoms**: Low FPS, long step times

**Solutions**:
1. Reduce world dimensions
2. Disable structural plasticity if not needed
3. Use smaller vision buffers
4. Monitor memory usage
5. Consider checkpointing for long runs

### Error Messages

**RuntimeError**: Standard error for C++ runtime issues

**KeyError**: When accessing invalid configuration keys

**FileNotFoundError**: When loading/saving files

**ValueError**: For invalid parameter values

### Debugging Tips

1. **Use thought traces**: `agent.getThoughtTrace()` for behavioral insights
2. **Monitor system counts**: Check neural and synaptic activity
3. **Check developmental stage**: Verify progression through stages
4. **Use checkpointing**: Save intermediate states for debugging
5. **Enable logging**: Use `brain.logStatus()` for diagnostics

### Getting Help

1. **Documentation**: Read `docs/ARCHITECTURE.md` for system details
2. **Examples**: Check `HOW_TO_USE.md` for comprehensive examples
3. **Community**: Join NLM discussion forums
4. **Bug reports**: Report issues with stack traces and reproduction steps

---

## Conclusion

The NLM Python API provides comprehensive access to a sophisticated neural simulation framework. By combining ease of use with advanced features, it supports everything from simple neural demonstrations to complex cognitive architectures.

Key strengths:
- **Modular design**: Easy to integrate specific components
- **Progressive complexity**: Start simple, add features as needed
- **Performance optimization**: Efficient for large-scale simulations
- **Comprehensive documentation**: Clear examples and troubleshooting

The API is designed to be both accessible to beginners and powerful for advanced users, making it suitable for research, education, and development of neural architectures.

---

*Version: Phase 6 Complete*  
*Last Updated: 2026-11-15*  
*License: MIT"