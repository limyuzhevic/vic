# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project in **Phase 6: FINAL INTEGRATION**. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

**Key Features:**
- ✅ **Phase 6 Integration** - All previous phases completed and integrated
- ✅ **Python Bindings** (`pynlm` package) for easy access
- ✅ **C++ API** for maximum performance
- ✅ **Complete Brain Loop** - Memory, prediction, cognition, neuromodulation, development
- ✅ **Checkpointing & Replay** - Save/load brain states, replay experiences

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API
4. Running the new Phase 6 integration demo

---

## Part 1: Building the C++ Project from Source

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
| `nlm_phase6_demo` | Executable | **Phase 6 integration demo** ✅ (new!) |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `pynlm` | Python Extension | Python bindings module |

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

## Part 3: Phase 6 Integration Demo

The project now includes a complete **Phase 6 Integration Demo** that verifies all brain systems work together:

### What the Demo Tests

| System | Test | Expected Result |
|--------|------|-----------------|
| **Memory Systems** | Working Memory + Episodic Memory | Both systems integrated and functional |
| **Neuromodulation** | Dopamine, Curiosity, Novelty | All neuromodulators working together |
| **Prediction System** | Neural prediction and error computation | Prediction integrated with learning |
| **Development** | Age-dependent plasticity | Development affects learning rates |
| **Checkpointing** | Save/load brain state | Persistence system working |
| **Replay** | Memory consolidation | Replay system functional |

### Running Phase 6 Integration Demo

```bash
# After building with CMake
./nlm_phase6_demo

# Or build and run in one command
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
./nlm_phase6_demo
```

### Sample Output

The demo provides detailed verification:

```
=== NLM Phase 6 Integration Demo ===
Testing the integrated artificial brain...

--- Integration Verification ---
✓ Integration verification passed!

--- Memory Integration Test ---
✓ Working memory has active traces
✓ Episodic memory has episodes

--- Neuromodulation Integration Test ---
✓ Neuromodulation systems are functional

--- Checkpoint Test ---
✓ Checkpoint saved successfully
✓ Checkpoint loaded successfully

--- Replay Test ---
✓ Episodes available for replay
✓ Replay system can retrieve episodes

--- Full Integration Experiment ---
Total reward: 0.123
Avg firing rate: 8.45
Episodes stored: 12
Dopamine level: 0.876

=== INTEGRATION STATUS ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
Checkpointing: WORKING
Replay: WORKING
```

---

## Part 4: Using the NLM Python API

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

## Part 6: Advanced Features and Configuration

### Phase 6 Advanced Configuration

For users who want to explore the complete Phase 6 integration capabilities:

```bash
# Build with full Phase 6 features
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DDISABLE_PHASE6=OFF
make -j$(nproc)

# Run the complete integration demo
./nlm_phase6_demo
```

### Configuration for Advanced Usage

The NLM configuration supports fine-grained control over brain systems:

```python
import pynlm

# Advanced configuration example
config = pynlm.createDefaultConfig()

# Memory systems
config.set("working_memory.enable", True)
config.set("working_memory.capacity", 1000)
config.set("episodic_memory.enable", True)
config.set("episodic_memory.max_episodes", 100)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Prediction system
config.set("prediction.enable", True)
config.set("prediction.horizon", 10)

# Development
config.set("development.enable", True)
config.set("development.plasticity_decay", 0.1)

# Checkpointing
config.set("checkpoint.enable", True)
config.set("checkpoint.path", "./brain_checkpoint.bin")

# Replay
config.set("replay.enable", True)
config.set("replay.consolidation_rate", 0.5)
```

### Python Module API Reference

#### Core Classes

**`pynlm.Config`** - Configuration system for NLM
- `createDefaultConfig()` - Create default configuration
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `set(key, value)` - Set configuration value

**`pynlm.Brain`** - Central neural simulation brain
- `initialize()` - Initialize the brain
- `step(step_number, timestamp)` - Perform simulation step
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getAverageFiringRate()` - Get average firing rate
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state

**`pynlm.AgentBrain`** - Agent interface for world interaction
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted)` - Apply reward-based learning
- `enableRewardModulation(True/False)` - Enable/disable reward modulation

**`pynlm.SimpleWorld`** - Simple 2D simulation world
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `update(timestep)` - Update world state
- `getSensoryPercept()` - Get sensory perception
- `applyMotorCommand(action, timestamp)` - Apply motor action
- `addObject(worldObject)` - Add world object

#### Enumerations

**NeuronType** - Type of neuron
- `Sensory`, `Motor`, `Internal`, `Excitatory`, `Inhibitory`, `Modulatory`

**ActionType** - Type of action
- `MoveForward`, `TurnLeft`, `Interact`, `Wait`, etc.

**DevelopmentalStage** - Brain development stage
- `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging`

**FiringState** - Neuron firing state
- `Resting`, `Active`, `Refractory`, `Inhibited`

