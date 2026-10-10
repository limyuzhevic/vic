# NLM (Neural Learning Machine) - IMPROVED HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

**This document covers the IMPROVED NLM Python API with enhanced features and user-friendly interface.**

This document covers:
1. Building the C++ project from source
2. Installing the improved Python library
3. Using the **IMPROVED** NLM Python API

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

## Part 2: Installing the Improved Python Library

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

# Test improved API
brain.enable_stats_collection()
print("Improved API test successful!")
```

---

## Part 3: Using the IMPROVED NLM Python API

### Enhanced Basic Usage

#### Creating and Initializing a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.enable_stats_collection()
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation with Enhanced Features

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.enable_stats_collection()

# Initialize brain
brain.initialize()

# Run simulation for 1000 steps with enhanced monitoring
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Get final statistics
print(f"Simulation complete!")
print(f"Statistics: {brain.get_stats().get_summary()}")
```

#### Creating an Enhanced World

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

# Run world update with enhanced status
status = world.update(timestep=0.1)
print(f"World status: {status}")
```

### Advanced Agent Example with Enhanced Features

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced features."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize with monitoring
    brain = pynlm.createBrain(config)
    brain.enable_stats_collection()
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
    
    # 6. Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop with monitoring
    for step in range(num_steps):
        # Update world
        world_status = world.update(timestep=0.1)
        
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
        reward = world_status.get('agent_energy', 0.0) / 10.0 if world_status.get('agent_energy') else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress with enhanced information
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Dev stage: {brain.getDevelopmentalStage()}")
    
    # 8. Return comprehensive results
    return {
        'brain_status': brain.get_status(),
        'world_status': world.get_agent_status(),
        'agent_status': {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'prediction_error': agent.getPredictionError(),
            'developmental_stage': agent.getDevelopmentalStage(),
        }
    }

# Run the enhanced simulation
results = run_agent_simulation(1000)
print(f"Enhanced agent simulation complete!")
```

### Key Classes and Methods (Enhanced API)

#### Enhanced Brain Class

```python
# Creation
brain = pynlm.createBrain(config)
brain.enable_stats_collection()  # Enable monitoring

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)                    # Run one simulation step
brain.step(step_number, time)               # Run step with timestamp
brain.reset()                               # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)    # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()                 # Total neurons
brain.getTotalSynapseCount()                # Total synapses
brain.getFiringNeuronCount()                # Currently firing
brain.getAverageFiringRate()                # Average firing rate
brain.getExcitationInhibitionRatio()       # E/I balance
brain.get_stats().get_summary()             # Enhanced statistics
brain.get_developmental_stage_info()        # Development stage info

# State
brain.save("checkpoint.bin")                # Save state
brain.load("checkpoint.bin")                # Load state
brain.get_status()                          # Get comprehensive status
```

#### Enhanced Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Enhanced configuration with validation
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("simulation_timestep", 0.001)

# Query and validation
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    print(f"Neurons: {neurons}")

# File I/O with error handling
try:
    config.loadFromFile("config.json")
    config.saveToFile("config.json")
except Exception as e:
    print(f"Config error: {e}")
```

#### Enhanced AgentBrain Class

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
agent.get_developmental_stage_info()        # Enhanced development info

# Enhanced subsystem enable/disable
agent.enableRewardModulation(True/False)    # With validation
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Enhanced statistics
agent.getCuriosityLevel()                   # Get curiosity level
agent.getNoveltyLevel()                     # Get novelty level
agent.getPredictionError()                  # Get prediction error
agent.getNeuromodulationLevel()             # Get neuromodulation level
agent.get_status()                          # Get comprehensive status
```

#### Enhanced SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Enhanced configuration with validation
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Enhanced control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Enhanced agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# Enhanced state access
percept = world.getSensoryPercept()
world_status = world.get_agent_status()    # Enhanced status
body = world.getAgentBody()

# Enhanced world objects with validation
world.add_object(x, y, obj_type, value=1.0, radius=0.5)
world.removeObject(x, y)
world.isValidPosition(x, y)

# Enhanced properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enhanced Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Enhanced developmental stages
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

# Enhanced world object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Enhanced Configuration Options

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
| `simulation_timestep` | float | 0.001 | Simulation timestep (s) |

### Enhanced Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Enhanced Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.enable` | bool | true | Enable prediction error signals |

---

## Part 5: Enhanced Example Scripts

### Minimal Enhanced Example

```python
import pynlm

# Create and initialize with monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.enable_stats_collection()
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Enhanced simulation complete!")
print(f"Statistics: {brain.get_stats().get_summary()}")
```

### Enhanced Environment Interaction

```python
import pynlm

# Setup with enhanced monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.enable_stats_collection()
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode with enhanced monitoring
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

print(f"Enhanced episode complete!")
print(f"Brain status: {brain.get_status()}")
print(f"World status: {world.get_agent_status()}")
```

---

## Enhanced Troubleshooting

### Build Issues

**"CMake cannot find Python:"**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**"Compilation errors:"**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**`ImportError: No module named 'pynlm'`:**
```bash
pip install --force-reinstall .
```

**"Segmentation faults:"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify configuration is valid before creating brain

### Enhanced API Issues

**"My brain isn't doing anything":**
- Did you call `brain.initialize()`?
- Try enabling stats collection: `brain.enable_stats_collection()`
- Check developmental stage: `brain.get_developmental_stage_info()`

**"The agent isn't moving":**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify agent's motor capabilities: `agent.get_motor_output_size()`

**"Everything is 0":**
- Brains need time to "warm up" - try more steps
- Check statistics: `brain.get_stats().get_summary()`
- Some neurons need input to fire - make sure sensory input is connected

**"Enhanced API not working":**
- Ensure you're using the latest pynlm module
- Try importing the enhanced module: `import pynlm_improved as pynlm`
- Check version compatibility

---

## Enhanced Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Enhanced experiment descriptions
- [enhanced_docs/README_ENHANCED.md](enhanced_docs/README_ENHANCED.md) - Enhanced API documentation
- [enhanced_docs/ADVANCED_FEATURES.md](enhanced_docs/ADVANCED_FEATURES.md) - Advanced features guide