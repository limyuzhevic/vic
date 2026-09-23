# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the enhanced NLM Python API with modern Python features

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

# Check module metadata
print(pynlm.__version__)       # "0.1.0"
print(pynlm.__author__)        # "NLM Authors"
print(pynlm.__license__)       # "MIT"
print(pynlm.__description__)   # "Python bindings for NLM..."

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.total_neurons}")      # Pythonic property
print(f"Synapses: {brain.total_synapses}")    # Pythonic property
```

---

## Part 3: Using the Enhanced Python Library

### Modern Python Features

The enhanced NLM Python API now includes modern Python features:

#### 1. Module Metadata

```python
import pynlm

# Access module information
print(pynlm.__version__)       # "0.1.0"
print(pynlm.__author__)        # "NLM Authors"
print(pynlm.__license__)       # "MIT"
print(pynlm.__description__)   # "Python bindings for NLM..."
```

#### 2. Custom Exception Hierarchy

```python
import pynlm

# New custom exception types
brain = pynlm.createBrain(config)

# Example: Catch specific NLM errors
try:
    config.loadFromFile("config.json")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
```

#### 3. Context Managers

```python
# Automatic resource management and cleanup
with brain:
    for i in range(1000):
        brain.step(i)
```

#### 4. Pythonic Properties

```python
# Direct property access (replaces verbose getter methods)
brain.total_neurons       # Instead of brain.getTotalNeuronCount()
brain.total_synapses      # Instead of brain.getTotalSynapseCount()
brain.firing_neurons      # Instead of brain.getFiringNeuronCount()
brain.average_firing_rate  # Instead of brain.getAverageFiringRate()

config.neuron_count       # Instead of config.set("brain.neuron_count")
config.synapse_density    # Instead of verbose config access
```

#### 5. Convenience Functions

```python
# Quick setup and helpers
agent = pynlm.setup_agent_simulation(config, world)

# Statistical helpers
stats = pynlm.brain_stats(brain)
print(f"Total neurons: {stats['neurons']}")

# Enable all features
pynlm.enable_all_features(agent)

# Complete simulation
pynlm.run_episode(brain, agent, world, 500)

# Safe operations
success = pynlm.safe_brain_operation(brain, lambda b: b.step(0))
```

### Enhanced Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters using Pythonic properties
# config.neuron_count = 1000
# config.synapse_density = 0.1
# config.learning_rate = 0.001

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.total_neurons}")      # Pythonic property
print(f"Total synapses: {brain.total_synapses}")    # Pythonic property
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
        print(f"Step {step}: {brain.firing_neurons} firing neurons")  # Pythonic property
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

### Enhanced Agent Example with Modern Features

```python
import pynlm

