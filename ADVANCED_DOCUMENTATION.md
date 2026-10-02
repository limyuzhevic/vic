# NLM Advanced Documentation
# Advanced Features and Usage Guide

This document provides comprehensive documentation for the advanced features added to the NLM project, including batch processing capabilities, enhanced Python APIs, and performance optimization tools.

## Table of Contents

1. Overview of Advanced Features
2. Batch Processing Capabilities
3. Enhanced Python APIs
4. Performance Optimization
5. Configuration Management
6. Error Handling and Validation
7. Advanced Tutorials
8. Migration Guide
9. Troubleshooting

---

## 1. Overview of Advanced Features

This documentation covers the advanced features added to improve the NLM (Neural Learning Machine) project:

### Key Improvements

- **Enhanced Python Bindings**: Improved error handling, validation, and additional APIs
- **Batch Processing**: Parallel execution of multiple simulations with result aggregation
- **Learning Analysis**: Advanced tools for analyzing learning dynamics and brain behavior
- **Performance Optimization**: Tools for benchmarking and optimizing simulation performance
- **Configuration Management**: Better configuration validation and parameter checking
- **Comprehensive Documentation**: Detailed examples and tutorials for all skill levels

### Files Modified/Created

- `python/bindings.cpp`: Enhanced Python bindings with validation and error handling
- `python/__init__.py`: Proper Python package structure
- `python/advanced.py`: Advanced API extensions
- `examples/`: Comprehensive example scripts
- `results/`: Example output files
- `configs/default.cfg`: Fixed configuration bug

---

## 2. Batch Processing Capabilities

### 2.1 Batch Processing Overview

The NLM batch processing system enables efficient execution of multiple simulations:

```python
from nlm_batch import BatchProcessor, BatchSimulationConfig

# Create configurations for batch processing
configs = [
    BatchSimulationConfig(
        name="small_brain",
        config=small_config,
        steps=1000,
        neurons=500
    ),
    BatchSimulationConfig(
        name="medium_brain", 
        config=medium_config,
        steps=1000,
        neurons=1500
    ),
    BatchSimulationConfig(
        name="large_brain",
        config=large_config, 
        steps=1000,
        neurons=3000
    )
]

# Create batch processor
processor = BatchProcessor(max_workers=4, enable_parallel=True)

# Run batch processing
results = processor.run_batch(configs)

# Analyze results
analysis = processor.analyze_batch_results()
```

### 2.2 Batch Configuration Options

Each batch configuration supports:

- **name**: Unique identifier for the configuration
- **config**: NLM configuration object
- **steps**: Number of simulation steps
- **neurons**: Target neuron count
- **world_size**: World dimensions (width, height)
- **enable_features**: Dictionary of enabled subsystems
- **priority**: Execution priority (normal, high, critical)

### 2.3 Performance Features

- **Parallel Execution**: Run multiple simulations concurrently
- **Result Caching**: Save intermediate results to disk
- **Progress Tracking**: Monitor simulation progress
- **Resource Management**: Efficient memory and CPU usage
- **Scalability**: Handle large numbers of simulations

---

## 3. Enhanced Python APIs

### 3.1 Advanced API Overview

The enhanced Python API provides:

- **Type Hints**: Complete type annotations for better IDE support
- **Advanced Functions**: Batch processing, learning analysis, export/import
- **Error Handling**: Clear, actionable error messages
- **Configuration Validation**: Parameter range checking and validation

### 3.2 Core Advanced Functions

#### NLMAdvancedAPI Class

```python
from nlm_advanced import NLMAdvancedAPI, SimulationResult, LearningMetrics

# Run batch simulations
results = NLMAdvancedAPI.run_batch_simulation(
    configs=[config1, config2, config3],
    steps_per_config=1000,
    parallel=True,
    callback=lambda i, step, total, max: print(f"Progress: {i+1}/{total}")
)

# Analyze learning dynamics
metrics = NLMAdvancedAPI.advanced_learning_analysis(brain, "comprehensive")

# Export/import brain state
NLMAdvancedAPI.export_brain_state(brain, "brain_state.json", "json")
imported_state = NLMAdvancedAPI.import_brain_state("brain_state.json", "json")

# Create brain from pattern
pattern = [0.1, 0.5, 0.9, 0.2, 0.7]
brain = NLMAdvancedAPI.create_brain_from_pattern(pattern)
```

