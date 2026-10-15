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

### Advanced Python API Usage

#### Enhanced Configuration with Validation

```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()

# Safely set configuration values with type inference
config.set_safe("brain.neuron_count", 2000)
config.set_safe("brain.connection_probability", 0.15)
config.set_safe("simulation.timestep", 0.001)

# Validate configuration
is_valid, error_msg = config.validate_config()
if is_valid:
    print("✓ Configuration is valid")
else:
    print(f"✗ Configuration error: {error_msg}")

# Create a preset configuration
config.create_preset("development")

# Export configuration for debugging
config_dict = config.export_config()
print(f"Configuration keys: {len(config_dict)}")
```

#### Enhanced Brain Initialization and Error Handling

```python
import pynlm

# Create brain with enhanced safety
config = pynlm.createDefaultConfig()
config.set_safe("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)

# Initialize with validation and error handling
try:
    if brain.initialize_safe():
        print("✓ Brain initialized successfully")
        print(f"  Created {brain.getTotalNeuronCount()} neurons")
    else:
        print("✗ Brain initialization failed")
except Exception as e:
    print(f"✗ Brain initialization error: {e}")
```

#### Safe Simulation with Error Handling

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# Run simulation with error handling
print("Running simulation with error handling...")
for step in range(100):
    try:
        if not brain.step_safe(step):
            print(f"Simulation stopped at step {step}")
            break
    except Exception as e:
        print(f"Simulation error at step {step}: {e}")
        break

print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
```

#### Comprehensive Brain Analysis

```python
import pynlm
from nlm_introspection import BrainIntrospection, create_introspection

# Create brain and analysis tool
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# Create introspection tool
introspection = BrainIntrospection(brain)

# Start comprehensive analysis
print("Starting brain analysis...")
analysis = introspection.start_analysis()

if "error" in analysis:
    print(f"Analysis failed: {analysis['error']}")
else:
    print("✓ Analysis completed successfully")
    print(f"  Brain state: {analysis['brain_state']}")
    print(f"  Neuron count: {analysis['basic_info']['neuron_count']}")
    print(f"  Synapse count: {analysis['basic_info']['synapse_count']}")
    print(f"  System health: {analysis['system_health']['overall_status']}")
    print(f"  Performance: {analysis['performance_metrics']['summary']['throughput']:.2f} steps/sec")
    
    # Get detailed analysis
    detailed_analysis = introspection.get_detailed_analysis("comprehensive")
    print(f"  Neural dynamics: {detailed_analysis['neural_dynamics']['activity_level']:.2f}")
```

#### Serialization and Checkpoint Management

```python
import pynlm
from nlm_serialization import (
    create_checkpoint_manager, 
    SerializationFormat, 
    CompressionLevel
)

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# Create checkpoint manager
checkpoint_manager = create_checkpoint_manager()

# Save checkpoint with advanced options
checkpoint_id = checkpoint_manager.save_checkpoint(
    brain=brain,
    name="experiment_1",
    format=SerializationFormat.HYBRID,
    tags=["experiment", "demo", "test"],
    metadata={
        "experiment_name": "Demo Experiment",
        "description": "A demonstration of NLM serialization",
        "researcher": "Dr. Smith",
    },
    description="Advanced checkpoint with validation and compression",
    include_stats=True,
    compression=True
)

print(f"✓ Checkpoint saved with ID: {checkpoint_id}")

# List checkpoints
checkpoints = checkpoint_manager.list_checkpoints()
print(f"Available checkpoints: {len(checkpoints)}")
for checkpoint in checkpoints:
    print(f"  - {checkpoint['name']}: {checkpoint['neuron_count']} neurons")

# Load checkpoint
loaded_brain = checkpoint_manager.load_checkpoint("experiment_1")
print(f"✓ Loaded brain with {loaded_brain.getTotalNeuronCount()} neurons")

# Export to different format
export_path = checkpoint_manager.export_checkpoint(
    "experiment_1",
    "export_example.json",
    SerializationFormat.JSON
)
print(f"✓ Exported to JSON: {export_path}")
```

#### Command Line Interface

```python
# Run the NLM CLI
# Open terminal in the NLM directory and run:
# python nlm_cli.py --help

# Example commands:
# Create a brain
python nlm_cli.py create --component=brain --neuron-count=1000

# Run a simulation
python nlm_cli.py simulate --steps=1000 --enable-reward-modulation

# Explore brain state
python nlm_cli.py explore

# Create a configuration
python nlm_cli.py config create --filename=my_config.json --neuron-count=2000

# Run a demonstration
python nlm_cli.py demo basic --steps=500
```

#### Performance Monitoring and Real-time Analysis

```python
import pynlm
from nlm_introspection import create_introspection

