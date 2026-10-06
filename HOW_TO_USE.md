# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API (ENHANCED WITH ADVANCED EXAMPLES AND PHASE 6 FEATURES)

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
| `nlm_phase6_demo` | Executable | Phase 6 integration demo |
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

## Part 3: Using the Python Library (ENHANCED WITH ADVANCED EXAMPLES AND PHASE 6 FEATURES)

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

### Advanced Usage (NEW SECTIONS WITH PHASE 6 FEATURES)

#### Advanced Configuration Setup

```python
import pynlm

# Create configuration with advanced settings for Phase 6
config = pynlm.createDefaultConfig()

# Scale up for complex simulations
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.2)
config.set("brain.region_count", 3)
config.set("brain.initial_weight_mean", 0.6)
config.set("brain.initial_weight_std", 0.15)

# Enable all plasticity mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.005)

config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0002f)
config.set("plasticity.structural.pruning_rate", 0.00002f)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.2f)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.threshold", 0.3f)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 0.8f)

# Configure memory systems
config.set("replay_interval", 100)
config.set("consolidation_interval", 500)
config.set("working_memory_capacity", 100)
config.set("episodic_memory.max_episodes", 2000)

print("Advanced configuration created")
print(f"Neuron count: {config.getOr('brain.neuron_count', 0)}")
print(f"STDP enabled: {config.getOr('plasticity.stdp.enable', False)}")
print(f"Curiosity enabled: {config.getOr('neuromod.curiosity.enable', False)}")
```

#### Phase 6 Complete Brain with All Learning Systems

```python
import pynlm
import time

print("=== NLM Phase 6: Complete Integrated Brain ===\n")

# Create Phase 6 configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.25)
config.set("brain.region_count", 4)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.dopamine.scale", 1.5f)
config.set("replay_interval", 50)
config.set("consolidation_interval", 200)

# Create brain with all systems
brain = pynlm.createBrain(config)
brain.initialize()

# Create world for environment interaction
world = pynlm.createSimpleWorld()
world.configure(width=32, height=32, visionWidth=12, visionHeight=12)
world.reset()
world.setAgentStart(16.0, 16.0)

# Create agent interface with all subsystems
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print(f"Phase 6 brain created successfully:")
print(f"  - {brain.getTotalNeuronCount()} neurons with {brain.getTotalSynapseCount()} synapses")
print(f"  - {brain.getRegionCount()} brain regions")
print(f"  - Memory: Working={brain.getWorkingMemory() is not None}, " +
                 f"Episodic={brain.getEpisodicMemory() is not None}")
print(f"  - Neuromodulation: Dopamine={brain.getDopamine() is not None}, " +
                 f"Curiosity={brain.getCuriosity() is not None}, " +
                 f"Novelty={brain.getNoveltyLevel() is not None}")
print(f"  - Development: {brain.getDevelopmentalStage()}")

# Run Phase 6 integrated simulation
print("\n=== Running Phase 6 Integrated Simulation ===")
simulation_steps = 3000
for step in range(simulation_steps):
    # Update environment
    world.update(timestep=0.05)
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process input through agent interface
    agent.processSensoryInput(percept)
    
    # Run neural computation step
    brain.step(step)
    
    # Generate motor action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning from experience
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Periodic development updates
    if step % 200 == 0:
        agent.updateDevelopment(0.1)
        
        # Log progress
        print(f"Step {step}/{simulation_steps}:")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  - Development stage: {brain.getDevelopmentalStage()}")
        
        # Check memory systems
        if brain.getWorkingMemory():
            print(f"  - Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
        if brain.getEpisodicMemory():
            print(f"  - Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")

print(f"\n=== Phase 6 Simulation Complete ===")
print(f"Final statistics:")
print(f"  - Total reward collected: {brain.getTotalSpikeCount()} (simplified metric)")
print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"  - E/I balance: {brain.getExcitationInhibitionRatio():.2f}")
print(f"  - Development stage: {brain.getDevelopmentalStage()}")
```

#### Memory-Enhanced Learning Agent

```python
import pynlm

print("=== Memory-Enhanced Learning Agent ===\n")

# Configuration optimized for memory-based learning
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2500)
config.set("brain.synapse_density", 0.18)
config.set("replay_interval", 75)
config.set("consolidation_interval", 300)
config.set("working_memory_capacity", 150)
config.set("episodic_memory.max_episodes", 1500)

# Create brain with strong memory capabilities
brain = pynlm.createBrain(config)
brain.initialize()

# Create world with interesting environment
world = pynlm.createSimpleWorld()
world.configure(width=24, height=24, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(12.0, 12.0)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable memory-focused learning
agent.enableStructuralPlasticity(True)  # Growth of new connections
agent.enableDevelopment(True)          # Brain maturation
agent.enableCuriosity(True)            # Exploration of novel experiences
agent.enableRewardModulation(True)     # Reward-based learning

print("Memory-enhanced agent created successfully")
print(f"Agent capacity: {config.getOr('working_memory_capacity', 0)} working memory traces")

# Run memory-focused simulation
print("\n--- Running Memory-Enhanced Simulation ---")
for step in range(2000):
    world.update(0.1)
    
    # Get environmental percept
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Take action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reinforcement learning
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Periodic development and memory updates
    if step % 100 == 0:
        agent.updateDevelopment(0.1)
        
        # Monitor memory systems
        wm = brain.getWorkingMemory()
        em = brain.getEpisodicMemory()
        
        print(f"Step {step}: ")
        if wm:
            print(f"  - Working memory: {wm.getActiveTraces()} active traces")
        if em:
            print(f"  - Episodic memory: {em.getEpisodeCount()} episodes stored")
        print(f"  - Development stage: {brain.getDevelopmentalStage()}")
        print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")

print("\n=== Memory-Enhanced Simulation Complete ===")
```

