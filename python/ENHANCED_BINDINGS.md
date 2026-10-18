# NLM Python Bindings - Enhanced Documentation

# Introduction

This document provides comprehensive documentation for the enhanced NLM (Neural Learning Machine) Python bindings. These bindings offer a Pythonic interface to the C++ neural simulation framework with improved error handling, convenience methods, performance monitoring, and context management.

## Key Improvements

### 1. Enhanced Error Handling
- Custom Python exceptions (`NLMException`, `ConfigurationError`, `InitializationError`, `SimulationError`)
- Detailed error messages with context
- Proper exception chaining

### 2. Pythonic Convenience Methods
- Property-style access for frequently used attributes
- Context managers for automatic resource cleanup
- Rich comparison methods (`__repr__`, `__eq__`, `__hash__`)
- Container-like methods for collections
- Statistics dictionaries and helpers

### 3. Performance Monitoring APIs
- Performance metrics collection
- Benchmarking utilities
- Profiling tools
- System information reporting

### 4. Advanced Simulation Utilities
- Learning experiment runners
- Episode management
- Checkpoint management
- Neuroscience-specific utilities

## Installation

### Prerequisites
- Python 3.8+
- C++ compiler supporting C++20
- CMake 3.16+
- pybind11 and scikit-build-core

### Installation from Source

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install in development mode
pip install -e .

