# NLM - Advanced Python Bindings Documentation

This document covers the advanced features available in the enhanced Python bindings for NLM (Neural Learning Machine).

## Overview

The enhanced Python bindings provide comprehensive access to all neural systems, including:

- **Memory Systems**: Working memory, episodic memory, semantic memory, procedural memory, associative memory
- **Prediction Systems**: State prediction, error computation, confidence tracking
- **Neuromodulation Systems**: Dopamine, curiosity, novelty, prediction error
- **Development Systems**: Developmental stages, plasticity modulation, maturation
- **Cognition Systems**: Neural planning, concept formation, attention
- **Advanced Utilities**: Batch processing, performance monitoring, debugging tools

## Quick Start Examples

### 1. Complete Brain with All Systems

```python
import pynlm

# Create advanced brain with all subsystems
config = pynlm.createAdvancedBrain()
brain = pynlm.createBrain(config)
brain.initialize()

# Setup world and agent
world = pynlm.createArenaWorld()
agent = pynlm.createSimpleAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableNovelty(True)

# Run simulation with monitoring
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    if step % 100 == 0:
        stats = brain.getBrainStats()
        print(f"Step {step}: Firing={stats['firing_neurons']}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}")
```

### 2. Memory Systems Demonstration

```python
import pynlm

# Create brain and initialize
brain = pynlm.createBasicBrain()
brain.initialize()

# Working Memory - Store and retrieve patterns
brain.wmStore([0.1, 0.5, 0.9, 0.3, 0.7])  # Store pattern
wm_content = brain.wmRetrieve()
print(f"Working memory content: {wm_content}")
print(f"Contains pattern values: {brain.wmContains(neuron_id)}")

# Episodic Memory - Store experiences
episode = pynlm.EpisodicMemoryItem()
episode.positionX = 10.0
episode.positionY = 15.0
episode.action = pynlm.ActionType.Eat
episode.reward = 1.0
brain.emStoreEpisode(episode)
print(f"Stored episode at position ({episode.positionX}, {episode.positionY})")

# Semantic Memory - Store facts
brain.smStoreFact("color_red", "#FF0000")
brain.smStoreFact("shape_circle", "round")
red_is_round = brain.smRetrieveFact("color_red") is not None
print(f"Semantic fact stored: {red_is_round}")

# Procedural Memory - Learn skills
brain.pmLearnSkill("jump", [0.8, 0.9, 0.1])
skill = brain.pmGetSkill("jump")
print(f"Learned skill: {skill}")

# Associative Memory - Create associations
brain.amAssociate([0.1, 0.2], [0.3, 0.4], 0.8)
strength = brain.amGetAssociationStrength([0.1, 0.2], [0.3, 0.4])
print(f"Association strength: {strength}")
```

### 3. Prediction and Neuromodulation

```python
import pynlm

# Create exploration scenario
brain = pynlm.createExplorationScenario()
world = brain["world"]
agent = brain["agent"]

# Run episode with prediction and neuromodulation
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Apply reward modulation
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Check prediction error
    prediction_error = agent.getPredictionError()
    if abs(prediction_error) > 0.1:
        print(f"Step {step}: High prediction error = {prediction_error:.3f}")
    
    # Monitor neuromodulation levels
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    if curiosity > 0.8:
        print(f"Step {step}: High curiosity = {curiosity:.3f}")
```

### 4. Advanced Cognition and Development

```python
import pynlm

# Create development scenario
brain = pynlm.createDevelopmentScenario()

# Set developmental stage to critical period
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
print(f"Current developmental stage: {brain.getDevelopmentalStage()}")

# Neural Planning - Plan actions
brain.setPlanningDepth(3)
current_state = [0.1, 0.2, 0.3, 0.4, 0.5]
planned_action = brain.planAction(current_state)
print(f"Planned action: {planned_action}")

# Concept Formation - Learn concepts
concept_id = brain.learnConcept(
    pattern=[0.7, 0.8, 0.9],
    features=[0.1, 0.2],
    reward=1.0,
    currentTime=100
)
print(f"Discovered concept ID: {concept_id}")

# Attention System - Focus attention
brain.focusAttention(pynlm.RegionId(0))
attention_focus = brain.getAttentionalFocus()
print(f"Attention focused on regions: {attention_focus}")

# Run development
brain.develop()
print(f"Developmental stage after development: {brain.getDevelopmentalStage()}")
```

