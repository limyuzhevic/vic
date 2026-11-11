# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

Key improvements in this version:
- **Neuromodulation**: Full dopamine system with reward prediction error
- **Development**: Age-dependent plasticity and maturation
- **Memory**: Working memory with proper active trace management
- **Configuration**: Improved JSON/YAML support for better type parsing
- **Integration**: All systems properly connected and working together

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API with all new features

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
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON

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
| `nlm_phase6_demo` | Executable | Phase 6 integration demo |
| `nlm_benchmark` | Executable | Performance benchmarks |
| `nlm_test` | Executable | Unit test suite |
| `pynlm` | Python Module | Python bindings with all features |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_BUILD_PYTHON_BINDINGS=ON

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -DNLM_BUILD_PYTHON_BINDINGS=ON

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DNLM_BUILD_PYTHON_BINDINGS=ON
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install pybind11 scikit-build-core pytest numpy

# Build and install with Python bindings enabled
cmake -S . -B build -DNLM_BUILD_PYTHON_BINDINGS=ON
cd build
make
pip install .  # Install from build directory

# Or install in development mode
pip install -e .  # This will build with Python bindings
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
# pip install . will automatically enable Python bindings

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

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")

# Check Python features
print("Python features available:", pynlm.has_python_bindings())
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

#### Running a Simulation with Learning

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with learning capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize()
agent.enableRewardModulation(True)  # Dopamine learning
agent.enableCuriosity(True)         # Exploration
agent.enableDevelopment(True)      # Age-dependent plasticity

# Run simulation for 1000 steps with learning
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
```

#### Creating a Simple World with Advanced Features

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

### Complete Agent Example with All New Features

```python
import pynlm

def run_agent_simulation_with_all_features(num_steps=1000):
    """Run a complete agent simulation with all new features."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface with learning
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable ALL advanced learning features
    agent.enableRewardModulation(True)      # Dopamine reward prediction error
    agent.enableStructuralPlasticity(True) # Synaptogenesis (growing new connections)
    agent.enableDevelopment(True)          # Age-dependent plasticity and maturation
    agent.enableCuriosity(True)            # Novelty-driven exploration
    agent.enablePrediction(True)           # Forward modeling
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step with neuromodulation
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation (dopamine learning)
        reward = percept.getEnergy() if percept else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation_with_all_features(1000)
```

### Key Classes and Methods (WITH NEW FEATURES)

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step with neuromodulation
brain.step(step_number, time)    # Run step with timestamp and dopamine
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data with neuromodulation
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getDevelopmentalStage()     # Current developmental stage

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class (IMPROVED)

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O (IMPROVED: JSON/YAML support)
config.loadFromFile("config.json")  # Now supports JSON and YAML
config.saveToFile("config.json")  # Saves in JSON format
```

#### AgentBrain Class (ENHANCED WITH NEUROMODULATION)

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation (NEW!)
agent.applyRewardModulation(reward, predicted_reward)  # Dopamine learning
agent.applyCuriosityModulation(novelty, threshold)    # Exploration
agent.applyDevelopmentModulation(delta, timestep)   # Age-dependent changes

# Development (ENHANCED)
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()
agent.getDevelopmentalProgress()

# Subsystem enable/disable (ENHANCED)
agent.enableRewardModulation(True/False)      # Dopamine reward learning
agent.enableCuriosity(True/False)           # Novelty-driven exploration
agent.enableDevelopment(True/False)         # Age-dependent plasticity
agent.enablePrediction(True/False)          # Forward modeling
agent.enableStructuralPlasticity(True/False)  # Growing new connections

# Statistics (ENHANCED)
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
agent.getDevelopmentalStage()
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
body = world.getAgentWorldObject()

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

### Enumerations (UPDATED)

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages (ENHANCED)
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
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

### Example Configuration File (JSON Format)

```json
{
  "brain.neuron_count": 1000,
  "brain.synapse_density": 0.1,
  "world.width": 20,
  "brain.development.enabled": true,
  "neuromodulation.dopamine.baseline": 0.2,
  "neuromodulation.curiosity.threshold": 0.3
}
```

---

## Advanced Features

NLM now includes these advanced features for sophisticated brain simulation:

1. **Advanced Configuration System**:
   - **JSON/YAML Support**: Full JSON and YAML parsing for flexible configuration
   - **Type-Aware**: Automatic type detection and parsing (bool, int, float, strings, arrays)
   - **Better Error Handling**: Improved validation and error messages

2. **Neuromodulation Features**:
   - **Dopamine**: Reward prediction error with burst/dip dynamics, baseline, eligibility traces
   - **Curiosity**: Novelty-driven exploration with configurable thresholds
   - **Novelty**: Surprise detection and prediction error computation

3. **Memory Features**:
   - **Working Memory**: Persistent activity with proper active trace management
   - **Episodic Memory**: Experience storage and replay capabilities
   - **Associative Memory**: Pattern completion and Hebbian learning

4. **Prediction and Cognition**:
   - **Prediction System**: Forward modeling with prediction errors
   - **Concept Formation**: Pattern abstraction from experiences
   - **Neural planning**: Action sequence evaluation with costs

5. **Development Features**:
   - **Developmental Stages**: Initial, CriticalPeriod, Maturation, Adult, Aging
   - **Synaptogenesis**: Activity-dependent formation of new connections
   - **Maturation**: Gradual improvement of neural properties

6. **Integration Features**:
   - **All Systems Connected**: Memory, neuromodulation, prediction, and cognition all integrated
   - **Proper Development**: Age-dependent effects on plasticity
   - **Checkpoint System**: Save/load with JSON/YAML support

## Commands for Advanced Usage

NLM now includes these advanced commands for debugging and experimentation:

### Configuration Management
```bash
# Load JSON config
./nlm --config=config.json

# Load YAML config
./nml --config=config.yaml

# Save config to JSON
./nlm --save_config=checkpoint.json
```

### Debugging and Analysis
```bash
# Enable detailed logging
./nlm --log_level=Debug

# Set dopamine baseline
./nlm --dopamine_baseline=0.2

# Configure world dimensions
./nlm --world_width=20 --world_height=20
```

### Advanced Features
```bash
# Enable prediction debugging
./nml --prediction_debug

# Set development stage
./nml --development_stage=Maturation

# Configure novelty detection threshold
./nml --novelty_threshold=0.5
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions