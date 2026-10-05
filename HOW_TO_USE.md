# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the **enhanced NLM Python API** with **new Pythonic interface**
4. **Advanced Usage** of all new features
5. **Migration Guide** for upgrading from old API

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

## Part 3: Using the Enhanced NLM Python API

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

### Enhanced Agent Example with New Pythonic Interface

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced Pythonic API."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface (now uses context manager)
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world (using new convenience method)
    agent.initialize(world)
    
    # 6. Enable subsystems (using new convenience methods)
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
        
        # Process sensory input in brain (using new Pythonic names)
        agent.processInput(percept)
        
        # Run brain step (using new Pythonic convenience methods)
        brain.stepWithTime(step, 0.0)  # Pythonic version of brain.step()
        
        # Decode motor command from brain activity (using new Pythonic names)
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation (using new Pythonic names)
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development (using new Pythonic names)
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

### Enhanced API Method Documentation

#### Brain Class - Enhanced Pythonic Methods

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)                      # Run one simulation step
brain.stepWithTime(step_number, time)        # Run step with timestamp
brain.reset()                                 # Reset brain state

# Input/Output (Pythonic names)
brain.processInput(input)                     # Inject sensory input
brain.injectNeuron(neuron_id, current)        # Inject current to specific neuron
brain.injectByType(neuron_type, current)      # Inject to all neurons of specific type
brain.produceAction()                         # Produce motor action

# Statistics (new Pythonic names)
brain.getTotalNeuronCount()                  # Get total neuron count
brain.getTotalSynapseCount()                 # Get total synapse count
brain.getActiveNeuronCount()                 # Get count of active neurons
brain.getFiringNeuronCount()                 # Get count of currently firing neurons
brain.getAverageFiringRate()                 # Get average firing rate
brain.getExcitationInhibitionRatio()        # Get E/I balance
brain.getTotalSpikeCount()                   # Get total spike count

# State
brain.save("checkpoint.bin")                 # Save state
brain.load("checkpoint.bin")                 # Load state

# Regions
brain.addRegion("cortex")                    # Add new neural region
brain.getRegion(region_id)                   # Get a region by ID
brain.getRegionCount()                       # Get number of regions
brain.getRegionIds()                         # Get all region IDs
brain.getRegions()                           # Get all regions

# Development (Pythonic names)
brain.getDevelopmentalStage()               # Get current developmental stage
brain.setDevelopmentalStage(stage)          # Set developmental stage

# Configuration
brain.getConfig()                            # Get the configuration
brain.logStatus()                            # Log brain status

# Batch operations (new features)
brain.injectCurrentBatch(injections)         # Inject to multiple neurons
brain.resetAll()                             # Reset brain and all neurons
brain.getAllNeuronsSummary()                 # Get summary of all neuron statistics

# Context manager support (new feature)
with brain as brain_ctx:
    # Access context wrapper for state management
    state = brain_ctx.getSimulationState()
    brain_ctx.setSimulationState({"stage": pynlm.DevelopmentalStage.CriticalPeriod})
```

#### AgentBrain Class - Enhanced Pythonic Methods

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing (Pythonic names)
agent.processInput(percept)                  # Process sensory input
agent.decodeMotorCommand()                   # Decode motor command

# Neuromodulation (Pythonic names)
agent.applyRewardModulation(reward, predicted_reward)
agent.getNeuromodulationLevel()              # Get neuromodulation level
agent.getCuriosityLevel()                    # Get curiosity level
agent.getNoveltyLevel()                      # Get novelty level
agent.getPredictionError()                   # Get prediction error

# Development (Pythonic names)
agent.updateDevelopment(timestep)             # Update development
agent.getDevelopmentalStage()               # Get developmental stage

# Configuration convenience methods (Pythonic names)
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)
agent.isRewardModulationEnabled()            # Check if enabled
agent.isStructuralPlasticityEnabled()       # Check if enabled
agent.isDevelopmentEnabled()                 # Check if enabled
agent.isCuriosityEnabled()                   # Check if enabled

# Reset
agent.reset()                                # Reset agent for new episode

# State management (new features)
agent.getAgentState()                        # Get complete agent brain state
agent.setAgentState(state)                   # Set agent brain state

# Batch operations (new features)
agent.processInputsBatch(percepts)          # Process multiple sensory percepts
agent.updateDevelopmentsBatch(timesteps)     # Update for multiple timesteps

# Context manager support (new feature)
with agent as agent_ctx:
    # Access context wrapper for state management
    state = agent_ctx.getAgentState()
    agent_ctx.setAgentState({"neuromodulation": 0.5})

# Error handling and validation (new feature)
is_valid, error_msg = agent.validateState()
if not is_valid:
    print(f"State validation failed: {error_msg}")

# Pythonic clone support (new feature)
agent_clone = agent.clone()                   # Create a copy/clone
agent_copySettings(source_agent)              # Copy configuration settings
```

