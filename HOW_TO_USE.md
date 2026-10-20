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

## Part 5: Configuration and Schema Examples

### Schema-based Configuration

```python
import pynlm

# Create enhanced configuration with schema validation
config = pynlm.createDefaultConfig()

# Register schema for brain configuration
brain_schema = pynlm.ConfigSchema(
    name="brain",
    description="Brain configuration parameters with validation"
)

# Add brain parameters with validation rules
brain_schema.add_parameter(
    name="neuron_count",
    type="int",
    description="Number of neurons (1-1000000)",
    required=True,
    default_value=1000,
    min_value=1,
    max_value=1000000,
    help_text="Total neurons: determines brain capacity"
)

brain_schema.add_parameter(
    name="synapse_density",
    type="double",
    description="Synapse connectivity density (0.0-1.0)",
    required=False,
    default_value=0.1,
    min_value=0.0,
    max_value=1.0,
    custom_validator=lambda x, y: ValidationResult.Invalid("Density too low") if x < 0.01 else ValidationResult.Valid()
)

# Apply schema
config.registerSchema("brain", brain_schema)

# Validate configuration
validation = config.validateSchema("brain")
if validation.valid:
    print("Configuration is valid!")
else:
    print("Configuration errors:")
    for error in validation.errors:
        print(f"  - {error}")

# Load configuration with schema validation
config.loadFromSchemaFile("config.json", "brain")
config.saveToSchemaFile("validated_config.json", "brain")
```

### Enhanced Configuration Examples

**Advanced Configuration with Templates:**
```python
import pynlm

# Create configuration with enhanced features
config = pynlm.createDefaultConfig()

# Apply research template
config.applyTemplate("research")

# Create custom template with dependencies
config.addTemplate(
    name="high_performance_research",
    parameters={
        "brain.neuron_count": 5000,
        "brain.synapse_density": 0.15,
        "plasticity.stdp.learning_rate": 0.002,
        "development.critical_period_length": 200,
        "neuromodulation.dopamine.baseline": 0.2,
        "neuromodulation.curiosity.enable": True,
        "memory.working_memory_capacity": 100,
        "memory.episodic_memory_size": 5000,
        "log_level": "INFO",
        "log_system_metrics": True,
        "log_performance_metrics": True,
        "log_async": True,
        "async_queue_size": 10000,
        "sample_rate": 0.5
    },
    description="High-performance research configuration",
    category="system"
)

# Apply advanced configuration
config.applyTemplate("high_performance_research")

# Validate configuration
validation = config.validateSchema()
if validation.valid:
    print("Advanced configuration applied successfully")
else:
    print("Configuration validation failed:", validation.errors)
```

**Configuration Management with History:**
```python
import pynlm

# Enable configuration history
config = pynlm.createDefaultConfig()
config.enableHistory(True)

# Make configuration changes with history tracking
config.set("brain.neuron_count", 1000, reason="Initial configuration")
config.set("plasticity.stdp.learning_rate", 0.001, reason="Standard learning rate")

# Check configuration history
history = config.getHistory()
for change in history:
    print(f"{change.timestamp}: {change.key} = {change.newValue} ({change.reason})")

# Create snapshots for version control
snapshot1 = config.createSnapshot("version_1_0")
config.set("brain.neuron_count", 2000, reason="Upgraded for larger simulation")
snapshot2 = config.createSnapshot("version_1_1")

# Rollback to previous version
config.rollbackToSnapshot("version_1_0")
```

**Multi-format Configuration Support:**
```python
import pynlm
import tempfile
import os

# Create sample configurations in different formats
json_config = {
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15,
        "plasticity": {
            "stdp": {"learning_rate": 0.001}
        }
    }
}

yaml_config = """
brain:
  neuron_count: 2000
  synapse_density: 0.15
  plasticity:
    stdp:
      learning_rate: 0.001
"""

# Save and load different formats
with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
    import json
    json.dump(json_config, f)
    json_file = f.name

with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
    f.write(yaml_config)
    yaml_file = f.name

try:
    # Load JSON configuration
    config1 = pynlm.createDefaultConfig()
    config1.loadFromFile(json_file, "brain")
    
    # Load YAML configuration
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile(yaml_file, "brain")
    
    # Save configuration in multiple formats
    config2.saveToFile("output_config.json", "brain")
    config2.saveToFile("output_config.yaml", "brain")
    config2.saveToFile("output_config.ini", "brain")
    
    print("Configuration saved in multiple formats")
    
finally:
    # Clean up temporary files
    os.unlink(json_file)
    os.unlink(yaml_file)
```

---

## Enhanced Logging Documentation

### Comprehensive Logging Guide

NLM now features an enhanced logging infrastructure with advanced features for production use and debugging. The logging system supports multiple levels, categories, and output formats.

#### Log Levels and Categories

**Enhanced Log Levels:**
- `TRACE (-1)`: Finer-grained debugging level for detailed system internals
- `DEBUG (0)`: Standard debug information
- `INFO (1)`: General informational messages
- `WARNING (2)`: Warning conditions that don't prevent operation
- `ERROR (3)`: Error conditions affecting operation
- `CRITICAL (4)`: Critical errors requiring immediate attention
- `FATAL (5)`: Fatal errors causing system shutdown

**Log Categories:**
- `Brain`: Neural processing, neuron dynamics, synaptic operations
- `Memory`: Working memory, episodic memory, associative memory
- `Neuromodulation`: Dopamine, curiosity, novelty detection
- `Cognition`: Planning, attention, concept formation
- `Development`: Synaptogenesis, pruning, maturation
- `Performance`: Simulation performance, throughput metrics
- `System`: System initialization, configuration, shutdown
- `Debug`: Deep debugging information, raw data

#### Enhanced Logging Examples

**Basic Enhanced Logging:**
```python
import pynlm

# Configure enhanced logging
config = pynlm.createDefaultConfig()
config.set("log_level", "INFO")
config.set("log_async", True)
config.set("log_system_metrics", True)
config.set("log_performance_metrics", True)

# Create brain and configure logging
brain = pynlm.createBrain(config)
brain.initialize()

# Enable log categories
pynlm.Logger.enableCategory("Brain", True)
pynlm.Logger.enableCategory("Memory", True)
pynlm.Logger.enableCategory("Neuromodulation", True)
pynlm.Logger.enableCategory("Cognition", True)
pynlm.Logger.enableCategory("Development", True)
pynlm.Logger.enableCategory("Performance", True)
pynlm.Logger.enableCategory("System", True)

# Log with enhanced levels
pynlm.Logger.trace("Detailed debugging information")
pynlm.Logger.debug("Debug information")
pynlm.Logger.info("Informational message")
pynlm.Logger.warning("Warning message")
pynlm.Logger.error("Error message")
pynlm.Logger.critical("Critical error")
pynlm.Logger.fatal("Fatal error")
```

**Structured Logging:**
```python
import pynlm

# Create configuration with structured logging support
config = pynlm.createDefaultConfig()
config.set("log_file", "simulation.log")
config.set("log_async", True)
config.set("log_system_metrics", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Log structured data
for step in range(100):
    brain.step(step)
    
    # Create structured log entry
    log_entry = {
        "timestamp": time.time(),
        "step": step,
        "firing_neurons": brain.getFiringNeuronCount(),
        "total_neurons": brain.getTotalNeuronCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "e_i_ratio": brain.getExcitationInhibitionRatio(),
        "simulation_data": {
            "time": step * 0.1,
            "neuron_activity": "active" if brain.getFiringNeuronCount() > 0 else "inactive"
        },
        "metadata": {
            "project": "NLM",
            "version": "6.0.0",
            "simulation_type": "integration_test"
        }
    }
    
    # Log as structured data
    pynlm.Logger.logStructured("Brain Statistics", log_entry)
    
    if step % 20 == 0:
        print(f"Logged structured data for step {step}")
```

**Performance Monitoring with Logging:**
```python
import pynlm
import time

# Configure for performance monitoring
config = pynlm.createDefaultConfig()
config.set("log_level", "INFO")
config.set("log_async", True)
config.set("log_performance_metrics", True)
config.set("log_system_metrics", True)
config.set("log_sampling", 0.1)  # Sample 10% of logs

brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
start_time = time.time()
steps_completed = 0
performance_log_interval = 100

for step in range(1000):
    step_start = time.time()
    
    # Simulation step
    brain.step(step)
    
    # Check performance
    step_time = time.time() - step_start
    steps_completed += 1
    
    if step % performance_log_interval == 0:
        avg_step_time = (time.time() - start_time) / steps_completed
        rate = steps_completed / (time.time() - start_time)
        print(f"Step {step}: {avg_step_time*1000:.2f}ms avg, {rate:.1f} steps/sec")
        
        # Log performance metrics
        pynlm.Logger.logPerformanceMetrics(
            step=step,
            avg_step_time=avg_step_time,
            firing_rate=brain.getAverageFiringRate(),
            memory_usage=brain.getTotalNeuronCount() * 100
        )
```

### Enhanced CLI Documentation

