# NLM Python Bindings - Quick Start Guide

This document provides a quick start guide for using the improved NLM (Neural Learning Machine) Python bindings.

## Overview

The NLM Python bindings provide a modern, Pythonic interface to the NLM neural simulation framework. The bindings have been completely improved with better organization, enhanced functionality, and comprehensive documentation while maintaining full backward compatibility.

## Key Improvements

### **Version Information**
```python
import pynlm
print(pynlm.__version__)        # "0.2.0"
print(pynlm.__version_name__)    # "NLM Python Bindings v0.2.0"
print(pynlm.__author__)          # "NLM Authors"
print(pynlm.__license__)         # "MIT"
```

### **New Features**

#### 1. **Progress Tracking**
```python
import pynlm

# Create progress tracker
tracker = pynlm.createProgressTracker()
tracker.setMaxSteps(1000)

# Use with simulation
for step in range(1000):
    brain.step(step)
    tracker.incrementStep(f"Step {step + 1}")
```

#### 2. **Utility Classes**
```python
import pynlm.utils as nlm_utils

# Create experiment with progress tracking
experiment = nlm_utils.Experiment(config, "my_experiment")

def progress_callback(progress, message):
    print(f"Progress: {progress:.1%}: {message}")

# Run with progress tracking
experiment.run(world, progress_callback, steps=1000)

# Analyze results
stats = experiment.get_statistics()
nlm_utils.Visualization.print_brain_summary(experiment.brain)
```

#### 3. **Enhanced Factory Functions**
```python
import pynlm

# All factory functions available
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
tracker = pynlm.createProgressTracker()
```

#### 4. **Custom Exceptions**
```python
import pynlm

# Python-style exception handling
try:
    brain.save("/invalid/path")
except pynlm.NLMBrainError as e:
    print(f"Brain error: {e}")
except pynlm.NLMConfigError as e:
    print(f"Config error: {e}")
```

## Quick Start Examples

### **Example 1: Basic Brain Simulation**
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### **Example 2: Complete Agent-World Simulation**
```python
import pynlm
import pynlm.utils as nlm_utils

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with progress tracking
experiment = nlm_utils.Experiment(config, "agent_simulation")
experiment.run(world, steps=500)
```

### **Example 3: Experiment with Progress Tracking**
```python
import pynlm.utils as nlm_utils

def progress_callback(progress, message):
    print(f"Progress: {progress:.1%}: {message}")

# Create experiment
config = nlm_utils.create_default_config()
experiment = nlm_utils.Experiment(config, "learning_experiment")

# Run with progress tracking
experiment.run(world=None, progress_callback=progress_callback, steps=1000)

# Save results
experiment.save("experiment_results")
```

### **Example 4: Data Analysis and Visualization**
```python
import pynlm.utils as nlm_utils

# Create and run experiment
experiment = nlm_utils.Experiment(config, "analysis_experiment")
experiment.run(world, steps=200)

# Visualize results
nlm_utils.Visualization.print_brain_summary(experiment.brain)

# Access data
stats = experiment.get_statistics()
print(f"Total spikes: {stats['brain_stats']['total_spikes']}")
print(f"Average firing rate: {stats['brain_stats']['average_firing_rate']:.2f}")

# Save data
experiment.save("experiment_data")
```

## Installation

The NLM Python bindings are already installed. To verify:

```bash
python3 -c "import pynlm; print(pynlm.__version__)"
```

To run the examples:

```bash
python3 examples.py
python3 test_bindings.py
python3 verify_improvements.py
python3 final_verification.py
```

## API Reference

### **Core Classes**

#### `Config`
- Configuration class for NLM system
- Methods: `set()`, `get()`, `has()`, `loadFromFile()`, `saveToFile()`

#### `Brain`
- Central neural simulation brain class
- Methods: `initialize()`, `step()`, `reset()`, `save()`, `load()`

#### `SimpleWorld`
- Simple 2D world for NLM simulation
- Methods: `configure()`, `update()`, `applyMotorCommand()`, `getSensoryPercept()`

#### `AgentBrain`
- Agent brain interface connecting NLM brain to world
- Methods: `initialize()`, `processSensoryInput()`, `decodeMotorCommand()`

### **Types**

#### `NeuronId`, `SynapseId`, `RegionId`, `PopulationId`
- Unique identifiers for neural components

#### `NeuronType`, `SynapseType`, `DevelopmentalStage`
- Enumeration types for neural properties

### **Enumerations**

- `NeuronType`: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- `SynapseType`: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging
- `ActionType`: MoveForward, TurnLeft, Look, Interact, Eat, Drink, etc.

### **Utility Classes**

#### `Experiment`
- High-level simulation management
- Progress tracking integration
- Results collection and analysis

#### `DataHandler`
- Data save/load utilities
- Spike pattern analysis
- Simulation data management

#### `Visualization`
- Text-based brain and world summaries
- Matplotlib plotting (when available)
- Progress tracking visualizations

### **Factory Functions**

#### Core Factories
- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain with configuration
- `createSimpleWorld()` - Create simulation world
- `createAgentBrain(brain)` - Create agent interface
- `createProgressTracker()` - Create progress tracker

#### Utility Functions
- `create_default_config()` - Create default config (Python style)
- `run_quick_test()` - Run quick functionality test

### **Exceptions**

- `NLMException` - Base exception
- `NLMConfigError` - Configuration errors
- `NLMBrainError` - Brain operation errors
- `NLMWorldError` - World simulation errors
- `NLMSimulationError` - Simulation errors
- `NLMMemoryError` - Memory management errors

## Examples Directory

Run the comprehensive examples:

```bash
python3 examples.py
```

This script demonstrates:
- Basic brain creation and simulation
- Progress tracking
- Complete agent-world simulation
- Memory management and checkpointing
- Advanced configuration
- Error handling
- Neuromodulation
- Memory systems
- Utility classes

## Testing

Run the complete test suite:

```bash
python3 test_bindings.py
python3 verify_improvements.py
python3 final_verification.py
```

## Documentation

For detailed documentation:

1. **Main Documentation**: Read this file
2. **Improvement Plan**: `NLM_Python_Bindings_Improvement_Plan.md`
3. **Examples**: `examples.py`
4. **Tests**: `test_bindings.py`

## Support

For questions or issues:
- Check the documentation
- Run the examples to see if they work
- Use the verification scripts
- All errors are caught and handled with descriptive messages

## Backward Compatibility

**All existing code continues to work without modification:**

```python
# This still works exactly as before:
import pynlm

config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
```

The improved API maintains full compatibility with existing code while adding new functionality and better Python conventions.

---

**Version:** 0.2.0  
**License:** MIT  
**Author:** NLM Authors  
**Last Updated:** 2026-10-12