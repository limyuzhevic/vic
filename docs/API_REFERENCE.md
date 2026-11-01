# Enhanced NLM Python Bindings

This document describes the improvements made to the NLM (Neural Learning Machine) Python bindings to make them more powerful, user-friendly, and feature-rich.

## Summary of Improvements

The NLM Python bindings have been significantly enhanced with the following features:

### 1. **Advanced Configuration API**
- **Dictionary-based Configuration**: Create and configure brains using Python dictionaries
- **Configuration Templates**: Ready-to-use configuration templates for common use cases
- **Configuration Validation**: Built-in validation with detailed error messages
- **Configuration Management**: Advanced configuration management tools

### 2. **Enhanced Brain Inspection and Debugging**
- **Brain Inspection**: Detailed inspection reports for brain state
- **Performance Profiling**: Performance profiling tools
- **Optimization Hints**: Automatic optimization suggestions
- **Configuration Analysis**: Detailed configuration analysis

### 3. **Batch Operations for Performance**
- **Batch Current Injection**: Efficiently inject current to multiple neurons
- **Batch Statistics**: Get batch statistics for neuron populations
- **Performance Optimization**: Performance-focused operations

### 4. **Learning Rate Control**
- **Adjustable Learning Rates**: Fine-tune plasticity parameters
- **STDP and Hebbian Control**: Separate control over different plasticity mechanisms
- **Dynamic Adjustments**: Adjust learning rates during simulation

### 5. **Simple Simulation Tools**
- **Quick Simulation Creation**: Easy creation of simple simulations
- **Brain Reset Tools**: Reset and reinitialize brain simulations
- **Template Simulations**: Pre-configured simulation templates

### 6. **Error Handling and Validation**
- **Configuration Validation**: Comprehensive validation with detailed error messages
- **Robust Error Handling**: Graceful handling of invalid configurations
- **Helpful Error Messages**: Clear, actionable error messages

### 7. **Documentation and Examples**
- **Comprehensive Documentation**: Detailed documentation with examples
- **Interactive Demos**: Interactive demonstration scripts
- **Usage Guidelines**: Best practices and usage guidelines

## Installation and Usage

### Prerequisites
- Python 3.8 or higher
- NLM C++ library built with Python bindings

### Basic Installation
```bash
# Install the NLM Python package
pip install pynlm

# Or install in development mode
pip install -e .
```

### Basic Usage
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Set configuration parameters
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 2)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation completed. Neurons: {brain.getTotalNeuronCount()}")
```

## Advanced Features

### Configuration from Dictionary
```python
# Advanced configuration using dictionary
advanced_params = {
    "brain.neuron_count": 3000,
    "brain.region_count": 4,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.ltp_weight": 0.025,
    "plasticity.stdp.ltd_weight": 0.02,
    "neuromod.dopamine.enable": True,
    "neuromod.dopamine.baseline": 0.15,
    "memory.working.capacity": 500,
    "performance.simd.enable": True,
    "debug.log_level": "INFO"
}

config = pynlm.createConfigFromDict(advanced_params)
brain = pynlm.createBrain(config)
brain.initialize()
```

### Batch Operations
```python
# Batch operations for better performance
pynlm.batchInjectCurrent(brain, pynlm.NeuronType.Excitatory, 50.0, 10)

# Get batch statistics for excitatory neurons
stats = pynlm.batchGetNeuronStats(brain, pynlm.NeuronType.Excitatory)
print(f"Stats - Count: {stats[0]}, Avg potential: {stats[1]:.3f}")
```

### Brain Inspection
```python
# Get detailed brain inspection
inspection = pynlm.inspectBrain(brain, verbose=True)
print(inspection)

# Get performance optimization hints
hints = pynlm.optimizeBrainConfig(config)
print(f"Optimization hints:\n{hints}")
```

### Learning Rate Control
```python
# Adjust learning rates
result = pynlm.adjustLearningRates(brain, 1.2, 0.8)
print(f"Learning rates adjusted: {result}")
```

### Configuration Validation
```python
# Validate configuration
validation = pynlm.validateBrainConfig(config)
if "valid" in validation.lower():
    print("Configuration is valid!")
else:
    print(f"Configuration issues:\n{validation}")
```

### Performance Profiling
```python
# Profile brain performance
profile_result = pynlm.profileBrain(brain, 100)
print(f"Performance profile:\n{profile_result}")
```

### Simple Simulation
```python
# Create simple simulation quickly
simple_brain = pynlm.createSimpleSimulation(100, 20)
print(f"Simple simulation created with {simple_brain.getTotalNeuronCount()} neurons")

# Reset brain simulation
result = pynlm.resetBrainSimulation(brain)
print(f"Brain reset: {result}")
```

## Configuration Templates

### Fast Training Configuration
```python
fast_training_config = {
    "brain.neuron_count": 1000,
    "plasticity.stdp.ltp_weight": 0.01,
    "plasticity.stdp.ltd_weight": 0.005,
    "performance.simd.enable": True,
    "debug.log_level": "WARN"
}