def run_enhanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with enhanced Python features."""
    
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
    
    # 6. Enable all features using convenience function
    pynlm.enable_all_features(agent)
    
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
        
        # Print enhanced progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.firing_neurons}")
            print(f"  Average firing rate: {brain.average_firing_rate:.2f}")
            
            # Use convenience function for comprehensive agent state
            agent_state = pynlm.get_agent_state(agent)
            print(f"  Curiosity level: {agent_state['curiosity_level']:.3f}")
            print(f"  Novelty level: {agent_state['novelty_level']:.3f}")
            print(f"  Prediction error: {agent_state['prediction_error']:.3f}")
            print(f"  Development stage: {agent_state['developmental_stage']}")

# Run the simulation
run_enhanced_agent_simulation(1000)
```

### Key Classes and Methods (Enhanced)

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

# Statistics (using Pythonic properties)
brain.total_neurons       # Total neurons
brain.total_synapses      # Total synapses
brain.firing_neurons      # Currently firing
brain.average_firing_rate  # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")      # Save state
brain.load("checkpoint.bin")      # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query using Pythonic properties
print(config.neuron_count)      # Direct property access
print(config.synapse_density)   # Direct property access

# Modify using Pythonic properties
config.neuron_count = 1000
config.synapse_density = 0.1

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

# Statistics using convenience function
state = pynlm.get_agent_state(agent)
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

### Enhanced Convenience Functions

#### Quick Setup Helper

```python
# One-liner for complete agent setup
agent = pynlm.setup_agent_simulation(config, world)
```

#### Statistical Helper

```python
# Get comprehensive brain statistics
stats = pynlm.brain_stats(brain)
print(f"Neurons: {stats['neurons']}")
print(f"Synapses: {stats['synapses']}")
print(f"Firing neurons: {stats['firing_neurons']}")
```

#### World Helper

```python
# Create configured world quickly
world = pynlm.create_world_with_objects(20, 20, 8, 8)
```

#### Complete Simulation

```python
# Run complete episode with all features
pynlm.run_episode(brain, agent, world, 500, 0.1)
```

#### Error Handling Helper

```python
# Safe operation execution
success = pynlm.safe_brain_operation(brain, lambda b: b.step(0))
```

### Modern Usage Examples

#### Complete Enhanced Agent Example

```python
import pynlm

class SmartAgent:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Quick setup
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable all features for advanced learning
        pynlm.enable_all_features(self.agent)
    
    def run_simulation(self, steps=1000):
        print(f"Starting agent simulation with {steps} steps")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(step)
            
            # Get action from brain
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            reward = self._get_reward(percept, action)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                stats = pynlm.brain_stats(self.brain)
                agent_state = pynlm.get_agent_state(self.agent)
                print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
                      f"curiosity: {agent_state['curiosity_level']:.2f}")
    
    def _get_reward(self, percept, action):
        # Example reward function
        return percept.getInternal()[0] if percept.getInternal() else 0.0

def advanced_simulation_example():
    """Example of advanced simulation using all features"""
    print("=== Advanced NLM Simulation Example ===")
    
    agent = SmartAgent()
    agent.run_simulation(1000)
    
    print("=== Simulation Complete ===")
    print(f"Final brain stats: {pynlm.brain_stats(agent.brain)}")
    print(f"Final agent state: {pynlm.get_agent_state(agent.agent)}")

# Run the example
advanced_simulation_example()
```

#### Using Pythonic Properties

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use Pythonic properties instead of verbose getter methods
print(f"Total neurons: {brain.total_neurons}")
print(f"Firing neurons: {brain.firing_neurons}")
print(f"Average firing rate: {brain.average_firing_rate}")

# Context manager for automatic cleanup
with brain:
    for i in range(1000):
        brain.step(i)
        if i % 100 == 0:
            print(f"Progress: {i} steps")

# Property-based configuration
config.neuron_count = 1000
config.synapse_density = 0.1
config.learning_rate = 0.001
```

#### Error Handling with Custom Exceptions

```python
import pynlm

def robust_simulation():
    try:
        # Create and configure
        config = pynlm.createDefaultConfig()
        config.neuron_count = 100
        
        # Use Pythonic property access
        if not config.neuron_count:
            raise pynlm.ConfigurationError("Required neuron_count not found")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Safe operation
        success = pynlm.safe_brain_operation(brain, lambda b: b.step(0))
        
        return success
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration problem: {e}")
        return False
    except pynlm.SimulationError as e:
        print(f"Simulation failed: {e}")
        return False
    except pynlm.ValidationError as e:
        print(f"Validation error: {e}")
        return False
    except pynlm.NLMException as e:
        print(f"NLM error: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False

# Usage
robust_simulation()
```

---

## Enhanced Configuration Options

### Modern Configuration (Updated)

#### Brain Configuration

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

#### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Enhanced Configuration (Modern)

```python
# Modern Pythonic configuration
config = pynlm.createDefaultConfig()

# Property-based configuration (replaces set() calls)
config.neuron_count = 1000
config.synapse_density = 0.1
config.learning_rate = 0.001

# Property-based access (replaces get() calls)
print(f"Neurons: {config.neuron_count}")
print(f"Synapse density: {config.synapse_density}")

# File I/O (unchanged)
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

---

## Enhanced Example Scripts

### Minimal Example (Modern)

```python
import pynlm

# Create and initialize using Pythonic syntax
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction (Enhanced)

```python
import pynlm

# Setup using enhanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all features
pynlm.enable_all_features(agent)

# Run episode using convenience function
pynlm.run_episode(brain, agent, world, 500)
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

### Modern Error Handling

**Custom Exception Examples:**
```python
try:
    config = pynlm.createDefaultConfig()
    config.loadFromFile("config.json")
except pynlm.ConfigurationError as e:
    print(f"Fix configuration: {e}")
except pynlm.SimulationError as e:
    print(f"Fix simulation: {e}")
except pynlm.ValidationError as e:
    print(f"Fix validation: {e}")
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory issue: {e}")
```

**Safe Operations:**
```python
# Check if operation succeeds
success = pynlm.safe_brain_operation(brain, lambda b: b.step(0))
if success:
    print("Operation completed successfully")
else:
    print("Operation failed - safer than try/catch")
```

### Enhanced Runtime Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try using `pynlm.brain_stats(brain)` to check current state
- Verify Pythonic properties: `brain.total_neurons`, `brain.firing_neurons`

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Use `pynlm.enable_all_features(agent)` to enable all features
- Verify agent state: `pynlm.get_agent_state(agent)`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Use `pynlm.brain_stats(brain)` to check progress
- Some neurons need input to fire - make sure sensory input is connected

### Performance Tips (Modern)

1. **Use Pythonic properties**: `brain.total_neurons` instead of `brain.getTotalNeuronCount()`
2. **Leverage convenience functions**: `pynlm.setup_agent_simulation()`, `pynlm.run_episode()`
3. **Monitor statistics**: Use `pynlm.brain_stats()` for performance tracking
4. **Safe operations**: Use `pynlm.safe_brain_operation()` for robust error handling
5. **Context managers**: Use `with brain:` for automatic resource management

---

## Modern Python Features

The enhanced NLM Python API now provides:

1. **Modern Python features**: Properties, context managers, custom exceptions
2. **Convenience functions**: Simplified common patterns
3. **Better error handling**: Custom exception hierarchy with specific error types
4. **Performance improvements**: Batch operations and efficient helpers
5. **Backward compatibility**: Existing code continues to work
6. **Enhanced documentation**: Comprehensive examples and migration guides
7. **Property-based API**: More intuitive, Pythonic interface
8. **Automatic resource management**: Context managers for safety

## Migration Guide

### From Old to New API

**Old API (verbose):**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(brain.getTotalNeuronCount())        # Old way
print(brain.getFiringNeuronCount())       # Old way
```

**New Enhanced API (Pythonic):**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(brain.total_neurons)                # New Pythonic property
print(brain.firing_neurons)               # New Pythonic property

# Using convenience functions
stats = pynlm.brain_stats(brain)
print(f"Brain has {stats['neurons']} neurons")
```

## Conclusion

The enhanced NLM Python API now provides:

1. **Modern Python features**: Properties, context managers, custom exceptions
2. **Convenience functions**: Simplified common patterns
3. **Better error handling**: Custom exception hierarchy
4. **Performance improvements**: Batch operations and efficient helpers
5. **Backward compatibility**: Existing code continues to work
6. **Enhanced documentation**: Comprehensive examples and migration guides
7. **Property-based API**: More intuitive, Pythonic interface
8. **Automatic resource management**: Context managers for safety

These improvements make NLM more accessible to Python developers while maintaining the power of the underlying C++ neural simulation framework.