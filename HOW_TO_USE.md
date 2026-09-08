# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

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

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters with nested sections
# config.set("system.neuron_count", 2000)
# config.set("memory.capacity", 5000)
# config.set("neuromodulation.dopamine_base", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Working with Memory Systems

```python
import pynlm

# Create a configuration optimized for memory systems
config = pynlm.createDefaultConfig()

# Enable and configure memory systems
config.set("memory.capacity", 5000)                    # Working memory capacity
config.set("memory.episodic.max_episodes", 1000)       # Episodic memory storage
config.set("memory.associative.strength", 0.8)          # Association strength
config.set("memory.plasticity", 0.01)                  # Memory decay rate

# Create brain with memory support
brain = pynlm.createBrain(config)
brain.initialize()

# Store some memory
brain.storeInWorkingMemory(1, "pattern_1")
brain.storeInEpisodicMemory("episode_1", 1000.0)

# Retrieve memory
memory_content = brain.retrieveFromWorkingMemory(1)
print(f"Working memory: {memory_content}")

# Clear working memory when no longer needed
brain.clearWorkingMemory()
```

#### Working with Neuromodulation

```python
import pynlm

# Create brain with neuromodulation
config = pynlm.createDefaultConfig()

# Configure neuromodulation systems
config.set("neuromodulation.dopamine.base", 0.1)
config.set("neuromodulation.dopamine.phasic_strength", 1.0)
config.set("neuromodulation.acetylcholine.level", 0.05)
config.set("neuromodulation.norepinephrine.arousal", 1.0)
config.set("neuromodulation.serotonin.mood", 0.5)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable neuromodulation
brain.enableNeuromodulation(True)

# Simulate some neuromodulation effects
brain.updateDopamineLevel(0.3)
brain.updateAcetylcholineLevel(0.2)
brain.updateNorepinephrineLevel(0.5)
brain.updateSerotoninLevel(0.4)

print(f"Dopamine level: {brain.getDopamineLevel():.2f}")
print(f"Acetylcholine level: {brain.getAcetylcholineLevel():.2f}")
```

#### Working with Cognitive Systems

```python
import pynlm

# Create brain with cognitive systems
config = pynlm.createDefaultConfig()

# Configure cognitive systems
config.set("cognition.planner.depth", 3)
config.set("cognition.planner.confidence", 0.5)
config.set("cognition.attention.focus_width", 0.5)
config.set("cognition.concept.formation_threshold", 0.3)
config.set("cognition.self_model.capability", 0.5)

brain = pynlm.createBrain(config)
brain.initialize()

# Initialize cognitive systems
brain.initializeNeuralPlanner()
brain.initializeConceptFormation()
brain.initializeSelfModel()

# Use cognitive systems
planner = brain.getNeuralPlanner()
concept = brain.getConceptFormation()
self_model = brain.getSelfModel()

# Plan an action
best_action = planner.planAction([0.1, 0.5, 0.3], 1.0)
print(f"Planned action: {best_action}")

# Update concept formation
concept.updateConcept([0.1, 0.5, 0.3])

# Update self-model
current_state = brain.getCurrentNeuralState()
self_model.recordSelfAction(best_action, current_state, [0.2, 0.6, 0.4])

# Get predicted consequences
predicted = self_model.predictActionConsequence(best_action, current_state)
print(f"Predicted outcome: {predicted}")
```

#### Working with Prediction System

```python
import pynlm

# Create brain with prediction
config = pynlm.createDefaultConfig()
config.set("prediction.neural.enabled", True)
config.set("prediction.action.enabled", True)
config.set("prediction.error_threshold", 0.1)

brain = pynlm.createBrain(config)
brain.initialize()

# Initialize prediction system
brain.initializePredictionSystem()

# Get prediction system
prediction_system = brain.getPredictionSystem()

# Simulate a prediction scenario
current_state = [0.1, 0.2, 0.3, 0.4, 0.5]
predicted_next = prediction_system.predictNextState(current_state)
print(f"Predicted next state: {predicted_next}")

# Update with actual outcome
actual_next = [0.15, 0.25, 0.35, 0.45, 0.55]
prediction_system.updatePredictions(predicted_next, actual_next)

print(f"Prediction error: {prediction_system.getPredictionError():.4f}")
print(f"Prediction confidence: {prediction_system.getConfidence():.4f}")
```

#### Advanced Agent with All Systems

