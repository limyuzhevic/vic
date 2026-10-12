# NLM (熙然) - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project implementing brain-inspired neural computation. The system is designed to create a neural substrate that develops and learns from experience, with the goal of investigating whether cognition can emerge from spiking neural dynamics.

**Key Differentiators:**
- **Spiking Neural Dynamics**: Real Leaky Integrate-and-Fire (LIF) neurons, not artificial neural networks
- **Developmental Learning**: Brain starts primitive and matures over time
- **Event-Driven Computation**: Efficient simulation based on actual neuronal events
- **Multi-Modal Processing**: Sensory input, motor output, and learning loops
- **No Pretrained Knowledge**: All learning emerges from neural interaction

## Quick Start

### Installation

```bash
# From source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# For Python bindings
pip install pybind11 scikit-build-core
pip install .
```

### Basic Usage

```python
import pynlm

# Create brain with default configuration
config = pynlm.Config()
config.set("brain.neuron_count", 1000)

brain = pynlm.Brain(config)
brain.initialize()

# Basic simulation loop
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Advanced Usage

```python
import pynlm

# Create complex brain configuration
config = pynlm.Config()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# Initialize and configure brain
brain = pynlm.Brain(config)
brain.initialize()
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)

# Create agent with world interaction
agent = pynlm.AgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Advanced simulation with learning
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress periodically
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Learning rate: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
```

## Current Phase

**PHASE 2: REAL NEURAL COMPUTATION**

Phase 2 implements the core neural substrate:

### Neural Computation
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Learning demonstrated through measurable synaptic changes

### System Architecture
- Core brain infrastructure with 500+ neurons
- Event-driven simulation engine
- Configurable plasticity parameters
- Developmental timeline support

## Python Bindings

The NLM project includes Python bindings that provide access to the neural computation system through pybind11. The Python bindings enable researchers and developers to:

### Key Features
- **Brain Simulation**: Create and simulate neural brains
- **Agent Control**: Implement agents with sensory perception and motor actions
- **World Interaction**: Simulate environments and agent-environment interactions
- **Configuration Management**: Set and retrieve brain configuration parameters
- **Serialization**: Save and load brain states for reproducibility
- **Analysis**: Analyze neural dynamics and population activity

### Installation

The Python bindings can be built and installed using CMake:

```bash
# Build Python bindings
cmake -DBUILD_PYTHON_PACKAGE=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
make -j4

# Install Python package
make install
```

### Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create and configure a simple world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation loop
for step in range(10):
    # Update world
    world.update(0.1)
    
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
```

## Examples

The Python package includes several example scripts:

- `basic_example.py` - Demonstrates basic brain and world interaction
- `population_dynamics.py` - Shows population activity analysis
- `configuration_serialization.py` - Demonstrates configuration and state serialization

These examples can be run directly after installation:

```bash
python examples/basic_example.py
python examples/population_dynamics.py
python examples/configuration_serialization.py
```

## Build Instructions

### Standard Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### With Python Bindings

```bash
mkdir build
cd build
cmake -DBUILD_PYTHON_PACKAGE=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
make -j4
```

### Installation

```bash
cd build
make install
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Scientific documentation
├── python/                  # Python bindings
│   ├── CMakeLists.txt      # Enhanced CMake configuration
│   ├── __init__.py        # Python package
│   ├── pynlm.py          # Python bindings module
│   ├── pyproject.template.in  # Package metadata template
│   └── examples/          # Example scripts
│       ├── basic_example.py
│       ├── population_dynamics.py
│       └── configuration_serialization.py
├── examples/                # CMake examples directory
│   └── CMakeLists.txt      # Examples build configuration
├── build.py                 # User-friendly build script
├── ENHANCED_CMAKE_CONFIG.md  # Detailed CMake documentation
├── src/                     # Source code
├── tests/                   # C++ unit tests
└── test_pynlm_basic.py       # Python binding tests
```

### Phase 2 Implementation Achievements

#### Neural Computation
- Real LIF neuron dynamics implemented
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Learning demonstrated through measurable synaptic changes

#### System Architecture
- Core brain infrastructure with 500+ neurons
- Event-driven simulation engine
- Configurable plasticity parameters
- Developmental timeline support

## Building

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Bindings Build
```bash
mkdir build
cd build
cmake -DBUILD_PYTHON_PACKAGE=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
make -j4
```

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Package Installation
```bash
cd build
make install
```

The Python bindings will be installed to:
- Libraries: `lib/pynlm/`
- Python package: `share/pynlm/`
- Python scripts: `bin/` (examples)