#### SimulationResult Class

Dataclass containing comprehensive simulation metrics:

```python
@dataclass
class SimulationResult:
    total_steps: int
    total_spikes: int
    final_firing_rate: float
    memory_episodes: int
    total_reward: float
    duration_seconds: float
    final_neural_state: Optional[StateVector] = None
```

#### LearningMetrics Class

Dataclass containing advanced learning analysis:

```python
@dataclass
class LearningMetrics:
    weight_change_magnitude: float
    learning_rate_estimate: float
    stability_score: float
    adaptation_rate: float
    memory_retention: float
```

### 3.3 Error Handling

The enhanced API provides:

- **Input Validation**: Check for None values and invalid parameters
- **Meaningful Error Messages**: Clear descriptions of what went wrong
- **Exception Types**: Specific exception types for different error conditions
- **Graceful Degradation**: Fallback behavior when errors occur

---

## 4. Performance Optimization

### 4.1 Performance Benchmarking

```python
from nlm_batch import run_performance_benchmark

# Create configurations for benchmarking
configs = create_test_configurations()

# Run performance benchmark
analysis = run_performance_benchmark(configs)

# Analyze results
print(f"Average efficiency: {analysis['performance_summary']['average_efficiency_score']:.3f}")
print(f"Best configuration: {analysis['recommendations'][0]}")
```

### 4.2 Optimization Recommendations

The system automatically generates recommendations based on:

- **Efficiency Scores**: Suggest configurations with better performance
- **Firing Rate Analysis**: Recommend adjustments for abnormal firing rates
- **Neuron Count Optimization**: Suggest optimal neuron counts for different tasks
- **Connection Density**: Recommend connection density for specific use cases

### 4.3 Performance Monitoring

Real-time monitoring of:

- **Steps per second**: Simulation speed
- **Spikes per second**: Neural activity rate
- **Memory usage**: RAM consumption
- **CPU utilization**: Processor usage
- **I/O operations**: Disk read/write performance

---

## 5. Configuration Management

### 5.1 Configuration Validation

The enhanced configuration system includes:

- **Parameter Validation**: Check for valid ranges and types
- **Dependency Checking**: Ensure configuration consistency
- **Default Values**: Provide sensible defaults for missing parameters
- **Configuration Templates**: Predefined templates for common use cases

### 5.2 Configuration Classes

#### BatchSimulationConfig

```python
@dataclass
class BatchSimulationConfig:
    name: str
    config: Any
    steps: int = 1000
    neurons: int = 1000
    world_size: tuple = (20, 20)
    enable_features: Dict[str, bool] = field(default_factory=dict)
    priority: str = "normal"
```

#### Config Variations

Create diverse configurations for comparative analysis:

```python
from nlm_batch import BatchProcessor

processor = BatchProcessor()

# Create configuration variations
variations = {
    "neuron_count": [500, 1000, 2000],
    "connection_density": [0.05, 0.1, 0.15],
    "learning_rate": [0.001, 0.005, 0.01]
}

configs = processor.create_config_variations(
    base_config, 
    variations, 
    prefix="comparative_test"
)
```

---

## 6. Error Handling and Validation

### 6.1 Error Types

- **ConfigurationError**: Invalid configuration parameters
- **SimulationError**: Simulation execution failures
- **ImportError**: File import errors
- **ValidationError**: Parameter validation failures

### 6.2 Validation Examples

```python
from nlm_advanced import validate_config, validate_parameters

# Validate configuration
if validate_config(config):
    print("Configuration is valid")
else:
    print("Configuration has errors")

# Validate parameters
if validate_parameters({
    "neuron_count": 1000,
    "connection_density": 0.1,
    "learning_rate": 0.001
}):
    print("Parameters are valid")
else:
    print("Parameters have errors")
```

