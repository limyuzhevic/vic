# NLM Python Bindings - Enhanced API for Advanced Neural Simulation

## Overview

This document describes the enhanced Python bindings for the NLM (熙然 - "serene flow") neural learning machine. The Python bindings provide comprehensive access to the Phase 6 integrated artificial brain with all its cognitive, memory, prediction, and neuromodulation systems.

## Key Improvements

### 1. **Enhanced Memory Systems Integration**
- **Working Memory**: Transient storage with active maintenance and competitive retrieval
- **Episodic Memory**: Rich experience storage with pattern completion and replay
- **Associative Memory**: Pattern associations with strength-based retrieval
- **Integrated storage**: Memory systems are now connected to sensory processing and action selection

### 2. **Advanced Prediction System**
- **Multi-step prediction**: Predicts future sensory input and action consequences
- **Prediction error signals**: Compute and use prediction errors for learning
- **Temporal context**: Uses episodic memory to provide temporal context for predictions
- **Confidence tracking**: Maintains prediction confidence levels

### 3. **Sophisticated Cognitive Processing**
- **Neural Planner**: Multi-step action planning with temporal reasoning
- **Concept Formation**: Pattern discovery and category formation from experience
- **Attentional Selection**: Competitive attention with top-down and bottom-up bias
- **Integrated cognition**: Cognitive systems work together as a unified architecture

### 4. **Complete Neuromodulation**
- **Dopamine**: Reward prediction error signaling, memory consolidation, attention modulation
- **Curiosity**: Exploration motivation and novel behavior
- **Novelty**: Novelty detection and change response
- **Prediction Error**: Error signals for learning and adaptation

### 5. **Enhanced Developmental Processes**
- **Multi-stage development**: Initial, critical period, maturation, adult, and aging stages
- **Experience-dependent development**: Development affected by learning history
- **Structural plasticity**: Synaptogenesis, pruning, and maturation

### 6. **Robust Persistence**
- **Checkpoint save/load**: Complete brain state serialization with validation
- **Incremental saving**: Support for long-running simulations
- **Version compatibility**: Checkpoint format with version checking
- **Memory state persistence**: Working and episodic memory state saved

## API Overview

### Core Classes

#### `Config`
Configuration management for the neural network:

```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)
```

#### `Brain`
Main neural computation engine with integrated systems:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Basic operations
brain.step(step)
brain.reset()

# Statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

# Memory access
if brain.getWorkingMemory():
    brain.getWorkingMemory().store([0.5, 0.7, 0.9], 1.0)
    
if brain.getEpisodicMemory():
    brain.getEpisodicMemory().storeEpisode({
        'timestamp': step,
        'neurons': [1001, 1002],
        'activations': [0.6, 0.8],
        'reward': 0.5,
        'metadata': 'Test experience'
    })

# Prediction system
if brain.getPredictionSystem():
    error = brain.getPredictionSystem().getPredictionError()
    confidence = brain.getPredictionSystem().getConfidence()

# Cognitive systems
if brain.getPlanner():
    brain.getPlanner().setPlanningDepth(3)

if brain.getConceptFormation():
    concepts = brain.getConceptFormation().getConcepts()

if brain.getAttention():
    brain.getAttention().setInhibitionStrength(0.5)
    brain.getAttention().setExcitationStrength(1.5)

# Neuromodulation
if brain.getDopamine():
    brain.getDopamine().signalReward(0.8)
    brain.getDopamine().signalRewardPredictionError(0.1)

# Persistence
brain.save("checkpoint.nlm")
brain.load("checkpoint.nlm")
```

#### `AgentBrain`
Connects the neural brain to the world:

```python
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Main simulation loop
for step in range(100):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = world.computeReward(agent)
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

#### `SimpleWorld`
Virtual environment for agent interaction:

```python
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

# Set starting position
world.setAgentStart(7.5, 7.5)

# Control and sensing
world.update(timestep)
percept = world.getSensoryPercept()
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())

# Environment management
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
```

#### Memory Classes

**WorkingMemory**: Transient information storage:

```python
if brain.getWorkingMemory():
    wm = brain.getWorkingMemory()
    
    # Store sensory patterns
    wm.store([0.5, 0.7, 0.9, 0.3], 1.0)
    
    # Store to specific neurons
    wm.storeToNeuron(neuron_id, activation)
    
    # Retrieve patterns
    pattern = wm.retrieve()
    
    # Control capacity
    wm.setStabilityFactor(0.8)
    
    # Boost capacity during high dopamine
    wm.boostCapacity(0.5)
```

**EpisodicMemory**: Experience storage and replay:

```python
if brain.getEpisodicMemory():
    em = brain.getEpisodicMemory()
    
    # Store experiences
    em.storeEpisode({
        'timestamp': step,
        'neurons': [1001, 1002, 1003],
        'activations': [0.6, 0.8, 0.4],
        'reward': 0.5,
        'metadata': 'Exploration phase'
    })
    
    # Retrieve similar experiences
    episodes = em.getRecentEpisodes(5)
    similar = em.retrieveSimilar(sensory_pattern, 10)
    
    # Time-based retrieval
    temporal = em.retrieveTemporal(start_step, end_step, 5)
    
    # Consolidation
    em.consolidate(0.7)
    em.setConsolidationFactor(0.8)
```

