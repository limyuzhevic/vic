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

## Part 5: Advanced CLI and Configuration Management

### Advanced CLI Overview

The NLM Advanced CLI provides comprehensive command-line interfaces for power users working with the NLM brain simulation system. It includes:

- **Advanced Configuration Management**: Import/export configurations in multiple formats
- **Enhanced Simulation Control**: Custom simulation parameters and real-time monitoring
- **Power User Features**: Advanced debugging and performance profiling
- **Brain State Management**: Direct manipulation and analysis

#### Basic CLI Commands

```bash
# Show help information
nlm-cli --help

# Show version information
nlm-cli --version

# Enable verbose output
nlm-cli --verbose

# Suppress output
nlm-cli --quiet
```

#### Configuration Management

```bash
# Export configuration to file
nlm-cli config export --file config.json --format json
nlm-cli config export --file config.yaml --format yaml
nlm-cli config export --file config.txt --format table

# Import configuration from file
nlm-cli config import --file config.json --format json
nlm-cli config import --file config.yaml --format yaml

# Validate configuration schema
nlm-cli config validate

# Compare configuration versions
nlm-cli config diff --source current --target default

# List all configuration keys
nlm-cli config list
```

#### Simulation Control

```bash
# Run custom simulation
nlm-cli simulate custom --timestep 0.001 --neurons 1000 --regions 2 --plasticity

# Run in real-time with monitoring
nlm-cli simulate realtime --speed 2.0 --monitoring --checkpoint-interval 100

# Start simulation monitoring
nlm-cli simulate monitor --log-level INFO --metrics --performance

# Create checkpoint
nlm-cli simulate checkpoint --directory checkpoints --compress --keep 5

# Restart from checkpoint
nlm-cli simulate restart --checkpoint checkpoints/brain_checkpoint_20250101_120000.bin

# Configure learning rate schedule
nlm-cli simulate schedule --type exponential_decay --initial 0.01 --decay 0.001 --steps 1000
```

#### Power User Features

```bash
# Advanced debugging
nlm-cli debug advanced --neuron-debug --synapse-monitor --memory-leak

# Performance profiling
nlm-cli profiler start --output profile.json --duration 60 --detailed
nlm-cli profiler report --input profile.json --format html

# Neuron-by-neuron debugging
nlm-cli neuron debug --id 1 --show-potential --show-spikes --show-synapses
nlm-cli neuron debug --population sensory --show-activations --history-size 100

# Synapse-level monitoring
nlm-cli synapse monitor --show-weights --plasticity-events --delays
nlm-cli synapse monitor --filter type excitatory --history 50

# Network-wide statistics
nlm-cli network stats --summary --detailed --performance
```

#### Brain State Management

```bash
# Manual brain state manipulation
nlm-cli state manual --command inject --neuron-id 1 --current 50.0
nlm-cli state manual --command set-weight --source 1 --dest 2 --weight 0.5
nlm-cli state manual --command set-development-stage --stage maturation

# Pattern injection for testing
nlm-cli state pattern --type pre-post --pre-neuron 1 --post-neuron 2 --repetitions 10
nlm-cli state pattern --type random --neurons 20 --current-range 10-50 --steps 100

# Network topology exploration
nlm-cli state topology --measure clustering --measure shortest-path
nlm-cli state topology --identify hubs --threshold 0.8

# Weight visualization
nlm-cli state visualize --type network --output network_graph.png
nlm-cli state visualize --type distribution --output weight_histogram.png

# Brain state analysis
nlm-cli state analysis --complexity --information-bottleneck --generalization
```

### Configuration File Formats

#### JSON Format (`config.json`)

```json
{
  "neuron_count": 1000,
  "region_count": 1,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "stdp_tau": 20.0,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "connection_probability": 0.15,
  "replay_interval": 100,
  "consolidation_interval": 1000
}
```

#### YAML Format (`config.yaml`)

```yaml
# NLM Configuration
neuron_count: 1000
region_count: 1
simulation_timestep: 0.001
random_seed: 42

# Plasticity Parameters
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
stdp_tau: 20.0
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001

# Network Properties
connection_probability: 0.15
replay_interval: 100
consolidation_interval: 1000
```

#### Table Format (`config.txt`)

```text
# Configuration File
# Format: key = value
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001
random_seed = 42
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
connection_probability = 0.15
replay_interval = 100
consolidation_interval = 1000
```

### Advanced Configuration Options

#### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of neural regions |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |

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

### Performance Optimization

#### Large-Scale Simulation Tips

1. **Use checkpointing for long runs**:
   ```bash
   # Create periodic checkpoints
   nlm-cli simulate checkpoint --directory checkpoints --compress --keep 10
   
   # Use checkpoint for restart
   nlm-cli simulate restart --checkpoint checkpoints/brain_checkpoint.bin
   ```

2. **Monitor performance during simulation**:
   ```bash
   # Start monitoring with performance metrics
   nlm-cli simulate monitor --performance --metrics --log-level INFO
   ```

3. **Use parallel processing**:
   ```bash
   # Run simulation with multiple threads
   nlm-cli simulate custom --parallel --num-threads 8 --neurons 5000
   ```

4. **Optimize memory usage**:
   ```bash
   # Clean up old checkpoints
   nlm-cli simulate checkpoint --directory checkpoints --keep 5
   ```

### Advanced Usage Examples

#### Example 1: Complete Development Workflow

