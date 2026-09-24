# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API with new integrated features
4. Comprehensive documentation resources

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with performance optimizations
cmake .. -DNLM_PERFORMANCE_ENABLED=ON
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library with Enhanced Features

### Basic Usage with New Phase 6 Features

#### Creating a Brain with Enhanced Configuration

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Configure new Phase 6 features
# Enable predictive coding
config.set("neuromod.prediction.enable", True)
config.set("neuromod.prediction.confidence_threshold", 0.5)

# Configure memory systems
config.set("memory.working_memory_capacity", 1000)
config.set("memory.episodic_max_episodes", 1000)
config.set("memory.associative_threshold", 0.7)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.threshold", 0.3)
config.set("neuromod.novelty.enable", True)

# Configure concept formation
config.set("cognition.concept_learning_rate", 0.01)
config.set("cognition.concept_stability_threshold", 0.8)
config.set("cognition.max_concepts", 100)

# Configure development
config.set("development.planning_depth", 5)
config.set("development.enable_age_effects", True)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Working memory capacity: {brain.getWorkingMemory().getCapacity()}")
print(f"Episodic episodes: {brain.getEpisodicMemory().getMaxEpisodes()}")
```

#### Running a Simulation with All Integrated Systems

```python
import pynlm

def run_phase6_simulation(num_steps=1000):
    """Run a complete Phase 6 simulation with all integrated systems."""
    
    # 1. Create configuration with all Phase 6 features
    config = pynlm.createDefaultConfig()
    
    # Configure for complex behavior
    config.set("brain.neuron_count", 5000)
    config.set("memory.working_memory_capacity", 500)
    config.set("memory.episodic_max_episodes", 500)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction.enable", True)
    config.set("cognition.concept_learning_rate", 0.01)
    config.set("cognition.concept_stability_threshold", 0.8)
    config.set("development.enable_age_effects", True)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePredictiveCoding(True)
    agent.enableConceptFormation(True)
    
    # 7. Configure memory systems
    if agent.getWorkingMemory():
        agent.getWorkingMemory().setCapacity(500)
    if agent.getEpisodicMemory():
        agent.getEpisodicMemory().setMaxEpisodes(500)
    
    # 8. Configure prediction system
    if agent.getPredictionSystem():
        agent.getPredictionSystem().setConfidenceThreshold(0.5)
    
    # 9. Configure concept formation
    if agent.getConceptFormation():
        agent.getConceptFormation().setLearningRate(0.01)
        agent.getConceptFormation().setStabilityThreshold(0.8)
    
    # 10. Configure attention
    if agent.getAttention():
        agent.getAttention().setInhibitionStrength(0.5)
        agent.getAttention().setExcitationStrength(1.5)
    
    # 11. Configure development
    if agent.getDevelopment():
        agent.getDevelopment().setPlanningDepth(5)
        agent.getDevelopment().enableAgeEffects(True)
    
    # 12. Configure neuromodulation
    if agent.getDopamine():
        agent.getDopamine().setScale(1.0)
    if agent.getCuriosity():
        agent.getCuriosity().setThreshold(0.3)
    if agent.getNovelty():
        agent.getNovelty().setThreshold(0.2)
    
    # 13. Run simulation loop
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
        
        # Print comprehensive progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Working memory traces: {agent.getWorkingMemory().getActiveTraces() if agent.getWorkingMemory() else 0}")
            print(f"  Episodic episodes: {agent.getEpisodicMemory().getEpisodeCount() if agent.getEpisodicMemory() else 0}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"  Concepts formed: {agent.getConceptFormation().getConceptCount() if agent.getConceptFormation() else 0}")
            print(f"  Attention winners: {agent.getAttention().getWinnerCount() if agent.getAttention() else 0}")
    
    return brain, agent, world

# Run the simulation
if __name__ == "__main__":
    brain, agent, world = run_phase6_simulation(1000)
    print("Phase 6 simulation complete!")
```

### Enhanced Agent Example with New Features

```python
import pynlm

def run_advanced_agent_simulation(num_steps=1000):
    """Run an advanced agent simulation with all Phase 6 features."""
    
    # 1. Create configuration with optimal settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("memory.working_memory_capacity", 300)
    config.set("memory.episodic_max_episodes", 300)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.2)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.15)
    config.set("neuromod.prediction.enable", True)
    config.set("neuromod.prediction.confidence_threshold", 0.6)
    config.set("cognition.concept_learning_rate", 0.02)
    config.set("cognition.concept_stability_threshold", 0.7)
    config.set("cognition.max_concepts", 200)
    config.set("development.enable_age_effects", True)
    config.set("development.planning_depth", 3)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.005)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create world with obstacles
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Add some obstacles
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Wall, value=1.0))
    world.addObject(pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Resource, value=2.0))
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePredictiveCoding(True)
    agent.enableConceptFormation(True)
    
    # 7. Configure memory systems
    if agent.getWorkingMemory():
        agent.getWorkingMemory().setCapacity(300)
        agent.getWorkingMemory().setDecayRate(0.01)
    if agent.getEpisodicMemory():
        agent.getEpisodicMemory().setMaxEpisodes(300)
        agent.getEpisodicMemory().setReplayInterval(50)
    
    # 8. Configure prediction system
    if agent.getPredictionSystem():
        agent.getPredictionSystem().setConfidenceThreshold(0.6)
        agent.getPredictionSystem().setErrorDecay(0.95)
    
    # 9. Configure concept formation
    if agent.getConceptFormation():
        agent.getConceptFormation().setLearningRate(0.02)
        agent.getConceptFormation().setStabilityThreshold(0.7)
        agent.getConceptFormation().setGeneralizationEnabled(True)
        agent.getConceptFormation().setMaxConcepts(200)
    
    # 10. Configure attention
    if agent.getAttention():
        agent.getAttention().setInhibitionStrength(0.6)
        agent.getAttention().setExcitationStrength(1.4)
        agent.getAttention().enableTopDownBias(True)
    
    # 11. Configure development
    if agent.getDevelopment():
        agent.getDevelopment().setPlanningDepth(3)
        agent.getDevelopment().enableAgeEffects(True)
        agent.getDevelopment().setPlasticityModulation(True)
    
    # 12. Configure neuromodulation
    if agent.getDopamine():
        agent.getDopamine().setScale(1.0)
        agent.getDopamine().setResponseGain(0.5)
    if agent.getCuriosity():
        agent.getCuriosity().setThreshold(0.2)
        agent.getCuriosity().setDecayRate(0.98)
    if agent.getNovelty():
        agent.getNovelty().setThreshold(0.15)
        agent.getNovelty().setResponseGain(0.3)
    
    # 13. Setup tracking for analysis
    curiosity_history = []
    novelty_history = []
    prediction_error_history = []
    concept_count_history = []
    
    # 14. Run simulation loop
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
        
        # Calculate reward from world state
        reward = 0.0
        if world.getAgentBody():
            # Check for resources (positive reward)
            for obj in world.getObjects():
                if obj.type == pynlm.WorldObjectType.Resource:
                    dist = ((obj.x - world.getAgentBody().x) ** 2 + 
                           (obj.y - world.getAgentBody().y) ** 2) ** 0.5
                    if dist < 2.0:
                        reward += obj.value
                # Check for hazards (negative reward)
                elif obj.type == pynlm.WorldObjectType.Hazard:
                    dist = ((obj.x - world.getAgentBody().x) ** 2 + 
                           (obj.y - world.getAgentBody().y) ** 2) ** 0.5
                    if dist < 2.0:
                        reward -= obj.value
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record statistics for analysis
        curiosity_history.append(agent.getCuriosityLevel())
        novelty_history.append(agent.getNoveltyLevel())
        prediction_error_history.append(agent.getPredictionError())
        concept_count_history.append(agent.getConceptFormation().getConceptCount() 
                                    if agent.getConceptFormation() else 0)
        
        # Print detailed progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Working memory: {agent.getWorkingMemory().getActiveTraces() if agent.getWorkingMemory() else 0}")
            print(f"  Episodic memory: {agent.getEpisodicMemory().getEpisodeCount() if agent.getEpisodicMemory() else 0}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"  Concepts: {agent.getConceptFormation().getConceptCount() if agent.getConceptFormation() else 0}")
            print(f"  Attention winners: {agent.getAttention().getWinnerCount() if agent.getAttention() else 0}")
            print(f"  Reward: {reward:.3f}")
            
            # Check for completion criteria
            if agent.getCuriosityLevel() > 0.8 and agent.getNoveltyLevel() > 0.8:
                print("  -> High exploration motivation detected")
            if agent.getPredictionError() < 0.1:
                print("  -> Stable prediction detected")
            if agent.getConceptFormation() and agent.getConceptFormation().getConceptCount() > 10:
                print("  -> Rich conceptual knowledge developed")
    
    # 15. Analysis and reporting
    print("\n=== Simulation Analysis ===")
    print(f"Average curiosity: {sum(curiosity_history)/len(curiosity_history):.3f}")
    print(f"Average novelty: {sum(novelty_history)/len(novelty_history):.3f}")
    print(f"Average prediction error: {sum(prediction_error_history)/len(prediction_error_history):.3f}")
    print(f"Final concepts: {concept_count_history[-1]}")
    print(f"Concept growth: {concept_count_history[-1] - concept_count_history[0]}")
    
    return brain, agent, world

# Run the advanced simulation
if __name__ == "__main__":
    brain, agent, world = run_advanced_agent_simulation(1000)
    print("Advanced Phase 6 simulation complete!")
```

### Key Classes and Methods (Enhanced)

#### Brain Class (Enhanced)

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# Memory system access
brain.getWorkingMemory()          # Access working memory
brain.getEpisodicMemory()         # Access episodic memory
brain.getAssociativeMemory()      # Access associative memory
brain.getNeuralPrediction()       # Access neural prediction
brain.getConceptFormation()       # Access concept formation
brain.getAttention()              # Access attention system
brain.getPlanner()                # Access neural planner

# Development system
brain.getDevelopmentalStage()    # Get current developmental stage
brain.setDevelopmentalStage(stage)  # Set developmental stage

# Neuromodulation
brain.getDopamine()              # Access dopamine system
brain.getCuriosity()              # Access curiosity system
brain.getNovelty()                # Access novelty system
brain.getPredictionErrorSignal()  # Access prediction error

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class (Enhanced)

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)
config.set("memory.working_memory_capacity", 500)
config.set("memory.episodic_max_episodes", 500)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.prediction.enable", True)
config.set("cognition.concept_learning_rate", 0.01)
config.set("development.enable_age_effects", True)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class (Enhanced)

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)
agent.enablePredictiveCoding(True/False)
agent.enableConceptFormation(True/False)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
agent.enableNovelty(True/False)
agent.enablePredictiveCoding(True/False)
agent.enableConceptFormation(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
agent.getConceptCount()
agent.getPredictionAccuracy()
```

#### SimpleWorld Class (Enhanced)

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)

