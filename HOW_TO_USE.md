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

### Advanced Usage (Phase 5+ Features)

#### Context Manager for Automatic Resource Management (NEW!)

```python
import pynlm

# Create advanced configuration with Phase 5+ features
config = pynlm.createAdvancedConfig()

# Use context manager for automatic cleanup and initialization
with pynlm.createBrainWithCleanup(config) as brain:
    # Brain is automatically initialized
    # All resources are automatically cleaned up when context exits
    for step in range(1000):
        brain.step(step)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Brain is automatically reset here - no manual cleanup needed!
print("Context exited - resources automatically cleaned up")
```

#### Event Callback System (NEW!)

```python
import pynlm

# Create event callback system for custom event handling
callbacks = pynlm.createEventCallbackSystem()

# Define custom event handlers
def on_spike(event):
    """Handle spike events from the neural network"""
    print(f"Neuron {event.neuron_id} spiked at time {event.timestamp}")

def on_step(step, time):
    """Handle simulation step events"""
    print(f"Simulation step {step} at {time}ms")

def on_error(error):
    """Handle error events"""
    print(f"ERROR: {error}")

# Register callbacks
callbacks.registerSpikeCallback(on_spike)
callbacks.registerStepCallback(on_step)
callbacks.registerErrorCallback(on_error)

# Use callbacks with brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation - callbacks will be triggered
for step in range(100):
    brain.step(step)

# Check callback statistics
print(f"Callback system stats:")
print(f"  Spike callbacks: {callbacks.getSpikeCallbackCount()}")
print(f"  Step callbacks: {callbacks.getStepCallbackCount()}")
print(f"  Error callbacks: {callbacks.getErrorCallbackCount()}")
```

#### Advanced Configuration (NEW!)

```python
import pynlm

# Create advanced configuration optimized for complex simulations
config = pynlm.createAdvancedConfig()

# Access and modify advanced configuration parameters
print("Learning rate:", config.get("plasticity.stdp.learning_rate"))
print("Neuron count:", config.get("brain.neuron_count"))
print("Curiosity enabled:", config.get("neuromodulation.curiosity.enable"))

# Modify configuration
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.002)

print("Updated configuration successful!")
```

#### Save Configuration to JSON (NEW!)

```python
import pynlm

# Create configuration
config = pynlm.createAdvancedConfig()

# Save configuration to JSON file
pynlm.saveConfigToJSON(config, "advanced_config.json")

print("Configuration saved to advanced_config.json")
print("You can now edit this JSON file and reload it:")

# Load configuration from JSON
import json
with open("advanced_config.json", "r") as f:
    loaded_config = json.load(f)
    print(f"Loaded configuration with {len(loaded_config)} parameters")
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

### Advanced Usage with New Features

#### Context Manager for Automatic Resource Management (NEW!)

```python
import pynlm

# Create advanced configuration with Phase 5+ features
config = pynlm.createAdvancedConfig()

# Use context manager for automatic cleanup and initialization
with pynlm.createBrainWithCleanup(config) as brain:
    # Brain is automatically initialized
    # All resources are automatically cleaned up when context exits
    for step in range(1000):
        brain.step(step)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Brain is automatically reset here - no manual cleanup needed!
print("Context exited - resources automatically cleaned up")
```

#### Event Callback System (NEW!)

```python
import pynlm

# Create event callback system for custom event handling
callbacks = pynlm.createEventCallbackSystem()

# Define custom event handlers
def on_spike(event):
    """Handle spike events from the neural network"""
    print(f"Neuron {event.neuron_id} spiked at time {event.timestamp}")

def on_step(step, time):
    """Handle simulation step events"""
    print(f"Simulation step {step} at {time}ms")

def on_error(error):
    """Handle error events"""
    print(f"ERROR: {error}")

# Register callbacks
callbacks.registerSpikeCallback(on_spike)
callbacks.registerStepCallback(on_step)
callbacks.registerErrorCallback(on_error)

# Use callbacks with brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation - callbacks will be triggered
for step in range(100):
    brain.step(step)

# Check callback statistics
print(f"Callback system stats:")
print(f"  Spike callbacks: {callbacks.getSpikeCallbackCount()}")
print(f"  Step callbacks: {callbacks.getStepCallbackCount()}")
print(f"  Error callbacks: {callbacks.getErrorCallbackCount()}")
```

#### Advanced Configuration and JSON Support (NEW!)

```python
import pynlm

# Create advanced configuration optimized for complex simulations
config = pynlm.createAdvancedConfig()

# Access and modify advanced configuration parameters
print("Learning rate:", config.get("plasticity.stdp.learning_rate"))
print("Neuron count:", config.get("brain.neuron_count"))
print("Curiosity enabled:", config.get("neuromodulation.curiosity.enable"))

# Modify configuration
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.002)

print("Updated configuration successful!")

# Save configuration to JSON file
pynlm.saveConfigToJSON(config, "advanced_config.json")

print("Configuration saved to advanced_config.json")
print("You can now edit this JSON file and reload it using a JSON parser.")
```

#### Complete Agent Example with Advanced Features (NEW!)

```python
import pynlm

def run_advanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with all advanced features."""
    
    # 1. Create advanced configuration
    config = pynlm.createAdvancedConfig()
    
    # 2. Create brain and initialize with context manager
    with pynlm.createBrainWithCleanup(config) as brain:
        # 3. Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        
        # 4. Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # 5. Initialize agent with world
        agent.initialize(world)
        
        # 6. Create and configure event callbacks
        callbacks = pynlm.createEventCallbackSystem()
        
        def on_step_with_progress(step, time):
            """Callback to monitor simulation progress"""
            if step % 100 == 0:
                print(f"Progress: Step {step}, "
                      f"{brain.getFiringNeuronCount()} neurons firing, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        def on_spike_with_stats(event):
            """Callback to monitor neural activity"""
            pass  # Can implement spike statistics here
        
        def on_simulation_error(error):
            """Callback to handle errors gracefully"""
            print(f"Simulation error: {error}")
            # Can implement error recovery here
        
        callbacks.registerStepCallback(on_step_with_progress)
        callbacks.registerSpikeCallback(on_spike_with_stats)
        callbacks.registerErrorCallback(on_simulation_error)
        
        # 7. Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # 8. Run simulation loop with advanced features
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
            
            # Print detailed progress every 500 steps
            if step % 500 == 0:
                print(f"Advanced simulation progress:")
                print(f"  Step: {step}/{num_steps}")
                print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
                print(f"  Total spikes: {brain.getTotalSpikeCount()}")
                print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
                print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

# Run the advanced simulation
print("Starting advanced agent simulation with all features...")
run_advanced_agent_simulation(2000)
print("Advanced simulation complete!")
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
