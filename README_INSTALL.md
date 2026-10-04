# NLM (Neural Learning Machine) - Installation and Quick Start

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project implementing Phase 6 final integration. This document provides comprehensive setup instructions and quick start guides for both the C++ core and Python bindings.

## System Requirements

### C++ Dependencies
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Python Dependencies
- **pip**: Python package installer
- **scikit-build-core**: Modern build system for Python extensions
- **pybind11**: Python bindings generation
- **pytest**: Testing framework
- **numpy**: Numerical computations

## Installation from Source

### Step 1: Clone the Repository

```bash
# Clone the NLM repository
git clone https://github.com/nlm-project/nlm.git

# Navigate to the project directory
cd nlm
```

### Step 2: Install Python Dependencies

```bash
# Install required Python packages
pip install pybind11 scikit-build-core pytest numpy
```

### Step 3: Build the C++ Core

```bash
# Create build directory
mkdir -p build
cd build

# Configure CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets (use all available CPU cores)
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Step 4: Install Python Bindings

```bash
# Install the NLM Python module from source
cd ..
pip install .

# Or install in development mode (for editing)
pip install -e .

# Verify installation
python -c "import pynlm; print('NLM version:', pynlm.__version__ if hasattr(pynlm, '__version__') else 'NLM Python bindings loaded successfully')"
```

## Installation from Package

If you have a pre-built wheel or package, you can install directly:

```bash
# Install from wheel file
pip install nlm-*.whl

# Or install from conda channel (if available)
conda install nlm -c nlm-channel
```

## Quick Start Tutorials

### 1. Create Your First Brain

```python
#!/usrbin/env python3
"""
Example: Create and run your first NLM brain
"""
import pynlm

# Step 1: Create a default configuration
config = pynlm.createDefaultConfig()

# Step 2: Create a brain with the configuration
brain = pynlm.createBrain(config)

# Step 3: Initialize the brain
brain.initialize()

# Step 4: Run some simulation steps
for step in range(100):
    brain.step(step)