#### Command Line Interface Examples

**Basic CLI Usage:**
```bash
# Show help and available commands
./nlm_cli --help

# Run a basic simulation
./nlm_cli simulate --config config.json --steps 1000

# Enable enhanced logging
./nlm_cli simulate --log-level INFO --log-async --log-system-metrics

# Run with curiosity and development
./nlm_cli simulate --enable-curiosity --enable-development --enable-reward
```

**Configuration Management:**
```bash
# Validate configuration file
./nlm_cli config validate config.json

# Generate configuration template
./nlm_cli config generate brain.json brain

# Migrate configuration
./nlm_cli config migrate v5.2.0 v6.0.0
```

**Logging Management:**
```bash
# Check logging status
./nlm_cli log status

# Enable log categories
./nlm_cli log enable Brain
./nlm_cli log enable Memory
./nlm_cli log enable Neuromodulation

# Set category levels
./nlm_cli log set-level Brain:INFO
./nlm_cli log set-level Memory:WARNING
./nlm_cli log set-level System:DEBUG

# Export logging configuration
./nlm_cli log export current_logging.json
```

**Performance Monitoring:**
```bash
# Start performance monitoring
./nlm_cli monitor --interval 0.5 --metrics --system

# Monitor specific categories
./nlm_cli monitor --categories Brain,Memory --performance
```

### Advanced Usage Examples

**Complex Brain Simulation with Enhanced Features:**
```python
import pynlm

# Create complex simulation configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("development.critical_period_length", 200)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromodulation.dopamine.baseline", 0.2)
config.set("neuromodulation.curiosity.enable", True)
config.set("neuromodulation.novelty.enable", True)
config.set("memory.working_memory_capacity", 100)
config.set("memory.episodic_memory_size", 5000)
config.set("log_level", "INFO")
config.set("log_async", True)
config.set("log_system_metrics", True)
config.set("log_performance_metrics", True)

# Create simulation components
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("=== Complex Simulation Started ===")

# Run complex simulation
for step in range(3000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in agent
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 300 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev stage: {brain.getDevelopmentalStage()}")
        
        # Log structured data
        pynlm.Logger.logStructured("Progress", {
            "step": step,
            "firing_neurons": brain.getFiringNeuronCount(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "development_stage": str(brain.getDevelopmentalStage()),
            "simulation_progress": step / 3000.0
        })

print("\n=== Complex simulation completed ===")
```

**Configuration Schema Example:**
```python
import pynlm

# Create configuration with schema validation
config = pynlm.createDefaultConfig()

# Define configuration schema
brain_schema = {
    "name": "brain",
    "description": "Brain configuration with validation",
    "parameters": [
        {
            "name": "neuron_count",
            "type": "int",
            "description": "Number of neurons",
            "required": True,
            "default": 1000,
            "min": 1,
            "max": 1000000
        },
        {
            "name": "synapse_density",
            "type": "double",
            "description": "Synapse density",
            "required": False,
            "default": 0.1,
            "min": 0.0,
            "max": 1.0
        }
    ]
}

# Register schema
config.registerSchemaFromDefinition("brain", brain_schema)

# Validate configuration
validation = config.validateSchema("brain")
if validation.valid:
    print("Configuration validated successfully")
    
    # Apply configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    
    print(f"Configured with {config.get('brain.neuron_count')} neurons")
else:
    print("Configuration validation failed:", validation.errors)
```

**Performance Optimization Example:**
```python
import pynlm
import time

# Performance-optimized configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("log_level", "WARNING")
config.set("log_async", True)
config.set("log_sampling", 0.05)
config.set("log_system_metrics", False)
config.set("log_performance_metrics", False)

brain = pynlm.createBrain(config)
brain.initialize()

# High-performance simulation
start_time = time.time()
steps_completed = 0

for step in range(2000):
    brain.step(step)
    steps_completed += 1
    
    if step % 500 == 0:
        elapsed = time.time() - start_time
        rate = steps_completed / elapsed
        print(f"Progress: {step}/2000 steps, {rate:.1f} steps/sec")

elapsed = time.time() - start_time
rate = steps_completed / elapsed
print(f"\nPerformance summary:")
print(f"Total steps: {steps_completed}")
print(f"Total time: {elapsed:.2f}s")
print(f"Performance: {rate:.1f} steps/sec")
```

---

## Troubleshooting

### Enhanced Troubleshooting

**Configuration Validation Error Diagnosis:**
```python
import pynlm

# Comprehensive configuration validation
config = pynlm.createDefaultConfig()

# Load configuration
if not config.loadFromFile("simulation_config.json"):
    print("Failed to load configuration file")
    exit(1)

# Validate configuration
validation = config.validateSchema()
if not validation.valid:
    print("Configuration validation errors:")
    
    for error in validation.errors:
        print(f"  - {error}")
    
    for param, param_result in validation.parameterResults.items():
        if not param_result.valid:
            print(f"  Parameter '{param}': {param_result.message}")
            
    print("\nSuggested fixes:")
    print("  1. Check required parameters are present")
    print("  2. Verify parameter types and ranges")
    print("  3. Ensure parameter dependencies are satisfied")
    print("  4. Review schema definition for conflicts")
```

**Performance Troubleshooting:**
```python
import pynlm
import psutil
import time

# Performance monitoring and diagnosis
class PerformanceMonitor:
    def __init__(self):
        self.metrics = {}
        self.start_time = time.time()
    
    def start_monitoring(self, config):
        """Start performance monitoring"""
        self.metrics['start_time'] = time.time()
        self.metrics['config'] = config
        
        # Enable performance metrics logging
        config.set("log_performance_metrics", True)
        config.set("log_system_metrics", True)
    
    def log_performance(self, step, brain):
        """Log performance metrics"""
        process = psutil.Process()
        
        metrics = {
            'timestamp': time.time(),
            'step': step,
            'cpu_percent': process.cpu_percent(),
            'memory_mb': process.memory_info().rss / 1024 / 1024,
            'neurons_firing': brain.getFiringNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'step_time': time.time() - self.metrics.get('last_step_time', time.time())
        }
        
        self.metrics['last_step_time'] = time.time()
        self.metrics['performance_data'].append(metrics)
        
        return metrics
    
    def analyze_performance(self):
        """Analyze performance data"""
        if not self.metrics.get('performance_data'):
            return "No performance data available"
        
        data = self.metrics['performance_data']
        
        # Calculate performance statistics
        step_times = [d['step_time'] for d in data]
        avg_step_time = sum(step_times) / len(step_times)
        
        return {
            'avg_step_time': avg_step_time,
            'min_step_time': min(step_times),
            'max_step_time': max(step_times),
            'steps_per_second': 1.0 / avg_step_time if avg_step_time > 0 else 0,
            'total_steps': len(data),
            'total_time': data[-1]['timestamp'] - data[0]['timestamp']
        }

# Usage example
monitor = PerformanceMonitor()
monitor.start_monitoring(config)

# Run simulation with monitoring
for step in range(1000):
    brain.step(step)
    metrics = monitor.log_performance(step, brain)
    
    if step % 100 == 0:
        print(f"Step {step}: {metrics['step_time']*1000:.2f}ms")

# Analyze performance
analysis = monitor.analyze_performance()
print(f"\nPerformance Analysis:")
for key, value in analysis.items():
    if isinstance(value, float):
        print(f"  {key}: {value:.3f}")
    else:
        print(f"  {key}: {value}")
```

**Logging Troubleshooting:**
```python
import pynlm

# Comprehensive logging troubleshooting
class LoggingTroubleshooter:
    def __init__(self):
        self.log_file = "nlm.log"
    
    def check_log_file(self):
        """Check log file status and permissions"""
        import os
        import stat
        
        if not os.path.exists(self.log_file):
            print(f"Log file {self.log_file} does not exist")
            return False
        
        file_stat = os.stat(self.log_file)
        if not file_stat.st_size:
            print(f"Log file {self.log_file} is empty")
            return False
        
        # Check write permissions
        if not os.access(self.log_file, os.W_OK):
            print(f"Log file {self.log_file} is not writable")
            return False
        
        # Check file size
        file_size_mb = file_stat.st_size / (1024 * 1024)
        print(f"Log file size: {file_size_mb:.2f} MB")
        
        return True
    
    def diagnose_logging_issues(self):
        """Diagnose common logging issues"""
        issues = []
        
        # Check if logging is enabled
        if not hasattr(pynlm.Logger, 'enableCategory'):
            issues.append("Logger does not support category-based logging")
        
        # Check log file
        if not self.check_log_file():
            issues.append("Log file issues detected")
        
        # Check system resources
        import psutil
        memory = psutil.virtual_memory()
        if memory.available < 100 * 1024 * 1024:  # Less than 100MB
            issues.append(f"Low memory available: {memory.available / (1024*1024):.1f}MB")
        
        disk = psutil.disk_usage('.')
        if disk.free < 100 * 1024 * 1024:  # Less than 100MB free
            issues.append(f"Low disk space: {disk.free / (1024*1024):.1f}MB free")
        
        return issues
    
    def suggest_fixes(self, issues):
        """Suggest fixes for logging issues"""
        print("\nSuggested fixes:")
        
        for issue in issues:
            if "category-based logging" in issue:
                print("  - Enable log categories: pynlm.Logger.enableCategory('Brain', True)")
            
            elif "Log file" in issue and "writable" in issue:
                print(f"  - Fix log file permissions: chmod +w {self.log_file}")
            
            elif "low memory" in issue.lower():
                print("  - Reduce logging overhead or increase system memory")
                print("  - Disable system metrics logging: config.set('log_system_metrics', False)")
            
            elif "low disk space" in issue.lower():
                print("  - Configure log rotation: config.set('max_file_size', 10 * 1024 * 1024)")
                print("  - Enable log compression: config.set('compress_logs', True)")

# Usage example
troubleshooter = LoggingTroubleshooter()
issues = troubleshooter.diagnose_logging_issues()

if issues:
    print(f"Logging issues detected: {len(issues)}")
    for issue in issues:
        print(f"  - {issue}")
    
    troubleshooter.suggest_fixes(issues)
else:
    print("No logging issues detected")
```

