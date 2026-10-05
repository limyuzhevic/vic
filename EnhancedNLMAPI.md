# Enhanced NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the enhanced Python API of the Neural Learning Machine (NLM) framework. The bindings.cpp file implements a complete, user-friendly Python interface that makes NLM accessible to Python developers while maintaining full access to the underlying C++ power and flexibility.

## Key Features

### 1. Complete System Coverage

The enhanced bindings provide comprehensive coverage of all NLM systems:

#### Neuromodulation Systems
- **Dopamine**: Reward and reinforcement learning signal
  - `Dopamine.setLevel(level)`: Set dopamine concentration
  - `Dopamine.getLevel()`: Get current dopamine level
  - `Dopamine.signalReward(reward)`: Signal reward learning
  - `Dopamine.enableLearning(enabled)`: Enable/disable learning

- **Curiosity**: Exploration motivation based on novelty and prediction error
  - `Curiosity.getLevel()`: Get curiosity level
  - `Curiosity.update(novelty, prediction_error, dt)`: Update curiosity
  - `Curiosity.setNoveltyWeight(weight)`: Set novelty contribution
  - `Curiosity.setPredictionErrorWeight(weight)`: Set prediction error contribution

- **Novelty**: Novelty detection from observations
  - `Novelty.getLevel()`: Get current novelty level
  - `Novelty.detectNovelty(observation, previous_observation)`: Detect novelty
  - `Novelty.update(dt)`: Decay novelty over time
  - `Novelty.setLevel(level)`: Set novelty level directly

- **PredictionError**: Prediction error signal for learning
  - `PredictionError.getError()`: Get prediction error value
  - `PredictionError.computeError(predicted, actual)`: Compute error
  - `PredictionError.updatePrediction(prediction)`: Update prediction
  - `PredictionError.getMagnitude()`: Get error magnitude

- **Reward**: Reward signal for reinforcement learning
  - `Reward.getValue()`: Get current reward
  - `Reward.add(delta)`: Accumulate reward
  - `Reward.computeReward(observation)`: Compute reward from observation

#### Memory Systems
- **NeuralWorkingMemory**: Transient active information storage
  - `NeuralWorkingMemory.store(pattern, strength)`: Store neural pattern
  - `NeuralWorkingMemory.retrieve()`: Retrieve current content
  - `NeuralWorkingMemory.contains(neuron)`: Check if neuron is in memory
  - `NeuralWorkingMemory.update(dt)`: Update memory (decay and maintenance)
  - `NeuralWorkingMemory.clear()`: Clear working memory
  - `NeuralWorkingMemory.setCapacity(capacity)`: Set memory capacity
  - `NeuralWorkingMemory.getCapacity()`: Get memory capacity

- **NeuralEpisodicMemory**: Experience storage with neural encoding
  - `NeuralEpisodicMemory.storeEpisode(episode)`: Store experience episode
  - `NeuralEpisodicMemory.retrieveSimilar(pattern, max_results)`: Find similar episodes
  - `NeuralEpisodicMemory.retrieveTemporal(start_time, end_time, max_results)`: Get episodes from time window
  - `NeuralEpisodicMemory.replayEpisode(episode)`: Replay stored experience
  - `NeuralEpisodicMemory.consolidate(threshold)`: Consolidate old episodes
  - `NeuralEpisodicMemory.enableReplay(enabled)`: Enable/disable replay

- **NeuralAssociativeMemory**: Pattern relationship storage
  - `NeuralAssociativeMemory.associate(pattern_a, pattern_b, strength)`: Create association
  - `NeuralAssociativeMemory.retrieve(query_pattern, max_results)`: Find associated patterns
  - `NeuralAssociativeMemory.spreadActivation(cue_pattern, steps)`: Chain associations
  - `NeuralAssociativeMemory.clear()`: Clear all associations

#### Development Systems
- **DevelopmentSystem**: Neural development orchestration
  - `DevelopmentSystem.getStage()`: Get current developmental stage
  - `DevelopmentSystem.setStage(stage)`: Set developmental stage
  - `DevelopmentSystem.advanceStage()`: Advance to next stage
  - `DevelopmentSystem.update(brain, step)`: Update development
  - `DevelopmentSystem.isCriticalPeriod()`: Check if in critical period
  - `DevelopmentSystem.getPlasticityModifier()`: Get current plasticity