# Or build only
python -m build --wheel
pip install dist/*.whl
```

## Quick Start Guide

### Basic Usage

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Use as context manager for automatic cleanup
with brain:
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Get statistics
    stats = brain.getStatistics()
    print(f"Total neurons: {stats['neuron_count']}")
```

### Complete Agent Example

```python
import pynlm

def run_complete_agent_simulation():
    # Setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    num_steps = 500
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain, world, agent

# Run simulation
brain, world, agent = run_complete_agent_simulation()
```

## Enhanced Brain API

### Core Methods

#### Initialization and Lifecycle
```python
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this first!

# Using as context manager (automatic cleanup)
with brain:
    brain.step(0)
    # Cleanup happens automatically on exit

brain.reset()  # Reset to initial state
```

#### Simulation Control
```python
brain.step(step)           # Basic step
brain.step(step, time)      # Step with timestamp
brain.injectCurrent(neuron, current)
brain.injectCurrentToNeurons(neuron_type, current)

# Wait for conditions with timeout
brain.waitForSpikes(min_spikes=10, max_steps=1000)
```

#### Statistics and Monitoring
```python
stats = brain.getStatistics()
# Returns dictionary with:
# - neuron_count
# - synapse_count  
# - firing_neuron_count
# - active_neuron_count
# - average_firing_rate
# - excitation_inhibition_ratio
# - total_spike_count

# Check if brain is initialized
if brain.isInitialized():
    print("Brain is ready for simulation")

# Get random seed
seed = brain.getRandomSeed()
brain.setRandomSeed(123)  # For reproducible results
```

### Advanced Brain Features

#### Statistics and Diagnostics
```python
# Get detailed statistics
brain_stats = {
    'neurons': brain.getTotalNeuronCount(),
    'synapses': brain.getTotalSynapseCount(),
    'firing': brain.getFiringNeuronCount(),
    'active': brain.getActiveNeuronCount(),
    'rate': brain.getAverageFiringRate(),
    'balance': brain.getExcitationInhibitionRatio(),
    'spikes': brain.getTotalSpikeCount()
}

# Development stage
stage = brain.getDevelopmentalStage()
print(f"Current development stage: {stage}")
```

#### Configuration Management
```python
config = brain.getConfig()
if config.has("brain.neuron_count"):
    neurons = config.getOr("brain.neuron_count", 1000)

config.set("brain.neuron_count", 500)
config.saveToFile("my_config.cfg")
```

## Enhanced AgentBrain API

### Core Agent Methods
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Process sensory input
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Get action from brain
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

### Advanced Agent Features
```python
# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check subsystem status
if agent.isRewardModulationEnabled():
    print("Reward modulation is active")

# Get neuromodulation levels
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
```

### Episode Management
```python
# Run complete episode
steps_completed = agent.runEpisode(world, numSteps=200, timestep=0.1)

# Get episode statistics
episode_stats = agent.getEpisodeStatistics(numSteps=500)
```

## Enhanced SimpleWorld API

### Basic World Operations
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Set agent start position
world.setAgentStart(10.0, 10.0)

# Update world
world.update(0.1)  # 0.1 second timestep
```

### Advanced World Features
```python
# Create and add world objects
resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(resource)

# Check valid positions
if world.isValidPosition(15.0, 15.0):
    print("Position is valid")

# Run episode with agent
world.runEpisode(agent, numSteps=300)

# Get world statistics
world_stats = world.getWorldStatistics()
```

## Performance Monitoring APIs

### Performance Metrics
```python
from pynlm import PerformanceMetrics

metrics = PerformanceMetrics()
metrics.recordNeuron(100)
metrics.recordSynapse(500)
metrics.recordSpike(25)

print(f"Neurons processed: {metrics.getNeuronsProcessed()}")
print(f"Total spikes: {metrics.getTotalSpikes()}")
print(f"Average firing rate: {metrics.getAverageFiringRate()} Hz")
```

### Benchmarking
```python
# Benchmark simulation
metrics = pynlm.benchmarkSimulation(brain, numSteps=1000, 
                                    description="Small brain simulation")

# Profile with detailed output
metrics = pynlm.profileSimulation(brain, numSteps=500)
```

### System Information
```python
info = pynlm.getSystemInfo()
print(f"Version: {info['version']}")
print(f"Description: {info['description']}")
```

## Simulation Utilities

### Learning Experiments
```python
# Run multiple learning episodes
episode_results = pynlm.runLearningExperiment(
    brain, world, agent, 
    numEpisodes=10, timestep=0.1
)

# Each episode result is a dictionary with statistics
for i, episode in enumerate(episode_results):
    print(f"Episode {i}: {episode['total_spikes']} spikes, "
          f"{episode['total_actions']} actions")
```

### Configuration Utilities
```python
# Create random configuration
config = pynlm.createRandomConfig(seed=42, neuronCount=1000)
pynlm.validateConfiguration(config)  # Will raise exception if invalid
```

### Checkpoint Management
```python
# Save model checkpoint
pynlm.saveModelCheckpoint(brain, "model_checkpoint.bin", 
                          "Best performing model after training")

# Load model checkpoint
loaded_brain = pynlm.loadModelCheckpoint("model_checkpoint.bin")
loaded_brain.initialize()
```

## Neuroscience-Specific Utilities

### Sensory Stimulation
```python
# Create sensory stimulation pattern
pattern = [1.0, 0.5, 0.2, 0.0, 0.0]  # Different intensities
pynlm.createSensoryStimulation(brain, pattern, current=50.0)
```

### Neural Synchronization Measurement
```python
sync_results = pynlm.measureNeuralSynchronization(brain, durationSteps=100)
print(f"Synchronization index: {sync_results['synchronization_index']:.3f}")
print(f"Mean spikes per step: {sync_results['mean_spikes_per_step']:.2f}")
```

## Error Handling

### Custom Exceptions
```python
import pynlm

try:
    # This will raise an exception if invalid
    config = pynlm.createRandomConfig(neuronCount=-1)
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.InitializationError as e:
    print(f"Initialization error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
```

## Examples Directory

The enhanced bindings include comprehensive examples in the `examples/` directory:

### 1. Basic Brain Simulation
- `basic_brain.py`: Simple brain simulation
- Demonstrates core brain functionality

### 2. Complete Agent System
- `complete_agent.py`: Full agent-world interaction
- Shows learning and development

### 3. Performance Benchmarking
- `benchmark.py`: Performance testing utilities
- Demonstrates profiling and metrics

### 4. Learning Experiments
- `learning_experiments.py`: Systematic learning studies
- Shows episode management and analysis

### 5. Advanced Features
- `advanced_features.py`: Demonstrates context managers
- Shows error handling and utilities

## API Reference

### Class Summary

| Class | Purpose |
|-------|---------|
| `Brain` | Central neural simulation brain |
| `AgentBrain` | Agent interface for brain-world interaction |
| `SimpleWorld` | Simple 2D simulation world |
| `Config` | Configuration management |
| `Action` | Motor action representation |
| `WorldObject` | World object representation |
| `AgentBody` | Agent physical state |
| `ActionResult` | Action execution results |
| `SensoryPercept` | Sensory input from world |
| `PerformanceMetrics` | Performance measurement |

### Module Functions

| Function | Purpose |
|----------|---------|
| `createDefaultConfig()` | Create default configuration |
| `createBrain(config)` | Create brain with config |
| `createSimpleWorld()` | Create simulation world |
| `createAgentBrain(brain)` | Create agent interface |
| `benchmarkSimulation(brain, steps, desc)` | Benchmark performance |
| `profileSimulation(brain, steps)` | Profile execution |
| `runLearningExperiment(brain, world, agent, episodes)` | Run learning studies |
| `validateConfiguration(config)` | Validate configuration |
| `createRandomConfig(seed, neurons)` | Create randomized config |

## Migration Guide

### From Basic to Enhanced API

**Basic API:**
```python
# Old way - minimal functionality
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
```

**Enhanced API:**
```python
# New way - with convenience methods and error handling
brain = pynlm.createBrain(config)
brain.initialize()

# Use context manager for safety
with brain:
    brain.step(0)
    stats = brain.getStatistics()
```

### Key Improvements

1. **Better Error Handling**: Custom exceptions instead of crashes
2. **Convenience Methods**: `getStatistics()`, `isInitialized()`, `setRandomSeed()`
3. **Context Management**: Automatic cleanup with `with brain:`
4. **Performance Monitoring**: Built-in benchmarking and profiling
5. **Advanced Utilities**: Learning experiments, checkpoints, neuroscience tools
6. **Type Safety**: Better parameter validation and type checking
7. **Documentation**: Comprehensive docstrings and examples

## Troubleshooting

### Common Issues

**Issue: "ImportError: No module named 'pynlm'"**
```bash
# Solution: Reinstall the module
pip install --force-reinstall .
```

**Issue: "Segmentation fault"**
```python
# Solution: Always call initialize() before step()
brain = pynlm.createBrain(config)
brain.initialize()  # <- This is required!
brain.step(0)       # Now safe to use
```

**Issue: Configuration validation errors**
```python
# Solution: Use validation utilities
config = pynlm.createRandomConfig(42, 1000)
try:
    pynlm.validateConfiguration(config)
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Config error: {e}")
```

### Performance Tips

1. **Use context managers**: `with brain:` ensures cleanup
2. **Set random seeds**: `brain.setRandomSeed(42)` for reproducible results
3. **Monitor performance**: Use `benchmarkSimulation()` for timing
4. **Batch operations**: Process multiple steps before checking statistics

## Future Enhancements

The enhanced bindings provide a foundation for future improvements:

1. **Advanced Machine Learning**: Integrate modern ML algorithms
2. **GPU Acceleration**: Support for GPU-accelerated neural computation
3. **Distributed Computing**: Multi-agent and multi-world simulations
4. **Advanced Visualization**: Real-time neural activity visualization
5. **Cloud Integration**: Support for distributed computing platforms
6. **Scientific Computing**: Integration with NumPy, SciPy, and other scientific libraries

## Conclusion

The enhanced NLM Python bindings provide a powerful, Pythonic interface to the neural simulation framework. With improved error handling, convenience methods, performance monitoring, and comprehensive utilities, users can now build sophisticated neural simulations with ease while maintaining access to the full power of the C++ implementation.

The enhanced bindings maintain backward compatibility while adding significant new functionality for both beginners and advanced users. The combination of Pythonic interfaces, comprehensive documentation, and performance monitoring tools makes NLM more accessible and powerful than ever before.

## References

1. NLM Project Documentation: https://github.com/nlm-project/nlm
2. Pybind11 Documentation: https://pybind11.readthedocs.io/
3. C++20 Best Practices: ISO/IEC 14882:2020
4. Neural Network Simulation: Current research in artificial developmental systems

## Acknowledgments

This enhanced version of the NLM Python bindings builds upon the foundational work of:

- The NLM project developers and researchers
- The pybind11 community for Python binding technology
- The C++ community for modern language features
- The open-source community for supporting tools and libraries

Special thanks to all contributors who have helped improve the neural simulation framework.
