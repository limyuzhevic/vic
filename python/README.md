# NLM Python Bindings - Phase 4+ Cognitive Systems API Reference

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Phase 4+ cognitive systems Python bindings.

## Overview

The NLM Python bindings provide comprehensive access to Phase 4+ cognitive systems, including:

- **NeuralPlanner**: Action planning and sequence evaluation
- **ConceptFormation**: Abstract concept learning and categorization
- **NeuralWorkingMemory**: Working memory with competitive selection
- **NeuralEpisodicMemory**: Episodic memory storage and replay
- **PredictionSystem**: Predictive coding and forward models
- **Neuromodulators**: Reward, curiosity, prediction error, and novelty systems
- **SelfModel**: Internal model of the agent and body schema
- **SocialLearning**: Learning from observation and imitation
- **AttentionalSelection**: Neural attention through competition

## Installation

### Prerequisites

```bash
pip install pynlm
```

The package includes:
- C++ bindings using pybind11
- scikit-build for compilation
- NumPy support for array operations

### Building from Source

```bash
# Clone the repository
cd nlm

# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install
make install
```

## Basic Usage

### 1. Creating the Brain

```python
import pynlm as nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Create brain with configuration
brain = nlm.createBrain(config)

# Create simple world for testing
world = nlm.createSimpleWorld()

# Create agent brain interface
agent_brain = nlm.createAgentBrain(brain)
```

### 2. Using Cognitive Systems

```python
# Get access to Phase 4+ cognitive systems
cognitive_systems = brain.get_cognitive_systems()

# Neural Planner for action planning
neural_planner = cognitive_systems.get_neural_planner()

# Concept Formation for learning concepts
concept_formation = cognitive_systems.get_concept_formation()

# Prediction System for forward models
prediction_system = cognitive_systems.get_prediction_system()

# Working Memory for information maintenance
working_memory = cognitive_systems.get_neural_working_memory()

# Episodic Memory for experience storage
episodic_memory = cognitive_systems.get_episodic_memory()

# Self-Model for body schema
self_model = cognitive_systems.get_self_model()

# Social Learning for imitation
 social_learning = cognitive_systems.get_social_learning()

# Attentional Selection for focus
attentional_selection = cognitive_systems.get_attentional_selection()
```

### 3. Neuromodulation Systems

```python
# Access neuromodulators
neuromodulators = brain.get_neuromodulators()

# Reward prediction error (Dopamine)
reward = neuromodulators.get_reward()

# Exploration motivation (Curiosity)
curiosity = neuromodulators.get_curiosity()

# Prediction error
prediction_error = neuromodulators.get_prediction_error()

# Novelty detection
novelty = neuromodulators.get_novelty()
```

## Phase 4+ Cognitive Systems API

### NeuralPlanner

```python
# Plan actions based on current state and goals
target_reward = 0.8
best_action = neural_planner.planAction(current_state, target_reward)

# Evaluate action sequences
candidate = neural_planner.evaluateSequence(action_sequence, start_state)

# Configure planning
neural_planner.setPlanningDepth(5)  # Look ahead 5 steps
neural_planner.setPlanningConfidence(0.7)
neural_planner.setCurrentGoal(goal_state)

# Update planning from experience
neural_planner.updatePlanQuality(planned_actions, actual_actions, actual_reward)

# Check planning success
success = neural_planner.wasRecentPlanSuccessful()
```

### ConceptFormation

```python
# Present experiences to learn concepts
concept_id = concept_formation.presentExperience(pattern, features, reward, timestep)

# Get matching concepts
best_concept = concept_formation.getMatchingConcept(pattern)
concept_count = concept_formation.getConceptCount()

# Access concept details
concept = concept_formation.getConcept(concept_id)
prototype = concept_formation.getConceptPrototype(concept_id)
instances = concept_formation.getConceptInstances(concept_id)
stability = concept_formation.getConceptStability(concept_id)

# Concept properties
is_novel = concept_formation.isNovel(pattern)
can_merge = concept_formation.isConceptStable(concept_id)

# Configuration
concept_formation.setFormationThreshold(0.7)
concept_formation.setStabilityWindow(10)
```

### NeuralWorkingMemory

