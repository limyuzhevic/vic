// Enhanced Python Bindings for NLM
// File: enhanced_python_bindings.md

# Enhanced NLM Python API

This document describes the enhanced Python bindings for NLM, which provide high-level convenience classes, advanced APIs, and comprehensive tools for neural simulation research and development.

## Overview

The enhanced Python bindings extend the original API with:

- **High-level convenience classes** for rapid prototyping
- **Advanced experiment management** tools
- **Performance monitoring** and analysis
- **Enhanced configuration** utilities
- **Research-focused tools** for computational neuroscience

## Key Features

### 1. High-Level Convenience Classes

#### SimpleBrain
A high-level brain interface designed for rapid setup and experimentation without low-level complexity.

**Constructor:**
```python
SimpleBrain()
SimpleBrain(neuron_count, regions)
```

**Key Methods:**
- `learn(steps, input_pattern)` - Learn from experience through simulation
- `getLearningStats()` - Get comprehensive learning performance statistics
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Example:**
```python
# Quick setup and learning
brain = SimpleBrain(500, 2)
brain.learn(1000)  # Learn from 1000 simulation steps
stats = brain.getLearningStats()
print(f"Learned {stats['firing_neurons']} firing neurons")
```

#### LearningAgent
Complete agent-world integration for reinforcement learning and embodied cognition research.

**Constructor:**
```python
LearningAgent(width, height)
```

**Key Methods:**
- `runEpisode(max_steps=1000)` - Run a complete episode and return rewards
- `getAgentStats()` - Get comprehensive agent performance statistics
- `getWorldInfo()` - Get detailed world state information
- `isAlive()` - Check if agent is alive (health and energy > 0)

**Example:**
```python
# Create an agent in a 20x20 world
agent = LearningAgent(20, 20)

# Run multiple episodes
for episode in range(10):
    rewards = agent.runEpisode(max_steps=500)
    stats = agent.getAgentStats()
    print(f"Episode {episode}: Reward {sum(rewards):.2f}, "
          f"Curiosity {stats['curiosity_level']:.3f}")
    
    if not agent.isAlive():
        print("Agent died - episode ended")
        break
```

#### ExperimentManager
Comprehensive experiment management for systematic research and reproducible results.

**Constructor:**
```python
ExperimentManager()
```

**Key Methods:**
- `createExperiment(name, brain_template)` - Create a new experiment
- `runExperiment(name, brain, environment, max_steps)` - Run a single experiment
- `runBatch(names, brain_template, environment, max_steps)` - Run multiple experiments
- `saveResults(filepath)` - Save experiment results to file
- `getExperimentNames()` - Get all experiment names
- `clearExperiments()` - Clear all experiments

**Example:**
```python
# Set up experiment management
exp_manager = ExperimentManager()

# Create custom brain configuration
config = createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

brain = createBrain(config)

# Create and run experiments
exp1 = exp_manager.createExperiment("connectivity_test", brain)
exp2 = exp_manager.createExperiment("learning_test", brain)

exp_manager.runExperiment("connectivity_test", brain, None, 500)
exp_manager.runExperiment("learning_test", brain, None, 1000)

# Save results
exp_manager.saveResults("experiment_results.json")
```

### 2. Advanced APIs

#### PerformanceMonitor
Track and analyze performance metrics during brain simulation.

**Constructor:**
```python
PerformanceMonitor(brain)
```

**Key Methods:**
- `step()` - Record a simulation step and update performance metrics
- `getStepsPerSecond()` - Get processing speed in steps per second
- `getMaxMemoryUsage()` - Get maximum memory usage in KB
- `getSummary()` - Get comprehensive performance summary

**Example:**
```python
brain = createBrain(500, 2)
monitor = PerformanceMonitor(brain)

for i in range(1000):
    brain.step(i)
    monitor.step()

summary = monitor.getSummary()
print(f"Performance: {summary['steps_per_second']:.1f} steps/sec")
print(f"Memory: {summary['max_memory_kb']:.1f} KB")
print(f"Firing rate: {summary['average_firing_rate']:.2f} Hz")
```

#### SimpleVisualizer
Basic visualization utilities for NLM data analysis and research.

**Methods:**
- `plotFiringRate(brain, width=800, height=600)` - Plot firing rate over time
- `plotSynapseWeights(brain)` - Plot synaptic weight distribution
- `plotActivityPattern(brain, region_id=1)` - Plot neural activity pattern

**Example:**
```python
brain = createBrain(300, 1)

# Create visualizations
SimpleVisualizer.plotFiringRate(brain)
SimpleVisualizer.plotSynapseWeights(brain)
SimpleVisualizer.plotActivityPattern(brain, 1)
```

