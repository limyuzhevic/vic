# NLM Python API - Enhanced Version

## Overview

This is the enhanced Python API for NLM (Neural Learning Machine), providing comprehensive error handling, type safety, consistent design patterns, and detailed documentation for developers working with the neural simulation framework.

## Key Improvements

### 1. **Enhanced Error Handling**
- **Custom Exception Hierarchy**: Comprehensive exception system with:
  - `NLMException` - Base exception for all NLM errors
  - `ConfigurationError` - Configuration-related errors
  - `BrainInitializationError` - Brain initialization failures
  - `SimulationError` - Simulation execution errors
  - `InvalidParameterError` - Parameter validation errors
  - `ResourceError` - Resource management errors

- **Detailed Error Messages**: Clear, actionable error messages that help identify and fix issues

- **Error Context**: Errors include operation context for better debugging

### 2. **Type Safety and Validation**
- **Parameter Validation**: Comprehensive validation of all parameters
- **Type Checking**: Runtime type checking with clear error messages
- **Range Validation**: Validation of numeric parameter ranges
- **File Path Validation**: Validation of file paths and file existence

### 3. **API Consistency**
- **Uniform Naming**: Consistent parameter and method naming conventions
- **Standardized Signatures**: Consistent method signatures across related classes
- **Return Value Policies**: Standardized return value handling
- **Documentation Style**: Consistent docstring format with parameter and return documentation

### 4. **Enhanced Documentation**
- **Comprehensive Docstrings**: Detailed documentation for all classes, methods, and parameters
- **Usage Examples**: Code examples for common use cases
- **Parameter Descriptions**: Clear descriptions of all method parameters
- **Return Value Documentation**: Detailed descriptions of return values
- **Error Documentation**: Documentation of all possible exceptions

### 5. **Developer Tools**
- **Debug Monitor**: Debugging utilities for monitoring system state
- **Convenience Functions**: High-level helper functions for common tasks
- **Simulation Runner**: Built-in simulation runner with result tracking
- **Validation Utilities**: Type and parameter validation utilities

## Installation

### Prerequisites
- Python 3.8 or higher
- CMake 3.16 or higher
- C++20 compatible compiler

### Installation from Source

```bash
# Clone the repository
cd /path/to/nlm-project

# Install Python package
pip install -e .

# Or install in development mode
pip install -e python/
```

### Quick Start

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Core API Documentation

### Types

#### NeuronId
- **Attributes**: `value` (int)
- **Methods**: `index()`, `__eq__()`, `__ne__()`, `__hash__()`, `__repr__()`, `__str__()`

#### SynapseId
- **Attributes**: `value` (int)
- **Methods**: Similar to NeuronId

#### RegionId
- **Attributes**: `value` (int)
- **Methods**: Similar to NeuronId

#### PopulationId
- **Attributes**: `value` (int)
- **Methods**: Similar to NeuronId

### Enumerations

#### NeuronType
- `Excitatory`: Excitatory neuron that increases activity
- `Inhibitory`: Inhibitory neuron that decreases activity
- `Modulatory`: Modulating neuron affecting neuromodulators
- `Sensory`: Sensory neuron receiving environmental input
- `Motor`: Motor neuron producing action output
- `Internal`: Internal neuron for internal processing

#### ActionType
- `MoveForward`, `MoveBackward`: Movement actions
- `TurnLeft`, `TurnRight`: Rotation actions
- `Look`, `LookUp`, `LookDown`: Looking actions
- `Interact`, `Eat`, `Drink`: Interaction actions
- `Rest`, `Wait`: Resting actions
- `Custom`: Custom action type

#### WorldObjectType
- `Empty`: Empty space
- `Resource`: Resource object
- `Hazard`: Hazardous object
- `Wall`: Wall boundary
- `Marker`: Marker object

### Core Classes

#### Config
**Purpose**: Configuration management for neural systems

**Methods**:
- `loadFromFile(filepath)`: Load configuration from JSON file
- `loadFromArgs(argc, argv)`: Load from command line arguments
- `saveToFile(filepath)`: Save to JSON file
- `has(key)`: Check if key exists
- `getKeys()`: Get all keys
- `clear()`: Clear all entries
- `summary()`: Get configuration summary
- `getOr(key, default)`: Get value or default
- `set(key, value)`: Set value with type support

