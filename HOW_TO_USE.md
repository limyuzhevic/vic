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

## Part 8: Enhanced Python Features and Examples

The NLM Python bindings have been significantly enhanced with new features and comprehensive examples:

### New Python Module Features

#### 1. Module Version
```python
import pynlm

# Get the NLM module version
print(f"NLM Version: {pynlm.__version__}")  # "0.1.0"
```

#### 2. Enhanced Error Handling
```python
import pynlm

# Use enhanced error handling
try:
    config = pynlm.createDefaultConfig()
    # Configuration operations that may raise exceptions
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.InvalidArgument as e:
    print(f"Invalid argument: {e}")
```

#### 3. Input Validation
```python
import pynlm

# Validate configuration parameters
pynlm.validate_brain_config(config)

# Validate world dimensions
pynlm.validate_world_dimensions(20, 20, 8, 8)
```

#### 4. Comprehensive Examples Directory

The enhanced NLM includes a comprehensive examples directory:

```
examples/
├── basic_examples.py           # Fundamental usage patterns
├── advanced_examples.py        # Advanced simulation techniques  
├── agent_behavior_examples.py   # Specialized agent behaviors
├── profiling_examples.py       # Performance profiling tools
├── session_management.py       # Save/load functionality
└── README.md                   # Examples documentation
```

#### 5. Helper Functions and Utilities

Import from `helpers/advanced_helpers.py`:

```python
from helpers.advanced_helpers import (
    # Agent creation and configuration
    create_agent_with_behavior,
    create_explorer_agent,
    create_survival_agent,
    create_developer_agent,
    
    # Simulation and analysis
    simulate_episode,
    run_multiple_episodes,
    analyze_episode_results,
    
    # Session management
    save_agent_session,
    load_agent_session,
    
    # Performance tools
    NLMProfiler,
    create_profiling_wrapper,
    
    # Enums and constants
    NLMTaskTypes,
    NLMActionTypes
)
```

### Example Usage Patterns

#### Advanced Configuration
```python
import pynlm
from helpers.advanced_helpers import create_agent_with_behavior, NLMTaskTypes

# Create agent with advanced configuration
brain, world, agent = create_agent_with_behavior(
    world_width=30,
    world_height=30,
    task_type=NLMTaskTypes.EXPLORATION,
    enable_subsystems={
        "reward_modulation": True,
        "structural_plasticity": True,
        "development": True,
        "curiosity": True
    }
)
```

#### Development Experiment
```python
import pynlm
from helpers.advanced_helpers import create_explorer_agent, simulate_episode

# Create development agent
brain, world, agent = create_explorer_agent()

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run development experiment
episode_results = []
for episode in range(5):
    result = simulate_episode(brain, world, agent, max_steps=500, verbose=True)
    episode_results.append(result)
```

#### Performance Profiling
```python
import pynlm
from helpers.advanced_helpers import NLMProfiler

# Create profiler
profiler = NLMProfiler("My Simulation")

# Run simulation with profiling
for step in range(100):
    world, time_update = profiler.time_operation("world_update", world.update, 0.1)
    brain, time_brain = profiler.time_operation("brain_step", brain.step, step)
    action, time_action = profiler.time_operation("action_decode", agent.decodeMotorCommand)
    world, time_apply = profiler.time_operation("action_apply", world.applyMotorCommand, action, world.getSimulationTime())

# Get performance report
report = profiler.get_summary()
print(f"Total time: {report['total_time']:.2f}s")
print(f"Average operation time: {report['average_operation_time']:.4f}s")
```

#### Session Management
```python
import pynlm
from helpers.advanced_helpers import save_agent_session

# Create and run agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Save agent session
session = save_agent_session(
    brain, world, agent,
    session_name="my_exploration",
    custom_metadata={
        "task": "exploration",
        "duration": 500,
        "neurons": 1000
    }
)
```

#### Advanced Examples

Run the comprehensive examples:

```bash
# Launch basic examples
python examples/basic_examples.py

# Run advanced examples  
python examples/advanced_examples.py

# Run profiling examples
python examples/profiling_examples.py

# Run session management examples
python examples/session_management.py
```

Each example directory provides:

- **Documentation and comments** explaining the concepts
- **Complete runnable examples** with output analysis
- **Error handling** for robust operation
- **Performance monitoring** where applicable
- **Session management** for reproducibility

### Key Enhancements Summary

#### 1. Python Module Improvements
- ✅ Added module version (`__version__ = "0.1.0"`)
- ✅ Enhanced error handling with custom exception classes
- ✅ Input validation functions for configuration and parameters
- ✅ Template-based ID class bindings to reduce boilerplate

#### 2. Examples and Documentation
- ✅ Comprehensive examples directory with multiple use cases
- ✅ Helper functions for common tasks and configurations
- ✅ Performance profiling tools for optimization
- ✅ Session management for reproducibility
- ✅ Updated documentation with enhanced features

#### 3. Advanced Capabilities
- ✅ Development stage experiments
- ✅ Memory system analysis
- ✅ Curiosity-driven behavior
- ✅ Performance profiling and optimization
- ✅ Agent behavior specialization

### Quick Start with Enhanced Features

For users who want to leverage the enhanced features:

```python
# 1. Import enhanced modules
import pynlm
from helpers.advanced_helpers import create_explorer_agent, NLMProfiler

# 2. Create advanced agent
brain, world, agent = create_explorer_agent()

# 3. Run simulation with profiling
profiler = NLMProfiler("Advanced Simulation")

for step in range(200):
    world, _ = profiler.time_operation("world_update", world.update, 0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# 4. Get performance report
report = profiler.get_summary()
print(f"Simulation completed in {report['total_time']:.2f}s")
```

### Migration Guide

If you're upgrading from previous versions:

#### Breaking Changes
- None - all changes are backward compatible
- Enhanced functionality adds to existing API
- New features are opt-in via imports

#### New Features Available
```python
# Old way (still works)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# New way with enhanced features
brain, world, agent = create_explorer_agent()
```

#### Additional Resources
- `examples/` - Comprehensive examples directory
- `helpers/advanced_helpers.py` - Utility functions and helpers
- `docs/` - Technical documentation and architecture
- `README.md` - Project overview and setup instructions

## Quick Reference: Enhanced NLM Python API

| Feature | Import | Description |
|---------|--------|-------------|
| Module Version | `import pynlm` | `pynlm.__version__` returns "0.1.0" |
| Error Handling | `import pynlm` | Custom exception classes for better error messages |
| Validation | `import pynlm` | `validate_brain_config()`, `validate_world_dimensions()` |
| Agent Creation | `helpers/advanced_helpers` | `create_explorer_agent()`, `create_survival_agent()`, etc. |
| Simulation | `helpers/advanced_helpers` | `simulate_episode()`, `run_multiple_episodes()` |
| Profiling | `helpers/advanced_helpers` | `NLMProfiler`, `create_profiling_wrapper()` |
| Sessions | `helpers/advanced_helpers` | `save_agent_session()`, `load_agent_session()` |

### Recommended Workflow

1. **Start with basic usage** using the existing `easy_usage.md` guide
2. **Explore examples** in the `examples/` directory
3. **Use helper functions** for common tasks and configurations
4. **Enable profiling** for performance optimization
5. **Implement session management** for reproducibility
6. **Create custom examples** using the helper functions and utilities

The enhanced NLM Python bindings provide a comprehensive toolkit for neural simulation, from simple getting started examples to advanced research and development scenarios.

### Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