- **Synaptogenesis**: Synapse formation
  - `Synaptogenesis.getFormationRate()`: Get synapse formation rate
  - `Synaptogenesis.setFormationRate(rate)`: Set formation rate
  - `Synaptogenesis.update(brain, rng)`: Update synapse formation

- **Maturation**: Neural maturation
  - `Maturation.getProgress()`: Get maturation progress (0-1)
  - `Maturation.setProgress(progress)`: Set maturation progress
  - `Maturation.update(brain, step)`: Update maturation
  - `Maturation.getMatureThreshold()`: Get mature threshold

- **Pruning**: Synapse elimination
  - `Pruning.getThreshold()`: Get pruning threshold
  - `Pruning.setThreshold(threshold)`: Set pruning threshold
  - `Pruning.getPruningRate()`: Get pruning rate
  - `Pruning.setPruningRate(rate)`: Set pruning rate
  - `Pruning.pruneSynapse(synapse)`: Force prune specific synapse

#### Advanced Action Selection and Motor Control
- **Action**: Motor action representation
  - `Action.getType()`: Get action type
  - `Action.setType(type)`: Set action type
  - `Action.getParameters()`: Get action parameters
  - `Action.setParameters(params)`: Set action parameters
  - `Action.getName()`: Get human-readable action name
  - `Action.clone()`: Create copy of action

- **AgentBrain**: High-level agent interface
  - `AgentBrain.initialize(world)`: Initialize with world
  - `AgentBrain.processSensoryInput(percept)`: Process sensory input
  - `AgentBrain.decodeMotorCommand()`: Decode motor command from brain
  - `AgentBrain.applyRewardModulation(reward, predicted_reward)`: Apply reward modulation
  - `AgentBrain.updateDevelopment(timestep)`: Update development system

#### Visualization and Monitoring Tools
- **VisualizationInterface**: Real-time visualization with multiple output modes
  - `VisualizationInterface.initialize(config)`: Initialize with configuration
  - `VisualizationInterface.update()`: Update visualization data
  - `VisualizationInterface.render()`: Render to screen
  - `VisualizationInterface.close()`: Close visualization
  - `VisualizationInterface.setVisualizationMode(mode)`: Set visualization mode
  - `VisualizationInterface.getVisualizationMode()`: Get current mode
  - `VisualizationInterface.visualizeNetwork(brain)`: Visualize neural network
  - `VisualizationInterface.visualizeActivity(brain)`: Visualize neural activity
  - `VisualizationInterface.visualizeMemorySystems(brain)`: Visualize memory systems
  - `VisualizationInterface.visualizeNeuromodulation(brain)`: Visualize neuromodulation

- **MonitoringSystem**: Comprehensive real-time monitoring
  - `MonitoringSystem.initialize(brain)`: Initialize with brain reference
  - `MonitoringSystem.update(dt)`: Update monitoring data
  - `MonitoringSystem.startExperiment(experiment)`: Start experiment monitoring
  - `MonitoringSystem.stopExperiment()`: Stop experiment monitoring
  - `MonitoringSystem.generateReport()`: Generate comprehensive status report
  - `MonitoringSystem.exportData(filename, format)`: Export monitoring data
  - `MonitoringSystem.setUpdateInterval(interval)`: Set update interval
  - `MonitoringSystem.enableRealTimeUpdates(enabled)`: Enable/disable real-time updates

#### Advanced Experiment Systems
- **Experiment**: Experiment framework for testing
  - `Experiment.setLearningMode(enabled)`: Enable/disable learning mode
  - `Experiment.setDevelopmentFocus(enabled)`: Set development focus
  - `Experiment.addMetric(name, value)`: Add performance metric

- **ExperimentRunner**: Automated experiment execution
  - `ExperimentRunner.runExperiment(experiment)`: Run experiment
  - `ExperimentRunner.stopExperiment()`: Stop running experiment

### 2. Pythonic API Design