**Exceptions**:
- `ConfigurationError`: File I/O or parsing errors
- `InvalidParameterError`: Invalid parameter values

#### Brain
**Purpose**: Central neural simulation brain with integrated systems

**Key Methods**:
- `initialize()`: Initialize brain with configuration
- `step(step, time)`: Perform simulation step
- `receiveSensoryInput(input)`: Process sensory input
- `produceAction()`: Generate motor output
- `save(filepath)`, `load(filepath)`: Checkpoint operations
- `getTotalNeuronCount()`, `getTotalSynapseCount()`: Statistics
- `getTotalSpikeCount()`: Spike counting
- `getDevelopmentalStage()`: Get current stage

**Exceptions**:
- `BrainInitializationError`: Initialization failures
- `SimulationError`: Step execution failures
- `InvalidParameterError`: Invalid parameters
- `ResourceError`: File I/O errors

#### SensoryInput and Subclasses
- **SensoryInput**: Base class for all sensory inputs
- **Vision**: Visual input (width, height, channels)
- **Audio**: Audio input (sample rate, samples)
- **InternalSignals**: Internal neural signals

#### Action
**Purpose**: Motor output representation

**Attributes**:
- `type`: ActionType
- `parameters`: List[float]

**Methods**:
- `getType()`: Get action type
- `getParameters()`: Get action parameters
- `getName()`: Get descriptive name
- `clone()`: Create copy

#### World
**Purpose**: Environment simulation

**Classes**:
- **SimpleWorld**: Simple 2D world
- **WorldObject**: Objects in the world (position, type, properties)

**Methods**:
- `configure(width, height, visionWidth, visionHeight)`: Set world dimensions
- `update(timestep)`: Update world state
- `getSensoryPercept()`: Get sensory input
- `getAgentBody()`: Get agent state

#### AgentBrain
**Purpose**: Interface between brain and world

**Key Methods**:
- `processSensoryInput(percept)`: Process sensory data
- `decodeMotorCommand()`: Convert neural activity to action
- `applyRewardModulation(reward, predicted)`: Learn from rewards
- `updateDevelopment(timestep)`: Update developmental state

### Convenience Functions

#### create_brain_with_validation(config=None, **kwargs)
Create a brain with validation and error handling.

#### run_simulation(brain, world=None, agent=None, num_steps=100, **kwargs)
Run a simulation with result tracking and error handling.

#### DebugMonitor(brain, world=None, agent=None)
Debug monitoring utilities for development.

## Error Handling Examples

### Custom Exceptions

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    config.set("invalid_value", "not_a_number")
    
    # This will raise InvalidParameterError
    value = config.getOr("invalid_value", 0)
    
except pynlm.InvalidParameterError as e:
    print(f"Invalid parameter: {e}")
    # Error message includes parameter name, expected type, and actual value
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    
except pynlm.ResourceError as e:
    print(f"Resource error: {e}")
```

### Simulation with Error Handling

```python
import pynlm

try:
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with monitoring
    for step in range(100):
        brain.step(step)
        
except pynlm.SimulationError as e:
    print(f"Simulation failed: {e}")
    
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization failed: {e}")
```

## Advanced Usage

### Agent-World Simulation

```python
import pynlm

# Create complete agent-world system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Debug and Monitoring

```python
import pynlm

# Create debug monitor
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(10, 10, 5, 5)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

monitor = pynlm.DebugMonitor(brain, world, agent)

# Capture system state during simulation
for step in range(20):
    world.update(0.1)
    monitor.capture_state(f"Step {step}")
    # ... simulation code

# Get stability report
report = monitor.get_stability_report()
print(f"Stable: {report['stable']}")
print(f"Issues: {report['issues']}")
print(f"Warnings: {report['warnings']}")
```

### Performance Analysis

```python
import pynlm

# Run performance benchmark
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run many steps
steps = 1000
for step in range(steps):
    brain.step(step)

# Analyze performance
print(f"Performance Statistics:")
print(f"  Steps: {steps}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Average spikes per step: {brain.getTotalSpikeCount() / steps:.2f}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

## Development Tools

### Type Safety Validation

```python
import pynlm