#### Prediction System

```python
if brain.getPredictionSystem():
    pred = brain.getPredictionSystem()
    
    # Update with context
    recent_episodes = brain.getEpisodicMemory().getRecentEpisodes(3)
    pred.updateWithContext(brain, timestep, recent_episodes)
    
    # Get prediction accuracy
    error = pred.getPredictionError()
    confidence = pred.getConfidence()
```

#### Cognitive Systems

**NeuralPlanner**: Action planning with temporal reasoning:

```python
if brain.getPlanner():
    planner = brain.getPlanner()
    planner.setPlanningDepth(3)
    
    # Get planned actions (implementation-specific)
    plan = planner.getPlan()
```

**ConceptFormation**: Pattern discovery and category formation:

```python
if brain.getConceptFormation():
    cf = brain.getConceptFormation()
    
    # Get discovered concepts
    concepts = cf.getConcepts()
    
    # Get concept strengths
    for concept in concepts:
        strength = cf.getConceptStrength(concept)
```

**AttentionalSelection**: Competitive focus control:

```python
if brain.getAttention():
    attention = brain.getAttention()
    attention.setInhibitionStrength(0.5)
    attention.setExcitationStrength(1.5)
    
    # Use episodic memory context
    recent_episodes = brain.getEpisodicMemory().getRecentEpisodes(5)
    attention.updateWithMemoryContext(recent_episodes)
    
    # Process competition
    competitors = [1001, 1002, 1003]
    attention.processCompetition(competitors)
```

#### Neuromodulation Classes

**Dopamine**: Reward and prediction error processing:

```python
if brain.getDopamine():
    da = brain.getDopamine()
    
    print(f"Level: {da.getLevel():.3f}")
    print(f"Plasticity factor: {da.getPlasticityFactor():.3f}")
    
    # Signal different types of learning
    da.signalReward(0.8)  # Positive reward
    da.signalRewardPredictionError(-0.2)  # Prediction error
    
    # Configure dopamine dynamics
    da.setBaseline(0.1)
    da.setPeak(2.0)
    da.setDecayRate(0.05)
    da.setReleaseRate(1.5)
    
    da.reset()
```

**Curiosity**: Exploration motivation:

```python
if brain.getCuriosity():
    curiosity = brain.getCuriosity()
    print(f"Level: {curiosity.getLevel():.3f}")
```

**Novelty**: Change detection:

```python
if brain.getNovelty():
    novelty = brain.getNovelty()
    print(f"Level: {novelty.getLevel():.3f}")
```

**PredictionError**: Error signaling:

```python
if brain.getPredictionErrorSignal():
    pe = brain.getPredictionErrorSignal()
    print(f"Level: {pe.getLevel():.3f}")
```

### Advanced Functions for Power Users

#### Complex Simulation Functions

**Advanced Simulation with Callback**:

```python
def run_advanced_simulation(brain, world, agent, num_steps, callback):
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        callback(step)

# Usage
pynlm.runAdvancedSimulation(brain, world, agent, 100, 
                           lambda step: print(f"Step {step}"))
```

**Learning Episode with Exploration**:

```python
def run_learning_episode(brain, world, agent, num_steps, exploration_rate):
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Curiosity-driven exploration
        if agent.getCuriosityLevel() > exploration_rate:
            import random
            if random.random() < exploration_rate:
                action = pynlm.ActionType.MoveForward
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = 0.0
        if action == pynlm.ActionType.Interact:
            reward = 1.0
        elif action == pynlm.ActionType.MoveForward:
            reward = 0.2
        
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)

# Usage
pynlm.runLearningEpisode(brain, world, agent, 50, 0.3)
```

**Complex Configuration Builder**:

```python
config = pynlm.createComplexConfig(
    neuronCount=1200,
    regionCount=4,
    connectionProbability=0.15,
    stdpLtpWeight=0.015,
    stdpLtdWeight=0.018,
    synaptogenesisRate=0.0002,
    pruningRate=0.00002
)
```

**Phase 6 Integration Test**:

```python
# Run comprehensive integration test
pynlm.runPhase6IntegrationTest(brain)
```

### Enumerations

#### NeuronType
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

#### ActionType
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Interact
pynlm.ActionType.Wait
pynlm.ActionType.LookLeft
pynlm.ActionType.LookRight
```

#### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

#### WorldObjectType
```python
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

### Error Handling

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Your simulation code
    
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Installation

#### Prerequisites
- Python 3.8 or higher
- C++17 compiler (GCC 9+, Clang 8+, or MSVC 2019+)
- CMake 3.16 or higher

#### From Source