#### Neuromodulation Demo (Curiosity and Reward Learning)

```python
import pynlm

print("=== Neuromodulation Demo: Curiosity and Reward Learning ===\n")

# Create configuration with strong neuromodulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("neuromod.dopamine.scale", 2.0f)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.threshold", 0.4f)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 1.2f)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Neuromodulation systems activated:")
print(f"  - Dopamine system: {'ACTIVE' if brain.getDopamine() else 'INACTIVE'}")
print(f"  - Curiosity system: {'ACTIVE' if agent.getCuriosityLevel() > 0 else 'INACTIVE'}")
print(f"  - Novelty detection: {'ACTIVE' if agent.getNoveltyLevel() > 0 else 'INACTIVE'}")

# Run simulation focusing on neuromodulation
print("\n--- Neuromodulation Dynamics ---")
for step in range(1000):
    world.update(0.1)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Take action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Log neuromodulation levels
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  - Dopamine level: {brain.getDopamine().getLevel():.3f if brain.getDopamine() else 0.0:.3f}")
        print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  - Reward prediction: {agent.getNeuromodulationLevel():.3f}")

print("\n=== Neuromodulation Demo Complete ===")
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
brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()        # Total spikes
brain.getDevelopmentalStage()    # Current developmental stage

# Memory systems
brain.getWorkingMemory()          # Working memory interface
brain.getEpisodicMemory()         # Episodic memory interface
brain.getAssociativeMemory()      # Associative memory interface

# Neuromodulation systems
brain.getDopamine()              # Dopamine system
brain.getCuriosity()             # Curiosity system
brain.getNoveltyLevel()          # Novelty detection
brain.getPredictionErrorSignal() # Prediction error system

# State management
brain.save("checkpoint.bin")       # Save brain state
brain.load("checkpoint.bin")       # Load brain state

# Regions
region_id = brain.addRegion("cortex")  # Add a new region
region = brain.getRegion(region_id)    # Get region by ID
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
config.has("brain.neuron_count")  # Check if key exists
config.getKeys()                  # Get all configuration keys

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)
config.set("plasticity.stdp.enable", True)

# File I/O
config.loadFromFile("config.json")  # Load from JSON file
config.saveToFile("config.json")    # Save to JSON file

# Get values with defaults
neuron_count = config.getOr("brain.neuron_count", 1000)
synapse_density = config.getOr("brain.synapse_density", 0.1)
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
agent.decodeMotorCommand()

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
pynlm.ActionType.Custom

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker

# Motor command enum
pynlm.MotorCommand.MoveForward
pynlm.MotorCommand.MoveBackward
pynlm.MotorCommand.TurnLeft
pynlm.MotorCommand.TurnRight
pynlm.MotorCommand.LookLeft
pynlm.MotorCommand.LookRight
pynlm.MotorCommand.Interact
pynlm.MotorCommand.Wait
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
| `neuromod.curiosity.threshold` | float | 0.3 | Curiosity activation threshold |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.novelty.sensitivity` | float | 0.8 | Novelty sensitivity |

### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `replay_interval` | int | 100 | Memory replay interval |
| `consolidation_interval` | int | 1000 | Memory consolidation interval |
| `working_memory_capacity` | int | 100 | Working memory capacity |
| `episodic_memory.max_episodes` | int | 1000 | Maximum episodic memory episodes |

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
- [LIMITATIONS.md](docs/LIMITATIONS.md) - Scientific limitations and disclaimers
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Complete Phase 6 integration audit

---

## Quick Start Summary

### For Beginners:
1. `pip install pynlm` (install from source)
2. `import pynlm` (in Python)
3. `brain = pynlm.createBrain(pynlm.createDefaultConfig())`
4. `brain.initialize()`
5. `for i in range(100): brain.step(i)`

### For Advanced Users (Phase 6):
1. Configure advanced settings (neuromodulation, memory, plasticity)
2. Create agent with all learning subsystems enabled
3. Run environment interaction with reward learning
4. Monitor development stages and memory consolidation
5. Use checkpointing for persistence

That's it! You're now fully equipped to use NLM, including all Phase 6 advanced features and capabilities.