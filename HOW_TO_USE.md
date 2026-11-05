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

---

## Part 8: Advanced Features and Power User Options (Phase 6)

This section covers the advanced configuration, debugging, and performance tuning options available in NLM Phase 6.

### Advanced Configuration

NLM supports two configuration file formats with extensive options for power users:

#### JSON Configuration (Phase 6 Advanced)
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "enable_learning": true,
  "enable_plasticity": true,
  "enable_development": true,
  "enable_neuromodulation": true,
  "debug_mode": false,
  "working_memory_capacity_fraction": 0.1,
  "episodic_memory_max_episodes": 1000,
  "memory_replay_interval": 100,
  "exploration_balance": 0.3,
  "curiosity_threshold": 0.7,
  "reward_pe_decay_rate": 0.99,
  "dopamine_learning_rate": 0.05,
  "curiosity_bonus_weight": 0.2,
  "novelty_sensitivity": 0.3,
  "prediction_error_sensitivity": 0.4,
  "critical_period_strength": 1.0,
  "maturation_acceleration": 1.2,
  "synaptic_delivery_threads": 4,
  "enable_optimized_spike_delivery": true,
  "spike_delivery_batch_size": 100,
  "cache_connectivity": true
}
```

#### Legacy Key=Value Format (Phase 5/6 Compatible)
```
neuron_count = 1000
region_count = 2
connection_probability = 0.1
enable_learning = true
enable_plasticity = true
enable_development = true
enable_neuromodulation = true
debug_mode = false
working_memory_capacity_fraction = 0.1
episodic_memory_max_episodes = 1000
memory_replay_interval = 100
exploration_balance = 0.3
curiosity_threshold = 0.7
reward_pe_decay_rate = 0.99
dopamine_learning_rate = 0.05
curiosity_bonus_weight = 0.2
novelty_sensitivity = 0.3
prediction_error_sensitivity = 0.4
critical_period_strength = 1.0
maturation_acceleration = 1.2
synaptic_delivery_threads = 4
enable_optimized_spike_delivery = true
spike_delivery_batch_size = 100
cache_connectivity = true
```

### Advanced Command Line Options (Phase 6)

NLM now supports extensive command-line options for debugging, validation, and controlling simulation behavior:

#### Core Options
```bash
# Basic execution
nlm [OPTIONS]

# Debug and validation
nlm --debug                  # Enable debug logging
nlm --validate               # Validate configuration before running
nlm --profile                # Enable performance profiling

# Configuration management
nlm --export                 # Export current config to JSON
nlm --import=config.json     # Import configuration from JSON file
nlm --config=file.cfg        # Legacy config file support

# Learning control (independent mechanisms)
nlm --enable-learning        # Enable learning mechanisms (default)
nlm --no-learning            # Disable learning mechanisms
nlm --enable-plasticity      # Enable synaptic plasticity (default)
nlm --no-plasticity          # Disable synaptic plasticity
nlm --enable-development     # Enable developmental processes (default)
nlm --no-development         # Disable developmental processes
nlm --enable-neuromodulation  # Enable neuromodulation (default)
nlm --no-neuromodulation     # Disable neuromodulation
nlm --enable-curiosity       # Enable curiosity-driven exploration
nlm --no-curiosity           # Disable curiosity
nlm --enable-novelty         # Enable novelty detection
nlm --no-novelty             # Disable novelty detection

# Network inspection
nlm --inspect=neurons        # Inspect neuron structure
nlm --inspect=synapses       # Inspect synapse statistics
nlm --inspect=regions        # Inspect brain region structure
n(target|------|-------------|---)

For complete documentation including installation, building, configuration, and advanced features, please refer to the README.md file which contains comprehensive documentation for all phases of the NLM project.

This HOW_TO_USE.md file focuses on basic usage patterns and common scenarios for beginners.

For advanced features, configuration validation, network inspection, performance tuning, and comprehensive command-line options, please see the main README.md file.

