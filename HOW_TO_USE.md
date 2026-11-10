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

  world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
  ```

---

## Advanced Python API Features

### Configuration Schema Validation

NLM now includes advanced configuration schema validation to ensure your configurations are correct before simulation.

```python
import pynlm

# Create configuration with schema validation
config = pynlm.createDefaultConfig()

# Define schema with validators
def validateNeuronCount(value):
    return 10 <= value <= 10000

def validateConnectionProbability(value):
    return 0.0 <= value <= 1.0

# Add schema (automatically performed when setting values with validation)
config.addSchema({
    "neuron_count": {
        "type": "int",
        "required": True,
        "minValue": 10,
        "maxValue": 10000,
        "description": "Number of neurons in the brain"
    },
    "connection_probability": {
        "type": "double",
        "required": False,
        "minValue": 0.0,
        "maxValue": 1.0,
        "description": "Probability of synapse formation"
    }
})

# Validate configuration
if config.validateSchema():
    print("✓ Configuration schema validation passed!")
else:
    print("✗ Configuration schema validation failed")
    print("Invalid keys:", config.getInvalidKeys())

# Set values with automatic validation
config.set("neuron_count", 200)  # Valid
config.set("connection_probability", 0.3)  # Valid

# This will raise an exception due to validation
try:
    config.set("neuron_count", 5)  # Too small
    print("ERROR: Invalid value accepted!")
except Exception as e:
    print(f"✓ Correctly rejected invalid value: {e}")
```

### Command Line Interface (CLI)

NLM includes a powerful CLI tool for common operations:

```bash
# Help and usage information
./nlm-cli --help
./nlm-cli help

# Version information
./nlm-cli --version
./nlm-cli version

# Run basic simulation
./nlm-cli run --steps=1000 --neurons=500 --timestep=0.001

# Run simulation with verbose output
./nlm-cli run --steps=500 --neurons=100 --verbose

# Configuration management
./nlm-cli config --load=default.cfg
./nlm-cli config --save=my_config.cfg
./nlm-cli config --show

# Brain state management
./nlm-cli brain --save=checkpoint.brain
./nlm-cli brain --load=checkpoint.brain

# System information
./nlm-cli --info
```

### Batch Operations and Performance Monitoring

```python
import pynlm
import time

# Create configuration with performance settings
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run performance benchmark
print("Running performance benchmark...")
start_time = time.time()

for step in range(1000):
    brain.step(step)
    
    # Batch processing with progress monitoring
    if step % 100 == 0:
        elapsed = time.time() - start_time
        print(f"  Step {step}: {elapsed:.2f}s elapsed, "
              f"{brain.getTotalSpikeCount()}/{step} spikes")

elapsed = time.time() - start_time
steps_per_second = 1000 / elapsed
print(f"\nBenchmark completed:")
print(f"  Time: {elapsed:.2f}s")
print(f"  Steps/sec: {steps_per_second:.0f}")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
```

### Configuration File Management

```python
import pynlm
import os

# Create and save configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 300)
config.set("connection_probability", 0.2)
config.set("stdp_ltp_weight", 0.025)
config.set("stdp_ltd_weight", 0.01)

# Save configuration
config.saveToFile("production_config.cfg")
print("✓ Configuration saved to production_config.cfg")

# Load and validate configuration
config2 = pynlm.createDefaultConfig()
if config2.loadFromFile("production_config.cfg"):
    print("✓ Configuration loaded successfully")
    
    # Validate against schema
    if config2.validateSchema():
        print("✓ Configuration schema validation passed")
    else:
        print("✗ Configuration schema validation failed")
        print(f"Invalid keys: {config2.getInvalidKeys()}")
        
    print("\nConfiguration summary:")
    print(config2.summary())
else:
    print("✗ Failed to load configuration")

# Clean up
if os.path.exists("production_config.cfg"):
    os.remove("production_config.cfg")
    print("✓ Cleaned up production_config.cfg")
```

### Error Handling and Robustness

```python
import pynlm

# Create configuration with error handling
config = pynlm.createDefaultConfig()

# Test error handling during configuration loading
print("Testing error handling...")

# Test 1: Load non-existent file
if not config.loadFromFile("nonexistent.cfg"):
    print("✓ Correctly handled missing configuration file")

# Test 2: Validate configuration with schema
def validateCustom(value):
    return value > 0 and value < 1000

config.addSchema({
    "test_value": {
        "type": "int",
        "validator": validateCustom,
        "description": "Test value with custom validation"
    }
})

# Set valid value
config.set("test_value", 500)
print("✓ Valid value accepted")

# Set invalid value (should raise exception)
try:
    config.set("test_value", -10)
    print("✗ ERROR: Invalid value was accepted!")
except Exception as e:
    print(f"✓ Invalid value correctly rejected: {e}")

# Show invalid keys
invalid_keys = config.getInvalidKeys()
if invalid_keys:
    print(f"⚠ Invalid configuration keys: {invalid_keys}")
else:
    print("✓ All configuration keys are valid")
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance measurements
- [LIMITATIONS.md](docs/LIMITATIONS.md) - Project limitations and future work

---

## Quick Reference

### Python API Functions

| Function | Description | Example |
|----------|-------------|---------|
| `createDefaultConfig()` | Create default configuration | `config = pynlm.createDefaultConfig()` |
| `createBrain(config)` | Create brain with configuration | `brain = pynlm.createBrain(config)` |
| `createSimpleWorld()` | Create simple 2D world | `world = pynlm.createSimpleWorld()` |
| `createAgentBrain(brain)` | Create agent interface | `agent = pynlm.createAgentBrain(brain)` |

### Configuration Keys

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | int | 1000 | Number of neurons |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | double | 0.1 | Synapse formation probability |
| `simulation_timestep` | double | 0.001 | Simulation timestep |
| `stdp_ltp_weight` | double | 0.01 | STDP LTP weight |
| `stdp_ltd_weight` | double | 0.012 | STDP LTD weight |

### Common Errors

1. **Not initialized**: Always call `brain.initialize()` before `brain.step()`
2. **Configuration not loaded**: Check return value of `loadFromFile()`
3. **Invalid values**: Use schema validation to catch errors early
4. **Missing files**: Handle file not found errors gracefully

### Performance Tips

1. Use `nlm-cli run --steps=1000 --neurons=500` for benchmarking
2. Enable validation with `config.validateSchema()`
3. Use batch operations for large simulations
4. Monitor performance with the CLI tool

### Next Steps

1. Read [ARCHITECTURE.md](docs/ARCHITECTURE.md) to understand the system architecture
2. Try the phase demos: `./nlm_phase3_demo`, `./nlm_phase4_demo`, `./nlm_phase6_demo`
3. Build with Python bindings: `pip install .`
4. Run the benchmark: `./nlm_benchmark`
5. Explore the Python API examples in this guide

---

NLM is continuously evolving with new features and improvements. Check the project documentation for the latest updates and examples.