---

## Advanced Usage Examples

### Complex Simulation with Advanced Features

**Example: Multi-phase Development Simulation:**
```python
import pynlm

# Advanced simulation configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 8000)
config.set("brain.synapse_density", 0.2)
config.set("development.critical_period_length", 300)
config.set("development.synaptogenesis_rate", 0.002)
config.set("development.pruning_rate", 0.0002)
config.set("plasticity.stdp.learning_rate", 0.003)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 25.0)
config.set("plasticity.hebbian.learning_rate", 0.001)
config.set("neuromodulation.dopamine.baseline", 0.15)
config.set("neuromodulation.dopamine.scale", 1.2)
config.set("neuromodulation.curiosity.enable", True)
config.set("neuromodulation.curiosity.exploration_bonus", 0.5)
config.set("neuromodulation.novelty.enable", True)
config.set("neuromodulation.novelty.sensitivity", 0.8)
config.set("memory.working_memory_capacity", 200)
config.set("memory.episodic_memory_size", 10000)
config.set("memory.consolidation_rate", 0.02)
config.set("log_level", "INFO")
config.set("log_async", True)
config.set("log_system_metrics", True)
config.set("log_performance_metrics", True)
config.set("log_sampling", 0.2)

# Create advanced simulation
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width(50, height=50, visionWidth=15, visionHeight=15)
world.reset()
agent.initialize(world)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("=== Advanced Multi-phase Simulation Started ===")

# Phase 1: Synaptogenesis (steps 0-1000)
print("\nPhase 1: Synaptogenesis and initial learning")
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 200 == 0:
        print(f"  Synaptogenesis step {step}: {brain.getFiringNeuronCount()} firing neurons")
        pynlm.Logger.logStructured("Synaptogenesis", {
            "phase": 1,
            "step": step,
            "neuron_count": brain.getTotalNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "development_stage": str(brain.getDevelopmentalStage())
        })

# Phase 2: Critical period (steps 1000-2000)
print("\nPhase 2: Critical period and rapid learning")
for step in range(1000, 2000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 200 == 0:
        print(f"  Critical period step {step}: {brain.getFiringNeuronCount()} firing neurons")
        pynlm.Logger.logStructured("Critical Period", {
            "phase": 2,
            "step": step,
            "neuron_count": brain.getTotalNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "development_stage": str(brain.getDevelopmentalStage()),
            "plasticity_rate": brain.getAverageFiringRate()
        })

# Phase 3: Maturation (steps 2000-3000)
print("\nPhase 3: Maturation and specialization")
for step in range(2000, 3000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 200 == 0:
        print(f"  Maturation step {step}: {brain.getFiringNeuronCount()} firing neurons")
        pynlm.Logger.logStructured("Maturation", {
            "phase": 3,
            "step": step,
            "neuron_count": brain.getTotalNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "development_stage": str(brain.getDevelopmentalStage()),
            "synaptic_efficiency": brain.getExcitationInhibitionRatio()
        })

# Phase 4: Adult specialization (steps 3000-4000)
print("\nPhase 4: Adult specialization and expert performance")
for step in range(3000, 4000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    if step % 200 == 0:
        print(f"  Adult step {step}: {brain.getFiringNeuronCount()} firing neurons")
        pynlm.Logger.logStructured("Adult Specialization", {
            "phase": 4,
            "step": step,
            "neuron_count": brain.getTotalNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "development_stage": str(brain.getDevelopmentalStage()),
            "expertise_level": brain.getAverageFiringRate()
        })

print("\n=== Advanced multi-phase simulation completed ===")
```

### Configuration Schema Examples

**Example: Complex Configuration Schema:**
```python
import pynlm

# Create comprehensive configuration schema
config = pynlm.createDefaultConfig()

# Define hierarchical configuration schema
brain_schema = pynlm.ConfigSchema(
    name="brain",
    description="Comprehensive brain configuration"
)

# Add nested parameters with validation
brain_parameters = [
    {
        "name": "neuron_count",
        "type": "int",
        "description": "Total number of neurons",
        "required": True,
        "default": 1000,
        "min": 1,
        "max": 1000000,
        "category": "Basic Configuration"
    },
    {
        "name": "synapse_density",
        "type": "double",
        "description": "Average synaptic connectivity",
        "required": False,
        "default": 0.1,
        "min": 0.0,
        "max": 1.0,
        "category": "Neural Connectivity"
    },
    {
        "name": "v_thresh",
        "type": "double",
        "description": "Neuronal threshold potential (mV)",
        "required": False,
        "default": -50.0,
        "min": -100.0,
        "max": 0.0,
        "category": "Neuronal Properties"
    },
    {
        "name": "plasticity.stdp.learning_rate",
        "type": "double",
        "description": "STDP learning rate",
        "required": False,
        "default": 0.001,
        "min": 0.0,
        "max": 0.1,
        "category": "Plasticity"
    },
    {
        "name": "plasticity.hebbian.learning_rate",
        "type": "double",
        "description": "Hebbian learning rate",
        "required": False,
        "default": 0.0005,
        "min": 0.0,
        "max": 0.1,
        "category": "Plasticity"
    }
]

# Add parameters to schema
for param_data in brain_parameters:
    param = nlm.ParameterMetadata(
        name=param_data["name"],
        type=param_data["type"],
        description=param_data["description"],
        required=param_data["required"],
        default_value=param_data["default"],
        help_text=f"Configure {param_data['name']} for {param_data.get('category', '')}"
    )
    
    if "min" in param_data:
        param.hasMin = True
        param.minValue = param_data["min"]
    
    if "max" in param_data:
        param.hasMax = True
        param.maxValue = param_data["max"]
    
    brain_schema.addParameter(param)

# Register schema
config.registerSchema("brain", brain_schema)

# Load configuration
config.loadFromFile("complex_config.json")

# Validate configuration
validation = config.validateSchema("brain")
if validation.valid:
    print("Configuration validated successfully")
    
    # Use validated configuration
    neuron_count = config.getOr("brain.neuron_count", 1000)
    learning_rate = config.getOr("brain.plasticity.stdp.learning_rate", 0.001)
    
    print(f"Brain configured: {neuron_count} neurons, STDP learning rate: {learning_rate}")
else:
    print("Configuration validation failed:")
    for error in validation.errors:
        print(f"  - {error}")
```

**Example: Template-based Configuration:**
```python
import pynlm

# Create template-based configuration system
config = pynlm.createDefaultConfig()

# Define research template
research_template = {
    "name": "research_standard",
    "description": "Standard configuration for research simulations",
    "parameters": {
        "brain.neuron_count": 5000,
        "brain.synapse_density": 0.15,
        "brain.v_thresh": -50.0,
        "brain.v_rest": -70.0,
        "brain.tau_mem": 20.0,
        "brain.tau_ref": 2.0,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.002,
        "plasticity.stdp.tau_plus": 20.0,
        "plasticity.stdp.tau_minus": 20.0,
        "plasticity.hebbian.enable": True,
        "plasticity.hebbian.learning_rate": 0.001,
        "plasticity.structural.enable": True,
        "plasticity.structural.growth_rate": 0.0001,
        "neuromodulation.dopamine.baseline": 0.1,
        "neuromodulation.dopamine.scale": 1.0,
        "neuromodulation.curiosity.enable": True,
        "neuromodulation.curiosity.exploration_bonus": 0.5,
        "neuromodulation.novelty.enable": True,
        "neuromodulation.novelty.sensitivity": 0.8,
        "memory.working_memory_capacity": 100,
        "memory.episodic_memory_size": 5000,
        "memory.consolidation_rate": 0.02,
        "development.critical_period_length": 200,
        "development.synaptogenesis_rate": 0.002,
        "development.pruning_rate": 0.0002,
        "log_level": "INFO",
        "log_system_metrics": True,
        "log_performance_metrics": True,
        "log_async": True,
        "async_queue_size": 10000,
        "sample_rate": 0.5
    },
    "category": "research"
}

# Add template to configuration
config.addTemplate(
    name=research_template["name"],
    parameters=research_template["parameters"],
    description=research_template["description"],
    category=research_template["category"]
)

# Apply research template
config.applyTemplate("research_standard")

# Customize template
config.set("brain.neuron_count", 8000, reason="Increased for more complex simulations")
config.set("plasticity.stdp.learning_rate", 0.003, reason="Faster learning for research")

# Validate customized configuration
validation = config.validateSchema()
if validation.valid:
    print("Research configuration applied and validated successfully")
    
    # Use configuration
    print(f"Research simulation: {config.get('brain.neuron_count')} neurons")
    print(f"Learning rate: {config.get('plasticity.stdp.learning_rate')}")
else:
    print("Configuration validation failed:", validation.errors)
```