#### Snake Case Naming
All methods and attributes use Pythonic snake_case naming:
- `get_neurons_count()` instead of `getNeuronsCount()`
- `set_learning_rate()` instead of `setLearningRate()`
- `process_sensory_input()` instead of `processSensoryInput()`

#### Property Access
Enhanced classes provide property-like access:
```python
# Property access
brain = pynlm.Brain()
stage = brain.get_developmental_stage()
brain.set_developmental_stage(pynlm.DevelopmentalStage.CriticalPeriod)

# Pythonic property access (if available)
print(brain.developmental_stage)  # If property implemented
brain.developmental_stage = pynlm.DevelopmentalStage.Maturation
```

#### Context Managers
All major classes support context managers for resource management:
```python
# Safe brain operations with automatic cleanup
with brain.context():
    brain.step()
    brain.save("checkpoint.brain")
    # Brain automatically resets on exit

# Safe configuration management
with config.context():
    config.load_from_file("settings.json")
    config.set("learning_rate", 0.01)
    # Config automatically cleared on exit
```

### 3. Advanced Convenience Functions

#### High-Level Learning Controls
```python
# Enable all learning systems
pynlm.enable_all_learning_systems(brain)

# Disable all learning systems
pynlm.disable_all_learning_systems(brain)

# Quick setup for learning experiments
pynlm.setup_learning_experiment(brain, experiment)

# Quick setup for development experiments
pynlm.setup_development_experiment(brain, experiment)
```

#### Memory System Controls
```python
# Configure memory systems with Pythonic API
pynlm.setup_memory_systems(brain, working_memory_capacity=200, episodic_memory_limit=2000)

# Working memory operations
wm = brain.get_working_memory()
wm.store(pattern, strength=1.0)
wm.update(dt)

# Episodic memory operations
em = brain.get_episodic_memory()
em.storeEpisode(episode)
em.replayEpisode(episode)
```

#### Development Stage Controls
```python
# Development stage management
brain.set_developmental_stage(pynlm.DevelopmentalStage.Initial)

# Check developmental conditions
if brain.is_critical_period():
    print("In critical period - high plasticity")

if brain.is_mature():
    print("Brain is mature - stable performance")

# Advance development
pynlm.advance_development_stage(brain)
```

#### Experiment Setup Helpers
```python
# Setup convenience functions
pynlm.setup_basic_brain(brain)      # For simple simulations
pynlm.setup_advanced_brain(brain)   # For complex simulations
```

### 4. Enhanced Documentation

#### Comprehensive Docstrings
All classes and methods include comprehensive docstrings with:
- Description of purpose
- Parameter types and descriptions
- Return value descriptions
- Usage examples
- Error conditions

#### API Reference
- Class definitions with inheritance hierarchy
- Method signatures with parameter types
- Return value types
- Exception documentation
- Example usage snippets

#### Tutorial Examples
- Step-by-step tutorials for beginners
- Advanced usage examples
- Best practices and patterns
- Common use cases and workflows

### 5. Robust Error Handling

#### Custom Exception Classes
```python
import pynlm

try:
    brain = pynlm.Brain()
    brain.initialize()
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
except pynlm.NeuromodulationError as e:
    print(f"Neuromodulation error: {e}")
```

#### Clear Error Messages
All exceptions include:
- Clear, descriptive error messages
- Context about what went wrong
- Suggestions for resolution
- Stack traces for debugging

#### Type Checking and Validation
- Runtime type checking for method parameters
- Validation of configuration values
- Graceful handling of edge cases
- Clear feedback for invalid inputs

#### Graceful Error Recovery
- Automatic cleanup on errors
- State preservation where possible
- Optional error continuation

### 6. Learning and Development Controls

#### Python-Friendly Learning Controls
```python
# Enable learning with Pythonic API
brain.enable_all_learning_systems()

# Control neuromodulation learning
brain.get_dopamine().enable_learning(True)
brain.get_curiosity().enable_learning(True)

# Set development stages
brain.set_developmental_stage(pynlm.DevelopmentalStage.CriticalPeriod)

# Monitor learning progress
print(f"Learning rate: {brain.get_learning_rate()}")
print(f"Plasticity: {brain.get_plasticity_modifier()}")
```

