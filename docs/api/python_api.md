# NLM Python API Reference

## Overview

The Python API provides simplified access to NLM functionality from Python scripts. It wraps the C++ NLM libraries and provides a more accessible interface for users who prefer Python over C++.

The Python API includes:

1. **Brain Class** - Main brain interface
2. **Config Class** - Configuration management
3. **AgentBrain Class** - Agent interface
4. **SimpleWorld Class** - Environment interface
5. **Memory Systems** - Working memory, episodic memory, associative memory
6. **Prediction System** - Temporal and action prediction
7. **Neuromodulation System** - Reward, curiosity, novelty
8. **Development System** - Developmental processes
9. **Utility Classes** - Various helper classes

## Brain Class

### Brain Class

The `Brain` class is the main interface for NLM brain simulation. It provides methods for creating, initializing, and running brain simulations.

#### Brain.createBrain(config)

**Description**: Creates a new Brain instance with the specified configuration.

**Parameters**:
- `config`: Configuration object (see `Config` class)

**Returns**: Brain instance

**Example**:
```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)

brain = pynlm.createBrain(config)
```

#### Brain.initialize()

**Description**: Initializes the brain and all its subsystems. Must be called before any other operations.

**Returns**: `True` if initialization succeeds, `False` otherwise

**Example**:
```python
brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Failed to initialize brain!")
    return 1
```

#### Brain.step(step_number)

**Description**: Advances the brain simulation by one step.

**Parameters**:
- `step_number`: Current simulation step (used for time tracking and debugging)

**Example**:
```python
for step in range(100):
    brain.step(step)
```

#### Brain.reset()

**Description**: Resets the brain to its initial state while maintaining configuration.

**Example**:
```python
brain.reset()  # Can be called repeatedly
```

#### Brain.save(filename)

**Description**: Saves brain state to file.

**Parameters**:
- `filename`: File path to save to

**Example**:
```python
brain.save("checkpoint.bin")
```

#### Brain.load(filename)

**Description**: Loads brain state from file.

**Parameters**:
- `filename`: File path to load from

**Example**:
```python
brain.load("checkpoint.bin")
```

#### Brain.getTotalNeuronCount()

**Description**: Gets total number of neurons in the brain.

**Returns**: Total neuron count

**Example**:
```python
total_neurons = brain.getTotalNeuronCount()
```

#### Brain.getTotalSynapseCount()

**Description**: Gets total number of synapses in the brain.

**Returns**: Total synapse count

**Example**:
```python
total_synapses = brain.getTotalSynapseCount()
```

#### Brain.getFiringNeuronCount()

**Description**: Gets number of neurons currently firing spikes.

**Returns**: Firing neuron count

**Example**:
```python
firing = brain.getFiringNeuronCount()
```

#### Brain.getAverageFiringRate()

**Description**: Gets average firing rate across all neurons (Hz).

**Returns**: Average firing rate

**Example**:
```python
avg_rate = brain.getAverageFiringRate()
```

#### Brain.getExcitationInhibitionRatio()

**Description**: Gets ratio of excitation to inhibition in the network (E/I ratio).

**Returns**: E/I ratio

**Example**:
```python
ei_ratio = brain.getExcitationInhibitionRatio()
```

#### Brain.getTotalSpikeCount()

**Description**: Gets total number of spikes generated during simulation.

**Returns**: Total spike count

**Example**:
```python
total_spikes = brain.getTotalSpikeCount()
```

#### Brain.getLastUpdateTime()

**Description**: Gets time of last simulation step (ms).

**Returns**: Last update time

**Example**:
```python
last_time = brain.getLastUpdateTime()
```

#### Brain.getDevelopmentalStage()

**Description**: Gets current developmental stage of the brain.

**Returns**: Developmental stage

**Example**:
```python
stage = brain.getDevelopmentalStage()
```

#### Brain.getCurrentPlasticityRate()

**Description**: Gets current plasticity rate for current developmental stage.

