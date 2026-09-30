# NLM Troubleshooting Guide

## Overview

This document provides comprehensive troubleshooting guidance for the NLM Python bindings (`pynlm`). It covers common installation issues, runtime problems, performance bottlenecks, and solutions for various error scenarios.

## Table of Contents

1. [Installation Issues](#installation-issues)
   - Python Import Errors
   - Build Failures
   - Missing Dependencies
   - Permission Problems
2. [Runtime Issues](#runtime-issues)
   - Brain Initialization Errors
   - Simulation Failures
   - Memory Issues
   - Performance Problems
3. [Configuration Problems](#configuration-problems)
   - Invalid Settings
   - Parameter Conflicts
   - Configuration Loading/Saving
4. [World and Agent Issues](#world-and-agent-issues)
   - World Configuration
   - Agent Initialization
   - Sensory Processing
   - Motor Command Execution
5. [Development and Learning Issues](#development-and-learning-issues)
   - Plasticity Problems
   - Neuromodulation Errors
   - Developmental Stage Issues
6. [Advanced Troubleshooting](#advanced-troubleshooting)
   - Memory Profiling
   - Performance Analysis
   - Debugging Techniques
7. [Getting Help](#getting-help)

---

## Installation Issues

### Python Import Errors

**Problem:** `ImportError: No module named 'pynlm'`

**Symptoms:**
- Cannot import the pynlm module in Python
- Python suggests checking Python path
- Module not found in site-packages

**Solutions:**

#### Solution 1: Verify Installation

```bash
# Check if pynlm is installed
python3 -c "import pynlm; print('pynlm imported successfully')"

# List installed packages
pip list | grep pynlm

# Check installation location
python3 -c "import pynlm; print(pynlm.__file__)"
```

#### Solution 2: Reinstall Python Bindings

```bash
# Uninstall and reinstall
pip uninstall pynlm
pip install -e .  # Development mode
# OR
pip install .     # Regular install

# Verify installation
python3 -c "import pynlm; print('pynlm version:', pynlm.__version__)"
```

#### Solution 3: Check Python Environment

```bash
# Check Python version and paths
python3 --version
python3 -c "import sys; print('Python path:'); [print(p) for p in sys.path]"

# Check if in virtual environment
python3 -c "import sys; print('In virtual environment:', 'venv' in sys.prefix or 'virtualenv' in sys.prefix)"

# Activate virtual environment if needed
source venv/bin/activate  # For virtual environments
```

#### Solution 4: Build Directory Issues

```bash
# Check build directory
ls -la build/

# Rebuild if necessary
rm -rf build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ..
pip install -e .
```

### Build Failures

**Problem:** CMake build fails with compilation errors

**Symptoms:**
- CMake configuration errors
- Compiler warnings/errors during build
- Missing include files
- Library linking issues

**Solutions:**

#### Solution 1: Check CMake Configuration

```bash
# Create fresh build directory
rm -rf build
mkdir build
cd build

# Run CMake with verbose output
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON

# Check for specific errors
make VERBOSE=1 2>&1 | grep -i "error" | head -20
```

#### Solution 2: Install Missing Dependencies

```bash
# Common dependencies for Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    python3-dev \
    python3-pip \
    libpython3-dev \
    libboost-dev \
    libeigen3-dev

# Common dependencies for CentOS/RHEL
sudo yum groupinstall -y "Development Tools"
sudo yum install -y \
    cmake3 \
    python3-devel \
    python3-pip \
    python3-devel
```

#### Solution 3: Compiler Issues

**Problem:** Compiler too old or incompatible

**Solutions:**

```bash
# Check compiler version
g++ --version

# Install newer compiler (Ubuntu)
sudo apt-get install g++-12
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

# Set compiler for CMake
cmake .. -DCMAKE_CXX_COMPILER=g++-12
```

#### Solution 4: Memory Issues During Build

**Problem:** Build process runs out of memory

**Solutions:**

```bash
# Build with fewer parallel jobs
make -j4  # Instead of -j$(nproc)

# Use swap space if necessary
sudo fallocate -l 2G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile

# Monitor memory usage
watch -n 5 free -h
```

#### Solution 5: Permission Issues

**Problem:** Permission denied during installation

**Solutions:**

```bash
# Use virtual environment (recommended)
python3 -m venv nlm_env
source nlm_env/bin/activate
pip install -e .

# Or install with user flag
pip install --user -e .

# Check file permissions
ls -la $(python3 -c "import site; print(site.getsitepackages()[0])")
```

### Missing Dependencies

**Problem:** Required Python packages not installed

**Symptoms:**
- `ModuleNotFoundError: No module named 'pybind11'`
- `ModuleNotFoundError: No module named 'scikit-build-core'`
- Missing development dependencies

**Solutions:**

```bash
# Install required Python packages
pip install pybind11 scikit-build-core pytest numpy

# For development, install all dependencies
pip install -e .[dev]

# Check installed packages
pip list
```

### Permission Problems

**Problem:** Permission denied accessing files or directories

**Solutions:**

```bash
# Check file permissions
ls -la /path/to/nlm

# Fix permissions (if needed)
chmod -R 755 .
chmod -R 644 *.py *.cpp *.h

# Use sudo for system-wide installation (not recommended)
sudo pip install -e .
```

## Runtime Issues

### Brain Initialization Errors

**Problem:** Brain initialization fails

**Symptoms:**
- `RuntimeError: Brain not initialized` when calling `step()`
- Configuration validation errors
- Memory allocation failures

**Solutions:**

#### Solution 1: Check Initialization Order

```python
import pynlm

# Correct initialization order
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Always call initialize before any operations
brain.initialize()

# Now you can safely call step()
brain.step(0)
```

#### Solution 2: Configuration Validation

```python
import pynlm

# Test configuration
config = pynlm.createDefaultConfig()

# Validate configuration keys
if config.has("brain.neuron_count"):
    neurons = config.get("brain.neuron_count")
    print(f"Current neuron count: {neurons}")
else:
    print("brain.neuron_count not set, using default")

# Check configuration summary
print(config.summary())
```

#### Solution 3: Memory Issues

**Problem:** Memory allocation failures

**Solutions:**

```python
import pynlm
import gc

# Start with smaller configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)  # Start small

brain = pynlm.createBrain(config)
brain.initialize()

# Gradually increase size
for neurons in [100, 500, 1000, 2000]:
    config.set("brain.neuron_count", neurons)
    brain = pynlm.createBrain(config)  # Recreate with new config
    brain.initialize()
    brain.step(0)
    print(f"Successfully tested with {neurons} neurons")

# Force garbage collection
gc.collect()
```

### Simulation Failures

**Problem:** Simulation fails during execution

**Symptoms:**
- Unexpected crashes during simulation
- Zero spikes despite valid configuration
- Incorrect state transitions

**Solutions:**

#### Solution 1: Step-by-Step Testing

```python
import pynlm

# Test incremental simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Test single steps
for i in range(10):
    brain.step(i)
    print(f"Step {i}: {brain.getTotalSpikeCount()} spikes")
    
    if brain.getTotalSpikeCount() == 0:
        print(f"Warning: No spikes at step {i}")
```

#### Solution 2: Debug with Logging

```python
import pynlm
import logging

# Enable logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger('pynlm')

# Test with logging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(20):
    logger.debug(f"Starting step {step}")
    brain.step(step)
    logger.debug(f"Completed step {step}: {brain.getFiringNeuronCount()} active neurons")
```

#### Solution 3: Check for Common Issues

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check brain state
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Test with different configurations
configs_to_test = [
    {"brain.neuron_count": 100},
    {"brain.neuron_count": 500},
    {"brain.neuron_count": 1000}
]

for test_config in configs_to_test:
    test_brain = pynlm.createBrain(config)
    test_brain.initialize()
    print(f"\nTest with {test_config['brain.neuron_count']} neurons:")
    print(f"  Total neurons: {test_brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {test_brain.getTotalSynapseCount()}")
```

### Memory Issues

**Problem:** Memory usage problems during runtime

**Symptoms:**
- High memory usage
- Memory leaks
- Out of memory errors

**Solutions:**

#### Solution 1: Monitor Memory Usage

```python
import pynlm
import psutil
import os

process = psutil.Process(os.getpid())

def monitor_memory(duration=60):
    """Monitor memory usage during simulation."""
    print("Monitoring memory usage...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(duration):
        brain.step(step)
        
        if step % 10 == 0:
            mem_mb = process.memory_info().rss / 1024 / 1024
            print(f"Step {step}: Memory usage = {mem_mb:.1f} MB")
            print(f"  Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")

# Run memory monitoring
monitor_memory(60)
```

#### Solution 2: Use Checkpointing

```python
import pynlm
import tempfile

# Create temporary directory for checkpoints
temp_dir = tempfile.mkdtemp()
print(f"Using temporary directory: {temp_dir}")

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with checkpointing
for step in range(1000):
    brain.step(step)
    
    # Save checkpoint periodically
    if step % 100 == 0:
        checkpoint_file = f"{temp_dir}/checkpoint_{step}.bin"
        brain.save(checkpoint_file)
        print(f"Checkpoint saved: {checkpoint_file}")
        
        # Check memory usage
        import psutil
        process = psutil.Process()
        mem_mb = process.memory_info().rss / 1024 / 1024
        print(f"  Memory usage: {mem_mb:.1f} MB")

# Clean up
import shutil
shutil.rmtree(temp_dir)
```

#### Solution 3: Memory Optimization

```python
import pynlm
import gc

# Optimize memory usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with memory optimization
for step in range(500):
    brain.step(step)
    
    # Force garbage collection periodically
    if step % 50 == 0:
        gc.collect()
        print(f"Step {step}: Memory optimized")

# Clear large objects
del brain
gc.collect()
```

### Performance Problems

**Problem:** Simulation performance issues

**Symptoms:**
- Slow simulation steps
- Low steps per second
- Bottlenecks in specific operations

**Solutions:**

#### Solution 1: Profile Performance

```python
import pynlm
import time
import cProfile
import pstats

# Profile simulation performance
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Profile simulation
pr = cProfile.Profile()
pr.enable()

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

pr.disable()

# Print profiling results
stats = pstats.Stats(pr)
stats.sort_stats('cumulative')
stats.print_stats(20)  # Top 20 functions
```

#### Solution 2: Optimize Configuration

```python
import pynlm

# Test different configurations
configs = [
    {"name": "Minimal", "brain.neuron_count": 100, "plasticity.stdp.enable": False},
    {"name": "Balanced", "brain.neuron_count": 1000, "plasticity.stdp.enable": True},
    {"name": "High Performance", "brain.neuron_count": 2000, "plasticity.stdp.enable": True, "plasticity.hebbian.enable": True}
]

for test_config in configs:
    config = pynlm.createDefaultConfig()
    for key, value in test_config.items():
        if key != "name":
            config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for step in range(50):
        brain.step(step)
    end_time = time.time()
    
    print(f"{test_config['name']}:")
    print(f"  Time: {end_time - start_time:.3f}s")
    print(f"  Steps/sec: {50 / (end_time - start_time):.1f}")
    print(f"  Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
```

## Configuration Problems

### Invalid Settings

**Problem:** Invalid configuration settings

**Symptoms:**
- Configuration validation errors
- Ignored settings
- Unexpected default values

**Solutions:**

#### Solution 1: Configuration Validation

```python
import pynlm

# Test configuration settings
config = pynlm.createDefaultConfig()

# Valid settings
test_settings = [
    ("brain.neuron_count", 1000, int),
    ("brain.synapse_density", 0.1, float),
    ("plasticity.stdp.enable", True, bool),
    ("neuromod.dopamine.scale", 1.0, float),
    ("development.critical_period_duration", 100.0, float)
]

for key, expected_value, expected_type in test_settings:
    # Set configuration
    config.set(key, expected_value)
    
    # Verify setting
    if config.has(key):
        actual_value = config.get(key)
        actual_type = type(actual_value)
        
        print(f"{key}:")
        print(f"  Expected: {expected_value} ({expected_type.__name__})")
        print(f"  Actual: {actual_value} ({actual_type.__name__})")
        print(f"  Match: {actual_value == expected_value and actual_type == expected_type}")
    else:
        print(f"{key}: NOT FOUND in configuration")
```

#### Solution 2: Configuration Loading/Saving

```python
import pynlm
import json
import tempfile

# Test configuration persistence
config = pynlm.createDefaultConfig()

# Add custom settings
config.set("brain.neuron_count", 1500)
config.set("brain.synapse_density", 0.08)
config.set("plasticity.stdp.learning_rate", 0.001)

# Save to file
with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
    config.saveToFile(f.name)
    print(f"Configuration saved to: {f.name}")

# Load from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile(f.name)

# Verify loaded configuration
print("Original configuration:")
print(f"  neuron_count: {config.get('brain.neuron_count')}")
print(f"  synapse_density: {config.get('brain.synapse_density')}")

print("\nLoaded configuration:")
print(f"  neuron_count: {loaded_config.get('brain.neuron_count')}")
print(f"  synapse_density: {loaded_config.get('brain.synapse_density')}")

# Check if they match
print(f"\nConfigurations match: {config.get('brain.neuron_count') == loaded_config.get('brain.neuron_count')}")
```

### Parameter Conflicts

**Problem:** Conflicting configuration parameters

**Symptoms:**
- Parameter overrides not expected
- Configuration values don't match expectations
- Inconsistent behavior

**Solutions:**

```python
import pynlm

# Test parameter order and overrides
config = pynlm.createDefaultConfig()

# Set initial values
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)

print("Initial configuration:")
print(f"  neuron_count: {config.get('brain.neuron_count')}")
print(f"  synapse_density: {config.get('brain.synapse_density')}")

# Override values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.2)

print("\nAfter overrides:")
print(f"  neuron_count: {config.get('brain.neuron_count')}")
print(f"  synapse_density: {config.get('brain.synapse_density')}")

# Check all configuration keys
print("\nAll configuration keys:")
for key in config.getKeys():
    print(f"  {key}: {config.get(key)}")
```

## World and Agent Issues

### World Configuration

**Problem:** World configuration issues

**Symptoms:**
- Invalid world dimensions
- World reset failures
- Agent positioning problems

**Solutions:**

#### Solution 1: World Configuration Validation

```python
import pynlm

# Test world configuration
world = pynlm.createSimpleWorld()

# Test different configurations
test_configs = [
    {"width": 10, "height": 10, "visionWidth": 6, "visionHeight": 6},
    {"width": 20, "height": 20, "visionWidth": 8, "visionHeight": 8},
    {"width": 30, "height": 30, "visionWidth": 12, "visionHeight": 12}
]

for config in test_configs:
    world.configure(**config)
    print(f"Configuration {config}:")
    print(f"  Width: {world.getWidth()}, Height: {world.getHeight()}")
    print(f"  Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"  Max Energy: {world.getMaxEnergy()}")
    print()
```

#### Solution 2: World Initialization

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

print("Before reset:")
print(f"  Simulation time: {world.getSimulationTime()}")

world.reset()

print("\nAfter reset:")
print(f"  Simulation time: {world.getSimulationTime()}")

# Set agent start position
world.setAgentStart(10.0, 10.0)
print("\nAgent start position set to (10.0, 10.0)")

# Update world
world.update(0.1)
print(f"\nAfter update: Simulation time = {world.getSimulationTime()}")
```

### Agent Initialization

**Problem:** Agent initialization issues

**Symptoms:**
- Agent fails to initialize with world
- Sensory processing errors
- Motor command decoding failures

**Solutions:**

#### Solution 1: Agent Initialization Testing

```python
import pynlm

# Test agent initialization
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create and initialize agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Agent initialization successful!")
print(f"  Brain initialized: {brain.getTotalNeuronCount()} neurons")
print(f"  Agent initialized: {agent.getBrain() is brain}")
```

#### Solution 2: Agent Functionality Testing

```python
import pynlm

# Create complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(15.0, 15.0)

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.initialize(world)

# Test agent functionality
for step in range(10):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    print(f"\nStep {step}:")
    print(f"  World time: {world.getSimulationTime():.2f}")
    print(f"  Percept available: {percept is not None}")
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Decode motor command
    action = agent.decodeMotorCommand()
    print(f"  Action: {action.getType().value}")
    
    # Apply motor command
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print agent state
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction Error: {agent.getPredictionError():.3f}")
    print(f"  Development Stage: {agent.getDevelopmentalStage()}")
```

### Sensory Processing

**Problem:** Sensory processing issues

**Symptoms:**
- Vision data problems
- Audio processing errors
- Internal signal issues

**Solutions:**

#### Solution 1: Sensory Data Testing

```python
import pynlm
import numpy as np

# Create agent with sensory testing
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Test vision processing
vision = pynlm.Vision(8, 8, 1)
test_data = [i * 0.1 for i in range(64)]  # Linear gradient
vision.setData(test_data)

print("Vision data test:")
print(f"  Width: {vision.getWidth()}, Height: {vision.getHeight()}")
print(f"  Channels: {vision.getChannels()}")
print(f"  Data: {test_data[:10]}... (truncated)")

# Test audio processing
audio = pynlm.Audio(22050, 22050)
sine_wave = [np.sin(2 * np.pi * 440 * i / 22050) for i in range(22050)]
audio.setData(sine_wave)

print(f"\nAudio test:")
print(f"  Sample rate: {audio.getSampleRate()}")
print(f"  Samples: {audio.getNumSamples()}")

# Test internal signals
internal = pynlm.InternalSignals()
internal.addSignal(0.5)
internal.addSignal(0.3)
internal.addSignal(0.8)

print(f"\nInternal signals test:")
print(f"  Signals added: {3}")
```

### Motor Command Execution

**Problem:** Motor command execution issues

**Symptoms:**
- Invalid motor commands
- Action type errors
- Parameter validation issues

**Solutions:**

#### Solution 1: Action Testing

```python
import pynlm

# Test different action types
action_types = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.TurnRight,
    pynlm.ActionType.Interact,
    pynlm.ActionType.Eat,
    pynlm.ActionType.Rest,
    pynlm.ActionType.Custom
]

print("Action type testing:")
for action_type in action_types:
    action = pynlm.Action(action_type)
    print(f"  {action_type.value}: {action.getType().value}")

# Test actions with parameters
print(f"\nActions with parameters:")
action1 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 0.5])
print(f"  Custom action parameters: {action1.getParameters()}")

# Test action cloning
cloned_action = action1.clone()
print(f"  Cloned action type: {cloned_action.getType().value}")
print(f"  Original == Clone: {action1.getType() == cloned_action.getType()}")
```

## Development and Learning Issues

### Plasticity Problems

**Problem:** Plasticity system issues

**Symptoms:**
- No synaptic changes despite learning
- Plasticity too aggressive
- Plasticity system not enabled

**Solutions:**

#### Solution 1: Plasticity System Testing

```python
import pynlm

# Test plasticity system
config = pynlm.createDefaultConfig()
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

print("Plasticity system test:")
print(f"  STDP enabled: {config.get('plasticity.stdp.enable')}")
print(f"  Hebbian enabled: {config.get('plasticity.hebbian.enable')}")
print(f"  Structural enabled: {config.get('plasticity.structural.enable')}")

# Test brain plasticity
for step in range(10):
    brain.step(step)
    
    if step % 5 == 0:
        print(f"\nStep {step}:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
```

### Neuromodulation Errors

**Problem:** Neuromodulation system issues

**Symptoms:**
- Reward processing failures
- Curiosity system problems
- Neuromodulation not affecting learning

**Solutions:**

#### Solution 1: Neuromodulation Testing

```python
import pynlm

# Create agent with neuromodulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.initialize(world)

print("Neuromodulation system test:")
print(f"  Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"  Curiosity enabled: {agent.isCuriosityEnabled()}")
print(f"  Development enabled: {agent.isDevelopmentEnabled()}")

# Test neuromodulation
for step in range(10):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 3 == 0:
        print(f"\nStep {step}:")
        print(f"  Reward: {reward:.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
```

### Developmental Stage Issues

**Problem:** Developmental stage system problems

**Symptoms:**
- Development not progressing
- Developmental stage stuck
- Critical period issues

**Solutions:**

#### Solution 1: Development System Testing

```python
import pynlm

# Test development system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.enableDevelopment(True)
agent.initialize(world)

print("Development system test:")
print(f"  Initial development stage: {agent.getDevelopmentalStage()}")

# Test development progression
for step in range(30):
    agent.updateDevelopment(0.1)
    
    if step % 10 == 0:
        print(f"\nStep {step}:")
        print(f"  Development stage: {agent.getDevelopmentalStage()}")
        print(f"  Brain development stage: {brain.getDevelopmentalStage()}")
```

## Advanced Troubleshooting

### Memory Profiling

**Problem:** Memory usage analysis

**Solutions:**

```python
#!/usr/bin/env python3
"""Memory profiling tool for NLM."""

import pynlm
import tracemalloc
import time
import gc

def profile_memory_usage():
    """Profile memory usage during simulation."""
    print("Memory Profiling")
    print("=" * 50)
    
    # Start memory tracking
    tracemalloc.start()
    
    # Create system
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get initial snapshot
    snapshot1 = tracemalloc.take_snapshot()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        if step % 25 == 0:
            # Force garbage collection
            gc.collect()
            
            # Take snapshot and compare
            snapshot2 = tracemalloc.take_snapshot()
            
            # Calculate statistics
            top_stats = snapshot2.compare_to(snapshot1, 'lineno')
            
            print(f"\nStep {step} Memory Analysis:")
            for stat in top_stats[:5]:  # Top 5 memory allocations
                print(f"  {stat}")
            
            snapshot1 = snapshot2

if __name__ == "__main__":
    profile_memory_usage()
```

### Performance Analysis

```python
#!/usr/bin/env python3
"""Performance analysis tool for NLM."""

import pynlm
import time
import psutil
import os

def analyze_performance():
    """Analyze simulation performance."""
    print("Performance Analysis")
    print("=" * 50)
    
    process = psutil.Process(os.getpid())
    
    # Test different configurations
    configs = [
        {"name": "Small", "brain.neuron_count": 500},
        {"name": "Medium", "brain.neuron_count": 1000},
        {"name": "Large", "brain.neuron_count": 2000}
    ]
    
    for config_spec in configs:
        print(f"\n{config_spec['name']} Configuration:")
        print("-" * 30)
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_spec["brain.neuron_count"])
        
        # Measure performance
        start_time = time.time()
        start_mem = process.memory_info().rss / 1024 / 1024  # MB
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        steps = 100
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        end_mem = process.memory_info().rss / 1024 / 1024  # MB
        
        # Calculate metrics
        elapsed = end_time - start_time
        steps_per_sec = steps / elapsed
        mem_change = end_mem - start_mem
        mem_per_step = mem_change / steps
        
        print(f"  Time: {elapsed:.3f}s")
        print(f"  Steps/sec: {steps_per_sec:.1f}")
        print(f"  Memory change: {mem_change:.1f} MB")
        print(f"  Memory per step: {mem_per_step:.3f} MB")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")

if __name__ == "__main__":
    analyze_performance()
```

### Debugging Techniques

**Solution 1: Enable Debug Output**

```python
import pynlm
import logging

# Enable debug logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger('pynlm')

# Run simulation with debug output
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Debug simulation:")
for step in range(10):
    logger.debug(f"Starting step {step}")
    brain.step(step)
    logger.debug(f"Step {step} completed: {brain.getFiringNeuronCount()} firing neurons")
```

**Solution 2: Step-by-Step Debugging**

```python
import pynlm

# Debug step by step
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Step-by-step debugging:")
for step in range(5):
    print(f"\n--- Step {step} ---")
    
    # Check brain state before step
    print(f"Before step {step}:")
    print(f"  Active neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Execute step
    brain.step(step)
    
    # Check brain state after step
    print(f"After step {step}:")
    print(f"  Active neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

## Getting Help

### Online Resources

- **Documentation:** https://nlm.readthedocs.io
- **GitHub Issues:** https://github.com/nlm-project/nlm/issues
- **GitHub Discussions:** https://github.com/nlm-project/nlm/discussions
- **Examples and Tutorials:** Check repository for example scripts

### Support Channels

1. **File an Issue:** Use GitHub Issues for bug reports and feature requests
2. **Ask Questions:** GitHub Discussions for general questions
3. **Commercial Support:** Contact for enterprise support
4. **Community:** Join discussions and forums

### How to Report Issues

When reporting an issue, include:

1. **System Information:**
   - Python version
   - NLM version
   - Operating system
   - Hardware specifications

2. **Error Information:**
   - Error messages
   - Stack traces
   - Code snippets that reproduce the issue

3. **Environment Details:**
   - Configuration settings
   - Simulation parameters
   - Reproducible steps

4. **Logs and Data:**
   - Relevant log files
   - Simulation output
   - Configuration files

**Example Issue Report Format:**

```
## Issue Report

### System Information
- Python: 3.9.0
- NLM: 0.1.0
- OS: Ubuntu 20.04
- Hardware: 16GB RAM

### Error Details
Error: ImportError: No module named 'pynlm'
Stack trace: [full stack trace here]

### Steps to Reproduce
1. Run: python3 -c "import pynlm"
2. Observe error

### Expected Behavior
Module should import successfully

### Actual Behavior
Module cannot be imported

### Environment
Configuration:
```
# Configuration details here
```
```

### Additional Information
Any additional context or information about the issue
```

### Verification

Run tests to verify fixes:

```bash
# Basic functionality test
python3 test_basic.py

# Advanced functionality test
python3 test_advanced.py

# Memory test
python3 test_memory.py

# Performance test
python3 test_performance.py
```

## Conclusion

This troubleshooting guide provides comprehensive solutions to common issues with NLM Python bindings. For persistent problems, use the issue reporting guidelines to get help from the NLM community.

---

*This document is continuously updated based on user feedback and issue reports.*