#### Development Stage Management
```python
# Stage progression
stages = [
    pynlm.DevelopmentalStage.Initial,
    pynlm.DevelopmentalStage.CriticalPeriod,
    pynlm.DevelopmentalStage.Maturation,
    pynlm.DevelopmentalStage.Adult,
    pynlm.DevelopmentalStage.Aging
]

for stage in stages:
    brain.set_developmental_stage(stage)
    print(f"Stage: {stage}, Plasticity: {brain.get_plasticity_modifier()}")
```

### 7. Memory System Controls

#### Working Memory Operations
```python
# Working memory is core to NLM's functionality
wm = brain.get_working_memory()

# Store and retrieve information
wm.store([0.1, 0.5, 0.9, 0.2, 0.7], strength=0.8)
memory_pattern = wm.retrieve()

# Monitor working memory state
print(f"Capacity: {wm.getCapacity()}")
print(f"Active traces: {wm.getActiveTraces()}")
print(f"Memory activity: {wm.getMemoryActivity()}")
```

#### Episodic Memory Controls
```python
# Episodic memory stores experiences for learning
em = brain.get_episodic_memory()

# Store experiences
episode = {
    'timestamp': brain.get_simulation_time(),
    'sensory_state': vision_data,
    'action': action,
    'reward': reward_value
}
em.storeEpisode(episode)

# Retrieve similar experiences
similar_episodes = em.retrieveSimilar(query_pattern, max_results=5)

# Replay experiences for consolidation
em.replayEpisode(episode)

# Enable/disable replay
em.enableReplay(True)
```

### 8. Monitoring and Visualization Integration

#### Python Bindings for Monitoring
```python
# Setup monitoring system
monitoring = pynlm.MonitoringSystem()
monitoring.initialize(brain)

# Configure monitoring
monitoring.setUpdateInterval(0.1)  # 10 Hz
monitoring.enableRealTimeUpdates(True)
monitoring.enableMemoryTracking(True)

# Get monitoring data
report = monitoring.generateReport()
print(report)

# Export data
monitoring.exportData("simulation_data.csv", pynlm.ExportFormat.CSV)
```

#### Visualization Integration
```python
# Setup visualization
viz = pynlm.VisualizationInterface()
config = pynlm.VisualizationConfig()
config.windowWidth = 1920
config.windowHeight = 1080
config.showGrid = True
config.showLabels = True

# Initialize and use visualization
if viz.initialize(config):
    viz.visualizeNetwork(brain)
    viz.visualizeActivity(brain)
    viz.visualizeMemorySystems(brain)
    
    # Real-time monitoring
    while viz.isActive():
        viz.update()
        viz.render()
```

### 9. Educational Features

#### Examples and Tutorials
```python
# Basic example
import pynlm

# Create and configure brain
brain = pynlm.PyBrain()
brain.initialize()

# Process sensory input
vision_input = pynlm.PyVision(640, 480)
brain.inject_sensory_input(vision_input)

# Take simulation step
brain.step()

# Get motor action
action = brain.produce_motor_action()

# Use context manager for safe operations
with brain.context():
    # Brain operations
    pass

# Advanced usage
wm = brain.get_working_memory()
em = brain.get_episodic_memory()
dopamine = brain.get_dopamine()
```

#### Interactive Demonstrations
- Jupyter notebook integration
- Real-time visualization examples
- Interactive parameter tuning
- Step-by-step learning guides

#### Step-by-Step Guides
1. **Getting Started**: Install and create your first brain
2. **Basic Simulation**: Run a simple simulation with sensory input and actions
3. **Memory Systems**: Explore working and episodic memory
4. **Neuromodulation**: Understand learning and motivation systems
5. **Development**: Control brain development stages
6. **Advanced Features**: Use visualization and monitoring

## API Reference

### Core Classes

#### PyBrain
- `__init__(config=None)`: Constructor
- `initialize()`: Initialize brain
- `step(step)`: Take simulation step
- `inject_sensory_input(input)`: Inject sensory input
- `produce_action()`: Produce motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `context()`: Get context manager
- Property access: `get_config()`, `get_total_neuron_count()`, etc.