# Create brain and monitoring tool
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# Create introspection tool
introspection = create_introspection(brain)

# Start real-time monitoring
print("Starting real-time monitoring...")
introspection.start_real_time_monitoring(interval=0.5)

# Run simulation while monitoring
print("Running simulation with monitoring...")
for step in range(50):
    brain.step_safe(step)
    
    if (step + 1) % 10 == 0:
        performance = introspection.performance.get_performance_report()
        print(f"  Step {step + 1}: {performance['summary']['throughput']:.1f} steps/sec, "
              f"Avg time: {performance['summary']['average_step_time']:.3f}s")

# Get comprehensive performance report
performance = introspection.performance.get_performance_report()
print(f"\nPerformance Summary:")
print(f"  Total steps: {performance['summary']['total_steps']}")
print(f"  Total time: {performance['summary']['total_time']:.2f}s")
print(f"  Throughput: {performance['summary']['throughput']:.2f} steps/sec")
print(f"  Average step time: {performance['summary']['average_step_time']:.3f}s")
print(f"  Uptime: {performance['summary']['uptime']:.1f}s")
print(f"  Peak memory: {performance['resource_usage']['peak_memory_mb']:.1f} MB")
```

#### Logging Configuration and Management

```python
import pynlm
from nlm_enhanced_bindings import configure_logging

# Configure logging for detailed output
logging_config = {
    "level": "DEBUG",
    "console_output": True,
    "file": "nlm_debug.log",
}

configure_logging(logging_config)

# Create brain and set up logging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize with logging
print("Initializing brain with logging...")
if brain.initialize_safe():
    print("✓ Brain initialized successfully")
else:
    print("✗ Brain initialization failed")

# Perform operations that generate log messages
brain.step_safe(0)
brain.step_safe(1)
```

#### Advanced Error Handling and Recovery

```python
import pynlm
from nlm_enhanced_bindings import NLMError, BrainError

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Attempt initialization with comprehensive error handling
try:
    print("Attempting brain initialization...")
    if brain.initialize_safe():
        print("✓ Brain initialized successfully")
    else:
        print("✗ Brain initialization failed")
        
except BrainError as e:
    print(f"✗ Brain error: {e}")
    # Attempt recovery
    try:
        print("Attempting brain reset and re-initialization...")
        brain.reset_safe()
        if brain.initialize_safe():
            print("✓ Brain recovered and re-initialized successfully")
        else:
            print("✗ Recovery failed")
    except Exception as e:
        print(f"✗ Recovery failed: {e}")
        
except NLMError as e:
    print(f"✗ NLM error: {e}")
    
except Exception as e:
    print(f"✗ Unexpected error: {e}")
    # Log error for debugging
    import traceback
    traceback.print_exc()
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

### Advanced Python Bindings

NLM now provides enhanced Python bindings with improved error handling, validation, and API design. The enhanced bindings include:

#### Enhanced Error Handling

```python
import pynlm

# Enhanced configuration with validation
config = pynlm.createDefaultConfig()

# Set configuration with automatic type detection and validation
config.set_safe("brain.neuron_count", "2000")  # Works with strings
config.set_safe("brain.connection_probability", 0.15)

# Validate configuration
if config.has("brain.neuron_count"):
    neurons = config.get_int_or("brain.neuron_count", 0)
    print(f"Neuron count: {neurons}")

# Get configuration with safe defaults
probability = config.get_float_or("brain.connection_probability", 0.1)
print(f"Connection probability: {probability}")

# Create preset configurations
config.create_preset("minimal")
config.create_preset("standard")
config.create_preset("development")
```

#### Enhanced Brain Operations

```python
import pynlm

# Create brain with enhanced safety
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize with validation
try:
    brain.initialize_safe()
    print("✓ Brain initialized successfully")
except Exception as e:
    print(f"✗ Initialization failed: {e}")

# Safe simulation with error recovery
try:
    for step in range(100):
        if not brain.step_safe(step):
            print(f"Simulation stopped at step {step}")
            break
except Exception as e:
    print(f"✗ Simulation error: {e}")

# Enhanced statistics
stats = brain.get_detailed_stats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.3f}")
```

#### Logging Configuration

```python
import pynlm

# Configure logging for debugging
import logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

# Enable NLM logging
pynlm.enable_debug_logging(True)
pynlm.set_log_level(pynlm.LogLevel.DEBUG)

# Create brain and generate logs
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()
brain.step_safe(0)
```

#### Serialization with Enhanced Bindings