### Error Handling Examples

**Comprehensive Error Handling:**
```python
import pynlm

def run_enhanced_simulation(config_file, max_steps=1000):
    """Run enhanced simulation with comprehensive error handling"""
    
    try:
        # Load configuration with validation
        config = pynlm.createDefaultConfig()
        
        if not config.loadFromFile(config_file):
            raise pynlm.ConfigurationError(f"Failed to load configuration from {config_file}")
        
        # Validate configuration
        validation = config.validateSchema()
        if not validation.valid:
            raise pynlm.ConfigurationError(
                f"Configuration validation failed: {'; '.join(validation.errors)}"
            )
        
        # Create brain with error handling
        try:
            brain = pynlm.createBrain(config)
            brain.initialize()
        except Exception as e:
            raise pynlm.SimulationError(f"Failed to initialize brain: {e}")
        
        # Create agent with error handling
        try:
            agent = pynlm.createAgentBrain(brain)
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            agent.initialize(world)
            
            # Enable all subsystems
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
        except Exception as e:
            raise pynlm.SimulationError(f"Failed to initialize agent or world: {e}")
        
        # Run simulation with error recovery
        for step in range(max_steps):
            try:
                # Update world
                world.update(0.1)
                
                # Get percept
                percept = world.getSensoryPercept()
                
                # Process in agent
                agent.processSensoryInput(percept)
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
                
                # Log progress
                if step % 100 == 0:
                    print(f"Step {step}: "
                          f"Firing: {brain.getFiringNeuronCount()}, "
                          f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                          f"Novelty: {agent.getNoveltyLevel():.3f}")
                
            except Exception as step_error:
                print(f"Warning: Error at step {step}: {step_error}")
                
                # Attempt recovery
                try:
                    brain.reset()
                    world.reset()
                    agent.initialize(world)
                    print(f"Recovered from step error at {step}")
                except:
                    print(f"Failed to recover from step error at {step}")
                    raise pynlm.SimulationError(f"Failed to recover from step {step} error")
        
        print(f"\nSimulation completed successfully!")
        print(f"Total steps: {max_steps}")
        print(f"Final firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
        
        return True
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        return False
    
    except pynlm.SimulationError as e:
        print(f"Simulation error: {e}")
        return False
    
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False

# Usage example
def main():
    print("=== Enhanced NLM Simulation ===")
    
    # Create default configuration file
    default_config = {
        "brain": {
            "neuron_count": 3000,
            "synapse_density": 0.1,
            "v_thresh": -50.0,
            "v_rest": -70.0
        },
        "plasticity": {
            "stdp": {"learning_rate": 0.001},
            "hebbian": {"learning_rate": 0.0005}
        }
    }
    
    import json
    with open("default_simulation_config.json", 'w') as f:
        json.dump(default_config, f, indent=2)
    
    # Run simulation
    success = run_enhanced_simulation("default_simulation_config.json", max_steps=500)
    
    if success:
        print("\n✓ Simulation completed successfully")
    else:
        print("\n✗ Simulation failed")

if __name__ == "__main__":
    main()
```

**Exception Handling with Custom Exception Types:**
```python
import pynlm

# Custom exception types for NLM
class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class ConfigurationError(NLMError):
    """Configuration-related errors"""
    pass

class SimulationError(NLMError):
    """Simulation runtime errors"""
    pass

class LoggingError(NLMError):
    """Logging-related errors"""
    pass

class ValidationError(NLMError):
    """Validation errors"""
    pass

# Enhanced error handling with custom exceptions
def run_simulation_with_exceptions(config_file, max_steps=1000):
    """Run simulation with custom exception handling"""
    
    try:
        # Load configuration
        config = pynlm.createDefaultConfig()
        
        if not config.loadFromFile(config_file):
            raise ConfigurationError(f"Failed to load configuration from {config_file}")
        
        # Validate configuration
        validation = config.validateSchema()
        if not validation.valid:
            raise ValidationError(
                f"Configuration validation failed with {len(validation.errors)} errors"
            )
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent and world
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(max_steps):
            try:
                world.update(0.1)
                
                # Get percept and process
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                # Get action and apply
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
                
            except Exception as step_error:
                raise SimulationError(f"Error at step {step}: {step_error}")
        
        print("✓ Simulation completed successfully")
        return True
        
    except ConfigurationError as e:
        print(f"⚠ Configuration error: {e}")
        print("Please check your configuration file and try again")
        return False
    
    except SimulationError as e:
        print(f"⚠ Simulation error: {e}")
        print("Attempting recovery...")
        # Implement recovery logic
        return False
    
    except LoggingError as e:
        print(f"⚠ Logging error: {e}")
        print("Check log file permissions and disk space")
        return False
    
    except ValidationError as e:
        print(f"⚠ Validation error: {e}")
        print("Please fix configuration validation issues")
        return False
    
    except NLMError as e:
        print(f"⚠ NLM error: {e}")
        return False
    
    except Exception as e:
        print(f"⚠ Unexpected error: {e}")
        return False

# Usage with exception handling
def main():
    print("=== NLM Simulation with Enhanced Error Handling ===")
    
    try:
        success = run_simulation_with_exceptions("config.json", max_steps=500)
        
        if success:
            print("\n🎉 Simulation completed successfully!")
        else:
            print("\n❌ Simulation failed")
            
    except KeyboardInterrupt:
        print("\n⚠ Simulation interrupted by user")
    except Exception as e:
        print(f"\n💥 Critical error: {e}")
        print("Please check the error details and try again")

if __name__ == "__main__":
    main()
```

---

## Best Practices Guide

### Performance Optimization Best Practices

**1. Efficient Configuration for Large Simulations:**
```python
# High-performance configuration for large-scale simulations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("log_level", "WARNING")  # Reduce log volume
config.set("log_async", True)      # Prevent I/O blocking
config.set("log_sampling", 0.1)    # Sample 10% of logs
config.set("log_system_metrics", False)  # Disable for performance
config.set("log_performance_metrics", False)
```

**2. Memory Management:**
```python
# Memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Balanced memory usage
config.set("log_file", "")              # Disable file logging
config.set("log_async", True)           # Reduce memory pressure
config.set("log_level", "ERROR")        # Minimal logging overhead

# Force garbage collection
import gc
gc.collect()
```

**3. Real-time Simulation Performance:**
```python
# For real-time applications
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("simulation_timestep", 0.001)  # Small timestep
config.set("log_level", "INFO")
config.set("log_async", True)
config.set("async_queue_size", 5000)  # Manage memory

# Performance monitoring
import time
start_time = time.time()
for step in range(1000):
    step_start = time.time()
    brain.step(step)
    step_time = time.time() - step_start
    
    if step_time > 0.1:  # Step takes too long
        print(f"Performance warning: Step {step} took {step_time*1000:.2f}ms")
```

### Configuration Management Best Practices

**1. Hierarchical Configuration:**
```python
# Use categories for organized configuration
config = pynlm.createDefaultConfig()

# Brain configuration
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)

# Plasticity configuration  
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.learning_rate", 0.001)

# Neuromodulation configuration
config.set("neuromodulation.dopamine.baseline", 0.2)
config.set("neuromodulation.curiosity.enable", True)

# Memory configuration
config.set("memory.working_memory_capacity", 100)
config.set("memory.episodic_memory_size", 5000)

# Validate each category
brain_validation = config.validateSchema("brain")
if brain_validation.valid:
    print("Brain configuration is valid")

neuromod_validation = config.validateSchema("neuromodulation")
if neuromod_validation.valid:
    print("Neuromodulation configuration is valid")
```

**2. Configuration Templates:**
```python
# Create reusable configuration templates
config = pynlm.createDefaultConfig()

# Research template
config.addTemplate("research", {
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.learning_rate": 0.002,
    "development.critical_period_length": 200,
    "neuromodulation.dopamine.baseline": 0.2,
    "neuromodulation.curiosity.enable": True,
    "memory.working_memory_capacity": 100,
    "memory.episodic_memory_size": 5000,
    "log_level": "INFO",
    "log_system_metrics": True,
    "log_performance_metrics": True
}, "Research-grade configuration", "brain")

# Production template
config.addTemplate("production", {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.1,
    "log_level": "WARNING",
    "log_async": True,
    "log_sampling": 0.1,
    "log_system_metrics": False
}, "Production-ready configuration", "system")

# Apply templates
config.applyTemplate("research")
```

