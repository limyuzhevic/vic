# NLM Enhanced Python API Documentation

This document describes the enhanced Python API for NLM (Phase 6 - Final Integration).

## Overview

The enhanced NLM Python API provides comprehensive access to all integrated brain systems:
- **Memory Systems**: Working Memory, Episodic Memory, Associative Memory
- **Neuromodulation**: Dopamine, Curiosity, Novelty, Prediction Error
- **Prediction & Cognition**: Prediction System, Planner, Concept Formation, Attention
- **Development System**: Developmental Stages, Plasticity Modulation
- **Advanced Utilities**: Batch processing, monitoring, configuration management

## Quick Start Guide

### 1. Basic Brain Setup

```python
import pynlm

# Create configuration with presets
config = pynlm.createDefaultConfig()

# Configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("neuromod.dopamine.scale", 1.5)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### 2. Complete Agent Simulation

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all subsystems for Phase 6
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        status = pynlm.getSystemStats(brain)
        print(f"Step {step}: Firing rate={status['firing_rate']:.2f}, "
              f"Curiosity={status['curiosity_level']:.3f}")
```

## Advanced Memory System Usage

### Working Memory

```python
# Access working memory
working_memory = brain.getWorkingMemory()

# Store information
working_memory.store([0.5, 0.8, 0.3])  # Store pattern
working_memory.storeToNeuron(neuron_id, 0.7)  # Store to specific neuron

# Monitor working memory
print(f"Active traces: {working_memory.getActiveTraces()}")
print(f"Capacity: {working_memory.getCapacity()}")
print(f"Memory activity: {working_memory.getMemoryActivity()}")

# Adjust parameters
working_memory.setCapacity(50)
working_memory.setDecayRate(0.01)

# Run competition for winner selection
working_memory.runCompetition()
print(f"Winning neurons: {[str(n) for n in working_memory.getWinners()]}")
```

### Episodic Memory

```python
# Access episodic memory
episodic_memory = brain.getEpisodicMemory()

# Store an experience
episode = pynlm.EpisodicMemoryItem()
episode.timestamp = 100
episode.neurons = [neuron_id1, neuron_id2]
episode.values = [0.5, 0.8]
episode.metadata = "Exploration of new area"

episodic_memory.storeEpisode(episode)

# Retrieve recent episodes
recent_episodes = episodic_memory.getRecentEpisodes(5)
for i, ep in enumerate(recent_episodes):
    print(f"Episode {i}: {ep.observationCount} observations, "
          f"avg reward: {ep.avgReward:.3f}")

# Consolidate important memories
episodic_memory.consolidate(0.7)  # Only episodes with >0.7 relevance
```

## Neuromodulation Control

### Dopamine System

```python
# Access dopamine
 dopamine = brain.getDopamine()

# Monitor and control dopamine
dopamine.setLevel(0.8)  # Increase motivation
print(f"Current dopamine level: {dopamine.getLevel()}")

# Signal reward events
dopamine.signalReward(1.0)  # Positive outcome
dopamine.signalRewardPredictionError(0.2)  # Prediction error

# Plasticity factor affects learning
dopamine.getPlasticityFactor()  # Returns current plasticity multiplier
```

### Curiosity and Exploration

```python
# Access curiosity
curiosity = brain.getCuriosity()

# Configure curiosity behavior
curiosity.setNoveltyWeight(0.7)
curiosity.setPredictionErrorWeight(0.3)

# Get curiosity level
print(f"Exploration drive: {curiosity.getExplorationDrive():.3f}")

# Curiosity drives exploration based on novelty and prediction error
# Higher curiosity = more exploration of novel situations
```

### Novelty Detection

```python
# Access novelty detection
novelty = brain.getNovelty()

# Set current novelty level
novelty.setLevel(0.6)  # High novelty encourages exploration

# Get novelty history
for i, level in enumerate(novelty.getHistory()):
    if i % 100 == 0:  # Print every 100th entry
        print(f"Time {i}: Novelty = {level:.3f}")

# Novelty detection from sensory patterns
# Can detect novel visual, auditory, or internal patterns
```

## Prediction and Planning

### Prediction System

