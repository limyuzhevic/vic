# NLM - API Reference Guide

**Complete Python Bindings Documentation**

---

## Overview

This document provides comprehensive documentation for all Python bindings of the NLM (Neural Learning Machine) framework. It covers all classes, methods, enumerations, and data structures exposed by the pynlm module.

**Structure:**
- Core Configuration Classes
- Brain and Neural Components
- Agent and World Classes
- Sensory/Motor Processing
- Data Types and Enumerations
- Memory and Memory Systems
- Neuromodulation Systems
- Development Systems
- Utility Functions

---

## Part 1: Core Configuration Classes

### 1.1 Config Class

The `Config` class manages NLM system configuration parameters.

#### Creation and Basic Operations

```python
import pynlm

# Create empty configuration
config = pynlm.Config()

# Create default configuration
config = pynlm.createDefaultConfig()

# Load configuration from JSON file
config.loadFromFile("config.json")

# Load configuration from command line arguments
import sys
config.loadFromArgs(len(sys.argv), sys.argv)

# Save configuration to JSON file
config.saveToFile("config.json")

# Clear all configuration entries
config.clear()
```

#### Configuration Query and Set Methods

```python
# Check if configuration key exists
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Get all configuration keys
all_keys = config.getKeys()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.dopamine.scale", 1.5)

# Get configuration values
if config.has("brain.neuron_count"):
    # Note: Config class doesn't have get() method, use has() to check
    pass
```

#### Configuration Summary

```python
# Get configuration summary string
summary = config.summary()
print(summary)
```

#### Configuration String Representation

```python
# Get string representation
repr_str = repr(config)
print(repr_str)
```

### 1.2 Configuration Parameters

#### Brain Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Plasticity Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.strength` | float | 0.5 | Hebbian learning strength |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.rate` | float | 0.01 | Structural plasticity rate |

#### Neuromodulation Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.sensitivity` | float | 0.7 | Curiosity sensitivity |
| `neuromod.curiosity.exploration_bonus` | float | 0.3 | Curiosity exploration bonus |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.3 | Novelty detection threshold |
| `neuromod.development.enable` | bool | true | Enable developmental neuromodulation |
| `neuromod.development.boost` | float | 1.0 | Developmental boost factor |

---

## Part 2: Brain and Neural Components

### 2.1 Brain Class

The `Brain` class represents the central neural simulation system.

#### Creation and Initialization

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation Control

```python
# Run single simulation step
brain.step(step_number)                    # With step number only
brain.step(step_number, time)              # With step number and timestamp

# Reset brain state
brain.reset()
```

#### Sensory and Motor Processing

```python
# Inject sensory input into brain
sensory_input = pynlm.SensoryInput()  # or specific type like pynlm.Vision()
brain.receiveSensoryInput(sensory_input)

# Inject current into specific neuron
brain.injectCurrent(neuron_id, current_value)

# Inject current into all neurons of specific type
brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, current_value)

# Get motor action from brain
action = brain.produceAction()
```

#### State Management

```python
# Save brain state to file
brain.save("brain_state.bin")

# Load brain state from file
brain.load("brain_state.bin")
```

#### Neural Regions

```python
# Add new neural region
region_id = brain.addRegion("cortex")

# Get region by ID
region = brain.getRegion(region_id)

# Get region count
region_count = brain.getRegionCount()

# Get all region IDs
region_ids = brain.getRegionIds()

# Get all regions
regions = brain.getRegions()
```

#### Neural Statistics

```python
# Get neural counts
total_neurons = brain.getTotalNeuronCount()
total_synapses = brain.getTotalSynapseCount()
active_neurons = brain.getActiveNeuronCount()
firing_neurons = brain.getFiringNeuronCount()

# Get firing rates
average_firing_rate = brain.getAverageFiringRate()
excitation_inhibition_ratio = brain.getExcitationInhibitionRatio()

# Get spike counts
total_spikes = brain.getTotalSpikeCount()

# Get developmental stage
development_stage = brain.getDevelopmentalStage()
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)

# Get configuration
brain_config = brain.getConfig()

# Log brain status
brain.logStatus()
```

#### Specialized Methods

