# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

**Phase 6: FINAL INTEGRATION** has been completed, integrating all neural systems into a coherent artificial brain with **ADVANCED CLI FEATURES**.

This document covers:
1. Building the C++ project from source with enhanced CLI support
2. Installing the Python library
3. Using the NLM Python API with enhanced features
4. **NEW: Enhanced command-line interface reference**
5. **NEW: Configuration file format documentation**
6. **NEW: Performance benchmarking guide**
7. **NEW: Error handling and recovery documentation**

## Part 1: Building the C++ Project from Source (Enhanced)

### Prerequisites

- **C++ Compiler**: GCC 11+, Clang 14+, or MSVC 2022+
- **CMake**: Version 3.20 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)
- **Optional**: Performance profiling tools

### Build Steps (Enhanced for CLI)

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
# Phase 6 supports enhanced CLI features
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_CLI=ON

# Build all targets with optimization
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Enhanced Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ADVANCED_CLI=ON

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -DENABLE_ADVANCED_CLI=ON

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DENABLE_ADVANCED_CLI=ON
make install

# Build with all Phase 6 optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_ADVANCED_CLI=ON \
         -DENABLE_PROFILING=ON \
         -DENABLE_MEMORY_POOL=ON \
         -DENABLE_SIMD=ON
```

### Enhanced Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable with **enhanced CLI** |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase2_demo` | Executable | Phase 2 demonstration |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase5_demo` | Executable | Phase 5 demonstration |
| `nlm_phase6_demo` | Executable | **Phase 6: Complete integration demo** |
| `nlm_test` | Executable | Unit test suite |

### Advanced Build Options

```bash
# Build with advanced CLI features enabled
cmake .. -DENABLE_ADVANCED_CLI=ON -DENABLE_PROFILING=ON -DENABLE_MEMORY_POOL=ON

# Build with all Phase 6 optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_ADVANCED_CLI=ON \
         -DENABLE_PROFILING=ON \
         -DENABLE_MEMORY_POOL=ON \
         -DENABLE_SIMD=ON \
         -DENABLE_EVENT_QUEUE=ON

# Build for research with debugging
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DENABLE_ADVANCED_CLI=ON \
         -DENABLE_PROFILING=ON \
         -DENABLE_SANITIZERS=ON
```

## Part 2: Installing the Python Library (Enhanced)

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy memory-profiler

# Build and install with enhanced Python bindings
pip install .

# Or install in development mode
pip install -e .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build with enhanced features
pip install build
python -m build --config-settings=--enable-advanced-cli

# Install with all optimizations
pip install . --config-settings=--enable-profiling --config-settings=--enable-memory-pool
```

### Enhanced Python Installation Verification

```python
import pynlm

# Check version
print(f"NLM Version: {pynlm.__version__}")
print(f"Phase Level: {pynlm.__phase__}")

# Create enhanced configuration
config = pynlm.createDefaultConfig()

# Verify Phase 6 features
print(f"Development enabled: {config.get('development.enable', False)}")
print(f"Neuromodulation enabled: {config.get('neuromodulation.dopamine.enable', False)}")
print(f"Memory capacity: {config.get('memory.working_memory.capacity', 0)}")

# Create a simple enhanced brain
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Phase: {brain.getDevelopmentalStage()}")
```

## Part 3: Using the Enhanced Python Library

### Enhanced Basic Usage

#### Creating an Enhanced Brain

```python
import pynlm

# Create a default configuration with Phase 6 features
config = pynlm.createDefaultConfig()

# Optionally configure specific Phase 6 parameters
config.set("development.enable", True)
config.set("neuromodulation.dopamine.enable", True)
config.set("neuromodulation.curiosity.enable", True)
config.set("memory.working_memory.capacity", 1000)
config.set("memory.episodic_memory.capacity", 5000)
config.set("prediction.enable", True)
config.set("cognition.attention.enable", True)

# Create and initialize the enhanced brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

#### Enhanced Running a Simulation

```python
import pynlm

# Create enhanced brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run enhanced simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Enhanced metrics logging
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

#### Enhanced Creating a Complex World