# New Phase 6 world features
world.enablePredictiveCoding(True)
world.setReplayInterval(100)
world.enableMemoryReplay(True)
```

### Enumerations (Enhanced)

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
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Wait
pynlm.ActionType.Explore
pynlm.ActionType.Learn

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
pynlm.WorldObjectType.Obstacle
pynlm.WorldObjectType.Goal
```

### New Python API Methods

#### Memory System Methods

```python
# Working memory
brain.getWorkingMemory().getCapacity()                    # Get capacity
brain.getWorkingMemory().setCapacity(500)                  # Set capacity
brain.getWorkingMemory().getActiveTraces()                 # Get active traces
brain.getWorkingMemory().getDecayRate()                    # Get decay rate
brain.getWorkingMemory().setDecayRate(0.01)                 # Set decay rate
brain.getWorkingMemory().getMaxAge()                       # Get max age
brain.getWorkingMemory().setMaxAge(1000)                    # Set max age

# Episodic memory
brain.getEpisodicMemory().getMaxEpisodes()                 # Get max episodes
brain.getEpisodicMemory().setMaxEpisodes(500)               # Set max episodes
brain.getEpisodicMemory().getEpisodeCount()                 # Get episode count
brain.getEpisodicMemory().getReplayInterval()               # Get replay interval
brain.getEpisodicMemory().setReplayInterval(100)            # Set replay interval
brain.getEpisodicMemory().enableReplay(True/False)         # Enable/disable replay

# Associative memory
brain.getAssociativeMemory().getAssociationStrength(1000) # Get strength
brain.getAssociativeMemory().setAssociationStrength(1000, 0.8)  # Set strength
brain.getAssociativeMemory().getCompletionThreshold()       # Get completion threshold
brain.getAssociativeMemory().setCompletionThreshold(0.7)    # Set completion threshold
brain.getAssociativeMemory().enableCompletion(True/False)  # Enable/disable completion

# Neural prediction
brain.getNeuralPrediction().getConfidenceThreshold()        # Get confidence threshold
brain.getNeuralPrediction().setConfidenceThreshold(0.5)     # Set confidence threshold
brain.getNeuralPrediction().enableConfidenceTracking(True/False)  # Enable tracking
brain.getNeuralPrediction().getErrorDecay()                 # Get error decay
brain.getNeuralPrediction().setErrorDecay(0.95)              # Set error decay
```

