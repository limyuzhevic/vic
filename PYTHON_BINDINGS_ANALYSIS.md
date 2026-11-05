# Enhanced Python Bindings for NLM (Phase 6 API)

## Overview

This document describes the enhanced Python bindings (`pynlm`) that provide full access to the Phase 6 systems of the Neural Learning Machine (NLM) framework.

## What's New in Phase 6 Bindings

The enhanced bindings transform `pynlm` from a basic skeleton (~15% functionality) into a comprehensive Python interface for neural simulation research (~60% functionality).

### Key Enhancements

#### 1. Complete Phase 6 Cognitive Systems
- **Neural Planner**: Multi-step action planning with goal evaluation
- **Concept Formation**: Pattern discovery from experience
- **Attentional Selection**: Neural attention through competition
- **Prediction System**: Sensory prediction and error computation

#### 2. Full Neuromodulation Systems
- **Dopamine**: Reward and reinforcement learning signal
- **Curiosity**: Exploration motivation based on novelty and prediction error
- **Novelty**: Novelty detection from sensory changes
- **Prediction Error**: Prediction error signal for curiosity and learning

#### 3. Advanced Memory Systems
- **Working Memory**: Transient active information storage using neural dynamics
- **Episodic Memory**: Experience storage with pattern completion and replay
- **Associative Memory**: Pattern associations with spreading activation

#### 4. Rich Configuration API
- Full parameter setting and retrieval with type-safe access
- Command-line and file-based configuration loading
- Runtime configuration with validation

#### 5. Comprehensive Diagnostic Systems
- Real-time brain statistics (spike counts, firing rates, etc.)
- Agent diagnostics (curiosity, novelty, prediction error)
- Plasticity counters (reflex bonuses, planner overrides, etc.)
- Development stage tracking

#### 6. Social Learning and Language
- **Text Processing**: Phase 6 language understanding and encoding
- **Social Learning**: Learning from other agents' behavior
- **Self-Model**: Internal model of one's own capabilities
- **Multi-agent Studies**: Observation and imitation of other agents

## API Reference

### Core Classes

#### `pynlm.Brain`
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step_number)
brain.step(step_number, timestamp)
brain.reset()

# Input/Output
brain.receiveSensoryInput(sensory_input)
action = brain.produceAction()

# Access to Phase 6 systems
brain.getWorkingMemory()      # NeuralWorkingMemory
brain.getEpisodicMemory()     # NeuralEpisodicMemory
brain.getAssociativeMemory()  # NeuralAssociativeMemory
brain.getPredictionSystem()   # PredictionSystem
brain.getPlanner()            # NeuralPlanner
brain.getConceptFormation()   # ConceptFormation
brain.getAttention()          # AttentionalSelection
brain.getDevelopmentSystem()  # DevelopmentSystem
brain.getDopamine()           # Dopamine
brain.getCuriosity()          # Curiosity
brain.getNovelty()            # Novelty
brain.getPredictionErrorSignal()  # PredictionError

# Statistics
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getActiveNeuronCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
brain.getExcitationInhibitionRatio()
brain.getTotalSpikeCount()
brain.getPendingSpikeEventCount()

# State management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")

# Development
brain.getDevelopmentalStage()
brain.setDevelopmentalStage(stage)
```

#### `pynlm.AgentBrain`
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Sensory processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Motor decoding
action = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)
agent.updateDevelopment(timestep)

# Phase 6 advanced features
agent.encodeText("hello world")           # Language processing
agent.getThoughtTrace()                   # Cognitive transparency
agent.getInternalFiringRates()           # Internal neural activity
agent.getConceptCount()                   # Concept count
agent.getSelfModelConfidence()            # Self-model confidence
agent.getSocialObservationCount()         # Social observation count
agent.observeOtherAgent()                 # Social learning
agent.getReflexBonusCount()               # Reflex bonus events
agent.getReflexSuppressCount()            # Reflex suppression events
agent.getWtaPulseCount()                  # Winner-take-all pulses
agent.getPlannerOverrideCount()           # Planner overrides
agent.getPlannerAgreementCount()          # Planner agreements
```

#### `pynlm.Config`
```python
config = pynlm.createDefaultConfig()

# Get values with type safety
value_int = config.getOr("brain.neuron_count", 1000)
value_float = config.getOr("learning.rate", 0.001)
value_string = config.getOr("brain.name", "default")
value_bool = config.getOr("plasticity.enabled", True)

# Check existence
if config.has("brain.neuron_count"):
    pass

# Set values
config.set("brain.neuron_count", 2000)
config.set("learning.rate", 0.01)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### `pynlm.SimpleWorld`
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Simulation
world.update(timestep=0.1)
world.applyMotorCommand(action, time)

# Access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource))
world.removeObject(x, y)
```

### Memory System APIs

#### Working Memory
```python
working_memory = brain.getWorkingMemory()
working_memory.store(pattern, strength=1.0)
working_memory.storeToNeuron(neuron_id, activation)
memory = working_memory.retrieve()
activation = working_memory.getNeuronActivation(neuron_id)
working_memory.update(dt)
working_memory.clear()
```

