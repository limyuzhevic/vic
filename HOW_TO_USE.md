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

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

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
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
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

# NLM (Neural Learning Machine) - HOW TO USE Phase 6

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. Phase 6 implements **complete system integration** where all previously separate components now work together as a coherent artificial brain.

This document covers:
1. Building the C++ project with Phase 6 support
2. Installing the Python library for Phase 6
3. Using the enhanced NLM Python API for Phase 6

---

## Part 1: Building the C++ Project with Phase 6

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

# Configure with CMake for Phase 6 (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PHASE6_INTEGRATION=ON

# Build all targets
make -j$(nproc)

# Run Phase 6 tests (if built)
ctest --output-on-failure -R phase6
```

### Build Outputs - Phase 6 Enhanced

After successful compilation with Phase 6 integration, you will have:

| Target | Type | Phase 6 Features |
|--------|------|------------------|
| `nlm` | Executable | Main simulation with full integration |
| `nlm_core` | Static Library | Core brain components + Phase 6 systems |
| `nlm_agent` | Static Library | Agent system + memory/neuromodulation |
| `nlm_world` | Static Library | World simulation + perception |
| `nlm_phase6_demo` | **NEW IN PHASE 6** | Comprehensive integration test |
| `nlm_phase6_test` | **NEW IN PHASE 6** | Unit test suite for Phase 6 |
| `nlm_test_legacy` | Executable | Legacy tests (Phase 2-5) |
| `nlm_test_phase6` | Executable | Phase 6 integration tests |

### Build Options - Phase 6 Enhanced

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_PHASE6_INTEGRATION=ON

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -DENABLE_PHASE6_INTEGRATION=ON

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DENABLE_PHASE6_INTEGRATION=ON
make install

# Phase 6 specific options:
-DENABLE_CHECKPOINTING=ON      # Enable persistent state
-DENABLE_REPLAY=ON             # Enable memory replay
-DENABLE_DEVELOPMENT=ON        # Enable developmental stages
-DENABLE_PREDICTION=ON         # Enable prediction system
-DENABLE_PLANNING=ON           # Enable planning system
-DENABLE_CONCEPT_FORMATION=ON   # Enable concept formation
```

---

## Part 2: Installing the Python Library - Enhanced for Phase 6

### From Source (Recommended for Phase 6 Development)

```bash
# Install build dependencies including Phase 6 support
pip install pybind11 scikit-build-core pytest numpy

# Build and install with Phase 6 integration
pip install .

# Or install in development mode with Phase 6 features
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (Phase 6 enabled by default)
pip install .

# Build only (without installing) with Phase 6
pip install build
python -m build

# Install specific Phase 6 build from wheel
pip install dist/*.whl
```

### Verifying Phase 6 Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a Phase 6 brain with integrated systems
config = pynlm.createDefaultConfig()

# Phase 6: Configure enhanced features
config.set("brain.neuron_count", 500)  # Larger brain
config.set("memory.working_memory_size", 1000)  # Working memory
config.set("memory.episodic_capacity", 100)  # Episodic memory
config.set("development.critical_period_duration", 100.0)  # Development
config.set("neuromod.dopamine_scale", 1.0)  # Neuromodulation
config.set("prediction.horizon", 10)  # Prediction

brain = pynlm.createBrain(config)

print(f"Phase 6 Total neurons: {brain.getTotalNeuronCount()}")
print(f"Phase 6 Ready with: Memory, Neuromodulation, Prediction, Development")
```

---

## Part 3: Using the Enhanced Python Library - Phase 6 API

### Basic Usage - Phase 6 Enhanced

#### Creating a Phase 6 Brain

```python
import pynlm

# Create a default Phase 6 configuration
config = pynlm.createDefaultConfig()

# Phase 6: Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("memory.working_memory_size", 2000)
# config.set("neuromod.curiosity_scale", 0.8)
# config.set("development.critical_period_duration", 200.0)

# Create and initialize the Phase 6 brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Phase 6 Total neurons: {brain.getTotalNeuronCount()}")
print(f"Phase 6 Total synapses: {brain.getTotalSynapseCount()}")
print(f"Phase 6 Memory capacity: {brain.getWorkingMemory() and brain.getWorkingMemory().getCapacity() or 0}")
```

#### Running a Phase 6 Simulation

```python
import pynlm

# Create brain with Phase 6 features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps with Phase 6 integration
for step in range(1000):
    brain.step(step)
    
    # Phase 6: Log comprehensive statistics
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}, Firing: {brain.getFiringNeuronCount()}")
        print(f"  Working Memory: {brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}")
        print(f"  Episodic Memory: {brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount() or 0}")
        print(f"  Development Stage: {brain.getDevelopmentalStage()}")
```

#### Creating a Phase 6 World with Enhanced Features

```python
import pynlm

# Create a Phase 6 world with enhanced capabilities
world = pynlm.createSimpleWorld()

# Phase 6: Configure advanced world features
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8,
    internalSignalsSize=4,      # Enhanced internal signals
    proprioceptionSize=3,       # Enhanced proprioception
    maxEnergy=1000.0,            # Higher energy capacity
    energyDecayRate=0.01         # Energy dynamics
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Phase 6 Agent Example