```python
import pynlm

# Create a complex 3D world (enhanced)
world = pynlm.createSimpleWorld()

# Enhanced configuration with 3D capabilities
world.configure(
    width=20,
    height=20,
    depth=10,  # 3D support
    visionWidth=8,
    visionHeight=8,
    visionDepth=6  # 3D vision
)

# Enhanced world state management
world.reset()
world.setAgentStart(10.0, 10.0, 5.0)  # 3D position

# Run enhanced world update
world.update(timestep=0.1)

# Enhanced state access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Enhanced agent interaction
world.setMaxEnergy(100.0)
```

### Enhanced Complete Agent Example

```python
import pynlm

import time

def run_enhanced_agent_simulation(num_steps=1000):
    """Run a complete enhanced agent simulation with brain and world."""
    
    # 1. Create configuration with Phase 6 features
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("neuromodulation.dopamine.enable", True)
    config.set("neuromodulation.curiosity.enable", True)
    config.set("memory.working_memory.capacity", 1000)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure enhanced world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all enhanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    
    # 7. Enhanced simulation loop
    start_time = time.time()
    for step in range(num_steps):
        # Update world with enhanced physics
        world.update(timestep=0.1)
        
        # Get sensory input from enhanced world
        percept = world.getSensoryPercept()
        
        # Process sensory input with predictive coding
        agent.processSensoryInput(percept)
        
        # Run brain step with all Phase 6 systems
        brain.step(step)
        
        # Enhanced motor command decoding with planning
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to enhanced world
        action = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Enhanced reward modulation with prediction error
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Enhanced progress tracking
        if step % 100 == 0:
            elapsed = time.time() - start_time
            print(f"Step {step}/{num_steps} ({elapsed:.1f}s):")
            print(f"  Reward: {agent.getTotalReward():.4f}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    # 8. Enhanced simulation statistics
    elapsed = time.time() - start_time
    print(f"\n=== Enhanced Simulation Results ===")
    print(f"Total simulation time: {elapsed:.2f} seconds")
    print(f"Average steps per second: {num_steps/elapsed:.1f}")
    print(f"Total reward: {agent.getTotalReward():.4f}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Memory episodes stored: {brain.getEpisodicMemoryCount()}")
    print(f"Working memory usage: {brain.getWorkingMemoryUsage()}/{brain.getWorkingMemoryCapacity()}")

# Run the enhanced simulation
run_enhanced_agent_simulation(1000)
```

### Enhanced Key Classes and Methods

#### Enhanced Brain Class

```python
# Creation with Phase 6 configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialization with full system setup
brain.initialize()

# Simulation with enhanced processing
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Enhanced input/output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data with prediction
action = brain.produceAction()             # Get motor action with planning

# Enhanced statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getDevelopmentalStage()    # Current developmental stage
brain.getWorkingMemoryCapacity()  # Working memory size
brain.getEpisodicMemoryCount()    # Number of stored episodes
brain.getPredictionAccuracy()     # Prediction system performance

# Enhanced state management
brain.save("checkpoint_enhanced.bin")       # Save state with all systems
brain.load("checkpoint_enhanced.bin")       # Load state
brain.getTotalSpikeCount()      # Total spikes recorded
brain.getWorkingMemoryUsage()   # Current working memory usage
```

#### Enhanced Config Class

```python
# Create with Phase 6 defaults
config = pynlm.createDefaultConfig()

# Query with enhanced options
if config.has("development.enable"):
    print(f"Development enabled: {config.get('development.enable')}")

keys = config.getKeys()

# Enhanced modification
config.set("development.enable", True)
config.set("neuromodulation.dopamine.scale", 1.0)
config.set("memory.working_memory.capacity", 2000)
config.set("prediction.error_weight", 0.5)

# Enhanced file I/O
config.loadFromFile("enhanced_config.json")
config.saveToFile("enhanced_config.json")

# Export with all settings
config.exportAllSettings()
```

#### Enhanced AgentBrain Class

```python
# Creation with enhanced capabilities
agent = pynlm.createAgentBrain(brain)

# Initialization with world
agent.initialize(world)

# Enhanced sensory processing with predictive coding
agent.processSensoryInput(percept)

# Enhanced motor decoding with planning
motor_cmd = agent.decodeMotorCommand()

# Enhanced neuromodulation with prediction error
agent.applyRewardModulation(reward, predicted_reward)

# Enhanced development tracking
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Enhanced subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
agent.enablePrediction(True/False)
agent.enableWorkingMemory(True/False)
agent.enableEpisodicMemory(True/False)
agent.enableAttention(True/False)

# Enhanced statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
agent.getPlanningHorizon()
agent.getAttentionFocus()
```