#### Neuromodulation Methods

```python
# Dopamine
brain.getDopamine().getScale()                             # Get scale
brain.getDopamine().setScale(1.0)                           # Set scale
brain.getDopamine().getResponseGain()                       # Get response gain
brain.getDopamine().setResponseGain(0.5)                    # Set response gain
brain.getDopamine().getLearningRate()                       # Get learning rate
brain.getDopamine().setLearningRate(0.001)                   # Set learning rate

# Curiosity
brain.getCuriosity().getThreshold()                         # Get threshold
brain.getCuriosity().setThreshold(0.3)                       # Set threshold
brain.getCuriosity().getDecayRate()                         # Get decay rate
brain.getCuriosity().setDecayRate(0.98)                      # Set decay rate
brain.getCuriosity().getExplorationBias()                   # Get exploration bias
brain.getCuriosity().setExplorationBias(0.5)                # Set exploration bias

# Novelty
brain.getNovelty().getThreshold()                           # Get threshold
brain.getNovelty().setThreshold(0.2)                         # Set threshold
brain.getNovelty().getResponseGain()                        # Get response gain
brain.getNovelty().setResponseGain(0.3)                      # Set response gain
brain.getNovelty().getMemoryLength()                        # Get memory length
brain.getNovelty().setMemoryLength(100)                      # Set memory length

# Prediction error
brain.getPredictionErrorSignal().getErrorThreshold()        # Get error threshold
brain.getPredictionErrorSignal().setErrorThreshold(0.1)      # Set error threshold
brain.getPredictionErrorSignal().getLearningRate()           # Get learning rate
brain.getPredictionErrorSignal().setLearningRate(0.001)       # Set learning rate
```

