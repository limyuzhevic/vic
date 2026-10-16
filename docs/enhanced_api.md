# Enhanced Python API for NLM

This document provides an overview of the enhanced Python API for the NLM (Neural Learning Machine) framework.

## Overview

The Enhanced Python API builds upon the original low-level bindings to provide:
- More intuitive and Pythonic interfaces
- Higher-level abstractions and convenience methods
- Advanced configuration and brain management tools
- Visualization and analysis utilities
- Comprehensive error handling and validation
- Extensive examples and documentation

## Key Enhancements

### 1. Pythonic Interfaces

#### Context Managers
- `training_context()` for structured training sessions
- `episode_context()` for agent episodes
- Automatic cleanup and resource management

#### Iterators
- Brain objects are iterable for easy training loops
- Direct iteration over training steps

#### Properties
- Read-only properties for common operations
- Enhanced getters with validation

### 2. Configuration Management

#### ConfigBuilder
- Method chaining for configuration
- Validation of configuration parameters
- Support for nested configuration sections (brain, plasticity, neuromod, simulation, world)

#### EnhancedConfig
- File-based configuration loading
- Configuration parameter grouping
- Comprehensive configuration summaries

### 3. Brain Management

#### EnhancedBrain
- Named brains for easy identification
- State history recording
- Automatic analysis capabilities
- Pythonic iteration interface
- Training context managers

#### BrainManager
- Batch brain creation
- Brain naming and lookup
- Multiple brain management

### 4. Agent Enhancements

#### EnhancedAgent
- Episode context managers
- Automatic reward tracking
- Enhanced neuromodulation statistics
- Pythonic interface for agent operations

### 5. World Enhancements

#### EnhancedWorld
- Default configuration methods
- Object history tracking
- Simulation run methods
- Enhanced configuration helpers

### 6. Analysis Tools

#### BrainAnalysis
- Stability metrics
- Development trajectory analysis
- Spike pattern analysis
- Comprehensive statistics

#### VisualizationTools
- Firing dynamics plotting
- Data export/import (JSON)
- Optional matplotlib integration

### 7. Error Handling

#### EnhancedErrorHandler
- Contextual error messages
- Automatic error recovery suggestions
- Detailed error reporting

#### Validation
- Configuration parameter validation
- Input validation
- Type checking

## API Comparison

### Original API
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

# Get stats
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Enhanced API
```python
import pynlm
from pynlm.enhanced import create_enhanced_brain, training_context

# Create enhanced brain with name
brain = create_enhanced_brain(name="MyBrain")
brain.initialize()

# Use training context manager
with training_context(brain, steps=100, enable_learning=True) as session:
    session.brain.enable_learning(True)
    session.run(100)

# Analyze automatically
analysis = brain.analyze()
print(f"Stability: {analysis.get_stability():.2f}")
print(f"Total spikes: {analysis.stats.get('total_spikes', 0)}")
```

## Installation and Setup

### Prerequisites
```bash
pip install pynlm[dev]  # Install with development dependencies
pip install matplotlib numpy pandas  # For visualization
```

### Basic Usage
```python
import pynlm
from pynlm.enhanced import (
    create_enhanced_brain,
    create_enhanced_agent,
    create_enhanced_world,
    create_enhanced_config,
    ConfigBuilder,
    training_context,
    BrainAnalysis
)

# Create components
config = create_enhanced_config()
brain = create_enhanced_brain(config, name="MyBrain")
world = create_enhanced_world()
agent = create_enhanced_agent(brain, name="MyAgent")

# Initialize
brain.initialize()
world.configure_with_defaults()
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Analyze results
analysis = brain.analyze()
```

## Examples

See the `examples/` directory for comprehensive examples:

- `basic_brain.py` - Basic brain usage
- `config_examples.py` - Configuration examples
- `agent_training.py` - Agent and world examples
- `analysis_examples.py` - Analysis and visualization

## Migration Guide

### Upgrading from Original API

#### Configuration
- **Original**: Direct use of `pynlm.createDefaultConfig()`
- **Enhanced**: Use `ConfigBuilder` for method chaining or `create_enhanced_config()`

#### Brain Creation
- **Original**: `pynlm.createBrain(config)`
- **Enhanced**: `create_enhanced_brain(config, name="MyBrain")`