**3. Configuration History and Version Control:**
```python
# Enable configuration history
config = pynlm.createDefaultConfig()
config.enableHistory(True)

# Make configuration changes
config.set("brain.neuron_count", 2000, reason="Initial configuration")
config.set("plasticity.stdp.learning_rate", 0.001, reason="Standard learning rate")

# Check history
history = config.getHistory()
for change in history:
    print(f"{change.timestamp}: {change.key} = {change.newValue} ({change.reason})")

# Create snapshots for version control
snapshot1 = config.createSnapshot("version_1_0")
snapshot2 = config.createSnapshot("version_1_1")

# Rollback if needed
config.rollbackToSnapshot("version_1_0")
```

### Logging Best Practices

**1. Appropriate Log Levels:**
```python
# Use correct log levels for different message types
pynlm.Logger.logDebug("Debug information for troubleshooting")      # Use for detailed debugging
pynlm.Logger.logInfo("Information about simulation progress")         # Use for general information
pynlm.Logger.logWarning("Potential issue that doesn't prevent operation") # Use for warnings
pynlm.Logger.logError("Error condition that affects operation")        # Use for errors
pynlm.Logger.logCritical("Critical error that requires immediate attention") # Use for critical errors
pynlm.Logger.logFatal("Fatal error causing system shutdown")           # Use for fatal errors
```

**2. Structured Logging:**
```python
# Use structured logging for better log analysis
pynlm.Logger.logStructured("Brain Initialization", {
    "neuron_count": brain.getTotalNeuronCount(),
    "synapse_count": brain.getTotalSynapseCount(),
    "firing_rate": brain.getAverageFiringRate(),
    "development_stage": brain.getDevelopmentalStage(),
    "timestamp": time.time(),
    "simulation_id": "sim_12345"
})

# JSON logging for machine-readable logs
pynlm.Logger.logJson("Simulation Step", {
    "step": step,
    "neuron_activity": brain.getFiringNeuronCount(),
    "reward": current_reward,
    "performance": {
        "steps_per_second": steps_per_second,
        "memory_usage": memory_usage
    }
})
```

**3. Category-based Logging:**
```python
# Enable appropriate log categories
pynlm.Logger.enableCategory("Brain", True)
pynlm.Logger.enableCategory("Memory", True)
pynlm.Logger.enableCategory("Neuromodulation", True)
pynlm.Logger.enableCategory("Cognition", True)
pynlm.Logger.enableCategory("Development", True)
pynlm.Logger.enableCategory("Performance", True)
pynlm.Logger.enableCategory("System", True)

# Set category-specific log levels
pynlm.Logger.setCategoryLevel("Brain", "INFO")
pynlm.Logger.setCategoryLevel("Memory", "WARNING")
pynlm.Logger.setCategoryLevel("Neuromodulation", "INFO")
pynlm.Logger.setCategoryLevel("Cognition", "DEBUG")
pynlm.Logger.setCategoryLevel("Development", "INFO")
pynlm.Logger.setCategoryLevel("Performance", "INFO")
pynlm.Logger.setCategoryLevel("System", "INFO")
```

### Error Handling Best Practices

**1. Graceful Error Recovery:**
```python
import pynlm

def run_simulation_with_recovery(config_file, max_steps):
    """Run simulation with comprehensive error handling and recovery"""
    
    # Load configuration
    config = pynlm.createDefaultConfig()
    if not config.loadFromFile(config_file):
        print(f"Failed to load configuration from {config_file}")
        return False
    
    # Validate configuration
    validation = config.validateSchema()
    if not validation.valid:
        print("Configuration validation errors:")
        for error in validation.errors:
            print(f"  - {error}")
        return False
    
    # Create brain
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
    except Exception as e:
        print(f"Error initializing brain: {e}")
        
        # Attempt recovery with reduced configuration
        try:
            config.set("brain.neuron_count", 1000, reason="Recovery configuration")
            config.set("log_level", "ERROR", reason="Reduce logging overhead")
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            print("Recovered with reduced configuration")
        except:
            print("Recovery failed, aborting simulation")
            return False
    
    # Run simulation with error handling
    try:
        for step in range(max_steps):
            try:
                brain.step(step)
                
                # Check for simulation health
                if brain.getFiringNeuronCount() == 0 and step > 100:
                    print(f"Warning: No firing neurons at step {step}")
                
                # Periodic status report
                if step % 100 == 0:
                    print(f"Progress: Step {step}/{max_steps}, "
                          f"Firing neurons: {brain.getFiringNeuronCount()}")
                    
            except Exception as step_error:
                print(f"Error at step {step}: {step_error}")
                
                # Attempt recovery
                try:
                    brain.reset()
                    print(f"Recovered from step error at {step}")
                except:
                    print(f"Failed to recover from step error at {step}")
                    return False
        
        print("Simulation completed successfully")
        return True
        
    except Exception as sim_error:
        print(f"Simulation failed: {sim_error}")
        return False

# Usage example
if __name__ == "__main__":
    success = run_simulation_with_recovery("simulation_config.json", 1000)
    if success:
        print("✓ Simulation completed successfully")
    else:
        print("✗ Simulation failed")
```

**2. Input Validation:**
```python
def validate_simulation_parameters(config):
    """Validate simulation parameters before running"""
    
    errors = []
    
    # Check neuron count
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 1 or neuron_count > 1000000:
        errors.append(f"Invalid neuron count: {neuron_count} (must be 1-1,000,000)")
    
    # Check timestep
    timestep = config.getOr("simulation.timestep", 0.001)
    if timestep <= 0 or timestep > 1.0:
        errors.append(f"Invalid timestep: {timestep} (must be > 0 and <= 1.0)")
    
    # Check memory configuration
    memory_capacity = config.getOr("memory.working_memory_capacity", 50)
    if memory_capacity < 10 or memory_capacity > 1000:
        errors.append(f"Invalid working memory capacity: {memory_capacity} (must be 10-1000)")
    
    # Check developmental parameters
    critical_period = config.getOr("development.critical_period_length", 100)
    if critical_period < 10 or critical_period > 1000:
        errors.append(f"Invalid critical period length: {critical_period} (must be 10-1000)")
    
    if errors:
        print("Configuration validation errors:")
        for error in errors:
            print(f"  - {error}")
        return False
    
    print("Configuration validation passed")
    return True
```

**3. Exception Hierarchy:**
```python
try:
    # Simulation code
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(max_steps):
        brain.step(step)
        
except pynlm.ConfigurationError as e:
    # Configuration-related errors
    print(f"Configuration error: {e}")
    
except pynlm.SimulationError as e:
    # Simulation runtime errors
    print(f"Simulation error: {e}")
    
except pynlm.MemoryError as e:
    # Memory allocation errors
    print(f"Memory error: {e}")
    # Attempt recovery with reduced configuration
    
except pynlm.PerformanceError as e:
    # Performance-related errors
    print(f"Performance error: {e}")
    
except Exception as e:
    # Unexpected errors
    print(f"Unexpected error: {e}")
    # Log error and attempt graceful shutdown
```

### Development Workflow Recommendations

**1. Testing and Validation:**
```bash
# Build with tests
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
ctest --output-on-failure

# Run specific tests
./nlm_test --gtest_filter="*logging*"
./nlm_test --gtest_filter="*config*"

# Run enhanced logging demo
./nlm_phase6_enhanced_demo

# Run original demo (backward compatibility)
./nlm_phase6_demo
```

**2. Code Quality and Linting:**
```bash
# Check code quality
make check-style
make run-clang-tidy

# Validate documentation
generate-documentation --check

# Type checking (if applicable)
run-typecheck
```

**3. Integration Testing:**
```python
# Integration test for enhanced features
import pynlm

def test_enhanced_logging():
    """Test enhanced logging features"""
    config = pynlm.createDefaultConfig()
    config.set("log_level", "INFO")
    config.set("log_async", True)
    config.set("log_system_metrics", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test logging at different levels
    pynlm.Logger.logTrace("Trace message")
    pynlm.Logger.logDebug("Debug message")
    pynlm.Logger.logInfo("Info message")
    pynlm.Logger.logWarning("Warning message")
    pynlm.Logger.logError("Error message")
    pynlm.Logger.logCritical("Critical message")
    pynlm.Logger.logFatal("Fatal message")
    
    print("Enhanced logging test passed")

def test_configuration_validation():
    """Test configuration validation"""
    config = pynlm.createDefaultConfig()
    
    # Set valid configuration
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    # Validate
    validation = config.validateSchema("brain")
    assert validation.valid, "Configuration validation failed"
    
    # Test invalid configuration
    config.set("brain.neuron_count", -1)  # Invalid
    validation = config.validateSchema("brain")
    assert not validation.valid, "Invalid configuration should fail validation"
    
    print("Configuration validation test passed")

def test_performance_monitoring():
    """Test performance monitoring"""
    config = pynlm.createDefaultConfig()
    config.set("log_performance_metrics", True)
    config.set("log_system_metrics", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with performance monitoring
    start_time = time.time()
    for step in range(100):
        brain.step(step)
    
    elapsed = time.time() - start_time
    rate = 100 / elapsed
    print(f"Performance: {rate:.1f} steps/sec")
    
    # Check that performance metrics were logged
    print("Performance monitoring test passed")

# Run integration tests
if __name__ == "__main__":
    test_enhanced_logging()
    test_configuration_validation()
    test_performance_monitoring()
    print("All integration tests passed")
```

