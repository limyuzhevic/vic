# NLM (Neural Learning Machine) - Complete Setup and Usage Guide

NLM is a sophisticated neural simulation framework designed to create brain-like systems that learn from experience through brain-inspired architecture. It combines real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Installation](#installation)
3. [Basic Usage](#basic-usage)
4. [Command Line Interface](#command-line-interface)
5. [Advanced Features](#advanced-features)
6. [Performance Optimization](#performance-optimization)
7. [Development Guide](#development-guide)
8. [Documentation](#documentation)
9. [Troubleshooting](#troubleshooting)
10. [Examples](#examples)

---

# Quick Start

## What is NLM?

NLM (熙然) is a **brain simulator** for computers that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:

- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

## Basic Example

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think for 100 steps
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

## Installation

### System Requirements

- Python 3.8+
- C++ compiler (g++, clang, or MSVC)
- CMake 3.16+

### Quick Installation

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Run the complete setup script
./setup.sh

# Verify installation
nlm --help
```

### Manual Installation

```bash
# Install Python dependencies
pip install -e .
pip install pytest numpy matplotlib scipy

# Build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e .
```

## Basic Usage

### 1. Create a Simple Brain

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### 2. Run a Simulation

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 3. Brain in a World

```python
import pynlm

# Setup brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getAgentBody().health * 0.1
    predicted = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted)

print("Simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

---

# Installation

## Prerequisites

### Ubuntu/Debian

```bash
apt-get update
apt-get install -y build-essential cmake python3-dev python3-pip python3-venv
```

### macOS

```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew and then:
brew install cmake python3
```

### Windows

```powershell
# Install Visual Studio Build Tools
# Install Python from python.org
```

## Build Process

### Using Setup Script (Recommended)

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Run the complete setup script
./setup.sh
```

### Manual Build

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# Install Python bindings
pip install -e ..
```

## Verification

### Test Installation

```bash
# Run validation script
python3 validate_setup.py

# Test basic functionality
nlm run --steps 100

# Run examples
python3 examples.py basic

# Test development tools
python3 dev_tools.py validate-all
```

### Check Python Bindings

```python
import pynlm
print("pynlm version:", pynlm.__version__ if hasattr(pynlm, '__version__') else '0.1.0')

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Brain created: {brain.getTotalNeuronCount()} neurons")
```

## Development Environment

### Virtual Environment

```bash
# Create virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install development dependencies
pip install -e .
pip install pytest pytest-cov pytest-xdist numpy matplotlib scipy
```

### Running Tests

```bash
# Run all tests
pytest tests/

# Run tests with coverage
pytest --cov=nlm --cov-report=html tests/

# Run specific test modules
pytest tests/test_brain.py
pytest tests/test_memory.py

# Run tests in parallel
pytest -n auto tests/
```

---

# Command Line Interface

## Main CLI Commands

```bash
# Show help
nlm --help

# List available commands
nlm help

# Run a simple simulation
nlm run --steps 100

# Run simulation with world
nlm run --steps 500 --world-config "width=20 height=20 visionWidth=8 visionHeight=8"

# Create custom brain configuration
nlm config create --parameters "neuron_count=2000 learning_rate=0.01"

# List configuration options
nlm config list

# Save brain state
nlm brain save --file brain_state.json

# Load brain state
nlm brain load --file brain_state.json

# Run benchmark
nlm benchmark --steps 100 --repeats 5

# Run examples
nlm example basic
nlm example world
nlm example agent
nlm example all
```

## CLI Command Reference

### `nlm run`

Run a NLM simulation.

**Options:**
- `--steps, -s`: Number of simulation steps (default: 100)
- `--world-config`: World configuration
- `--agent-config`: Agent configuration
- `--brain-config`: Brain configuration file path
- `--verbose, -v`: Enable verbose output

### `nlm config`

Manage configuration.

**Subcommands:**
- `create`: Create configuration with parameters
- `list`: List available configuration options

### `nlm brain`

Brain management commands.

**Subcommands:**
- `save`: Save brain state to file
- `load`: Load brain state from file

### `nlm analyze`

Analyze brain data.

**Options:**
- `--format`: Output format (json, csv, text)

### `nlm benchmark`

Run performance benchmarks.

**Options:**
- `--steps, -s`: Steps per benchmark run
- `--repeats`: Number of benchmark repetitions
- `--world`: Include world simulation

### `nlm example`

Run built-in examples.

**Arguments:**
- `name`: Example name (basic, world, agent, learning, all)

### `nlm help`

Show help information.

**Arguments:**
- `topic`: Help topic

---

# Advanced Features

## Learning Features

### Reward Modulation

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

# Apply reward modulation
reward = 1.0
predicted = agent.getPredictionError()
agent.applyRewardModulation(reward, predicted)
```

### Curiosity-Driven Learning

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)

print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
```

### Development Process

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableDevelopment(True)

# Run development simulation
for step in range(100):
    agent.updateDevelopment(0.1)
    print(f"Step {step}: {agent.getDevelopmentalStage()}")
```

## Memory Systems

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Brain has integrated memory systems:
# - Working memory (persistent activity)
# - Episodic memory (experience storage)
# - Associative memory (pattern completion)

# Use memory features
print(f"Total spikes (episodic memory): {brain.getTotalSpikeCount()}")
```

## Prediction System

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print(f"Prediction error: {agent.getPredictionError():.3f}")
```

---

# Performance Optimization

## Multi-Thread Simulation

```python
import pynlm
import threading
from concurrent.futures import ThreadPoolExecutor

class ParallelSimulator:
    def __init__(self, steps, num_threads=4):
        self.steps = steps
        self.num_threads = num_threads
        
    def run_thread(self, thread_id, steps_per_thread):
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for step in range(steps_per_thread):
            brain.step(step)
        
        return brain
    
    def run_parallel(self):
        steps_per_thread = self.steps // self.num_threads
        
        with ThreadPoolExecutor(max_workers=self.num_threads) as executor:
            futures = [
                executor.submit(self.run_thread, i, steps_per_thread)
                for i in range(self.num_threads)
            ]
            
            return [future.result() for future in futures]

# Run parallel simulation
simulator = ParallelSimulator(steps=1000, num_threads=4)
results = simulator.run_parallel()
print(f"Parallel simulation completed")
```

## Memory Optimization

```python
import pynlm
import gc

# Force garbage collection periodically
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        gc.collect()
```

---

# Development Guide

## Project Structure

```
src/
├── brain/              # Core brain components
├── memory/             # Memory systems
├── cognition/          # Cognitive systems
├── plasticity/         # Plasticity rules
├── development/        # Development systems
├── neuromodulation/     # Neuromodulation
├── sensory/            # Sensory processing
├── motor/              # Motor system
├── environment/        # Environment interface
├── visualization/      # Visualization
├── performance/        # Performance optimizations
└── core/               # Core utilities

python/                # Python bindings

configs/               # Configuration files
docs/                  # Documentation
tests/                  # Test suite
examples.py            # Examples and tutorials
```

## Coding Standards

### Python Style

- **Indentation**: 4 spaces
- **Line length**: 100 characters
- **Import order**: Standard library, third-party, local
- **Naming**: snake_case for functions and variables

### C++ Style

- **Indentation**: 2 spaces
- **Line length**: 80 characters
- **Naming**: CamelCase for classes, snake_case for functions

## Testing Strategy

```bash
# Run all tests
pytest tests/

# Run with coverage
pytest --cov=nlm --cov-report=html tests/

# Run integration tests
pytest tests/integration/
```

## Performance Testing

```bash
# Run performance benchmarks
python3 perf_analysis/benchmark.py --suite

# Profile code
python3 -m cProfile -o profile.stats examples.py all
```

---

# Documentation

## Documentation Sources

- **HOW_TO_USE.md**: This comprehensive user guide
- **easy_usage.md**: Basic usage guide
- **docs/ARCHITECTURE.md**: Technical architecture
- **docs/SCIENCE.md**: Scientific background
- **examples.py**: Usage examples and tutorials

## Documentation Generation

```bash
# Generate API documentation
pydoc -o docs/api python/pynlm.py

# Build documentation with Sphinx
make -C docs html
```

---

# Troubleshooting

## Common Issues

### Issue 1: Brain Initialization Fails

**Solution**:

```python
import pynlm

# Robust initialization
brain = None
for attempt in range(3):
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        print(f"Success: {brain.getTotalNeuronCount()} neurons")
        break
    except Exception as e:
        print(f"Attempt {attempt + 1} failed: {e}")
```

### Issue 2: Performance Problems

**Solution**:

```python
# Optimize configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("plasticity_enabled", False)
```

### Issue 3: Memory Issues

**Solution**:

```python
import gc

# Force garbage collection
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        gc.collect()
```

---

# Examples

## Basic Examples

### Simple Brain

```python
python3 examples.py basic
```

### World Simulation

```python
python3 examples.py world
```

### Advanced Agent

```python
python3 examples.py advanced
```

## Running All Examples

```bash
python3 examples.py all

# Or use the demo script
./demo.py
```

---

# See Also

- [NLM Documentation](https://nlm.readthedocs.io) - Complete documentation
- [NLM GitHub Repository](https://github.com/nlm-project/nlm) - Source code
- [NLM Examples](https://github.com/nlm-project/examples) - Additional examples
- [NLM Discussions](https://github.com/nlm-project/nlm/discussions) - Community forum

---

# Quick Reference

## Command Line

```bash
# Quick commands
nlm run --steps 100                    # Simple simulation
nlm example basic                      # Basic example
nlm benchmark --steps 200 --repeats 3   # Performance test
```

## Python API

```python
import pynlm

# Create and run brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)

print(f"Result: {brain.getTotalSpikeCount()} spikes")
```

## Configuration

```python
import pynlm

# Custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("learning_rate", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()
```

---

**Need Help?**

- Check the [Documentation](https://nlm.readthedocs.io)
- Visit the [GitHub Repository](https://github.com/nlm-project/nlm)
- Join the [NLM Community](https://github.com/nlm-project/nlm/discussions)

---

*Last updated: October 2026*
*Version: 0.1.0*