#### Training
- **Original**: Manual loop with `brain.step(i)`
- **Enhanced**: Use `training_context()` for structured training

#### Analysis
- **Original**: Direct calls to brain statistics methods
- **Enhanced**: Automatic analysis with `brain.analyze()`

#### Error Handling
- **Original**: Basic exception handling
- **Enhanced**: Automatic error handling with contextual messages

## Advanced Features

### 1. Context Managers
```python
# Training context
with training_context(brain, steps=1000, enable_learning=True) as session:
    session.brain.enable_learning(True)
    session.run(1000)
    analysis = session.analyze()

# Episode context
with agent.episode_context(max_steps=1000) as episode:
    world.update(0.1)
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
```

### 2. Configuration Builder
```python
# Method chaining
builder = ConfigBuilder()
builder.set_brain(
    neuron_count=1000,
    connection_probability=0.05
).set_plasticity(
    stdp_enable=True,
    stdp_learning_rate=0.001
).set_neuromod(
    curiosity_enable=True,
    novelty_enable=True
)

config = builder.create()
```

### 3. Brain Analysis
```python
# Automatic analysis
analysis = brain.analyze()

# Get metrics
print(f"Stability: {analysis.get_stability():.2f}")
print(f"Spike patterns: {analysis.get_spike_pattern()}")
print(f"Development trajectory: {len(analysis.get_development_trajectory())} steps")
```

### 4. Visualization
```python
from pynlm.enhanced import VisualizationTools

# Plot firing dynamics
VisualizationTools.plot_firing_dynamics(brain._state_history)

# Export data
VisualizationTools.export_to_json(brain._state_history, "brain_history.json")
```

### 5. Error Handling
```python
from pynlm.enhanced import ValidationError, NeuralNetworkError

try:
    config = ConfigBuilder().set("invalid_key", 123).create()
except ValidationError as e:
    print(f"Configuration error: {e}")
except NeuralNetworkError as e:
    print(f"Neural network error: {e}")
```

## Technical Details

### Module Structure

The enhanced API is organized into several modules:

- `pynlm.enhanced` - Main enhanced API module
- `pynlm.enhanced.config` - Configuration management
- `pynlm.enhanced.brain` - Brain management and analysis
- `pynlm.enhanced.agent` - Agent interfaces
- `pynlm.enhanced.world` - World simulation
- `pynlm.enhanced.visualization` - Visualization tools
- `pynlm.enhanced.errors` - Error handling

### Backward Compatibility

The enhanced API maintains full backward compatibility:

- All original functions are re-exported
- Original classes inherit from enhanced versions
- Existing code continues to work without modification

### Performance Considerations

- Enhanced features are optional and add minimal overhead
- Context managers provide automatic resource management
- Analysis is performed on-demand when requested
- Configuration validation occurs only when explicitly requested

## Troubleshooting

### Common Issues

#### Import Errors
```python
# Error: No module named 'pynlm.enhanced'
Solution: Install with dev dependencies
pip install pynlm[dev]

# Error: Cannot import pynlm
Solution: Check Python environment
python -c "import sys; print(sys.path)"
```

#### Configuration Errors
```python
# Error: Invalid configuration
Solution: Use ConfigBuilder with validation
builder = ConfigBuilder().set("brain.neuron_count", 1000)
builder.validate()
config = builder.create()
```

#### Visualization Errors
```python
# Error: matplotlib not installed
Solution: Install matplotlib for visualization
pip install matplotlib

# Error: No matplotlib available
Solution: Skip visualization or install matplotlib
from pynlm.enhanced import VisualizationTools
try:
    VisualizationTools.plot_firing_dynamics(history)
except ImportError:
    print("Visualization not available - install matplotlib")
```

### Getting Help

- Check the examples in the `examples/` directory
- Read the function docstrings for detailed information
- Visit the NLM project documentation
- Check the issue tracker for common problems

## Future Enhancements

The enhanced API is designed to be extensible:

### Planned Features
- Multi-agent systems
- Real-time simulation
- Custom neural network architectures
- Reinforcement learning integration
- Cloud-based simulation
- Advanced visualization features

### Contribution Guidelines
- Follow the existing code style
- Add comprehensive tests
- Include examples in the examples directory
- Update documentation
- Ensure backward compatibility

## License

The enhanced API is licensed under the same terms as the original NLM framework.