```bash
# Clone the repository
cd /path/to/nlm
cd python

# Install dependencies
pip install pybind11 scikit-build-core numpy

# Build and install
pip install -e .
```

#### Quick Test

```bash
# Test the Python bindings
python -c "import pynlm; print('NLM version:', pynlm.__version__)"

# Run the test suite
cd /path/to/nlm/python
python test_python_bindings.py
```

### Performance Considerations

#### Memory Management
- Working memory has configurable capacity to balance performance and capability
- Episodic memory uses consolidation to manage memory usage
- Neural connectivity is event-driven for efficient spike processing

#### Parallel Processing
- The C++ core supports parallel processing where safe
- Memory pools reduce allocation overhead
- SIMD vectorization improves numerical performance

#### Large-Scale Simulation
- For large networks, use `enableStructuralPlasticity(True)` to control growth
- Enable `enableDevelopment(True)` for experience-dependent development
- Use checkpointing (`save()`/`load()`) for long-running simulations

### Advanced Usage Examples

#### Multi-Agent Simulation
```python
# Create multiple brains and agents
brains = [pynlm.createBrain(config) for _ in range(3)]
agents = [pynlm.createAgentBrain(brain) for brain in brains]

# Share world
world = pynlm.createSimpleWorld()

# Run social learning simulation
for step in range(100):
    for agent in agents:
        percept = world.getSensoryPercept(agent)
        agent.processSensoryInput(percept)
        agent.getBrain().step(step)
        
        # Social learning: observe other agents
        for other_agent in agents:
            if other_agent != agent:
                # Social learning implementation
                pass
        
        action = agent.decodeMotorCommand()
        world.applyAction(agent, action)
```

#### Cognitive Architecture Configuration
```python
# Configure for different cognitive emphases
config = pynlm.createDefaultConfig()

# Memory-intensive configuration
config.set("neuron_count", 2000)
config.set("working_memory_capacity", 200)
config.set("episodic_memory_limit", 5000)

# Prediction-intensive configuration  
config.set("prediction_system_enabled", True)
config.set("planning_depth", 5)

# Development-intensive configuration
config.set("development_enabled", True)
config.set("plasticity_rate", 1.2)
config.set("critical_period_length", 1000)
```

#### Real-Time Control
```python
import threading
import time

def real_time_simulation(brain, world, agent, duration, fps=30):
    start_time = time.time()
    step_time = 1.0 / fps
    
    step = 0
    while time.time() - start_time < duration:
        step_start = time.time()
        
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Check for real-time control signals
        if check_control_signal():
            agent.enableCuriosity(not agent.isCuriosityEnabled())
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        step += 1
        
        # Sleep to maintain target frame rate
        elapsed = time.time() - step_start
        if elapsed < step_time:
            time.sleep(step_time - elapsed)

# Run in background thread
thread = threading.Thread(target=real_time_simulation, args=(brain, world, agent, 60.0))
thread.start()
```

## Version Information

```python
import pynlm
print(f"NLM Version: {pynlm.__version__}")
print(f"Description: {pynlm.__description__}")
```

- **Version**: 0.1.0
- **Build Date**: $(date -u +%Y-%m-%d)
- **API Stability**: Alpha (breaking changes may occur)
- **License**: MIT

## Troubleshooting

### Common Issues

#### Python Import Error
```bash
# Install or upgrade pip
pip install --upgrade pip

# Install pybind11
pip install pybind11

# Reinstall pynlm
pip install --force-reinstall pynlm
```

#### Compilation Errors
```bash
# Ensure you have a C++17 compatible compiler
# On Ubuntu:
sudo apt-get update
sudo apt-get install g++

# Set compiler environment variables
export CC=gcc
export CXX=g++
```

#### Runtime Errors
```python
# Enable logging for debugging
import logging
logging.basicConfig(level=logging.DEBUG)

# Check NLM exceptions
try:
    # Your code
    pass
except pynlm.NLMException as e:
    print(f"NLM error occurred: {e}")
```

#### Performance Issues
```python
# Profile your simulation
import cProfile
import pstats

cProfile.run('run_simulation()', 'simulation_stats.prof')
stats = pstats.Stats('simulation_stats.prof')
stats.sort_stats('cumulative').print_stats(20)
```

## Future Enhancements

The Python bindings will be expanded to include:

1. **WebSocket Interface**: Real-time remote control and monitoring
2. **Machine Learning Integration**: Pre-trained models for initialization
3. **Visualization**: Built-in plotting and visualization tools
4. **Distributed Computing**: Multi-process simulation for large-scale models
5. **Scripting**: Python scripting for custom simulation control
6. **Parameter Optimization**: Automated hyperparameter tuning

## Acknowledgments

This enhanced Python API builds upon the foundational work of the NLM project, integrating all Phase 6 systems into a cohesive, user-friendly interface. The API represents a significant step forward in making advanced neural simulation accessible to researchers, developers, and students.

Special thanks to all contributors who have worked on the NLM architecture, memory systems, prediction systems, cognitive processing, and neuromodulation components that make this integration possible.