```python
import pynlm

def run_phase6_agent_simulation(num_steps=1000):
    """Run a complete Phase 6 agent simulation with all integrated systems."""
    
    # 1. Create Phase 6 configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create Phase 6 brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create Phase 6 agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure Phase 6 world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable ALL Phase 6 subsystems
    agent.enableRewardModulation(True)      # Dopamine learning
    agent.enableStructuralPlasticity(True)  # Brain growth
    agent.enableDevelopment(True)           # Developmental stages
    agent.enableCuriosity(True)             # Exploration drive
    
    # 7. Phase 6: Enhanced simulation loop with full integration
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input (Phase 6: processed into working memory)
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain (Phase 6: memory, prediction, attention)
        agent.processSensoryInput(percept)
        
        # Run brain step (Phase 6: complete integration demo)
        brain.step(step)
        
        # Decode motor command from brain (Phase 6: planning, neuromodulation)
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Phase 6: Apply reward modulation with prediction error
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Phase 6: Update development system
        agent.updateDevelopment(0.1)
        
        # Print comprehensive Phase 6 statistics every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Working memory traces: {brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}")
            print(f"  Episodic episodes: {brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount() or 0}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")

# Run the Phase 6 simulation
run_phase6_agent_simulation(1000)
```

### Phase 6 Key Classes and Methods - Enhanced

#### Brain Class - Phase 6 Enhanced

```python
# Creation with Phase 6 features
config = pynlm.createDefaultConfig()
config.set("memory.working_memory_size", 1000)
config.set("memory.episodic_capacity", 100)
config.set("development.critical_period_duration", 100.0)
config.set("neuromod.dopamine_scale", 1.0)
config.set("prediction.horizon", 10)

brain = pynlm.createBrain(config)

# Initialization (Phase 6: all subsystems)
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
brain.injectCurrent(neuron, current)      # Direct neuron stimulation
brain.produceAction()                      # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getWorkingMemory()           # Phase 6: Working memory system
brain.getEpisodicMemory()          # Phase 6: Episodic memory system
brain.getDevelopmentalStage()     # Phase 6: Development stage
brain.getPredictionError()        # Phase 6: Prediction error

# State
brain.save("checkpoint.bin")       # Phase 6: Save with memory
brain.load("checkpoint.bin")       # Phase 6: Load with memory

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class - Phase 6 Enhanced

```python
# Create
config = pynlm.createDefaultConfig()

# Phase 6: Enhanced configuration queries
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Phase 6: Modify with new options
config.set("brain.neuron_count", 1000)
config.set("memory.working_memory_size", 2000)
config.set("memory.episodic_capacity", 500)
config.set("development.critical_period_duration", 200.0)
config.set("development.maturation_rate", 0.02)
config.set("neuromod.dopamine_scale", 1.5)
config.set("neuromod.curiosity_scale", 0.8)
config.set("neuromod.novelty_threshold", 0.05)
config.set("prediction.horizon", 20)
config.set("prediction.accuracy_threshold", 0.8)

# Phase 6: File I/O with enhanced support
config.loadFromFile("phase6_config.json")
config.saveToFile("phase6_config.json")
```

#### AgentBrain Class - Phase 6 Enhanced

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Enhanced Phase 6 sensory processing
agent.processSensoryInput(percept)

# Enhanced Phase 6 motor decoding
motor_cmd = agent.decodeMotorCommand()

# Phase 6: Enhanced neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Phase 6: Advanced development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Phase 6: Enhanced subsystem management
agent.enableRewardModulation(True)      # Dopamine
agent.enableStructuralPlasticity(True)  # Brain growth
agent.enableDevelopment(True)           # Developmental stages
agent.enableCuriosity(True)             # Exploration

# Phase 6: Enhanced statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
agent.getPlannerState()                 # Phase 6: Planning
agent.getConceptState()                 # Phase 6: Concepts
agent.getAttentionState()               # Phase 6: Attention
```

#### SimpleWorld Class - Phase 6 Enhanced

```python
# Creation
world = pynlm.createSimpleWorld()

# Phase 6: Enhanced configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8,
               internalSignalsSize=4, proprioceptionSize=3,
               maxEnergy=1000.0)

# Phase 6: Advanced control
world.reset()
world.update(timestep)
world.setRandomSeed(42)
world.setAgentStart(x, y)

# Phase 6: Enhanced agent interaction
world.applyMotorCommand(motor_cmd, current_time)

# Phase 6: Enhanced state access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Phase 6: Enhanced world objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Phase 6: Enhanced properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(1000.0)
```

### Phase 6 Enumerations - Enhanced

