# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

**Phase 6 Status**: Most cognitive systems are implemented but **disconnected** from the main brain loop. Phase 6 focuses on integration rather than adding new features.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API for Phase 6 integration


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
    
    # 6. Enable subsystems (Phase 6 integration)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop (Phase 6 focus on integration)
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain (enable working memory if available)
        agent.processSensoryInput(percept)
        
        # Run brain step (this now integrates working memory, episodic memory,
        # prediction systems if they are available via getWorkingMemory(),
        # getEpisodicMemory(), etc.)
        brain.step(step)
        
        # Get action and apply to world
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress (Phase 6 metrics)
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            
            # Phase 6 integration metrics (if available)
            if brain.getWorkingMemory():
                print(f"  Working memory: Connected")
            if brain.getEpisodicMemory():
                print(f"  Episodic memory: Active")
            if brain.getPredictionSystem():
                print(f"  Prediction system: Active")

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

### Memory Integration Configuration (Phase 6)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `working_memory.enable` | bool | true | Enable working memory integration |
| `episodic_memory.enable` | bool | true | Enable episodic memory integration |
| `semantic_memory.enable` | bool | false | Enable semantic memory (EXPERIMENTAL) |
| `procedural_memory.enable` | bool | false | Enable procedural memory (EXPERIMENTAL) |
| `associative_memory.enable` | bool | false | Enable associative memory (EXPERIMENTAL) |

### Prediction System Configuration (Phase 6)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `prediction_system.enable` | bool | true | Enable prediction system |
| `prediction_system.horizon` | int | 5 | Prediction horizon (steps) |
| `prediction_system.confidence_threshold` | float | 0.5 | Confidence threshold for predictions |

### Cognitive Integration Configuration (Phase 6)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neural_planner.enable` | bool | true | Enable neural planner |
| `concept_formation.enable` | bool | true | Enable concept formation |
| `self_model.enable` | bool | false | Enable self-model (EXPERIMENTAL) |
| `social_learning.enable` | bool | false | Enable social learning (EXPERIMENTAL) |

### Development Configuration (Phase 6)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.initial_plasticity_rate` | float | 1.0 | Initial plasticity rate |
| `development.critical_period_end` | int | 100 | Steps until critical period ends |
| `development.maturation_rate` | float | 0.5 | Rate of maturation |
| `development.plasticity_decay` | float | 0.95 | Plasticity decay per timestep |

### Neuromodulation Configuration (Phase 6)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.serotonin.enable` | bool | false | Enable serotonin modulation (EXPERIMENTAL) |
| `neuromod.norepinephrine.enable` | bool | false | Enable norepinephrine modulation (EXPERIMENTAL) |
| `neuromod.acetylcholine.enable` | bool | false | Enable acetylcholine modulation (EXPERIMENTAL) |

---

---

## Part 5: Example Scripts

### Minimal Example (Phase 6 Ready)

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()

# Enable Phase 6 integration features
config.set("working_memory.enable", True)
config.set("episodic_memory.enable", True)
config.set("prediction_system.enable", True)
config.set("neural_planner.enable", True)
config.set("concept_formation.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Simulate (Phase 6 integration happens automatically if systems are available)
for i in range(100):
    brain.step(i)
    
    # Phase 6 integration metrics (if available)
    if brain.getWorkingMemory():
        print(f"Step {i}: Working memory active")
    if brain.getEpisodicMemory():
        print(f"Step {i}: Episodic memory active")
    if brain.getPredictionSystem():
        print(f"Step {i}: Prediction system active")

print("Simulation complete with Phase 6 integration!")
```

### Environment Interaction (Phase 6 Integration)

```python
import pynlm

# Setup with Phase 6 integration
config = pynlm.createDefaultConfig()

# Enable Phase 6 integration features
config.set("working_memory.enable", True)
config.set("episodic_memory.enable", True)
config.set("prediction_system.enable", True)
config.set("neural_planner.enable", True)
config.set("concept_formation.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Phase 6 integration monitoring
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Working memory: {'Active' if brain.getWorkingMemory() else 'Not Available'}")
        print(f"  Episodic memory: {'Active' if brain.getEpisodicMemory() else 'Not Available'}")
        print(f"  Prediction system: {'Active' if brain.getPredictionSystem() else 'Not Available'}")
        
        # Check if memory systems are working (Phase 6 requirement)
        if not brain.getWorkingMemory():
            print(f"  ⚠️ WARNING: Working memory not integrated!")
        if not brain.getEpisodicMemory():
            print(f"  ⚠️ WARNING: Episodic memory not integrated!")
        if not brain.getPredictionSystem():
            print(f"  ⚠️ WARNING: Prediction system not integrated!")
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

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

**Phase 6 Integration Issues:**
- Many memory and cognitive systems exist but are disconnected
- If `getWorkingMemory()`, `getEpisodicMemory()`, or `getPredictionSystem()` return nullptr, it's a Phase 6 integration issue
- Review `docs/PHASE6_FINAL_AUDIT.md` for current integration status
- Focus on connecting existing systems rather than adding new features

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Current integration status (Phase 6 priority: CONNECT EXISTING SYSTEMS)

**Current Status:** NLM has sophisticated **architecture** but functions as a **basic neural simulator** rather than an integrated artificial brain. Phase 6 goal is to make existing components work together as a coherent whole.