```bash
#!/bin/bash
# Complete workflow for NLM experiment with monitoring

set -e

EXPERIMENT_NAME="nlm_experiment_$(date +%Y%m%d_%H%M%S)"
CHECKPOINT_DIR="checkpoints/${EXPERIMENT_NAME}"
PROFILE_DIR="profiles/${EXPERIMENT_NAME}"

# 1. Initialize with configuration
nlm-cli config import --file config.yaml --format yaml
nlm-cli config validate

# 2. Run experiment with monitoring
mkdir -p "${CHECKPOINT_DIR}"
mkdir -p "${PROFILE_DIR}"

echo "Starting experiment with monitoring..."
nlm-cli simulate realtime --speed 1.0 --monitoring --checkpoint-interval 1000

# 3. Analyze results
echo "Analyzing experiment results..."
nlm-cli profiler start --output "${PROFILE_DIR}/profile.json" --duration 60 --detailed
nlm-cli network stats --detailed
nlm-cli state analysis --complexity --generalization

# 4. Export final configuration
nlm-cli config export --file "${EXPERIMENT_NAME}_final_config.json" --format json

echo "Experiment completed successfully! Results in: ${EXPERIMENT_NAME}"
```

#### Example 2: Production Environment Setup

```bash
#!/bin/bash
# Production workflow for NLM simulation

set -e

# 1. Run simulation with production settings
nlm-cli simulate custom --timestep 0.0005 --neurons 5000 --regions 3

# 2. Enable comprehensive monitoring
nlm-cli simulate monitor --log-level INFO --metrics --performance

# 3. Create periodic checkpoints
nlm-cli simulate checkpoint --directory prod_checkpoints --compress --keep 10

# 4. Analyze production results
nlm-cli network stats --summary --detailed
nlm-cli state analysis --complexity --generalization

# 5. Validate results
nlm-cli config validate

# 6. Export production configuration
nlm-cli config export --file production_config.json --format json
```

### Integration with External Tools

#### Python Integration

```python
#!/usr/bin/env python3
# Python script to integrate with NLM Advanced CLI

import subprocess
import json
import time

def run_nlm_cli_command(command):
    """Run NLM CLI command and return output"""
    try:
        result = subprocess.run(
            command.split(),
            capture_output=True,
            text=True,
            check=True
        )
        return result.stdout, result.stderr, result.returncode
    except subprocess.CalledProcessError as e:
        return e.stdout, e.stderr, e.returncode

def run_complete_experiment(config_file):
    """Run complete NLM experiment with monitoring"""
    
    # 1. Import configuration
    stdout, stderr, code = run_nlm_cli_command(
        f"nlm-cli config import --file {config_file} --format yaml"
    )
    if code != 0:
        print(f"Configuration import failed: {stderr}")
        return False
    
    # 2. Validate configuration
    stdout, stderr, code = run_nlm_cli_command("nlm-cli config validate")
    if code != 0:
        print(f"Configuration validation failed: {stderr}")
        return False
    
    # 3. Run simulation with profiling
    print("Starting simulation with profiling...")
    profile_proc = subprocess.Popen(
        "nlm-cli profiler start --output experiment_profile.json --duration 300 --detailed",
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    
    # 4. Monitor simulation
    monitor_proc = subprocess.Popen(
        "nlm-cli simulate monitor --performance --metrics",
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    
    # 5. Wait for completion
    profile_proc.wait()
    monitor_proc.terminate()
    
    # 6. Analyze results
    stdout, stderr, code = run_nlm_cli_command(
        "nlm-cli network stats --detailed"
    )
    if code == 0:
        print("Network statistics:")
        print(stdout)
    
    return True

# Example usage
if __name__ == "__main__":
    success = run_complete_experiment("config.yaml")
    if success:
        print("Experiment completed successfully!")
    else:
        print("Experiment failed!")
```

### Error Handling

#### Common Errors and Solutions

**Configuration Errors:**
- **Symptom**: Configuration validation fails
- **Solution**: Check required keys and value types in configuration
- **Command**: `nlm-cli config validate --fix`

**Brain Initialization Errors:**
- **Symptom**: "Failed to initialize brain"
- **Solution**: Check system resources and configuration compatibility
- **Command**: `nlm-cli brain info` to check system status

**Memory Issues:**
- **Symptom**: Simulation crashes with memory error
- **Solution**: Reduce neuron count, increase checkpoint frequency
- **Command**: `nlm-cli simulate checkpoint --keep 3`

**Performance Issues:**
- **Symptom**: Slow simulation performance
- **Solution**: Use checkpointing, increase threads, optimize configuration
- **Command**: `nlm-cli profiler start --output perf.json --duration 30`

#### Error Codes

The CLI uses the following error codes:
- `0`: Success
- `1`: General error
- `2`: Configuration error
- `3`: Brain initialization error
- `4`: Simulation error
- `5`: Debug error
- `6`: Profiling error
- `7`: State management error
- `8`: I/O error

### Environment Configuration

Set environment variables for advanced CLI usage:

```bash
# Set configuration file location
export NLM_CONFIG_FILE=config.yaml

# Set log level
export NLM_LOG_LEVEL=DEBUG

# Set log file
export NLM_LOG_FILE=/var/log/nlm.log

# Set profiling output directory
export NLM_PROFILE_DIR=/opt/nlm/profiles

# Set checkpoint directory
export NLM_CHECKPOINT_DIR=/opt/nlm/checkpoints
```

### Configuration File Locations

The Advanced CLI looks for configuration files in the following order:

1. Current directory (`./config.json`, `./config.yaml`, `./config.txt`)
2. Home directory (`~/.config/nlm/config.json`)
3. Environment variable (`$NLM_CONFIG_FILE`)
4. Compiled-in defaults

## Further Reading

- [AdvancedCLI.md](AdvancedCLI.md) - Advanced CLI documentation and examples
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