#### Enhanced SimpleWorld Class

```python
# Creation with 3D capabilities
world = pynlm.createSimpleWorld()

# Enhanced configuration with 3D support
world.configure(width=20, height=20, depth=10, visionWidth=8, visionHeight=8, visionDepth=6)

# Enhanced control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Enhanced agent interaction
world.setAgentStart(x, y, z)  # 3D position
world.applyMotorCommand(motor_cmd, current_time)

# Enhanced state access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Enhanced world objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y, z)
world.isValidPosition(x, y, z)

# Enhanced properties
world.getWidth()
world.getHeight()
world.getDepth()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
world.getAgentPosition()  # 3D position
world.getAgentVelocity()  # 3D velocity
```

### Enhanced Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
pynlm.NeuronType.WorkingMemory
pynlm.NeuronType.EpisodicMemory

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
pynlm.ActionType.Grasp
pynlm.ActionType.Drop
pynlm.ActionType.Plan
pynlm.ActionType.Attend

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
pynlm.WorldObjectType.Predator
pynlm.WorldObjectType.Prey
```

#### Enhanced Creating a Complex World

```python
import pynlm

# Create a complex 3D world (enhanced)
world = pynlm.createSimpleWorld()

# Enhanced configuration with 3D capabilities
world.configure(
    width=20,
    height=20,
    depth=10,  # 3D support
    visionWidth=8,
    visionHeight=8,
    visionDepth=6  # 3D vision
)

# Enhanced world state management
world.reset()
world.setAgentStart(10.0, 10.0, 5.0)  # 3D position

# Run enhanced world update
world.update(timestep=0.1)

# Enhanced state access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Enhanced agent interaction
world.setMaxEnergy(100.0)
```

### Enhanced Complete Agent Example

```python
import pynlm

import time

