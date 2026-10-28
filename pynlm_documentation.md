# NLM Python Bindings - Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings. These bindings expose the full C++ NLM framework functionality through a Pythonic interface, providing advanced neuroscience research capabilities with Python ease of use.

## Installation

### Prerequisites

- Python 3.8 or higher
- Build system with CMake
- pybind11 library
- C++ compiler with C++20 support

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd nlm-python-bindings

# Build using scikit-build (recommended)
pip install .

# Or build manually
mkdir build
cd build
cmake ..
make -j4
pip install .
```

### Installation from Binary

If pre-built wheels are available, you can install directly:

```bash
pip install pynlm
```

## Quick Start Guide

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.create_default_config()

# Create a brain
brain = pynlm.createBrain(config)

# Create a world
world = pynlm.createSimpleWorld()
world.configure(100, 100, 32, 32)  # width, height, vision_width, vision_height

# Create an agent brain
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run a simulation step
brain.step(1)

# Get sensory input
percept = world.getSensoryPercept()
vision = percept.getVision()

# Get motor command
action = agent_brain.decodeMotorCommand()
```

### Advanced Usage

```python
# Create comprehensive setup
config = pynlm.create_large_scale_config()
setup = pynlm.create_complete_agent_setup(config)

brain = setup['brain']
world = setup['world']
agent_brain = setup['agent_brain']

# Configure and run agent
agent_brain.initialize(world)
state = agent_brain.get_agent_state()

# Run episode
episode_config = {'max_steps': 100, 'max_time': 10.0}
reward, steps = agent_brain.run_agent_episode(world, episode_config)

# Get system status
performance = brain.get_performance_stats()
memory_status = brain.get_memory_system_status()
neuromodulation = brain.get_neuromodulation_system()
```

## API Documentation

### Core Classes

#### Config
Configuration management with Pythonic dict-like interface.

**Properties:**
- `keys`: Get all configuration keys
- `has(key)`: Check if key exists
- `get(key, default)`: Get value with default
- `set(key, value)`: Set value
- `to_dict()`: Convert to Python dictionary
- `from_dict(dict)`: Load from dictionary
- `to_json()`: Convert to JSON string
- `from_json(json)`: Load from JSON string

**Methods:**
- `loadFromFile(filepath)`: Load from JSON file
- `saveToFile(filepath)`: Save to JSON file
- `loadFromArgs(argc, argv)`: Load from command line arguments
- `clear()`: Clear all configuration
- `summary()`: Get configuration summary
- `validate()`: Validate configuration

#### Brain
Central neural simulation brain class.

**Properties:**
- `spike_system`: Access to spike system
- `working_memory`: Access to working memory
- `episodic_memory`: Access to episodic memory
- `associative_memory`: Access to associative memory
- `prediction_system`: Access to prediction system
- `planner`: Access to neural planner
- `concept_formation`: Access to concept formation
- `attention`: Access to attentional selection
- `development_system`: Access to development system
- `dopamine`: Access to dopamine neuromodulator
- `curiosity`: Access to curiosity neuromodulator
- `novelty`: Access to novelty neuromodulator
- `prediction_error`: Access to prediction error

**Methods:**
- `step(step)`: Perform simulation step
- `step(step, time)`: Perform simulation step with time
- `receiveSensoryInput(input)`: Inject sensory input
- `injectCurrent(neuron, current)`: Inject current to neuron
- `injectCurrentToNeurons(type, current)`: Inject current to neuron type
- `produceAction()`: Produce motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `addRegion(name)`: Add neural region
- `getRegion(id)`: Get region by ID
- `getTotalNeuronCount()`: Get total neuron count
- `getTotalSynapseCount()`: Get total synapse count

#### SimpleWorld
Simple 2D world for NLM simulation.

**Properties:**
- `width`, `height`: World dimensions
- `vision_width`, `vision_height`: Vision dimensions