**4. Performance Benchmarking:**
```bash
#!/bin/bash
# Performance benchmark script

echo "=== NLM Performance Benchmark ==="

echo "1. Building with optimizations..."
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PERF_MEASUREMENTS=ON
make -j$(nproc)

echo "2. Running performance benchmarks..."

# Benchmark 1: Simple brain
./nlm_bench --scenario=simple --steps=1000

echo "Benchmark 1 complete"

# Benchmark 2: Complex brain
./nlm_bench --scenario=complex --steps=5000

echo "Benchmark 2 complete"

# Benchmark 3: Logging performance
./nlm_bench --scenario=logging --steps=1000 --log-level=INFO

echo "Benchmark 3 complete"

# Benchmark 4: Memory usage
./nlm_bench --scenario=memory --steps=2000 --check-memory

echo "Benchmark 4 complete"

echo "=== All benchmarks complete ==="
```

**5. Documentation Updates:**
```bash
#!/bin/bash
# Documentation update script

echo "=== Updating Documentation ==="

# Generate API documentation
python generate_docs.py --output=api_docs.html

# Update user guides
generate_user_guides.py --update all

# Validate documentation links
validate_docs_links.py

# Check documentation completeness
check_documentation_completeness.py

echo "=== Documentation update complete ==="
```

---

## Troubleshooting Guide

### Enhanced Troubleshooting with New Error Messages

NLM now provides enhanced error messages and diagnostic information for troubleshooting.

#### Configuration Validation Error Diagnosis

**Error Types:**
- `CONFIG_INVALID_PARAMETER`: Invalid parameter value
- `CONFIG_MISSING_REQUIRED`: Missing required parameter
- `CONFIG_TYPE_MISMATCH`: Type validation failed
- `CONFIG_RANGE_VIOLATION`: Value outside valid range
- `CONFIG_DEPENDENCY_FAILED`: Dependent parameter validation failed

**Example Error Messages:**
```
Configuration validation failed:
  - Invalid parameter 'brain.neuron_count': value -1 is outside valid range [1, 1000000]
  - Missing required parameter 'plasticity.stdp.learning_rate'
  - Type mismatch for 'neuromodulation.dopamine.baseline': expected double, got string
  - Dependency failed for 'memory.working_memory_capacity': requires 'brain.neuron_count' to be > 1000
```

**Troubleshooting Steps:**
```bash
# Diagnose configuration issues
./nlm_cli config validate --schema brain --verbose config.json

# Get specific parameter validation
./nlm_cli config validate --param neuron_count --value 5000 --schema brain

# Get schema documentation
./nlm_cli config generate --schema brain --format=markdown > brain_schema.md
```

#### Performance Troubleshooting

**Common Performance Issues:**
- High memory usage
- Slow simulation steps
- Log file I/O bottlenecks
- Lock contention in multi-threaded environments

**Performance Diagnosis:**
```bash
# Enable performance monitoring
./nlm_cli monitor --interval=0.5 --metrics

# Check performance metrics
./nlm_cli log categories --performance

# Analyze log performance
./nlm_cli log export performance.json

# Performance analysis script
python analyze_performance.py performance.json
```

**Troubleshooting Commands:**
```bash
# Optimize for performance
./nlm_cli simulate --log-level=WARNING --log-async --log-sampling=0.1

# Debug performance issues
./nlm_cli simulate --log-level=DEBUG --log-system-metrics

# Monitor resource usage
./nlm_cli monitor --system --interval=1
```

#### Logging Troubleshooting

**Common Logging Issues:**
- Log files not created
- Log messages missing
- Performance degradation due to logging
- Async logging configuration problems

**Logging Diagnosis:**
```bash
# Check logging status
./nlm_cli log status

# Enable debug logging for troubleshooting
./nlm_cli log enable Debug
./nlm_cli log set-level Debug:TRACE

# Check log file permissions
ls -la nlm.log

# Monitor log performance
./nlm_cli monitor --metrics

# Clear log files for testing
./nlm_cli log export --current debug_config.json
./nlm_cli log disable all
rm -f nlm.log
./nlm_cli log config debug_config.json
```

**Solution Examples:**
```python
# Fix log file permissions
import os
import stat

# Check permissions
file_stat = os.stat("nlm.log")
if not file_stat.st_mode & stat.S_IWRITE:
    os.chmod("nlm.log", stat.S_IWRITE)
    print("Fixed log file permissions")

# Check disk space
import shutil
total, used, free = shutil.disk_usage(".")
if free < 100 * 1024 * 1024:  # Less than 100MB free
    print(f"Warning: Low disk space ({free / (1024*1024):.1f}MB free)")
    # Configure smaller log files
    pynlm.Logger.setLogConfig({"max_file_size": 5 * 1024 * 1024})

# Check for memory issues
import psutil
memory = psutil.virtual_memory()
if memory.available < 500 * 1024 * 1024:  # Less than 500MB available
    print(f"Warning: Low memory available ({memory.available / (1024*1024):.1f}MB)")
    # Disable memory-intensive features
    pynlm.Logger.setLogConfig({"log_system_metrics": False})
```

### Configuration Validation Error Diagnosis

**Error Classification:**

**1. Schema Validation Errors:**
- Missing required parameters
- Invalid parameter types
- Parameter value constraints violated
- Schema inheritance issues

**2. Configuration Loading Errors:**
- File not found
- Invalid file format
- Corrupted configuration files
- Permission issues

**3. Runtime Configuration Errors:**
- Invalid runtime parameter values
- Configuration conflicts
- Resource constraints violated

**Diagnosis Tools:**
```cpp
// Detailed error reporting
class ConfigDiagnostics {
public:
    static std::string diagnose(const std::string& configFile);
    static std::vector<std::string> validate(const std::map<std::string, std::any>& config);
    static std::string suggestFix(const std::string& error);
};

// Usage example
std::string diagnosis = ConfigDiagnostics::diagnose("config.json");
std::cout << diagnosis << std::endl;
```

### Performance Troubleshooting

**Performance Bottleneck Detection:**
```cpp
// Performance monitoring and diagnosis
class PerformanceProfiler {
public:
    void profileSimulation(const std::function<void()>& simulationFunction);
    void profileLogging(const std::function<void()>& loggingFunction);
    void profileConfiguration(const std::function<void()>& configFunction);
    std::string generatePerformanceReport() const;
};
```

**Common Performance Issues and Solutions:**

1. **Memory Issues:**
   - Problem: High memory usage during large simulations
   - Solution: Use memory-efficient configuration, disable unnecessary features

2. **CPU Issues:**
   - Problem: Slow simulation steps
   - Solution: Optimize configuration, enable SIMD optimizations

3. **I/O Issues:**
   - Problem: Log file I/O bottlenecks
   - Solution: Use async logging, configure appropriate file rotation

4. **Lock Contention:**
   - Problem: Thread contention in multi-threaded environments
   - Solution: Optimize async queue size, configure finer-grained locking

### Logging Troubleshooting

**Log Message Issues:**
- Missing log messages
- Incorrect log levels
- Category filtering problems
- Async logging issues

**Log File Issues:**
- Log files not created
- Log file permissions
- Log file rotation problems
- Log compression failures

**Log Performance Issues:**
- Slow log writes
- High memory usage from logging
- Performance degradation

**Diagnosis Commands:**
```bash
# Check current logging configuration
./nlm_cli log status

# Enable debug logging for troubleshooting
./nlm_cli log enable Debug
./nlm_cli log set-level Debug:TRACE

# Check log file properties
ls -la nlm.log
stat nlm.log

# Monitor log performance
./nlm_cli monitor --metrics

# Test log rotation
./nlm_cli log config test_rotation.json
./nlm_cli simulate --steps=1000
ls -la *.log*
```

**Solution Examples:**
```python
# Fix log file permissions
import os
import stat

# Check permissions
file_stat = os.stat("nlm.log")
if not file_stat.st_mode & stat.S_IWRITE:
    os.chmod("nlm.log", stat.S_IWRITE)
    print("Fixed log file permissions")

# Check disk space
import shutil
total, used, free = shutil.disk_usage('.')
if free < 100 * 1024 * 1024:  # Less than 100MB free
    print(f"Warning: Low disk space ({free / (1024*1024):.1f}MB free)")
    # Configure smaller log files
    pynlm.Logger.setLogConfig({"max_file_size": 5 * 1024 * 1024})

# Check for memory issues
import psutil
memory = psutil.virtual_memory()
if memory.available < 500 * 1024 * 1024:  # Less than 500MB available
    print(f"Warning: Low memory available ({memory.available / (1024*1024):.1f}MB)")
    # Disable memory-intensive features
    pynlm.Logger.setLogConfig({"log_system_metrics": False})
```

