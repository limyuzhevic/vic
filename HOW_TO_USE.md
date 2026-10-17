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

### Quick Verification

After installation, you can test that everything works:

```python
import pynlm

# Test basic functionality
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Python bindings loaded successfully!")
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

---

## Part 3: Using the NLM Python API

### Overview

The NLM Python API provides a high-level interface to the C++ neural simulation framework. It allows you to:

1. **Create and configure virtual brains**
2. **Simulate brain activity over time**
3. **Interact with simulated environments**
4. **Use agents to connect brains to worlds**
5. **Enable learning and development features**

### Core Classes

| Python Class | C++ Equivalent | Purpose |
|-------------|----------------|---------|
| `pynlm.Config` | `Config` | Brain configuration |
| `pynlm.Brain` | `Brain` | Neural simulation engine |
| `pynlm.SimpleWorld` | `SimpleWorld` | Simulation environment |
| `pynlm.AgentBrain` | `AgentBrain` | Brain-world interface |
| `pynlm.Vision` | `Vision` | Visual sensory input |
| `pynlm.Action` | `Action` | Motor output commands |

### Advanced Features

#### 1. Using Neuromodulation

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation with learning
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### 2. Working with Different Neuron Types

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Inject current into specific neuron types
brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0f)
brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 2.0f)
brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, 1.0f)

# Check brain statistics
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
```

#### 3. Configuration and Customization

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Customize settings
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.2)
config.set("stdp_ltp_weight", 0.025)
config.set("stdp_ltd_weight", 0.02)

# Save configuration to file
config.saveToFile("my_config.json")

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

#### 4. Advanced Agent Features

```python
import pynlm

# Create agent with advanced features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True, 0.5)  # Learning rate
agent.enableCuriosity(True, 0.3)         # Exploration drive
agent.enableDevelopment(True)            # Brain maturation
agent.enableStructuralPlasticity(True, 0.001)  # Synapse growth

# Run enhanced simulation
for step in range(2000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check neuromodulation levels
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Reward modulation: {agent.getNeuromodulationLevel()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel()}")
        print(f"  Novelty level: {agent.getNoveltyLevel()}")
        print(f"  Prediction error: {agent.getPredictionError()}")
        print(f"  Developmental stage: {agent.getDevelopmentalStage()}")
```

---

## Part 4: Building for Advanced Users

### C++ Development

For C++ developers, NLM provides extensive APIs and flexibility:

#### Direct Brain Access

```cpp
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"

// Create brain with custom config
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 5000);
auto brain = std::make_shared<nlm::Brain>(config);

if (brain->initialize()) {
    // Access neural components directly
    auto spikeSystem = brain->getSpikeSystem();
    auto workingMemory = brain->getWorkingMemory();
    auto dopamine = brain->getDopamine();
    
    // Run simulation with direct control
    for (int step = 0; step < 10000; ++step) {
        brain->step(step, step * 0.001);
        // Custom intervention logic here
    }
}
```

#### Environment Customization

```cpp
#include "world/GridWorldEnvironment.hpp"

// Create custom environment
auto env = std::make_unique<nlm::GridWorldEnvironment>(20, 20);
env->reset();

// Custom reward function
float customReward = env->computeCustomReward(agentState);
env->applyAction(agentAction);
```

### Python Extension Development

For those interested in extending the Python API, NLM uses pybind11, making it easy to add custom functionality:

```python
# Example: Custom sensor wrapper
import pynlm

class CustomSensor:
    def __init__(self, world):
        self.world = world
        self.customData = []
    
    def getEnhancedPercept(self):
        percept = self.world.getSensoryPercept()
        # Add custom processing
        enhanced = percept.clone()
        enhanced.addCustomSignal(self.getCustomValue())
        return enhanced

# Use custom sensor
agent.processSensoryInput(customSensor.getEnhancedPercept())
```

---

## Part 5: Troubleshooting

### Common Issues and Solutions

#### "Brain not doing anything"

**Problem**: Brain appears inactive during simulation.

**Solution**:
- Ensure `brain.initialize()` is called before simulation
- Check that `brain.step()` is being called in the loop
- Verify that neurons have appropriate parameters
- Consider increasing simulation steps

#### "Agent not moving"

**Problem**: Agent doesn't respond to sensory input or produce actions.

**Solution**:
- Ensure `world.update()` is called before getting percept
- Check that `agent.processSensoryInput()` is called
- Verify that `agent.decodeMotorCommand()` is working
- Make sure `world.applyMotorCommand()` is called

#### "Everything is 0"

**Problem**: All values are zero, no activity observed.

**Solution**:
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters (neuron_count, connection_probability)
- Enable neuromodulation for better exploration

### Debug Commands

```python
# Enable verbose logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Check brain configuration
brain.logStatus()

# Monitor system state
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

---

## Part 6: Performance Optimization

### For Large-Scale Simulations

#### 1. Use Appropriate Configuration

```python
# For large simulations, consider these settings
config.set("neuron_count", 10000)
config.set("region_count", 5)
config.set("connection_probability", 0.05)  # Less dense connections
config.set("simulation_timestep", 0.0005)   # Smaller time steps
```

#### 2. Parallel Processing

```cpp
// Use SIMD optimizations where available
#pragma GCC optimize("O3")
#pragma GCC target("avx2")
```

#### 3. Memory Management

```python
# For long simulations, consider checkpointing
brain.save("checkpoint_1000.bin")
# Later...
brain.load("checkpoint_1000.bin")
```

### Monitoring Tools

```python
# Add performance monitoring
import time

start_time = time.time()
for step in range(10000):
    brain.step(step)
    
end_time = time.time()
print(f"Simulation time: {end_time - start_time:.2f}s")
print(f"Steps per second: {10000 / (end_time - start_time):.0f}")
```

---

## Part 7: Advanced Topics

### 1. Memory Management

```python
# Manual memory management for advanced users
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Save checkpoints regularly
brain.save("checkpoint_500.bin")

# Load from checkpoint if needed
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
brain2.load("checkpoint_500.bin")
```

### 2. Custom Actions

```python
import pynlm

# Create custom action types
custom_action = pynlm.Action(pynlm.ActionType.Custom)
custom_action.setParameters([0.5, 0.3, 0.8])

# Use custom action
world.applyMotorCommand(custom_action, world.getSimulationTime())
```

### 3. Sensory Input Processing

```python
import pynlm

# Create custom vision input
vision = pynlm.Vision(32, 32, 3)
# Fill with custom data
vision_data = [0.1] * 32 * 32 * 3
vision.setData(vision_data)

# Process through agent
agent.processSensoryInput(vision)
```

---

## Conclusion

NLM provides a comprehensive framework for building and simulating artificial brains. Whether you're a beginner looking to understand basic concepts or an advanced user wanting to push the boundaries of neural simulation, NLM offers the tools you need.

**For more information:**
- Visit the documentation website
- Join the NLM community forum
- Check out example projects and tutorials

That's it! You're now ready to use NLM for neural simulation and research.

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

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