#### PyConfig
- `__init__()`: Constructor
- `load_from_file(filepath)`: Load configuration from file
- `save_to_file(filepath)`: Save configuration to file
- `has(key)`: Check if key exists
- `get(key, default=None)`: Get configuration value
- `set(key, value)`: Set configuration value
- `get_keys()`: Get all keys
- `clear()`: Clear all entries
- `summary()`: Get configuration summary
- `context()`: Get context manager

#### PyAction
- `__init__(type=None, parameters=[])`: Constructor
- `get_type()`: Get action type
- `set_type(type)`: Set action type
- `get_parameters()`: Get parameters
- `set_parameters(params)`: Set parameters
- `get_name()`: Get action name
- `add_parameter(value)`: Add parameter
- `clear_parameters()`: Clear parameters
- `clone()`: Create copy
- `context()`: Get context manager

#### PySensoryInput (Abstract Base)
- `get_type()`: Get sensory input type
- `get_data()`: Get sensory data
- `get_dimensions()`: Get dimensions
- `get_timestamp()`: Get timestamp
- `set_timestamp(timestamp)`: Set timestamp
- `get_value(index)`: Get value at index
- `set_value(index, value)`: Set value at index

#### PyVision (SensoryInput subclass)
- `__init__(width=None, height=None, channels=3)`: Constructor
- `set_data(data)`: Set vision data
- `get_width()`, `get_height()`, `get_channels()`: Get dimensions
- `get_pixel(x, y, channel=0)`: Get pixel value
- `set_pixel(x, y, channel, value)`: Set pixel value

#### PyAudio (SensoryInput subclass)
- `__init__(sampleRate=None, numSamples)`: Constructor
- `set_data(data)`: Set audio data
- `set_sample_rate(rate)`: Set sample rate
- `get_sample_rate()`, `get_num_samples()`: Get audio properties
- `get_sample(index)`: Get sample value
- `set_sample(index, value)`: Set sample value

#### PyInternalSignals (SensoryInput subclass)
- `__init__()`: Constructor
- `add_signal(value)`: Add signal
- `get_signals()`: Get all signals
- `clear_signals()`: Clear signals
- `get_signal_count()`: Get number of signals
- `get_signal(index)`: Get signal at index
- `set_signal(index, value)`: Set signal at index

#### PyAgentBrain
- `__init__(brain)`: Constructor
- `initialize(world)`: Initialize with world
- `process_sensory_input(percept)`: Process sensory input
- `decode_motor_command()`: Decode motor command
- `apply_reward_modulation(reward, predicted_reward)`: Apply reward modulation
- `update_development(timestep)`: Update development
- `get_developmental_stage()`: Get development stage
- `enable_reward_modulation(enabled)`: Enable/disable reward modulation
- `enable_structural_plasticity(enabled)`: Enable/disable structural plasticity
- `enable_development(enabled)`: Enable/disable development
- `enable_curiosity(enabled)`: Enable/disable curiosity
- `reset()`: Reset agent
- `context()`: Get context manager

#### PySimpleWorld
- `__init__()`: Constructor
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `reset()`: Reset world
- `set_agent_start(x, y)`: Set agent start position
- `update(timestep)`: Update world
- `apply_motor_command(cmd, currentTime)`: Apply motor command
- `get_sensory_percept()`: Get sensory percept
- `get_agent_body()`: Get agent body
- `get_width()`, `get_height()`: Get world dimensions
- `context()`: Get context manager

### Convenience Functions

#### Configuration Helpers
- `create_default_config()`: Create default configuration
- `create_example_config(config)`: Create example configuration

#### Brain Setup Helpers
- `create_brain()`: Create and initialize brain
- `create_brain_with_config(config)`: Create brain with custom config
- `setup_basic_brain(brain)`: Setup brain for basic simulation
- `setup_advanced_brain(brain)`: Setup brain for advanced simulation

#### System Setup Helpers
- `setup_memory_systems(brain, **kwargs)`: Setup memory systems
- `setup_neuromodulation(brain, **kwargs)`: Setup neuromodulation
- `setup_learning_mode(brain)`: Enable learning mode
- `setup_development_mode(brain)`: Enable development mode