**Methods:**
- `configure(width, height, vision_width, vision_height)`: Configure world
- `reset()`: Reset world
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world
- `applyMotorCommand(cmd, currentTime)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body
- `addObject(obj)`: Add world object
- `removeObject(x, y)`: Remove object
- `isValidPosition(x, y)`: Check valid position

#### AgentBrain
Connects NLM brain to the world, handling sensory transduction and motor decoding.

**Properties:**
- `sensory_input_size`: Expected sensory input size
- `motor_output_size`: Expected motor output size

**Methods:**
- `initialize(world)`: Initialize with world
- `getSensoryInputSize()`: Get sensory input size
- `getMotorOutputSize()`: Get motor output size
- `processSensoryInput(percept)`: Process sensory percept
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predictedReward)`: Apply reward modulation
- `updateDevelopment(timestep)`: Update development system
- `getNeuromodulationLevel()`: Get neuromodulation level
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error
- `reset()`: Reset agent for new episode
- `getBrain()`: Get underlying brain

#### SensoryPercept
Sensory data container.

**Methods:**
- `getVision()`, `setVision(data)`: Vision data
- `getTouch()`, `setTouch(data)`: Touch data
- `getInternal()`, `setInternal(data)`: Internal data
- `getProprioception()`, `setProprioception(data)`: Proprioception data
- `getAudio()`, `setAudio(data)`: Audio data
- `getAllSignals()`: Get all signals
- `getTimestamp()`, `setTimestamp(timestamp)`: Timestamp
- `hasVision()`, `hasTouch()`, etc.: Check sensory availability
- `getAllSensoryData()`: Get all data as dictionary
- `setSensoryData(dict)`: Set data from dictionary
- `getSensorySummary()`: Get human-readable summary

### Neuromodulation Systems

#### Dopamine
Reward and reinforcement learning signal.

**Methods:**
- `getLevel()`: Get current level
- `setLevel(level)`: Set level
- `signalReward(reward)`: Signal reward
- `signalRewardPredictionError(error)`: Signal reward prediction error
- `getPlasticityFactor()`: Get plasticity factor
- `update(dt)`: Update neuromodulator

#### Curiosity
Exploration motivation based on novelty and prediction error.

**Methods:**
- `getLevel()`: Get curiosity level
- `getExplorationDrive()`: Get exploration drive
- `update(novelty, predictionError, dt)`: Update curiosity
- `setNoveltyWeight(weight)`: Set novelty weight
- `setPredictionErrorWeight(weight)`: Set prediction error weight
- `reset()`: Reset curiosity

#### Novelty
Novelty detection signal.

**Methods:**
- `getLevel()`: Get novelty level
- `setLevel(level)`: Set level
- `detectNovelty(observation, previousObservation)`: Detect novelty
- `detectNovelty(currentPattern, previousPattern)`: Detect from patterns
- `update(dt)`: Update novelty
- `getHistory()`: Get novelty history
- `clearHistory()`: Clear history

#### PredictionError
Prediction error signal for curiosity and learning.

**Methods:**
- `getError()`: Get error value
- `computeError(predicted, actual)`: Compute error
- `updatePrediction(newPrediction)`: Update prediction
- `getHistory()`: Get error history
- `clearHistory()`: Clear history
- `getMagnitude()`: Get error magnitude

#### Reward
Reward signal for reinforcement learning.

**Methods:**
- `getValue()`: Get current reward value
- `setValue(value)`: Set value
- `add(delta)`: Add to reward
- `reset()`: Reset reward
- `computeReward(observation)`: Compute reward from observation
- `getHistory()`: Get reward history
- `clearHistory()`: Clear history

### Memory Systems

#### NeuralWorkingMemory
Working memory using persistent neural activity.

**Methods:**
- `store(pattern, strength)`: Store pattern
- `storeToNeuron(neuron, activation)`: Store to neuron
- `retrieve()`: Retrieve current content
- `contains(neuron)`: Check if neuron in memory
- `getNeuronActivation(neuron)`: Get neuron activation
- `update(dt)`: Update memory
- `clear()`: Clear memory
- `getActiveTraces()`: Get active trace count
- `getCapacity()`: Get memory capacity
- `setCapacity(cap)`: Set capacity

