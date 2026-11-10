# NLM Python Bindings Documentation

This document provides comprehensive documentation for the enhanced NLM Python bindings, including all new functionality added to support the full C++ API.

## Overview

The NLM (Neural Learning Machine) Python bindings provide access to the complete C++ neural simulation framework, including:

- **Core Neural Components**: Neurons, synapses, populations, regions
- **Memory Systems**: Working memory, episodic memory, semantic memory, procedural memory, associative memory
- **Prediction System**: Predictive coding and prediction error computation
- **Cognition Systems**: Neural planning, self-model, social learning
- **Development System**: Neural development and maturation
- **Neuromodulation**: Dopamine, acetylcholine, norepinephrine, serotonin, curiosity, novelty, reward
- **Advanced Features**: Checkpoint management, developmental stages, performance optimization

## Table of Contents

1. [Installation and Setup](#installation-and-setup)
2. [Basic Usage Examples](#basic-usage-examples)
3. [Core Neural Components](#core-neural-components)
4. [Memory Systems](#memory-systems)
5. [Prediction System](#prediction-system)
6. [Cognition Systems](#cognition-systems)
7. [Development System](#development-system)
8. [Neuromodulation Systems](#neuromodulation-systems)
9. [Advanced Features](#advanced-features)
10. [Performance Tips](#performance-tips)
11. [Troubleshooting](#troubleshooting)
12. [API Reference](#api-reference)

## Installation and Setup

### Prerequisites

- Python 3.8 or higher
- CMake 3.16 or higher
- C++20 compatible compiler
- pybind11 2.11.0 or higher

### Building from Source

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON

# Build the Python module
make pynlm

# Install the module
make install

# Or install directly with pip
pip install .
```

### Development Installation

For development purposes, you can install in editable mode:

```bash
pip install -e .
```

## Basic Usage Examples

### Example 1: Creating a Simple Brain

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Example 2: Basic Simulation Loop

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
```

### Example 3: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## Core Neural Components

### Neuron Class

The `Neuron` class represents a single neuron implementing Leaky Integrate-and-Fire dynamics.

```python
import pynlm

# Create a neuron with specific ID
neuron_id = pynlm.NeuronId(0)
neuron = pynlm.Neuron(neuron_id)

# Configure neuron properties
neuron.setType(pynlm.NeuronType.Excitatory)
neuron.setThreshold(-50.0)
neuron.setRestingPotential(-70.0)
neuron.setResetPotential(-70.0)
neuron.setLeakConductance(10.0)
neuron.setRefractoryPeriod(9)

# Initialize with random parameters (requires RandomGenerator)
rng = pynlm.RandomGenerator(42)
neuron.initializeRandom(rng)

# Simulate neuron activity
for t in range(100):
    # Inject current
    neuron.injectCurrent(5.0)
    
    # Update neuron
    fired = neuron.stepLIF(t, 0.1)
    
    if fired:
        print(f"Neuron fired at time {t}")
```

**Key Methods:**
- `getId()`, `setType()`, `getMembranePotential()`, `setMembranePotential()`
- `injectCurrent()`, `getTotalCurrent()`, `clearTotalCurrent()`
- `stepLIF()`, `checkThreshold()`, `recordSpike()`, `getSpikeHistory()`
- `addIncomingSynapse()`, `addOutgoingSynapse()`
- `enablePlasticity()`, `getPlasticityFlags()`

### Synapse Class

The `Synapse` class represents a synaptic connection between neurons with plasticity and delays.

```python
import pynlm

# Create a synapse
synapse_id = pynlm.SynapseId(0)
neuron_id = pynlm.NeuronId(0)
synapse = pynlm.Synapse(synapse_id, neuron_id, neuron_id)  # source = dest for self-connection

# Configure synapse properties
synapse.setWeight(0.5)
synapse.setDelay(1)
synapse.setType(pynlm.SynapseType.Excitatory)

# Enable plasticity
synapse.enablePlasticity(hebbian=True, stdp=True, rewardModulated=True)

# Initialize with random parameters
rng = pynlm.RandomGenerator(42)
synapse.initializeRandom(rng)

# Update synapse
for t in range(100):
    synapse.step(t)
    
    # Record spike activity
    synapse.recordPreSpike(t)
    synapse.recordPostSpike(t)
```

**Key Methods:**
- `getId()`, `getSourceNeuron()`, `getDestinationNeuron()`
- `getWeight()`, `setWeight()`, `addToWeight()`, `getDelay()`
- `getType()`, `isExcitatory()`, `isInhibitory()`
- `recordPreSpike()`, `recordPostSpike()`, `getPreSpikeHistory()`
- `getEligibilityTrace()`, `setEligibilityTrace()`, `getEfficacy()`

### NeuralPopulation Class

The `NeuralPopulation` class represents a group of neurons with shared properties and statistics.

```python
import pynlm

# Create a population with 100 neurons
population_id = pynlm.PopulationId(0)
population = pynlm.NeuralPopulation(population_id, 100)

# Set neuron type for all neurons in population
population.setNeuronType(pynlm.NeuronType.Excitatory)

# Initialize all neurons randomly
rng = pynlm.RandomGenerator(42)
population.initializeRandom(rng)

# Create neurons with sequential IDs
start_id = pynlm.NeuronId(1000)
population.createNeurons(start_id)

# Step all neurons
for t in range(100):
    population.step(t)
```

**Key Methods:**
- `getId()`, `getSize()`, `isEmpty()`, `getNeuron(index)`
- `getNeuronType()`, `setNeuronType()`
- `getActiveCount()`, `getFiringCount()`, `getAverageFiringRate()`
- `getActivityLevel()`, `recordActivity()`
- `getMeanMembranePotential()`, `getVarianceMembranePotential()`

### NeuralRegion Class

The `NeuralRegion` class represents a brain region containing multiple populations and local connectivity.

```python
import pynlm

# Create a neural region
region_id = pynlm.RegionId(0)
region = pynlm.NeuralRegion(region_id, "Cortex")

# Set next neuron index for ID allocation
region.setNextNeuronIndex(1000)

# Add a population of 50 excitatory neurons
population_id = region.addPopulation(50, pynlm.NeuronType.Excitatory)

# Add synapses between neurons
source_neuron = pynlm.NeuronId(1000)
dest_neuron = pynlm.NeuronId(1001)
synapse_id = region.addSynapse(source_neuron, dest_neuron, 0.5, 1)

# Initialize random connectivity
rng = pynlm.RandomGenerator(42)
region.initializeRandomConnectivity(rng, 0.1, 0.5, 0.1)

# Step all components
for t in range(100):
    region.step(t)
```

**Key Methods:**
- `getId()`, `getName()`, `setName()`
- `addPopulation()`, `getPopulation()`, `getPopulationCount()`
- `addSynapse()`, `getSynapse()`, `getSynapseCount()`
- `getSynapsesFrom()`, `getSynapsesTo()`
- `getTotalNeuronCount()`, `getActiveNeuronCount()`

## Memory Systems

### WorkingMemory Class

The `WorkingMemory` class stores temporary active information for quick access.

```python
import pynlm

# Create working memory
memory = pynlm.WorkingMemory()

# Store values for specific neurons
neuron1 = pynlm.NeuronId(1)
neuron2 = pynlm.NeuronId(2)

memory.store(neuron1, 0.8)
memory.store(neuron2, 0.3)

# Retrieve values
value1 = memory.retrieve(neuron1)
value2 = memory.retrieve(neuron2)

print(f"Neuron 1 value: {value1}")
print(f"Neuron 2 value: {value2}")

# Check if neuron has stored value
if memory.contains(neuron1):
    print("Neuron 1 has stored value")

# Clear all stored values
memory.clear()
```

### EpisodicMemory Class

The `EpisodicMemory` class stores events and experiences as episodes.

```python
import pynlm

# Create episodic memory
episodic_memory = pynlm.EpisodicMemory()

# Create an episodic memory item
episode = pynlm.EpisodicMemoryItem(
    timestamp=100,
    neurons=[pynlm.NeuronId(1), pynlm.NeuronId(2)],
    values=[0.8, 0.3],
    metadata="Initial exploration"
)

# Store the episode
episodic_memory.storeEpisode(episode)

# Retrieve recent episodes
recent_episodes = episodic_memory.getRecentEpisodes(5)
for i, ep in enumerate(recent_episodes):
    print(f"Episode {i}: timestamp={ep.timestamp}, neurons={ep.neurons}")

# Consolidate relevant episodes (move to long-term)
episodic_memory.consolidate(0.5)
```

### SemanticMemory Class

The `SemanticMemory` class stores facts and knowledge.

```python
import pynlm

# Create semantic memory
semantic_memory = pynlm.SemanticMemory()

# Store facts
semantic_memory.storeFact("predator_risk", "high")
semantic_memory.storeFact("food_location", "center")
semantic_memory.storeFact("water_source", "north")

# Retrieve facts
predator_risk = semantic_memory.retrieveFact("predator_risk")
food_location = semantic_memory.retrieveFact("food_location")

print(f"Predator risk: {predator_risk}")
print(f"Food location: {food_location}")

# Check if fact exists
if semantic_memory.hasFact("food_location"):
    print("Food location is known")

# Get all facts
all_facts = semantic_memory.getAllFacts()
for key, value in all_facts:
    print(f"{key}: {value}")
```

### ProceduralMemory Class

The `ProceduralMemory` class stores learned skills and habits.

```python
import pynlm

# Create procedural memory
procedural_memory = pynlm.ProceduralMemory()

# Learn a skill
skill_neural_pattern = [pynlm.NeuronId(1), pynlm.NeuronId(2), pynlm.NeuronId(3)]
procedural_memory.learnSkill("escape_route", skill_neural_pattern)

# Update proficiency
procedural_memory.updateProficiency("escape_route", 0.1)

# Get skill
skill = procedural_memory.getSkill("escape_route")
if skill:
    print(f"Skill name: {skill.name}")
    print(f"Neural pattern length: {len(skill.neuralPattern)}")
    print(f"Proficiency: {skill.proficiency}")
```

### AssociativeMemory Class

The `AssociativeMemory` class stores relationships between representations.

```python
import pynlm

# Create associative memory
associative_memory = pynlm.AssociativeMemory()

# Create association between neurons
neuron_a = pynlm.NeuronId(1)
neuron_b = pynlm.NeuronId(2)

associative_memory.associate(neuron_a, neuron_b, 0.7)

# Get associated neurons
associated = associative_memory.getAssociations(neuron_a)
print(f"Neurons associated with {neuron_a}: {associated}")

# Get association strength
strength = associative_memory.getAssociationStrength(neuron_a, neuron_b)
print(f"Association strength: {strength}")

# Update association
associative_memory.updateAssociation(neuron_a, neuron_b, 0.05)

# Clear all associations
associative_memory.clear()
```

## Prediction System

### PredictionSystem Class

The `PredictionSystem` predicts future sensory states and computes prediction error.

```python
import pynlm

# Create prediction system
prediction_system = pynlm.PredictionSystem()

# Create a simple sensory input (e.g., vision)
vision = pynlm.Vision(8, 8, 3)  # 8x8 image with 3 channels

# Set some dummy data
import random
data = [random.random() for _ in range(8 * 8 * 3)]
vision.setData(data)

# Make a prediction
predicted_state = prediction_system.predictNextState(vision)

# Train with observation
prediction_system.train(vision)

# Get prediction statistics
error = prediction_system.getPredictionError()
confidence = prediction_system.getConfidence()
print(f"Prediction error: {error}")
print(f"Prediction confidence: {confidence}")

# Clear history
prediction_system.clearHistory()
```

## Cognition Systems

### NeuralPlanner Class

The `NeuralPlanner` uses learned predictions to plan multi-step actions.

```python
import pynlm

# Create neural planner
planner = pynlm.NeuralPlanner()

# Set planning depth
planner.setPlanningDepth(5)

# Set current goal (example: navigate to center)
goal = [10.0, 10.0]  # x, y coordinates
planner.setCurrentGoal(goal)

# Plan action given current state
current_state = [5.0, 5.0, 0.0]  # x, y, orientation
planned_action = planner.planAction(current_state, targetReward=0.5)

print(f"Planned action: {planned_action}")

# Evaluate a sequence of actions
action_sequence = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.MoveForward
]

candidate = planner.evaluateSequence(action_sequence, current_state)
print(f"Sequence reward: {candidate.expectedReward}")
print(f"Sequence confidence: {candidate.confidence}")
```

**Key Methods:**
- `initialize(brain)`, `planAction()`, `evaluateSequence()`
- `getPlanningDepth()`, `setPlanningDepth()`, `getPlanningConfidence()`
- `setActionQuality()`, `getActionQuality()`, `getCurrentGoal()`

### SelfModel Class

The `SelfModel` represents the agent's internal model of itself.

```python
import pynlm

# Create self-model
self_model = pynlm.SelfModel()

# Initialize with brain (requires brain instance)
# brain = pynlm.createBrain(pynlm.createDefaultConfig())
# self_model.initialize(brain)

# Record self-action
before_state = [1.0, 2.0, 0.5]  # Example sensory state
action = pynlm.ActionType.MoveForward
after_state = [1.5, 2.0, 0.5]  # Result of action

self_model.recordSelfAction(action, before_state, after_state)

# Predict consequence of action
predicted_state = self_model.predictActionConsequence(action, before_state)
print(f"Predicted state: {predicted_state}")

# Get confidence in self-model for action
confidence = self_model.getSelfModelConfidence(action)
print(f"Confidence in action: {confidence}")
```

**Key Methods:**
- `initialize(brain)`, `recordSelfAction()`, `predictActionConsequence()`
- `getSelfModelConfidence()`, `computeSelfGeneratedLikeness()`
- `getPreferredAction()`, `getCapabilityLevel()`

### SocialLearning Class

The `SocialLearning` class enables learning from observing other agents.

```python
import pynlm

# Create social learning
social_learning = pynlm.SocialLearning()

# Initialize with brain (requires brain instance)
# brain = pynlm.createBrain(pynlm.createDefaultConfig())
# social_learning.initialize(brain)

# Record observation of another agent's action
observed_action = pynlm.ActionType.MoveForward
observer_state = [1.0, 2.0, 0.5]
resulting_state = [1.5, 2.0, 0.5]  # Effect of observed action

social_learning.observeAgentAction(observed_action, observer_state, resulting_state)

# Get best action to imitate
current_state = [1.2, 1.8, 0.5]
imitation_action = social_learning.getImitationAction(current_state)
print(f"Best imitation action: {imitation_action}")

# Get imitation action with similarity score
action_and_similarity = social_learning.getImitationActionAndSimilarity(current_state)
print(f"Action: {action_and_similarity[0]}, Similarity: {action_and_similarity[1]}")

# Learn communication signal
signal_pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
signal_reward = 1.0
social_learning.learnCommunicationSignal(signal_pattern, signal_reward)
```

**Key Methods:**
- `initialize(brain)`, `observeAgentAction()`
- `getImitationAction()`, `getImitationActionAndSimilarity()`
- `learnCommunicationSignal()`, `detectSignal()`
- `computeSimilarity()`, `hasSocialKnowledge()`

## Development System

### DevelopmentSystem Class

The `DevelopmentSystem` orchestrates neural development through different stages.

```python
import pynlm

# Create development system
development = pynlm.DevelopmentSystem()

# Set current developmental stage
development.setStage(pynlm.DevelopmentalStage.Initial)
print(f"Current stage: {development.getStage()}")
print(f"Stage name: {development.getStageName()}")

# Advance to next stage
development.advanceStage()
print(f"Advanced stage: {development.getStage()}")

# Get plasticity modifier for current stage
plasticity_modifier = development.getPlasticityModifier()
print(f"Plasticity modifier: {plasticity_modifier}")

# Check critical period
critical_period = development.isCriticalPeriod()
print(f"Is critical period: {critical_period}")

# Get developmental age
age = development.getDevelopmentalAge()
print(f"Developmental age: {age}")

# Update development system with brain and random generator
brain = pynlm.createBrain(pynlm.createDefaultConfig())
rng = pynlm.RandomGenerator(42)

development.update(brain, rng, 0.1)

# Set developmental age
development.setDevelopmentalAge(10.5)
```

**Key Methods:**
- `getStage()`, `setStage()`, `advanceStage()`, `getStageName()`
- `update(brain, timestep)` or `update(brain, rng, dt)`
- `getPlasticityModifier()`, `isCriticalPeriod()`, `getCriticalPeriodProgress()`
- `getDevelopmentalAge()`, `setDevelopmentalAge()`

## Neuromodulation Systems

### Dopamine Class

The `Dopamine` class represents reward and reinforcement learning signals.

```python
import pynlm

# Create dopamine neuromodulator
dopamine = pynlm.Dopamine()

# Set reward level
dopamine.setLevel(0.8)
print(f"Dopamine level: {dopamine.getLevel()}")

# Signal reward
dopamine.signalReward(1.0)

# Signal reward prediction error
dopamine.signalRewardPredictionError(0.5)

# Update neuromodulator state
for t in range(100):
    dopamine.update(0.1)
```

### Acetylcholine Class

The `Acetylcholine` class represents attention and memory consolidation.

```python
import pynlm

# Create acetylcholine neuromodulator
ach = pynlm.Acetylcholine()

# Set level
ach.setLevel(0.6)
print(f"Acetylcholine level: {ach.getLevel()}")

# Update neuromodulator state
for t in range(100):
    ach.update(0.1)
```

### Norepinephrine Class

The `Norepinephrine` class represents arousal and vigilance.

```python
import pynlm

# Create norepinephrine neuromodulator
ne = pynlm.Norepinephrine()

# Set level
ne.setLevel(0.7)
print(f"Norepinephrine level: {ne.getLevel()}")

# Update neuromodulator state
for t in range(100):
    ne.update(0.1)
```

### Serotonin Class

The `Serotonin` class represents mood, impulsivity, and social behavior.

```python
import pynlm

# Create serotonin neuromodulator
5ht = pynlm.Serotonin()

# Set level
5ht.setLevel(0.5)
print(f"Serotonin level: {5ht.getLevel()}")

# Update neuromodulator state
for t in range(100):
    5ht.update(0.1)
```

### Curiosity Class

The `Curiosity` class represents exploration motivation based on novelty and prediction error.

```python
import pynlm

# Create curiosity system
curiosity = pynlm.Curiosity()

# Initialize with brain (requires brain instance)
# brain = pynlm.createBrain(pynlm.createDefaultConfig())
# curiosity.initialize(brain)

# Update curiosity based on novelty and prediction error
novelty = 0.3
prediction_error = 0.4
curiosity.update(novelty, prediction_error, 0.1)

print(f"Curiosity level: {curiosity.getLevel()}")
print(f"Exploration drive: {curiosity.getExplorationDrive()}")

# Set curiosity parameters
curiosity.setNoveltyWeight(0.6)
curiosity.setPredictionErrorWeight(0.4)
```

### Novelty Class

The `Novelty` class detects novelty from observations.

```python
import pynlm

# Create novelty detector
novelty = pynlm.Novelty()

# Initialize with brain (requires brain instance)
# brain = pynlm.createBrain(pynlm.createDefaultConfig())
# novelty.initialize(brain)

# Create two observations
observation1 = [1.0, 2.0, 3.0, 4.0]
observation2 = [1.1, 2.1, 3.1, 4.1]  # Slightly different

# Detect novelty
novelty.detectNovelty(observation1, observation2)

# Get novelty level
level = novelty.getLevel()
print(f"Novelty level: {level}")

# Update novelty over time
for t in range(100):
    novelty.update(0.1)

# Get novelty history
history = novelty.getHistory()
print(f"Novelty history length: {len(history)}")
```

### Reward Class

The `Reward` class represents reward signals for reinforcement learning.

```python
import pynlm

# Create reward signal
reward = pynlm.Reward()

# Set initial reward value
reward.setValue(0.0)

# Add reward
reward.add(0.5)

# Get current value
print(f"Current reward value: {reward.getValue()}")

# Reset accumulated reward
reward.reset()

# Compute reward from observation
observation = [1.0, 2.0, 3.0]
computed_reward = reward.computeReward(observation)
print(f"Computed reward: {computed_reward}")

# Get reward history
history = reward.getHistory()
print(f"Reward history length: {len(history)}")
```

### PredictionError Class

The `PredictionError` class computes prediction error signals.

```python
import pynlm

# Create prediction error system
pred_error = pynlm.PredictionError()

# Initialize with brain (requires brain instance)
# brain = pynlm.createBrain(pynlm.createDefaultConfig())
# pred_error.initialize(brain)

# Compute prediction error
predicted = 0.5
actual = 0.8
pred_error.computeError(predicted, actual)

print(f"Prediction error: {pred_error.getError()}")

# Update prediction
pred_error.updatePrediction(0.6)

# Get error magnitude
magnitude = pred_error.getMagnitude()
print(f"Error magnitude: {magnitude}")

# Get error history
history = pred_error.getHistory()
print(f"Error history length: {len(history)}")
```

## Advanced Features

### Checkpoint Management

The Brain class provides checkpoint management functionality for saving and loading simulation state.

```python
import pynlm

# Create configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for step in range(100):
    brain.step(step)

# Save brain state (checkpoint)
brain.save("checkpoint.bin")
print("Brain state saved to checkpoint.bin")

# Clear brain state
brain.reset()

# Load brain state from checkpoint
brain.load("checkpoint.bin")
print("Brain state loaded from checkpoint.bin")

# Verify brain state is restored
print(f"Total neurons after load: {brain.getTotalNeuronCount()}")
```

### Developmental Stage Management

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor developmental stage changes
print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

# Update development for several steps
for step in range(50):
    brain.step(step)
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getDevelopmentalStage()}")

# Manually set developmental stage (for testing)
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
print(f"Set developmental stage to: {brain.getDevelopmentalStage()}")
```

### Curiosity and Novelty Management

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Initialize with world (simplified setup)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent.initialize(world)

# Enable curiosity system
agent.enableCuriosity(True)

# Run simulation with curiosity
for step in range(100):
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Apply reward modulation
    reward = 0.1  # Simplified reward
    agent.applyRewardModulation(reward, 0.0)
    
    # Update curiosity
    agent.updateDevelopment(0.1)
    
    # Print curiosity level
    if step % 20 == 0:
        print(f"Step {step}: Curiosity = {agent.getCuriosityLevel():.3f}, Novelty = {agent.getNoveltyLevel():.3f}")
```

### Reward Prediction Error Access

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Initialize with world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent.initialize(world)

# Enable reward modulation for RPE
agent.enableRewardModulation(True)

# Run simulation with RPE tracking
for step in range(100):
    # Get prediction error
    prediction_error = agent.getPredictionError()
    print(f"Step {step}: Prediction Error = {prediction_error:.3f}")
    
    # Apply reward with predicted reward
    reward = 0.1
    predicted_reward = 0.05
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Update development
    agent.updateDevelopment(0.1)
```

## Performance Tips

### Memory Management

1. **Clear unused memories**: Regularly call `clear()` on memory systems when they are no longer needed
   ```python
   # Clear episodic memory periodically
   episodic_memory.clear()
   ```

2. **Limit history size**: For systems with historical data, periodically trim old entries
   ```python
   # Limit prediction error history
   if pred_error_history.size() > 1000:
       # Remove oldest entries
       del pred_error_history[0:100]
   ```

### Computational Efficiency

1. **Batch operations**: When possible, group related operations to reduce overhead
   ```python
   # Process multiple neurons in batch
   for neuron in neurons:
       neuron.step(current_time)
   ```

2. **Use constant references**: For read-only access, use constant references
   ```python
   # For read-only access to population
   const_population = population
   neuron = const_population.getNeuron(0)
   ```

3. **Pre-allocate resources**: Initialize with expected sizes to avoid reallocations
   ```python
   # Create population with expected size
   population = pynlm.NeuralPopulation(pop_id, expected_size)
   ```

### Simulation Optimization

1. **Adjust time steps**: Use appropriate time step sizes for your simulation scale
   ```python
   # Smaller steps for fine-grained control
   brain.step(step)
   
   # Larger steps for coarse-grained simulation
   # brain.step(step, time=step * 0.1)
   ```

2. **Enable appropriate subsystems**: Only enable subsystems relevant to your simulation
   ```python
   agent.enableRewardModulation(True)
   agent.enableCuriosity(True)
   # Disable unused systems
   agent.enableDevelopment(False)
   ```

## Troubleshooting

### Common Issues and Solutions

**Issue: "RuntimeError: initialization failed"**

**Solution:**
1. Ensure `brain.initialize()` is called before `brain.step()`
2. Check that configuration is valid
3. Verify all required dependencies are properly initialized

**Code example:**
```python
# Correct initialization sequence
brain = pynlm.createBrain(config)
brain.initialize()  # <-- This is required!

# Then run simulation
for step in range(100):
    brain.step(step)  # Now this will work
```

**Issue: Memory usage grows over time**

**Solution:**
1. Periodically clear memory systems
2. Implement memory limits
3. Use garbage collection appropriately

**Code example:**
```python
# Periodically clear episodic memory
if step % 1000 == 0:
    episodic_memory.clear()
    working_memory.clear()
```

**Issue: Simulation becomes unstable**

**Solution:**
1. Check for numerical overflow
2. Verify neuron parameters are reasonable
3. Ensure proper time step sizes

**Code example:**
```python
# Check neuron parameters
if neuron.getMembranePotential() > 100.0:
    neuron.setMembranePotential(100.0)
    
if neuron.getMembranePotential() < -100.0:
    neuron.setMembranePotential(-100.0)
```

**Issue: Slow performance**

**Solution:**
1. Profile your code to identify bottlenecks
2. Use numpy for vector operations where possible
3. Consider using C++ extensions for performance-critical sections
4. Monitor and optimize memory usage

**Code example:**
```python
import time
import numpy as np

start_time = time.time()

# Your simulation code here
for step in range(1000):
    brain.step(step)

end_time = time.time()
print(f"Simulation took {end_time - start_time:.2f} seconds")
```

### Error Messages

**"ImportError: cannot import name 'SomeClass'"**

**Meaning:** The requested class is not available in the current version.

**Solution:**
- Check the available classes in the current bindings
- Update your code to use available alternatives
- Verify you're using the correct import statement

**"RuntimeError: Invalid neuron ID"**

**Meaning:** A neuron ID is out of range or invalid.

**Solution:**
- Check that neurons are properly initialized with valid IDs
- Verify ID allocation logic
- Ensure you're not using uninitialized neurons

**"RuntimeError: Synapse connection failed"**

**Meaning:** A synapse operation failed, often due to invalid neuron IDs.

**Solution:**
- Check that both source and destination neurons exist
- Verify neuron IDs are valid
- Ensure synapses are properly initialized

## API Reference

This section provides detailed API documentation for all available classes and methods.

### Configuration Class

**Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary

### Brain Class

**Methods:**
- `initialize()` - Initialize brain with configuration
- `step(currentStep[, currentTime])` - Run simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current into neuron
- `injectCurrentToNeurons(type, current)` - Inject current to neurons by type
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `addRegion([name])` - Add neural region
- `getRegion(id)` - Get region by ID
- `getTotalNeuronCount()`, `getTotalSynapseCount()` - Get counts
- `getActiveNeuronCount()`, `getFiringNeuronCount()` - Get active/firing counts
- `getDevelopmentalStage()`, `setDevelopmentalStage(stage)` - Get/set developmental stage

### AgentBrain Class

**Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development
- `getDevelopmentalStage()`, `getNeuromodulationLevel()` - Get statistics
- `getCuriosityLevel()`, `getNoveltyLevel()`, `getPredictionError()` - Get neuromodulation levels
- `enableRewardModulation(bool)`, `enableCuriosity(bool)`, etc. - Enable/disable subsystems

### World Classes

**SimpleWorld:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `update(timestep)` - Update world simulation
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory perception
- `getAgentBody()` - Get agent body state

### Sensory Classes

**Vision, Audio, InternalSignals:**
- All inherit from SensoryInput
- Set/get data methods for sensory input

### Action Classes

**Action:**
- `getType()`, `setType(type)` - Get/set action type
- `getParameters()`, `setParameters(params)` - Get/set action parameters

### Enumerations

**NeuronType:**
- Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal

**SynapseType:**
- Excitatory, Inhibitory, Modulatory, Electrical, GapJunction

**DevelopmentalStage:**
- Initial, CriticalPeriod, Maturation, Adult, Aging

**FiringState:**
- Resting, Active, Refractory, Inhibited

**ActionType:**
- MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight, Look, LookUp, LookDown, Interact, Eat, Drink, Rest, Wait, Custom

**MotorCommand:**
- MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait

**WorldObjectType:**
- Empty, Resource, Hazard, Wall, Marker

### Error Handling

The bindings use standard pybind11 exception handling. All C++ exceptions are automatically converted to Python exceptions with appropriate error messages.

**Common Exceptions:**
- `RuntimeError` - General runtime errors
- `ValueError` - Invalid parameter values
- `TypeError` - Type mismatches
- `IndexError` - Invalid indices

### Performance Considerations

**Memory Management:**
- Memory objects are automatically managed via reference counting
- Clear memory systems periodically to free unused memory
- Use constant references for read-only access

**Thread Safety:**
- The bindings are not thread-safe
- Ensure only one thread accesses simulation objects at a time
- Use appropriate synchronization if multiple threads are required

**Python Integration:**
- Use numpy arrays for large numerical data
- Consider using cython for performance-critical sections
- Profile your code to identify bottlenecks

## Conclusion

The enhanced NLM Python bindings provide comprehensive access to the full C++ neural simulation framework. This documentation covers all available functionality, from basic neural simulation to advanced cognitive modeling, prediction systems, and neuromodulation.

The Python API maintains the power and flexibility of the C++ implementation while providing a more accessible and user-friendly interface for researchers and developers. Whether you're building simple neural models or complex agent simulations, the NLM Python bindings provide all the tools you need.

For further questions or support, please refer to the main NLM documentation and the C++ source code for implementation details.