#### Cognition Methods

```python
# Concept formation
brain.getConceptFormation().getLearningRate()                # Get learning rate
brain.getConceptFormation().setLearningRate(0.01)            # Set learning rate
brain.getConceptFormation().getStabilityThreshold()          # Get stability threshold
brain.getConceptFormation().setStabilityThreshold(0.8)        # Set stability threshold
brain.getConceptFormation().getMaxConcepts()                 # Get max concepts
brain.getConceptFormation().setMaxConcepts(200)               # Set max concepts
brain.getConceptFormation().enableGeneralization(True/False)  # Enable/disable generalization
brain.getConceptFormation().getSimilarityThreshold()          # Get similarity threshold
brain.getConceptFormation().setSimilarityThreshold(0.7)       # Set similarity threshold
brain.getConceptFormation().getConceptCount()                # Get concept count

# Neural planner
brain.getPlanner().getPlanningDepth()                        # Get planning depth
brain.getPlanner().setPlanningDepth(5)                        # Set planning depth
brain.getPlanner().getMaxPlans()                             # Get max plans
brain.getPlanner().setMaxPlans(100)                           # Set max plans
brain.getPlanner().enableActionEvaluation(True/False)        # Enable/disable action evaluation
brain.getPlanner().getEvaluationConfidence()                 # Get evaluation confidence
brain.getPlanner().setEvaluationConfidence(0.8)              # Set evaluation confidence
```