```python
# Get predicted state (for prediction systems)
predicted_state = brain.getPredictedState(step)

# Update prediction based on actual observation
brain.updatePrediction(percept)

# Get curiosity level
brain.getCuriosityLevel()

# Get novelty level
brain.getNoveltyLevel()

# Get prediction error
brain.getPredictionError()

# Get exploration drive
brain.getExplorationDrive()

# Get cognitive load
brain.getCognitiveLoad()

# Get brain energy
brain.getBrainEnergy()
```

### 2.2 Neuron Types and Populations

#### NeuronType Enum

```python
# Neuron type enumeration
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
```

#### PopulationId and RegionId Classes

```python
# Population identifier
population_id = pynlm.PopulationId(123)

# Region identifier
region_id = pynlm.RegionId(456)

# Neuron identifier
neuron_id = pynlm.NeuronId(789)

# Synapse identifier
synapse_id = pynlm.SynapseId(101112)
```

#### Unique Identifier Methods

```python
# Get index from identifier
population_index = population_id.index()
region_index = region_id.index()
neuron_index = neuron_id.index()
synapse_index = synapse_id.index()

# Equality comparison
neuron_id1 == neuron_id2
neuron_id1 != neuron_id2

# Hash for dictionary/set operations
hash_value = hash(neuron_id)

# String representation
str_representation = repr(neuron_id)
```

---

## Part 3: Agent and World Classes

### 3.1 AgentBrain Class

The `AgentBrain` class connects the NLM brain to the world environment.

#### Creation and Initialization

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)
```

#### Sensory Processing

```python
# Process sensory percept and inject into brain
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### Motor Decoding

```python
# Decode brain motor activity into motor command
motor_cmd = agent.decodeMotorCommand()
```

#### Neuromodulation

```python
# Apply reward-based neuromodulation
agent.applyRewardModulation(reward, predicted_reward)
```

#### Development

```python
# Update development system
agent.updateDevelopment(timestep)

# Get current developmental stage
development_stage = agent.getDevelopmentalStage()
```

#### Statistics

```python
# Get neuromodulation level
neuromodulation_level = agent.getNeuromodulationLevel()

# Get curiosity level
curiosity_level = agent.getCuriosityLevel()

# Get novelty level
novelty_level = agent.getNoveltyLevel()

# Get prediction error
prediction_error = agent.getPredictionError()

# Get sensory input size
sensory_input_size = agent.getSensoryInputSize()

# Get motor output size
motor_output_size = agent.getMotorOutputSize()
```

#### Subsystem Control

```python
# Enable/disable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
reward_modulation_enabled = agent.isRewardModulationEnabled()
structural_plasticity_enabled = agent.isStructuralPlasticityEnabled()
development_enabled = agent.isDevelopmentEnabled()
curiosity_enabled = agent.isCuriosityEnabled()
```

#### State Management

```python
# Reset agent for new episode
agent.reset()

# Get underlying brain
underlying_brain = agent.getBrain()
```

### 3.2 SimpleWorld Class

The `SimpleWorld` class represents the simulation environment.

#### Creation and Configuration

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
```

#### World Control

```python
# Reset world to initial state
world.reset()

# Update world by timestep
world.update(timestep)

# Set random seed for reproducible results
world.setRandomSeed(42)
```

#### Agent Interaction

```python
# Set agent starting position
world.setAgentStart(x, y)

# Apply motor command to world
action = pynlm.Action(pynlm.ActionType.MoveForward)
world.applyMotorCommand(action, current_time)
```

#### State Access

```python
# Get sensory percept from world
percept = world.getSensoryPercept()

# Get agent body state
agent_body = world.getAgentBody()
```

#### World Objects

```python
# Create world object
resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=5.0)

# Add object to world
world.addObject(resource)

# Remove object
world.removeObject(x, y)

# Check if position is valid
is_valid = world.isValidPosition(x, y)
```

#### World Properties

```python
# Get world dimensions
width = world.getWidth()
height = world.getHeight()

# Get agent body
agent_body = world.getAgentBody()

# Get simulation time
simulation_time = world.getSimulationTime()

# Get maximum energy
max_energy = world.getMaxEnergy()
world.setMaxEnergy(100.0)