**Returns**: Current plasticity rate

**Example**:
```python
plasticity = brain.getCurrentPlasticityRate()
```

#### Brain.getCurrentLearningRate()

**Description**: Gets current learning rate for current developmental stage.

**Returns**: Current learning rate

**Example**:
```python
learning_rate = brain.getCurrentLearningRate()
```

#### Brain.getNeuromodulationLevel(neuromodulator)

**Description**: Gets current level of a specific neuromodulator.

**Parameters**:
- `neuromodulator`: Neuromodulator name ("dopamine", "curiosity", "novelty")

**Returns**: Neuromodulator level (0.0-1.0)

**Example**:
```python
dopamine = brain.getNeuromodulationLevel("dopamine")
curiosity = brain.getNeuromodulationLevel("curiosity")
```

#### Brain.applyRewardModulation(reward, predicted_reward)

**Description**: Applies reward modulation based on received reward.

**Parameters**:
- `reward`: Actual received reward
- `predicted_reward`: Predicted reward (for error calculation)

**Example**:
```python
brain.applyRewardModulation(actual_reward, predicted_reward)
```

#### Brain.getCuriosityLevel()

**Description**: Gets current curiosity level.

**Returns**: Curiosity level (0.0-1.0)

**Example**:
```python
curiosity = brain.getCuriosityLevel()
```

#### Brain.getNoveltyLevel()

**Description**: Gets current novelty level.

**Returns**: Novelty level (0.0-1.0)

**Example**:
```python
novelty = brain.getNoveltyLevel()
```

#### Brain.updateDevelopment(timestep)

**Description**: Advances development by specified timestep.

**Parameters**:
- `timestep`: Time step for development (ms)

**Example**:
```python
brain.updateDevelopment(0.1f)
```

## Config Class

### Config Class

The `Config` class provides configuration management for NLM systems.

#### Config.createDefaultConfig()

**Description**: Creates a default configuration with recommended settings.

**Returns**: Default configuration object

**Example**:
```python
config = pynlm.createDefaultConfig()
```

#### Config.loadFromFile(filename)

**Description**: Loads configuration from file.

**Parameters**:
- `filename`: File path to load from

**Example**:
```python
config.loadFromFile("config.json")
```

#### Config.saveToFile(filename)

**Description**: Saves configuration to file.

**Parameters**:
- `filename`: File path to save to

**Example**:
```python
config.saveToFile("my_config.json")
```

#### Config.has(key)

**Description**: Checks if configuration has the specified key.

**Parameters**:
- `key`: Configuration key (dot-separated path)

**Returns**: `True` if key exists, `False` otherwise

**Example**:
```python
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
```

#### Config.get(key, default=None)

**Description**: Gets configuration value.

**Parameters**:
- `key`: Configuration key
- `default`: Default value if key doesn't exist (optional)

**Returns**: Configuration value

**Example**:
```python
neurons = config.get("brain.neuron_count", 1000)
learning_rate = config.get("plasticity.stdp.learning_rate", 0.001)
```

#### Config.set(key, value)

**Description**: Sets configuration value.

**Parameters**:
- `key`: Configuration key
- `value`: Value to set

**Example**:
```python
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.005)
```

#### Config.getKeys()

**Description**: Gets list of all configuration keys.

**Returns**: List of configuration keys

**Example**:
```python
keys = config.getKeys()
```

## AgentBrain Class

### AgentBrain Class

The `AgentBrain` class provides the interface for sensory-motor integration between the brain and environment.

#### AgentBrain.createAgentBrain(brain)

**Description**: Creates an agent brain interface.

**Parameters**:
- `brain`: Brain instance

**Returns**: AgentBrain instance

**Example**:
```python
agent = pynlm.createAgentBrain(brain)
```

#### AgentBrain.initialize(world)

**Description**: Initializes the agent with a world reference.

**Parameters**:
- `world`: World instance for agent interaction

**Example**:
```python
agent.initialize(world)
```

#### AgentBrain.processSensoryInput(percept)