#### Development Methods

```python
# Development system
brain.getDevelopmentSystem().getPlanningDepth()              # Get planning depth
brain.getDevelopmentSystem().setPlanningDepth(3)              # Set planning depth
brain.getDevelopmentSystem().enableAgeEffects(True/False)   # Enable/disable age effects
brain.getDevelopmentSystem().enablePlasticityModulation(True/False)  # Enable/disable plasticity modulation
brain.getDevelopmentSystem().getDevelopmentalStage()          # Get current developmental stage
brain.getDevelopmentSystem().setDevelopmentalStage(stage)    # Set developmental stage
```

### Configuration Options (New Phase 6 Features)

#### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory_capacity` | int | 1000 | Capacity of working memory (number of traces) |
| `memory.episodic_max_episodes` | int | 1000 | Maximum number of episodes in episodic memory |
| `memory.associative_threshold` | float | 0.5 | Association strength threshold |
| `memory.replay_interval` | int | 100 | Number of steps between memory replay |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |
| `neuromod.dopamine.response_gain` | float | 0.5 | Dopamine response gain |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.threshold` | float | 0.3 | Curiosity activation threshold |
| `neuromod.curiosity.decay_rate` | float | 0.98 | Curiosity decay rate per step |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.2 | Novelty detection threshold |
| `neuromod.novelty.response_gain` | float | 0.3 | Novelty response gain |
| `neuromod.prediction.enable` | bool | true | Enable predictive coding |
| `neuromod.prediction.confidence_threshold` | float | 0.5 | Confidence threshold for prediction |
| `neuromod.prediction.error_decay` | float | 0.95 | Prediction error decay rate |

#### Cognition Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.concept_learning_rate` | float | 0.01 | Learning rate for concept formation |
| `cognition.concept_stability_threshold` | float | 0.8 | Stability threshold for concepts |
| `cognition.max_concepts` | int | 100 | Maximum number of concepts |
| `cognition.generalization_enabled` | bool | true | Enable concept generalization |
| `cognition.similarity_threshold` | float | 0.7 | Similarity threshold for concept grouping |

#### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.planning_depth` | int | 5 | Planning depth for action sequences |
| `development.enable_age_effects` | bool | true | Enable age-dependent changes |
| `development.plasticity_modulation` | bool | true | Enable developmental modulation of plasticity |
| `development.critical_period_duration` | int | 1000 | Duration of critical period |

### Example Scripts (New Phase 6 Examples)

#### Minimal Predictive Coding Example

```python
import pynlm

# Create and initialize with predictive coding
config = pynlm.createDefaultConfig()
config.set("neuromod.prediction.enable", True)
config.set("neuromod.prediction.confidence_threshold", 0.5)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with prediction
for step in range(100):
    brain.step(step)
    
    # Get prediction based on current sensory state
    prediction = brain.getNeuralPrediction()
    if prediction:
        confidence = prediction.getConfidence()
        error = prediction.getPredictionError()
        print(f"Step {step}: Prediction confidence={confidence:.3f}, error={error:.3f}")

print("Predictive coding simulation complete!")
```