#### Control Functions
- `advance_development_stage(brain)`: Advance development stage
- `set_development_stage(brain, stage)`: Set development stage
- `enable_all_learning_systems(brain)`: Enable all learning systems
- `disable_all_learning_systems(brain)`: Disable all learning systems

#### Educational Features
- `print_example_usage()`: Print usage examples
- `show_banner()`: Show API banner

### Error Classes

- `NLMException`: Base NLM exception
- `ConfigurationError`: Configuration-related errors
- `MemoryError`: Memory system errors
- `NeuromodulationError`: Neuromodulation system errors
- `DevelopmentError`: Development system errors
- `VisualizationError`: Visualization system errors
- `ExperimentError`: Experiment system errors

### Constants

- `INVALID_NEURON_ID`: Invalid neuron identifier
- `INVALID_SYNAPSE_ID`: Invalid synapse identifier
- `INVALID_REGION_ID`: Invalid region identifier
- `INVALID_POPULATION_ID`: Invalid population identifier

### Version Information

- `__version__`: API version ("2.0.0 Enhanced")
- `__author__`: Author/credit information

## Best Practices

### 1. Resource Management
```python
# Always use context managers for resource safety
with brain.context():
    # Brain operations
    pass

with config.context():
    # Configuration operations
    pass
```

### 2. Error Handling
```python
try:
    brain = pynlm.Brain()
    brain.initialize()
    # ... your code ...
except pynlm.NLMException as e:
    print(f"Error: {e}")
    # Handle error appropriately
except Exception as e:
    print(f"Unexpected error: {e}")
    # Log and re-raise or handle
```

### 3. Configuration
```python
# Create configuration with Pythonic API
config = pynlm.Config()
config.load_from_file("settings.json")

# Set values using Pythonic naming
config.set_learning_rate(0.01)
config.set_neuromodulation_level(0.5)

# Get values
learning_rate = config.get_learning_rate()
```

### 4. Memory Management
```python
# Monitor and manage memory systems
wm = brain.get_working_memory()
wm.setCapacity(1000)  # Set working memory capacity
wm.enableRehearsal(True)  # Enable memory rehearsal

em = brain.get_episodic_memory()
em.setMaxEpisodes(10000)  # Set episode limit
em.enableReplay(True)  # Enable episode replay
```

### 5. Development and Learning
```python
# Control development stages
brain.set_developmental_stage(pynlm.DevelopmentalStage.CriticalPeriod)

# Enable learning systems
pynlm.enable_all_learning_systems(brain)

# Monitor development
if brain.is_critical_period():
    print("High plasticity period - ideal for learning")
```

### 6. Testing and Debugging
```python
# Use example usage for testing
import example_usage
example_usage.main()

# Print API banner for quick reference
pynlm.show_banner()

# Print usage examples
pynlm.print_example_usage()
```

## Installation and Setup

### Prerequisites
- Python 3.7 or higher
- Pybind11 development headers
- C++ compiler with C++11 support

### Building and Installing
```bash
# Build the bindings
mkdir build
cd build
cmake ..
make

# Install
sudo make install
```

### Python Usage
```python
import pynlm

# Quick test
brain = pynlm.Brain()
print(f"Brain created: {brain}")
```

## License and Credits

This enhanced Python API is part of the NLM framework development effort. The bindings provide complete coverage of all NLM systems with a focus on Pythonic design, ease of use, and comprehensive documentation.

### Key Contributors
- NLM Development Team
- Python Bindings Team
- Documentation Team

### Version History
- **Version 2.0.0 Enhanced**: Complete system coverage with Pythonic API
- **Version 1.0.0**: Initial basic bindings

## Support and Community

For support, questions, or contributions:
- Check the NLM documentation
- Join the NLM community
- Report issues on the GitHub repository
- Contribute to the Python bindings

## Conclusion

The enhanced NLM Python API provides a comprehensive, user-friendly interface to the NLM framework, making it accessible to Python developers while maintaining full access to the underlying C++ power and flexibility. The bindings support complete system coverage, Pythonic design patterns, advanced convenience functions, robust error handling, and extensive educational features.

This documentation provides a complete reference for using the enhanced Python API to build neural simulation applications, research systems, and educational tools using the NLM framework.