#### Config Class - Enhanced Methods

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

#### SimpleWorld Class - Enhanced Methods

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
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
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

## Part 4: Enhanced Configuration Options

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

### Enhanced Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Enhanced Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Enhanced Example Scripts

### Enhanced Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.stepWithTime(i, 0.0)  # Use enhanced step method

print("Simulation complete!")
```

### Enhanced Environment Interaction

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
    agent.processInput(world.getSensoryPercept())
    brain.stepWithTime(0, 0.0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Part 6: Advanced Usage

This section covers **advanced features** using the new Pythonic interface.

### Context Manager Usage

The NLM Python API now supports context managers for automatic resource management and state handling:

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use brain as a context manager
with brain as brain_ctx:
    # Access the brain through the context wrapper
    state = brain_ctx.getSimulationState()
    print(f"Current state: {state}")
    brain_ctx.step(10)
    brain_ctx.setSimulationState({"stage": pynlm.DevelopmentalStage.CriticalPeriod})

# Context manager automatically handles cleanup

# Agent brain context manager usage
agent = pynlm.createAgentBrain(brain)
with agent as agent_ctx:
    # Access agent through context wrapper
    state = agent_ctx.getAgentState()
    agent_ctx.setAgentState({"neuromodulation": 0.5})
```

### Batch Operations

Process multiple neurons and sensory inputs efficiently:

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Batch neuron current injection
injections = [
    (pynlm.createNeuronId(0), 5.0),
    (pynlm.createNeuronId(1), 3.0),
    (pynlm.createNeuronId(2), 7.0),
    # ... more injections
]
brain.injectCurrentBatch(injections)

# Batch sensory input processing
percepts = []
for i in range(50):
    percept = pynlm.SensoryPercept()
    vision_data = [0.1 * j for j in range(256)]
    percept.setVision(vision_data)
    percepts.append(percept)

agent.processInputsBatch(percepts)

# Batch development updates
timesteps = [0.1] * 50
agent.updateDevelopmentsBatch(timesteps)

# Batch action production (when needed)
actions = brain.produceActionsBatch(50)
```

### State Management

Save, load, and manage simulation states easily:

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Save brain state to file
brain.save("my_checkpoint.bin")

# Load brain state from file
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load("my_checkpoint.bin")

# Get simulation state as dictionary
simulation_state = brain.getSimulationState()
print(f"Current state: {simulation_state}")

# Get agent state as dictionary
agent_state = agent.getAgentState()
print(f"Agent state: {agent_state}")

# Set simulation state from dictionary
brain.setSimulationState({
    "stage": pynlm.DevelopmentalStage.CriticalPeriod,
    "reset_spikes": 100
})

# Set agent state from dictionary
agent.setAgentState({
    "neuromodulation": 0.5,
    "curiosity_enabled": True,
    "development_stage": pynlm.DevelopmentalStage.Adult
})

# Export/import state for serialization
state_dict = agent_state.copy()
with open("agent_state.json", "w") as f:
    # Convert to JSON-serializable format
    json.dump(state_dict, f)
```

### Error Handling and Validation

Validate system state and handle errors gracefully:

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Validate agent brain state
is_valid, error_msg = agent.validateState()
if not is_valid:
    print(f"WARNING: State validation failed: {error_msg}")
    # Handle specific error cases
    if "Reward modulation is disabled" in error_msg:
        agent.enableRewardModulation(True)
    elif "Curiosity is disabled" in error_msg:
        agent.enableCuriosity(True)