#### Concept Formation Example

```python
import pynlm

# Create and initialize with concept formation
config = pynlm.createDefaultConfig()
config.set("cognition.concept_learning_rate", 0.02)
config.set("cognition.concept_stability_threshold", 0.7)
config.set("cognition.max_concepts", 50)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with concept formation
for step in range(200):
    brain.step(step)
    
    # Get sensory input and present to concept formation
    # (This would typically come from the world/environment)
    sensory_pattern = [step % 10, (step * 2) % 10, step % 5]  # Example pattern
    
    concept_id = brain.getConceptFormation().getMatchingConcept(sensory_pattern)
    if concept_id > 0:
        concept = brain.getConceptFormation().getConcept(concept_id)
        print(f"Step {step}: Matched concept {concept_id} with stability {concept.stability:.3f}")

print("Concept formation simulation complete!")
```

#### Neuromodulation Example

```python
import pynlm

# Create and initialize with enhanced neuromodulation
config = pynlm.createDefaultConfig()
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.threshold", 0.2)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.threshold", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with neuromodulation
for step in range(150):
    brain.step(step)
    
    # Get neuromodulation levels
    curiosity = brain.getCuriosity().getLevel()
    novelty = brain.getNovelty().getLevel()
    dopamine = brain.getDopamine().getLevel()
    
    print(f"Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}, Dopamine={dopamine:.3f}")
    
    # Apply neuromodulation effects
    brain.applyNeuromodulation(curiosity + novelty)

print("Neuromodulation simulation complete!")
```

#### Development Example

```python
import pynlm

# Create and initialize with developmental stages
config = pynlm.createDefaultConfig()
config.set("development.enable_age_effects", True)
config.set("development.planning_depth", 3)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with development
for step in range(300):
    brain.step(step)
    
    # Get developmental stage
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: Developmental stage {stage}")
    
    # Observe developmental effects
    if stage == pynlm.DevelopmentalStage.CriticalPeriod:
        print("  -> Critical period: high plasticity")
    elif stage == pynlm.DevelopmentalStage.Adult:
        print("  -> Adult: stable behavior")
    elif stage == pynlm.DevelopmentalStage.Aging:
        print("  -> Aging: declining plasticity")

print("Development simulation complete!")
```

---

## Part 4: Configuration Options

### Brain Configuration

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

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.005 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.structural.synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `plasticity.structural.pruning_rate` | float | 0.00001 | Rate of synapse elimination |

### Enhanced Phase 6 Configuration

#### Memory System Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory_capacity` | int | 1000 | Number of traces working memory can hold |
| `memory.episodic_max_episodes` | int | 1000 | Maximum episodes in episodic memory |
| `memory.episodic_replay_interval` | int | 100 | Steps between memory replay (×1000 = consolidation) |
| `memory.associative_threshold` | float | 0.5 | Association strength threshold for completion |
| `memory.associative_decay_rate` | float | 0.95 | Decay rate for associations |

#### Neuromodulation System Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Global dopamine scaling factor |
| `neuromod.dopamine.response_gain` | float | 0.5 | Dopamine response gain |
| `neuromod.dopamine.learning_rate` | float | 0.001 | Dopamine learning rate |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.curiosity.threshold` | float | 0.3 | Threshold for curiosity activation |
| `neuromod.curiosity.decay_rate` | float | 0.98 | Decay rate for curiosity (≈ 20 steps half-life) |
| `neuromod.curiosity.exploration_bias` | float | 0.5 | Bias toward exploration vs. exploitation |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.threshold` | float | 0.2 | Threshold for novelty detection |
| `neuromod.novelty.response_gain` | float | 0.3 | Novelty response gain |
| `neuromod.novelty.memory_length` | int | 100 | Number of past states to compare for novelty |
| `neuromod.prediction.enable` | bool | true | Enable predictive coding |
| `neuromod.prediction.confidence_threshold` | float | 0.5 | Confidence threshold for prediction use |
| `neuromod.prediction.error_decay` | float | 0.95 | Prediction error decay rate |
| `neuromod.prediction.learning_rate` | float | 0.001 | Prediction learning rate |