#### NeuralEpisodicMemory
Episodic memory storing experiences.

**Methods:**
- `storeEpisode(episode)`: Store episode
- `retrieveSimilar(pattern, maxResults)`: Retrieve similar episodes
- `retrieveTemporal(startTime, endTime, maxResults)`: Retrieve from time window
- `retrieveByLocation(x, y, radius, maxResults)`: Retrieve by location
- `retrieveAfterAction(action, maxResults)`: Retrieve after action
- `replayEpisode(episode)`: Replay episode
- `updateRelevance(episodeId, relevanceDelta)`: Update relevance
- `getEpisode(index)`: Get episode by index
- `getEpisodeCount()`: Get episode count
- `getRecentEpisodes(count)`: Get recent episodes

#### NeuralAssociativeMemory
Associative memory for pattern relationships.

**Methods:**
- `associate(patternA, patternB, strength)`: Create association
- `associateFromExperience(episode)`: Create from episode
- `retrieve(queryPattern, maxResults)`: Retrieve associated patterns
- `getAssociationStrength(patternA, patternB)`: Get association strength
- `updateAssociation(patternA, patternB, delta)`: Update association
- `spreadActivation(cuePattern, steps)`: Spread activation
- `clear()`: Clear associations

### Cognition Systems

#### NeuralPlanner
Plans multi-step actions using learned predictions.

**Methods:**
- `planAction(currentState, targetReward)`: Plan next action
- `evaluateSequence(actions, startState)`: Evaluate action sequence
- `getPlanningDepth()`: Get planning depth
- `setPlanningDepth(depth)`: Set planning depth
- `getPlanningConfidence()`: Get planning confidence
- `updatePlanQuality(plannedActions, actualActions, actualReward)`: Update plans
- `clearCache()`: Clear planning cache
- `setActionQuality(action, quality)`: Set action quality

#### ConceptFormation
Discovers recurring patterns from experience.

**Methods:**
- `presentExperience(pattern, features, reward, currentTime)`: Present experience
- `getMatchingConcept(pattern, similarityThreshold)`: Get matching concept
- `getConcept(conceptId)`: Get concept by ID
- `getConceptCount()`: Get concept count
- `mergeConcepts(conceptA, conceptB)`: Merge concepts

### Development Systems

#### DevelopmentSystem
Orchestrates neural development.

**Methods:**
- `getStage()`: Get current developmental stage
- `setStage(stage)`: Set developmental stage
- `advanceStage()`: Advance developmental stage
- `getStageName()`: Get stage name
- `update(brain, currentStep)`: Update development
- `update(brain, rng, dt)`: Update with random generator
- `getPlasticityModifier()`: Get plasticity modifier
- `isCriticalPeriod()`: Check if critical period
- `getCriticalPeriodProgress()`: Get critical period progress

#### Synaptogenesis
Formation of new synapses.

**Methods:**
- `getFormationRate()`: Get formation rate
- `setFormationRate(rate)`: Set formation rate
- `getTargetDensity()`: Get target density
- `setTargetDensity(density)`: Set target density

#### Pruning
Removal of weak synapses.

**Methods:**
- `getThreshold()`: Get pruning threshold
- `setThreshold(threshold)`: Set pruning threshold
- `getPruningRate()`: Get pruning rate
- `setPruningRate(rate)`: Set pruning rate

#### Maturation
Neural maturation to adult properties.

**Methods:**
- `getProgress()`: Get maturation progress
- `setProgress(progress)`: Set progress
- `getMatureThreshold()`: Get mature threshold
- `getMatureRestingPotential()`: Get mature resting potential
- `getMatureTimeConstant()`: Get mature time constant

### Prediction Systems

#### PredictionSystem
Predicts future sensory states.

**Methods:**
- `predictNextState(currentState)`: Predict next state
- `updatePredictions(predicted, actual)`: Update predictions
- `getPredictionError()`: Get prediction error
- `getConfidence()`: Get confidence
- `getErrorHistory()`: Get error history
- `clearHistory()`: Clear history
- `train(observation)`: Train prediction system