### 5. Batch Processing and Performance Monitoring

```python
import pynlm

# Create multiple brains for comparison
brain1 = pynlm.createBasicBrain()
brain2 = pynlm.createAdvancedBrain()
brain3 = pynlm.createQuickTestBrain()

brains = [brain1, brain2, brain3]
steps = [100, 200, 150]

# Initialize all brains
for brain in brains:
    brain.initialize()

# Run batch processing
results = pynlm.runMultipleBrainsParallel(brains, steps)
print("Batch processing results:")
for result in results:
    print(f"  Brain {result['brain_index']}: {result['steps']} steps")

# Monitor performance
performance = pynlm.profilePerformance(brains, steps)
print(f"\nPerformance report:")
print(f"  Total execution time: {performance['total_execution_time_ms']} ms")
print(f"  Brains processed: {performance['brains_processed']}")
print(f"  Steps per second: {performance['steps_per_second']:.2f}")

# Get detailed performance for each brain
for brain_perf in performance['brain_performance']:
    print(f"  Brain {brain_perf['brain_index']}:")
    print(f"    Neurons: {brain_perf['neuron_count']}")
    print(f"    Synapses: {brain_perf['synapse_count']}")
    print(f"    Final spikes: {brain_perf['final_spike_count']}")
```

### 6. Real-time Monitoring and Anomaly Detection

```python
import pynlm
import time

# Create brain and world
brain = pynlm.createExplorationScenario()
world = brain["world"]
agent = brain["agent"]

# Start real-time monitoring
print("Starting real-time monitoring...")
monitoring_data = pynlm.monitorSimulationRealTime(
    brain, 
    list(range(100)),  # steps to simulate
    0.1  # timestep
)

# Analyze monitoring data for anomalies
anomalies = pynlm.detectAnomalies(brain, list(range(100)), 0.1)

print(f"\nMonitoring completed:")
print(f"  Total monitoring points: {len(monitoring_data)}")
print(f"  Spike anomalies detected: {len(anomalies['spike_anomalies'])}")
print(f"  Firing rate anomalies detected: {len(anomalies['firing_rate_anomalies'])}")
print(f"  Connectivity anomalies detected: {len(anomalies['connectivity_anomalies'])}")

# Show sample monitoring data
if monitoring_data:
    sample = monitoring_data[0]
    print(f"\nSample monitoring data (step 0):")
    for key, value in sample.items():
        print(f"  {key}: {value}")
```

### 7. System Health and Connectivity Analysis

```python
import pynlm

# Create multiple brains for health analysis
brains = [
    pynlm.createBasicBrain(),
    pynlm.createAdvancedBrain(),
    pynlm.createQuickTestBrain()
]

brain_names = ["Basic Brain", "Advanced Brain", "Test Brain"]

# Initialize brains
for brain in brains:
    brain.initialize()

# Get system health reports
health_reports = pynlm.getSystemHealth(brains, brain_names)

print("System Health Analysis:")
for report in health_reports:
    print(f"\n{report['name']}:")
    print(f"  Status: {report['status']}")
    print(f"  Neurons: {report['neuron_count']}")
    print(f"  Synapses: {report['synapse_count']}")
    print(f"  Development Stage: {report['developmental_stage']}")
    print(f"  E/I Ratio: {report['e_i_ratio']:.3f}")
    print(f"  Balance: {report['e_i_status']}")
    
    if report['status'] == 'warning' and 'issues' in report:
        print(f"  Issues: {report['issues']}")

# Connectivity analysis for each brain
print("\nConnectivity Analysis:")
for i, brain in enumerate(brains):
    analysis = pynlm.analyzeConnectivity(brain)
    print(f"\nBrain {i+1}:")
    print(f"  Total Neurons: {analysis['total_neurons']}")
    print(f"  Total Synapses: {analysis['total_synapses']}")
    print(f"  E/I Balance: {analysis['e_i_balance']}")
    print(f"  Network Efficiency: {analysis['network_efficiency']}")
    print(f"  Stability: {analysis['stability_indication']}")
```

## Advanced Use Cases

### Exploration Scenario

```python
import pynlm

# Create exploration scenario with curiosity and reward learning
brain = pynlm.createExplorationScenario()
world = brain["world"]
agent = brain["world"]["agent"]  # This is just an example, fix the reference

# Run exploration episode
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Apply reward-based learning
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 50 == 0:
        print(f"Exploration step {step}: "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}")
```