### 6.3 Error Recovery

- **Automatic Retries**: Retry failed operations with exponential backoff
- **Fallback Values**: Use default values when parameters are invalid
- **Graceful Degradation**: Continue with reduced functionality
- **Error Logging**: Detailed logging for debugging

---

## 7. Advanced Tutorials

### 7.1 Complete Tutorial

The `examples/complete_tutorial.py` provides:

- Beginner-level examples
- Intermediate agent simulations
- Advanced batch processing
- Error handling demonstrations
- Comprehensive analysis

### 7.2 Advanced Tutorial

The `examples/advanced_tutorial.py` covers:

- Advanced configuration creation
- Diverse configuration generation
- Advanced agent simulation
- Learning dynamics analysis
- Configuration optimization

### 7.3 Batch Processing Tutorial

The `examples/batch_processing.py` covers:

- Batch configuration creation
- Parallel execution setup
- Result analysis and visualization
- Performance benchmarking
- Result aggregation

---

## 8. Migration Guide

### 8.1 From Basic to Advanced

Migration is straightforward - existing code continues to work:

```python
# Old code (still works)
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# New advanced API (optional)
from nlm_advanced import NLMAdvancedAPI

brain = NLMAdvancedAPI.createBrain(config)  # Enhanced version
```

### 8.2 New Features Usage

To use new features, import the appropriate modules:

```python
# For advanced features
from nlm_advanced import NLMAdvancedAPI, run_advanced_simulation

# For batch processing
from nlm_batch import BatchProcessor, BatchSimulationConfig

# For examples
from examples.complete_tutorial import run_complete_tutorial
```

---

## 9. Troubleshooting

### 9.1 Common Issues

#### Issue: "ImportError: No module named 'nlm_advanced'"

**Solution**: Install the advanced modules:

```bash
pip install -e .
```

Or install dependencies:

```bash
pip install pybind11 scikit-build-core pytest numpy
```

#### Issue: "Segmentation fault" or "CRASH"

**Solution**:
1. Check that `initialize()` is called before `step()`
2. Verify configuration parameters are valid
3. Ensure all required dependencies are installed
4. Check for memory leaks in custom C++ extensions

#### Issue: "Brain not initialized"

**Solution**:
1. Always call `brain.initialize()` before simulation
2. Check configuration for required parameters
3. Verify all subsystems are properly configured

### 9.2 Debugging Tips

- **Enable Logging**: Set log level to debug for detailed output
- **Check Configuration**: Verify all required configuration parameters are set
- **Validate Inputs**: Use the validation functions to check parameters
- **Review Documentation**: Consult the appropriate documentation for your use case
- **Use Examples**: Start with the example scripts to understand the API

---

## Appendix

### A.1 Configuration File Structure

Enhanced configuration files support:

- **Validation**: Automatic parameter validation
- **Type Checking**: Parameter type enforcement
- **Dependency Checking**: Ensure configuration consistency
- **Documentation**: Inline parameter descriptions

### A.2 Performance Metrics

Key performance metrics tracked:

- **Simulation Speed**: Steps per second
- **Neural Activity**: Spikes per second
- **Memory Efficiency**: Memory usage per neuron
- **CPU Utilization**: Processor usage percentage
- **I/O Performance**: Disk read/write speeds

### A.3 API Reference

For detailed API documentation, refer to:

- `python/bindings.cpp`: Core bindings
- `python/__init__.py`: Package exports
- `python/advanced.py`: Advanced API
- `examples/`: Complete examples

---

## Acknowledgments

These improvements were developed to enhance the NLM project with:

- Better user experience through improved APIs
- Enhanced performance through batch processing
- Better error handling and validation
- Comprehensive documentation and examples
- Advanced features for experienced users

---

*Last updated: 2026-10-02*
*For support, visit: https://github.com/nlm-project/nlm*