## Migration Guide

### Upgrading from Previous Versions

NLM v6.0.0 introduces significant enhancements while maintaining backward compatibility. This guide covers migration from previous versions.

#### Version Compatibility Matrix

| From Version | To Version | Breaking Changes | Migration Required |
|--------------|------------|------------------|-------------------|
| v5.2.0       | v6.0.0     | ✓                | Yes               |
| v4.1.0       | v6.0.0     | ✓                | Yes               |
| v3.0.0       | v6.0.0     | ✓                | Yes               |
| v2.5.0       | v6.0.0     | ✓                | Yes               |
| v1.0.0       | v6.0.0     | ✓                | Yes               |

#### Migration Requirements

**1. Configuration Files:**
- Previous `.cfg` format → New JSON/YAML/INI support
- Legacy parameters → New schema-based configuration
- Runtime settings → Enhanced configuration options

**2. Code Changes:**
- Old logging API → Enhanced logging API
- Basic configuration → Schema-based validation
- Simple simulation → Advanced simulation features

**3. Performance Considerations:**
- Default settings optimized for performance
- Enhanced logging with optional performance impact
- New memory management features

#### Migration Steps

**Step 1: Assess Current Configuration:**
```bash
# Analyze current configuration
./nlm_cli config analyze --current > analysis_report.txt

# Check for deprecated parameters
./nlm_cli config check --deprecated

# Get migration recommendations
./nlm_cli config migrate --from=v5.2.0 --to=v6.0.0 > migration_plan.txt
```

**Step 2: Update Configuration Files:**
```python
# Convert configuration files
import pynlm

# Load legacy configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("legacy_config.cfg")

# Convert to new format
config.saveToFile("legacy_config.json", "legacy")
config.saveToFile("legacy_config.yaml", "legacy")
config.saveToFile("legacy_config.ini", "legacy")

# Apply schema
config.registerSchema("legacy", legacy_schema)
config.validateSchema("legacy")
```

**Step 3: Update Code:**
```cpp
// Old code (v5.2.0)
nlm::Logger& logger = nlm::Logger::getInstance();
logger.setLevel(nlm::Logger::Level::Info);
logger.log("Simulation step completed");

// New code (v6.0.0) - fully backward compatible
nlm::Logger& logger = nlm::Logger::getGlobal();
logger.setLevel(nlm::Logger::Level::Info);
logger.log("Simulation step completed");

// Enhanced logging (new features)
nlm::LogConfig config;
config.globalLevel = nlm::EnhancedLogLevel::Info;
config.useEnhancedLevels = true;
config.useAsyncLogging = true;
logger.setLogConfig(config);

logger.trace("Detailed debug information");
logger.debug("Debug information");
logger.info("Informational message");
logger.warning("Warning message");
logger.error("Error message");
logger.critical("Critical error");
logger.fatal("Fatal error");
```

**Step 4: Test Migration:**
```bash
# Run integration tests
./nlm_phase6_demo

# Run enhanced logging demo
./nlm_phase6_enhanced_demo

# Run configuration validation tests
./nlm_test --gtest_filter="*config*"
```

### Configuration Migration

**Migration from Legacy .cfg Files:**

**Legacy Format (v5.2.0):**
```ini
# legacy_config.cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
```

**New Format (v6.0.0):**
```json
{
  "legacy": {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01
  },
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "region_count": 2,
    "connection_probability": 0.1
  },
  "plasticity": {
    "stdp": {"learning_rate": 0.01},
    "hebbian": {"learning_rate": 0.0005}
  }
}
```

**Migration Process:**
```cpp
// Migration helper function
void migrateLegacyConfig(const std::string& legacyPath, const std::string& newPath) {
    Config legacyConfig;
    legacyConfig.loadFromFile(legacyPath);
    
    // Extract legacy parameters
    auto random_seed = legacyConfig.getOr("random_seed", 42);
    auto simulation_timestep = legacyConfig.getOr("simulation_timestep", 0.001);
    auto neuron_count = legacyConfig.getOr("neuron_count", 1000);
    auto region_count = legacyConfig.getOr("region_count", 2);
    auto connection_probability = legacyConfig.getOr("connection_probability", 0.1);
    auto plasticity_learning_rate = legacyConfig.getOr("plasticity_learning_rate", 0.01);
    
    // Create new configuration
    Config newConfig;
    
    // Legacy category (backward compatibility)
    newConfig.set("legacy.random_seed", random_seed);
    newConfig.set("legacy.simulation_timestep", simulation_timestep);
    newConfig.set("legacy.neuron_count", neuron_count);
    newConfig.set("legacy.region_count", region_count);
    newConfig.set("legacy.connection_probability", connection_probability);
    newConfig.set("legacy.plasticity_learning_rate", plasticity_learning_rate);
    
    // New categories
    newConfig.set("brain.neuron_count", neuron_count);
    newConfig.set("brain.synapse_density", 0.1);
    newConfig.set("brain.region_count", region_count);
    newConfig.set("brain.connection_probability", connection_probability);
    
    newConfig.set("plasticity.stdp.learning_rate", plasticity_learning_rate);
    newConfig.set("plasticity.hebbian.learning_rate", 0.0005);
    
    // Save new configuration
    newConfig.saveToFile(newPath);
}
```

**Automatic Migration:**
```python
import pynlm
import json
import yaml

def auto_migrate_config(input_file, output_file):
    """Automatically migrate configuration file"""
    
    # Detect file format
    if input_file.endswith('.cfg'):
        # Legacy .cfg format
        config = pynlm.createDefaultConfig()
        config.loadFromFile(input_file)
        
        # Convert to new format
        migrated = {
            "legacy": {},
            "brain": {},
            "plasticity": {},
            "neuromodulation": {},
            "memory": {},
            "development": {}
        }
        
        # Copy legacy parameters
        migrated["legacy"]["random_seed"] = config.getOr("random_seed", 42)
        migrated["legacy"]["simulation_timestep"] = config.getOr("simulation_timestep", 0.001)
        migrated["legacy"]["neuron_count"] = config.getOr("neuron_count", 1000)
        migrated["legacy"]["region_count"] = config.getOr("region_count", 2)
        migrated["legacy"]["connection_probability"] = config.getOr("connection_probability", 0.1)
        migrated["legacy"]["plasticity_learning_rate"] = config.getOr("plasticity_learning_rate", 0.01)
        
        # Add new parameters
        migrated["brain"]["neuron_count"] = migrated["legacy"]["neuron_count"]
        migrated["brain"]["synapse_density"] = 0.1
        migrated["brain"]["region_count"] = migrated["legacy"]["region_count"]
        migrated["brain"]["connection_probability"] = migrated["legacy"]["connection_probability"]
        
        migrated["plasticity"]["stdp"] = {"learning_rate": migrated["legacy"]["plasticity_learning_rate"]}
        migrated["plasticity"]["hebbian"] = {"learning_rate": 0.0005}
        
        # Save in multiple formats
        with open(output_file + ".json", 'w') as f:
            json.dump(migrated, f, indent=2)
        
        with open(output_file + ".yaml", 'w') as f:
            yaml.dump(migrated, f)
            
        print(f"Configuration migrated to {output_file}.json and {output_file}.yaml")
        
    else:
        print(f"Unsupported file format: {input_file}")
        return False
    
    return True
```

### API Compatibility Guide

**Backward Compatibility Matrix:**

| Old API (v5.2.0)                | New API (v6.0.0)                   | Status          |
|--------------------------------|-----------------------------------|-----------------| |
| `Logger::getInstance()`         | `Logger::getGlobal()`              | ✅ Compatible   |
| `Logger::setLevel()`            | `Logger::setLogConfig()`           | ✅ Compatible   |
| `Logger::log()`                 | `Logger::logInfo()`                | ✅ Compatible   |
| `Config::loadFromFile()`        | `Config::loadFromFile()`           | ✅ Compatible   |
| `Config::saveToFile()`          | `Config::saveToFile()`             | ✅ Compatible   |
| `Config::has()`                 | `Config::has()`                    | ✅ Compatible   |
| `Config::get()`                 | `Config::get()`                    | ✅ Compatible   |
| `Config::set()`                 | `Config::set()`                    | ✅ Compatible   |
| `Brain::create()`               | `pynlm.createBrain()`              | ✅ Compatible   |
| `Agent::create()`               | `pynlm.createAgentBrain()`         | ✅ Compatible   |
| `World::create()`               | `pynlm.createSimpleWorld()`        | ✅ Compatible   |

**Migration Examples:**