# Get energy decay rate
energy_decay_rate = world.getEnergyDecayRate()
world.setEnergyDecayRate(0.1)
```

---

## Part 4: Sensory and Motor Processing

### 4.1 SensoryInput Base Class

Base class for all sensory inputs.

#### Properties

```python
# Get sensory input type
input_type = sensory_input.getType()

# Get raw data
data = sensory_input.getData()

# Get dimensions
dimensions = sensory_input.getDimensions()

# Get timestamp
timestamp = sensory_input.getTimestamp()
sensory_input.setTimestamp(new_timestamp)
```

### 4.2 Vision Class

Vision sensory input for visual processing.

#### Creation and Configuration

```python
# Create vision input
vision = pynlm.Vision(8, 8, 3)  # width, height, channels

# Set vision data
vision_data = [i * 0.1 for i in range(8 * 8 * 3)]  # 192 values
vision.setData(vision_data)
```

#### Properties

```python
# Get vision dimensions
width = vision.getWidth()
height = vision.getHeight()
channels = vision.getChannels()
```

### 4.3 Audio Class

Audio sensory input for sound processing.

#### Creation and Configuration

```python
# Create audio input
audio = pynlm.Audio(44100, 1024)  # sample rate, num samples

# Set audio data
audio_data = [i * 0.01 for i in range(1024)]  # 1024 samples
audio.setData(audio_data)

# Set sample rate
audio.setSampleRate(44100)
```

#### Properties

```python
# Get sample rate
sample_rate = audio.getSampleRate()

# Get number of samples
num_samples = audio.getNumSamples()
```

### 4.4 InternalSignals Class

Internal signals for internal state representation.

```python
# Create internal signals
internal = pynlm.InternalSignals()

# Add signal values
internal.addSignal(0.5)
internal.addSignal(0.3)
internal.addSignal(0.8)

# Clear all signals
internal.clearSignals()
```

### 4.5 SensoryPercept Class

Combined sensory percept containing all sensory inputs.

#### Properties

```python
# Get individual sensory components
vision = percept.getVision()
touch = percept.getTouch()
internal = percept.getInternal()
proprioception = percept.getProprioception()
audio = percept.getAudio()

# Set individual sensory components
percept.setVision(vision_data)
percept.setTouch(touch_data)
percept.setInternal(internal_data)
percept.setProprioception(proprioception_data)
percept.setAudio(audio_data)

# Get all signals
all_signals = percept.getAllSignals()

# Get timestamp and set timestamp
percept.getTimestamp()
percept.setTimestamp(new_timestamp)
```

### 4.6 Action Class

Motor action output from the brain.

#### Creation and Configuration

```python
# Create action with type
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create action with type and parameters
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.8, 1.0])
```

#### Properties

```python
# Get action type
action_type = action.getType()
action.setType(new_action_type)

# Get and set parameters
parameters = action.getParameters()
action.setParameters(parameters)

# Get action name
name = action.getName()

# Clone action
action_clone = action.clone()
```

### 4.7 WorldObject Class

Representation of objects in the world.

#### Creation and Configuration

```python
# Create world object
world_object = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0, radius=0.5)

# Create simpler object
world_object = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource)
```

#### Properties

```python
# Position
world_object.x = 10.0
world_object.y = 20.0

# Object type
world_object.type = pynlm.WorldObjectType.Resource

# Value
world_object.value = 5.0

# Radius
world_object.radius = 1.0

# Active state
world_object.active = True
```

### 4.8 AgentBody Class

Agent's physical state in the world.

#### Properties

```python
# Position
agent_body.x = 15.0
agent_body.y = 20.0

# Orientation
agent_body.orientation = 0.5

# Velocity
agent_body.velocityX = 0.1
agent_body.velocityY = 0.0

# Angular velocity
agent_body.angularVelocity = 0.0

# Energy
agent_body.energy = 100.0

# Health
agent_body.health = 1.0

# Age
agent_body.age = 5

# Movement states
agent_body.isMoving = True
agent_body.isTurning = False

# Time tracking
agent_body.lastActionTime = 10.5

