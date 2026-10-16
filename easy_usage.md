# NLM Python Bindings Documentation

## Overview

This document provides comprehensive documentation for the NLM Python bindings (`pynlm`). These bindings expose the full functionality of the NLM (Neural Learning Machine) neural simulation framework through Python.

## Installation

### Prerequisites

- Python 3.8 or higher
- pip
- A C++ compiler with C++20 support

### Installation Steps

1. **Install from source**:

   ```bash
   pip install -e .
   ```

2. **Build from source (if needed)**:

   ```bash
   cd /path/to/nlm
   pip install -e . --verbose
   ```

   This will automatically build the C++ extension using scikit-build.

### Quick test

After installation, you can test the bindings:

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain
brain = pynlm.createBrain(config)

print("NLM Python bindings imported successfully!")
print(f"Brain created: {brain}")
```

## Basic Usage Examples

### 1. Basic Brain Initialization and Simulation

```python
import pynlm
import time

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

print("Brain initialized successfully!")
print(f"Brain has {brain.getRegionCount()} regions")
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getTotalSynapseCount()} synapses")

# Run a simple simulation
print("\n=== Running Simulation ===")
for step in range(10):
    # Inject sensory input
    # (In a real scenario, you'd use actual sensory data)
    
    # Step the simulation
    brain.step(step, step * 0.001)
    
    # Print status every few steps
    if step % 2 == 0:
        print(f"Step {step}: Spikes={brain.getTotalSpikeCount()}, "
              f"Active={brain.getActiveNeuronCount()}, "
              f"Firing={brain.getFiringNeuronCount()}")

print(f"\nFinal simulation completed!")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
```

### 2. Working Memory Example

```python
import pynlm

# Create brain and configure
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get working memory system
working_memory = brain.getWorkingMemory()

print("=== Working Memory Example ===")

# Store some information
pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
working_memory.store(pattern, strength=1.0)
print(f"Stored pattern: {pattern}")
print(f"Working memory capacity: {working_memory.getCapacity()}")
print(f"Working memory decay rate: {working_memory.getDecayRate():.3f}")

# Retrieve and check content
retrieved = working_memory.retrieve()
print(f"Retrieved pattern: {retrieved}")

# Update working memory with decay
time_step = 0.1
working_memory.update(time_step)
print(f"Updated working memory after {time_step}s")

# Check active traces
print(f"Active memory traces: {working_memory.getActiveTraces()}")
```

### 3. Memory Systems Integration

```python
import pynlm

# Create brain and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Memory Systems Integration ===")

# Access different memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
semantic_memory = brain.getSemanticMemory()
associative_memory = brain.getAssociativeMemory()

# Working memory example
wm_pattern = [0.2, 0.4, 0.6, 0.8, 0.3]
working_memory.store(wm_pattern)
print(f"Working memory stores: {working_memory.getCurrentSize()} items")

# Episodic memory example
from pynlm import EpisodicMemoryItem
episode = EpisodicMemoryItem()
episode.timestamp = 1000
episode.metadata = "First exploration"
# Would add neural patterns in real usage
# episode.neurons = [neuron1, neuron2, ...]
# episode.values = [0.5, 0.8, ...]

episodic_memory.storeEpisode(episode)
print(f"Episodic memory has: {episodic_memory.getEpisodeCount()} episodes")

# Semantic memory example
semantic_memory.storeFact("temperature", "warm")
semantic_memory.storeFact("location", "forest")
print(f"Semantic memory facts: {semantic_memory.getAllFacts()}")

# Associative memory example
# In real usage, you'd have actual neuron IDs
# associative_memory.associate(neuron_a, neuron_b, strength=0.7)
print("Associative memory available for connection strengths")
```

### 4. Neuromodulation Systems

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Neuromodulation Systems ===")

# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Dopamine example
print("Dopamine neuromodulator:")
print(f"  Name: {dopamine.getName()}")
print(f"  Level: {dopamine.getLevel():.3f}")
print(f"  Plasticity factor: {dopamine.getPlasticityFactor():.3f}")

# Simulate reward signaling
dopamine.signalReward(1.0)
dopamine.signalRewardPredictionError(0.1)
print("  Applied reward and prediction error signals")

# Curiosity example
print("Curiosity neuromodulator:")
print(f"  Name: {curiosity.getName()}")
print(f"  Exploration level: {curiosity.getExplorationLevel():.3f}")

# Novelty example
print("Novelty neuromodulator:")
print(f"  Name: {novelty.getName()}")
input_pattern = [0.1, 0.5, 0.9]
novelty_score = novelty.getNoveltyScore(input_pattern)
print(f"  Novelty score for {input_pattern}: {novelty_score:.3f}")

# Prediction Error example
print("Prediction Error neuromodulator:")
print(f"  Name: {prediction_error.getName()}")
prediction = [0.2, 0.4, 0.6]
target = [0.1, 0.3, 0.5]
error = prediction_error.getError(prediction, target)
print(f"  Prediction error: {error:.3f}")
```