else:
    print("State is valid and healthy")

# Enhanced error handling in simulation loop
def safe_simulation_step(world, agent, brain, step_num):
    try:
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processInput(percept)
        
        # Run brain step
        brain.stepWithTime(step_num, 0.0)
        
        # Decode command
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        return True, "Step completed successfully"
        
    except Exception as e:
        # Handle specific errors
        if "Brain not initialized" in str(e):
            brain.initialize()
        elif "World not configured" in str(e):
            world.configure(20, 20, 8, 8)
            world.reset()
        else:
            print(f"Unexpected error: {e}")
            return False, str(e)

# Run multiple steps with error handling
for step in range(100):
    success, message = safe_simulation_step(world, agent, brain, step)
    if not success:
        print(f"Step {step} failed: {message}")
```

### Clone and Copy Operations

Create copies of brains and agent interfaces:

```python
import pynlm

# Create original brain and agent
original_brain = pynlm.createBrain(pynlm.createDefaultConfig())
original_brain.initialize()

original_agent = pynlm.createAgentBrain(original_brain)

# Clone the agent (creates new brain with same configuration)
cloned_agent = original_agent.clone()
cloned_agent.initialize(world)  # Must re-initialize in a world

# Create multiple identical agents for comparison
agents = []
for i in range(5):
    new_agent = original_agent.clone()
    new_agent.initialize(world)
    agents.append(new_agent)

# Copy settings from original to multiple agents
for agent in agents:
    agent.copySettings(original_agent)

# Copy developmental state between brains
target_brain = pynlm.createBrain(pynlm.createDefaultConfig())
target_brain.initialize()
original_brain.copyState(target_brain)

# Create a clone with modified configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
modified_brain = pynlm.createBrain(config)
modified_brain.initialize()
```

---

## Part 7: Migration Guide

This guide helps you **migrate from the old NLM Python API** to the enhanced Pythonic interface.

### Summary of Changes

| Old API Method | New Pythonic API Method | Description |
|----------------|-------------------------|-------------|
| `agent.processSensoryInput(percept)` | `agent.processInput(percept)` | More Pythonic name |
| `agent.decodeMotorCommand()` | `agent.decodeMotorCommand()` | Same name (already Pythonic) |
| `agent.applyRewardModulation(reward, predicted)` | `agent.applyRewardModulation(reward, predicted)` | Same name (already Pythonic) |
| `brain.step(step)` | `brain.stepWithTime(step, 0.0)` | Enhanced with timestamp support |
| `brain.getFiringNeuronCount()` | `brain.getFiringNeuronCount()` | Same name (already Pythonic) |
| `brain.getTotalNeuronCount()` | `brain.getTotalNeuronCount()` | Same name (already Pythonic) |
| `agent.enableRewardModulation(True)` | `agent.enableRewardModulation(True)` | Same name (already Pythonic) |
| `agent.getNeuromodulationLevel()` | `agent.getNeuromodulationLevel()` | Same name (already Pythonic) |
| `agent.getCuriosityLevel()` | `agent.getCuriosityLevel()` | Same name (already Pythonic) |

**Note:** The NLM API has always been relatively Pythonic. The main enhancements are:
- Context manager support
- Batch operations
- State management
- Validation
- Clone and copy operations
- Enhanced statistics access

### Migration Examples

#### Basic Agent Usage

**Old API:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(0.0, 0.0)
```

**New Pythonic API:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processInput(percept)
    brain.stepWithTime(step, 0.0)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(0.0, 0.0)
```

#### Enhanced Agent with New Features

**Old API:**
```python
import pynlm

def run_simulation():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.0, 0.0)
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print("Simulation complete!")

run_simulation()
```

**New Pythonic API:**
```python
import pynlm

def run_enhanced_simulation():
    # Create everything
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run enhanced simulation loop
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processInput(percept)
        brain.stepWithTime(step, 0.0)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.0, 0.0)
        
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Enhanced simulation complete!")