```python
import pynlm
from nlm_serialization import create_checkpoint_manager

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# Create advanced checkpoint manager
checkpoint_manager = create_checkpoint_manager()

# Save with advanced options
checkpoint_id = checkpoint_manager.save_checkpoint(
    brain=brain,
    name="advanced_checkpoint",
    tags=["demo", "advanced"],
    metadata={"version": "2.0", "author": "Advanced User"},
    description="Advanced checkpoint with all features"
)

# Load checkpoint
loaded_brain = checkpoint_manager.load_checkpoint(checkpoint_id)
print(f"Loaded brain with {loaded_brain.getTotalNeuronCount()} neurons")
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

## Advanced Features and Tools

NLM now provides several advanced features to enhance your brain simulation experience:

### 1. Command Line Interface (CLI)

The NLM CLI provides a comprehensive command-line interface for running simulations, managing configurations, and exploring brain states without writing Python code.

**Installation and Usage:**
```bash
# The NLM CLI is included with the pynlm installation
# Create a brain
python nlm_cli.py create --component=brain --neuron-count=1000

# Run a simulation with learning
python nlm_cli.py simulate --steps=1000 --enable-reward-modulation --enable-curiosity

# Explore brain state
python nlm_cli.py explore

# Create and manage configurations
python nlm_cli.py config create --filename=my_config.json --neuron-count=2000
python nlm_cli.py config list

# Run demonstrations
python nlm_cli.py demo basic --steps=500
python nlm_cli.py demo learning --steps=1000

# Help and documentation
python nlm_cli.py help
python nlm_cli.py help create
```

**Available Commands:**
- **create** - Create NLM components (brain, agent, world, config)
- **simulate** - Run simulations with configurable parameters
- **explore** - Analyze and display brain state information
- **config** - Manage configuration files and settings
- **checkpoint** - Save and load brain checkpoints
- **demo** - Run built-in demonstrations
- **help** - Show help information

### 2. Enhanced Python Bindings

The enhanced Python bindings provide improved error handling, validation, and API design for better reliability and usability.

**Key Features:**
- **Safe initialization** with configuration validation
- **Enhanced error handling** with detailed error messages
- **Type inference** for configuration values
- **Preset configurations** for common use cases
- **Detailed statistics** and introspection

**Example Usage:**
```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()

# Safe configuration with type inference
config.set_safe("brain.neuron_count", "2000")  # Accepts strings
config.set_safe("brain.connection_probability", 0.15)

# Validate configuration
if config.validate_config():
    print("✓ Configuration is valid")
else:
    print("✗ Configuration validation failed")

# Create preset configurations
config.create_preset("minimal")
config.create_preset("standard")
config.create_preset("development")
config.create_preset("high_plasticity")

# Enhanced brain initialization with safety
brain = pynlm.createBrain(config)
if brain.initialize_safe():
    print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
else:
    print("✗ Brain initialization failed")

# Safe simulation with error handling
for step in range(100):
    if not brain.step_safe(step):
        print(f"Simulation stopped at step {step}")
        break

# Get detailed statistics
stats = brain.get_detailed_stats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"System health: {stats['system_health']['overall_status']}")
```

### 3. Serialization and Checkpoint Management

The serialization module provides comprehensive brain state serialization with support for multiple formats, compression, and advanced features.

**Key Features:**
- **Multiple serialization formats** (binary, JSON, pickle, compressed, base64, hybrid)
- **Automatic compression** with configurable levels
- **Checkpoint management** with automatic indexing
- **Experiment snapshots** and training checkpoints
- **Export/import** with format conversion
- **Validation** and error checking

**Example Usage:**
```python
import pynlm
from nlm_serialization import (
    create_checkpoint_manager,
    SerializationFormat,
    CompressionLevel
)

# Create brain and checkpoint manager
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

checkpoint_manager = create_checkpoint_manager()

# Save with advanced options
checkpoint_id = checkpoint_manager.save_checkpoint(
    brain=brain,
    name="experiment_1",
    format=SerializationFormat.HYBRID,
    tags=["experiment", "demo", "test"],
    metadata={
        "experiment_name": "Demo Experiment",
        "description": "A demonstration of NLM serialization",
        "researcher": "Dr. Smith",
    },
    description="Advanced checkpoint with validation and compression",
    include_stats=True,
    compression=True
)

print(f"✓ Checkpoint saved with ID: {checkpoint_id}")

# List all checkpoints
checkpoints = checkpoint_manager.list_checkpoints()
for checkpoint in checkpoints:
    print(f"  - {checkpoint['name']}: {checkpoint['neuron_count']} neurons")

# Load checkpoint
loaded_brain = checkpoint_manager.load_checkpoint("experiment_1")
print(f"✓ Loaded brain with {loaded_brain.getTotalNeuronCount()} neurons")

# Export to different format
export_path = checkpoint_manager.export_checkpoint(
    "experiment_1",
    "export_example.json",
    SerializationFormat.JSON
)
print(f"✓ Exported to JSON: {export_path}")

