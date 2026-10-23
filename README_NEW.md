# NLM Python Package - Quick Start Guide

## Overview

The NLM (Neural Learning Machine) Python package provides powerful tools for neural simulation and artificial brain research. This document provides a quick start guide for using the enhanced features.

## Key Improvements

### 🚀 New Features

1. **Advanced Convenience Functions** - 18 high-level functions for different use cases
2. **Command-Line Interface** - 8+ commands for advanced usage
3. **Comprehensive Examples** - 10 example demonstrations
4. **Enhanced Configuration** - Better validation and source tracking
5. **Modern Packaging** - Updated with pyproject.toml and CI/CD support

### 📦 What's New

- **`pynlm_advanced.py`** - Advanced convenience functions
- **`nlm_cli.py`** - Command-line interface
- **`nlm_examples.py`** - Complete example gallery
- **Enhanced pyproject.toml** - Modern Python packaging
- **Comprehensive documentation** - Improvement summary and examples

## Quick Start

### 1. Installation

```bash
# Install the package
pip install -e .

# Or install with development dependencies
pip install -e .[dev]
```

### 2. Basic Usage

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for i in range(100):
    brain.step(i)
    if i % 25 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

### 3. Using Advanced Features

```python
# Import advanced functions
from pynlm_advanced import (
    create_advanced_brain,
    create_agent_world,
    run_simulation_with_logging
)

# Create an advanced brain with all features
brain, config = create_advanced_brain(
    neuron_count=1500,
    enable_curiosity=True,
    enable_memory=True,
    enable_development=True
)

# Run simulation with logging
stats = run_simulation_with_logging(
    brain, max_steps=1000, log_interval=100,
    enable_progress=True, enable_detailed_stats=True
)

print(f"Simulation complete! Average firing rate: {stats['avg_firing_rate']:.3f}")
```

### 4. Using Command Line Interface

```bash
# Show help
nlm-cli --help

# Create an advanced brain
nlm-cli create --advanced --neurons 1000

# Run a simulation
nlm-cli run --steps 500 --logging

# Explore brain properties
nlm-cli explore --steps 100

# Agent-world simulation
nlm-cli run --agent --world-width 30 --world-height 30 --steps 1000
```

### 5. Running Examples

```bash
# Run all examples
python nlm_examples.py

# Run specific examples
python -c "from nlm_examples import example_3_agent_environment_simulation; example_3_agent_environment_simulation()"
```

## Advanced Examples

### Agent-Environment Simulation

```python
from pynlm_advanced import create_agent_world

# Create agent and world
brain, agent, world = create_agent_world(
    neuron_count=1000,
    world_width=30,
    world_height=30,
    enable_curiosity=True,
    enable_rewards=True
)

# Run simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {agent.getCuriosityLevel():.3f} curiosity, "
              f"{brain.getFiringNeuronCount()} firing neurons")
```

### Development Studies

```python
from pynlm_advanced import create_development_experiment

# Create brain for developmental studies
brain, config, stages = create_development_experiment(
    neuron_count=1200,
    development_stages=8,
    enable_stdp=True,
    enable_hebbian=True,
    enable_structural=True
)

# Simulate development
for age in range(0, 2500, 100):
    brain.develop()
    for step in range(10):
        brain.step(age + step)
    
    stage = brain.getDevelopmentalStage()
    print(f"Age {age}: {stage}, firing rate: {brain.getAverageFiringRate():.3f}")
```

### Memory Systems

```python
from pynlm_advanced import create_memory_experiment

# Create brain for memory studies
brain, config = create_memory_experiment(
    episodes=50,
    memory_capacity=500,
    replay_frequency=20
)

# Accumulate experience
for episode in range(50):
    for step in range(20):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        brain.step(step + episode * 20)
    
    if episode % 10 == 0 and brain.getEpisodicMemory():
        episodes = brain.getEpisodicMemory().getEpisodesForReplay(3)
        print(f"Episode {episode}: Replaying {len(episodes)} episodes")
```

## Configuration System

The NLM package includes an enhanced configuration system with:

