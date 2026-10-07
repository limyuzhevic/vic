# NLM Enhanced Python API Examples

This directory contains comprehensive examples demonstrating the enhanced Python API
for the NLM (Neural Learning Machine) neural simulation framework.

## Overview

The enhanced Python API provides a much more Pythonic interface to the NLM neural
simulation framework, making it easier to use for beginners while providing
powerful features for experienced users.

## Key Improvements

### 1. Pythonic Context Managers
- Automatic resource management using `with` statements
- Automatic cleanup and reset of brain state
- Safer, more intuitive usage patterns

### 2. Enhanced Configuration API
- Convenience methods for getting/setting configuration values
- Type-safe accessors with default values
- Better error handling and validation
- Pythonic property access patterns

### 3. Python Properties and Accessors
- Properties for common brain statistics (neuron_count, firing_rate, etc.)
- Read-only access to important metrics
- Cleaner, more Pythonic interface

### 4. Convenience Methods
- `runSimulation()` - Run simulations with optional logging
- `runToSteadyState()` - Automatically run to convergence
- Enhanced agent episode management
- Simplified workflow patterns

### 5. Advanced APIs
- Factory functions for different use cases (development, exploration, performance)
- Advanced brain creation with optimized settings
- Visualization helpers and analysis tools
- Training plan generators

### 6. Enhanced Error Handling
- Better exception conversion from C++ to Python
- More informative error messages
- Validation of input parameters

## Examples Included

### Basic Usage Examples
- `example_1_basic_usage()` - Basic brain creation and simulation
- `example_2_context_manager()` - Using context managers for resource management
- `example_3_enhanced_config_api()` - Enhanced configuration API

### Convenience and Integration
- `example_4_convenience_methods()` - Convenience methods for common operations
- `example_5_advanced_configuration()` - Advanced configuration factory functions
- `example_6_agent_simulation()` - Complete agent simulation with world
- `example_7_complete_simulation()` - Complete simulation using advanced factory

### Error Handling and Advanced Features
- `example_8_error_handling()` - Enhanced error handling and validation
- `example_9_advanced_features()` - Advanced features and utilities

## Running the Examples

### Prerequisites

1. Build and install the NLM Python bindings:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   cd ..
   pip install -e .
   ```

2. Install dependencies:
   ```bash
   pip install numpy matplotlib
   ```

### Running Examples

Save this file as `enhanced_api_examples.py` and run:

```bash
python enhanced_api_examples.py
```

Each example is self-contained and can be run individually:

```bash
python -c "from enhanced_api_examples import example_1_basic_usage; example_1_basic_usage()"
```

### Individual Example Files

For easier testing, each example can be run separately:

- `example_1_basic_usage.py` - Basic usage
- `example_2_context_manager.py` - Context managers
- `example_3_enhanced_config_api.py` - Enhanced configuration
- And so on...

## Advanced Usage

### Custom Training Plans

Use the `VisualizationHelper.createTrainingPlan()` method to create custom
training plans for different task types:

```python
helper = pynlm.createVisualizationHelper()
training_plan = helper.createTrainingPlan(config, "exploration")
```

### Performance Optimization

Use the performance-optimized configuration factory:

```python
config = pynlm.createPerformanceOptimizedConfig(
    enableSIMD=True,
    enableMemoryPools=True,
    enablePruning=True
)
```

### Development Workflows

Create development-focused brains:

```python
config = pynlm.createDevelopmentConfig()
brain = pynlm.createDevelopmentBrain("critical")
```

### Visualization and Analysis

Use the visualization helper for analysis:

```python
helper = pynlm.createVisualizationHelper()
metrics = helper.analyzeBrainConnectivity(brain)
visualization_code = helper.createNeuralNetworkGraph(brain, "network.dot")
```

## API Reference

### Core Classes

- **Brain**: Enhanced brain class with Python properties and context manager support
- **Config**: Enhanced configuration class with convenient getter/setter methods
- **AgentBrain**: Enhanced agent brain interface with Pythonic properties
- **SimpleWorld**: Enhanced world simulation with convenience methods

### Factory Functions

- `createDefaultConfig()` - Create default configuration
- `createDevelopmentConfig()` - Create development-focused configuration
- `createExplorationConfig()` - Create exploration-focused configuration
- `createPerformanceOptimizedConfig()` - Create performance-optimized configuration
- `createAdvancedBrain()` - Create advanced brain with optimized settings
- `createExplorationWorld()` - Create exploration world with resources and hazards

### Advanced Utilities

- `createVisualizationHelper()` - Create visualization and analysis helper
- `runCompleteSimulation()` - Run complete simulation with agent and learning

## Error Handling

The enhanced API provides better error handling:

```python
try:
    config = pynlm.createDefaultConfig()
    config.set("invalid.negative.value", -100)
except Exception as e:
    print(f"Configuration error: {e}")

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.runSimulation(100)
except Exception as e:
    print(f"Brain operation error: {e}")
```

## Benefits

### For Beginners
- Simpler, more intuitive API
- Better error messages
- Examples and documentation
- Automatic resource management

### For Experienced Users
- Advanced factory functions
- Customization options
- Performance optimization settings
- Analysis and visualization tools
- Training plan generation

### For Researchers
- Enhanced configuration options
- Development-focused tools
- Performance monitoring
- Export capabilities
- Visualization helpers

## Migration Guide

If you're migrating from the original NLM Python API to the enhanced API:

### Changes

1. **Context Managers**: Replace manual cleanup with `with` statements
2. **Configuration**: Use `config.getInt()`, `config.getFloat()` instead of direct access
3. **Brain Properties**: Use `brain.neuron_count` instead of `brain.getTotalNeuronCount()`
4. **Agent Features**: Use `agent.enableAllFeatures()` for convenience
5. **Simulations**: Use `brain.runSimulation()` for simplified workflows

### Example Migration

```python
# Old way
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)

# New way
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
with pynlm.createBrain(config) as brain:
    brain.initialize()
    brain.runSimulation(100)
```

## Troubleshooting

### Common Issues

1. **Import Error**: Ensure NLM is installed correctly
   ```bash
   pip install -e .
   ```

2. **Build Errors**: Ensure all dependencies are installed
   ```bash
   # Check for CMake, Python development headers, pybind11
   ```

3. **Memory Issues**: Use performance-optimized configurations for large simulations

4. **Configuration Errors**: Use proper configuration keys

5. **Context Manager Issues**: Ensure proper cleanup of resources

### Getting Help

- Check the documentation in the `docs/` directory
- Run individual examples to identify issues
- Use the enhanced error messages for debugging
- Consult the examples for best practices

## Future Enhancements

The enhanced API is designed for extensibility:

1. **Additional Factory Functions**: More specialized configuration creators
2. **Advanced Visualization**: More sophisticated analysis tools
3. **Performance Monitoring**: Real-time performance tracking
4. **Export Capabilities**: More formats for data export
5. **Integration APIs**: Better integration with other tools

## Acknowledgments

This enhanced API was developed to make the NLM neural simulation framework more
accessible and user-friendly while maintaining all the power and flexibility of
the original C++ implementation.

The enhancements were designed by considering:
- Pythonic programming practices
- User experience and documentation
- Performance and scalability
- Extensibility and maintainability