# Create experiment snapshot
experiment_id = checkpoint_manager.create_experiment_snapshot(
    brain=brain,
    experiment_name="Demo Experiment",
    steps=1000,
    metrics={
        "performance": "good",
        "accuracy": 0.85,
        "training_time": "10:30",
    }
)
print(f"✓ Experiment snapshot created: {experiment_id}")
```

### 4. Brain Introspection and Analysis

The introspection module provides comprehensive brain state analysis and monitoring capabilities.

**Key Features:**
- **Comprehensive analysis** of brain state and system health
- **Performance metrics** and monitoring
- **Real-time monitoring** with configurable intervals
- **System health checking** with detailed reporting
- **Advanced dynamics analysis** with multiple depths
- **Export capabilities** for analysis results

**Example Usage:**
```python
import pynlm
from nlm_introspection import BrainIntrospection, create_introspection

# Create brain and introspection tool
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

# Create introspection tool
introspection = BrainIntrospection(brain)

# Start comprehensive analysis
print("Starting brain analysis...")
analysis = introspection.start_analysis()

if "error" in analysis:
    print(f"✗ Analysis failed: {analysis['error']}")
else:
    print("✓ Analysis completed successfully")
    print(f"  Brain state: {analysis['brain_state']}")
    print(f"  Neuron count: {analysis['basic_info']['neuron_count']}")
    print(f"  Synapse count: {analysis['basic_info']['synapse_count']}")
    print(f"  System health: {analysis['system_health']['overall_status']}")
    
    # Get detailed analysis
    detailed_analysis = introspection.get_detailed_analysis("comprehensive")
    print(f"  Neural dynamics: {detailed_analysis['neural_dynamics']['activity_level']:.2f}")

# Performance monitoring and real-time tracking
print("\nStarting performance monitoring...")
performance = introspection.performance.get_performance_report()
print(f"Performance: {performance['summary']['throughput']:.2f} steps/sec")
print(f"Uptime: {performance['summary']['uptime']:.1f}s")
print(f"Peak memory: {performance['resource_usage']['peak_memory_mb']:.1f} MB")

# Start real-time monitoring (runs in background)
introspection.start_real_time_monitoring(interval=0.5)

# Export analysis results
filename = introspection.export_analysis("json", "brain_analysis.json")
print(f"Analysis exported to: {filename}")
```

### 5. Logging Configuration

Configure detailed logging for debugging and monitoring.

**Example Usage:**
```python
import pynlm
import logging

# Configure logging
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

# Enable NLM logging
pynlm.enable_debug_logging(True)
pynlm.set_log_level(pynlm.LogLevel.DEBUG)

# Create brain and generate logs
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()
brain.step_safe(0)

# Get log messages
log_messages = pynlm.get_log_messages()
print(f"Log messages: {len(log_messages)}")
```

### 6. Error Handling and Recovery

Robust error handling with recovery mechanisms.

**Example Usage:**
```python
import pynlm
from nlm_enhanced_bindings import NLMError, BrainError

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Attempt initialization with comprehensive error handling
try:
    print("Attempting brain initialization...")
    if brain.initialize_safe():
        print("✓ Brain initialized successfully")
    else:
        print("✗ Brain initialization failed")
        
except BrainError as e:
    print(f"✗ Brain error: {e}")
    # Attempt recovery
    try:
        print("Attempting brain reset and re-initialization...")
        brain.reset_safe()
        if brain.initialize_safe():
            print("✓ Brain recovered and re-initialized successfully")
        else:
            print("✗ Recovery failed")
    except Exception as e:
        print(f"✗ Recovery failed: {e}")
        
except NLMError as e:
    print(f"✗ NLM error: {e}")
    
except Exception as e:
    print(f"✗ Unexpected error: {e}")
    # Log error for debugging
    import traceback
    traceback.print_exc()
```

### 7. System Health Monitoring

Monitor brain system health and performance.

**Example Usage:**
```python
import pynlm
from nlm_introspection import create_introspection

# Create brain and monitoring tool
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize_safe()

introspection = create_introspection(brain)

# System health check
health = introspection._get_system_health()
print(f"Overall system health: {health['overall_status']}")
print(f"Healthy: {health['healthy']}")

if health['warnings']:
    print("Warnings:")
    for warning in health['warnings']:
        print(f"  - {warning}")

if health['errors']:
    print("Errors:")
    for error in health['errors']:
        print(f"  - {error}")

# Performance indicators
performance = health['performance_indicators']
print(f"Stability score: {performance['stability_score']:.2f}")
print(f"Efficiency score: {performance['efficiency_score']:.2f}")
print(f"Resilience score: {performance['resilience_score']:.2f}")
```
