# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API
4. Using the NLM Command Line Interface (CLI)

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)
- **RapidJSON**: For JSON configuration support

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
| `nlm_cli` | Executable | Command line interface tool |

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
pip install pybind11 scikit-build-core pytest numpy rapidjson

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
print(pynlm.VERSION)

# Create a simple brain
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
print(f"Neurons: {brain.get_total_neuron_count()}")
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.create_default_config()

# Optionally configure specific parameters
config.set("neuron_count", 1000)
config.set("connection_probability", 0.1)

# Create and initialize the brain
brain = pynlm.create_brain(config)
brain.initialize()

print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.get_firing_neuron_count()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.create_simple_world()

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
world.set_agent_start(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.create_default_config()
    
    # 2. Create brain and initialize
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.create_agent_brain(brain)
    
    # 4. Create and configure world
    world = pynlm.create_simple_world()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.get_sensory_percept()
        
        # Process sensory input in brain
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decode_motor_command()
        
        # Apply motor command to world
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        # Apply reward modulation
        reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Novelty: {agent.get_novelty_level():.3f}")
            print(f"  Dev Stage: {brain.get_developmental_stage()}")
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation(1000)
```

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.create_brain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receive_sensory_input(sensory_input)  # Inject sensory data
action = brain.produce_action()             # Get motor action

# Statistics
brain.get_total_neuron_count()       # Total neurons
brain.get_total_synapse_count()      # Total synapses
brain.get_firing_neuron_count()     # Currently firing
brain.get_average_firing_rate()      # Average firing rate
brain.get_excitation_inhibition_ratio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.add_region("cortex")
region = brain.get_region(region_id)
```

#### Config Class

```python
# Create
config = pynlm.create_default_config()

# Query
if config.has("neuron_count"):
    keys = config.get_keys()

# Modify
config.set("neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.load_from_file("config.json")
config.save_to_file("config.json")
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.create_agent_brain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.process_sensory_input(percept)

# Motor decoding
motor_cmd = agent.decode_motor_command()

# Neuromodulation
agent.apply_reward_modulation(reward, predicted_reward)

# Development
agent.update_development(timestep)
agent.get_developmental_stage()

# Subsystem enable/disable
agent.enable_reward_modulation(True/False)
agent.enable_structural_plasticity(True/False)
agent.enable_development(True/False)
agent.enable_curiosity(True/False)

# Statistics
agent.get_curiosity_level()
agent.get_novelty_level()
agent.get_prediction_error()
agent.get_neuromodulation_level()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.create_simple_world()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.set_random_seed(42)

# Agent interaction
world.set_agent_start(x, y)
world.apply_motor_command(motor_cmd, current_time)

# State access
percept = world.get_sensory_percept()
body = world.get_agent_body()

# World objects
world.add_object(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.remove_object(x, y)
world.is_valid_position(x, y)

# Properties
world.get_width()
world.get_height()
world.get_simulation_time()
world.get_max_energy()
world.set_max_energy(100.0)
```

### Enumerations

```python
# Neuron types
pynlm.NEURON_TYPES.EXCITATORY
pynlm.NEURON_TYPES.INHIBITORY
pynlm.NEURON_TYPES.SENSORY
pynlm.NEURON_TYPES.MOTOR
pynlm.NEURON_TYPES.INTERNAL
pynlm.NEURON_TYPES.MODULATORY

# Developmental stages
pynlm.DEVELOPMENTAL_STAGE.INITIAL
pynlm.DEVELOPMENTAL_STAGE.CRITICAL_PERIOD
pynlm.DEVELOPMENTAL_STAGE.MATURATION
pynlm.DEVELOPMENTAL_STAGE.ADULT
pynlm.DEVELOPMENTAL_STAGE.AGING

# Action types
pynlm.ACTION_TYPES.MOVE_FORWARD
pynlm.ACTION_TYPES.MOVE_BACKWARD
pynlm.ACTION_TYPES.TURN_LEFT
pynlm.ACTION_TYPES.TURN_RIGHT
pynlm.ACTION_TYPES.INTERACT
pynlm.ACTION_TYPES.EAT
# ... and more

# World object types
pynlm.WORLD_OBJECT_TYPES.RESOURCE
pynlm.WORLD_OBJECT_TYPES.HAZARD
pynlm.WORLD_OBJECT_TYPES.WALL
pynlm.WORLD_OBJECT_TYPES.MARKER
```

---

## Part 4: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `region_count` | int | 1 | Number of neural regions |
| `connection_probability` | float | 0.05 | Probability of connection between neurons |
| `initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `v_rest` | float | -70.0 | Resting potential (mV) |
| `v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `tau_ref` | float | 2.0 | Refractory period (ms) |

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
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
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
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()
agent = pynlm.create_agent_brain(brain)
world = pynlm.create_simple_world()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(0)
    world.apply_motor_command(agent.decode_motor_command(), world.get_simulation_time())
```

---

## Part 6: Command Line Interface

### Quick Start

```bash
# Create a brain simulation
nlm create my_brain --neurons 1000 --steps 500

# Run an existing simulation
nlm run my_brain --steps 200

# Run a performance benchmark
nlm benchmark 1000

# Show help
nlm help

# Show version information
nlm version
```

### Available Commands

| Command | Description |
|---------|-------------|
| `create` | Create a new brain simulation |
| `run` | Run an existing simulation |
| `config` | Manage configuration files |
| `benchmark` | Run performance benchmark |
| `help` | Show help information |
| `version` | Show version information |

### Configuration Management

```bash
# Create a configuration file
nlm config my_config create

# Load and view configuration
nlm config my_config show

# Validate configuration
nlm config my_config validate

# Load configuration from file
nlm config my_config load config.json
```

### Example: Complete Workflow

```bash
# Create a new brain simulation
nlm create my_brain --neurons 1000 --steps 500

# Run the simulation
nlm run my_brain --steps 1000

# Load and examine configuration
nlm config my_brain show

# Validate configuration
nlm config my_brain validate

# Run benchmark
nlm benchmark 500 --steps 2000
```

---

## Part 7: Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11 rapidjson
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

### CLI Issues

**Command not found:**
```bash
# Make sure nlm_cli.py is in your PATH or run it directly:
python nlm_cli.py --help
```

**Permission denied:**
```bash
chmod +x nlm_cli.py
```

---

## Part 8: Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

---

## Installation Summary

The NLM project now provides:

1. **Python Bindings** with simplified, Pythonic APIs
2. **Command Line Interface** for common operations
3. **Configuration System** with JSON/YAML support
4. **Error Handling** and validation
5. **Performance Tools** and benchmarking
6. **Documentation** and examples
7. **Testing Framework** with comprehensive test suites

### Quick Installation

```bash
# Clone and build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install .

# Install CLI tool
pip install -e .
```

### First Steps

```bash
# Create your first brain simulation
nlm create test_brain --neurons 100 --steps 100

# Run a benchmark
nlm benchmark 500

# Get help
nlm help
```

The NLM project continues to evolve with improved usability and functionality for both beginners and advanced users.