config = pynlm.createConfigFromDict(fast_training_config)
```

### Precision Configuration
```python
precision_config = {
    "brain.neuron_count": 5000,
    "brain.initial_weight_std": 0.05,
    "plasticity.stdp.ltp_weight": 0.015,
    "plasticity.stdp.ltd_weight": 0.01,
    "plasticity.structural.enable": True,
    "memory.working.capacity": 500,
    "debug.log_level": "DEBUG"
}

config = pynlm.createConfigFromDict(precision_config)
```

### Curiosity-Driven Exploration
```python
exploration_config = {
    "brain.neuron_count": 2000,
    "neuromod.curiosity.enable": True,
    "neuromod.curiosity.threshold": 0.7,
    "development.enable": True,
    "development.initial_stage": "CriticalPeriod"
}

config = pynlm.createConfigFromDict(exploration_config)
```

## Error Handling

### Configuration Validation Examples
```python
# Test invalid configuration
invalid_config = pynlm.createDefaultConfig()
invalid_config.set("brain.neuron_count", -10)
validation = pynlm.validateBrainConfig(invalid_config)
print(f"Validation: {validation}")

# Test valid configuration
valid_config = pynlm.createDefaultConfig()
valid_config.set("brain.neuron_count", 500)
validation = pynlm.validateBrainConfig(valid_config)
print(f"Validation: {validation}")
```

## Performance Features

### Large Configuration Handling
```python
# Large configuration example
large_params = {
    "brain.neuron_count": 10000,
    "brain.region_count": 10,
    "plasticity.stdp.ltp_weight": 0.01,
    "memory.working.capacity": 5000,
    "performance.simd.enable": True
}

large_config = pynlm.createConfigFromDict(large_params)
validation = pynlm.validateBrainConfig(large_config)
print(f"Large config validation: {validation}")
```

### Performance Optimization
```python
# Get optimization hints
hints = pynlm.optimizeBrainConfig(large_config)
print(f"Optimization hints:\n{hints}")
```

## Examples and Demos

### Example Scripts
The following example scripts demonstrate the enhanced features:

1. **`test_bindings.py`**: Comprehensive test suite for all new features
2. **`demo_enhanced_bindings.py`**: Interactive demonstration of all features
3. **`docs/CONFIGURATION_GUIDE.md`**: Detailed configuration guide with examples
4. **`docs/API_REFERENCE.md`**: Complete API reference

### Running Examples
```bash
# Run the test suite
python test_bindings.py

# Run the interactive demo
python demo_enhanced_bindings.py

# View documentation
# docs/CONFIGURATION_GUIDE.md
```

## Best Practices

### Configuration Best Practices
1. **Use Descriptive Names**: Use clear, descriptive configuration parameter names
2. **Validate Before Use**: Always validate configuration before using
3. **Use Templates**: Use templates for common configurations
4. **Document Parameters**: Document all configuration parameters
5. **Test Configuration**: Test configuration before deployment

### Python Best Practices
1. **Type Safety**: Use type-safe configuration access
2. **Error Handling**: Handle configuration errors gracefully
3. **Documentation**: Document configuration usage
4. **Testing**: Test configuration functionality
5. **Logging**: Log configuration changes

## Migration Guide

### From Basic to Advanced Usage
If you're migrating from basic NLM usage to advanced usage:

1. **Start with Basic**: Begin with basic usage and gradually explore advanced features
2. **Use Templates**: Use configuration templates to get started quickly
3. **Validate Configuration**: Always validate configuration before use
4. **Test Performance**: Test performance before deployment
5. **Document Usage**: Document your configuration and usage patterns

## Troubleshooting

### Common Issues and Solutions

1. **Import Error**: If `pynlm` cannot be imported
   - Ensure NLM is installed with Python bindings
   - Check if dependencies are installed
   - Verify Python path

2. **Configuration Validation Errors**
   - Check parameter names and types
   - Use configuration templates
   - Validate configuration before use

3. **Performance Issues**
   - Use batch operations for better performance
   - Enable SIMD optimizations
   - Use configuration optimization hints

4. **Brain Simulation Issues**
   - Always call `initialize()` before `step()`
   - Check configuration parameters
   - Use simple simulation tools for testing

## Conclusion

The enhanced NLM Python bindings provide a comprehensive and powerful interface for working with the NLM neural simulation framework. With advanced configuration options, batch operations, brain inspection tools, and comprehensive documentation, users can now easily create, configure, and analyze NLM simulations with unprecedented flexibility and power.

These improvements make NLM more accessible to beginners while providing advanced capabilities for experienced users, significantly enhancing the overall user experience and functionality of the NLM Python bindings.