# Step 5: Access brain statistics
print(f"Brain Statistics:")
print(f"  Total Neurons: {brain.getTotalNeuronCount()}")
print(f"  Total Synapses: {brain.getTotalSynapseCount()}")
print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
print(f"  Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Excitation/Inhibition Ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"  Developmental Stage: {brain.getDevelopmentalStage()}")
```

### 2. Complete Agent-World Simulation

```python
#!/usr/bin/env python3
"""
Example: Complete agent-world simulation with learning
"""
import pynlm

# Create the core components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20.0, height=20.0, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run the complete simulation
print("Running agent-world simulation...")
for step in range(300):
    # Update world state
    world.update(0.1)
    
    # Agent perceives the world
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes information
    brain.step(step)
    
    # Brain makes decisions
    action = agent.decodeMotorCommand()
    
    # Agent acts in the world
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Brain learns from the outcome
    agent.applyRewardModulation(result.reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress every 50 steps
    if (step + 1) % 50 == 0:
        print(f"Step {step + 1}: Firing rate = {brain.getAverageFiringRate():.2f} Hz, "
              f"Curiosity = {agent.getCuriosityLevel():.3f}, "
              f"Neuromodulation = {agent.getNeuromodulationLevel():.3f}")

print(f"\nSimulation Complete!")
print(f"Final Performance:")
print(f"  Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Neuromodulation Level: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
print(f"  Developmental Stage: {agent.getDevelopmentalStage()}")
print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
print(f"  Prediction Error: {agent.getPredictionError():.3f}")
```

### 3. Save and Load Brain States

```python
#!/bin/env python3
"""
Example: Save and load brain states for persistent learning
"""
import pynlm
import os

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for step in range(100):
    brain.step(step)

# Save brain state
save_path = "my_brain_state.nlm"
if brain.save(save_path):
    print(f"✓ Brain state saved to {save_path}")
    print(f"  File size: {os.path.getsize(save_path) / 1024:.1f} KB")
else:
    print("✗ Failed to save brain state")

# Run more steps
for step in range(100, 200):
    brain.step(step)

print(f"Brain state after more steps: {brain.getTotalSpikeCount()} total spikes")

# Load saved brain state
load_path = "my_brain_state.nlm"
if brain.load(load_path):
    print(f"✓ Brain state loaded from {load_path}")
    print(f"  Loaded state: {brain.getTotalSpikeCount()} total spikes")
else:
    print("✗ Failed to load brain state")
```

### 4. Performance Profiling

```python
#!/bin/env/env python3
"""
Example: Profile brain performance and optimize parameters
"""
import pynlm
import time

# Create optimized configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 2)
config.set("connection_probability", 0.12)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run warmup period
print("Warming up...")
for i in range(100):
    brain.step(i)

# Profile performance
print("Profiling performance...")
start_time = time.time()

for i in range(1000):
    brain.step(i + 100)

end_time = time.time()

duration = end_time - start_time
steps_per_second = 1000.0 / duration

print(f"\nPerformance Results:")
print(f"  Total simulation time: {duration:.2f} seconds")
print(f"  Steps per second: {steps_per_second:.1f}")
print(f"  Average step time: {duration/1000*1000:.2f} ms")
print(f"  Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Network efficiency: {brain.getExcitationInhibitionRatio():.2f} (E/I ratio)")

# Use the built-in performance profiling
from pynlm.test_bindings import profileBrain

# Quick performance check
result = profileBrain(brain, 500)
total_spikes, avg_firing_rate, seconds = result

print(f"\nQuick Profile:")
print(f"  500 steps in {seconds:.2f} seconds")
print(f"  {500/seconds:.1f} steps/second")
print(f"  {total_spikes} total spikes")
print(f"  {avg_firing_rate:.2f} Hz average firing rate")
```

### 5. Advanced Features Demonstration

```python
#!/bin/env/python3
"""
Example: Demonstrate advanced NLM features
"""
import pynlm

# Create brain with advanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)
config.set("enable_structural_plasticity", True)
config.set("enable_predictive_plasticity", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=30.0, height=30.0, visionWidth=20, visionHeight=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("Advanced Features Demonstration")
print("===============================")

# Demonstrate memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

if working_memory and episodic_memory:
    print(f"Memory Systems:")
    print(f"  Working Memory: {working_memory.getActiveTraces()} active traces (capacity: {working_memory.getCapacity()})")
    print(f"  Episodic Memory: {episodic_memory.getEpisodeCount()} episodes (max: {episodic_memory.getMaxEpisodes()})")

# Demonstrate neuromodulation
print(f"\nNeuromodulation Systems:")
print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f} (reward prediction)")
print(f"  Curiosity: {agent.getCuriosityLevel():.3f} (exploration drive)")
print(f"  Novelty: {agent.getNoveltyLevel():.3f} (novelty detection)")
print(f"  Prediction Error: {agent.getPredictionError():.3f} (prediction accuracy)")

# Demonstrate development
print(f"\nDevelopment:")
print(f"  Current Stage: {agent.getDevelopmentalStage()}")
print(f"  Developmental Age: {agent.getBrain()->getConfig()->getOr<double>('developmental_age', 0.0)} time units")

# Demonstrate advanced simulation features
print(f"\nAdvanced Simulation Features:")
print(f"  Pending Spike Events: {brain.getPendingSpikeEventCount()}")
print(f"  Total Synapse Count: {brain.getTotalSynapseCount()}")
print(f"  Region Count: {brain.getRegionCount()}")

# Run advanced simulation
print(f"\nRunning advanced simulation...")
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(result.reward, 0.0)
    agent.updateDevelopment(0.1)

print(f"\nAdvanced Simulation Complete!")
print(f"  Final Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
print(f"  Final Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Development Progress: {agent.getDevelopmentalStage()}")
```

## Running Tests

### Unit Tests

```bash
# Run unit tests
pytest python/ --verbose --tb=short

# Run specific test categories
pytest python/test_bindings.py -v
pytest python/test_suite.py -v
```

### Comprehensive Test Suite

```bash
# Run the complete test suite
python python/test_suite.py

# Quick tests
python python/test_suite.py --quick

# Performance tests
python python/test_suite.py --performance

# Save test results
python python/test_suite.py --save-results
```

## Development Guide

### Building from Source

```bash
# Build with Debug symbols for development
mkdir -p build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### Python Development

```bash
# Install in development mode
pip install -e .

# Run Python code analysis
flake8 python/ --max-line-length=100
mypy python/ --ignore-missing-imports

# Generate documentation
pdoc pynlm --html --output-dir docs/python_api
```

## Troubleshooting

### Common Issues

#### "ModuleNotFoundError: No module named 'pynlm'"

**Solution**: Install the module correctly
```bash
pip install pynlm
# or
pip install -e .
```

#### "ImportError: libpynlm.so: cannot open shared object file"

**Solution**: Set library path
```bash
# On Linux
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/nlm/build

# On macOS
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/path/to/nlm/build

# Or install library system-wide
 sudo cp /path/to/nlm/build/*.so /usr/local/lib/
 sudo ldconfig
```

#### "Segmentation fault" or "Bus error"

**Solution**: Check for common causes
```python
# Validate configuration
if config.get("neuron_count", 1000) > 10000:
    print("Warning: Large neuron count may impact performance")

# Start with small simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
```

### Performance Optimization

```python
# Profile different components
start_time = time.time()
brain.step(100)
step_time = time.time() - start_time
print(f"Step time: {step_time:.4f}s")

# Profile entire simulation
start_time = time.time()
for i in range(1000):
    brain.step(i)
total_time = time.time() - start_time
print(f"Total time for 1000 steps: {total_time:.2f}s")
print(f"Average step time: {total_time/1000:.4f}s")
```

## API Reference

### Core Classes

- **Brain**: Central neural simulation brain class
- **AgentBrain**: Brain-world interface connecting NLM brain to world
- **SimpleWorld**: Simulation environment for NLM development
- **Config**: Configuration management
- **SensoryInput**: Base class for sensory input
- **Vision**: Visual sensory input
- **Action**: Motor action representation
- **WorldObject**: Objects in the simulation world
- **AgentBody**: Agent physical state
- **SensoryPercept**: Processed sensory data

### Factory Functions

- `createDefaultConfig()`: Create a default configuration
- `createBrain(config)`: Create a new brain with configuration
- `createSimpleWorld()`: Create a new simple world
- `createAgentBrain(brain)`: Create a new agent brain interface

### Advanced Features

- **Performance Profiling**: Analyze brain simulation performance
- **Memory Statistics**: Get comprehensive memory statistics
- **Development Simulation**: Simulate brain development over time
- **Reward Shaping**: Apply reward shaping with discounted returns
- **Sensory Pattern Analysis**: Analyze sensory patterns over time
- **Network Topology Analysis**: Analyze network topology statistics
- **Learning Curve Visualization**: Generate learning curve data for visualization

## Examples and Demos

### Example Scripts

1. `examples/basic_brain.py`: Basic brain creation and simulation
2. `examples/agent_simulation.py`: Complete agent-world simulation
3. `examples/save_load.py`: Save and load brain states
4. `examples/performance_profiling.py`: Performance profiling
5. `examples/advanced_features.py`: Advanced features demonstration

### Jupyter Notebooks

- `notebooks/getting_started.ipynb`: Introduction to NLM
- `notebooks/agent_behavior.ipynb`: Advanced agent behavior
- `notebooks/memory_systems.ipynb`: Memory system analysis
- `notebooks/development.ipynb`: Brain development studies

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright (c) 2024 NLM Project Authors. All rights reserved.

## Getting Help

### Documentation

- **Main Documentation**: `README.md`, `docs/` directory
- **Python API Reference**: `docs/PYTHON_API.md`
- **Advanced Usage**: `docs/ADVANCED_USAGE.md`
- **Architecture**: `docs/ARCHITECTURE.md`

### Community Support

- **GitHub Issues**: Report bugs and request features
- **Discussion Forums**: Community discussions and Q&A
- **Discord Server**: Real-time chat and support
- **Email**: support@nlm-project.org

### Development Support

- **Code Repository**: Source code and contributions
- **Development Guidelines**: Contribution standards and practices
- **Testing**: Comprehensive test suite and coverage
- **Documentation**: API documentation and tutorials

## Quick Reference

### Installation

```bash
pip install pynlm
# or for development
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Access statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Agent-World Loop

```python
import pynlm

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 16, 16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run loop
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

This comprehensive guide provides everything needed to get started with NLM, from basic installation to advanced research applications. The modular documentation structure allows users to progressively learn and master the system's capabilities.

---

**NLM - Neural Learning Machine**
**Phase 6: Final Integration**
**An experimental artificial developmental brain system**