```python
# Store information patterns
working_memory.store(pattern, strength=1.0)
working_memory.storeToNeuron(neuron_id, activation=0.8)

# Retrieve and maintain information
retrieved = working_memory.retrieve()
activation = working_memory.getNeuronActivation(neuron_id)

# Update and maintain
working_memory.update(dt=0.1)  # Update with timestep
working_memory.runCompetition()  # Select winners

# Configuration
working_memory.setCapacity(100)
working_memory.setDecayRate(0.01)
working_memory.strengthenMemory(1.5)  # Rehearse

# Status
memory_activity = working_memory.getMemoryActivity()
capacity = working_memory.getCapacity()
```

### NeuralEpisodicMemory

```python
# Create and store episodes
episode = nlm.EpisodicMemoryItem()
episode.sensoryState = sensory_pattern
episode.positionX = x
episode.positionY = y
episode.action = action_type
episode.reward = reward
episode.novelty = novelty

episodic_memory.storeEpisode(episode)

# Retrieve episodes
recent = episodic_memory.getRecentEpisodes(5)
by_location = episodic_memory.retrieveByLocation(x, y, radius=1.0)
by_time = episodic_memory.retrieveTemporal(start_time, end_time)
by_action = episodic_memory.retrieveAfterAction(action, count=5)

# Replay and consolidation
episodic_memory.enableReplay(True)
for episode_to_replay in episodic_memory.getEpisodesForReplay(3):
    episodic_memory.replayEpisode(episode_to_replay)

episodic_memory.consolidate(relevance_threshold)
```

### PredictionSystem

```python
# Make predictions
predicted_state = prediction_system.predictNextState(current_state)

# Update from actual observations
prediction_system.updatePredictions(predicted, actual)

# Get prediction statistics
error = prediction_system.getPredictionError()
confidence = prediction_system.getConfidence()
history = prediction_system.getErrorHistory()

# Train and reset
prediction_system.train(observation)
prediction_system.clearHistory()
prediction_system.reset()
```

### Neuromodulators

```python
# Reward (Dopamine) - Reward prediction error
reward.processReward(current, next, action, expected_reward)
current_reward = reward.getCurrentReward()
reward_error = reward.getPredictionError()

# Curiosity - Exploration motivation
curiosity.processSensoryInput(input_data)
curiosity_level = curiosity.getCuriosityLevel()
curiosity.update(dt)

# Prediction Error - Learning signal
pred_error.processPrediction(predicted, actual)
pred_error_value = pred_error.getPredictionError()
pred_error_confidence = pred_error.getConfidence()

# Novelty - Novelty detection
novelty.processInput(input_data)
novelty_level = novelty.getNoveltyLevel()
novelty_confidence = novelty.getConfidence()
```

### SelfModel

```python
# Learn body schema
for i in range(10):
    before = state_pattern
    after = resulting_state
    action = action_type
    
    self_model.recordSelfAction(action, before, after)

# Get predictions
predicted_consequence = self_model.predictActionConsequence(action, current_state)
confidence = self_model.getSelfModelConfidence(action)

# Get preferences
preferred_action = self_model.getPreferredAction(state)
capability = self_model.getCapabilityLevel()
body_awareness = self_model.getBodyAwareness()

# Update and reset
self_model.updateSelfModel(predicted, actual, action)
self_model.clear()
```

### SocialLearning

```python
# Observe other agents
for action, state in observed_actions:
    social_learning.observeAgentAction(action, state, resulting_state)

# Imitation capabilities
can_imitate = social_learning.canImitate(action_type)
best_imitation = social_learning.getImitationAction(current_state)

# Communication signals
 social_learning.learnCommunicationSignal(signal_pattern, reward)
signal_detected = social_learning.detectSignal(neural_pattern)

# Social knowledge update
social_learning.updateSocialKnowledge(interaction_reward)
```

### AttentionalSelection

```python
# Process competition
competitors = [neuron1, neuron2, neuron3, ...]
winners = attentional_selection.processCompetition(competitors)

# Focus and release
attentional_selection.focusOnRegion(region_id)
attentional_selection.releaseAttention()

# Configure competition
attentional_selection.setInhibitionStrength(0.5)
attentional_selection.setExcitationStrength(1.5)
attentional_selection.setCompetitionThreshold(0.3)

# Biases and salience
attentional_selection.applyTopDownBias(neuron, strength)
attentional_selection.applyBottomUpSalience(neuron, strength)

# Status
attended = attentional_selection.isAttended(neuron)
inhibition = attentional_selection.getInhibitionFor(neuron)
excitation = attentional_selection.getExcitationFor(neuron)
```

