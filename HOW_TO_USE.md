# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the enhanced NLM Python API (with new convenience features)

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

## Part 3: Using the Enhanced Python Library

### Basic Usage with New Convenience Features

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create and initialize the brain using enhanced convenience method
brain = pynlm.createBrainWithDefaults(neuronCount=1000, enableDevelopment=True)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain with advanced features
brain = pynlm.createBrainWithDefaults(neuronCount=500, enableLearning=True)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Enhanced Agent Simulation

```python
import pynlm

def run_advanced_agent_simulation(world_width=20, world_height=20, 
                                   num_steps=1000, strategy="balanced"):
    """Run a complete agent simulation with brain and world using new features."""
    
    # 1. Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize using convenience method
    brain = pynlm.createBrainWithDefaults(
        neuronCount=1000,
        regionCount=1,
        connectionProbability=0.1,
        enableLearning=True,
        enableDevelopment=True
    )
    brain.initialize()
    
    # 3. Create agent brain interface with strategy
    agent = pynlm.create_advanced_agent(brain, None, strategy)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=world_width, height=world_height, 
                   visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(world_width // 2, world_height // 2)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems based on strategy
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop with logging
    logs = []
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
        
        # Collect logs
        if step % 100 == 0:
            log_msg = f"Step {step}: "
            log_msg += f"Firing: {brain.getFiringNeuronCount()}, "
            log_msg += f"Curiosity: {agent.getCuriosityLevel():.3f}, "
            log_msg += f"Dev: {brain.getDevelopmentalStage()}"
            logs.append(log_msg)
            print(log_msg)
    
    # 8. Get brain statistics
    stats = pynlm.getBrainStats(brain)
    
    return logs, stats

# Run the simulation
logs, stats = run_advanced_agent_simulation(world_width=15, world_height=15, num_steps=500)

# Save experiment data
pynlm.export_experiment_data(logs, stats, "./experiments/advanced_agent_test")

# Print summary
print(f"\n=== Simulation Summary ===")
print(f"Total steps: {len(logs)}")
print(f"Final firing neurons: {stats['firing_neurons']}")
print(f"Final learning rate: {stats['excitatory_inhibition_ratio']:.3f}")
print(f"Development stage: {stats['developmental_stage']}")
```

#### Configuration from Dictionary

```python
import pynlm

# Create configuration from Python dictionary
config_dict = {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.05,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.dopamine.scale": 1.0,
    "neuromod.curiosity.enable": True,
    "development_synaptogenesis_rate": 0.001
}

config = pynlm.createConfigFromDict(config_dict)
brain = pynlm.createBrain(config)
```

---

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
brain.receiveSensoryInput(input)  # Inject sensory data
action = brain.produceAction()   # Get motor action

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

## Part 4: Advanced Features and Convenience Methods

The enhanced NLM Python bindings now include advanced features for expert users:

### 1. Convenience Methods

```python
# Create brain with optimized defaults
brain = pynlm.createBrainWithDefaults(neuronCount=1000, enableDevelopment=True)

# Run complete agent simulation with logging
logs = pynlm.run_agent_episode(brain, world, agent, 1000, verbose=True)

# Get comprehensive brain statistics
stats = pynlm.getBrainStats(brain)

# Create configuration from dictionary
config = pynlm.createConfigFromDict({"brain.neuron_count": 2000})
```

### 2. Advanced Functions

```python
# Explore neural network structure
exploration = pynlm.neural_network_explorer(brain)

# Save/load checkpoints with validation
pynlm.save_brain_checkpoint(brain, "./checkpoints/brain_state.bin")
pynlm.load_brain_checkpoint(brain, "./checkpoints/brain_state.bin")

# Benchmark performance
benchmark = pynlm.benchmark_brain_performance(brain, 1000, detailed=True)

# Create agent with specific strategy
agent = pynlm.create_advanced_agent(brain, world, "curiosity")

# Simulate to developmental stage
logs = pynlm.simulate_to_development_stage(brain, world, agent, pynlm.DevelopmentalStage.Maturation)

# Create brain from template
brain = pynlm.create_brain_from_template("learning", {"neuron_count": 3000})

# Export experiment data
pynlm.export_experiment_data(logs, metrics, "experiment_001")
```

