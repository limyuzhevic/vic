# NLM Python Bindings - Quick Start

## Overview

Welcome to the enhanced NLM (Neural Learning Machine) Python bindings! This guide provides quick access to all the improvements and new features available.

## What's New

### 🚀 Major Improvements

1. **Modular Architecture**: Split monolithic bindings into focused modules for better maintainability
2. **Advanced APIs**: New factory functions, experiment templates, and visualization tools
3. **Enhanced Error Handling**: Specific exception types and comprehensive validation
4. **Real Synaptic Dynamics**: Complete Tsodyks-Markram model implementation
5. **Backward Compatibility**: All original API calls continue to work unchanged

## Quick Test

Run the comprehensive test suite to verify all improvements:

```bash
python3 test_bindings_improvements.py
```

## Getting Started

### 1. Basic Usage (Beginner)

```python
import pynlm

# Create a default brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### 2. Advanced Configuration (Intermediate)

```python
import pynlm

# Create config optimized for reinforcement learning
config = pynlm.create_advanced_config(
    "reinforcement_learning",
    neuron_count=3000,
    curiosity_enabled=True,
    novelty_enabled=True
)

brain = pynlm.createBrain(config)
brain.initialize()
```

### 3. Complete Agent Simulation (Intermediate)

```python
import pynlm

# Setup complete agent with world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation completed successfully!")
```

### 4. Advanced Experiment Management (Expert)

```python
import pynlm

# Create specialized configuration
config = pynlm.create_advanced_config("pattern_recognition")

# Setup experiment with analysis
experiment = pynlm.NLMExperiment(config, "pattern_recognition_test")
results = experiment.run(num_episodes=50)

# Analyze results
print(f"Average spike count: {results['average_spike_count']:.2f}")
print(f"Average firing rate: {results['average_firing_rate']:.2f} Hz")
print(f"Total reward: {results['total_reward']:.2f}")

# Visualize neural connections
brain = experiment.brain
matrix = pynlm.NeuralNetworkVisualizer.create_connection_matrix(brain)
snapshot = pynlm.NeuralNetworkVisualizer.create_brain_snapshot(brain)
```

## Advanced Features

### Experiment Templates

```python
# Use built-in experiment templates
experiment = pynlm.NLMExperiment(config, "my_experiment")
results = experiment.run(num_episodes=100)
```

### Neural Network Visualization

```python
# Create connection matrix visualizations
matrix = pynlm.NeuralNetworkVisualizer.create_connection_matrix(brain)

# Analyze neural activity patterns
activity = pynlm.NeuralNetworkVisualizer.analyze_neural_activity(brain)

# Create comprehensive brain snapshots
snapshot = pynlm.NeuralNetworkVisualizer.create_brain_snapshot(brain)
```

### Batch Processing

```python
# Process multiple episodes efficiently
processor = pynlm.BatchProcessor()
all_results = processor.process_episodes(experiment, 50)
aggregated = processor.aggregate_results(all_results)
```

### Error Handling

```python
# Use enhanced error handling with specific exception types
try:
    config = pynlm.validate_config(config)
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

### System Information

```python
# Get system information and diagnostics
info = pynlm.get_system_info()
print(f"Framework: {info['framework']}")
print(f"Phase: {info['phase']}")
print(f"Version: {info['version']}")
```

### Quick Testing

```python
# Quick verification that brain is working
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

if pynlm.run_quick_test(brain):
    print("Brain is functioning correctly")
else:
    print("Brain test failed - check configuration")
```

## Available Configuration Types

The `create_advanced_config()` function supports these optimized configurations:

- `"reinforcement_learning"`: Optimized for reward-based learning
- `"pattern_recognition"`: Optimized for pattern association tasks
- `"memory_formation"`: Optimized for memory systems
- `"agent_learning"`: Optimized for agent-based learning

## Example: Create Specialized Configurations

```python
# Reinforcement learning configuration
rl_config = pynlm.create_advanced_config(
    "reinforcement_learning",
    neuron_count=2000,
    dopamine_scale=1.5,
    curiosity_enabled=True,
    novelty_enabled=True
)

# Pattern recognition configuration
pr_config = pynlm.create_advanced_config(
    "pattern_recognition",
    neuron_count=1500,
    episodic_capacity=5000,
    associative_capacity=10000
)

# Memory formation configuration
mf_config = pynlm.create_advanced_config(
    "memory_formation",
    neuron_count=3000,
    structural_plasticity_enabled=True,
    development_plasticity_rate=0.1
)
```

