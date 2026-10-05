# NLM Python Bindings Examples

This directory contains comprehensive examples demonstrating the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The enhanced Python bindings provide a comprehensive, Pythonic interface to the sophisticated neural simulation framework. This directory contains practical examples that showcase:

- Basic usage and fundamental capabilities
- Enhanced sensory processing utilities
- Advanced features and error handling
- Batch processing capabilities
- Complete working demonstrations

## Examples Included

### 1. Basic Example (`basic_example.py`)
A complete, runnable example that demonstrates:
- Basic simulation setup using factory functions
- Enhanced context manager for safe simulation execution
- Error handling and validation
- Statistical analysis and monitoring

### Future Examples
The examples directory will be expanded with additional demonstrations:

- **Vision/Audio Processing**: Advanced sensory data manipulation
- **Performance Optimization**: Batch operations and efficient data handling
- **Machine Learning Integration**: Using NLM with ML frameworks
- **Real-time Applications**: Live simulation with visualization
- **Research Applications**: Custom neural architectures and experiments

## Getting Started

### Prerequisites

1. Install the NLM framework with Python bindings
   ```bash
   # Build and install the NLM framework
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. Install Python dependencies
   ```bash
   pip install nlm-pybindings
   ```

### Running Examples

Run the basic example to see all enhanced features in action:

```bash
python examples/basic_example.py
```

### Key Features Demonstrated

The examples showcase the enhanced capabilities:

1. **Enhanced Error Handling**: Custom exception classes and validation
2. **Pythonic APIs**: Context managers for safe resource management
3. **Utility Classes**: VisionHelper, AudioHelper for data processing
4. **Safe Operations**: Safe save/load functions with comprehensive error handling
5. **Statistical Analysis**: Built-in monitoring and analysis tools
6. **Batch Processing**: Efficient handling of multiple simulations
7. **Backward Compatibility**: Original APIs continue to work seamlessly

## Example Structure

Each example follows this pattern:

```python
import nlm
import time

def example_function():
    # 1. Setup simulation
    world, brain, agent_brain, config = nlm.createSimulationSetup()
    
    # 2. Configure agent
    config.set("agent.name", "example_agent")
    
    # 3. Run simulation with context manager
    with nlm.SimulationContext(brain, max_steps=1000) as sim:
        results = sim.runSteps(500)
        # Process results
    
    # 4. Demonstrate enhanced utilities
    # ...
    
    # 5. Error handling
    # ...
```

## Advanced Features

The examples demonstrate advanced features that make the enhanced bindings powerful:

### 1. Context Managers
```python
with nlm.SimulationContext(brain, max_steps=1000) as sim:
    results = sim.runSteps(500)
    # Automatic cleanup and reset
```

### 2. Enhanced Error Handling
```python
try:
    # Operations that may throw exceptions
    result = unsafe_operation()
except nlm.PyNLMError as e:
    # Handle NLM-specific errors
    print(f"NLM error: {e}")
except nlm.PyValidationError as e:
    # Handle validation errors
    print(f"Validation error: {e}")
except nlm.PyConfigurationError as e:
    # Handle configuration errors
    print(f"Configuration error: {e}")
```

### 3. Utility Classes
```python
from nlm import VisionHelper, AudioHelper, SimulationUtils

# Create vision from Python list
vision = VisionHelper.createVisionFromPyList(image_data)

# Create gradient for testing
vision = VisionHelper.createGradientVision(64, 48)

# Safe save/load
nlm.saveSafely(brain, "brain.nlm")
nlm.loadSafely("brain.nlm", "brain")
```

### 4. Statistical Analysis
```python
with nlm.SimulationContext(brain, max_steps=1000) as sim:
    results = sim.runSteps(500)
    
    print(f"Average firing rate: {results['average_firing_rate']:.4f}")
    print(f"Min firing rate: {results['min_firing_rate']:.4f}")
    print(f"Max firing rate: {results['max_firing_rate']:.4f}")
    print(f"Steps completed: {results['steps_completed']}")
```

## Usage Tips

### 1. For Beginners
- Start with `basic_example.py` to understand the basic workflow
- Use the factory functions (`createSimulationSetup`, `createAgentConfig`) for quick setup
- Take advantage of context managers for safe resource management

### 2. For Advanced Users
- Explore batch processing for multiple simulations
- Use utility classes for custom data processing
- Implement custom error handling for specific use cases
- Integrate with existing ML workflows using the enhanced APIs

### 3. For Performance
- Use batch operations for multiple simulations
- Leverage efficient data transfer utilities
- Implement parallel processing for multiple agents
- Monitor performance with built-in statistics

## Getting Help

### Documentation
- [Enhanced Python Bindings Documentation](README.md in python/ directory)
- [API Reference](docs/)

### Examples
- Each example file includes detailed comments and explanations
- Examples demonstrate best practices and common patterns
- Error messages provide guidance for troubleshooting

### Support
- Check the examples for usage patterns
- Review the enhanced documentation for advanced features
- Report issues with detailed error information and environment

## Future Enhancements

The examples will be continuously updated with:

1. **New Examples**: Additional demonstrations of advanced features
2. **Performance Examples**: Optimized usage patterns for large-scale simulations
3. **Integration Examples**: Combining NLM with other frameworks
4. **Research Examples**: Custom neural architectures and experimental setups

## Conclusion

These examples provide a comprehensive introduction to the enhanced Python bindings. They demonstrate how the bindings make neural simulation accessible, powerful, and flexible while maintaining backward compatibility with existing code.

The enhanced bindings bridge the gap between the sophisticated C++ neural simulation framework and Python's ease of use, making advanced neural computation accessible to a wide range of users and applications.