#### NeuralPrediction
Implements temporal prediction using spiking substrate.

**Methods:**
- `initialize(brain)`: Initialize with brain
- `recordSensoryState(pattern, currentStep)`: Record sensory state
- `generatePrediction(currentStep)`: Generate prediction
- `updateWithObservation(actualState, currentStep)`: Update with observation
- `predictActionConsequence(action, currentState)`: Predict action consequence
- `getPredictionError()`: Get prediction error
- `getPredictionConfidence()`: Get prediction confidence

### Specialized Classes

#### Action
Action representation for motor output.

**Properties:**
- `type`: Action type
- `parameters`: Action parameters

**Methods:**
- `getType()`, `setType(type)`: Type
- `getParameters()`, `setParameters(params)`: Parameters
- `getName()`: Get action name
- `clone()`: Clone action

#### WorldObject
World object representation.

**Properties:**
- `x`, `y`: Position
- `radius`: Radius
- `type`: Object type
- `value`: Object value
- `active`: Active state

**Methods:**
- `getPosition()`: Get position as (x, y) tuple
- `setPosition(x, y)`: Set position
- `getRadiusSquared()`: Get radius squared
- `isWithinRange(x, y, radius)`: Check if within range
- `move(dx, dy)`: Move object
- `rotate(angleRadians)`: Rotate object

#### AgentBody
Agent body state.

**Properties:**
- `x`, `y`: Position
- `orientation`: Orientation
- `velocityX`, `velocityY`: Velocity components
- `angularVelocity`: Angular velocity
- `energy`: Energy level
- `health`: Health level
- `age`: Age
- `isMoving`: Moving state
- `isTurning`: Turning state

**Methods:**
- `getVelocityMagnitude()`: Get velocity magnitude
- `getAngle()`: Get direction angle
- `getDistanceTo(x, y)`: Get distance to position
- `isMoving()`: Check if moving
- `getMovementVector()`: Get movement vector
- `resetMovement()`: Reset movement vectors

## Convenience Functions

### Configuration Factory Functions

```python
# Create various configuration presets
config1 = pynlm.create_default_config()      # Standard configuration
config2 = pynlm.create_minimal_config()      # Minimal configuration
config3 = pynlm.create_large_scale_config()   # Large-scale configuration

# Load from dictionary
config = pynlm.load_config_from_dict({'key': 'value', 'number': 123})

# Clone and merge configurations
cloned = pynlm.clone_config(original)
merged = pynlm.merge_configs(config1, config2)
```

### World Factory Functions

```python
# Create basic world
world = pynlm.create_world()

# Create world with vision
vision_world = pynlm.create_vision_world(100, 100, 32, 32)

# Create agent setup
setup = pynlm.create_complete_agent_setup(config)
```

### Sensory Input Factory Functions

```python
# Create sensory inputs
vision = pynlm.create_vision(32, 32, 3)
audio = pynlm.create_audio(44100, 1024)

# Create actions
action1 = pynlm.create_action(pynlm.ActionType.MoveForward)
action2 = pynlm.create_action_with_params(pynlm.ActionType.Custom, [0.5, 0.3, 0.7])
```

### Error Handling Utilities

```python
# Safe operation wrapper
result = pynlm.try_safe_operation(lambda: some_operation())

# Exception catching with specific error types
result = pynlm.catch_exception(lambda: risky_operation(), "Database operation")
```

## Advanced Features

### Agent Integration Workflow

```python
def run_agent_experiment():
    # Create setup
    setup = pynlm.create_complete_agent_setup()
    brain = setup['brain']
    world = setup['world']
    agent_brain = setup['agent_brain']
    
    # Initialize
    agent_brain.initialize(world)
    
    # Configure sensors
    pynlm.configure_agent_sensors(agent_brain, world, 
                                 enable_vision=True, 
                                 enable_audio=False, 
                                 enable_internal=True)
    
    # Run episodes
    for episode in range(10):
        config = {'max_steps': 100, 'max_time': 10.0}
        reward, steps = agent_brain.run_agent_episode(world, config)
        
        # Log results
        print(f"Episode {episode}: Reward={reward:.2f}, Steps={steps}")
        
        # Reset for next episode
        agent_brain.reset()
        world.reset()
        world.setAgentStart(0.0, 0.0)
    
    # Get final statistics
    performance = brain.get_performance_stats()
    memory_status = brain.get_memory_system_status()
    neuromodulation = brain.get_neuromodulation_system()
    
    return performance, memory_status, neuromodulation
```