# Reset agent body
agent_body.reset()
```

---

## Part 5: Data Types and Enumerations

### 5.1 NeuronType Enum

| Enum Value | Description |
|------------|-------------|
| `Excitatory` | Excitatory neuron |
| `Inhibitory` | Inhibitory neuron |
| `Modulatory` | Neuromodulatory neuron |
| `Sensory` | Sensory neuron |
| `Motor` | Motor neuron |
| `Internal` | Internal neuron |

### 5.2 SynapseType Enum

| Enum Value | Description |
|------------|-------------|
| `Excitatory` | Excitatory synapse |
| `Inhibitory` | Inhibitory synapse |
| `Modulatory` | Neuromodulatory synapse |
| `Electrical` | Electrical synapse |
| `GapJunction` | Gap junction synapse |

### 5.3 DevelopmentalStage Enum

| Enum Value | Description |
|------------|-------------|
| `Initial` | Initial developmental stage |
| `CriticalPeriod` | Critical period stage |
| `Maturation` | Maturation stage |
| `Adult` | Adult stage |
| `Aging` | Aging stage |

### 5.4 FiringState Enum

| Enum Value | Description |
|------------|-------------|
| `Resting` | Neuron resting state |
| `Active` | Neuron active state |
| `Refractory` | Neuron refractory state |
| `Inhibited` | Neuron inhibited state |

### 5.5 ActionType Enum

| Enum Value | Description |
|------------|-------------|
| `MoveForward` | Move forward action |
| `MoveBackward` | Move backward action |
| `MoveLeft` | Move left action |
| `MoveRight` | Move right action |
| `TurnLeft` | Turn left action |
| `TurnRight` | Turn right action |
| `Look` | Look action |
| `LookUp` | Look up action |
| `LookDown` | Look down action |
| `Interact` | Interact action |
| `Eat` | Eat action |
| `Drink` | Drink action |
| `Rest` | Rest action |
| `Wait` | Wait action |
| `Custom` | Custom action |

### 5.6 MotorCommand Enum

| Enum Value | Description |
|------------|-------------|
| `MoveForward` | Move forward |
| `MoveBackward` | Move backward |
| `TurnLeft` | Turn left |
| `TurnRight` | Turn right |
| `LookLeft` | Look left |
| `LookRight` | Look right |
| `Interact` | Interact |
| `Wait` | Wait |

### 5.7 WorldObjectType Enum

| Enum Value | Description |
|------------|-------------|
| `Empty` | Empty space |
| `Resource` | Resource object |
| `Hazard` | Hazard object |
| `Wall` | Wall object |
| `Marker` | Marker object |

### 5.8 ActionResult Class

Result of applying an action to the world.

#### Properties

```python
# Reward value
action_result.reward = 1.0

# Success status
action_result.success = True

# Result message
action_result.message = "Collected resource"
```

---

## Part 6: Memory Systems

### 6.1 Memory Class

Base memory system.

#### Memory Operations

```python
# Create memory system
memory = pynlm.Memory()

# Store pattern
memory.store(pattern)

# Retrieve pattern
retrieved_pattern = memory.recall(cue)

# Get memory statistics
memory_stats = memory.getStatistics()
```

### 6.2 NeuralWorkingMemory Class

Working memory for active maintenance of information.

#### Working Memory Operations

```python
# Create working memory
working_memory = pynlm.NeuralWorkingMemory()

# Maintain active representations
working_memory.maintain(representation, importance)

# Update working memory
working_memory.update()

# Get active representations
active_representations = working_memory.getActiveRepresentations()
```

### 6.3 NeuralEpisodicMemory Class

Episodic memory for storing and replaying experiences.

#### Episodic Memory Operations

```python
# Create episodic memory
episodic_memory = pynlm.NeuralEpisodicMemory()

# Encode experience
episodic_memory.encode(experience, context)

# Replay experience
episodic_memory.replay()

# Consolidate memory
episodic_memory.consolidate()

# Get stored episodes
stored_episodes = episodic_memory.getEpisodes()
```

### 6.4 Concept Formation

```python
# Create concept formation system
concept_formation = pynlm.ConceptFormation()

# Extract concepts from patterns
concepts = concept_formation.extract_concepts(patterns)

