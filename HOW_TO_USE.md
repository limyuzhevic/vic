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
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

> Phase 5 note: `pip install .` now genuinely builds `pynlm` (verified:
> fresh venv, `pip install .`, then `import pynlm` and the easy_usage
> Example 1 flow run). Previously the root CMakeLists never built the
> module, so `pip install .` succeeded yet `import pynlm` failed. A
> plain C++ `cmake` build still does NOT include pynlm unless you pass
> `-DNLM_BUILD_PYTHON_BINDINGS=ON` (and have pybind11 installed); the
> standalone `cmake -S python -B build` route also still works.

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

### Advanced Configuration Options

NLM's Phase 6 integration experiment now supports a comprehensive set of configuration options for advanced users. Below is an example of using the enhanced Phase 6 configuration:

```python
import pynlm

# Create enhanced Phase 6 configuration
config = pynlm.createPhase6Config()

# Configure advanced parameters
config.maxSteps = 20000
config.neuronCount = 2000
config.regionCount = 2
config.connectionProbability = 0.15f

# Enable all advanced features
config.enableNeuromodulation = True
config.enablePrediction = True
config.enableCognition = True
config.enableCheckpointing = True
config.enableReplay = True
config.enableDevelopment = True

# Set advanced plasticity parameters
config.plasticityStrength = 1.5f
config.learningRate = 0.005f
config.noveltyThreshold = 0.05f
config.curiosityGain = 0.8f
config.dopamineBias = 0.1f

# Configure memory systems
config.workingMemoryCapacity = 1000
config.episodicMemoryMaxEpisodes = 5000
config.memoryDecayRate = 0.005f

# Configure replay and consolidation
config.replayInterval = 200
config.consolidationInterval = 5000
config.consolidationThreshold = 0.5f

# Set development parameters
config.developmentalStageRate = 2.0f
config.plasticityMaturationRate = 0.8f
config.neuronMaturationRate = 0.6f

# Configure logging and monitoring
config.enableDetailedLogging = True
config.logLevel = "INFO"
config.logFile = "nlm_experiment.log"

# Create brain with enhanced configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Create and configure agent with advanced features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure all advanced agent subsystems
agent.enableRewardModulation(config.enableNeuromodulation)
agent.enableStructuralPlasticity(config.enableDevelopment && config.plasticityStrength > 1.0f)
agent.enableDevelopment(config.enableDevelopment && config.developmentalStageRate > 0.0f)
agent.enableCuriosity(config.noveltyThreshold > 0.0f && config.curiosityGain > 0.0f)

# Run enhanced simulation with detailed monitoring
for step in range(config.maxSteps):
    world.update(0.001)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Log advanced metrics
    if step % 500 == 0:
        print(f"Advanced metrics at step {step}:")
        print(f"  Memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
        print(f"  Episodic memory: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
        print(f"  Neuromodulation: curiosity={agent.getCuriosityLevel():.3f}, novelty={agent.getNoveltyLevel():.3f}")
        print(f"  Development: stage={brain.getDevelopmentalStage()}")
        
        # Save checkpoint periodically
        if step % 2000 == 0 and config.enableCheckpointing:
            checkpoint_path = f"checkpoint_{step//1000}.bin"
            if brain.save(checkpoint_path):
                print(f"  Checkpoint saved: {checkpoint_path}")
```

### Example: Complex Experiment with Advanced Features

```python
import pynlm
from datetime import datetime

def run_advanced_experiment(config_path=None):
    """Run a complete advanced experiment with all features."""
    
    # Load configuration or create enhanced config
    if config_path:
        config = pynlm.createPhase6Config()
        # Load from file would require additional implementation
        print(f"Loading config from: {config_path}")
    else:
        config = create_advanced_experiment_config()
    
    # Create experiment runner
    experiment = pynlm.createPhase6Experiment()
    
    # Run the experiment with progress monitoring
    print("Starting Phase 6 Integration Experiment with Advanced Features")
    print("=" * 60)
    
    start_time = datetime.now()
    result = experiment.run(config)
    end_time = datetime.now()
    
    # Generate comprehensive report
    print("\n=== EXPERIMENT RESULTS ===")
    print(f"Duration: {(end_time - start_time).total_seconds():.1f} seconds")
    print(f"Total reward: {result.totalReward:.3f}")
    print(f"Average firing rate: {result.avgFiringRate:.3f}")
    print(f"Memory episodes stored: {result.memoryEpisodesStored:.0f}")
    
    print("\n=== SYSTEM INTEGRATION STATUS ===")
    print(f"Working Memory: {'✓' if result.memoryWorkingMemoryIntegrated else '✗'}")
    print(f"Episodic Memory: {'✓' if result.memoryEpisodicMemoryIntegrated else '✗'}")
    print(f"Neuromodulation: {'✓' if result.neuromodulationIntegrated else '✗'}")
    print(f"Prediction: {'✓' if result.predictionIntegrated else '✗'}")
    print(f"Development: {'✓' if result.developmentIntegrated else '✗'}")
    print(f"Checkpointing: {'✓' if result.checkpointingWorks else '✗'}")
    print(f"Replay: {'✓' if result.replayWorks else '✗'}")
    
    return result

def create_advanced_experiment_config():
    """Create a comprehensive advanced experiment configuration."""
    
    config = pynlm.createPhase6Config()
    
    # High-performance configuration
    config.neuronCount = 5000
    config.maxSteps = 50000
    config.regionCount = 4
    config.connectionProbability = 0.2f
    
    # Enable all advanced subsystems
    config.enableNeuromodulation = True
    config.enablePrediction = True
    config.enableCognition = True
    config.enableDevelopment = True
    config.enableCheckpointing = True
    config.enableReplay = True
    
    # Advanced plasticity settings
    config.plasticityStrength = 2.0f
    config.learningRate = 0.01f
    config.noveltyThreshold = 0.02f
    config.curiosityGain = 1.0f
    config.dopamineBias = 0.3f
    
    # Enhanced memory systems
    config.workingMemoryCapacity = 3000
    config.episodicMemoryMaxEpisodes = 20000
    config.associativeMemorySize = 2000
    config.memoryDecayRate = 0.01f
    
    # Advanced replay and consolidation
    config.replayInterval = 500
    config.consolidationInterval = 10000
    config.consolidationThreshold = 0.7f
    
    # Development and maturation
    config.developmentalStageRate = 3.0f
    config.plasticityMaturationRate = 1.2f
    config.neuronMaturationRate = 0.8f
    
    # Advanced logging and monitoring
    config.enableDetailedLogging = True
    config.logLevel = "DEBUG"
    config.logFile = "advanced_experiment.log"
    
    # Performance optimization
    config.enableMultithreading = True
    config.enableSIMD = True
    config.parallelThreads = 8
    
    return config
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