### Research Setup Function

```python
def create_research_setup(parameters):
    """Create a custom research setup with given parameters"""
    # Initialize configuration
    config = pynlm.Config()
    
    # Apply parameters
    for key, value in parameters.items():
        if key == 'random_seed':
            config.set('random_seed', value, pynlm.ConfigSource.Runtime)
        elif key == 'neuron_count':
            config.set('neuron_count', value, pynlm.ConfigSource.Runtime)
        elif key == 'region_count':
            config.set('region_count', value, pynlm.ConfigSource.Runtime)
        elif key == 'enable_vision':
            # Configure vision based on parameters
            pass
        elif key == 'enable_audio':
            # Configure audio based on parameters
            pass
    
    # Create brain and world
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Configure world based on parameters
    width = parameters.contains('width') ? parameters['width'] : 100;
    height = parameters.contains('height') ? parameters['height'] : 100;
    world.configure(width, height, 32, 32);
    
    return {
        'brain': brain,
        'world': world,
        'config': config
    }
```

### Simulation Utilities

```python
def run_long_simulation(brain, duration_steps, save_interval=1000):
    """Run a long simulation with periodic saves"""
    import time
    
    start_time = time.time()
    last_save = 0
    
    for step in range(duration_steps):
        brain.step(step)
        
        # Save periodically
        if step - last_save >= save_interval:
            timestamp = time.time() - start_time
            print(f"Step {step}, Time: {timestamp:.2f}s")
            last_save = step
    
    return time.time() - start_time
```

## Error Handling

### Custom Exceptions

The Python bindings provide specialized exception classes for different error types:

```python
try:
    # Access configuration
    value = config['missing_key']
except pynlm.MissingConfigError as e:
    print(f"Configuration key missing: {e}")
except pynlm.InvalidConfigError as e:
    print(f"Invalid configuration: {e}")
except pynlm.NeuralError as e:
    print(f"Neural system error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory system error: {e}")
except pynlm.ValueError as e:
    print(f"Invalid value: {e}")
except pynlm.IndexError as e:
    print(f"Index out of range: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Validation Functions

```python
# Comprehensive configuration validation
is_valid, errors, warnings = config.validate_config_comprehensive()

# Configuration integrity check
is_integrity_ok, integrity_warnings = config.validate_config_integrity()

# Safe operation wrapper
found, value = config.try_get('some_key')
if found:
    print(f"Value: {value}")
```

## Performance Monitoring

### System Statistics

```python
# Get performance statistics
stats = brain.get_performance_stats()

# Access various statistics
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.4f}")

# Spike system statistics
if 'spike_system' in stats:
    spike_stats = stats['spike_system']
    print(f"Total spikes: {spike_stats['total_spikes']}")
    print(f"Average spike rate: {spike_stats['average_spike_rate']:.4f}")
```

### Memory System Monitoring

```python
# Get memory system status
memory_status = brain.get_memory_system_status()

# Working memory
if 'working_memory' in memory_status:
    wm = memory_status['working_memory']
    print(f"Active traces: {wm['active_traces']}")
    print(f"Capacity: {wm['capacity']}")

# Episodic memory
if 'episodic_memory' in memory_status:
    em = memory_status['episodic_memory']
    print(f"Episode count: {em['episode_count']}")
    print(f"Average reward: {em['average_reward']:.4f}")
```

### Neuromodulation Monitoring

```python
# Get neuromodulation system status
neuromodulation = brain.get_neuromodulation_system()