#### Episodic Memory
```python
episodic_memory = brain.getEpisodicMemory()

# Store an experience
episode = EpisodicMemoryItem()
# Fill episode details
episodic_memory.storeEpisode(episode)

# Retrieve episodes
# By similarity
episodes = episodic_memory.retrieveSimilar(pattern, maxResults=5)

# By time window
episodes = episodic_memory.retrieveTemporal(start_time, end_time)

# By location
episodes = episodic_memory.retrieveByLocation(x, y, radius)

# By action
episodes = episodic_memory.retrieveAfterAction(action_type)
```

#### Associative Memory
```python
assoc_memory = brain.getAssociativeMemory()

# Create associations
assoc_memory.associate(pattern_a, pattern_b, strength=1.0)

# Retrieve associations
patterns = assoc_memory.retrieve(pattern)

# Spread activation
activated = assoc_memory.spreadActivation(pattern)
```

### Cognitive System APIs

#### NeuralPlanner
```python
planner = brain.getPlanner()
action = planner.planAction(current_state, target_reward=0.5)
candidate = planner.evaluateSequence(actions, start_state)
planner.setActionQuality(action, quality)
quality = planner.getActionQuality(action)
planner.setCurrentGoal(goal)
```

#### ConceptFormation
```python
concept_formation = brain.getConceptFormation()
concept_count = concept_formation.getConceptCount()
concept = concept_formation.getConcept(concept_id)
```

#### PredictionSystem
```python
prediction_system = brain.getPredictionSystem()
# Used internally by curiosity and novelty systems
```

### Neuromodulation APIs

#### Dopamine
```python
dopamine = brain.getDopamine()
level = dopamine.getLevel()
dopamine.setLevel(level)
plasticity_factor = dopamine.getPlasticityFactor()
dopamine.signalReward(reward)
dopamine.signalRewardPredictionError(error)
```

#### Curiosity
```python
curiosity = brain.getCuriosity()
level = curiosity.getLevel()
exploration_drive = curiosity.getExplorationDrive()
curiosity.setNoveltyWeight(weight)
curiosity.setPredictionErrorWeight(weight)
```

#### Novelty
```python
novelty = brain.getNovelty()
level = novelty.getLevel()
history = novelty.getHistory()
novelty.clearHistory()
```

#### PredictionError
```python
pred_error = brain.getPredictionErrorSignal()
error = pred_error.getError()
magnitude = pred_error.getMagnitude()
```

## Complete Example

```python
import pynlm

def run_enhanced_simulation(num_steps=1000):
    """Run a complete Phase 6 simulation with all advanced systems."""
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # 3. Enable all advanced systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 4. Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Phase 6: Language and social learning
        if step % 100 == 0:
            # Simple text input
            text_pattern = agent.encodeText(f"step_{step}")
            
            # Social learning - observe other agent
            if step > 10:
                # Simulate observing another agent's behavior
                agent.observeOtherAgent(action, percept.getVision(), percept.getTouch())
        
        # Print diagnostics
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Dev stage: {brain.getDevelopmentalStage()}")
            print(f"  Concepts: {agent.getConceptCount()}")
            print(f"  Self-model confidence: {agent.getSelfModelConfidence():.3f}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_enhanced_simulation(1000)
```

## Research Capabilities

The enhanced Phase 6 bindings enable advanced research capabilities:

1. **Multi-Agent Studies**: Social learning and imitation
2. **Language Processing**: Text understanding and encoding
3. **Cognitive Modeling**: Planning, concept formation, attention
4. **Neuromodulation Research**: Fine-grained control over learning systems
5. **Memory Research**: Working, episodic, and associative memory studies
6. **Development Research**: Lifespan development and plasticity
7. **Plasticity Research**: Fine-grained control over STDP, Hebbian, and structural plasticity
8. **Statistical Analysis**: Real-time metrics and diagnostic counters
9. **Reproducible Research**: Checkpointing and state management
10. **Behavioral Experiments**: Comprehensive action selection and reward systems

## Performance Considerations

- **Memory Usage**: Working and episodic memory systems can consume significant memory
- **Computation**: Phase 6 cognitive systems add computational overhead
- **Scalability**: Configuration of neuron and synapse counts affects performance
- **Real-time Use**: Consider world timestep and simulation step durations

## Migration Guide

For users upgrading from the basic bindings:

1. **New API Methods**: Most Phase 6 systems are now available as class methods
2. **Enhanced Configuration**: Configuration now supports parameter setting
3. **New Diagnostic Counters**: Access to detailed counters and statistics
4. **Phase 6 Systems**: Full access to cognitive, memory, and neuromodulation systems
5. **Social Learning**: New capabilities for multi-agent studies

## Files

- **pynlm/bindings.cpp**: Enhanced C++ bindings implementation
- **pyproject.toml**: Updated Python package configuration
- **python/CMakeLists.txt**: Enhanced build system
- **ENHANCED_PYEXAMPLE.py**: Complete demonstration of enhanced API
- **QUICK_TEST.md**: Quick validation test script
- **documentation/...**: Comprehensive documentation

## Installation

```bash
# Install the enhanced bindings
pip install -e .

# Or build from source
cd python
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install .
```

## Summary

The enhanced Phase 6 Python bindings transform pynlm from a basic skeleton into a comprehensive framework for neural simulation research, enabling advanced cognitive modeling, social learning, and multi-agent studies with full access to all Phase 6 systems.
