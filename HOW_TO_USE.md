# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the enhanced NLM Python API

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
print(pynlm.getVersion())

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.total_neurons}")  # NEW: Pythonic property!
print(f"Full stats: {brain.getStats()}")  # NEW: Get stats as dictionary!
```

---

## Part 3: Using the Enhanced Python Library

### Basic Usage with Enhanced API

#### Creating a Brain

```python
import pynlm

# Create a default configuration with standard settings (NEW!)
config = pynlm.Config()
config.configure_standard()  # NEW convenience method

# Or use Pythonic properties
config.neuron_count = 1000  # NEW: Property setter
config.synapse_density = 0.1  # NEW: Property setter

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.total_neurons}")  # NEW: Pythonic property!
print(f"Total synapses: {brain.total_synapses}")  # NEW: Pythonic property!
```

#### Running a Simulation (ENHANCED)

```python
import pynlm

# Create brain
config = pynlm.Config()
config.configure_standard()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.firing_neurons} firing neurons")  # NEW: Property
```

#### Creating a Simple World with Objects (NEW!)

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleDemoWorld()  # NEW: Creates world with objects!

# The world now contains:
# - A resource at (5.0, 5.0) with value 1.0
# - A hazard at (15.0, 15.0) with radius 0.5
# - A wall at (10.0, 5.0) with radius 0.5
# - Agent starts at (10.0, 10.0)

world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)

# Get resource value at a position
resource_value = world.getResourceAt(5.0, 5.0)
print(f"Resource at (5,5): {resource_value}")
```

### Enhanced Agent Example with Complete Workflow

```python
import pynlm

def run_enhanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced API."""
    
    # 1. Create configuration using enhanced API
    config = pynlm.Config()
    config.configure_standard()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent using convenience factory (NEW!)
    agent = pynlm.createDefaultAgent()  # NEW: Creates agent with brain!
    
    # 4. Create and configure world with objects (NEW!)
    world = pynlm.createSimpleDemoWorld()  # NEW: Pre-configured world
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enable_reward_modulation = True  # NEW: Property setter
    agent.enable_curiosity = True         # NEW: Property setter
    
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
        motor_cmd = agent.decode_motor_command()  # NEW: Pythonic property
        
        # Apply motor command to world
        world.apply_motor_command(motor_cmd, world.get_simulation_time())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Print enhanced progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.firing_neurons}")  # NEW: Property
            print(f"  Curiosity: {agent.curiosity_level:.3f}")  # NEW: Property
            print(f"  Novelty: {agent.novelty_level:.3f}")      # NEW: Property
            print(f"  Dev Stage: {brain.developmental_stage}")   # NEW: Property
            print(f"  Agent status: {agent.get_system_status()}")  # NEW: Enhanced method
    
    return brain, world, agent

# Run the enhanced simulation
brain, world, agent = run_enhanced_agent_simulation(1000)
```

### Context Managers (NEW!)

```python
# Use brain as context manager (automatic cleanup)
with pynlm.createDefaultBrain() as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
    print(f"Brain created {brain.total_neurons} neurons")

# Use agent as context manager
with pynlm.createDefaultAgent() as agent:
    world = pynlm.createSimpleDemoWorld()
    agent.initialize(world)
    # ... do work ...
    print(f"Agent curiosity: {agent.curiosity_level}")
```

### Key Classes and Methods (ENHANCED)

#### Brain Class (ENHANCED)

```python
# Creation
brain = pynlm.createBrain(config)
brain = pynlm.createDefaultBrain()  # NEW: Convenience factory

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics (ENHANCED)
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getStats()                  # NEW: All stats as dictionary
brain.total_neurons               # NEW: Pythonic property
brain.firing_neurons              # NEW: Pythonic property

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class (ENHANCED)

```python
# Create
config = pynlm.createDefaultConfig()

# Query using Pythonic properties
if config.neuron_count > 100:      # NEW: Property access
    print(f"Large brain: {config.neuron_count} neurons")

# Modify using Pythonic properties
config.neuron_count = 2000         # NEW: Property setter
config.synapse_density = 0.05      # NEW: Property setter

# Use convenience methods
config.configure_standard()        # NEW: Standard configuration
config.configure_compact()         # NEW: Compact configuration

# Dictionary-like interface (NEW!)
if "brain.neuron_count" in config:  # NEW: 'in' operator
    print(f"Neuron count: {config['brain.neuron_count']}")  # NEW: [] operator

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class (ENHANCED)

```python
# Creation
agent = pynlm.createAgentBrain(brain)
agent = pynlm.createDefaultAgent()  # NEW: Convenience factory

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()
motor_cmd = agent.decode_motor_command()  # NEW: Pythonic property

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# System status (NEW!)
status = agent.getSystemStatus()  # Returns dictionary with all system info
print(f"Curiosity level: {status['curiosity']}")

# Subsystem enable/disable using Pythonic properties
agent.enable_reward_modulation = True
agent.enable_curiosity = False

# Check status
if agent.isReady():  # NEW: Method to check if agent is ready
    print("Agent is ready for operation")
```

