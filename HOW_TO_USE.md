# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20 with Python bindings. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Using the NLM Python API (Python-first approach)
2. Installing the Python library
3. Building the C++ project from source (appendices)

---

## Part 1: Using the NLM Python API

### Quick Start for Python Users

The easiest way to get started is to use the Python bindings directly. NLM provides a complete Python API for creating and simulating artificial developmental brains.

### Basic Python Usage

#### Example 1: Simplest Brain Simulation

```python
import pynlm

# Create a default brain (Phase 6 integrated system)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} total spikes")
print(f"Currently firing: {brain.getFiringNeuronCount()} neurons")
```

#### Example 2: Complete Agent in World Environment

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
    
    # 6. Enable NLM's advanced features for realistic behavior
    agent.enableRewardModulation(True)      # Dopamine-based learning
    agent.enableStructuralPlasticity(True)  # Synaptogenesis and pruning
    agent.enableDevelopment(True)           # Age-dependent plasticity
    agent.enableCuriosity(True)             # Exploration drive
    
    # 7. Run simulation loop
    for step in range(num_steps):
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

### Python API Reference

#### Core Classes

**Brain Class**
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

**Config Class**
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

**AgentBrain Class**
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

**SimpleWorld Class**
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

#### Enumerations

**Neuron types**
```python
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal
```

**Action types**
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest
pynlm.ActionType.Wait
```

**Developmental stages**
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

### Advanced Python Usage

#### Configuration Customization

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify key parameters for specific use cases
config.set("brain.neuron_count", 2000)                    # More neurons for complex behavior
config.set("brain.synapse_density", 0.05)                  # Sparser connections
config.set("plasticity.stdp.enable", True)                # Enable spike-timing learning
config.set("plasticity.hebbian.enable", True)             # Enable Hebbian learning
config.set("plasticity.structural.enable", True)          # Enable structural plasticity
config.set("neuromod.dopamine.scale", 1.0)                # Reward sensitivity
config.set("neuromod.curiosity.enable", True)             # Exploration drive

# Create and initialize brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Complex Simulation with Multiple Brains

```python
import pynlm

# Create multiple interconnected brains
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 500)
brain1 = pynlm.createBrain(config1)

config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 300)
brain2 = pynlm.createBrain(config2)

# Initialize both brains
brain1.initialize()
brain2.initialize()

# Create agents for both brains
agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createBrain(brain2)  # Wrong! This should be createAgentBrain
# Correction:
agent2 = pynlm.createAgentBrain(brain2)

# Create shared world
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=12, visionHeight=12)

# Initialize agents with world
agent1.initialize(world)
agent2.initialize(world)

# Run simulation with inter-brain communication
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input from world
    percept1 = world.getSensoryPercept()
    agent1.processSensoryInput(percept1)
    
    # Brain1 produces action
    action1 = agent1.decodeMotorCommand()
    world.applyMotorCommand(action1, world.getSimulationTime())
    
    # Brain2 produces action (interacts with Brain1's actions in world)
    percept2 = world.getSensoryPercept()
    agent2.processSensoryInput(percept2)
    
    action2 = agent2.decodeMotorCommand()
    world.applyMotorCommand(action2, world.getSimulationTime())
    
    # Update both brains
    brain1.step(step)
    brain2.step(step)
```

#### Checkpoint and State Management

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for a while
for step in range(500):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}")

# Save brain state (checkpointing)
brain.save("my_brain_checkpoint.bin")
print("Brain state saved to checkpoint")

# Continue simulation after loading
for step in range(500, 1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}")

# Load saved state (resume from checkpoint)
brain.load("my_brain_checkpoint.bin")
print("Brain state loaded from checkpoint")

# Continue from saved state
for step in range(1000, 1500):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step} (after load)")
```

#### Performance Monitoring and Statistics

```python
import pynlm
import time

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
start_time = time.time()
total_steps = 10000

for step in range(total_steps):
    brain.step(step)
    
    # Monitor every 1000 steps
    if step % 1000 == 0 and step > 0:
        elapsed = time.time() - start_time
        steps_per_second = step / elapsed
        
        print(f"Performance:")
        print(f"  Steps per second: {steps_per_second:.0f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  E/I balance: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print()

elapsed = time.time() - start_time
print(f"Simulation complete: {total_steps} steps in {elapsed:.2f}s")
print(f"Average performance: {total_steps/elapsed:.0f} steps/second")
```