# Classify new patterns
classification = concept_formation.classify(new_pattern)
```

---

## Part 7: Neuromodulation Systems

### 7.1 Neuromodulator Class

Base neuromodulator system.

#### Neuromodulation Operations

```python
# Create neuromodulator
neuromodulator = pynlm.Neuromodulator()

# Apply neuromodulation
neuromodulator.apply(modulation_signal, target)

# Get neuromodulation level
level = neuromodulator.getLevel()
```

### 7.2 Reward Class

Reward system for reinforcement learning.

#### Reward Operations

```python
# Create reward system
reward = pynlm.Reward()

# Calculate reward
reward_value = reward.calculate(actual_outcome, expected_outcome)

# Apply reward
reward.apply()
```

### 7.3 PredictionError Class

Prediction error system for predictive coding.

#### Prediction Error Operations

```python
# Create prediction error system
prediction_error = pynlm.PredictionError()

# Calculate prediction error
error = prediction_error.calculate(expected, actual)

# Apply prediction error
prediction_error.apply()
```

### 7.4 Curiosity Class

Curiosity system for exploration and discovery.

#### Curiosity Operations

```python
# Create curiosity system
curiosity = pynlm.Curiosity()

# Detect novelty
novelty = curiosity.detect_novelty(percept)

# Generate exploration drive
exploration_drive = curiosity.generate_exploration_drive()
```

### 7.5 Novelty Class

Novelty detection system.

#### Novelty Operations

```python
# Create novelty detector
novelty = pynlm.Novelty()

# Calculate novelty
novelty_score = novelty.calculate(percept)

# Check for significant novelty
is_novel = novelty.is_novel(percept)
```

### 7.6 Dopamine Class

Dopamine neuromodulation system.

#### Dopamine Operations

```python
# Create dopamine system
dopamine = pynlm.Dopamine()

# Apply dopamine modulation
for neuron_id in neuron_ids:
    dopamine.apply(neuron_id, modulation_strength)
```

---

## Part 8: Development Systems

### 8.1 DevelopmentSystem Class

Developmental system for brain maturation.

#### Development Operations

```python
# Create development system
development = pynlm.DevelopmentSystem()

# Update development
development.update(timestep)

# Get current developmental stage
stage = development.getDevelopmentalStage()

# Get development age
age = development.getAge()
```

### 8.2 Synaptogenesis Class

Synaptogenesis system for creating new synapses.

#### Synaptogenesis Operations

```python
# Create synaptogenesis system
synaptogenesis = pynlm.Synaptogenesis()

# Generate new synapses
new_synapses = synaptogenesis.generate_synapses()

# Strengthen existing synapses
synaptogenesis.strengthen_synapses()
```

### 8.3 Pruning Class

Pruning system for eliminating weak synapses.

#### Pruning Operations

```python
# Create pruning system
pruning = pynlm.Pruning()

# Identify synapses to prune
pruning_targets = pruning.identify_pruning_targets()

# Remove weak synapses
pruning.remove_synapses(pruning_targets)
```

### 8.4 Maturation Class

Maturation system for changing neuron properties.

#### Maturation Operations

```python
# Create maturation system
maturation = pynlm.Maturation()

# Mature neurons
maturation.mature_neurons()

# Change neuron properties
maturation.change_properties()
```

---

## Part 9: Prediction Systems

### 9.1 NeuralPrediction Class

Neural prediction system for anticipating future states.

#### Prediction Operations

```python
# Create prediction system
prediction = pynlm.NeuralPrediction()

# Generate predictions
predictions = prediction.generate(percept)

# Update predictions
prediction.update(actual_outcome)

# Get prediction confidence
confidence = prediction.getConfidence()
```

### 9.2 PredictionSystem Class

Complete prediction system.

#### Prediction Operations

```python
# Create prediction system
pred_system = pynlm.PredictionSystem()

# Set prediction horizon
pred_system.setHorizon(50.0)

# Generate action sequences
action_sequences = pred_system.generateActions(current_state)