## Integration Examples

### Complete Cognitive Loop

```python
# Create complete system
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent_brain = nlm.createAgentBrain(brain)

# Run cognitive loop
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process through cognitive systems
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    
    # Execute action
    result = world.applyMotorCommand(action, step * 0.1)
    
    # Apply neuromodulation
    agent_brain.applyRewardModulation(result.reward, 0.5 * result.reward)
    
    # Update development
    agent_brain.updateDevelopment(1.0)
```

### Learning and Adaptation

```python
# Experience learning through interaction
for episode in range(50):
    # Generate random experience
    pattern = np.random.rand(10)
    reward = np.random.rand()
    
    # Learn through concept formation
    concept_id = concept_formation.presentExperience(pattern, reward, episode)
    
    # Store in working memory
    working_memory.store(pattern, reward)
    working_memory.runCompetition()
    
    # Update self-model
    action = nlm.ActionType.MoveForward
    self_model.recordSelfAction(action, pattern, pattern * 0.9)
    
    # Process through neuromodulators
    curiosity.processSensoryInput(pattern)
    novelty.processInput(pattern)
```

## Configuration Management

```python
# Create and configure brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Save and load configurations
config.saveToFile("brain_config.json")
loaded_config = nlm.createDefaultConfig()
loaded_config.loadFromFile("brain_config.json")

# Configuration parameters
brain_config = brain.getConfig()
brain_config.set("learning_rate", 0.01)
brain_config.set("memory_capacity", 1000)
brain_config.set("development_enabled", True)
```

## Performance Considerations

### Memory Management

- Working memory has configurable capacity and decay rates
- Episodic memory supports consolidation and replay
- Efficient storage of neural patterns and associations

### Computational Efficiency

- Planning uses predictive coding rather than exhaustive search
- Competition mechanisms ensure efficient resource allocation
- Neuromodulators provide sparse, focused learning signals

### Scalability

- Modular design allows selective activation of systems
- Configurable parameters for different task requirements
- Support for batch processing of experiences

## Error Handling

```python
try:
    # Your code here
    result = neural_planner.planAction(state, reward)
except nlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Type Hints and IDE Support

The bindings include Python type hints for better IDE support:

```python
from typing import List, Dict, Optional
import numpy as np

# Type hints for common operations
def process_experience(pattern: np.ndarray, reward: float) -> int:
    concept_id = concept_formation.presentExperience(pattern, reward, 0)
    return concept_id

def plan_action(state: np.ndarray, target: float) -> nlm.ActionType:
    return neural_planner.planAction(state, target)
```

## Examples Directory

The `examples/` directory contains comprehensive demonstrations:

1. **basic_cognitive_systems.py** - Basic usage of all Phase 4+ systems
2. **advanced_cognitive_systems.py** - Advanced techniques and integrations
3. **brain_development.py** - Development stages and adaptation

Run examples with:

```bash
python examples/basic_cognitive_systems.py
python examples/advanced_cognitive_systems.py
python examples/brain_development.py
```

## Development

### Building Bindings

```bash
# Build using scikit-build
python -m pip install -e .[dev]

# Or build manually
cd python
mkdir build && cd build
cmake ..
make -j4
```

### Testing

```bash
# Run tests
pytest tests/

# Test specific modules
pytest tests/test_basic_bindings.py
pytest tests/test_cognitive_systems.py
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Implement changes
4. Add tests
5. Run linting and type checking
6. Submit pull request

## Dependencies

- Python >= 3.8
- NumPy >= 1.20 (optional)
- pybind11 >= 2.11.0
- scikit-build >= 0.5.0

## License

MIT License. See LICENSE file for details.

## References

For more information about NLM and Phase 4+ systems, see:

- [NLM Documentation](https://nlm.readthedocs.io)
- [Phase 6 Integration Results](docs/PHASE6_FINAL_RESULTS.md)
- [Scientific Background](docs/SCIENCE.md)