```python
# Access prediction system
prediction_system = brain.getPredictionSystem()

# Make predictions
current_state = world.getSensoryPercept()
next_state = prediction_system.predictNextState(current_state)

# Monitor prediction quality
print(f"Prediction error: {prediction_system.getPredictionError():.3f}")
print(f"Prediction confidence: {prediction_system.getConfidence():.3f}")

# Train prediction system
prediction_system.train(current_state)

# Clear prediction history when needed
prediction_system.clearHistory()
```

### Neural Planner

```python
# Access neural planner
planner = brain.getPlanner()

# Configure planning depth
planner.setPlanningDepth(5)  # Look ahead 5 steps

# Get current goal
if planner.getCurrentGoal():
    print(f"Current goal: {planner.getCurrentGoal()}")

# Set specific goal (example: navigate to resource)
goal_state = [0.8, 0.2, 0.1]  # Target coordinates
planner.setCurrentGoal(goal_state)

# Plan action
state = [0.1, 0.1, 0.1]  # Current position
next_action = planner.planAction(state, 0.8)  # Target reward of 0.8
print(f"Planned action: {next_action}")

# Evaluate action sequence
actions = [ActionType.MoveForward, ActionType.TurnRight]
candidate = planner.evaluateSequence(actions, state)
print(f"Expected reward: {candidate.expectedReward:.3f}, "
      f"Confidence: {candidate.confidence:.3f}")

# Learn from experience
planner.setActionQuality(next_action, 0.9)  # High quality action
```

### Concept Formation

```python
# Access concept formation
concept_formation = brain.getConceptFormation()

# Present experiences for concept learning
pattern = [0.5, 0.7, 0.3]  # Sensory pattern
features = [0.8, 0.3, 0.6]  # Abstract features
reward = 0.5  # Associated reward
time = 100  # Simulation time

concept_id = concept_formation.presentExperience(pattern, features, reward, time)
if concept_id > 0:
    print(f"Created new concept {concept_id} or matched existing concept")

# Get discovered concepts
for concept in concept_formation.getConcepts():
    print(f"Concept {concept.id}: stability={concept.avgStability:.3f}, "
          f"observations={concept.totalObservations}")

# Merge similar concepts
concept_formation.mergeConcepts(1, 2)  # Merge concepts 1 and 2
```

## Development System

```python
# Access development system
development = brain.getDevelopmentSystem()

# Monitor developmental stage
stage = development.getStage()
print(f"Current stage: {stage}")
print(f"Stage name: {development.getStageName()}")

# Get plasticity modifier (affects learning rate)
plasticity_mod = development.getPlasticityModifier()
print(f"Plasticity modifier: {plasticity_mod:.3f}")

# Critical period status
critical = development.isCriticalPeriod()
progress = development.getCriticalPeriodProgress()
print(f"Critical period: {critical}, Progress: {progress:.3f}")

# Advance development
development.advanceStage()

# Set developmental age
development.setDevelopmentalAge(50.0)  # 50 simulation time units old
```

## Advanced Simulation Utilities

### Episode Management

```python
# Run a complete agent episode
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()
agent.initialize(world)

results = pynlm.runAgentEpisode(brain, world, agent, 500)
print(f"Episode results: {results}")
```

### Batch Processing

```python
# Run multiple simulations in parallel
brains = [pynlm.createBrain(config) for _ in range(5)]
agents = [pynlm.createAgentBrain(brain) for brain in brains]
worlds = [pynlm.createSimpleWorld() for _ in range(5)]

# Configure each world
for i, world in enumerate(worlds):
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(i * 3, i * 3)

# Initialize all agents
for i, (agent, world) in enumerate(zip(agents, worlds)):
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)

# Run batch simulation
batch_results = pynlm.batchSimulate(brains, worlds, agents, 200)
for i, result in enumerate(batch_results):
    print(f"Brain {i}: Firing rate={result['firing_rate']:.2f}, "
          f"Total spikes={result['total_spikes']}")
```

### Checkpoint and State Management

```python
# Save brain with statistics
brain = pynlm.createBrain(config)
brain.initialize()
# ... run simulation ...

pynlm.saveBrainWithStats(brain, "my_brain_checkpoint")
# Also creates "my_brain_checkpoint.stats.json" with performance metrics

# Load saved brain
loaded_brain = pynlm.loadBrainWithStats("my_brain_checkpoint")
```