```python
import pynlm

def run_advanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with all advanced brain systems."""
    
    # 1. Create configuration with all advanced features
    config = pynlm.createDefaultConfig()
    
    # Configure memory systems
    config.set("memory.capacity", 2000)
    config.set("memory.episodic.max_episodes", 500)
    config.set("memory.associative.strength", 0.7)
    
    # Configure neuromodulation
    config.set("neuromodulation.dopamine.base", 0.1)
    config.set("neuromodulation.dopamine.phasic_strength", 1.2)
    config.set("neuromodulation.novelty.threshold", 0.1)
    config.set("neuromodulation.curiosity.weight", 0.5)
    
    # Configure cognitive systems
    config.set("cognition.planner.depth", 4)
    config.set("cognition.attention.focus_width", 0.6)
    config.set("cognition.self_model.learning_rate", 0.01)
    
    # Configure prediction
    config.set("prediction.neural.enabled", True)
    config.set("prediction.action.enabled", True)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    
    # 7. Initialize cognitive systems
    brain.initializeNeuralPlanner()
    brain.initializeConceptFormation()
    brain.initializeSelfModel()
    brain.initializePredictionSystem()
    
    # 8. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Process in cognitive systems
        if step % 10 == 0:  # Plan every 10 steps
            current_state = percept.getVision()
            if current_state:
                # Use neural planner for action planning
                planner = brain.getNeuralPlanner()
                if planner:
                    # Plan action with current state
                    target_reward = 1.0 if world.getAgentAtPosition(world.getAgentPosition()) == pynlm.WorldObjectType.Resource else 0.0
                    planned_action = planner.planAction(current_state, target_reward)
                    
                    # Get motor command (combining planner with agent's output)
                    motor_cmd = agent.decodeMotorCommand()
                    
                    # Apply motor command to world
                    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Run brain step (processes all integrated systems)
        brain.step(step)
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Working memory size: {brain.getWorkingMemorySize()}")
            print(f"  Episodic episodes: {brain.getEpisodicEpisodeCount()}")
    
    # 9. Save final brain state
    brain.save(f"advanced_agent_simulation_step_{num_steps}.bin")
    
    print(f"\nSimulation complete! Brain saved to checkpoint.")

# Run the advanced simulation
run_advanced_agent_simulation(1000)
```

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
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
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
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

### Key Classes and Methods

#### Brain Class

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
brain.getActiveNeuronCount()      # Currently active neurons
brain.getFiringNeuronCount()     # Currently firing neurons
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()        # Total spike count
brain.getDevelopmentalStage()     # Get current developmental stage

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)

# ========== MEMORY SYSTEMS ==========

# Note: Direct Python API access may not be exposed, use Brain::step() for automatic updates

# ========== PREDICTION SYSTEM ==========

# Note: Direct Python API access may not be exposed, use Brain::step() for automatic updates

# ========== DEVELOPMENT SYSTEM ==========

brain.getDevelopmentalStage()        # Get current developmental stage
brain.setDevelopmentalStage(stage)  # Set developmental stage
brain.updateDevelopment(timestep)     # Update development (note: use Brain::step() for automatic updates)
brain.getMaturationProgress()        # Get maturation progress
brain.getCriticalPeriodEnd()         # Get critical period end time

# ========== NEUROMODULATION SYSTEMS ==========

# Note: Direct Python API access may not be exposed, use Brain::step() for automatic updates

# ========== ADVANCED STATISTICS ==========

# Note: Direct Python API access may not be exposed

# ========== CHECKPOINT MANAGEMENT ==========

# Note: Direct Python API access may not be exposed

# Configuration
brain.getConfig()                    # Get the configuration
brain.logStatus()                    # Log brain status
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

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

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

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
```

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
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
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
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 4: Advanced Cognitive Features

### Neuromodulation Systems

NLM now includes a complete set of four neuromodulators that work together to guide learning and behavior:

#### Dopamine (DA)
Dopamine signals reward prediction errors and modulates behavioral vigor. It influences STDP rates based on prediction errors, helping the brain learn from success and failure.

#### Acetylcholine (ACh)
Acetylcholine controls attention and working memory encoding. It enhances signal-to-noise ratio in neural populations, allowing the brain to focus on relevant information.

#### Norepinephrine (NE)
Norepinephrine regulates arousal and vigilance. It enhances plasticity during novel events and helps the brain respond to changing environments.

#### Serotonin (5-HT)
Serotonin stabilizes mood and promotes patience. It inhibits impulsive responses and helps with delayed gratification, supporting long-term planning.

#### Neuromodulation Integration Example

```python
import pynlm