**Description**: Processes sensory input from the world.

**Parameters**:
- `percept`: Sensory perception data

**Example**:
```python
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### AgentBrain.decodeMotorCommand()

**Description**: Decodes motor command from brain activity.

**Returns**: Selected action based on brain state

**Example**:
```python
action = agent.decodeMotorCommand()
```

#### AgentBrain.applyRewardModulation(reward, predicted_reward)

**Description**: Applies reward modulation through agent interface.

**Parameters**:
- `reward`: Actual received reward
- `predicted_reward`: Predicted reward

**Example**:
```python
agent.applyRewardModulation(reward, predicted_reward)
```

#### AgentBrain.enableRewardModulation(enable)

**Description**: Enables or disables reward modulation.

**Parameters**:
- `enable`: Whether to enable reward modulation

**Example**:
```python
agent.enableRewardModulation(True)
```

#### AgentBrain.enableCuriosity(enable)

**Description**: Enables or disables curiosity system.

**Parameters**:
- `enable`: Whether to enable curiosity

**Example**:
```python
agent.enableCuriosity(True)
```

#### AgentBrain.enableStructuralPlasticity(enable)

**Description**: Enables or disables structural plasticity.

**Parameters**:
- `enable`: Whether to enable structural plasticity

**Example**:
```python
agent.enableStructuralPlasticity(True)
```

#### AgentBrain.enableDevelopment(enable)

**Description**: Enables or disables development.

**Parameters**:
- `enable`: Whether to enable development

**Example**:
```python
agent.enableDevelopment(True)
```

#### AgentBrain.enableCuriosity(enable)

**Description**: Enables or disables curiosity system.

**Parameters**:
- `enable`: Whether to enable curiosity

**Example**:
```python
agent.enableCuriosity(True)
```

#### AgentBrain.getCuriosityLevel()

**Description**: Gets current curiosity level.

**Returns**: Curiosity level (0.0-1.0)

**Example**:
```python
curiosity = agent.getCuriosityLevel()
```

#### AgentBrain.getNoveltyLevel()

**Description**: Gets current novelty level.

**Returns**: Novelty level (0.0-1.0)

**Example**:
```python
novelty = agent.getNoveltyLevel()
```

#### AgentBrain.getPredictionError()

**Description**: Gets current prediction error.

**Returns**: Prediction error

**Example**:
```python
error = agent.getPredictionError()
```

#### AgentBrain.getNeuromodulationLevel(neuromodulator)

**Description**: Gets current level of a specific neuromodulator.

**Parameters**:
- `neuromodulator`: Neuromodulator name

**Returns**: Neuromodulator level (0.0-1.0)

**Example**:
```python
dopamine = agent.getNeuromodulationLevel("dopamine")
```

## SimpleWorld Class

### SimpleWorld Class

The `SimpleWorld` class represents the simulated environment in which NLM agents operate.

#### SimpleWorld.createSimpleWorld()

**Description**: Creates a new simple world instance.

**Returns**: SimpleWorld instance

**Example**:
```python
world = pynlm.createSimpleWorld()
```

#### SimpleWorld.configure(width, height, vision_width, vision_height)

**Description**: Configures world dimensions and agent perception.

**Parameters**:
- `width`: World width
- `height`: World height
- `vision_width`: Agent vision width
- `vision_height`: Agent vision height

**Example**:
```python
world.configure(width=20, height=20, vision_width=8, vision_height=8)
```

#### SimpleWorld.reset()

**Description**: Resets world to initial state.

**Example**:
```python
world.reset()
```

#### SimpleWorld.update(timestep)

**Description**: Updates world simulation by specified timestep.

**Parameters**:
- `timestep`: Time step for update (ms)

**Example**:
```python
world.update(0.1f)
```

#### SimpleWorld.setAgentStart(x, y)

**Description**: Sets agent starting position.

**Parameters**:
- `x`: X coordinate
- `y`: Y coordinate

**Example**:
```python
world.setAgentStart(10.0, 10.0)
```

#### SimpleWorld.applyMotorCommand(action, time)

**Description**: Applies motor command to world.

**Parameters**:
- `action`: Action to execute
- `time`: Current time

**Example**:
```python
world.applyMotorCommand(action, world.getSimulationTime())
```

#### SimpleWorld.getSensoryPercept()

**Description**: Gets sensory percept from world.

**Returns**: Sensory perception data

**Example**:
```python
percept = world.getSensoryPercept()
```

#### SimpleWorld.getAgentBody()

**Description**: Gets agent body representation.

**Returns**: Agent body

**Example**:
```python
body = world.getAgentBody()
```

#### SimpleWorld.isValidPosition(x, y)

**Description**: Checks if position is valid in world.

**Parameters**:
- `x`: X coordinate
- `y`: Y coordinate

**Returns**: Whether position is valid

**Example**:
```python
if world.isValidPosition(x, y):
    world.applyMotorCommand(action, time)