Phase 6 introduces extensive advanced configuration and control options for power users, including:

- Enhanced configuration system with JSON support
- Command-line validation and debugging tools
- Network inspection and analysis capabilities
- Performance tuning options
- Learning mechanism control
- Configuration import/export functionality

For complete documentation of all advanced features and Phase 6 capabilities, please refer to the main README.md file.

---

## Getting Help

### Online Resources

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Discussions**: Project GitHub discussions
- **Examples**: https://github.com/nlm-project/nlm-examples

### Support

For questions not covered in this documentation:
1. Check the main README.md for comprehensive documentation
2. Review the source code comments and headers
3. Use `nlm --help` for current command-line options
4. Experiment with small configurations first

### Project-Specific Questions

For NLM-specific questions:
- **Phase 1**: Basic project setup and building
- **Phase 2**: Core brain components and simulation
- **Phase 3**: World interaction and agent systems
- **Phase 4**: Advanced cognition and prediction systems
- **Phase 5**: Performance optimizations and advanced features
- **Phase 6**: Final integration and advanced configuration

### Example Search

Use `grep -r "example" .` to find example scripts and code snippets.

---

## Quick Reference

### Basic Commands

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run basic simulation
./nlm

# Run Phase 6 integration test
./nlm_phase6_demo

# Run unit tests
./nlm_test
```

### Python Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
```

### Configuration Files

- **Default configuration**: `configs/default.cfg`
- **Phase 6 advanced configuration**: See README.md for examples
- **Python examples**: Available in main project directory

---

## Best Practices

### For Beginners

1. **Always call `initialize()` before `step()`**
2. **Use small configurations for testing**
3. **Enable debugging for complex simulations**
4. **Use validation before running production configurations**
5. **Start with examples and modify gradually**

### For Advanced Users

1. **Use `--debug --validate` for complex configurations**
2. **Profile performance with `--profile`**
3. **Inspect networks with `--inspect=neurons`**
4. **Control learning mechanisms precisely**
5. **Export/import configurations for reproducibility**

---

## Next Steps

1. **Read README.md** for comprehensive documentation
2. **Check examples** in the project directory
3. **Experiment with configurations**
4. **Join discussions** for questions
5. **Contribute** to the project

The NLM project continues to evolve. Always check the latest documentation for the most current features and capabilities.

---

## Version Information

- **Current Phase**: Phase 6: Final Integration
- **Documentation**: README.md contains comprehensive documentation
- **Last Updated**: 2026
- **Status**: All documentation available in main README.md

For the most complete and up-to-date documentation, please refer to the main README.md file.

---

# NLM (Neural Learning Machine) - Advanced Features Reference (Phase 6)

This section documents the advanced features available in NLM Phase 6. For complete documentation, please refer to the main README.md file.

## Advanced Features Summary

Phase 6 introduces extensive advanced configuration and control options:

### 1. Enhanced Configuration System
- JSON/YAML configuration file support
- Comprehensive parameter validation
- Type conversion and error handling
- Configuration import/export functionality

### 2. Command-Line Control
- `--debug` / `--no-debug` for logging control
- `--validate` for configuration validation
- `--profile` for performance profiling
- `--inspect=neurons` for network analysis
- Independent learning mechanism control

### 3. Performance Tuning
- Thread-based synaptic delivery
- Optimized spike delivery algorithms
- Connectivity caching
- Batch processing for better performance

### 4. Memory System Configuration
- Working memory capacity control
- Episodic memory management
- Memory replay and consolidation
- Memory system performance tuning

### 5. Neuromodulation Control
- Dopamine-based learning
- Curiosity-driven exploration
- Novelty detection
- Reward prediction error handling

### 6. Developmental System
- Critical period control
- Maturation acceleration
- Age-dependent plasticity
- Developmental stage management

For complete documentation of all features, parameters, and examples, please refer to the main README.md file.

---

*End of Advanced Features Reference. See main README.md for comprehensive documentation.*
