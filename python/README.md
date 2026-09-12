# NLM Python API Documentation
# Complete reference guide

This document provides comprehensive documentation for the NLM Python API, including all enhanced features and capabilities.

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start](#quick-start)
3. [API Reference](#api-reference)
4. [Brain Class](#brain-class)
5. [AgentBrain Class](#agentbrain-class)
6. [Configuration Class](#configuration-class)
7. [Action and Percept Classes](#action-and-percept-classes)
8. [World Classes](#world-classes)
9. [Enhanced Features](#enhanced-features)
10. [Examples and Demos](#examples-and-demos)
11. [Installation](#installation)
12. [Testing](#testing)

## Introduction

The NLM (Neural Learning Machine) Python API provides a powerful interface to the NLM C++ neural simulation framework. The enhanced version includes access to all brain systems, comprehensive helper methods, and a rich set of tools for creating and simulating artificial brains.

### Key Features

- **Complete Brain System Access**: Direct access to memory, cognition, neuromodulation, and plasticity systems
- **Helper Methods**: Convenience functions for common operations
- **Enhanced Documentation**: Comprehensive docstrings and examples
- **Pythonic Interface**: Methods designed for easy Python use
- **Type Safety**: Strong typing with proper error handling
- **Performance Monitoring**: Built-in tools for monitoring and analysis

## Quick Start

### Installation

```bash
# Install the package
pip install -e .

# Run tests
python test_nlm_api.py

# Run all examples
python -m examples
```

### Basic Usage

```python
import nlm

# Create a brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Create world and agent
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## API Reference

### Brain Class

The `Brain` class represents the central neural simulation engine with access to all brain systems.

#### Properties

- `total_neuron_count`: Total number of neurons in the brain
- `total_synapse_count`: Total number of synapses
- `region_count`: Number of regions
- `total_spike_count`: Total spikes generated
- `firing_neuron_count`: Currently firing neurons
- `average_firing_rate`: Average firing rate in Hz

#### Methods

##### Core Simulation

- `initialize()`: Initialize the brain with configuration
- `step(step, time=None)`: Perform a simulation step
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state to file
- `load(filepath)`: Load brain state from file

##### System Access

- `getWorkingMemory()`: Get working memory system
- `getEpisodicMemory()`: Get episodic memory system
- `getAssociativeMemory()`: Get associative memory system
- `getPredictionSystem()`: Get prediction system
- `getPlanner()`: Get neural planner
- `getConceptFormation()`: Get concept formation system
- `getAttention()`: Get attentional selection system
- `getDopamine()`: Get dopamine neuromodulation system
- `getCuriosity()`: Get curiosity neuromodulation system
- `getNovelty()`: Get novelty detection system
- `getPredictionErrorSignal()`: Get prediction error system
- `getSTDP()`: Get STDP plasticity system
- `getHebbian()`: Get Hebbian plasticity system
- `getStructuralPlasticity()`: Get structural plasticity system
- `getRandomGenerator()`: Get random number generator

##### Statistics and Monitoring

- `getRegionStatistics()`: Get statistics for all regions as dictionary
- `getMemoryStats()`: Get memory system statistics
- `getNeuromodulationState()`: Get neuromodulation system state
- `getPlasticityState()`: Get plasticity system state
- `getTopFiringNeurons(region_id, count)`: Get top firing neurons
- `getRegionStats(region_id)`: Get formatted statistics for a region

##### Development and Plasticity

- `getDevelopmentalStage()`: Get current developmental stage
- `setDevelopmentalStage(stage)`: Set developmental stage
- `isDecreasingPlasticity()`: Check if in stable development phase
- `shouldReacquire(step)`: Check if memory replay should occur

### AgentBrain Class

The `AgentBrain` class connects the NLM brain to the world, handling sensory processing and motor decoding.

#### Properties

- `sensory_input_size`: Expected sensory input size
- `motor_output_size`: Expected motor output size

#### Methods

- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory percept and inject into brain
- `decodeMotorCommand()`: Decode brain motor activity into motor command
- `applyRewardModulation(reward, predicted_reward)`: Apply reward-based neuromodulation
- `updateDevelopment(timestep)`: Update development system
- `enableRewardModulation(enable)`: Enable/disable reward modulation
- `enableCuriosity(enable)`: Enable/disable curiosity
- `enableDevelopment(enable)`: Enable/disable development

### Configuration Class

The `Config` class manages brain configuration parameters.

#### Methods

- `loadFromFile(filepath)`: Load configuration from JSON file
- `loadFromArgs(argc, argv)`: Load configuration from command line arguments
- `saveToFile(filepath)`: Save configuration to JSON file
- `has(key)`: Check if configuration key exists
- `getKeys()`: Get all configuration keys
- `clear()`: Clear all configuration entries
- `summary()`: Get configuration summary string

#### Type-Safe Accessors

- `get_int(key)`: Get integer configuration value
- `get_float(key)`: Get float configuration value
- `get_bool(key)`: Get boolean configuration value
- `get_string(key)`: Get string configuration value

### Action and Percept Classes

#### Action Class

- `Action(type=None, parameters=None)`: Create action
- `getType()`: Get action type
- `setType(type)`: Set action type
- `getParameters()`: Get action parameters
- `setParameters(params)`: Set action parameters
- `getName()`: Get action name
- `clone()`: Create copy of action

#### SensoryPercept Class

- `SensoryPercept()`: Create empty percept
- `setVision(vision)`: Set vision data
- `setTouch(touch)`: Set touch data
- `setInternal(internal)`: Set internal signals
- `setProprioception(proprio)`: Set proprioception data
- `setAudio(audio)`: Set audio data

### World Classes

#### SimpleWorld Class

- `SimpleWorld()`: Create simple 2D world
- `configure(width, height, visionWidth, visionHeight)`: Configure world dimensions
- `reset()`: Reset world to initial state
- `setAgentStart(x, y)`: Set agent starting position
- `update(timestep)`: Update world by timestep
- `applyMotorCommand(cmd, time)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body state

## Enhanced Features

### 1. Comprehensive System Access

The enhanced API provides direct access to all brain systems:

```python
# Memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Cognitive systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

# Neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Plasticity systems
stdp = brain.getSTDP()
hebbian = brain.getHebbian()
structural_plasticity = brain.getStructuralPlasticity()
```

### 2. Helper Methods

```python
# Region statistics
region_stats = brain.getRegionStatistics()
for region_id, stats in region_stats.items():
    print(f"Region {region_id}: {stats['neuron_count']} neurons, {stats['firing_rate_hz']} Hz")

# Memory statistics
memory_stats = brain.getMemoryStats()

# Neuromodulation state
neuromod_state = brain.getNeuromodulationState()

# Plasticity state
plasticity_state = brain.getPlasticityState()

# Top firing neurons
top_neurons = brain.getTopFiringNeurons(region_id, 10)

# Development status
is_developing = brain.isDecreasingPlasticity()
```

### 3. Type-Safe Configuration

```python
# Python-friendly configuration access
config = nlm.createDefaultConfig()

# Set configuration values
config.set("test.int", 42)
config.set("test.float", 3.14)
config.set("test.bool", True)
config.set("test.string", "hello world")

# Get configuration values with type safety
int_val = config.get_int("test.int")
float_val = config.get_float("test.float")
bool_val = config.get_bool("test.bool")
string_val = config.get("test.string")
```

### 4. Action and Percept Serialization

```python
# Action serialization
action = nlm.Action(nlm.ActionType.MoveForward)
action.setParameters([0.5, 0.3])

# Convert to dictionary for storage
action_dict = nlm.Action.action_to_dict(action)

# Convert back to action
new_action = nlm.Action.action_from_dict(action_dict)

# Percept serialization
percept = nlm.SensoryPercept()
percept.setVision([0.1] * 256)

percept_dict = nlm.SensoryPercept.percept_to_dict(percept)
new_percept = nlm.SensoryPercept.percept_from_dict(percept_dict)
```

### 5. Simulation Patterns

```python
# Pattern 1: Simple brain simulation
def run_brain_simulation(brain, num_steps):
    for step in range(num_steps):
        brain.step(step)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Pattern 2: Complete agent simulation
def run_agent_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Pattern 3: Learning simulation with monitoring
def run_learning_simulation(brain, world, agent, num_steps):
    learning_stats = {
        'total_spikes': [],
        'curiosity_levels': [],
        'action_counts': {},
        'development_stages': []
    }
    
    for step in range(num_steps):
        # ... simulation steps ...
        
        # Collect statistics
        learning_stats['total_spikes'].append(brain.getTotalSpikeCount())
        learning_stats['curiosity_levels'].append(agent.getCuriosityLevel())
        learning_stats['development_stages'].append(brain.getDevelopmentalStage())
    
    return learning_stats
```

## Examples and Demos

### Example Scripts

The `examples/` directory contains comprehensive example scripts:

- `example_1_simple_brain.py`: Simple brain simulation
- `example_2_agent_world.py`: Agent-world interaction
- `example_3_learning_simulation.py`: Learning simulation with visualization
- `example_4_advanced_usage.py`: Advanced API usage examples
- `example_5_performance_monitoring.py`: Performance monitoring and analysis

### Demo Scripts

The `demo/` directory contains demo scripts:

- `nlm_demo.py`: Quick demo of enhanced API features

### Documentation

The `python/` directory contains:

- `NLM_API_QUICK_START.md`: Quick start guide
- `README.md`: Main README with comprehensive documentation

## Installation

### Prerequisites

- Python 3.8+
- pip

### Installation Steps

```bash
# Clone the repository
# or download the package

# Install the package
pip install -e .

# Install additional dependencies for examples
pip install matplotlib numpy memory_profiler
```

### Build from Source

For development or custom builds:

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build Python bindings
make pynlm

# Install Python package
pip install -e ../python
```

## Testing

### Running Tests

Run the comprehensive test suite with:

```bash
python test_nlm_api.py
```

### Test Coverage

The test suite covers:

- Basic brain functionality
- Helper methods
- Agent functionality
- Action and percept classes
- Configuration system
- Simulation patterns
- Checkpoints
- Module metadata

### Expected Test Results

All tests should pass, indicating:

- API is working correctly
- Enhanced features are functional
- Type safety is maintained
- Performance is acceptable

## Common Patterns

### Pattern 1: Silent Brain Test

```python
import nlm
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
for i in range(10):
    brain.step(i)
print("Silent brain test done!")
```

### Pattern 2: Brain Watching a World

```python
import nlm
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
print(f"Watched world for 30 steps, firing rate: {brain.getAverageFiringRate()} Hz")
```

### Pattern 3: Complete Agent with Learning

```python
import nlm
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()
world = nlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
print("Agent simulation complete!")
```

## Error Handling

The NLM Python API includes comprehensive error handling:

### Common Exceptions

- `RuntimeError`: Generic runtime errors
- `ValueError`: Invalid values or parameters
- `KeyError`: Missing configuration keys
- `TypeError`: Type mismatches

### Best Practices

1. **Always check for errors**: Use try-catch blocks for critical operations
2. **Validate parameters**: Check that parameters are within valid ranges
3. **Handle missing keys**: Use `has()` method before accessing configuration values
4. **Monitor performance**: Use performance monitoring tools for long simulations

## Performance Considerations

### Memory Management

- Use helper methods to avoid keeping unnecessary references
- Clean up references when no longer needed
- Monitor memory usage during long simulations

### Simulation Performance

- Use batch operations when possible
- Monitor system resources during simulations
- Implement checkpointing for long-running simulations

### Type Conversion Overhead

- Minimize type conversions in tight loops
- Use pre-allocated data structures where possible
- Profile performance for critical sections

## Migration Guide

### From Old API to New API

The enhanced API maintains backward compatibility while adding new features:

#### Old Methods (Still Available)

- `brain.getTotalNeuronCount()`: Available
- `brain.getTotalSynapseCount()`: Available
- `brain.getFiringNeuronCount()`: Available
- `brain.getAverageFiringRate()`: Available
- `brain.step(step)`: Available

#### New Methods (Enhanced API)

- `brain.getWorkingMemory()`: New
- `brain.getEpisodicMemory()`: New
- `brain.getAssociativeMemory()`: New
- `brain.getPredictionSystem()`: New
- `brain.getPlanner()`: New
- `brain.getConceptFormation()`: New
- `brain.getAttention()`: New
- `brain.getDopamine()`: New
- `brain.getCuriosity()`: New
- `brain.getNovelty()`: New
- `brain.getPredictionErrorSignal()`: New
- `brain.getSTDP()`: New
- `brain.getHebbian()`: New
- `brain.getStructuralPlasticity()`: New
- `brain.getPendingSpikeEventCount()`: New
- `brain.getRandomGenerator()`: New

### Migration Examples

#### Example 1: Accessing Memory Systems

```python
# Old way (limited access)
brain.logStatus()

# New way (full access)
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()
```

#### Example 2: Configuration Access

```python
# Old way (manual parsing)
config_str = config.summary()

# New way (type-safe)
config = nlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

neuron_count = config.get_int("brain.neuron_count")
stdp_enabled = config.get_bool("plasticity.stdp.enable")
```

#### Example 3: Action Handling

```python
# Old way (limited)
action = nlm.Action()

# New way (enhanced)
action = nlm.Action(nlm.ActionType.MoveForward)
action.setParameters([0.5, 0.3, 0.8])
action_dict = nlm.Action.action_to_dict(action)
new_action = nlm.Action.action_from_dict(action_dict)
```

## Future Enhancements

### Planned Features

1. **Advanced Visualization**: Real-time brain visualization tools
2. **Machine Learning Integration**: Integration with ML frameworks
3. **Distributed Computing**: Support for parallel and distributed simulations
4. **Web API**: REST API for remote brain simulation
5. **Mobile Support**: Python bindings for mobile platforms
6. **Cloud Integration**: Integration with cloud computing platforms

### Community Contributions

The NLM Python API is an open-source project. Contributions are welcome:

- Bug fixes and improvements
- New example scripts
- Documentation updates
- Performance optimizations
- Integration with external tools

## Support

### Getting Help

- **Documentation**: Check the `docs/` directory for comprehensive documentation
- **Examples**: Run the example scripts in the `examples/` directory
- **Demos**: Try the demo scripts in the `demo/` directory
- **Tests**: Run the test suite to verify installation

### Reporting Issues

- **Bug Reports**: Report issues through the GitHub repository
- **Feature Requests**: Suggest new features through the GitHub repository
- **Questions**: Ask questions on the NLM discussion forum

### Support Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Examples**: https://github.com/nlm-project/nlm-examples
- **Demos**: https://github.com/nlm-project/nlm-demos

## Conclusion

The enhanced NLM Python API provides a comprehensive and powerful interface to the NLM brain simulation framework. With access to all brain systems, helper methods, type-safe configuration, and extensive documentation, the enhanced API makes it easy to create sophisticated neural simulations while remaining accessible to beginners.

The enhancements build upon the existing API while adding new capabilities for advanced users, making NLM more powerful and versatile than ever before.