### Performance Monitoring

NLM provides comprehensive performance monitoring:

```python
import pynlm
import time

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
start_time = time.time()
step_count = 1000

for step in range(step_count):
    brain.step(step)

end_time = time.time()
wall_time = end_time - start_time

# Performance metrics
print(f"Wall clock time: {wall_time:.2f}s")
print(f"Simulation rate: {step_count/wall_time:.0f} steps/s")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Developmental stage
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

---

## Part 7: Troubleshooting

### Common Issues and Solutions

#### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python3)
```

**Memory allocation issues:**
```bash
# Build with optimized settings
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_OPTIMIZE_MEMORY=ON
```

**Python module not found:**
```bash
# Verify installation
pip install --force-reinstall .
# Check if module is available
python -c "import pynlm; print(pynlm.__version__)"
```

#### Runtime Issues

**Brain not learning:**
- Ensure `enableRewardModulation(True)` is called
- Check that sensory input is properly connected
- Verify developmental stage is appropriate

**Performance slow:**
- Increase simulation timestep
- Reduce neuron count for faster testing
- Check that `enableDevelopment(True)` is set for optimal performance

**Memory leaks:**
- Enable checkpointing to manage memory
- Monitor working memory usage
- Consider reducing episodic memory capacity

### Getting Help

#### Documentation Resources

- **Architecture Guide**: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
- **Science Background**: [docs/SCIENCE.md](docs/SCIENCE.md)
- **Phase 6 Results**: [docs/PHASE6_FINAL_RESULTS.md](docs/PHASE6_FINAL_RESULTS.md)
- **Performance Analysis**: [docs/PERFORMANCE.md](docs/PERFORMANCE.md)

#### Python-Specific Documentation

- **API Reference**: See `python/core/` for detailed documentation
- **Examples**: Look at the examples in this file
- **Tutorials**: Check online documentation at https://nlm.readthedocs.io

#### C++ API Documentation

- **Brain Class**: Refer to `src/brain/Brain.hpp` for full C++ API
- **Source Code**: All source files are documented with comments
- **Phase 6**: See `src/experiments/Phase6IntegratedExperiment.cpp` for complete implementation

### Verification Commands

#### Quick Verification

```bash
# Verify build completed successfully
ls -la build/nlm*
ls -la build/libnlm*.a
ls -la build/python/pynlm*.so

# Test C++ executable
./build/nlm

# Test Python module
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); print('✓ Python module working')"

# Test Phase 6 demo
./build/nlm_phase6_demo
```

#### Performance Tests

```bash
# Run unit tests
./build/nlm_test

# Run Phase 6 integration
./build/nlm_phase6_demo

# Run memory benchmark
./build/nlm_phase5_bench (if available)
```

---

## Further Reading

### Key Documentation Files

1. **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** - System architecture overview
2. **[SCIENCE.md](docs/SCIENCE.md)** - Scientific background and principles
3. **[EXPERIMENTS.md](docs/EXPERIMENTS.md)** - Experiment descriptions and results
4. **[PHASE6_FINAL_RESULTS.md](docs/PHASE6_FINAL_RESULTS.md)** - Complete Phase 6 integration results
5. **[PERFORMANCE.md](docs/PERFORMANCE.md)** - Performance optimizations and benchmarks
6. **[LIMITATIONS.md](docs/LIMITATIONS.md)** - Scientific limitations and assumptions

### Research Papers

- "Artificial Developmental Brains: Neural Systems that Grow and Learn"
- "Event-Driven Neural Computation for Spiking Networks"
- "Integrated Memory and Prediction in Artificial Brains"

### Online Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **API Reference**: https://nlm.readthedocs.io/en/latest/api/
- **Examples**: https://github.com/nlm-project/nlm/tree/main/examples

### Support

#### Community Support

- **GitHub Issues**: Report bugs and feature requests
- **Discussions**: Ask questions and share ideas
- **Contributing**: Fork and contribute to the project

#### Professional Support

- **Commercial Licensing**: Available for enterprise use
- **Custom Development**: Tailored implementations for research
- **Consulting**: Expert guidance on NLM integration

---

## Project Status

**Current Phase**: ✅ **Phase 6: FINAL INTEGRATION** (Complete)

**Integration Status**:
- ✅ Memory Systems (Working + Episodic)
- ✅ Neuromodulation (Dopamine + Curiosity + Novelty)
- ✅ Prediction System
- ✅ Development System
- ✅ Checkpointing & Persistence
- ✅ Replay & Consolidation
- ✅ Python Bindings (pynlm)
- ✅ Phase 6 Integration Demo

**Next Steps**:
1. Continue performance optimizations (Phase 7)
2. Add more advanced user commands and features
3. Enhance Python API with additional utilities
4. Expand documentation and examples

---

*© 2026 NLM Project - All rights reserved. MIT License.*

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