# Check individual neuromodulators
if 'dopamine' in neuromodulation:
    dopamine = neuromodulation['dopamine']
    print(f"Dopamine level: {dopamine['level']:.4f}")
    print(f"Plasticity factor: {dopamine['plasticity_factor']:.4f}")

if 'curiosity' in neuromodulation:
    curiosity = neuromodulation['curiosity']
    print(f"Curiosity level: {curiosity['level']:.4f}")
    print(f"Exploration drive: {curiosity['exploration_drive']:.4f}")
```

## Examples and Tutorials

### Basic Tutorial

This section provides step-by-step tutorials for using the NLM Python bindings.

#### Tutorial 1: Creating a Simple Brain

```python
import pynlm

# Step 1: Create configuration
config = pynlm.create_minimal_config()

# Step 2: Create brain
brain = pynlm.createBrain(config)

# Step 3: Create world
world = pynlm.createSimpleWorld()
world.configure(50, 50, 16, 16)

# Step 4: Create agent
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Step 5: Run simulation
brain.step(1)
print("Simulation completed successfully!")
```

#### Tutorial 2: Agent-Based Learning

```python
import pynlm

def train_agent():
    # Create setup
    setup = pynlm.create_complete_agent_setup()
    brain = setup['brain']
    world = setup['world']
    agent_brain = setup['agent_brain']
    
    # Initialize
    agent_brain.initialize(world)
    
    # Run multiple episodes
    for episode in range(10):
        config = {'max_steps': 50, 'max_time': 5.0}
        reward, steps = agent_brain.run_agent_episode(world, config)
        
        print(f"Episode {episode + 1}: Reward={reward:.2f}, Steps={steps}")
        
        # Reset for next episode
        agent_brain.reset()
        world.reset()
        world.setAgentStart(0.0, 0.0)
    
    return brain, world, agent_brain

# Run training
brain, world, agent_brain = train_agent()
```

### Advanced Examples

#### Example: Memory System Exploration

```python
import pynlm

def explore_memory_system():
    # Create setup
    setup = pynlm.create_complete_agent_setup()
    brain = setup['brain']
    
    # Get memory system status
    memory_status = brain.get_memory_system_status()
    
    # Explore working memory
    if 'working_memory' in memory_status:
        wm = memory_status['working_memory']
        print(f"Working memory active traces: {wm['active_traces']}")
        print(f"Working memory capacity: {wm['capacity']}")
    
    # Explore episodic memory
    if 'episodic_memory' in memory_status:
        em = memory_status['episodic_memory']
        print(f"Episodic memory episode count: {em['episode_count']}")
        print(f"Episodic memory average reward: {em['average_reward']:.4f}")
    
    # Get memory statistics
    print("\nMemory System Statistics:")
    for key, value in memory_status.items():
        if isinstance(value, dict):
            print(f"  {key}:")
            for subkey, subvalue in value.items():
                print(f"    {subkey}: {subvalue}")
        else:
            print(f"  {key}: {value}")
    
    return memory_status

# Run exploration
memory_stats = explore_memory_system()
```

#### Example: Neuromodulation Study

```python
import pynlm

def study_neuromodulation():
    # Create setup
    setup = pynlm.create_complete_agent_setup()
    brain = setup['brain']
    
    # Get neuromodulation system status
    neuromodulation = brain.get_neuromodulation_system()
    
    print("Neuromodulation System Status:")
    for mod_name, mod_data in neuromodulation.items():
        print(f"\n{mod_name.title()}:")
        if isinstance(mod_data, dict):
            for key, value in mod_data.items():
                print(f"  {key}: {value:.4f}")
    
    # Simulate neuromodulation update
    brain.update_neuromodulation(reward=1.0, novelty=0.5, prediction_error=0.2)
    
    print("\nAfter neuromodulation update:")
    neuromodulation_updated = brain.get_neuromodulation_system()
    for mod_name, mod_data in neuromodulation_updated.items():
        print(f"\n{mod_name.title()}:")
        if isinstance(mod_data, dict):
            for key, value in mod_data.items():
                print(f"  {key}: {value:.4f}")
    
    return neuromodulation, neuromodulation_updated