---

## Part 2: Installing the Python Library

### Prerequisites

- **Python**: 3.8 or higher
- **pip**: Python package installer

### Installation Methods

#### Method 1: Install from Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install NLM Python bindings
pip install .

# Or install in development mode (editable install)
pip install -e .
```

#### Method 2: Modern Python Packaging with pyproject.toml

```bash
# Ensure build tools are installed
pip install build

# Build distribution packages
python -m build

# Install from built package
pip install dist/*.whl
```

#### Method 3: Using Pre-built Wheels (if available)

```bash
# Check for pre-built wheels
pip search pynlm

# Install from PyPI (if released)
pip install pynlm
```

### Installation Verification

After installation, verify that NLM Python bindings work correctly:

```python
import pynlm

# Check version
print(f"NLM Python bindings version: {pynlm.__version__}")

# Create default configuration
config = pynlm.createDefaultConfig()
print("Default configuration created successfully")

# Create brain with configuration
brain = pynlm.createBrain(config)
print(f"Brain created with {brain.getTotalNeuronCount()} neurons")

# Initialize brain
brain.initialize()
print("Brain initialized successfully")

print("\n✓ NLM Python bindings installation verified!")
```

---

## Appendices

### Appendix A: Building the C++ Project from Source

**Note**: The C++ core is required for the Python bindings. If using pre-built wheels, this appendix may not be needed.

#### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

#### Build Steps

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

#### Build Outputs

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

#### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Appendix B: Advanced C++ Usage

For advanced users who need direct access to the C++ API or want to customize the brain components, the NLM framework provides extensive C++ classes and functions.

#### Key C++ Classes

- **Brain**: Main neural simulation class
- **Neuron**: LIF neuron implementation
- **Synapse**: Synaptic connection with plasticity
- **Config**: Configuration management
- **SpikeSystem**: Event-driven spike propagation
- **STDP/Hebbian**: Plasticity mechanisms
- **DevelopmentSystem**: Age-dependent changes
- **Neuromodulator**: Dopamine, curiosity, novelty systems
- **Memory systems**: Working memory, episodic memory, etc.

#### Example C++ Usage

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 1000);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize
    if (!brain->initialize()) {
        return 1;
    }
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    return 0;
}
```

### Appendix C: Troubleshooting

#### Common Installation Issues

**"ImportError: No module named 'pynlm'"**
- Solution: Ensure installation completed successfully. Try `pip install --force-reinstall .`

**"ImportError: libnlm_core.so: cannot open shared object file"**
- Solution: Ensure the C++ library was built and Python bindings were compiled
- On Linux: `export LD_LIBRARY_PATH=/path/to/nlm/build:$LD_LIBRARY_PATH`
- On macOS: `export DYLD_LIBRARY_PATH=/path/to/nlm/build:$DYLD_LIBRARY_PATH`

**"CMake cannot find Python"**
- Solution: Install Python development packages and ensure Python is in PATH
- On Ubuntu: `sudo apt-get install python3-dev python3-pip`

#### Common Runtime Issues

**"Segmentation fault on brain initialization"**
- Solution: Check that `initialize()` is called before any other operations
- Ensure configuration is properly set up
- Verify all required components are initialized

**"Brain produces no spikes or activity"**
- Solution: Check that neurons have appropriate parameters
- Ensure sufficient input is being injected
- Verify development stage is appropriate for activity

**"Simulation too slow or hangs"**
- Solution: Reduce neuron count for testing
- Check for infinite loops in custom code
- Monitor memory usage

#### Getting Help

- **Documentation**: Read `README.md`, `HOW_TO_USE.md`, and `easy_usage.md`
- **Examples**: Check the demo executables (`./nlm_phase6_demo`, `./nlm_phase3_demo`)
- **Community**: See project documentation for support channels

---

## Further Reading

- [README.md](README.md) - Project overview and Phase 6 integration achievements
- [easy_usage.md](easy_usage.md) - Beginner-friendly usage guide
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background and principles

---

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