#### SimpleWorld Class (ENHANCED)

```python
# Creation
world = pynlm.createSimpleWorld()
world = pynlm.createSimpleDemoWorld()  # NEW: World with objects

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

# Object creation (NEW!)
world.createResource(x, y, value=1.0)
world.createHazard(x, y)
world.createWall(x, y)

# World properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink

# World object types
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
```

### Configuration Options

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.random_seed` | int | 0 | Random seed for reproducibility |

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Advanced Factory Functions (NEW!)

```python
# Create brain with standard configuration and initialize it
brain = pynlm.createDefaultBrain()

# Create agent with brain
agent = pynlm.createDefaultAgent()

# Create world with objects
world = pynlm.createSimpleDemoWorld()

# Run complete episode
brain, world, agent = pynlm.runSimpleEpisode(100)

# Get version information
version = pynlm.getVersion()

# Get complete example script
example = pynlm.getExample()
```

### Example Scripts

#### Minimal Example

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

#### Environment Interaction (ENHANCED)

```python
import pynlm

# Setup with enhanced API
brain = pynlm.createDefaultBrain()
world = pynlm.createSimpleDemoWorld()
agent = pynlm.createDefaultAgent()

agent.initialize(world)

# Enable enhanced learning
agent.enable_reward_modulation = True
agent.enable_curiosity = True

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

**Pythonic API Issues:**
- Use `agent.enable_reward_modulation = True` instead of `agent.enableRewardModulation(True)`
- Access brain properties as `brain.total_neurons` instead of `brain.getTotalNeuronCount()`

---

## Advanced Topics

### Custom Configurations

```python
import pynlm

# Create custom configuration
custom_config = pynlm.Config()
custom_config.neuron_count = 2000
custom_config.synapse_density = 0.2
custom_config.random_seed = 12345

# Configure specific plasticity settings
custom_config.set("plasticity.stdp.learning_rate", 0.01)
custom_config.set("plasticity.hebbian.enable", False)

brain = pynlm.createBrain(custom_config)
brain.initialize()
```

### Enhanced Statistics Analysis

```python
import pynlm

# Create and run simulation
brain = pynlm.createDefaultBrain()
brain.initialize()

for step in range(1000):
    brain.step(step)

# Get comprehensive statistics
stats = brain.getStats()
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['firing_rate']:.2f} Hz")
print(f"E/I ratio: {stats['e_i_ratio']:.3f}")

# Check brain health
if brain.is_healthy:
    print("Brain is in healthy state")
else:
    print("Brain may need attention")
```

### Context Manager Best Practices

```python
import pynlm

# Use context managers for automatic resource management
try:
    with pynlm.createDefaultBrain() as brain:
        brain.initialize()
        for step in range(500):
            brain.step(step)
        print(f"Successfully ran simulation with {brain.total_neurons} neurons")
    
    # Brain is automatically reset after exiting context
    
except Exception as e:
    print(f"Simulation failed: {e}")
```

### Performance Monitoring

```python
import time
import pynlm

# Monitor performance with enhanced API
brain = pynlm.createDefaultBrain()
brain.initialize()

start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

elapsed = end_time - start_time
stats = brain.getStats()

print(f"Simulation completed in {elapsed:.2f} seconds")
print(f"Average firing rate: {stats['firing_rate']:.2f} Hz")
print(f"Total neurons: {stats['total_neurons']}")
```

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [README.md](README.md) - Project overview and setup instructions
- [easy_usage.md](easy_usage.md) - Quick start guide with examples
- [test_improvements.py](test_improvements.py) - Comprehensive Python tests
- [example.py](example.py) - Complete example script

## What's New in This Enhanced Version?

1. **Pythonic API**: Properties instead of getters/setters, context managers
2. **Convenience Factory Methods**: `createDefaultBrain()`, `createDefaultAgent()`, `createSimpleDemoWorld()`
3. **Enhanced Configuration**: `config.configure_standard()`, `config.configure_compact()`
4. **Statistics Enhancement**: `brain.getStats()`, `agent.getSystemStatus()`
5. **Object Creation**: `world.createResource()`, `world.createHazard()`, `world.createWall()`
6. **Dictionary Interface**: Config supports `[]` operator and `in` operator
7. **Type Hints**: Improved IDE support
8. **Better Documentation**: Comprehensive examples and guides
9. **Custom Exceptions**: More specific error handling
10. **Enhanced Testing**: Comprehensive test suite with examples

This enhanced version makes NLM much easier to use while providing more power for advanced users!

---

## License

MIT