```

#### SimpleWorld.addObject(object)

**Description**: Adds object to world.

**Parameters**:
- `object`: Object to add

**Example**:
```python
world_object = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(world_object)
```

#### SimpleWorld.removeObject(x, y)

**Description**: Removes object from world.

**Parameters**:
- `x`: X coordinate
- `y`: Y coordinate

**Example**:
```python
world.removeObject(x, y)
```

#### SimpleWorld.getWidth()

**Description**: Gets world width.

**Returns**: World width

**Example**:
```python
width = world.getWidth()
```

#### SimpleWorld.getHeight()

**Description**: Gets world height.

**Returns**: World height

**Example**:
```python
height = world.getHeight()
```

#### SimpleWorld.getSimulationTime()

**Description**: Gets current simulation time.

**Returns**: Simulation time

**Example**:
```python
time = world.getSimulationTime()
```

#### SimpleWorld.getMaxEnergy()

**Description**: Gets maximum agent energy.

**Returns**: Maximum energy

**Example**:
```python
energy = world.getMaxEnergy()
```

#### SimpleWorld.setMaxEnergy(energy)

**Description**: Sets maximum agent energy.

**Parameters**:
- `energy`: Maximum energy

**Example**:
```python
world.setMaxEnergy(100.0)
```

## Memory Systems

### WorkingMemory

```python
class WorkingMemory:
    def __init__(self, brain):
        self.brain = brain
        
    def add_pattern(self, pattern, priority=1.0):
        """Add pattern to working memory"""
        
    def retrieve(self, age):
        """Retrieve pattern by age"""
        
    def update_pattern(self, age, new_pattern):
        """Update existing pattern"""
        
    def get_load(self):
        """Get working memory load"""
        
    def get_stats(self):
        """Get working memory statistics"""
```

### EpisodicMemory

```python
class EpisodicMemory:
    def __init__(self, brain):
        self.brain = brain
        
    def store_experience(self, experience):
        """Store episodic experience"""
        
    def recall(self, pattern):
        """Recall experiences by pattern"""
        
    def recall_similar(self, pattern, threshold=0.7):
        """Recall similar experiences"""
        
    def get_stats(self):
        """Get episodic memory statistics"""
```

### AssociativeMemory

```python
class AssociativeMemory:
    def __init__(self, brain):
        self.brain = brain
        
    def form_association(self, trigger, response):
        """Form association between patterns"""
        
    def complete_pattern(self, partial):
        """Complete partial pattern"""
        
    def get_stats(self):
        """Get associative memory statistics"""
```

## Prediction System

### NeuralPrediction

```python
class NeuralPrediction:
    def __init__(self, brain):
        self.brain = brain
        
    def predict_next_events(self, state):
        """Predict next events based on current state"""
        
    def get_prediction_error(self):
        """Get current prediction error"""
        
    def get_prediction_confidence(self):
        """Get prediction confidence"""
        
    def get_prediction_results(self):
        """Get complete prediction results"""
        
    def update_prediction(self, state, target, reward):
        """Update prediction based on actual outcome"""