# Evaluate action sequences
evaluation = pred_system.evaluateActions(action_sequences)
```

---

## Part 10: Cognitive Systems

### 10.1 NeuralPlanner Class

Neural planning system for action sequence evaluation.

#### Planning Operations

```python
# Create neural planner
planner = pynlm.NeuralPlanner()

# Generate plans
plans = planner.generatePlans(current_state)

# Evaluate plans
plan_evaluations = planner.evaluatePlans(plans)

# Select best plan
best_plan = planner.selectBestPlan(plan_evaluations)
```

### 10.2 ConceptFormation Class

Concept formation system for abstraction and categorization.

#### Concept Operations

```python
# Create concept formation system
concept_formation = pynlm.ConceptFormation()

# Extract concepts
concepts = concept_formation.extractConcepts(patterns)

# Categorize patterns
categories = concept_formation.categorize(patterns)

# Update concepts
concept_formation.updateConcepts(new_patterns)
```

---

## Part 11: Utility Functions

### 11.1 Factory Functions

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Create simple world
world = pynlm.createSimpleWorld()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
```

### 11.2 Invalid Identifiers

```python
# Invalid neuron identifier
invalid_neuron_id = pynlm.INVALID_NEURON_ID

# Invalid synapse identifier
invalid_synapse_id = pynlm.INVALID_SYNAPSE_ID

# Invalid region identifier
invalid_region_id = pynlm.INVALID_REGION_ID

# Invalid population identifier
invalid_population_id = pynlm.INVALID_POPULATION_ID
```

### 11.3 Module Information

```python
# Get module version
version = pynlm.__version__

# Get module documentation
docstring = pynlm.__doc__
```

---

## Part 12: Advanced API Usage

### 12.1 Direct Brain Manipulation

```python
import pynlm

# Access brain internals directly
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get direct access to neuron populations
neurons = brain.getNeuronPopulations()

# Direct synapse manipulation
synapses = brain.getSynapses()

# Direct region access
regions = brain.getRegions()
```

### 12.2 Advanced Configuration

```python
# Create highly customized configuration
config = pynlm.createDefaultConfig()

# Set advanced parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.strength", 0.8)
config.set("neuromod.curiosity.sensitivity", 1.0)
config.set("neuromod.novelty.threshold", 0.1)
config.set("neuromod.development.boost", 2.0)

# Apply custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### 12.3 Performance Monitoring

```python
# Monitor brain performance during simulation
for step in range(1000):
    brain.step(step)
    
    # Log performance metrics
    firing_rate = brain.getAverageFiringRate()
    prediction_error = brain.getPredictionError()
    curiosity_level = brain.getCuriosityLevel()
    
    # Collect performance data
    performance_data.append({
        'step': step,
        'firing_rate': firing_rate,
        'prediction_error': prediction_error,
        'curiosity_level': curiosity_level
    })
```

---

## Part 13: Error Handling

### 13.1 Exception Types

```python
# Runtime error
raise pynlm.RuntimeError("Simulation error")

# Invalid configuration error
raise ValueError("Invalid configuration parameter")

# Invalid operation error
raise RuntimeError("Invalid operation")
```

### 13.2 Error Handling

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
    
except ValueError as e:
    print(f"Configuration error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

---

## Part 14: Integration Examples

### 14.1 Complete Agent Lifecycle

```python
import pynlm
import time

def complete_agent_lifecycle():
    """Complete example of agent lifecycle from creation to destruction."""
    
    # Phase 1: Initialization
    print("Phase 1: Initialization")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Phase 2: Learning
    print("Phase 2: Learning")
    for episode in range(100):
        world.reset()
        agent.reset()
        
        for step in range(50):
            world.update(0.05)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, brain.getPredictionError())
    
    # Phase 3: Performance Evaluation
    print("Phase 3: Performance Evaluation")
    final_firing_rate = brain.getAverageFiringRate()
    final_curiosity = agent.getCuriosityLevel()
    
    print(f"Final performance:")
    print(f"  Firing rate: {final_firing_rate:.2f} Hz")
    print(f"  Curiosity level: {final_curiosity:.3f}")
    
    # Phase 4: Checkpointing
    print("Phase 4: Checkpointing")
    brain.save("agent_checkpoint.bin")
    print("Checkpoint saved")
    
    # Phase 5: Recovery
    print("Phase 5: Recovery")
    new_brain = pynlm.createBrain(config)
    new_brain.initialize()
    new_brain.load("agent_checkpoint.bin")
    print("Checkpoint loaded")
    
    # Phase 6: Cleanup
    print("Phase 6: Cleanup")
    # Objects are automatically cleaned up by garbage collection