# Create debug monitor for type validation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Test type safety
neuron_id = pynlm.NeuronId(1)
print(f"Created neuron ID: {neuron_id}")

# This would raise InvalidParameterError if invalid
# pynlm._validate_type(123, str, "neuron_id")
```

### Batch Processing

```python
import pynlm
from typing import List, Dict, Any

def run_multiple_simulations(
    num_simulations: int = 10,
    steps_per_simulation: int = 100,
    configs: List[pynlm.Config] = None
) -> List[Dict[str, Any]]:
    """
    Run multiple simulations with error handling.
    
    Args:
        num_simulations: Number of simulations to run
        steps_per_simulation: Steps per simulation
        configs: Optional list of configurations
    
    Returns:
        List of simulation results
    """
    results = []
    
    if configs is None:
        configs = [pynlm.createDefaultConfig() for _ in range(num_simulations)]
    
    for i, config in enumerate(configs):
        try:
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(steps_per_simulation):
                brain.step(step)
            
            results.append({
                'simulation_id': i,
                'success': True,
                'total_spikes': brain.getTotalSpikeCount(),
                'neurons': brain.getTotalNeuronCount(),
                'synapses': brain.getTotalSynapseCount(),
                'firing_rate': brain.getAverageFiringRate(),
            })
            
        except Exception as e:
            results.append({
                'simulation_id': i,
                'success': False,
                'error': str(e),
                'error_type': type(e).__name__,
            })
    
    return results

# Run batch processing
batch_results = run_multiple_simulations(5, 50)
for result in batch_results:
    if result['success']:
        print(f"Simulation {result['simulation_id']}: Success, {result['total_spikes']} spikes")
    else:
        print(f"Simulation {result['simulation_id']}: Failed - {result['error_type']}: {result['error']}")
```

## Testing

### Running Tests

```bash
# Run all tests
pytest tests/ -v

# Run specific test module
pytest tests/test_basic_functionality.py -v

# Run tests with coverage
pytest tests/ --cov=pynlm --cov-report=html

# Run tests with verbose output
pytest tests/ -v --tb=short
```

### Test Categories

1. **BasicFunctionality**: Core brain and configuration operations
2. **ErrorHandling**: Custom exception handling and validation
3. **TypeSafety**: Type checking and parameter validation
4. **AgentWorldIntegration**: Complete agent-world simulation
5. **Performance**: Performance and edge cases
6. **FileOperations**: File I/O operations
7. **Validation**: State consistency and resource cleanup

## Configuration

### Default Configuration
The default configuration includes:
- `random_seed`: 42 (default)
- `simulation_timestep`: 0.001
- `max_energy`: 100.0
- `energy_decay_rate`: 0.01
- And more...

### Custom Configuration
Create custom configurations:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("custom_param", "value")
config.set("number_param", 42)
config.set("float_param", 3.14)
config.set("bool_param", True)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Configuration Files
Save and load configurations to JSON files:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("my_param", "my_value")
config.saveToFile("my_config.json")

# Later...
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

## Advanced Features

### Neuromodulation
Enable and monitor neuromodulation systems:

```python
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

### Developmental Stages
Monitor and control developmental stages:

```python
from pynlm import DevelopmentalStage

brain.setDevelopmentalStage(DevelopmentalStage.Maturation)
stage = brain.getDevelopmentalStage()
print(f"Current stage: {stage}")

# Enable development
agent.enableDevelopment(True)
agent.updateDevelopment(0.1)  # Update with time step
```

### Memory Systems
Integrated memory systems are available through the brain:
- Working memory (persistent activity)
- Episodic memory (experience storage)
- Associative memory (pattern associations)

Access through brain methods:
- `brain.getWorkingMemory()`
- `brain.getEpisodicMemory()`
- `brain.getAssociativeMemory()`

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
- **Check**: Did you call `brain.initialize()`?
- **Solution**: Always call initialize before simulation

#### "The agent isn't moving"
- **Check**: Did you call `world.applyMotorCommand()`?
- **Solution**: Ensure action is applied to world