```python
# Phase 6: Additional neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal
pynlm.NeuronType.WorkingMemory      # Phase 6: Dedicated working memory neuron
pynlm.NeuronType.EpisodicMemory     # Phase 6: Episodic memory neuron
pynlm.NeuronType.Prediction         # Phase 6: Prediction neuron
pynlm.NeuronType.Planner            # Phase 6: Planning neuron

# Phase 6: Enhanced developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
pynlm.DevelopmentalStage.Declining      # Phase 6: Added decline stage

# Phase 6: Enhanced action types
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.LookUp
pynlm.ActionType.LookDown
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
pynlm.ActionType.Plan           # Phase 6: Planning action
pynlm.ActionType.Explore         # Phase 6: Exploration action
```

---

## Part 4: Phase 6 Configuration Options

### Brain Configuration - Enhanced for Phase 6

| Parameter | Type | Default | Phase 6 Description |
|-----------|------|---------|---------------------|
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

### Phase 6 Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory_size` | int | 1000 | Working memory capacity |
| `memory.episodic_capacity` | int | 100 | Episodic memory slots |
| `memory.associative_strength` | float | 0.5 | Associative memory strength |
| `memory.replay_interval` | int | 100 | Replay frequency (steps) |
| `memory.consolidation_interval` | int | 1000 | Consolidation frequency (steps) |
| `memory.working_memory_decay` | float | 0.001 | Working memory decay rate |
| `memory.episodic_decay` | float | 0.0001 | Episodic memory decay rate |

### Phase 6 Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine_scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity_scale` | float | 0.5 | Curiosity drive strength |
| `neuromod.novelty_threshold` | float | 0.1 | Novelty detection threshold |
| `neuromod.serotonin_scale` | float | 1.0 | Serotonin modulation scale (Phase 6)
| `neuromod.norepinephrine_scale` | float | 1.0 | Norepinephrine modulation (Phase 6)
| `neuromod.acetylcholine_scale` | float | 1.0 | Acetylcholine modulation (Phase 6)

### Phase 6 Prediction Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.horizon` | int | 10 | Prediction time horizon |
| `prediction.accuracy_threshold` | float | 0.7 | Prediction accuracy requirement |
| `prediction.confidence_decay` | float | 0.01 | Confidence decay rate |
| `prediction.error_learning_rate` | float | 0.001 | Prediction error learning rate |
| `prediction.exploration_bonus` | float | 0.1 | Exploration bonus for prediction |

### Phase 6 Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.critical_period_duration` | float | 100.0 | Critical period length |
| `development.maturation_rate` | float | 0.01 | Maturation progression rate |
| `development.plasticity_decay` | float | 0.001 | Plasticity decay with age |
| `development.synapse_pruning_rate` | float | 0.00001 | Age-based synapse pruning |
| `development.reduction_rate` | float | 0.0001 | Neural reduction rate |

---

## Part 5: Phase 6 Example Scripts

### Phase 6 Minimal Example

```python
import pynlm

# Create and initialize Phase 6 brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("memory.working_memory_size", 500)

brain = pynlm.createBrain(config)
brain.initialize()

# Simulate with Phase 6 integration
for i in range(100):
    brain.step(i)

print("Phase 6 simulation complete!")
print(f"Neurons: {brain.getTotalNeuronCount()}, Memory traces: {brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0}")
```

### Phase 6 Environment Interaction

```python
import pynlm

# Setup Phase 6 integrated system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable Phase 6 features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run Phase 6 episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Phase 6 Troubleshooting

### Build Issues

**CMake cannot find Python (Phase 6):**
```bash
pip install scikit-build-core pybind11
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PHASE6_INTEGRATION=ON -DPython_EXECUTABLE=$(which python)
```

**Compilation errors (Phase 6 features):**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify Phase 6 headers are present
- Ensure all Phase 6 systems are compiled

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Phase 6 integration verification failed:**
- Ensure the module was built with `-DENABLE_PHASE6_INTEGRATION=ON`
- Check that Phase 6 subsystems are properly linked
- Verify configuration options are correctly passed

**Phase 6 simulation running too slowly:**
- Reduce `brain.neuron_count` for faster testing
- Decrease `world.update(timestep)` for coarse time steps
- Disable unnecessary Phase 6 features for performance

**Memory usage too high in Phase 6:**
- Reduce `memory.working_memory_size`
- Reduce `memory.episodic_capacity`
- Monitor Phase 6 memory trace accumulation

---

## Phase 6 Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Phase 6 integration overview
- [PHASE6_ARCHITECTURE.md](docs/PHASE6_ARCHITECTURE.md) - Detailed Phase 6 architecture
- [PHASE6_FINAL_RESULTS.md](docs/PHASE6_FINAL_RESULTS.md) - Phase 6 results and analysis
- [PHASE6_FEATURES.md](docs/PHASE6_FEATURES.md) - Complete Phase 6 feature list
- [PHASE6_TUTORIAL.md](docs/PHASE6_TUTORIAL.md) - Step-by-step Phase 6 tutorial
- [PHASE6_REFERENCE.md](docs/PHASE6_REFERENCE.md) - Phase 6 API and configuration reference
- [PHASE6_BEST_PRACTICES.md](docs/PHASE6_BEST_PRACTICES.md) - Phase 6 development and usage best practices

---

**Phase 6 is now the default mode for NLM - all systems are integrated and working together as a coherent artificial brain!**