# Run complete lifecycle
complete_agent_lifecycle()
```

### 14.2 Research Experiment Template

```python
import pynlm
import json
import statistics

def run_research_experiment(experiment_name, config_overrides, num_episodes):
    """
    Template for running research experiments with NLM.
    """
    experiment_config = {
        'name': experiment_name,
        'config': config_overrides,
        'episodes': num_episodes,
        'timestamp': time.time()
    }
    
    # Create experiment configuration
    config = pynlm.createDefaultConfig()
    for key, value in config_overrides.items():
        config.set(key, value)
    
    # Initialize experiment components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run experiment
    experiment_results = {
        'episodes': [],
        'final_metrics': {},
        'config': config_overrides
    }
    
    for episode in range(num_episodes):
        episode_result = run_episode(brain, agent, world, episode)
        experiment_results['episodes'].append(episode_result)
    
    # Calculate final metrics
    experiment_results['final_metrics'] = calculate_metrics(experiment_results['episodes'])
    
    # Save experiment results
    with open(f"{experiment_name}_results.json", 'w') as f:
        json.dump(experiment_results, f, indent=2, default=str)
    
    # Save brain state
    brain.save(f"{experiment_name}_final_brain.bin")
    
    return experiment_results

def run_episode(brain, agent, world, episode):
    """Run a single episode of the experiment."""
    world.reset()
    agent.reset()
    
    episode_data = {
        'episode': episode,
        'steps': 0,
        'total_reward': 0.0,
        'max_curiosity': 0.0,
        'avg_prediction_error': 0.0,
        'final_firing_rate': 0.0
    }
    
    for step in range(50):
        world.update(0.05)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(action_result.reward, brain.getPredictionError())
        agent.updateDevelopment(0.05)
        
        # Update episode data
        episode_data['steps'] += 1
        episode_data['total_reward'] += action_result.reward
        episode_data['max_curiosity'] = max(episode_data['max_curiosity'], agent.getCuriosityLevel())
        episode_data['avg_prediction_error'] += agent.getPredictionError()
        episode_data['final_firing_rate'] = brain.getAverageFiringRate()
    
    # Calculate averages
    episode_data['avg_prediction_error'] /= episode_data['steps']
    
    return episode_data

def calculate_metrics(episode_data):
    """Calculate summary metrics from episode data."""
    if not episode_data:
        return {}
    
    total_reward = [ep['total_reward'] for ep in episode_data]
    curiosity_levels = [ep['max_curiosity'] for ep in episode_data]
    prediction_errors = [ep['avg_prediction_error'] for ep in episode_data]
    firing_rates = [ep['final_firing_rate'] for ep in episode_data]
    
    return {
        'total_reward_mean': statistics.mean(total_reward),
        'total_reward_std': statistics.stdev(total_reward) if len(total_reward) > 1 else 0,
        'curiosity_mean': statistics.mean(curiosity_levels),
        'curiosity_std': statistics.stdev(curiosity_levels) if len(curiosity_levels) > 1 else 0,
        'prediction_error_mean': statistics.mean(prediction_errors),
        'prediction_error_std': statistics.stdev(prediction_errors) if len(prediction_errors) > 1 else 0,
        'firing_rate_mean': statistics.mean(firing_rates),
        'firing_rate_std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
        'learning_curve': total_reward
    }

# Example usage
experiment_config = {
    "brain.neuron_count": 2000,
    "plasticity.stdp.learning_rate": 0.001,
    "neuromod.curiosity.sensitivity": 0.8,
    "neuromod.novelty.threshold": 0.3
}

experiment_results = run_research_experiment(
    "memory_learning_experiment",
    experiment_config,
    200
)
```

---

## Part 15: Advanced Debugging and Testing

### 15.1 Debug Mode

```python
import pynlm