# Run study
original, updated = study_neuromodulation()
```

#### Example: Development System Analysis

```python
import pynlm

def analyze_development():
    # Create setup
    setup = pynlm.create_complete_agent_setup()
    brain = setup['brain']
    
    # Get development system
    dev_system = brain.getDevelopmentSystem()
    
    if dev_system:
        print("Development System Analysis:")
        print(f"Current developmental stage: {dev_system.getStageName()}")
        print(f"Developmental age: {dev_system.getDevelopmentalAge():.2f}")
        print(f"Plasticity modifier: {dev_system.getPlasticityModifier():.4f}")
        print(f"Critical period: {dev_system.isCriticalPeriod()}")
        if dev_system.isCriticalPeriod():
            print(f"Critical period progress: {dev_system.getCriticalPeriodProgress():.4f}")
        
        # Advance development
        print(f"\nAdvancing development by 10 time units...")
        new_stage = dev_system.advanceDevelopment(10.0)
        print(f"New developmental stage: {new_stage}")
        
        # Update development with random generator
        import random
        rng = random.Random(42)
        dev_system.updateDevelopment(brain, rng, 0.001)
        
        print(f"Updated developmental stage: {dev_system.getStageName()}")
    
    return dev_system

# Run analysis
dev_system = analyze_development()
```

## Migration Guide

### From C++ to Python

If you're migrating from the C++ NLM API to the Python bindings, here are some key differences:

1. **Memory Management**: Python handles memory automatically, so you don't need to manually manage object lifetimes.

2. **Type Safety**: Python type hints and duck typing provide flexibility while maintaining safety.

3. **Error Handling**: Custom exception classes replace C++ error codes.

4. **API Style**: Pythonic APIs with method chaining and dict-like access.

#### Example Migration

**C++ Code:**
```cpp
// Create brain with config
Config config;
brain->loadFromFile("config.json");
Brain brain(config);

// Simulation step
brain.step(1);

// Get sensory input
SensoryInput input = world.getSensoryPercept();
brain->receiveSensoryInput(input);
```

**Python Code:**
```python
import pynlm

// Create brain with config
config = pynlm.load_config_from_dict({'key': 'value'})
brain = pynlm.createBrain(config)

// Simulation step
brain.step(1)

// Get sensory input
world = pynlm.createSimpleWorld()
percept = world.getSensoryPercept()
brain.receiveSensoryInput(percept)
```

### Common Patterns

#### Configuration Management

```python
// Load configuration from file
config = pynlm.Config()
config.loadFromFile("config.json")

// Use dict-like interface
config['random_seed'] = 42
config['neuron_count'] = 1000

// Save configuration
config.saveToFile("my_config.json")
```

#### Agent Workflow

```python
// Complete agent workflow
def agent_workflow(config):
    // Create setup
    setup = pynlm.create_complete_agent_setup(config)
    brain = setup['brain']
    world = setup['world']
    agent_brain = setup['agent_brain']
    
    // Initialize
    agent_brain.initialize(world)
    
    // Run episode
    reward, steps = agent_brain.run_agent_episode(world, 
                                                   {'max_steps': 100})
    
    // Get results
    state = agent_brain.get_agent_state()
    performance = brain.get_performance_stats()
    
    return reward, steps, state, performance
```

#### Research Experiments

```python
def research_experiment():
    // Create custom setup
    parameters = {
        'neuron_count': 5000,
        'region_count': 20,
        'enable_vision': True,
        'enable_audio': False,
        'width': 200,
        'height': 200
    }
    
    setup = pynlm.create_research_setup(parameters)
    brain = setup['brain']
    world = setup['world']
    
    // Run experiment
    for step in range(1000):
        brain.step(step)
        
        // Get performance stats periodically
        if step % 100 == 0:
            stats = brain.get_performance_stats()
            print(f"Step {step}: {stats['total_neurons']} neurons")
    
    return brain, world
