# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document provides a comprehensive guide to using NLM, including installation, configuration, and advanced usage examples. For complete API reference documentation, see **AGENTS.md**.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Building from Source](#building-from-source)
3. [Installing Python Bindings](#installing-python-bindings)
4. [Basic Python Usage](#basic-python-usage)
5. [Complete Agent Example](#complete-agent-example)
6. [Configuration Options](#configuration-options)
7. [Advanced Topics](#advanced-topics)
8. [Troubleshooting](#troubleshooting)
9. [Best Practices](#best-practices)
10. [Further Reading](#further-reading)

## Getting Started

NLM is designed to create artificial brains that learn, remember, and make decisions through interaction with an environment.

### The Three Key Components

| Component | Purpose | Key Methods |
|-----------|---------|-------------|
| **Brain** | The artificial brain with neurons and synapses | `brain.initialize()`, `brain.step()`, `brain.produceAction()` |
| **World** | The environment the brain interacts with | `world.configure()`, `world.update()`, `world.applyMotorCommand()` |
| **Agent** | Bridge connecting brain to world | `agent.processSensoryInput()`, `agent.decodeMotorCommand()` |

### Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

## Building from Source

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

## Installing Python Bindings

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

## Basic Python Usage

### 1. Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### 2. Running a Simulation

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

### 3. Creating a Simple World

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

## Complete Agent Example

This example demonstrates a complete agent simulation with all components working together.

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    print("=== NLM Agent Simulation ===")
    print(f"Running {num_steps} simulation steps")
    print()
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain initialized successfully")
    print()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    print(f"Created world ({world.getWidth()}x{world.getHeight()})")
    print(f"Agent initialized with world")
    print()
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Enabled learning subsystems:")
    print("  - Reward modulation: Enabled")
    print("  - Structural plasticity: Enabled")
    print("  - Development: Enabled")
    print("  - Curiosity: Enabled")
    print()
    
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
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            print()
    
    print("=== Simulation Complete ===")
    print(f"Total steps simulated: {num_steps}")
    print(f"Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## Configuration Options

NLM supports extensive configuration through the `Config` class. The complete parameter reference is available in **AGENTS.md**.

### Common Configuration Parameters

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

### Configuration File Operations

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Save to file
if config.saveToFile("my_config.json"):
    print("Configuration saved successfully")

# Load from file
if config.loadFromFile("my_config.json"):
    print("Configuration loaded successfully")
    
# Modify configuration
config.set("brain.neuron_count", 2000)

# Save updated configuration
config.saveToFile("updated_config.json")

# Type conversion
neuron_count = config.asInt("brain.neuron_count")
threshold = config.asDouble("brain.v_thresh")
enabled = config.asBool("plasticity.stdp.enable")
```

## Advanced Topics

### 1. Custom Configuration

Create specialized configurations for different tasks:

```python
import pynlm

# High-performance configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("brain.tau_mem", 10.0)  # Faster dynamics
config.set("brain.tau_ref", 1.0)   # Higher firing rates

# Enable all learning mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Enhanced neuromodulation
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### 2. Type Conversion Utilities

Safe type conversion with error handling:

```python
import pynlm

# Create configuration with various types
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.v_thresh", -45.0)
config.set("plasticity.stdp.enable", True)
config.set("brain.synapse_indices", [1, 5, 10, 15])
config.set("brain.region_weights", [0.1, 0.5, 1.0, 2.0])

# Type conversion
try:
    neuron_count = config.asInt("brain.neuron_count")
    threshold = config.asDouble("brain.v_thresh")
    enabled = config.asBool("plasticity.stdp.enable")
    indices = config.asIntVector("brain.synapse_indices")
    weights = config.asDoubleVector("brain.region_weights")
    
    print(f"Neuron count: {neuron_count}")
    print(f"Threshold: {threshold}")
    print(f"STDP enabled: {enabled}")
    print(f"Indices: {indices}")
    print(f"Weights: {weights}")
    
except (KeyError, TypeError) as e:
    print(f"Error: {e}")
```

### 3. Batch Operations

Efficient bulk operations:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Batch set multiple configuration values
updates = {
    "brain.neuron_count": 1000,
    "brain.v_thresh": -50.0,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.dopamine.scale": 1.0,
    "neuromod.curiosity.enable": True
}
config.batchSet(updates)

# Batch get multiple configuration values
keys = ["brain.neuron_count", "brain.v_thresh", "plasticity.stdp.enable"]
values = config.batchGet(keys)

print("Batch operations completed")
for key, value in values.items():
    print(f"{key}: {value}")
```

### 4. Brain State Management

Save and load brain state (checkpointing):

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation
for step in range(10):
    brain.step(step)

# Save brain state (checkpointing)
if brain.save("checkpoint.bin"):
    print("Brain state saved successfully")
else:
    print("Failed to save brain state")

# Modify brain state
for step in range(10, 20):
    brain.step(step)

# Save updated brain state
brain.save("updated_checkpoint.bin")

# Load brain state
if brain.load("checkpoint.bin"):
    print("Brain state loaded successfully")
    
# Verify state is restored
for step in range(20):
    brain.step(step)
```

### 5. Validation and Error Handling

Configuration validation and error handling:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Add validation schema
schema = {
    "brain.neuron_count": pynlm.ConfigValidation("neuron_count", "Number of neurons")
        .addRule(lambda x: x >= 100)
        .addRule(lambda x: x <= 100000),
    "brain.v_thresh": pynlm.ConfigValidation("threshold", "Neuron threshold potential")
        .addRule(lambda x: x >= -100)
        .addRule(lambda x: x <= -20)
}
config.addSchema(schema)

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    errors = config.getErrors()
    print(f"Validation errors: {errors}")

# Safe key access
if config.has("brain.neuron_count"):
    neuron_count = config.get("brain.neuron_count")
    print(f"Neuron count: {neuron_count}")
else:
    print("Neuron count not configured")

# Type conversion with error handling
try:
    int_value = config.asInt("brain.neuron_count")
    print(f"Neuron count as int: {int_value}")
except KeyError:
    print("Neuron count key not found")
except TypeError:
    print("Neuron count value cannot be converted to int")
```

## Troubleshooting

### Common Issues and Solutions

#### 1. "Brain isn't doing anything"
**Symptoms**: No neurons firing, no learning
**Causes**:
- Forgot to call `initialize()`
- No sensory input connected
- Configuration parameters are too restrictive

**Solutions**:
```python
# Always initialize first
brain = pynlm.createBrain(config)
brain.initialize()  # Critical!

# Ensure sensory input
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### 2. Segmentation Faults
**Symptoms**: Program crashes unexpectedly
**Causes**:
- Calling `step()` before `initialize()`
- Invalid configuration values
- Memory corruption from invalid operations

**Solutions**:
```python
# Follow proper initialization sequence
brain.initialize()  # Always first
brain.step(0)        # Then step
```

#### 3. Import Error
**Error**: "No module named 'pynlm'"
**Solutions**:
```bash
# Reinstall the module
pip install --force-reinstall .

# Or install in development mode
pip install -e .
```

#### 4. "The agent isn't moving"
**Symptoms**: Agent stays in place
**Causes**:
- Motor command not applied
- `world.update()` not called
- No motor neurons active

**Solutions**:
```python
# Ensure world update is called
world.update(0.1)  # Each simulation step

# Apply motor command
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

#### 5. "Everything is 0"
**Symptoms**: No neural activity
**Causes**:
- Need time for neurons to "warm up"
- Insufficient input to neurons
- Configuration parameters too restrictive

**Solutions**:
```python
# Give neurons time to warm up
for step in range(100):
    brain.step(step)  # Let them settle

# Ensure sensory input is provided
agent.processSensoryInput(percept)
```

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

## Best Practices

### 1. Always Initialize Before Use
```python
brain.initialize()  # Never skip this!
for step in range(100):
    brain.step(step)
```

### 2. Reset Before Reuse
```python
brain.reset()  # Reset for new episode
for step in range(100):
    brain.step(step)
```

### 3. Use Configuration Wisely
```python
# Use defaults for simple cases
config = pynlm.createDefaultConfig()

# Customize only what's necessary
config.set("brain.neuron_count", 2000)  # Custom value
```

### 4. Enable Appropriate Learning Mechanisms
```python
# Enable based on your task
agent.enableRewardModulation(True)      # For reward-based learning
agent.enableCuriosity(True)             # For exploration
agent.enableStructuralPlasticity(True)  # For structural changes
agent.enableDevelopment(True)           # For developmental changes
```

### 5. Monitor Progress
```python
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Active neurons: {brain.getActiveNeuronCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
```

### 6. Use Type Conversion for Safety
```python
# Use type conversion methods for safety
neuron_count = config.asInt("brain.neuron_count")  # Safe to use as int
threshold = config.asDouble("brain.v_thresh")    # Safe to use as double
enabled = config.asBool("plasticity.stdp.enable")  # Safe to use as bool
```

### 7. Batch Operations for Efficiency
```python
# Batch set multiple configuration values
config.batchSet({
    "brain.neuron_count": 1000,
    "brain.v_thresh": -50.0,
    "plasticity.stdp.enable": True
})

# Batch get multiple configuration values
keys = ["brain.neuron_count", "brain.v_thresh", "plasticity.stdp.enable"]
values = config.batchGet(keys)
```

### 8. Save and Load Brain State
```python
# Save brain state (checkpointing)
brain.save("checkpoint.bin")

# Load brain state
brain.load("checkpoint.bin")
```

### 9. Use the Complete API Reference

For comprehensive API documentation including:
- Detailed method signatures and return types
- Parameter descriptions and types
- Complete examples for all classes
- Usage patterns and best practices
- Configuration parameter references

See **AGENTS.md** for the complete API reference.

### 10. Experiment with Different Configurations

```python
# Try different configurations for different tasks
config = pynlm.createDefaultConfig()

# For learning tasks
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# For exploration tasks
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# For developmental tasks
config.set("neuromod.curiosity.enable", True)
```

## Further Reading

- **AGENTS.md** - Complete API reference with all classes and methods
- **easy_usage.md** - Beginner-friendly guide with simple examples
- **README.md** - Project overview and quick start
- **docs/ARCHITECTURE.md** - System architecture overview
- **docs/SCIENCE.md** - Scientific background
- **docs/EXPERIMENTS.md** - Experiment descriptions
- **docs/PHASE6_FINAL_AUDIT.md** - Phase 6 implementation audit

## Key Improvements in This Documentation

This enhanced HOW_TO_USE.md now includes:

1. **Comprehensive table of contents** - Easy navigation to all sections
2. **Complete examples** - Working code for all major features
3. **Advanced topics** - Custom configurations, batch operations, validation
4. **Complete troubleshooting guide** - All common issues with solutions
5. **Best practices section** - Comprehensive guidelines for effective usage
6. **Reference to AGENTS.md** - Clear link to complete API documentation
7. **Better formatting and structure** - Professional documentation layout
8. **Enhanced explanations** - Clearer descriptions of concepts and usage
9. **Consistent style** - Uniform formatting across all sections
10. **Updated examples** - More realistic and complete code examples

## See Also

- **AGENTS.md** - Complete API reference with all classes and methods
- **easy_usage.md** - Beginner-friendly guide with simple examples
- **README.md** - Project overview and quick start
- **docs/ARCHITECTURE.md** - System architecture overview
- **docs/SCIENCE.md** - Scientific background
- **docs/EXPERIMENTS.md** - Experiment descriptions
- **docs/PHASE6_FINAL_AUDIT.md** - Phase 6 implementation audit

## Copyright and License

Copyright © NLM Project Authors. All rights reserved.

MIT License - See LICENSE file for details.