```

### ActionConsequencePredictor

```python
class ActionConsequencePredictor:
    def __init__(self, brain):
        self.brain = brain
        
    def predict_action_consequences(self, state):
        """Predict consequences of actions"""
        
    def select_best_action(self, consequences):
        """Select best action based on predictions"""
        
    def get_action_prediction_accuracy(self):
        """Get action prediction accuracy"""
```

## Neuromodulation System

### NeuromodulationManager

```python
class NeuromodulationManager:
    def __init__(self, brain):
        self.brain = brain
        self.dopamine_modulator = DopamineModulator(brain)
        self.curiosity_modulator = CuriosityModulator(brain)
        self.novelty_detector = NoveltyDetector(brain)
        self.attention_modulator = AttentionModulator(brain)
        
    def get_neuromodulation_level(self, neuromodulator):
        """Get level of specific neuromodulator"""
        
    def apply_reward_modulation(self, reward, predicted_reward):
        """Apply reward modulation"""
        
    def enable_neuromodulation(self, neuromodulator, enable):
        """Enable or disable specific neuromodulator"""
        
    def get_neuromodulation_stats(self):
        """Get neuromodulation statistics"""
```

### DopamineModulator

```python
class DopamineModulator:
    def __init__(self, brain):
        self.brain = brain
        
    def get_dopamine_level(self):
        """Get current dopamine level"""
        
    def update_dopamine(self, reward, predicted_reward):
        """Update dopamine based on reward prediction error"""
        
    def get_prediction_error(self):
        """Get prediction error"""
```

### CuriosityModulator

```python
class CuriosityModulator:
    def __init__(self, brain):
        self.brain = brain
        
    def get_curiosity_level(self):
        """Get current curiosity level"""
        
    def update_curiosity(self, entropy, novelty):
        """Update curiosity based on entropy and novelty"""
        
    def get_exploration_drive(self):
        """Get exploration drive"""
```

### NoveltyDetector

```python
class NoveltyDetector:
    def __init__(self, brain):
        self.brain = brain
        
    def detect_novelty(self, input):
        """Detect novelty in input pattern"""
        
    def get_novelty_level(self):
        """Get current novelty level"""
```

### AttentionModulator

```python
class AttentionModulator:
    def __init__(self, brain):
        self.brain = brain
        
    def get_attention_level(self):
        """Get current attention level"""
        
    def allocate_attention(self, stimuli):
        """Allocate attention across stimuli"""
        
    def get_selection_strength(self):
        """Get attention selection strength"""
        
    def get_inhibition_strength(self):
        """Get attention inhibition strength"""
```

## Development System

### DevelopmentManager

```python
class DevelopmentManager:
    def __init__(self, brain):
        self.brain = brain
        self.development_system = DevelopmentSystem(brain)
        self.structural_plasticity = StructuralPlasticity(brain)
        self.life_stage_behavior = LifeStageBehavior(brain)
        self.critical_period_manager = CriticalPeriodManager(brain)
        
    def update_development(self, timestep):
        """Update development"""
        
    def get_developmental_progress(self):
        """Get developmental progress"""
        
    def enable_experience_dependent_development(self, enable):
        """Enable or disable experience-dependent development"""
```

### DevelopmentSystem

```python
class DevelopmentSystem:
    def __init__(self, brain):
        self.brain = brain
        
    def update_development(self, timestep):
        """Update development by timestep"""
        
    def get_current_stage(self):
        """Get current developmental stage"""
        
    def get_developmental_progress(self):
        """Get developmental progress"""
        
    def enable_critical_periods(self, enable):
        """Enable or disable critical periods"""
```

### StructuralPlasticity

```python
class StructuralPlasticity:
    def __init__(self, brain):
        self.brain = brain
        
    def grow_new_synapse(self, location):
        """Create new synapse"""
        
    def prune_synapse(self, location):
        """Prune synapse"""
        
    def get_synaptic_growth_rate(self):
        """Get synaptic growth rate"""
        
    def get_synaptic_pruning_rate(self):
        """Get synaptic pruning rate"""