## Monitoring and Visualization

### System Statistics

```python
# Get comprehensive system statistics
stats = pynlm.getSystemStats(brain)
print("Brain Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")
```

### Performance Monitoring

```python
# Monitor performance over time
performance_data = pynlm.monitorPerformance(brain, 1000)

# Calculate efficiency score
efficiency = pynlm.calculateEfficiencyScore(performance_data)
print(f"Simulation efficiency: {efficiency:.3f}")
```

### Brain Status

```python
# Print comprehensive brain status
pynlm.printBrainStatus(brain)
```

## Configuration Utilities

### Create Configuration from Dictionary

```python
# Create config from Python dict
config_dict = {
    "brain.neuron_count": 1500,
    "brain.synapse_density": 0.15,
    "neuromod.dopamine.scale": 1.2,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "plasticity.structural.enable": True
}

config = pynlm.createConfigFromDict(config_dict)
brain = pynlm.createBrain(config)
```

### Configuration Convenience Methods

```python
config = pynlm.createDefaultConfig()

# Easy get/set methods
config.set("brain.neuron_count", 2000)
config.set("learning.rate", 0.001)
config.set("brain.v_thresh", -55.0)

# Get with defaults
neuron_count = config.getInt("brain.neuron_count", 1000)
learning_rate = config.getFloat("learning.rate", 0.01)
autonomy = config.getBool("agent.autonomy", False)
```

## Utility Functions

### Validation and Troubleshooting

```python
# Validate brain system integrity
errors = pynlm.validateBrain(brain)
if errors:
    print("Validation errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("Brain system is valid!")
```

### Version Information

```python
# Get version info
version = pynlm.version()
print(f"NLM Python Bindings: {version}")
```

### Cache Management

```python
# Clear caches
pynlm.clearAllCaches()
```

## Example Projects

### 1. Navigation Agent

```python
import pynlm
import random

def create_navigation_agent(config):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Place agent in random location
    x = random.uniform(5, 25)
    y = random.uniform(5, 25)
    world.setAgentStart(x, y)
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return brain, agent, world

def run_navigation_episode(brain, agent, world, max_steps=500):
    for step in range(max_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check if agent reached edge (navigation goal)
        body = world.getAgentBody()
        if body.x <= 1 or body.x >= 29 or body.y <= 1 or body.y >= 29:
            return True  # Reached boundary
    
    return False  # Timeout

# Main simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 800)

reach_count = 0
for episode in range(10):
    brain, agent, world = create_navigation_agent(config)
    reached = run_navigation_episode(brain, agent, world)
    if reached:
        reach_count += 1
    
    if episode % 5 == 0:
        stats = pynlm.getSystemStats(brain)
        print(f"Episode {episode}: Reached={reached}, "
              f"Firing rate={stats['firing_rate']:.2f}")

print(f"Success rate: {reach_count}/10 = {reach_count/10*100:.1f}%")
```

### 2. Pattern Recognition

```python
import pynlm
import numpy as np

def create_pattern_recognition_agent(config):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.reset()
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    # Add some test objects
    world.addObject(pynlm.WorldObject(2, 2, pynlm.WorldObjectType.Resource, value=1.0))
    world.addObject(pynlm.WorldObject(7, 7, pynlm.WorldObjectType.Hazard, value=0.5))
    
    return brain, agent, world

def train_pattern_recognition(brain, agent, world, patterns):
    concept_formation = brain.getConceptFormation()
    
    for episode in range(100):
        # Present random pattern
        pattern_idx = episode % len(patterns)
        world.reset()
        world.setAgentStart(patterns[pattern_idx][0], patterns[pattern_idx][1])
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward based on interaction with learned patterns
            reward = 0.0
            if pattern_idx == 0:  # Resource
                if world.getAgentBody().x > 1 and world.getAgentBody().x < 3:
                    reward = 1.0
            elif pattern_idx == 1:  # Hazard (avoid)
                if world.getAgentBody().x > 5 and world.getAgentBody().x < 7:
                    reward = -0.5
            
            # Present experience to concept formation
            pattern = agent.getSensoryInputSize() * [0.0]  # Simplified
            concept_formation.presentExperience(pattern, [], reward, step)
    
    # Show learned concepts
    print("\nLearned Concepts:")
    for concept in concept_formation.getConcepts():
        print(f"  Concept {concept.id}: {concept.categoryHint}, "
              f"Stability: {concept.avgStability:.3f}")
```