### 3. Enhanced Configuration Utilities

#### Convenience Functions

```python
# Create default configuration with learning parameters
config = createDefaultConfig()

# Configure learning based on mode
configureLearningConfig(config, "deep")  # Deep learning mode
configureLearningConfig(config, "fast")   # Fast learning mode
configureLearningConfig(config, "conservative")  # Conservative learning mode

# Create brain with specific parameters
brain = createBrain(1000, 4)
brain = createBrain(2000, 6, "configs/experiment_config.cfg")

# Create world with custom dimensions
world = createSimpleWorld(30, 30, 15, 15)
```

#### High-Level Experiment Types

```python
from enum import Enum

class ExperimentType(Enum):
    ConnectivityTest = 1      # Basic neural connectivity
    LearningExperiment = 2   # Synaptic learning and plasticity
    PlasticityTest = 3       # Comprehensive plasticity studies
    NeuromodulationTest = 4  # Neuromodulator system testing
    DevelopmentTest = 5     # Developmental processes
    Benchmark = 6           # Performance benchmarking
```

### 4. Research Tools

#### Phase 6 Integrated Experiment
Run comprehensive, integrated experiments that test all brain systems together:

```python
from enhanced_python_bindings import runPhase6Experiment

brain = createBrain(1500, 3)
world = createSimpleWorld(25, 25, 12, 12)

# Run Phase 6 integration experiment
experiment = runPhase6Experiment(brain, world, 2000)
print(f"Experiment completed with {experiment.getTotalNeuronCount()} neurons")
```

### 5. Backward Compatibility

The enhanced bindings maintain full backward compatibility with the original API:

```python
# Original API still works (imported from pynlm)
from pynlm import Config, Brain, SimpleWorld, AgentBrain

# Create and use components
config = Config()
brain = Brain(config)
world = SimpleWorld()
world.configure(20, 20)

brain.initialize()
brain.step(0)
```

## Advanced Usage Examples

### Example 1: Complete Research Pipeline
```python
import enhanced_python_bindings as nlm

# 1. Setup configuration and brain
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.curiosity.enable", True)

brain = nlm.createBrain(2000, 4)
world = nlm.createSimpleWorld(30, 30)

# 2. Create learning agent
agent = nlm.LearningAgent(30, 30)

# 3. Run multiple experiments
exp_manager = nlm.ExperimentManager()

# Experiment 1: Basic connectivity
exp1 = exp_manager.createExperiment("connectivity", brain)
exp_manager.runExperiment("connectivity", brain, world, 500)

# Experiment 2: Complex learning
exp2 = exp_manager.createExperiment("complex_learning", brain)
exp_manager.runExperiment("complex_learning", brain, world, 2000)

# 4. Save results
exp_manager.saveResults("research_results.json")

# 5. Performance analysis
monitor = nlm.PerformanceMonitor(brain)
for i in range(2000):
    brain.step(i)
    monitor.step()

summary = monitor.getSummary()
print(f"Research completed: {summary['steps_per_second']:.1f} steps/sec")
```

### Example 2: Performance Benchmarking
```python
import enhanced_python_bindings as nlm
import time

# Setup benchmark configuration
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("performance.vectorization_enabled", True)

brain = nlm.createBrain(5000, 5)

# Run performance benchmark
monitor = nlm.PerformanceMonitor(brain)
start_time = time.time()

for step in range(5000):
    brain.step(step)
    monitor.step()
    
    if step % 1000 == 0:
        print(f"Progress: {step}/5000 steps")

end_time = time.time()
summary = monitor.getSummary()

print(f"\n=== BENCHMARK RESULTS ===")
print(f"Total time: {end_time - start_time:.2f} seconds")
print(f"Steps per second: {summary['steps_per_second']:.1f}")
print(f"Memory usage: {summary['max_memory_kb']:.1f} KB")
print(f"Average firing rate: {summary['average_firing_rate']:.2f} Hz")
```

### Example 3: Developmental Learning Study
```python
import enhanced_python_bindings as nlm

# Setup developmental study
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("development.stage", "critical")
config.set("development.plasticity_decay_rate", 0.02)

brain = nlm.createBrain(1000, 3)
world = nlm.createSimpleWorld(20, 20)

# Create agent
agent = nlm.LearningAgent(20, 20)

# Run developmental learning episodes
episodes = []
for episode in range(5):
    print(f"\n--- Episode {episode + 1} ---")
    
    # Reset for new episode
    brain.reset()
    brain.initialize()
    world.reset()
    agent.getBrain().reset()
    agent.getBrain().initialize()
    
    # Run learning episode
    rewards = agent.runEpisode(max_steps=1000)
    episodes.append(rewards)
    
    # Check developmental changes
    stats = agent.getAgentStats()
    print(f"Developmental stage: {stats['developmental_stage']}")
    print(f"Curiosity level: {stats['curiosity_level']:.3f}")
    print(f"Average reward: {sum(rewards)/len(rewards):.3f}")

# Analyze developmental trajectory
print("\n=== Developmental Trajectory ===")
for i, rewards in enumerate(episodes):
    avg_reward = sum(rewards)/len(rewards)
    print(f"Episode {i+1}: Average reward {avg_reward:.3f}")
```