#### "Everything is 0"
- **Check**: Brains need time to "warm up" - try more steps
- **Solution**: Increase simulation steps, ensure sensory input is connected

#### "TypeError: 'int' object is not iterable"
- **Check**: Parameter type validation failed
- **Solution**: Check parameter types in your code

### Error Diagnosis

```python
import pynlm

try:
    # Your code here
    pass
    
except pynlm.InvalidParameterError as e:
    # Parameter validation error
    print(f"Parameter error: {e}")
    print(f"Expected: {e.expected_value}")
    print(f"Got: {e.actual_value}")
    
except pynlm.BrainInitializationError as e:
    # Brain initialization error
    print(f"Brain init error: {e}")
    
except pynlm.SimulationError as e:
    # Simulation execution error
    print(f"Simulation error: {e}")
    
except pynlm.ResourceError as e:
    # Resource management error
    print(f"Resource error: {e}")
```

## Migration Guide

### From Basic to Enhanced API

#### Before (Basic API)
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(10):
    brain.step(i)
```

#### After (Enhanced API)
```python
import pynlm

# Enhanced API with error handling
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

try:
    brain.initialize()
    
    for i in range(10):
        brain.step(i)
        
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization failed: {e}")
    
except pynlm.SimulationError as e:
    print(f"Simulation failed: {e}")
```

### Using Debug Monitor

```python
import pynlm

# Create debug monitor for development
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# ... setup world and agent ...

monitor = pynlm.DebugMonitor(brain, world, agent)

# Capture state during development
for step in range(20):
    # Your simulation code here
    monitor.capture_state(f"Processing step {step}")
    
    # Check for issues
    if brain.getTotalSpikeCount() == 0:
        print(f"Warning: No spikes at step {step}")

# Get stability report
report = monitor.get_stability_report()
if not report['stable']:
    print("System instability detected!")
    for issue in report['issues']:
        print(f"  - {issue}")
```

## Performance Tips

### For Large Simulations
```python
import pynlm

# Use batch processing for multiple simulations
configs = [pynlm.createDefaultConfig() for _ in range(10)]
results = pynlm.run_batch_simulations(configs, 100, 50)

# Analyze results
for result in results:
    print(f"Simulation completed with {result['total_spikes']} spikes")
```

### For Memory Efficiency
```python
import pynlm

# Reset brain when not in use
brain = pynlm.createBrain(config)
brain.initialize()

# Use brain
brain.step(0)

# Reset when done
brain.reset()

# Re-initialize when needed
brain.initialize()
```

### For Development
```python
import pynlm

# Use debug monitor during development
monitor = pynlm.DebugMonitor(brain, world, agent)

# Capture system state
monitor.capture_state("Initial state")

# Run simulation
for step in range(10):
    # ... simulation code
    monitor.capture_state(f"Step {step}")

# Check for issues
report = monitor.get_stability_report()
print(f"System stable: {report['stable']}")
```

## Future Enhancements

The enhanced Python API provides a foundation for future improvements:

### Planned Features
- **Distributed Computing**: Parallel simulation across multiple processes
- **Web API**: REST API for remote brain control
- **Visual Monitoring**: Real-time visualization of brain activity
- **Advanced Machine Learning**: Integration with ML frameworks
- **GPU Acceleration**: GPU-accelerated simulation

### Contribution Guidelines
- Report issues with detailed error messages
- Submit pull requests with comprehensive tests
- Follow the existing code style and conventions
- Add documentation for new features
- Maintain backward compatibility

## Support

### Getting Help
- **Documentation**: `help(pynlm)` or `pynlm.__doc__`
- **Examples**: Run the demo script: `python demo.py`
- **Tests**: Run test suite: `pytest tests/`
- **GitHub**: Report issues at https://github.com/nlm-project/nlm/issues

### Community
- **Discussions**: GitHub Discussions
- **Issues**: GitHub Issues
- **Contributing**: CONTRIBUTING.md
- **Code of Conduct**: CODE_OF_CONDUCT.md

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

This enhanced Python API was developed to provide a robust, user-friendly interface to the NLM neural simulation framework. It builds upon the existing foundation while adding comprehensive error handling, type safety, and developer tools.

Special thanks to the NLM research team for their contributions to the neural simulation framework.