```

## Troubleshooting

### Common Issues and Solutions

1. **ModuleNotFoundError: No module named 'pynlm'**
   - Ensure the package is installed: `pip install pynlm`
   - Check Python path: `pip install --user pynlm`

2. **ImportError: libpynlm.so: cannot open shared object file**
   - Install dependencies: `sudo apt-get install python3-dev build-essential cmake`
   - Reinstall the package: `pip install --force-reinstall pynlm`

3. **AttributeError: 'Brain' object has no attribute 'some_method'**
   - Check the API documentation for correct method names
   - Ensure you're using the latest version of the bindings

4. **TypeError: Expected int, got str**
   - Python bindings use duck typing, so check argument types
   - Use type conversion where needed

### Performance Tips

1. **For large simulations**:
   - Use `create_large_scale_config()` for better performance
   - Reduce `save_interval` in long simulations
   - Consider using `setCapacity()` for working memory

2. **For memory efficiency**:
   - Use `clear()` methods to free memory
   - Monitor `get_memory_system_status()`
   - Use `create_minimal_config()` for small simulations

3. **For real-time applications**:
   - Use `simulate_step_simple()` for faster execution
   - Monitor `get_performance_stats()` for bottlenecks
   - Consider using threads for multiple agents

### Debugging

```python
import pynlm
import traceback

// Enable detailed error reporting
def safe_operation():
    try:
        // Your code here
        pass
    except Exception as e:
        print(f"Error occurred: {e}")
        print("Stack trace:")
        traceback.print_exc()
        return False
    return True

// Use custom exceptions
config = pynlm.Config()
try:
    value = config.get_with_validation('non_existent_key')
except pynlm.MissingConfigError as e:
    print(f"Config error: {e}")
except pynlm.InvalidConfigError as e:
    print(f"Invalid config: {e}")
```

## Support and Contributions

### Getting Help

If you encounter issues or have questions:

1. **Documentation**: Check the latest documentation at https://nlm.readthedocs.io
2. **GitHub Issues**: Report bugs at https://github.com/nlm-project/nlm/issues
3. **Discussions**: Join the discussion forum at https://github.com/nlm-project/nlm/discussions
4. **Email**: Contact support@nlm.org for commercial support

### Contributing

We welcome contributions to the NLM Python bindings:

1. **Report Issues**: Submit bug reports and feature requests
2. **Fix Bugs**: Fix existing issues and improve code quality
3. **Add Features**: Implement new functionality and APIs
4. **Write Tests**: Add comprehensive test coverage
5. **Improve Documentation**: Update and extend documentation
6. **Examples**: Add new examples and tutorials

### Code Quality Standards

- Follow PEP 8 style guide
- Write comprehensive docstrings
- Add unit tests for new functionality
- Maintain backward compatibility
- Use type hints where appropriate
- Provide clear error messages

## Conclusion

The NLM Python bindings provide a powerful, Pythonic interface to the advanced neuroscience simulation capabilities of the NLM framework. This documentation covers the complete API, providing everything you need to get started with neural simulation, agent-based learning, and cognitive research.

Key features include:

- **Full-featured API**: Complete coverage of all NLM systems
- **Pythonic Design**: Dict-like interfaces, method chaining, and factory functions
- **Error Handling**: Custom exception classes and comprehensive error reporting
- **Performance Monitoring**: Built-in statistics and profiling tools
- **Comprehensive Documentation**: Extensive examples, tutorials, and migration guides
- **Production Ready**: Robust error handling and performance optimizations

Whether you're conducting research, developing applications, or learning about neural computation, the NLM Python bindings provide the tools you need to create sophisticated neural simulations with Python ease and power.

## Version Information

- **Package Version**: 0.1.0
- **C++ API Version**: NLM 2.0.0
- **Python Support**: 3.8+
- **Build System**: scikit-build with pybind11
- **License**: MIT
- **Documentation**: Auto-generated from source code

---

*This documentation is auto-generated from the source code and may be updated with each release. For the latest information, visit the official NLM documentation.*

For additional examples and advanced usage patterns, refer to the examples directory included with the package.