```

### LifeStageBehavior

```python
class LifeStageBehavior:
    def __init__(self, brain):
        self.brain = brain
        
    def get_current_behavior(self):
        """Get current behavior"""
        
    def get_exploration_rate(self):
        """Get exploration rate"""
        
    def get_exploitation_rate(self):
        """Get exploitation rate"""
        
    def select_action(self, possible_actions):
        """Select action based on life-stage behavior"""
```

### CriticalPeriodManager

```python
class CriticalPeriodManager:
    def __init__(self, brain):
        self.brain = brain
        
    def is_in_critical_period(self):
        """Check if in critical period"""
        
    def get_critical_period_sensitivity(self):
        """Get critical period sensitivity"""
        
    def get_critical_period_intensity(self):
        """Get critical period intensity"""
```

## Utility Classes

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
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
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Sleep
pynlm.ActionType.Explore
pynlm.ActionType.Investigate

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

### WorldObject Class

```python
class WorldObject:
    def __init__(self, x, y, object_type, value=1.0):
        self.x = x
        self.y = y
        self.type = object_type
        self.value = value
```

### Constants

```python
# Simulation constants
pynlm.SIMULATION_TIME_STEP = 0.1
pynlm.DEFAULT_WORLD_SIZE = 20
pynlm.DEFAULT_VISION_RANGE = 5
pynlm.MAX_ENERGY = 100.0
```

## Common Usage Patterns

### Pattern 1: Basic Brain Simulation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
    if step % 25 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Analyze results
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Pattern 2: Complete Agent-World System

```python
import pynlm

def run_agent_simulation():
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run episode
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = 1.0 if step % 50 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"Step {step}: Curiosity={curiosity:.2f}, Novelty={novelty:.2f}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation()
```

### Pattern 3: Memory-Episodic Learning

```python
import pynlm

# Create brain with episodic memory
config = pynlm.createDefaultConfig()
config.set("memory.episodic.enable", True)
config.set("memory.episodic.max_episodes", 50)

brain = pynlm.createBrain(config)
brain.initialize()

# Store experiences
for step in range(100):
    brain.step(step)
    
    # Create experience
    experience = {
        "id": step,
        "sensory_input": [0.5, 0.7, 0.3, 0.8, 0.4],
        "actions": [pynlm.ActionType.MoveForward, pynlm.ActionType.Interact],
        "rewards": [0.1, 1.0],
        "timestamp": step,
        "context": "exploration",
        "stage": brain.getDevelopmentalStage(),
    }
    
    # Store experience (Python API integration with C++)
    brain.storeEpisodicMemory(experience)
    
    # Attempt recall
    search_pattern = [0.5, 0.7, 0.3, 0.8, 0.4]
    recalled = brain.recallFromEpisodicMemory(search_pattern)
    
    if recalled:
        print(f"Step {step}: Recalled experience {recalled['id']}")

# Analyze memory statistics
stats = brain.getEpisodicMemoryStats()
print(f"Episodes stored: {stats['episodesStored']}")
print(f"Recall accuracy: {stats['recallAccuracy']}")
```

### Pattern 4: Prediction-Based Planning

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("prediction.enable", True)
config.set("prediction.horizon", 5)

brain = pynlm.createBrain(config)
brain.initialize()

# Make predictions and plan actions
for step in range(50):
    # Current state
    current_state = [0.5, 0.5, 0.5]  # Simplified state
    
    # Predict future events
    predictions = brain.predictNextEvents(current_state)
    
    # Select action based on predictions
    action = select_action_based_on_predictions(predictions)
    
    # Execute action
    brain.step(step)
    
    # Learn from prediction errors
    prediction_error = brain.getPredictionError()
    brain.applyPredictionErrorLearning(prediction_error)
    
    print(f"Step {step}: Predicted {predictions}, Error: {prediction_error:.3f}")
```