- **Type-safe access** to configuration values
- **Source tracking** (file, command-line, runtime)
- **Validation** of configuration options
- **Extensible framework** for new options

### Example Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set various configuration options
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)
config.set("development.enable", True)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

## Performance Profiling

The package includes performance profiling capabilities:

```python
from pynlm_advanced import run_simulation_with_logging

# Run simulation with detailed statistics
stats = run_simulation_with_logging(
    brain, max_steps=1000, log_interval=100,
    enable_detailed_stats=True
)

# Analyze performance
print(f"Average firing rate: {stats['avg_firing_rate']:.3f}")
print(f"Total spikes: {stats['total_spikes']}")
print(f"Max firing rate: {stats['max_firing_rate']:.3f}")
```

## Command Line Interface Details

### Available Commands

1. **`create`** - Create brains and configurations
2. **`run`** - Run simulations
3. **`explore`** - Explore brain properties
4. **`experiment`** - Run experimental protocols
5. **`manage`** - Manage saved brain states
6. **`visualize`** - Generate visualizations
7. **`benchmark`** - Run performance benchmarks
8. **`config`** - Configure global settings

### Create Command Options

- `--advanced` - Create advanced brain with all features
- `--agent` - Create agent-world setup
- `--development` - Create development experiment setup
- `--memory` - Create memory experiment setup
- `--curiosity` - Create curiosity experiment setup
- `--prediction` - Create prediction experiment setup
- `--neurons` - Number of neurons (default: 1000)
- `--density` - Connection density (default: 0.1)
- `--save` - Save brain state to file

### Run Command Options

- `--steps` - Number of simulation steps (default: 100)
- `--neurons` - Number of neurons (default: 1000)
- `--agent` - Run agent-world simulation
- `--logging` - Enable detailed logging
- `--detailed-stats` - Collect detailed statistics
- `--quiet` - Suppress output
- `--load` - Load brain state from file
- `--save` - Save brain state to file
- `--export-stats` - Export statistics to JSON file

## Testing and Development

### Running Tests

```bash
# Run unit tests
pytest

# Run all tests with coverage
pytest --cov=pynlm --cov-report=html

# Run specific test files
pytest tests/test_brain.py
```

### Development Setup

```bash
# Install development dependencies
pip install -e .[dev]

# Check code quality
black pynlm_advanced.py
isort pynlm_advanced.py
mypy pynlm_advanced.py
flake8 pynlm_advanced.py
```

## Troubleshooting

### Common Issues

**Issue: ImportError: No module named 'pynlm'**

Solution:
```bash
pip install -e .
```

**Issue: CMake cannot find Python**

Solution:
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Issue: Brain initialization fails**

Solution:
```python
# Ensure brain is initialized before use
brain.initialize()
```

**Issue: Simulation runs slowly**

Solution:
```python
# Use smaller brain configurations
brain = pynlm.createBrain(config)
# Increase simulation timestep for performance
```

## Additional Resources

### Documentation

- **IMPROVEMENTS_SUMMARY.md** - Complete improvement documentation
- **DEVELOPMENT.md** - Development guidelines
- **README.md** - Project overview
- **pyproject.toml** - Package configuration

### Examples

- **nlm_examples.py** - Complete example gallery
- **nlm_cli.py** - Command-line interface
- **pynlm_advanced.py** - Advanced convenience functions

### Testing

- **test_improvements.py** - Quick test suite
- **pytest** - Unit tests
- **examples/** - Example tests

## Support and Community

### Getting Help

- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Documentation**: https://nlm.readthedocs.io

### Contributing

- **Pull Requests**: Welcome!
- **Code Reviews**: Community-driven
- **Bug Reports**: Please report issues
- **Feature Requests**: Suggest new features

## License

MIT License - See LICENSE file for details.

## Acknowledgements

This project benefits from:
- **pybind11** - Python bindings for C++
- **scikit-build-core** - Modern Python packaging
- **CMake** - Build system
- **Python** - High-level language
- **Community Contributors** - Continuous improvement

## Changelog

For the latest changes and version history, see the CHANGELOG.md file.

---

*Last updated: 2026-10-23*
*NLM Python Package - Version 0.1.0 Enhanced*