# Enable debug mode
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable debug mode
brain.enableDebugMode(True)

# Run simulation with debug output
for step in range(50):
    brain.step(step)
    
    # Debug output
    print(f"Step {step}:")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Prediction error: {brain.getPredictionError():.3f}")
    print(f"  Curiosity level: {brain.getCuriosityLevel():.3f}")
```

### 15.2 Performance Testing

```python
import pynlm
import time
import statistics

def performance_test():
    """Performance test of NLM simulation."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Performance metrics
    step_times = []
    firing_rates = []
    
    # Run performance test
    start_time = time.time()
    for step in range(100):
        step_start = time.time()
        brain.step(step)
        step_time = time.time() - step_start
        
        step_times.append(step_time)
        firing_rates.append(brain.getAverageFiringRate())
    
    total_time = time.time() - start_time
    
    # Performance analysis
    print(f"Performance Test Results:")
    print(f"  Total time: {total_time:.3f}s")
    print(f"  Average step time: {statistics.mean(step_times)*1000:.2f}ms")
    print(f"  Min step time: {min(step_times)*1000:.2f}ms")
    print(f"  Max step time: {max(step_times)*1000:.2f}ms")
    print(f"  Std dev step time: {statistics.stdev(step_times)*1000:.2f}ms")
    print(f"  Average firing rate: {statistics.mean(firing_rates):.2f} Hz")
    print(f"  Firing rate std: {statistics.stdev(firing_rates):.2f} Hz")
    
    return {
        'total_time': total_time,
        'avg_step_time': statistics.mean(step_times),
        'min_step_time': min(step_times),
        'max_step_time': max(step_times),
        'firing_rate_mean': statistics.mean(firing_rates)
    }

# Run performance test
performance_results = performance_test()
```

---

## Part 16: Future Extensions and API Evolution

### 16.1 API Versioning

```python
# API versioning for future compatibility
pynlm.__version__ = "2.0.0"
pynlm.__api_version__ = "2.0"

# Check API version
if hasattr(pynlm, '__api_version__'):
    api_version = pynlm.__api_version__
    print(f"API version: {api_version}")
```

### 16.2 Future API Plans

#### Planned Features

1. **Enhanced Memory Systems**
   - Meta-memory for memory performance tracking
   - Cross-modal memory integration
   - Hierarchical memory systems

2. **Advanced Neuromodulation**
   - Multiple neuromodulator interactions
   - Contextual neuromodulation
   - Developmental neuromodulation

3. **Predictive Processing**
   - Hierarchical prediction
   - Bayesian brain implementation
   - Multi-level prediction errors

4. **Social Cognition**
   - Theory of mind
   - Social learning
   - Emotional intelligence

5. **Extended API Features**
   - Memory introspection APIs
   - Advanced debugging tools
   - Performance profiling APIs
   - Custom extension points

---

## Conclusion

The NLM Python API provides comprehensive access to the artificial brain system, with over 50 classes and methods for brain simulation, learning, memory, and cognition. The API is designed to be both powerful and accessible, supporting everything from simple demonstrations to complex research experiments.

**Key Features:**
- **Complete Brain Simulation**: LIF neurons, synaptic connections, plasticity mechanisms
- **Advanced Learning**: Reward modulation, structural plasticity, developmental systems
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction and Planning**: Neural prediction, action planning, concept formation
- **Performance Optimization**: Real-time monitoring, profiling, debugging tools
- **Extensibility**: Plugin system, custom components, research extensions

**API Usage Guidelines:**
1. **For beginners**: Use `easy_usage.md` for simple examples
2. **For advanced users**: Use `ADVANCED_USAGE.md` for complex configurations
3. **For research**: Use `HOW_TO_USE.md` for experiment templates
4. **For development**: Use `API_REFERENCE.md` for complete API documentation
5. **For optimization**: Use `PERFORMANCE_GUIDE.md` for performance improvements

The NLM framework continues to evolve, with new features and improvements added in future releases. Users are encouraged to contribute to the project and help shape the future of artificial cognitive systems.

---

*Document Version: 2.0.0*
*API Version: 2.0*
*Last Updated: October 2026*
*NLM Project Team*