### 5. Cognitive Systems

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Cognitive Systems ===")

# Access cognitive systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

# Neural Planner example
print("Neural Planner:")
print(f"  Planning depth: {planner.getPlanningDepth()}")
print(f"  Planning confidence: {planner.getPlanningConfidence():.3f}")

# Plan an action
# current_state would be actual neural activity data in real usage
# current_state = [0.1, 0.5, 0.3, ...]
# action = planner.planAction(current_state, target_reward=0.5)
# print(f"  Planned action: {action}")

# Concept Formation example
print("Concept Formation:")
# pattern would be actual neural pattern in real usage
# concept_formation.processPattern(pattern)
# concepts = concept_formation.getConcepts()
# print(f"  Number of concepts: {len(concepts)}")

# Attentional Selection example
print("Attentional Selection:")
print(f"  Attended regions: {attention.getAttendedRegions()}")
print(f"  Inhibition strength: {attention.getInhibitionStrength():.3f}")

# Focus attention on a region
# attention.focusOnRegion(region_id)
print("  Attention can be focused on specific regions")

# Development System example
print("Development System:")
development = brain.getDevelopmentSystem()
print(f"  Current stage: {development.getCurrentStage()}")
print(f"  Current age: {development.getAge():.1f}")
print(f"  Plasticity enabled: {development.isPlasticityEnabled()}")
print(f"  Plasticity rate: {development.getPlasticityRate():.3f}")
```

### 6. Agent Brain Example

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Initialize agent brain
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

print("=== Agent Brain Example ===")
print(f"Sensory input size: {agent_brain.getSensoryInputSize()}")
print(f"Motor output size: {agent_brain.getMotorOutputSize()}")

# Configure world
world.configure(100, 100, 20, 20)  # width, height, vision_width, vision_height
world.setAgentStart(50, 50)

# Example of processing sensory input and producing actions
# In a real scenario, you'd have actual sensory data
# percept = world.getSensoryPercept()
# agent_brain.processSensoryInput(percept)
# action = agent_brain.decodeMotorCommand()
# result = world.applyMotorCommand(action, world.getSimulationTime())
# 
# print(f"Action: {action}")
# print(f"Action result: {result.success}, Reward: {result.reward}")
```

### 7. Configuration Management

```python
import pynlm
import json

print("=== Configuration Management ===")

# Create default configuration
config = pynlm.createDefaultConfig()
print(f"Default config keys: {config.getKeys()}")

# Set configuration values
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)
config.set("neuron_count", 500)
config.set("region_count", 1)

print(f"Random seed: {config.get('random_seed')}")
print(f"Simulation timestep: {config.get('simulation_timestep')}")

# Save configuration to file
config.saveToFile("my_config.json")
print("Configuration saved to 'my_config.json'")

# Load configuration from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
print(f"Loaded config summary: {new_config.summary()}")

# Command line arguments
config_from_args = pynlm.createDefaultConfig()
# Simulate command line arguments
sys_argv = ["program", "--config", "my_config.json"]
config_from_args.loadFromArgs(len(sys_argv), sys_argv)
```

## API Reference

### Core Classes

#### Brain
Represents the central neural simulation brain with all integrated systems.

**Constructor:**
- `Brain(config: Config)` - Create brain with configuration