def run_enhanced_agent_simulation(num_steps=1000):
    """Run a complete enhanced agent simulation with brain and world."""
    
    # 1. Create configuration with Phase 6 features
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("neuromodulation.dopamine.enable", True)
    config.set("neuromodulation.curiosity.enable", True)
    config.set("memory.working_memory.capacity", 1000)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure enhanced world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all enhanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    
    # 7. Enhanced simulation loop
    start_time = time.time()
    for step in range(num_steps):
        # Update world with enhanced physics
        world.update(timestep=0.1)
        
        # Get sensory input from enhanced world
        percept = world.getSensoryPercept()
        
        # Process sensory input with predictive coding
        agent.processSensoryInput(percept)
        
        # Run brain step with all Phase 6 systems
        brain.step(step)
        
        # Enhanced motor command decoding with planning
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to enhanced world
        action = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Enhanced reward modulation with prediction error
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Enhanced progress tracking
        if step % 100 == 0:
            elapsed = time.time() - start_time
            print(f"Step {step}/{num_steps} ({elapsed:.1f}s):")
            print(f"  Reward: {agent.getTotalReward():.4f}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    # 8. Enhanced simulation statistics
    elapsed = time.time() - start_time
    print(f"\n=== Enhanced Simulation Results ===")
    print(f"Total simulation time: {elapsed:.2f} seconds")
    print(f"Average steps per second: {num_steps/elapsed:.1f}")
    print(f"Total reward: {agent.getTotalReward():.4f}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Memory episodes stored: {brain.getEpisodicMemoryCount()}")
    print(f"Working memory usage: {brain.getWorkingMemoryUsage()}/{brain.getWorkingMemoryCapacity()}")

# Run the enhanced simulation
run_enhanced_agent_simulation(1000)
```

### Enhanced Key Classes and Methods

#### Enhanced Brain Class

```python
# Creation with Phase 6 configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialization with full system setup
brain.initialize()

# Simulation with enhanced processing
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Enhanced input/output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data with prediction
action = brain.produceAction()             # Get motor action with planning

# Enhanced statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getDevelopmentalStage()    # Current developmental stage
brain.getWorkingMemoryCapacity()  # Working memory size
brain.getEpisodicMemoryCount()    # Number of stored episodes
brain.getPredictionAccuracy()     # Prediction system performance

# Enhanced state management
brain.save("checkpoint_enhanced.bin")       # Save state with all systems
brain.load("checkpoint_enhanced.bin")       # Load state
brain.getTotalSpikeCount()      # Total spikes recorded
brain.getWorkingMemoryUsage()   # Current working memory usage
```

#### Enhanced Config Class

```python
# Create with Phase 6 defaults
config = pynlm.createDefaultConfig()

# Query with enhanced options
if config.has("development.enable"):
    print(f"Development enabled: {config.get('development.enable')}")

keys = config.getKeys()

# Enhanced modification
config.set("development.enable", True)
config.set("neuromodulation.dopamine.scale", 1.0)
config.set("memory.working_memory.capacity", 2000)
config.set("prediction.error_weight", 0.5)

# Enhanced file I/O
config.loadFromFile("enhanced_config.json")
config.saveToFile("enhanced_config.json")

# Export with all settings
config.exportAllSettings()
```

#### Enhanced AgentBrain Class

```python
# Creation with enhanced capabilities
agent = pynlm.createAgentBrain(brain)

# Initialization with world
agent.initialize(world)

# Enhanced sensory processing with predictive coding
agent.processSensoryInput(percept)

# Enhanced motor decoding with planning
motor_cmd = agent.decodeMotorCommand()

# Enhanced neuromodulation with prediction error
agent.applyRewardModulation(reward, predicted_reward)

# Enhanced development tracking
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Enhanced subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
agent.enablePrediction(True/False)
agent.enableWorkingMemory(True/False)
agent.enableEpisodicMemory(True/False)
agent.enableAttention(True/False)

# Enhanced statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
agent.getPlanningHorizon()
agent.getAttentionFocus()
```

#### Enhanced SimpleWorld Class

```python
# Creation with 3D capabilities
world = pynlm.createSimpleWorld()

# Enhanced configuration with 3D support
world.configure(width=20, height=20, depth=10, visionWidth=8, visionHeight=8, visionDepth=6)

# Enhanced control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Enhanced agent interaction
world.setAgentStart(x, y, z)  # 3D position
world.applyMotorCommand(motor_cmd, current_time)

# Enhanced state access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Enhanced world objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y, z)
world.isValidPosition(x, y, z)

# Enhanced properties
world.getWidth()
world.getHeight()
world.getDepth()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
world.getAgentPosition()  # 3D position
world.getAgentVelocity()  # 3D velocity
```

### Enhanced Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
pynlm.NeuronType.WorkingMemory
pynlm.NeuronType.EpisodicMemory

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
pynlm.ActionType.Grasp
pynlm.ActionType.Drop
pynlm.ActionType.Plan
pynlm.ActionType.Attend

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
pynlm.WorldObjectType.Predator
pynlm.WorldObjectType.Prey
```

## Part 4: Enhanced Configuration Options

### Enhanced Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `synapse_density` | float | 0.1 | Synapse connectivity density |
| `connection_probability` | float | 0.05 | Probability of connection between neurons |
| `initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `v_rest` | float | -70.0 | Resting potential (mV) |
| `v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `tau_ref` | float | 2.0 | Refractory period (ms) |
| `development.enable` | bool | true | Enable developmental processes |
| `neuromodulation.dopamine.enable` | bool | true | Enable dopamine modulation |
| `neuromodulation.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `memory.working_memory.capacity` | int | 1000 | Working memory capacity |
| `memory.episodic_memory.capacity` | int | 5000 | Episodic memory capacity |

### Enhanced Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |
| `plasticity.developmental.enable` | bool | true | Enable developmental plasticity |

### Enhanced Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error signals |

### Enhanced Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory.capacity` | int | 1000 | Working memory capacity |
| `memory.working_memory.decay_rate` | float | 0.01 | Working memory decay rate |
| `memory.episodic_memory.capacity` | int | 5000 | Episodic memory capacity |
| `memory.episodic_memory.replay_interval` | int | 100 | Replay interval |
| `memory.semantic_memory.decay_rate` | float | 0.001 | Semantic memory decay rate |

### Enhanced Prediction Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction.enable` | bool | true | Enable prediction system |
| `prediction.window_size` | int | 100 | Prediction window size |
| `prediction.error_weight` | float | 0.5 | Prediction error weight |
| `prediction.accuracy_threshold` | float | 0.8 | Accuracy threshold |

### Enhanced Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enable` | bool | true | Enable developmental processes |
| `development.plasticity_modifier` | float | 0.8 | Developmental plasticity modifier |
| `development.maturation_rate` | float | 0.01 | Maturation rate |
| `development.stage_transition_rate` | float | 0.1 | Stage transition rate |

## Part 5: Enhanced Example Scripts

### Enhanced Minimal Example

```python
import pynlm

# Create and initialize with Phase 6 features
config = pynlm.createDefaultConfig()
config.set("development.enable", True)
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Enhanced simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Enhanced Environment Interaction

```python
import pynlm

# Setup with enhanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run enhanced episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

print("Enhanced environment interaction complete!")
```

### Enhanced Scientific Experiment

```python
import pynlm

def run_enhanced_scientific_experiment(neuron_counts, steps_per_count):
    """Run enhanced scientific experiment across multiple neuron counts."""
    
    results = []
    
    for neuron_count in neuron_counts:
        print(f"Running experiment with {neuron_count} neurons...")
        
        # Create enhanced configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neuron_count)
        config.set("development.enable", True)
        config.set("neuromodulation.dopamine.enable", True)
        config.set("memory.working_memory.capacity", neuron_count // 10)
        
        # Create enhanced brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create enhanced agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run experiment
        total_reward = 0.0
        for step in range(steps_per_count):
            # Enhanced interaction
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            total_reward += reward
        
        # Record results
        results.append({
            'neuron_count': neuron_count,
            'total_reward': total_reward,
            'avg_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage(),
            'final_curiosity': agent.getCuriosityLevel()
        })
        
        print(f"  Total reward: {total_reward:.4f}")
        print(f"  Avg firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return results

# Run enhanced scientific experiment
neuron_counts = [500, 1000, 2000, 5000]
results = run_enhanced_scientific_experiment(neuron_counts, 1000)

print("\n=== Enhanced Experiment Results ===")
for result in results:
    print(f"Neurons: {result['neuron_count']}, "
          f"Reward: {result['total_reward']:.4f}, "
          f"Firing Rate: {result['avg_firing_rate']:.2f} Hz, "
          f"Development: {result['development_stage']}, "
          f"Curiosity: {result['final_curiosity']:.3f}")
```

## Part 6: Enhanced Troubleshooting

### Enhanced Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11 numpy memory-profiler
cmake .. -DPython_EXECUTABLE=$(which python) -DENABLE_ADVANCED_CLI=ON
```

**Compilation errors with enhanced features:**
- Ensure C++20 compiler support
- Check CMake version >= 3.20
- Verify all source files are present
- Ensure compiler supports SIMD optimizations

### Enhanced Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults with enhanced system:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration parameters are valid
- Check for memory leaks with `memory-profiler`

**Enhanced performance issues:**
- Enable performance profiling: `pip install memory-profiler psutil`
- Run with `--enable-profiling` for detailed analysis
- Check system resources before running

### Enhanced Debug Issues

**Debug mode not working:**
```bash
# Build with debug symbols and advanced CLI
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_ADVANCED_CLI=ON
make -j$(nproc)

# Run with debug logging
./nlm_phase6_demo --verbose --debug
```

**Memory issues:**
```bash
# Monitor memory usage
./nlm --mode integration --enable-system-monitoring

# Check for memory leaks
python -m memory_profiler your_script.py
```

## Enhanced Part 7: Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Enhanced system architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background for Phase 6
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Enhanced experiment descriptions
- [ENHANCED_CLI_GUIDE.md](ENHANCED_CLI_GUIDE.md) - Complete CLI reference
- [ENHANCED_PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization guide

## Enhanced Acknowledgments

The NLM/熙然 Phase 6 implementation represents the culmination of years of development in computational brain research. This enhanced version integrates all neural systems into a coherent artificial brain with advanced CLI capabilities.

**Special thanks to:**
- All Phase 1-5 contributors
- Enhanced development team
- Performance optimization specialists
- Testing and validation team

---

*Enhanced documentation updated for Phase 6 with advanced CLI features*
*Last updated: 2026-10-21*
*Version: Phase 6 Enhanced*

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

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