**C++ API Migration:**
```cpp
// Old API (v5.2.0)
nlm::Logger& logger = nlm::Logger::getInstance();
logger.setLevel(nlm::Logger::Level::Info);
logger.log("Simulation step completed");

// New API (v6.0.0) - fully backward compatible
nlm::Logger& logger = nlm::Logger::getGlobal();
logger.setLevel(nlm::Logger::Level::Info);
logger.log("Simulation step completed");

// Enhanced logging (new features)
nlm::LogConfig config;
config.globalLevel = nlm::EnhancedLogLevel::Info;
config.useEnhancedLevels = true;
config.useAsyncLogging = true;
logger.setLogConfig(config);

logger.trace("Detailed debug information");
logger.debug("Debug information");
logger.info("Informational message");
logger.warning("Warning message");
logger.error("Error message");
logger.critical("Critical error");
logger.fatal("Fatal error");
```

**Python API Migration:**
```python
# Old API (v5.2.0)
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Old logging API
pynlm.Logger.log("Simulation step completed")

# New API (v6.0.0) - fully backward compatible
import pynlm

# Create and initialize brain with enhanced features
config = pynlm.createDefaultConfig()
config.set("log_level", "INFO", reason="Enable enhanced logging")
config.set("log_async", True, reason="Enable async logging")
config.set("log_system_metrics", True, reason="Enable system metrics")

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with enhanced logging
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        # Enhanced logging (new features)
        pynlm.Logger.logInfo(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        pynlm.Logger.logStructured("Brain Statistics", {
            "step": step,
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_neurons": brain.getTotalNeuronCount(),
            "firing_rate": brain.getAverageFiringRate()
        })

# Backward compatible logging (still works)
pynlm.Logger.log("Simulation step completed")
```

**CLI Migration:**
```bash
# Old CLI (v5.2.0) - limited options
./nlm_cli simulate --steps=1000 --brain=1000

# New CLI (v6.0.0) - comprehensive features
./nlm_cli simulate \
  --steps=1000 \
  --brain=1000 \
  --log-level=INFO \
  --log-async \
  --enable-curiosity \
  --enable-development \
  --enable-reward \
  --category=Brain \
  --category=Memory \
  --log-system-metrics
```

**Configuration Migration:**
```cpp
// Old configuration (v5.2.0)
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

// New configuration (v6.0.0) - backward compatible
{
  "legacy": {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
  },
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "region_count": 2,
    "connection_probability": 0.1
  },
  "plasticity": {
    "stdp": {"learning_rate": 0.01},
    "hebbian": {"learning_rate": 0.0005}
  }
}
```

### Migration Examples

**Complete Migration Example:**
```cpp
// Example: Complete migration from v5.2.0 to v6.0.0

// Step 1: Include new headers
#include "core/Logger/Logger.hpp"
#include "core/Logger/EnhancedLogger.hpp"
#include "core/Config/Config.hpp"

// Step 2: Update logging configuration
auto& logger = nlm::Logger::getGlobal();

nlm::LogConfig config;
config.globalLevel = nlm::EnhancedLogLevel::Info;
config.useEnhancedLevels = true;
config.useAsyncLogging = true;
config.logFile = "nlm.log";
config.logSystemMetrics = true;
config.logPerformanceMetrics = true;

// Enable categories
logger.enableCategory("Brain", true);
logger.enableCategory("Memory", true);
logger.enableCategory("Neuromodulation", true);
logger.enableCategory("Cognition", true);
logger.enableCategory("Development", true);
logger.enableCategory("Performance", true);
logger.enableCategory("System", true);

logger.setLogConfig(config);

// Step 3: Update configuration management
nlm::Config config;

// Register enhanced schema
auto brainSchema = std::make_shared<nlm::ConfigSchema>("brain", "Brain configuration");

nlm::ParameterMetadata neuronCount("neuron_count", "int", 
    "Number of neurons (1-1000000)", true, std::any(1000));
neuronCount.hasMin = true; neuronCount.minValue = 1;
neuronCount.hasMax = true; neuronCount.maxValue = 1000000;
brainSchema->addParameter(neuronCount);

config.registerSchema("brain", brainSchema);

// Load enhanced configuration
config.loadFromFile("enhanced_config.json");

// Validate configuration
auto validation = config.validateSchema("brain");
if (validation.valid) {
    std::cout << "Configuration is valid!" << std::endl;
} else {
    std::cout << "Configuration errors:" << std::endl;
    for (const auto& error : validation.errors) {
        std::cout << "  - " << error << std::endl;
    }
}

// Step 4: Update brain creation
nlm::Brain brain(config);
brain.initialize();

// Step 5: Update simulation loop
for (int step = 0; step < 1000; ++step) {
    brain.step(step);
    
    if (step % 100 == 0) {
        // Enhanced logging
        logger.logInfo("Step " + std::to_string(step) + 
                      ": " + std::to_string(brain.getFiringNeuronCount()) + 
                      " firing neurons");
        
        // Structured logging
        nlm::EnhancedLogEntry entry;
        entry.level = nlm::EnhancedLogLevel::Info;
        entry.category = "Brain";
        entry.message = "Processing completed";
        entry.fields["neuron_count"] = std::to_string(brain.getTotalNeuronCount());
        entry.fields["firing_rate"] = std::to_string(brain.getAverageFiringRate());
        
        logger.logEnhanced(entry);
    }
}

// Step 6: Update agent and world
nlm::AgentBrain agent(brain);
nlm::SimpleWorld world;
world.configure(20, 20, 8, 8);
world.reset();
agent.initialize(world);

// Run enhanced simulation
for (int step = 0; step < 1000; ++step) {
    world.update(0.1);
    
    auto percept = world.getSensoryPercept();
    agent.processSensoryInput(percept);
    brain.step(step);
    
    auto action = agent.decodeMotorCommand();
    world.applyMotorCommand(action, world.getSimulationTime());
    
    if (step % 100 == 0) {
        std::cout << "Step " << step << ": "
                  << "Firing: " << brain.getFiringNeuronCount() << ", "
                  << "Curiosity: " << agent.getCuriosityLevel() << ", "
                  << "Novelty: " << agent.getNoveltyLevel() << std::endl;
    }
}

std::cout << "Enhanced simulation completed successfully!" << std::endl;
```

**Python Migration Example:**
```python
# Example: Complete migration from v5.2.0 to v6.0.0

import pynlm

# Step 1: Create enhanced configuration
config = pynlm.createDefaultConfig()

# Enable enhanced logging
config.set("log_level", "INFO", reason="Enable enhanced logging")
config.set("log_async", True, reason="Enable async logging")
config.set("log_system_metrics", True, reason="Enable system metrics")
config.set("log_performance_metrics", True, reason="Enable performance metrics")
config.set("brain.neuron_count", 2000, reason="Configure for enhanced simulation")

# Create enhanced brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create enhanced agent
agent = pynlm.createAgentBrain(brain)

# Create enhanced world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize enhanced agent
agent.initialize(world)

# Enable all enhanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Step 2: Run enhanced simulation
print("=== Enhanced Simulation (v6.0.0) ===")

for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in agent
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev stage: {brain.getDevelopmentalStage()}")
        
        # Enhanced structured logging
        pynlm.Logger.logStructured("Brain Statistics", {
            "step": step,
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_neurons": brain.getTotalNeuronCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "development_stage": str(brain.getDevelopmentalStage())
        })

print("\n=== Enhanced simulation completed successfully! ===")

# Step 3: Backward compatibility check
print("\n=== Backward Compatibility Check ===")

# Old API still works (backward compatibility)
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
for step in range(100):
    brain2.step(step)

print("✓ Backward compatibility maintained - all old APIs still work")

# Step 4: Migration summary
print("\n=== Migration Summary ===")
print("✓ Enhanced logging infrastructure added")
print("✓ Configuration validation implemented")
print("✓ Schema-based configuration added")
print("✓ Async logging for performance")
print("✓ Structured logging with JSON support")
print("✓ Performance monitoring added")
print("✓ Backward compatibility maintained")
print("✓ New Python features added")
print("✓ Advanced CLI tool added")
print("✓ Comprehensive documentation added")
```

**Migration Checklist:**

**Configuration Files:**
- [ ] Convert `.cfg` files to JSON/YAML/INI format
- [ ] Add schema definitions for new parameters
- [ ] Test configuration validation
- [ ] Update configuration documentation

**Code Changes:**
- [ ] Update logging configuration to use enhanced features
- [ ] Add configuration validation where needed
- [ ] Test backward compatibility
- [ ] Update API documentation

**Testing:**
- [ ] Run existing tests to verify compatibility
- [ ] Test new features with unit tests
- [ ] Test enhanced logging with integration tests
- [ ] Test configuration validation with schema tests
- [ ] Test performance with benchmarking

**Documentation:**
- [ ] Update README.md with new features
- [ ] Update HOW_TO_USE.md with enhanced API
- [ ] Update easy_usage.md with new examples
- [ ] Add migration guide
- [ ] Add troubleshooting guide

This migration guide provides comprehensive guidance for upgrading from previous versions of NLM to v6.0.0, ensuring backward compatibility while leveraging all new features and capabilities.
