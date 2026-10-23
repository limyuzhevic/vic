# NLM Python Bindings Examples

This directory contains example scripts demonstrating the enhanced features of the NLM (Neural Learning Machine) Python bindings.

## Overview

These examples showcase the comprehensive Pythonic APIs available in the pynlm package, including:

- Basic brain creation and simulation
- Context manager support for automatic resource management
- Enhanced configuration systems
- Simulation session management with save/load capabilities
- Agent integration APIs
- Visualization support
- Batch operations for efficient simulation
- Event system for simulation monitoring
- Advanced configuration setup
- Memory management utilities

## Available Examples

### 1. Basic Brain Example (`basic_brain.py`)
A simple demonstration of creating a brain, running simulation steps, and accessing brain statistics.

### 2. Enhanced Brain Features (`enhanced_brain.py`)
Demonstrates advanced brain features including context managers, batch operations, and network statistics.

### 3. World Simulation (`world_simulation.py`)
Shows how to create a simulation world and integrate it with brain and agent components.

### 4. Event System (`event_system.py`)
Demonstrates the event system for monitoring spike events, learning events, and simulation events.

### 5. Visualization (`visualization.py`)
Shows visualization capabilities for brain networks and activity patterns.

### 6. Batch Operations (`batch_operations.py`)
Demonstrates efficient batch processing of multiple simulations.

### 7. Quick Start (`quick_start.py`)
A minimal example that demonstrates the essential usage pattern.

## Running Examples

### Prerequisites

- Install the pynlm package: `pip install .`
- Ensure all dependencies are installed (C++ compiler, pybind11, scikit-build-core)

### Running Examples

```bash
# Install the package
pip install .

# Run examples from the examples directory
python -m pynlm.examples

# Or run individual examples
python pynlm/examples/basic_brain.py
python pynlm/examples/enhanced_brain.py
# ... etc
```

## Example Structure

Each example script demonstrates a specific aspect of the NLM Python bindings:

1. **Basic Usage**: Simple brain creation and simulation
2. **Advanced Features**: Context managers, batch operations, enhanced APIs
3. **Agent Integration**: Complete agent workflow with world interaction
4. **Event Handling**: Real-time simulation monitoring
5. **Visualization**: Brain state visualization and analysis
6. **Performance**: Batch processing and parallel simulation

## Key Features Demonstrated

### Context Manager Support
```python
# Automatic resource management
with pynlm.simulation_session(brain, num_steps=1000) as session:
    session.run()
```

### Enhanced Configuration
```python
# Multiple configuration templates
config = pynlm.create_default_config()
config = pynlm.create_experimental_config()
config = pynlm.create_agent_config()
```

### Agent Integration
```python
# Complete agent workflow
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
percept = world.getSensoryPercept()
action = agent.decodeMotorCommand()
result = world.applyMotorCommand(action, time)
```

### Batch Operations
```python
# Efficient batch processing
brains = [createBrain(config) for _ in range(10)]
with pynlm.batch_simulation(brains, stepsPerBrain=1000) as simulator:
    results = simulator.run()
```

### Event System
```python
# Real-time simulation monitoring
event_manager = pynlm.EventManager()
event_manager.registerSpikeHandler(my_spike_handler)
event_manager.registerLearningHandler(my_learning_handler)
```

## Advanced Features

### Simulation Session Management
- Save/load brain states with named sessions
- Resume simulations from checkpoints
- Session history and management

### Memory Management
- Explicit memory allocation and deallocation
- Memory usage tracking
- Memory snapshots and restoration

### Visualization
- Network visualization
- Activity visualization
- Spike raster plots
- Weight matrix visualization

### Development APIs
- Behavioral model creation
- Agent evaluation
- Performance metrics
- Exploration statistics

## Customization

### Configuration Templates
The examples demonstrate various configuration templates:
- `create_default_config()`: Basic configuration
- `create_experimental_config()`: Full feature set
- `create_agent_config()`: Agent-specific settings
- `create_world_config()`: World-specific settings

### Custom Event Handlers
Examples show how to create custom event handlers for:
- Spike events
- Learning events
- Simulation events

### Custom Agent Policies
The event system examples demonstrate how to implement custom agent policies with callbacks.

## Best Practices

1. **Use Context Managers**: Always use context managers for automatic resource management
2. **Leverage Batch Operations**: Use batch operations for multiple simulations
3. **Monitor with Events**: Use the event system for real-time monitoring
4. **Manage Memory**: Use memory management APIs for large-scale simulations
5. **Save Checkpoints**: Regularly save brain states as checkpoints

## Files

- `__init__.py`: Main package initialization
- `examples/`: Directory containing example scripts
- `examples/__main__.py`: Entry point for running examples
- `examples/basic_brain.py`: Basic brain creation example
- `examples/enhanced_brain.py`: Enhanced features example
- `examples/world_simulation.py`: World simulation example
- `examples/event_system.py`: Event system example
- `examples/visualization.py`: Visualization example
- `examples/batch_operations.py`: Batch operations example
- `examples/quick_start.py`: Quick start example

## Getting Started

1. Install the pynlm package: `pip install .`
2. Run the quick start example: `python pynlm/examples/quick_start.py`
3. Explore other examples to discover more features
4. Refer to the documentation in the main package for detailed API reference

## Additional Resources

- [NLM Documentation](https://nlm.readthedocs.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [API Reference](https://nlm.readthedocs.io/en/latest/api.html)

## License

This project is licensed under the MIT License. See the LICENSE file for details.