**Key Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(step, time)` - Perform a simulation step
- `receiveSensoryInput(input)` - Inject sensory input into the brain
- `injectCurrent(neuron, current)` - Inject current into a specific neuron
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Access Methods:**
- `getWorkingMemory()` - Get working memory system
- `getEpisodicMemory()` - Get episodic memory system
- `getAssociativeMemory()` - Get associative memory system
- `getPredictionSystem()` - Get prediction system
- `getPlanner()` - Get neural planner
- `getConceptFormation()` - Get concept formation system
- `getAttention()` - Get attentional selection system
- `getDevelopmentSystem()` - Get development system
- `getDopamine()` - Get dopamine neuromodulator
- `getCuriosity()` - Get curiosity neuromodulator
- `getNovelty()` - Get novelty neuromodulator
- `getPredictionErrorSignal()` - Get prediction error signal
- `getSpikeSystem()` - Get spike system
- `getSTDP()` - Get STDP plasticity rule
- `getHebbian()` - Get Hebbian plasticity rule
- `getStructuralPlasticity()` - Get structural plasticity rule

#### Config
Configuration management system for NLM parameters.

**Constructor:**
- `Config()` - Create default configuration

**Key Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load configuration from command line arguments
- `saveToFile(filepath)` - Save configuration to JSON file
- `set(key, value)` - Set configuration value
- `get(key)` - Get configuration value
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary

#### SimpleWorld
Simple 2D world for NLM simulation with agent interaction.

**Constructor:**
- `SimpleWorld()` - Create simple world

**Key Methods:**
- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `reset()` - Reset world state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world simulation
- `applyMotorCommand(cmd, time)` - Apply motor command
- `getSensoryPercept()` - Get current sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove world object

#### AgentBrain
Connects NLM brain to the world, handling sensory and motor interfaces.

**Constructor:**
- `AgentBrain(brain)` - Create agent brain with brain reference

**Key Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept and inject into brain
- `decodeMotorCommand()` - Decode brain motor activity into motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep)` - Update development system

### Memory Systems

#### WorkingMemory
Working memory for temporary information storage using neural dynamics.

**Constructor:**
- `WorkingMemory()` - Create working memory system

**Key Methods:**
- `store(neuron, value, strength)` - Store item in working memory
- `retrieve(neuron)` - Retrieve item from working memory
- `contains(neuron)` - Check if item exists
- `clear()` - Clear all items
- `getCapacity()` - Get working memory capacity
- `getCurrentSize()` - Get current number of items
- `decay(decay_rate)` - Decay all items

#### EpisodicMemory
Episodic memory for storing events and experiences.

**Constructor:**
- `EpisodicMemory()` - Create episodic memory system

**Key Methods:**
- `storeEpisode(episode)` - Store an episode
- `retrieveEpisode(index)` - Retrieve an episode
- `getEpisodeCount()` - Get number of episodes
- `getRecentEpisodes(count)` - Get recent episodes
- `clear()` - Clear all episodes
- `consolidate(threshold)` - Consolidate episodes above relevance threshold

#### SemanticMemory
Semantic memory for acquired knowledge and facts.

**Constructor:**
- `SemanticMemory()` - Create semantic memory system

**Key Methods:**
- `storeFact(key, value)` - Store a fact
- `retrieveFact(key)` - Retrieve a fact
- `hasFact(key)` - Check if fact exists
- `getAllFacts()` - Get all facts
- `clear()` - Clear all facts

#### ProceduralMemory
Procedural memory for learned skills and habits.

**Constructor:**
- `ProceduralMemory()` - Create procedural memory system

**Key Methods:**
- `learnSkill(name, pattern)` - Learn a new skill
- `getSkill(name)` - Get a skill by name
- `getAllSkills()` - Get all skills
- `updateProficiency(name, delta)` - Update skill proficiency
- `clear()` - Clear all skills

#### AssociativeMemory
Associative memory for relationships between representations.

**Constructor:**
- `AssociativeMemory()` - Create associative memory system

**Key Methods:**
- `associate(a, b, strength)` - Create association between neurons
- `getAssociations(neuron)` - Get all associations for a neuron
- `getAssociationStrength(a, b)` - Get association strength
- `updateAssociation(a, b, delta)` - Update association strength
- `clear()` - Clear all associations

### Neuromodulation Systems

#### Dopamine
Dopamine neuromodulator for reward and reinforcement learning.

**Key Methods:**
- `signalReward(reward)` - Signal reward
- `signalRewardPredictionError(error)` - Signal prediction error

#### Curiosity
Curiosity neuromodulator for exploration motivation.

**Key Methods:**
- `setExplorationLevel(level)` - Set exploration level
- `getExplorationLevel()` - Get exploration level

#### Novelty
Novelty neuromodulator for novelty detection.

**Key Methods:**
- `getNoveltyScore(input)` - Get novelty score for input
- `setNoveltyThreshold(threshold)` - Set novelty threshold

#### PredictionError
Prediction error neuromodulator for learning from prediction errors.

**Key Methods:**
- `getError(prediction, target)` - Get prediction error
- `setLearningRate(rate)` - Set learning rate

### Prediction System

#### PredictionSystem
Prediction system for sensory prediction and error computation.