### 3. Developmental Learning

```python
import pynlm
import time

def create_developmental_agent(config):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    return brain, agent, world

def run_development_experiment(brain, agent, world, num_steps=1000):
    developmental_stage_log = []
    curiosity_log = []
    firing_rate_log = []
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log developmental metrics
        if step % 10 == 0:
            developmental_stage_log.append(agent.getDevelopmentalStage())
            curiosity_log.append(agent.getCuriosityLevel())
            firing_rate_log.append(brain.getAverageFiringRate())
    
    return {
        "developmental_stages": developmental_stage_log,
        "curiosity_levels": curiosity_log,
        "firing_rates": firing_rate_log,
        "final_stage": agent.getDevelopmentalStage(),
        "final_curiosity": agent.getCuriosityLevel()
    }

# Run developmental experiment
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 600)
config.set("neuromod.curiosity.enable", True)

brain, agent, world = create_developmental_agent(config)
results = run_development_experiment(brain, agent, world)

print("Developmental Experiment Results:")
print(f"  Final developmental stage: {results['final_stage']}")
print(f"  Final curiosity level: {results['final_curiosity']:.3f}")
print(f"  Average firing rate: {np.mean(results['firing_rates']):.2f}")
print(f"  Developmental stage transitions: {results['developmental_stages'].count(2)} "
      f"(Critical period)")
```

## Performance Tips

1. **Use batch processing for multiple simulations**
2. **Monitor system performance regularly**
3. **Use checkpoints for long-running simulations**
4. **Configure brain parameters for specific tasks**
5. **Monitor curiosity and novelty for exploration balance**

## Troubleshooting

### Common Issues

1. **"Brain not doing anything"**
   - Ensure `brain.initialize()` is called before `brain.step()`
   - Check that sensory input is connected
   - Verify world updates are being processed

2. **"Agent isn't moving"**
   - Ensure `world.applyMotorCommand()` is called
   - Check that `world.update()` is being called
   - Verify agent has motor capabilities

3. **"Everything is 0"**
   - Brains need time to "warm up" - try more steps
   - Some neurons need input to fire - ensure sensory input is connected
   - Check neuromodulation levels for motivation

4. **Performance issues**
   - Use smaller brain sizes for rapid testing
   - Monitor performance with `pynlm.monitorPerformance()`
   - Use checkpoints for long simulations

### Error Messages

- `RuntimeError`: Check Python bindings for issues
- `ImportError: No module named 'pynlm'`: Install with `pip install .`
- `AttributeError`: Check method names and parameters

## Migration Guide

### From Old API to New API

**Old (limited):**
```python
# Only basic brain functionality
brain = pynlm.createBrain()
brain.initialize()
brain.step(step)
```

**New (enhanced):**
```python
# Full access to all systems
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)

# Access memory, neuromodulation, cognition, development
working_memory = brain.getWorkingMemory()
novelty = brain.getNovelty()
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
development = brain.getDevelopmentSystem()

# Advanced utilities
stats = pynlm.getSystemStats(brain)
pynlm.runAgentEpisode(brain, world, agent)
```

## API Reference

For complete API documentation, refer to the pybind11 binding headers in the C++ source code.

Key enhancements over the original API:
- **Memory Systems**: WorkingMemory, EpisodicMemory, AssociativeMemory
- **Neuromodulation**: Dopamine, Curiosity, Novelty, PredictionError
- **Prediction & Cognition**: PredictionSystem, NeuralPlanner, ConceptFormation, AttentionalSelection
- **Development**: DevelopmentSystem with full lifecycle management
- **Utilities**: Batch processing, monitoring, configuration management, checkpointing
- **Monitoring**: getSystemStats(), monitorPerformance(), calculateEfficiencyScore()
- **Validation**: validateBrain(), printBrainStatus(), clearAllCaches()

## License

MIT License - See LICENSE file for details.