## API Reference

### Core Classes

| Class | Description | Key Methods |
|-------|-------------|-------------|
| `SimpleBrain` | High-level brain interface | `learn()`, `getLearningStats()`, `save()`, `load()` |
| `LearningAgent` | Agent-world integration | `runEpisode()`, `getAgentStats()`, `getWorldInfo()` |
| `ExperimentManager` | Experiment management | `createExperiment()`, `runExperiment()`, `saveResults()` |
| `PerformanceMonitor` | Performance tracking | `step()`, `getStepsPerSecond()`, `getSummary()` |
| `SimpleVisualizer` | Visualization tools | `plotFiringRate()`, `plotSynapseWeights()`, `plotActivityPattern()` |

### Utility Functions

| Function | Description | Parameters |
|----------|-------------|------------|
| `createDefaultConfig()` | Create default configuration | None |
| `createBrain(neuronCount, regions)` | Create brain with parameters | `neuronCount`, `regions` |
| `createSimpleWorld(width, height)` | Create world | `width`, `height` |
| `createAgentBrain(brain, world)` | Create agent interface | `brain`, `world` |
| `configureLearningConfig(config, mode)` | Configure learning based on mode | `config`, `mode` |
| `runPhase6Experiment(brain, world, steps)` | Run integrated Phase 6 experiment | `brain`, `world`, `steps` |

### Configuration Modes

| Mode | Description | Settings |
|------|-------------|---------|
| `deep` | Deep learning with high complexity | Low learning rates, high connectivity |
| `fast` | Fast learning with quick adaptation | High learning rates, reduced complexity |
| `conservative` | Conservative learning with stability | Low learning rates, minimal structural changes |

### Error Handling

The enhanced bindings use standard Python exception handling:

```python
try:
    brain = nlm.createBrain(1000, 3)
    agent = nlm.LearningAgent(20, 20)
except nlm.RuntimeError as e:
    print(f"Error: {e}")
```

## Installation and Setup

### Prerequisites
```bash
pip install pybind11 numpy matplotlib
```

### Build Instructions
```bash
# Build enhanced Python bindings
cmake .. -DBUILD_PYTHON_BINDINGS=ON -DPYTHON_BINDINGS_ENHANCED=ON
make -j4

# Install Python package
pip install pynlm-enhanced
```

### Quick Start
```python
import enhanced_python_bindings as nlm

# Create components
brain = nlm.createBrain(500, 2)
world = nlm.createSimpleWorld(20, 20)

# Run experiment
agent = nlm.LearningAgent(20, 20)
rewards = agent.runEpisode(1000)

print(f"Experiment completed with {len(rewards)} rewards")
```

## Migration Guide

### From Original to Enhanced API

**Original:**
```python
from pynlm import Config, Brain

config = Config()
brain = Brain(config)
brain.initialize()
```

**Enhanced:**
```python
from enhanced_python_bindings import createDefaultConfig, createBrain

config = createDefaultConfig()
brain = createBrain(1000, 3)
```

The enhanced API provides the same functionality with additional convenience methods and advanced features.

## Research Applications

The enhanced Python bindings support various research applications:

1. **Neural Circuit Development**: Study how neural circuits develop and mature through learning
2. **Reinforcement Learning**: Implement and test RL algorithms with biological neural systems
3. ** Neuromodulation Studies**: Investigate the role of neuromodulators in learning and behavior
4. **Performance Analysis**: Compare performance across different architectural configurations
5. **Experiment Management**: Systematically organize and reproduce complex experiments
6. **Developmental Research**: Study age-related changes in learning and plasticity

## Future Enhancements

Planned future enhancements include:

- **Advanced visualization** with matplotlib integration
- **Machine learning integration** for meta-learning research
- **Parallel processing** for large-scale simulations
- **Real-time monitoring** and visualization
- **Cloud-based computing** support for distributed experiments

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, research-focused interface for neural simulation while maintaining full backward compatibility. Researchers and developers can leverage high-level convenience classes for rapid prototyping or access advanced APIs for sophisticated computational neuroscience research.

The enhanced bindings bridge the gap between the accessibility of high-level APIs and the power of low-level neural simulation, making advanced neural research more accessible and efficient.