### Learning Scenario

```python
import pynlm

# Create learning scenario with structural plasticity
brain = pynlm.createLearningScenario()
world = brain["world"]
agent = brain["world"]["agent"]

# Run learning episode with structural changes
for step in range(800):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Apply reward modulation
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Learning step {step}: "
              f"Structural Plasticity={agent.isStructuralPlasticityEnabled()}, "
              f"Dev Stage={agent.getDevelopmentalStage()}")
```

### Development Scenario

```python
import pynlm

# Create development scenario
brain = pynlm.createDevelopmentScenario()
world = brain["world"]
agent = brain["world"]["agent"]

# Run developmental progression
for step in range(300):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Monitor developmental stage
    current_stage = agent.getDevelopmentalStage()
    print(f"Step {step}: Developmental Stage = {current_stage}")
    
    if current_stage == pynlm.DevelopmentalStage.Adult:
        print("Brain reached adulthood!")
        break
```

## API Reference Summary

### Core Classes

#### Brain Class
- **Methods**: `step()`, `produceAction()`, `reset()`, `save()`, `load()`
- **Memory Access**: `getWorkingMemory()`, `getEpisodicMemory()`, etc.
- **Prediction System**: `getPredictionSystem()`
- **Neuromodulation**: `getDopamine()`, `getCuriosity()`, etc.
- **Development**: `getDevelopmentSystem()`
- **Cognition**: `getPlanner()`, `getConceptFormation()`, `getAttention()`
- **Advanced Methods**: `wmStore()`, `wmRetrieve()`, `emStoreEpisode()`, etc.

#### Config Class
- **Methods**: `loadFromFile()`, `saveToFile()`, `set()`, `get()`, `has()`

#### AgentBrain Class
- **Methods**: `processSensoryInput()`, `decodeMotorCommand()`, `applyRewardModulation()`
- **Development**: `updateDevelopment()`, `getDevelopmentalStage()`
- **Neuromodulation**: `getCuriosityLevel()`, `getNoveltyLevel()`, `getPredictionError()`
- **Subsystem Control**: `enableRewardModulation()`, `enableStructuralPlasticity()`, etc.

#### SimpleWorld Class
- **Methods**: `configure()`, `reset()`, `update()`, `applyMotorCommand()`
- **State Access**: `getSensoryPercept()`, `getAgentBody()`

### Advanced Functions

#### Batch Processing
- `runMultipleBrainsParallel(brains, steps, timestep)`
- `profilePerformance(brains, steps, timestep)`

#### Monitoring and Analysis
- `monitorSimulationRealTime(brain, steps, timestep)`
- `detectAnomalies(brain, steps, timestep)`
- `analyzeConnectivity(brain)`
- `getSystemHealth(brains, names, detailed_analysis)`

#### Scenario Creation
- `createExplorationScenario()`
- `createLearningScenario()`
- `createDevelopmentScenario()`
- `createArenaWorld()`, `createMazeWorld()`

## Performance Tips

1. **For Large Simulations**: Use `createQuickTestBrain()` with smaller neuron counts for faster testing
2. **For Real-time Applications**: Enable batch processing and use `runMultipleBrainsParallel()`
3. **For Monitoring**: Use `monitorSimulationRealTime()` to track system state during operation
4. **For Debugging**: Use `detectAnomalies()` to identify issues in neural dynamics
5. **For Comparative Analysis**: Use `getSystemHealth()` to compare different brain configurations

## Error Handling

The enhanced bindings include comprehensive error handling:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Operations that can fail
    if not brain.getWorkingMemory().contains(neuron_id):
        print("Neuron not in working memory")
        
except Exception as e:
    print(f"Error: {e}")
```

## Memory Management

The bindings use `std::shared_ptr` for automatic memory management. Objects are copied when passed to Python and references are maintained internally.

## Thread Safety

The bindings are not thread-safe for concurrent modifications. Use separate brain instances for parallel processing.

## Version Information

- **NLM Version**: 2.0.0
- **Python Bindings**: Enhanced with advanced features
- **Build**: Built with pybind11 and scikit-build-core

---

This documentation covers the enhanced Python bindings for NLM. For more detailed information or specific use cases, refer to the individual function documentation or example scripts.