#### Cognition System Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `cognition.concept_learning_rate` | float | 0.01 | Learning rate for concept formation |
| `cognition.concept_stability_threshold` | float | 0.8 | Minimum stability for concept retention |
| `cognition.max_concepts` | int | 100 | Maximum number of concepts to store |
| `cognition.generalization_enabled` | bool | true | Enable concept generalization |
| `cognition.similarity_threshold` | float | 0.7 | Similarity threshold for concept grouping |
| `cognition.similarity_metric` | string | "euclidean" | Similarity metric for concepts |
| `cognition.aging_factor` | float | 0.01 | Rate at which concepts degrade with age |

#### Development System Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.planning_depth` | int | 5 | Number of steps ahead to plan |
| `development.enable_age_effects` | bool | true | Enable age-dependent developmental stages |
| `development.plasticity_modulation` | bool | true | Enable modulation of plasticity by development |
| `development.critical_period_duration` | int | 1000 | Number of steps for critical period (≈ 20s at 0.5 timestep) |
| `development.plasticity_decay_rate` | float | 0.99 | Rate of plasticity decay during aging |
| `development.excitability_modulation` | bool | true | Enable modulation of neural excitability by development |

### Agent Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `agent.reward_modulation.enable` | bool | true | Enable reward-based modulation |
| `agent.structural_plasticity.enable` | bool | true | Enable structural plasticity |
| `agent.development.enable` | bool | true | Enable developmental processes |
| `agent.curiosity.enable` | bool | true | Enable curiosity-driven behavior |
| `agent.novelty.enable` | bool | true | Enable novelty detection |
| `agent.prediction.enable` | bool | true | Enable predictive coding |
| `agent.concept_formation.enable` | bool | true | Enable concept formation |
| `agent.attention.enable` | bool | true | Enable attentional selection |
| `agent.planning.enable` | bool | true | Enable action planning |

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present
- Ensure enough memory for large-scale compilation

**Runtime Issues:**

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration parameters are valid
- Check for memory corruption (enable AddressSanitizer for debugging)

**Performance Issues:**
- Increase timestep for faster simulation
- Reduce neuron or synapse count
- Disable non-essential subsystems
- Check for memory leaks with valgrind

### Memory Issues

**Memory allocation failures:**
- Check available system memory
- Reduce configuration parameters
- Enable memory pool optimizations
- Use checkpointing for long simulations

**Memory fragmentation:**
- Use memory pools (enabled by default)
- Configure appropriate pool sizes
- Monitor memory usage during simulation

### Runtime Errors

**Configuration errors:**
- Validate all configuration parameters
- Check for missing required fields
- Ensure parameter values are within valid ranges

**Input/output errors:**
- Check file permissions
- Verify disk space availability
- Use absolute paths for file operations

### Performance Optimization

**For faster simulation:**
- Use Release build (`-DCMAKE_BUILD_TYPE=Release`)
- Enable SIMD vectorization
- Use performance-optimized build flags
- Consider hardware-specific optimizations

**For larger networks:**
- Enable memory pool optimizations
- Use sparse connectivity patterns
- Configure appropriate vectorization settings
- Monitor and tune cache parameters

---

## Further Reading

- [INTEGRATION_DIAGRAMS.md](docs/INTEGRATION_DIAGRAMS.md) - System architecture and integration diagrams
- [ERROR_HANDLING.md](docs/ERROR_HANDLING.md) - Comprehensive error handling documentation
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization and benchmark documentation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [README.md](README.md) - Project overview and build instructions
