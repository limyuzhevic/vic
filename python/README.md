# NLM Python Bindings - Enhanced Documentation

## Overview

The NLM (Neural Learning Machine) Python bindings provide access to a sophisticated neural simulation framework that implements artificial developmental intelligence. This documentation covers both basic and advanced usage patterns for the `pynlm` Python package.

## Table of Contents

1. [Quick Start](#quick-start)
2. [Installation](#installation)
3. [Basic Concepts](#basic-concepts)
4. [API Reference](#api-reference)
   - [Configuration](#configuration)
   - [Brain](#brain)
   - [Agent Brain](#agent-brain)
   - [World](#world)
   - [Sensory Processing](#sensory-processing)
   - [Motor Control](#motor-control)
5. [Advanced Usage](#advanced-usage)
   - [Performance Optimization](#performance-optimization)
   - [Memory Management](#memory-management)
   - [Experiment Management](#experiment-management)
   - [Data Analysis](#data-analysis)
6. [Examples](#examples)
   - [Basic Simulation](#basic-simulation)
   - [Learning Experiments](#learning-experiments)
   - [Complex Behaviors](#complex-behaviors)
   - [Research Applications](#research-applications)
7. [Troubleshooting](#troubleshooting)
8. [FAQ](#faq)
9. [Changelog](#changelog)

## Quick Start

This guide will get you running your first NLM simulation in under 5 minutes.

### Installation

```bash
# Install the package
cd path/to/nlm-repository
pip install -e .  # or pip install pynlm
```

### Basic Example

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation steps
for i in range(100):
    brain.step(i)

print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Simulation Example

```python
import pynlm
import time

# Setup simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
start_time = time.time()
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Final energy: {world.getAgentBody().energy}")
print(f"Final curiosity: {agent.getCuriosityLevel()}")
print(f"Final novelty: {agent.getNoveltyLevel()}")
```

## Installation

### Requirements

- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher
- pybind11

### Installation Methods

#### Method 1: Development Installation

```bash
cd path/to/nlm-repository
pip install -e .
```

#### Method 2: System Installation

```bash
pip install pynlm
```

#### Method 3: Conda Installation

```bash
conda install -c conda-forge pynlm
```

### Troubleshooting

#### Import Error

If you encounter import errors:

```python
# Check installation
import pynlm
print(pynlm.__version__)
```

If this fails, verify your installation with:

```bash
pip list | grep pynlm
```

#### Missing Dependencies

```bash
# Install build dependencies
apt-get update && apt-get install -y build-essential cmake
# or
conda install -c conda-forge cmake
```

## Basic Concepts

### Neural Components

- **Neurons**: Basic computational units that process information and fire spikes
- **Synapses**: Connections between neurons that transmit signals and enable learning
- **Regions**: Groups of neurons with specific functions
- **Populations**: Organized groups of neurons within regions

### Simulation Components

- **Brain**: The central neural system that processes information
- **Agent**: Interface between brain and environment
- **World**: Environment that provides sensory input and receives motor output

### Key Processes

- **Plasticity**: Learning through synaptic modification (STDP, Hebbian learning)
- **Neuromodulation**: Regulation of neural activity through chemicals like dopamine
- **Development**: Age-dependent changes in neural properties
- **Memory**: Storage and retrieval of experiences and patterns

## API Reference

### Configuration

The configuration system controls all aspects of NLM simulations.

#### `pynlm.createDefaultConfig()`

Creates a configuration object with sensible defaults for simulations.

```python
config = pynlm.createDefaultConfig()
```

#### Configuration Methods

```python
# Set configuration values
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.1)

# Get configuration values
neuron_count = config.getInt("neuron_count", 500)
connection_prob = config.getFloat("connection_probability", 0.1)
enabled = config.getBool("enable_learning", True)

# Check if configuration has a value
if config.has("custom_setting"):
    print("Custom setting is configured")

# Save/load configuration
config.saveToFile("my_config.json")
config.loadFromFile("my_config.json")

# Get all configuration keys
keys = config.getKeys()
```

### Brain

The brain is the central neural system that processes information and produces actions.

#### Creating a Brain

```python
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Core Brain Methods

```python
# Main simulation step
brain.step(step_number)

# With timing
brain.step(step_number, time)

# Process sensory input
brain.receiveSensoryInput(percept)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Reset brain
brain.reset()

# Save/load brain state
brain.save("brain_checkpoint.nlm")
brain.load("brain_checkpoint.nlm")
```

#### Advanced Brain Methods

```python
# Brain analysis and utilities
brain_summary = brain.logStatus()  # Get detailed status report
stats = brain.analyzeConnectivity()  # Get network connectivity analysis
brain.enableAdvancedLearning(True)  # Enable advanced learning features
brain.setLearningRate(0.01)  # Set learning rate
```

### Agent Brain

The agent brain provides the interface between the neural brain and the environment.

#### Creating an Agent Brain

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Agent Methods

```python
# Process sensory input
agent.processSensoryInput(percept)

# Get motor command
action = agent.decodeMotorCommand()

# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(timestep)

# Get neuromodulation levels
print(f"Dopamine level: {agent.getNeuromodulationLevel()}")
print(f"Curiosity level: {agent.getCuriosityLevel()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

### World

The world provides the environment in which the agent operates.

#### Creating a World

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
```

#### World Methods

```python
# Update world state
world.update(timestep)

# Apply motor command
result = world.applyMotorCommand(action, time)

# Get sensory percept
percept = world.getSensoryPercept()

# Get agent state
body = world.getAgentBody()
print(f"Position: ({body.x}, {body.y})")
print(f"Energy: {body.energy}")
print(f"Health: {body.health}")

# World control
world.setAgentStart(10, 10)
world.setRandomSeed(42)
```

### Sensory Processing

Sensory processing converts environmental information into neural signals.

#### Sensory Input Classes

- `Vision`: Visual information from the environment
- `Audio`: Sound information (currently not used in SimpleWorld)
- `InternalSignals`: Internal state information (energy, health, etc.)

#### Sensory Processing Example

```python
# Process different sensory modalities
vision_data = percept.getVision()
touch_data = percept.getTouch()
internal_data = percept.getInternal()

# Convert to Python lists for easier processing
vision_list = percept.getVisionAsList()
touch_list = percept.getTouchAsList()
internal_list = percept.getInternalAsList()

# Check availability
if percept.isVisionAvailable():
    print("Vision data available")

if percept.isTouchAvailable():
    print("Touch data available")

# Get specific internal signals
energy = percept.getEnergyLevel()
health = percept.getHealthLevel()
moving = percept.getMovementState()
turning = percept.getTurningState()
```

### Motor Control

Motor control converts neural activity into actions in the environment.

#### Action Classes

Actions include movement commands like MoveForward, TurnLeft, Interact, etc.

#### Motor Control Example

```python
# Decode motor command
action = agent.decodeMotorCommand()

# Get action details
type = action.getType()
parameters = action.getParameters()
name = action.getName()

# Check action type
if action.isMovement():
    print("Action is a movement")

if action.isInteracting():
    print("Action is an interaction")

# Get action name from type
from pynlm import ActionType
action_name = Action.getNameByType(ActionType.MoveForward)
```

## Advanced Usage

### Performance Optimization

#### Multi-threading and Parallel Processing

```python
# Enable multi-threading
brain.enableThreading(True)
brain.setThreadCount(4)  # Use 4 threads

# Parallel processing
results = brain.processInParallel([task1, task2, task3])

# Performance profiling
brain.profilePerformance()
```

#### Memory Management

```python
# Monitor memory usage
memory_usage = brain.monitorMemory()
print(f"Memory usage: {memory_usage} MB")

# Force garbage collection
import gc
gc.collect()

# Manage brain memory
brain.clearMemoryCache()
```

### Memory Management

#### Brain Cloning and Copying

```python
# Clone a brain
brain_clone = brain.clone()

# Copy brain state
brain.copyState(other_brain)

# Export/import brain state
data = brain.exportState()
brain.importState(data)

# Use brain templates
brain.applyTemplate("learning_template")
```

#### Advanced Configuration

```python
# Get available templates
templates = brain.getTemplateList()

# Create custom configuration
brain.createCustomTemplate({
    "neuron_count": 2000,
    "connection_probability": 0.05,
    "enable_structural_plasticity": True
})
```

### Experiment Management

#### Running Experiments

```python
# Run phase experiments
brain.runPhaseExperiment("phase_3", 1000)

# Get available phases
phases = brain.getAvailablePhases()

# Run continual learning experiments
tasks = [
    {"name": "task1", "steps_between": 100},
    {"name": "task2", "steps_between": 100}
]
brain.runContinualLearning(tasks, cycles=3)
```

#### Data Collection

```python
# Collect experiment metrics
metrics = brain.collectExperimentData({
    "metrics": ["reward", "firing_rate", "energy"],
    "sampling_rate": 10,
    "duration": 1000
})

# Analyze learning curves
learning_stats = brain.analyzeLearningCurve(metrics)
```

### Data Analysis

#### Network Analysis

```python
# Analyze network connectivity
connectivity = brain.analyzeConnectivity()
network_density = connectivity.getNetworkDensity()
mean_degree = connectivity.getMeanDegree()
clustering_coefficient = connectivity.getClusteringCoefficient()

# Analyze firing patterns
firing_patterns = brain.analyzeFiringPatterns()
topology = firing_patterns.getNetworkTopology()
```

#### Spike Train Analysis

```python
# Analyze spike trains
spike_stats = brain.analyzeSpikeTrain()
avg_firing_rate = spike_stats.getAverageFiringRate()
refractory_period = spike_stats.getRefractoryPeriod()
spike_intervals = spike_stats.getSpikeIntervals()
```

#### Population Analysis

```python
# Analyze population activity
population_stats = brain.analyzePopulationActivity()
population_activity = population_stats.getActivityLevel()
mean_population_rate = population_stats.getMeanPopulationRate()
```

## Examples

### Basic Simulation

```python
import pynlm

# Simple simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Learning Experiments

```python
import pynlm
import time

# Setup learning environment
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("enable_structural_plasticity", True)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Learning simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Energy={world.getAgentBody().energy:.1f}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}")
```

### Complex Behaviors

```python
import pynlm
import numpy as np

# Complex behavior with exploration/exploitation
class ForagingAgent:
    def __init__(self):
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.setup()
    
    def setup(self):
        self.world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
        self.world.reset()
        self.agent.initialize(self.world)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Track performance
        self.rewards = []
        self.exploration_rate = []
    
    def run_episode(self, max_steps=500):
        for step in range(max_steps):
            # Action selection with curiosity
            action = self.agent.decodeMotorCommand()
            
            # Environment interaction
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            reward = result.reward
            
            # Learning
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Sensory processing
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            
            # Update world
            self.world.update(0.1)
            
            # Track metrics
            self.rewards.append(reward)
            self.exploration_rate.append(1.0 - self.agent.getCuriosityLevel())
            
            # Early termination
            if self.world.getAgentBody().energy <= 0:
                break
        
        return self.rewards, self.exploration_rate

# Run simulation
agent = ForagingAgent()
rewards, exploration = agent.run_episode()

print(f"Episode completed. Total reward: {sum(rewards):.2f}")
print(f"Final exploration rate: {exploration[-1]:.3f}")
print(f"Mean reward: {np.mean(rewards):.3f}")
```

### Research Applications

```python
import pynlm
import matplotlib.pyplot as plt

# Research: Studying learning curves
class LearningStudy:
    def __init__(self):
        self.results = []
        self.config_template = {
            "neuron_count": 1000,
            "connection_probability": 0.1,
            "enable_structural_plasticity": True,
            "enable_curiosity": True
        }
    
    def run_parameter_study(self, param_values):
        """Run experiment with different parameter values"""
        for value in param_values:
            config = pynlm.createDefaultConfig()
            config.set("connection_probability", value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            learning_curve = []
            for step in range(500):
                # Process sensory input (using random data for this example)
                sensory_data = {
                    "vision": np.random.random(16*16),
                    "touch": np.random.random(8),
                    "internal": np.array([0.5, 0.8, 0.0, 0.0])
                }
                # In real usage, this would come from a world
                brain.receiveSensoryInput(sensory_data)
                brain.step(step)
                
                learning_curve.append(brain.getAverageFiringRate())
            
            self.results.append({
                "parameter": value,
                "learning_curve": learning_curve,
                "final_performance": learning_curve[-1]
            })
    
    def analyze_results(self):
        """Analyze and plot results"""
        import matplotlib.pyplot as plt
        
        plt.figure(figsize=(12, 8))
        
        # Plot learning curves
        plt.subplot(2, 2, 1)
        for result in self.results:
            plt.plot(result["learning_curve"], label=f"p={result['parameter']}")
        plt.xlabel("Step")
        plt.ylabel("Firing Rate")
        plt.title("Learning Curves")
        plt.legend()
        
        # Plot final performance
        plt.subplot(2, 2, 2)
        params = [r["parameter"] for r in self.results]
        performance = [r["final_performance"] for r in self.results]
        plt.scatter(params, performance)
        plt.xlabel("Connection Probability")
        plt.ylabel("Final Performance")
        plt.title("Performance vs Parameter")
        
        plt.tight_layout()
        plt.show()
        
        return self.results

# Run research study
study = LearningStudy()
study.run_parameter_study([0.05, 0.1, 0.15, 0.2])
results = study.analyze_results()
```

## Troubleshooting

### Common Issues

#### Import Error

**Problem**: Cannot import pynlm
**Solution**: Check installation
```bash
pip install -e .
```

#### Simulation Issues

**Problem**: Brain doesn't fire or learn
**Solution**: Check configuration
```python
# Verify configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("connection_probability", 0.2)

# Verify brain initialization
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Memory Issues

**Problem**: High memory usage
**Solution**: Monitor and optimize
```python
# Monitor memory usage
memory_usage = brain.monitorMemory()
print(f"Current memory usage: {memory_usage} MB")

# Force garbage collection
import gc
gc.collect()

# Disable unused features
brain.enableStructurallyPlasticity(False)
```

#### Performance Issues

**Problem**: Slow simulation
**Solution**: Optimize settings
```python
# Enable threading
brain.enableThreading(True)
brain.setThreadCount(4)

# Reduce simulation complexity
brain.setNeuronCount(500)  # Instead of 1000
```

### Debugging Tips

```python
# Enable debug output
brain.setDebugLevel(2)

# Validate brain state
is_valid = brain.validateBrainState()
if not is_valid:
    print("Brain state validation failed")

# Get diagnostic information
diagnostics = brain.getDebugInfo()
print(diagnostics)

# Run validation tests
brain.runDiagnostics()
```

## FAQ

### What is NLM?

NLM (Neural Learning Machine) is a neural simulation framework that implements artificial developmental intelligence. It simulates spiking neurons, synaptic connections, and learning processes.

### How much memory does it require?

Memory usage depends on the number of neurons and synapses. A typical simulation with 1000 neurons uses approximately 100-500 MB of RAM. Memory usage scales with simulation complexity.

### Can I run simulations in real-time?

Yes, with appropriate configuration and optimization. Enable threading and use lower simulation timestep settings for real-time performance.

### What programming languages can I use for experiments?

You can use Python for all experiment setup and analysis. The NLM bindings provide a Pythonic interface to the underlying C++ simulation engine.

### How do I get started with research?

1. Start with basic examples in the Examples section
2. Use the configuration system to control simulation parameters
3. Use the analysis tools to understand network properties
4. Modify the provided examples for your research questions

### What's the difference between curiosity and novelty?

- **Novelty**: Detection of new or different sensory input
- **Curiosity**: Motivation to explore and learn about the environment

Novelty drives curiosity, and curiosity influences exploration behavior.

## Changelog

### Version 1.0.0 (Current)

- Initial release with basic simulation capabilities
- Python bindings for Brain, AgentBrain, SimpleWorld
- Configuration system and sensory/motor processing
- Basic learning mechanisms (STDP, Hebbian learning)
- Comprehensive documentation and examples

### Future Plans

- [ ] Memory management utilities
- [ ] Advanced visualization tools
- [ ] Multi-agent simulation
- [ ] Real-time visualization
- [ ] Cloud computing integration
- [ ] Mobile device support
- [ ] Advanced analysis tools
- [ ] Research-focused features

## Contributing

### Guidelines

1. Follow the existing code style and conventions
2. Write comprehensive docstrings for new functions
3. Add examples to the documentation
4. Maintain backward compatibility
5. Include appropriate error handling
6. Add unit tests for new functionality

### How to Contribute

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Write tests and documentation
5. Submit a pull request

### Code Quality Standards

- Use descriptive variable and function names
- Include comprehensive docstrings
- Handle errors gracefully
- Follow PEP 8 style guidelines
- Include type hints where appropriate
- Add examples and usage documentation

## Contact

For questions, issues, or suggestions:

- GitHub Issues: https://github.com/yourusername/nlm-python-bindings/issues
- Documentation: https://yourusername.github.io/nlm-python-bindings/
- Email: your.email@example.com

---

*This documentation is continuously updated. Check the GitHub repository for the latest version.*