### 3. Error Handling

The enhanced bindings provide custom Python exceptions:

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.InitializationError as e:
    print(f"Initialization failed: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
```

---

## Part 5: Configuration Options

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

## Part 6: Advanced Experiment Scripts

### Enhanced Minimal Example

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

### Enhanced Environment Interaction

```python
import pynlm

# Setup with advanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.create_advanced_agent(brain, None, "balanced")
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

## Part 7: Troubleshooting

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

### Enhanced Error Handling

```python
# The enhanced bindings provide more informative error messages
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.InitializationError as e:
    print(f"Brain initialization failed: {e}")
    print("Please check your configuration settings")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
    print("Please verify configuration values")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
    print("Please check simulation parameters")
```

---

## Part 8: Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## New: Advanced Usage Guide

The enhanced NLM now includes several new advanced features:

### 1. Convenience Functions

These functions simplify common tasks and provide enhanced functionality:

| Function | Description | Use Case |
|----------|-------------|----------|
| `createBrainWithDefaults()` | Create brain with optimized defaults | Quick setup |
| `run_agent_episode()` | Run agent simulation with logging | Experimentation |
| `getBrainStats()` | Get comprehensive brain statistics | Analysis |
| `createConfigFromDict()` | Create config from Python dict | Programmatic config |

### 2. Advanced Features

| Feature | Description | Benefit |
|---------|-------------|---------|
| Custom exceptions | Pythonic error handling | Better debugging |
| Network explorer | Analyze brain structure | Network analysis |
| Checkpoint management | Save/load with validation | State persistence |
| Performance benchmarking | Measure simulation speed | Optimization |
| Brain templates | Create from presets | Quick prototyping |
| Strategy agents | Different agent behaviors | Behavioral studies |

### 3. Example Advanced Workflows

```python
import pynlm

# Example 1: Compare different brain templates
print("=== Brain Template Comparison ===")
templates = ["small", "medium", "large", "learning", "stable"]
results = {}

for template in templates:
    brain = pynlm.create_brain_from_template(template)
    brain.initialize()
    pynlm.run_simulation(brain, None, None, 100)
    stats = pynlm.getBrainStats(brain)
    results[template] = stats
    print(f"{template:10s}: {stats['total_neurons']} neurons, "
          f"{stats['average_firing_rate']:.2f} Hz avg")

# Example 2: Performance benchmarking
print("\n=== Performance Benchmarking ===")
brain = pynlm.createBrainWithDefaults(500)
benchmark = pynlm.benchmark_brain_performance(brain, 1000, detailed=True)
print(f"Speed: {benchmark['steps_per_second']:.0f} steps/sec")
print(f"Memory per step: {benchmark['simulation_time_per_step_ms']:.2f} ms")

# Example 3: Experiment with different strategies
print("\n=== Strategy Comparison ===")
strategies = ["curiosity", "exploitation", "balanced", "creative"]
for strategy in strategies:
    brain = pynlm.createBrainWithDefaults(300)
    world = pynlm.createSimpleWorld()
    agent = pynlm.create_advanced_agent(brain, world, strategy)
    logs = pynlm.run_agent_episode(brain, world, agent, 200)
    stats = pynlm.getBrainStats(brain)
    print(f"{strategy:12s}: {stats['average_firing_rate']:.2f} Hz, "
          f"{stats['excitatory_inhibition_ratio']:.3f} E/I ratio")
```

### 4. Getting Started with Advanced Features

1. **Start with convenience functions**: Use `createBrainWithDefaults()` and `run_agent_episode()`
2. **Explore brain statistics**: Use `getBrainStats()` to understand brain behavior
3. **Experiment with strategies**: Try different agent strategies for behavioral studies
4. **Benchmark performance**: Use `benchmark_brain_performance()` for optimization
5. **Save/load experiments**: Use enhanced checkpoint functions

The enhanced NLM provides tools for both simple usage and advanced research tasks, making it suitable for beginners, researchers, and developers alike.