## Python API Reference Summary

| Class | Key Methods | Description |
|-------|-------------|-------------|
| `Brain` | `createBrain()`, `initialize()`, `step()`, `reset()`, `save()`, `load()` | Main brain interface |
| `Config` | `createDefaultConfig()`, `loadFromFile()`, `saveToFile()`, `set()`, `get()`, `has()` | Configuration management |
| `AgentBrain` | `createAgentBrain()`, `initialize()`, `processSensoryInput()`, `decodeMotorCommand()`, `applyRewardModulation()` | Agent interface |
| `SimpleWorld` | `createSimpleWorld()`, `configure()`, `reset()`, `update()`, `setAgentStart()`, `applyMotorCommand()`, `getSensoryPercept()` | Environment interface |
| `WorkingMemory` | `add_pattern()`, `retrieve()`, `update_pattern()`, `get_load()`, `get_stats()` | Working memory system |
| `EpisodicMemory` | `store_experience()`, `recall()`, `recall_similar()`, `get_stats()` | Episodic memory system |
| `AssociativeMemory` | `form_association()`, `complete_pattern()`, `get_stats()` | Associative memory system |
| `NeuralPrediction` | `predict_next_events()`, `get_prediction_error()`, `get_prediction_confidence()`, `update_prediction()` | Temporal prediction |
| `ActionConsequencePredictor` | `predict_action_consequences()`, `select_best_action()`, `get_action_prediction_accuracy()` | Action consequence prediction |
| `NeuromodulationManager` | `get_neuromodulation_level()`, `apply_reward_modulation()`, `enable_neuromodulation()`, `get_neuromodulation_stats()` | Neuromodulation system |
| `DevelopmentManager` | `update_development()`, `get_developmental_progress()`, `enable_experience_dependent_development()` | Development system |

## Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
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
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Sleep
pynlm.ActionType.Explore
pynlm.ActionType.Investigate

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
pynlm.WorldObjectType.Agent
```

## Constants

```python
# Simulation constants
pynlm.SIMULATION_TIME_STEP = 0.1
pynlm.DEFAULT_WORLD_SIZE = 20
pynlm.DEFAULT_VISION_RANGE = 5
pynlm.MAX_ENERGY = 100.0
```

## Next Steps

1. **Get Started**: Try the basic examples in `docs/examples/basic_usage.cpp`
2. **Explore Documentation**: Read `HOW_TO_USE.md` for Python API details
3. **Run Demos**: Execute the demo programs (`nlm_phase3_demo`, `nlm_phase4_demo`, `nlm_phase6_demo`)
4. **Create Experiments**: Design and run your own NLM experiments
5. **Extend NLM**: Add new features and capabilities to NLM

The Python API provides comprehensive access to NLM's neural simulation capabilities. Whether you're interested in basic simulation, memory systems, prediction, neuromodulation, development, or action planning, the Python API provides the tools you need to create sophisticated neural experiments in a more accessible interface.

---

*Python API Reference*
- **Location**: `docs/api/python_api.md`
- **Source**: `python/bindings.cpp`
- **Documentation**: Complete Python API reference with examples

*Ready to start using the Python API?*

1. **Read the examples**: Check `docs/examples/basic_usage.cpp`
2. **Try Python API**: Use `HOW_TO_USE.md` for Python examples
3. **Run demo programs**: Execute the built-in demo programs
4. **Create your own**: Design custom NLM experiments
5. **Contribute**: Share your Python API usage with the community

The Python API is the foundation of NLM's accessibility and ease of use. It provides everything you need to work with NLM neural systems in a more familiar Python programming environment!

---

*Python API enables NLM to:
• Provide accessible neural simulation interface
• Simplify configuration and setup
• Enable rapid prototyping and experimentation
• Support research and education use cases
• Maintain compatibility with C++ core functionality

Your Python API research contributions will help advance neural system accessibility!

---