# Create brain with full neuromodulation
config = pynlm.createDefaultConfig()

# Configure all four neuromodulators
config.set("neuromod.dopamine.base", 0.1)
config.set("neuromod.dopamine.phasic_strength", 1.2)
config.set("neuromod.acetylcholine.level", 0.05)
config.set("neuromod.norepinephrine.arousal", 1.0)
config.set("neuromod.serotonin.mood", 0.5)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable full neuromodulation
brain.enableNeuromodulation(True)

# Simulate reward learning
simulation_steps = 1000
for step in range(simulation_steps):
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Get reward from world
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    
    # Apply reward modulation (dopamine signal)
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development stage
    agent.updateDevelopment(0.1)
    
    # Check for curiosity-driven exploration
    if agent.getCuriosityLevel() > 0.3:
        # Explore randomly to encourage discovery
        pass
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Dopamine: {brain.getDopamineLevel():.3f}")
        print(f"  Acetylcholine: {brain.getAcetylcholineLevel():.3f}")
        print(f"  Norepinephrine: {brain.getNorepinephrineLevel():.3f}")
        print(f"  Serotonin: {brain.getSerotoninLevel():.3f}")
```

### Cognitive Architecture

NLM's cognitive architecture includes four integrated systems that work together:

#### Neural Planner
- Multi-step action sequence evaluation
- Planning depth and confidence tracking
- Reward anticipation and value computation

#### Concept Formation
- Pattern clustering and generalization
- Prototype formation from experiences
- Instance-based and rule-based learning

#### Self-Model
- Body awareness and proprioception
- Action consequence prediction
- Causal reasoning about self

#### Social Learning
- Observation and imitation
- Theory of mind development
- Cultural transmission

#### Cognitive Integration Example

```python
import pynlm

# Create brain with full cognitive support
config = pynlm.createDefaultConfig()

# Configure cognitive systems
config.set("cognition.planner.depth", 4)
config.set("cognition.planner.confidence", 0.6)
config.set("cognition.attention.focus_width", 0.5)
config.set("cognition.concept.formation_threshold", 0.3)
config.set("cognition.self_model.capability", 0.7)

brain = pynlm.createBrain(config)
brain.initialize()

# Initialize all cognitive systems
brain.initializeNeuralPlanner()
brain.initializeConceptFormation()
brain.initializeSelfModel()
brain.initializeSocialLearning()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
agent.initialize(world)

# Run cognitive development simulation
for step in range(5000):
    # Update world
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Update cognitive systems
    # 1. Concept formation from sensory patterns
    concept = brain.getConceptFormation()
    concept.updateConcept(extract_features(percept))
    
    # 2. Self-model updates from action outcomes
    self_model = brain.getSelfModel()
    action = agent.decodeMotorCommand()
    self_model.recordSelfAction(action, percept.getVision(), get_world_state())
    
    # 3. Social learning from interactions
    social = brain.getSocialLearning()
    # Observe others' actions
    social.observeAgentAction(action, percept.getVision(), get_consequence(action))
    
    # 4. Neural planning for complex tasks
    if step % 100 == 0:
        planner = brain.getNeuralPlanner()
        current_state = percept.getVision()
        action = planner.planAction(current_state, expected_reward)
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Run brain step
    brain.step(step)
    
    # Apply neuromodulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Print cognitive stats
    if step % 500 == 0:
        print(f"Step {step}:")
        print(f"  Concepts formed: {len(concept.getConcepts())}")
        print(f"  Self-model confidence: {self_model.getBodyAwareness():.3f}")
        print(f"  Planning depth: {planner.getPlanningDepth()}")
        print(f"  Planning confidence: {planner.getPlanningConfidence():.3f}")
```

### Memory Systems Integration

NLM's memory systems work together to support learning and behavior:

#### Working Memory
- Stores current sensory patterns and actions
- Capacity: 2000+ items
- Active maintenance with decay

#### Episodic Memory
- Stores complete experiences with context
- Automatic capture every 10 steps
- Replay every 100 steps for consolidation

#### Associative Memory
- Forms pattern associations
- Spreading activation for recall
- Strength-based retrieval

#### Memory Integration Example

```python
import pynlm

# Create brain with enhanced memory
config = pynlm.createDefaultConfig()