run_enhanced_simulation()
```

### Migration Checklist

#### ✅ Step 1: Basic Usage (Already Compatible)
- [ ] Old method names still work with new API
- [ ] Basic agent interface unchanged
- [ ] Existing code will continue to work

#### ✅ Step 2: Enhance with New Features (Optional)
- [ ] Add context managers: `with brain as brain_ctx:`
- [ ] Use batch operations: `brain.injectCurrentBatch(injections)`
- [ ] Add state management: `agent.getAgentState()`
- [ ] Add validation: `agent.validateState()`
- [ ] Use cloning: `agent.clone()`
- [ ] Use enhanced statistics: `brain.getAllNeuronsSummary()`

#### ✅ Step 3: Error Handling (Recommended)
- [ ] Add try-catch blocks around simulation steps
- [ ] Use state validation for error recovery
- [ ] Implement graceful error handling

---

## Part 8: Enhanced Code Examples

### Complete Advanced Agent with All New Features

```python
import pynlm

def run_complete_advanced_simulation():
    """Complete agent simulation using all new Pythonic features."""
    
    print("=== NLM Advanced Pythonic Agent Simulation ===")
    
    # 1. Create and initialize everything
    print("1. Creating brain and agent...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 2. Enable all learning features
    print("2. Enabling learning features...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 3. Run simulation loop with advanced features
    print("3. Running simulation...")
    for step in range(500):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input (using Pythonic name)
        agent.processInput(percept)
        
        # Run brain step (with timestamp)
        brain.stepWithTime(step, 0.0)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
    
    # 4. Demonstrate state management
    print("4. Demonstrating state management...")
    agent_state = agent.getAgentState()
    print(f"   Agent state: {agent_state}")
    
    # Validate state
    is_valid, error_msg = agent.validateState()
    print(f"   State valid: {is_valid}")
    if not is_valid:
        print(f"   Error: {error_msg}")
    
    # 5. Demonstrate batch operations
    print("5. Demonstrating batch operations...")
    # Create multiple percepts for batch processing
    percepts = []
    for i in range(10):
        percept = pynlm.SensoryPercept()
        vision_data = [0.1 * j for j in range(256)]
        percept.setVision(vision_data)
        percepts.append(percept)
    
    agent.processInputsBatch(percepts)
    print("   Batch processing completed")
    
    # 6. Demonstrate clone operations
    print("6. Demonstrating clone operations...")
    agent_clone = agent.clone()
    agent_clone.initialize(world)  # Must re-initialize
    print("   Agent cloned successfully")
    
    # 7. Demonstrate context manager usage
    print("7. Demonstrating context manager usage...")
    with brain as brain_ctx:
        state = brain_ctx.getSimulationState()
        brain_ctx.setSimulationState({
            "stage": pynlm.DevelopmentalStage.CriticalPeriod
        })
    print("   Context manager operations completed")
    
    print("\n=== Simulation Complete ===")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")

# Run the complete simulation
run_complete_advanced_simulation()
```

### Performance Comparison: Old vs New API

```python
import time
import pynlm

def benchmark_api_versions():
    """Compare performance between old and new API versions."""
    
    print("=== Performance Benchmark: Old API vs New Pythonic API ===\n")
    
    # Old API version
    print("1. Old API Version:")
    start_time = time.time()
    
    config = pynlm.createDefaultConfig()
    brain_old = pynlm.createBrain(config)
    brain_old.initialize()
    
    world_old = pynlm.createSimpleWorld()
    agent_old = pynlm.createAgentBrain(brain_old)
    agent_old.initialize(world_old)
    
    for step in range(200):
        world_old.update(0.1)
        percept = world_old.getSensoryPercept()
        agent_old.processSensoryInput(percept)
        brain_old.step(step)
        action = agent_old.decodeMotorCommand()
        world_old.applyMotorCommand(action, world_old.getSimulationTime())
    
    old_time = time.time() - start_time
    print(f"   Time: {old_time:.3f} seconds")
    print(f"   Steps per second: {200/old_time:.1f}")
    
    # New Pythonic API version
    print("\n2. New Pythonic API Version:")
    start_time = time.time()
    
    config = pynlm.createDefaultConfig()
    brain_new = pynlm.createBrain(config)
    brain_new.initialize()
    
    world_new = pynlm.createSimpleWorld()
    agent_new = pynlm.createAgentBrain(brain_new)
    agent_new.initialize(world_new)
    
    for step in range(200):
        world_new.update(0.1)
        percept = world_new.getSensoryPercept()
        agent_new.processInput(percept)
        brain_new.stepWithTime(step, 0.0)
        action = agent_new.decodeMotorCommand()
        world_new.applyMotorCommand(action, world_new.getSimulationTime())
        
        # Use new convenience methods for statistics
        if step % 50 == 0:
            _ = brain_new.getFiringNeuronCount()
            _ = agent_new.getCuriosityLevel()
    
    new_time = time.time() - start_time
    print(f"   Time: {new_time:.3f} seconds")
    print(f"   Steps per second: {200/new_time:.1f}")
    
    # Performance improvement
    improvement = ((old_time - new_time) / old_time) * 100
    print(f"\n3. Performance Improvement: {improvement:.1f}%")
    
    # Feature comparison
    print("\n4. Feature Comparison:")
    print("   Old API Features:")
    print("     - Basic agent interface")
    print("     - Simple sensory processing")
    print("     - Basic motor decoding")
    print("     - Basic reward modulation")
    print("   ")
    print("   New Pythonic API Features:")
    print("     - Enhanced agent interface with convenience methods")
    print("     - Batch processing capabilities")
    print("     - State management and validation")
    print("     - Context manager support")
    print("     - Clone and copy operations")
    print("     - Error handling and recovery")
    print("     - Development stage tracking")
    print("     - Enhanced statistics access")

# Run benchmark
benchmark_api_versions()
```

---

## Part 9: Troubleshooting with Enhanced API

### Common Issues and Solutions

**Issue: Method not found**
**Problem:** Using old method names with new API
**Solution:** Update method names to Pythonic versions
```python
# Old (doesn't work with new API)
agent.processSensoryInput(percept)

# New (works)
agent.processInput(percept)
```

**Issue: State validation fails**
**Problem:** Agent state is invalid after operations
**Solution:** Use validation and handle errors
```python
# Validate before operations
is_valid, error_msg = agent.validateState()
if not is_valid:
    print(f"State validation failed: {error_msg}")
    # Fix issues based on error message
    if "Reward modulation is disabled" in error_msg:
        agent.enableRewardModulation(True)
    elif "Curiosity is disabled" in error_msg:
        agent.enableCuriosity(True)
```

**Issue: Context manager not working**
**Problem:** Using context manager incorrectly
**Solution:** Use correct context manager syntax
```python
# Old way (doesn't work with context manager)
brain.step(10)

# New way (with context manager)
with brain as brain_ctx:
    brain_ctx.step(10)
```

**Issue: Batch operations failing**
**Problem:** Batch operations with incorrect data types
**Solution:** Use correct data types for batch operations
```python
# Old way (single operation)
brain.injectCurrent(neuron_id, current)

# New way (batch operation)
injections = [(neuron_id1, current1), (neuron_id2, current2)]
brain.injectCurrentBatch(injections)
```

### Enhanced Error Messages

The enhanced Pythonic API provides more descriptive error messages:

```python
# Error message examples
try:
    brain.stepWithTime(step, 0.0)
except RuntimeError as e:
    if "Brain not initialized" in str(e):
        brain.initialize()
    elif "World not configured" in str(e):
        world.configure(20, 20, 8, 8)
        world.reset()
    else:
        print(f"Unexpected error: {e}")
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit

---

## Getting Started with Enhanced API

1. **Quick Start:** Read `easy_usage.md` for beginner-friendly examples
2. **Migration Guide:** Read this section for upgrading from old API
3. **Advanced Usage:** Read this section for advanced features
4. **Experiments:** Try the enhanced example scripts
5. **Troubleshooting:** Use the enhanced error handling features

The enhanced NLM Python API provides a **more Pythonic, efficient, and maintainable** interface for working with neural simulations. Take advantage of the new features to build more sophisticated and robust NLM applications!