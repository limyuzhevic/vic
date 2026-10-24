# NLM (Neural Learning Machine) - HOW TO USE (Enhanced)

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the enhanced Python library
3. Using the enhanced NLM Python API

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)
- **Modern Python environment**: For enhanced bindings

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

## Part 2: Installing the Enhanced Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install enhanced bindings
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

### Verifying Enhanced Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain with standard configuration
config = pynlm.createStandardConfig("standard")
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")

# Test enhanced features
pynlm.register_nlm_exceptions()
print("Enhanced features loaded successfully!")
```

---

## Part 3: Using the Enhanced Python Library

### Enhanced Basic Usage

#### Creating a Brain with Enhanced Configuration

```python
import pynlm

# Create a standard configuration with preset parameters
config = pynlm.createStandardConfig("standard")

# Optionally configure specific parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running an Enhanced Simulation

```python
import pynlm

# Create brain
config = pynlm.createStandardConfig("standard")
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with enhanced error handling
pynlm.register_nlm_exceptions()

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

### Enhanced Complete Agent Example

```python
import pynlm

def run_enhanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced brain and world."""
    
    # 1. Create configuration with enhanced settings
    config = pynlm.createStandardConfig("advanced")
    
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
    
    # 6. Enable ALL subsystems with enhanced features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # 7. Register enhanced error handling
    pynlm.register_nlm_exceptions()
    
    # 8. Run simulation loop
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
        reward = 0.0
        if percept.getInternal() and not percept.getInternal().empty():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the enhanced simulation
brain, agent, world = run_enhanced_agent_simulation(1000)
```

### Key Classes and Methods

#### Brain Class (Enhanced)

```python
# Creation
config = pynlm.createStandardConfig("standard")
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Enhanced simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Enhanced input/output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Enhanced statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()       # Total spike count
brain.getDevelopmentalStage()   # Current developmental stage
brain.getConfig()                # Get the configuration

# State management
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class (Enhanced)

```python
# Enhanced configuration creation and usage

# Create
config = pynlm.createStandardConfig("advanced")

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 2000)
config.set("learning.rate", 0.001)

# Enhanced file I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Configuration templates
minimal_config = pynlm.createStandardConfig("minimal")
standard_config = pynlm.createStandardConfig("standard")
advanced_config = pynlm.createStandardConfig("advanced")
```

#### AgentBrain Class (Enhanced)

```python
# Enhanced agent brain interface

# Creation
agent = pynlm.createAgentBrain(brain)

# Enhanced initialization
agent.initialize(world)

# Enhanced sensory processing
agent.processSensoryInput(percept)

# Enhanced motor decoding
motor_cmd = agent.decodeMotorCommand()

# Enhanced neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Enhanced development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Enhanced subsystem control
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Enhanced statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class (Enhanced)

```python
# Enhanced world simulation

# Creation
world = pynlm.createSimpleWorld()

# Enhanced configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Enhanced control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Enhanced agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# Enhanced state access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# Enhanced world objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Enhanced properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enhanced Example Scripts

#### Minimal Enhanced Example

```python
import pynlm

# Create and initialize with enhanced config
config = pynlm.createStandardConfig("minimal")
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Enhanced minimal simulation complete!")
```

#### Enhanced Environment Interaction

```python
import pynlm

# Setup with enhanced configuration and error handling
config = pynlm.createStandardConfig("standard")
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Enable enhanced error handling
pynlm.register_nlm_exceptions()

# Run enhanced episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Part 4: Enhanced Configuration Options

### Standard Configuration Templates

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Configuration Types

#### Minimal Configuration

```python
import pynlm

config = pynlm.createStandardConfig("minimal")
# Settings:
# - brain.neuron_count: 100
# - brain.synapse_density: 0.05
```

#### Standard Configuration

```python
import pynlm

config = pynlm.createStandardConfig("standard")
# Settings:
# - brain.neuron_count: 1000
# - brain.synapse_density: 0.1
# - plasticity.stdp.enable: True
# - plasticity.hebbian.enable: True
# - neuromod.dopamine.scale: 1.0
# - neuromod.curiosity.enable: True
```

#### Advanced Configuration

```python
import pynlm

config = pynlm.createStandardConfig("advanced")
# Settings:
# - brain.neuron_count: 5000
# - brain.synapse_density: 0.15
# - plasticity.stdp.enable: True
# - plasticity.stdp.learning_rate: 0.001
# - plasticity.hebbian.enable: True
# - plasticity.structural.enable: True
# - neuromod.dopamine.scale: 1.5
# - neuromod.curiosity.enable: True
# - neuromod.novelty.enable: True
# - brain.development.enabled: True
```

---

## Enhanced Example Scripts

### Enhanced Minimal Example

```python
import pynlm

# Create and initialize with enhanced config
config = pynlm.createStandardConfig("minimal")
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Enhanced minimal simulation complete!")
```

### Enhanced Environment Interaction

```python
import pynlm

# Setup with enhanced configuration and error handling
config = pynlm.createStandardConfig("standard")
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Enable enhanced error handling
pynlm.register_nlm_exceptions()

# Run enhanced episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Enhanced Troubleshooting

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
- Use `pynlm.register_nlm_exceptions()` for better error reporting

**Performance issues:**
- Use `createStandardConfig("minimal")` for faster testing
- Reduce simulation steps for debugging
- Monitor memory usage with larger configurations

### Enhanced Error Handling

**Custom exceptions:**
```python
import pynlm

pynlm.register_nlm_exceptions()

try:
    config = pynlm.createStandardConfig("advanced")
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except pynlm.ConfigurationError as e:
    print(f"Configuration problem: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation problem: {e}")
except pynlm.ResourceError as e:
    print(f"Resource problem: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

---

## Enhanced Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [python/ENHANCED_API_DOCS.md](python/ENHANCED_API_DOCS.md) - Complete enhanced API documentation
- [NLM_IMPROVEMENT_PLAN.md](NLM_IMPROVEMENT_PLAN.md) - Technical improvement plan

## Key Improvements Summary

1. **Enhanced Error Handling**: Custom exception hierarchy with detailed error messages
2. **Standard Configuration Templates**: `createStandardConfig()` with three preset configurations
3. **Convenience Functions**: `runCompleteAgentSimulation()` for easy simulation setup
4. **Enhanced Documentation**: Comprehensive examples and API reference
5. **Better Integration**: Improved error reporting and exception handling
6. **Resource Management**: Better support for advanced Python features
7. **Performance Optimizations**: Pre-configured settings for different use cases

## Conclusion

The enhanced NLM Python library provides a more intuitive, powerful, and user-friendly API while maintaining full backward compatibility. The improvements include:

- **Better error handling** with custom exception types
- **Pre-configured settings** for different use cases
- **Convenience functions** for common operations
- **Enhanced documentation** with comprehensive examples
- **Improved performance** through optimized configurations

These enhancements make the NLM framework more accessible to Python developers while providing the advanced features needed by experienced users.

Happy coding with the enhanced NLM! 🚀

---

**Enhanced Features Added:**
- Custom exception hierarchy (NLMError, ConfigurationError, SimulationError, ResourceError)
- Standard configuration templates (minimal, standard, advanced)
- Convenience function for complete agent simulations
- Enhanced error handling registration
- Comprehensive documentation with examples
- Better Pythonic API design
- Performance optimization presets
- Advanced error reporting

**Files Modified:**
- `python/bindings.cpp` - Enhanced Python bindings with new features
- `python/ENHANCED_API_DOCS.md` - Complete API documentation
- `easy_usage.md` - Enhanced beginner guide
- `HOW_TO_USE.md` - Enhanced advanced user guide
- `NLM_IMPROVEMENT_PLAN.md` - Technical improvement plan