# Configure memory systems
config.set("memory.capacity", 5000)
config.set("memory.episodic.max_episodes", 2000)
config.set("memory.associative.strength", 0.8)
config.set("memory.plasticity", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and simulate memory usage
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Store memorable experiences
simulation_steps = 3000
for step in range(simulation_steps):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Store in working memory
    # Brain automatically stores firing neurons in working memory each step
    
    # Capture episodic memory every 10 steps
    if step % 10 == 0:
        episode = create_episodic_memory(percept, step, agent.getMotorCommand())
        brain.storeInEpisodicMemory(episode)
    
    # Replay important memories every 100 steps
    if step % 100 == 0 and step > 0:
        brain.replayMemories()
    
    # Form associations between patterns
    if step % 50 == 0:
        form_associations(percept, previous_percept)
    
    # Run brain step (updates all memory systems)
    brain.step(step)
    
    # Apply neuromodulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Print memory stats
    if step % 500 == 0:
        print(f"Step {step}:")
        print(f"  Working memory size: {brain.getWorkingMemorySize()}")
        print(f"  Episodic episodes: {brain.getEpisodicEpisodeCount()}")
        print(f"  Association strength: {brain.getAssociativeStrength():.3f}")
```

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### Social Learning
- Observation and imitation of others
- Theory of mind development
- Cultural transmission of behaviors

#### Advanced Integration Example

```python
import pynlm

# Create socially-aware brain with full integration
config = pynlm.createDefaultConfig()

# Configure all cognitive systems
config.set("cognition.planner.depth", 5)
config.set("cognition.social.learning_rate", 0.3)
config.set("cognition.social.imitation_threshold", 0.7)

brain = pynlm.createBrain(config)
brain.initialize()

# Initialize all systems
brain.initializeNeuralPlanner()
brain.initializeConceptFormation()
brain.initializeSelfModel()
brain.initializeSocialLearning()

# Create multi-agent environment
agents = []
for i in range(3):
    agent = pynlm.createAgentBrain(brain)
    agents.append(agent)

# Run social learning simulation
for step in range(10000):
    # Each agent interacts with others
    for agent in agents:
        # Sense other agents' actions
        percept = agent.getSocialPercept()
        
        # Learn from observation
        social = brain.getSocialLearning()
        social.observeAgentAction(percept.action, percept.state, percept.outcome)
        
        # Update cognitive models
        brain.updateCognitiveModels()
        
        # Plan based on social knowledge
        planner = brain.getNeuralPlanner()
        plan = planner.planAction(percept.state, predict_social_outcome(percept.action))
        
        # Apply motor command
        agent.applyMotorCommand(plan, step)
    
    # Print social learning stats
    if step % 1000 == 0:
        social = brain.getSocialLearning()
        print(f"Step {step}:")
        print(f"  Social knowledge: {social.getKnowledgeStrength():.3f}")
        print(f"  Imitation rate: {social.getImitationRate():.3f}")
        print(f"  Cultural transmission: {social.getTransmissionStrength():.3f}")
```

---

## Phase 6: Advanced Usage

Phase 6 represents the full integration of all previously separate systems into a coherent artificial brain. This advanced usage guide covers:

### System Integration

The NLM brain integrates all major systems into a single, coherent architecture:

```cpp
// Brain::step() now includes:
// 1. Process spikes
// 2. Update neurons (LIF dynamics)
// 3. Detect spikes
// 4. Update working memory
// 5. Apply neuromodulation (DA, ACh, NE, 5-HT)
// 6. Apply plasticity (STDP, Hebbian)
// 7. Store episodic memory
// 8. Update prediction system
// 9. Update attention
// 10. Update concept formation
// 11. Structural plasticity
// 12. Replay
// 13. Development update
// 14. Consolidation
// 15. Checkpoint management
```

### Advanced Configuration

#### Nested Configuration Sections
The enhanced configuration system uses nested sections for organization:

```cpp
// Good - Related settings in one section
config.addSection("memory")
    .set<size_t>("capacity", 5000)
    .set<float>("decay_rate", 0.01)
    .set<size_t>("working_memory_size", 2000);

// Good - Neuromodulation system
config.addSection("neuromodulation")
    .set<float>("dopamine_base", 0.1)
    .set<bool>("reward_modulation_enabled", true)
    .set<float>("acetylcholine_level", 0.05)
    .set<float>("norepinephrine_arousal", 1.0);
```

#### Command Line Configuration

```bash
# Load from JSON file
./nlm --config=advanced_config.json

# Override specific values
./nlm --neuron-count=2000 \
      --memory-capacity=1000 \
      --simulation-timestep=0.0005 \
      --enable-reward-modulation \
      --enable-structural-plasticity

# Configure specific sections
./nlm --section.memory.capacity=2000 \
      --section.neuromodulation.dopamine_base=0.2 \
      --section.cognition.planner.depth=4 \
      --section.prediction.error_threshold=0.01
```

### Advanced Usage Patterns

#### 1. Developmental Learning

Developmental stages affect neural development over time:

```python
import pynlm

# Create developing brain
config = pynlm.createDefaultConfig()

brain = pynlm.createBrain(config)
brain.initialize()

# Simulate development over time
simulation_steps = 10000
for step in range(simulation_steps):
    brain.step(step)
    
    # Check developmental stage
    stage = brain.getDevelopmentalStage()
    if stage == pynlm.DevelopmentalStage.CriticalPeriod:
        # Enhanced plasticity during critical period
        brain.boostPlasticity()
    elif stage == pynlm.DevelopmentalStage.Maturation:
        # Maturation and refinement
        brain.refineConnections()
    elif stage == pynlm.DevelopmentalStage.Adult:
        # Adult stability and maintenance
        brain.maintainConnections()
    
    # Print developmental progress
    if step % 1000 == 0:
        print(f"Step {step}: {stage}")
```

#### 2. Checkpointing and Recovery

Robust checkpointing for long simulations:

```python
import pynlm
import os

# Create brain with checkpointing
config = pynlm.createDefaultConfig()
config.set("checkpoint.interval", 1000)
config.set("checkpoint.directory", "./checkpoints")
config.set("checkpoint.max_checkpoints", 10)
config.set("checkpoint.compressed", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run long simulation
simulation_steps = 5000
checkpoint_interval = 1000

for step in range(simulation_steps):
    brain.step(step)
    
    # Automatic checkpointing
    if step % checkpoint_interval == 0:
        checkpoint_file = f"checkpoints/simulation_step_{step}.bin"
        brain.save(checkpoint_file)
        print(f"Checkpoint saved: {checkpoint_file}")
    
    # Example of loading from checkpoint
    if step == 2000:  # Load at step 2000
        checkpoint_file = f"checkpoints/simulation_step_{step}.bin"
        if os.path.exists(checkpoint_file):
            print(f"Loading from checkpoint: {checkpoint_file}")
            brain.load(checkpoint_file)

print("Simulation complete!")
```

#### 3. Neuromodulation Dynamics

Complex neuromodulation patterns:

```python
import pynlm

# Create brain with advanced neuromodulation
config = pynlm.createDefaultConfig()

# Configure neuromodulators with interactions
config.set("neuromod.dopamine.base", 0.1)
config.set("neuromod.dopamine.phasic_strength", 1.5)
config.set("neuromod.acetylcholine.level", 0.08)
config.set("neuromod.norepinephrine.arousal", 1.2)
config.set("neuromod.serotonin.mood", 0.6)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable all neuromodulators
brain.enableNeuromodulation(True)

# Simulate neuromodulation dynamics over time
for step in range(2000):
    # Update all neuromodulators
    brain.updateAllNeuromodulators(step)
    
    # Apply reward prediction error
    reward = calculate_reward(step)
    prediction_error = expected_reward - reward
    brain.applyNeuromodulation(prediction_error)
    
    # Check for novel events
    if detect_novelty(step):
        # Boost norepinephrine for exploration
        brain.boostNorepinephrine()
    
    # Print neuromodulation state
    if step % 500 == 0:
        print(f"Step {step}:")
        print(f"  DA: {brain.getDopamineLevel():.3f}")
        print(f"  ACh: {brain.getAcetylcholineLevel():.3f}")
        print(f"  NE: {brain.getNorepinephrineLevel():.3f}")
        print(f"  5-HT: {brain.getSerotoninLevel():.3f}")
```

#### 4. Memory Integration

Complex memory systems working together:

```python
import pynlm

# Create brain with full memory support
config = pynlm.createDefaultConfig()

# Configure memory systems
config.set("memory.capacity", 10000)
config.set("memory.episodic.max_episodes", 5000)
config.set("memory.associative.strength", 0.9)

brain = pynlm.createBrain(config)
brain.initialize()

# Run memory-intensive simulation
for step in range(8000):
    # Update world and get input
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    # Process sensory input
    # Brain automatically stores in working memory
    
    # Capture episodic memory every 10 steps
    if step % 10 == 0:
        episode = create_memory_episode(step, percept, brain)
        brain.storeInEpisodicMemory(episode)
    
    # Replay important memories every 100 steps
    if step % 100 == 0 and step > 0:
        brain.replayMemories()
    
    # Form associations between patterns
    if step % 50 == 0:
        brain.associatePatterns(percept, previous_percept)
    
    # Update development
    brain.updateDevelopment(0.1)
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0]
    brain.applyRewardModulation(reward)
    
    # Print memory stats
    if step % 1000 == 0:
        print(f"Step {step}:")
        print(f"  Working memory: {brain.getWorkingMemorySize()}")
        print(f"  Episodic: {brain.getEpisodicEpisodeCount()}")
        print(f"  Associative strength: {brain.getAssociativeStrength():.3f}")
```

### Advanced Features

#### Prediction System
Advanced prediction for temporal processing and action planning:

```python
# Initialize prediction system
brain.initializePredictionSystem()

# Get prediction system
prediction = brain.getPredictionSystem()

# Predict next state
current_state = get_current_state()
predicted = prediction.predictNextState(current_state)

# Update with actual outcome
actual = get_actual_state()
prediction.updatePredictions(predicted, actual)

# Print prediction accuracy
error = prediction.getPredictionError()
confidence = prediction.getConfidence()
print(f"Prediction error: {error:.4f}, confidence: {confidence:.4f}")
```

#### Cognitive Planning
Goal-directed planning with neural networks:

```python
# Initialize neural planner
brain.initializeNeuralPlanner()
planner = brain.getNeuralPlanner()

# Plan actions with depth and confidence
for step in range(1000):
    current_state = get_current_state()
    target_reward = calculate_target_reward(step)
    
    # Plan action
    action = planner.planAction(current_state, target_reward)
    
    # Print planning stats
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Planned action: {action}")
        print(f"  Planning depth: {planner.getPlanningDepth()}")
        print(f"  Planning confidence: {planner.getPlanningConfidence():.3f}")
```

### Performance Optimization

#### Memory Management
- Configure memory usage limits
- Use checkpointing for long simulations
- Monitor and optimize memory usage

#### Computational Efficiency
- Use nested configuration for better organization
- Enable/disable features as needed
- Monitor simulation performance

#### Scalability
- Configure for different problem sizes
- Use appropriate checkpoint intervals
- Balance accuracy with performance

---

## Troubleshooting

### Common Issues and Solutions

#### 1. Memory Usage Too High
```bash
# Reduce memory usage
./nlm --memory-capacity=1000 --max-memory-usage=500000000

# Use compressed checkpoints
./nlm --checkpoint-compressed=true
```

#### 2. Simulation Speed
```bash
# Increase simulation speed
./nlm --simulation-speed=2.0

# Decrease timestep
./nlm --simulation-timestep=0.0001
```

#### 3. Configuration Errors
```bash
# Show current configuration
./nlm --show-config

# Validate configuration
./nlm --validate-config
```

### Performance Monitoring

Track simulation performance with:

```python
# Monitor brain state
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Memory usage: {brain.getMemoryUtilization():.3f}")
print(f"Prediction accuracy: {brain.getPredictionAccuracy():.3f}")
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [ENHANCED_CONFIG.md](docs/ENHANCED_CONFIG.md) - Enhanced configuration system
- [PHASE6_FINAL_RESULTS.md](docs/PHASE6_FINAL_RESULTS.md) - Phase 6 integration results
- [PHASE6_ARCHITECTURE.md](docs/PHASE6_ARCHITECTURE.md) - Phase 6 architecture

## Conclusion

Phase 6 represents the culmination of NLM development, integrating all previously separate systems into a coherent artificial brain. The enhanced configuration system, advanced cognitive features, and Phase 6 advanced usage patterns provide comprehensive tools for building sophisticated neural simulations.

The NLM brain now supports:

- **Real memory system integration** with working, episodic, and associative memory
- **Complete neuromodulation** with dopamine, acetylcholine, norepinephrine, and serotonin
- **Full cognitive architecture** including planning, concept formation, self-model, and social learning
- **Advanced prediction systems** for temporal processing and action planning
- **Enhanced configuration** with nested sections and validation
- **Checkpointing and recovery** for long simulations
- **Developmental stages** affecting learning and plasticity
- **Phase 6 advanced usage** for complex, integrated simulations

This represents a complete, brain-inspired artificial intelligence system capable of learning, memory, cognition, and adaptive behavior.