**Key Methods:**
- `predictNextState(current_state)` - Make prediction for next timestep
- `updatePredictions(predicted, actual)` - Update predictions based on observation
- `getPredictionError()` - Get prediction error
- `getConfidence()` - Get prediction confidence
- `train(observation)` - Train prediction model

### Cognitive Systems

#### PlanningCandidate
Action planning candidate with predicted outcomes.

**Attributes:**
- `actions` - List of planned actions
- `predicted_states` - Predicted state sequences
- `expected_reward` - Expected total reward
- `confidence` - Planning confidence
- `depth` - Planning depth

#### NeuralPlanner
Neural planner for action planning using predictive dynamics.

**Key Methods:**
- `planAction(state, target_reward)` - Plan next action
- `evaluateSequence(actions, start_state)` - Evaluate action sequence
- `setPlanningDepth(depth)` - Set planning depth
- `setCurrentGoal(goal)` - Set current goal

#### ConceptFormation
Concept formation from experience patterns.

**Key Methods:**
- `processPattern(pattern)` - Process pattern for concept formation
- `getConcepts()` - Get formed concepts

#### DevelopmentSystem
Development system for age-dependent changes.

**Key Methods:**
- `update(dt)` - Update development system
- `setCurrentStage(stage)` - Set current developmental stage
- `setPlasticityEnabled(enabled)` - Set plasticity enabled state
- `setPlasticityRate(rate)` - Set plasticity rate

### Utility Functions

#### createDefaultConfig()
Creates a default configuration object.

#### createBrain(config)
Creates a new brain with configuration.

#### createSimpleWorld()
Creates a new simple world.

#### createAgentBrain(brain)
Creates a new agent brain interface.

## Error Handling

The NLM Python bindings use standard Python exception handling. Key exceptions include:

- `RuntimeError` - For general runtime errors
- `ValueError` - For invalid parameter values
- `KeyError` - For missing configuration keys

Error handling can be done using standard Python try-except blocks:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    # ... perform operations
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except ValueError as e:
    print(f"Invalid value: {e}")
```

## Performance Considerations

### Memory Management

- Neural objects are passed by reference where possible
- Use `py::return_value_policy::reference_internal` for lightweight returns
- Monitor memory usage during long-running simulations

### Simulation Speed

- Use batch operations where possible
- Optimize configuration loading for frequent operations
- Consider using C++ acceleration for intensive computations

### Data Management

- Convert between Python and C++ data structures efficiently
- Use appropriate data types (float vs. double) based on precision needs
- Minimize temporary object creation in tight loops

## Advanced Topics

### Custom Configuration

Extend the configuration system by adding custom parameters:

```python
config = pynlm.createDefaultConfig()
# Add custom parameters
config.set("custom_param1", "value1")
config.set("custom_param2", 42)
```

### Multi-threaded Usage

The NLM framework is designed for single-threaded simulation. For multi-threaded applications:

- Use separate brain instances for different threads
- Ensure proper synchronization when sharing data
- Consider using Python's `multiprocessing` for true parallelism

### Serialization

Save and load brain states for resuming simulations:

```python
brain.save("brain_checkpoint.nlm")
# ... later ...
new_brain = pynlm.createBrain(config)
new_brain.load("brain_checkpoint.nlm")
```

## Troubleshooting

### Common Issues

1. **Import errors**: Ensure pynlm is properly installed
2. **Configuration errors**: Check JSON syntax and file paths
3. **Simulation errors**: Verify configuration parameters are valid
4. **Memory issues**: Monitor memory usage and object creation

### Getting Help

- Check the NLM documentation for detailed API information
- Review examples in the repository
- Use Python's help() function for specific classes and methods
- Report issues with detailed error messages and configuration

## Example Scripts

The repository includes example scripts demonstrating various usage patterns. Look for:

- `examples/` directory
- Jupyter notebooks in `notebooks/` directory
- Test scripts in `tests/` directory

## Future Enhancements

The Python bindings are actively developed. Planned enhancements include:

- Advanced visualization tools
- Machine learning integration
- Cloud deployment support
- Real-time data streaming interfaces
- Enhanced error reporting and debugging tools

## License

This documentation is licensed under the MIT License, consistent with the NLM framework.

## Acknowledgments

The NLM Python bindings leverage pybind11 for seamless C++/Python integration, enabling access to the full power of the NLM neural simulation framework from Python.

---

*Last updated: Current date*
*Version: Based on NLM Phase 6 implementation*
