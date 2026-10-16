# Enhanced API - Module initialization

This module provides the enhanced Python API for NLM (Neural Learning Machine)
with Pythonic interfaces, advanced configuration tools, visualization,
analysis, and comprehensive error handling.

The enhanced API is designed to be more intuitive and easier to use than
the original low-level bindings, while maintaining full backward
compatibility.

## Module Structure

The enhanced API is organized into the following submodules:

1. `core` - Core classes and utilities
2. `config` - Configuration management
3. `brain` - Brain management and analysis
4. `agent` - Agent interfaces
5. `world` - World simulation
6. `visualization` - Visualization tools
7. `errors` - Error handling and validation

Each submodule can be imported individually for specific functionality.

## Quick Start

Basic usage of the enhanced API:

    from pynlm.enhanced import (
        create_enhanced_brain,
        create_enhanced_config,
        create_enhanced_agent,
        create_enhanced_world,
        training_context,
        ConfigBuilder
    )

    # Create components with enhanced interfaces
    config = create_enhanced_config()
    brain = create_enhanced_brain(config, name="MyBrain")
    world = create_enhanced_world()
    agent = create_enhanced_agent(brain, name="MyAgent")

    # Initialize components
    brain.initialize()
    world.configure_with_defaults()
    world.reset()
    agent.initialize(world)

    # Use training context manager
    with training_context(brain, steps=100, enable_learning=True) as session:
        session.brain.enable_learning(True)
        session.run(100)
        analysis = session.analyze()

    # Analyze and visualize results
    print(f"Stability: {analysis.get_stability():.2f}")
    print(f"Total spikes: {analysis.stats.get('total_spikes', 0)}")

## Key Features

### 1. Context Managers
- `training_context()` for structured training sessions
- `episode_context()` for agent episodes
- Automatic resource management and cleanup

### 2. Configuration Builders
- `ConfigBuilder` for method chaining
- `EnhancedConfig` with validation
- Configuration parameter grouping

### 3. Brain Management
- `EnhancedBrain` with state history
- `BrainManager` for batch operations
- Automatic analysis capabilities

### 4. Agent Interfaces
- `EnhancedAgent` with episode context managers
- Automatic reward tracking
- Enhanced neuromodulation statistics

### 5. World Simulation
- `EnhancedWorld` with configuration helpers
- Object history tracking
- Simulation run methods

### 6. Analysis Tools
- `BrainAnalysis` for comprehensive metrics
- `VisualizationTools` for data visualization
- Development trajectory analysis

### 7. Error Handling
- `ErrorHandler` with contextual messages
- `ValidationError` and `NeuralNetworkError`
- Automatic error recovery suggestions

## Installation

To use the enhanced API:

```bash
pip install pynlm[dev]  # Install with development dependencies
pip install matplotlib numpy pandas  # For visualization
```

Import the enhanced API:

```python
import pynlm
from pynlm.enhanced import (
    create_enhanced_brain,
    create_enhanced_config,
    create_enhanced_agent,
    create_enhanced_world,
    training_context,
    ConfigBuilder
)
```

## Backward Compatibility

The enhanced API maintains full backward compatibility:

- All original functions are available through `pynlm`
- Original classes are accessible directly
- Existing code continues to work without modification

## Examples

Run the examples in the `examples/` directory:

```bash
python examples/basic_brain.py
python examples/config_examples.py
python examples/agent_training.py
python examples/analysis_examples.py
```

## Documentation

For comprehensive documentation, see:

- `docs/enhanced_api.md` - Complete API reference
- `examples/README.md` - Working examples
- Function docstrings in the enhanced_api module

## Advanced Usage

### Custom Configuration

```python
from pynlm.enhanced import ConfigBuilder

# Method chaining for configuration
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

### Brain Analysis

```python
from pynlm.enhanced import BrainAnalysis

# Analyze brain activity
analysis = brain.analyze()

# Get metrics
print(f"Stability: {analysis.get_stability():.2f}")
print(f"Spike patterns: {analysis.get_spike_pattern()}")
print(f"Development trajectory: {len(analysis.get_development_trajectory())} steps")
```

### Visualization

```python
from pynlm.enhanced import VisualizationTools

# Plot firing dynamics
VisualizationTools.plot_firing_dynamics(brain._state_history)

# Export data
VisualizationTools.export_to_json(brain._state_history, "brain_history.json")
```

### Error Handling

```python
from pynlm.enhanced import ValidationError, NeuralNetworkError

try:
    config = ConfigBuilder().set("brain.neuron_count", 1000).create()
except ValidationError as e:
    print(f"Configuration error: {e}")
except NeuralNetworkError as e:
    print(f"Neural network error: {e}")
```

## Module Subpackages

### core
Core classes and utilities for the enhanced API.

### config  
Configuration management with builders and validation.

### brain
Brain management and analysis tools.

### agent
Agent interfaces with enhanced features.

### world
World simulation with enhanced interfaces.

### visualization
Visualization tools for data analysis.

### errors
Error handling and validation utilities.

## Troubleshooting

### Common Issues

#### Import Errors
- **Error**: No module named 'pynlm.enhanced'
- **Solution**: Install with dev dependencies
  ```bash
  pip install pynlm[dev]
  ```

#### Configuration Errors
- **Error**: Invalid configuration
- **Solution**: Use ConfigBuilder with validation
  ```python
  builder = ConfigBuilder().set("brain.neuron_count", 1000)
  builder.validate()
  config = builder.create()
  ```

#### Visualization Errors
- **Error**: matplotlib not installed
- **Solution**: Install matplotlib
  ```bash
  pip install matplotlib
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