### Testing
```bash
cd build
ctest --output-on-failure
```

### Running Python Tests
```bash
python test_pynlm_basic.py
```

### Running Examples
```bash
python python/examples/basic_example.py
python python/examples/population_dynamics.py
python python/examples/configuration_serialization.py
```

## Python Bindings

The NLM project includes Python bindings that provide access to the neural computation system through pybind11. The Python bindings enable researchers and developers to:

### Key Features

- **Brain Simulation**: Create and simulate neural brains
- **Agent Control**: Implement agents with sensory perception and motor actions
- **World Interaction**: Simulate environments and agent-environment interactions
- **Configuration Management**: Set and retrieve brain configuration parameters
- **Serialization**: Save and load brain states for reproducibility
- **Analysis**: Analyze neural dynamics and population activity

### Installation

The Python bindings can be built and installed using CMake:

```bash
# Build Python bindings
cmake -DBUILD_PYTHON_PACKAGE=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
make -j4

# Install Python package
make install
```

### Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
config.set("brain.synapse_density", 0.1)

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create and simulate a world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for i in range(10):
    world.update(0.1)
    percept = world.getSensoryPercept()
    if percept:
        agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()
    if action:
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Examples

The Python package includes several example scripts:

- `basic_example.py` - Demonstrates basic brain and world interaction
- `population_dynamics.py` - Shows population activity analysis
- `configuration_serialization.py` - Demonstrates configuration and state serialization

These examples can be run directly after installation:

```bash
python examples/basic_example.py
python examples/population_dynamics.py
python examples/configuration_serialization.py
```

### Testing

Comprehensive tests are included for the Python bindings:

```bash
# Run all tests
ctest --output-on-failure

# Run Python-specific tests
python test_pynlm_basic.py
```

The test suite includes:
- Core functionality tests
- Configuration validation
- Integration tests for complete workflows
- Performance benchmarking
- Serialization/deserialization tests

## Build Instructions

### Standard Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### With Python Bindings

```bash
mkdir build
cd build
cmake -DBUILD_PYTHON_PACKAGE=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
make -j4
```

### Installation

```bash
cd build
make install
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Scientific documentation
├── python/                  # Python bindings
│   ├── CMakeLists.txt      # Enhanced CMake configuration
│   ├── __init__.py        # Python package
│   ├── pynlm.py          # Python bindings module
│   ├── pyproject.template.in  # Package metadata template
│   └── examples/          # Example scripts
│       ├── basic_example.py
│       ├── population_dynamics.py
│       └── configuration_serialization.py
├── examples/                # CMake examples directory
│   └── CMakeLists.txt      # Examples build configuration
├── build.py                 # User-friendly build script
├── ENHANCED_CMAKE_CONFIG.md  # Detailed CMake documentation
├── src/                     # Source code
├── tests/                   # C++ unit tests
└── test_pynlm_basic.py       # Python binding tests
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Implemented - Research Platform)
- Scaling infrastructure for millions of neurons
- Performance optimizations (SIMD, multithreading)
- Memory pools and event queues
- Phase 5 lifetime learning experiments (includes Phase 6: Social Development)

### Phase 6 (Implemented - Integration Infrastructure)
- Phase 6 integration experiment (Phase6Demo.cpp)
- Phase 6 integrated experiment (Phase6IntegratedExperiment.cpp)
- Full system integration framework ready for deployment
- All systems designed to work together as coherent brain loop

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 2 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 2 DOES investigate:
- Whether real neural computation can achieve learning
- Whether STDP and Hebbian plasticity can produce meaningful adaptation
- Whether event-driven spike propagation can support complex processing
- Whether developmental stages can modulate plasticity rates
- Whether replay can reinforce synaptic changes
- Whether the complete neural loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Implemented - Research Platform)
- Scaling infrastructure for millions of neurons
- Performance optimizations (SIMD, multithreading)
- Memory pools and event queues
- Phase 5 lifetime learning experiments (includes Phase 6: Social Development)

### Phase 6 (Implemented - Integration Infrastructure)
- Phase 6 integration experiment (Phase6Demo.cpp)
- Phase 6 integrated experiment (Phase6IntegratedExperiment.cpp)
- Full system integration framework ready for deployment
- All systems designed to work together as coherent brain loop

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 2 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 2 DOES investigate:
- Whether real neural computation can achieve learning
- Whether STDP and Hebbian plasticity can produce meaningful adaptation
- Whether event-driven spike propagation can support complex processing
- Whether developmental stages can modulate plasticity rates
- Whether replay can reinforce synaptic changes
- Whether the complete neural loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