## Error Handling Reference

### Custom Exception Types

- `NLMException`: Base exception for all NLM errors
- `ConfigurationError`: Configuration-related errors
- `ValidationError`: Parameter validation errors
- `SimulationError`: Simulation execution errors

### Utility Functions

- `validate_config()`: Validate configuration parameters
- `log_experiment_results()`: Log formatted experiment results
- `BatchProcessor`: Process and aggregate multiple episodes

## Testing

Run the comprehensive test suite:

```bash
python3 test_bindings_improvements.py
```

Expected output:
```
TEST SUMMARY
===================================
Basic Functionality: PASSED
Advanced APIs: PASSED
Error Handling: PASSED
Backward Compatibility: PASSED
System Information: PASSED

Total: 5
Passed: 5
Failed: 0

🎉 All tests passed! NLM Python bindings improvements are working correctly.
```

## Files Created

### New Python Files

- `test_bindings_improvements.py`: Comprehensive test suite
- `analyze_bindings.py`: Binding structure analysis tool

### New C++ Files

- `src/python/core_types.cpp`: Core identifier bindings
- `src/python/enums.cpp`: Enumeration bindings
- `src/python/config_module.cpp`: Configuration system
- `src/python/sensory_module.cpp`: Sensory systems
- `src/python/brain_module.cpp`: Brain core functionality
- `src/python/agent_module.cpp`: Agent interface classes
- `src/python/world_module.cpp`: World simulation
- `src/python/advanced_apis.cpp`: Advanced convenience functions
- `src/python/error_handling.cpp`: Enhanced error handling

### Documentation Files

- `NLM_PYTHON_IMPROVEMENTS.md`: Complete improvement documentation
- `IMPROVEMENTS_COMPLETE.md`: Full implementation summary
- `README.md`: Updated project documentation

## Migration Guide

### If You're Upgrading from Previous Version

✅ **No breaking changes**: All original API calls continue to work
✅ **Enhanced functionality**: New features are available alongside existing API
✅ **Improved error handling**: Better error messages and validation
✅ **Advanced tools**: New APIs for complex use cases

### New Features Available

- `create_advanced_config()`: Factory functions for specialized configurations
- `NLMExperiment`: Structured experiment management templates
- `NeuralNetworkVisualizer`: Advanced visualization tools
- `BatchProcessor`: Batch processing utilities
- Enhanced `Synapse` class: Real synaptic dynamics and diagnostic tools

## Performance Notes

- **Memory usage**: Modular architecture reduces memory footprint
- **Loading time**: Only required modules are loaded
- **Scalability**: Better organization supports larger and more complex simulations
- **Compatibility**: Maintained full backward compatibility

## Troubleshooting

### Common Issues and Solutions

**Issue**: "ImportError: cannot import name 'pynlm'"
**Solution**: Ensure the Python bindings are properly built. Run `pip install .` in the project directory.

**Issue**: "RuntimeError: brain initialization failed"
**Solution**: Check that `brain.initialize()` is called before `brain.step()`. Use `run_quick_test()` for verification.

**Issue**: "ConfigurationError: Missing required parameter"
**Solution**: Use `validate_config()` to check configuration parameters, or use `create_advanced_config()` for pre-built configurations.

## Support

For issues with the improved NLM Python bindings:

1. **Test Suite**: Run `python3 test_bindings_improvements.py` to identify specific failures
2. **Documentation**: Check `NLM_PYTHON_IMPROVEMENTS.md` for detailed usage examples
3. **API Reference**: All new methods are documented with examples
4. **Error Messages**: Specific exception types provide clear error information

## Conclusion

The enhanced NLM Python bindings provide:

- **Better organization** with modular architecture
- **Advanced features** for expert users
- **Improved error handling** with specific exceptions
- **Maintained compatibility** for existing code
- **Comprehensive testing** to ensure reliability

Get started today and discover the full power of the enhanced NLM Python bindings!
