# Enhanced NLM Python Bindings Examples

This directory contains examples demonstrating the new Python-specific features
and convenience methods in the NLM Python bindings.

## Available Examples

### 1. Basic Usage Example
**File:** `basic_usage.py`
**Description:** Shows basic usage of the enhanced Python bindings with configuration, brain creation, and simulation.

### 2. Advanced Features Example
**File:** `advanced_features.py`
**Description:** Demonstrates advanced features like performance monitoring, visualization, and serialization.

### 3. Development System Example
**File:** `development_example.py`
**Description:** Shows how to work with the developmental stages and development system.

### 4. Error Handling Example
**File:** `error_handling_example.py`
**Description:** Demonstrates the enhanced error handling with custom Python exceptions.

### 5. Memory Systems Example
**File:** `memory_systems_example.py`
**Description:** Shows how to work with the memory systems (working memory, episodic memory, etc.).

### 6. Prediction System Example
**File:** `prediction_example.py`
**Description:** Demonstrates working with the prediction system and prediction error signals.

### 7. Neuromodulation Example
**File:** `neuromodulation_example.py`
**Description:** Shows how to work with neuromodulation systems (dopamine, curiosity, novelty).

### 8. Integration Example
**File:** `integration_example.py`
**Description:** Shows complete integration with world simulation and agent brains.

### 9. Performance Monitoring Example
**File:** `performance_example.py`
**Description:** Demonstrates performance monitoring and profiling capabilities.

### 10. Visualization Example
**File:** `visualization_example.py`
**Description:** Shows how to export data for visualization and integrate with plotting libraries.

## Example Structure

Each example file follows this structure:

1. **Imports**: Standard Python libraries and pynlm module
2. **Function Definitions**: Each example is organized into focused functions
3. **Error Handling**: Demonstrates try/except blocks with enhanced error types
4. **Documentation**: Clear docstrings and comments explaining each step
5. **Output**: Clear console output showing results

## Usage Instructions

1. Ensure the NLM Python bindings are built and installed
2. Run examples from the project root directory:
   ```bash
   python examples/basic_usage.py
   ```
3. Each example can be run independently or as part of a comprehensive demonstration

## Example Categories

### Configuration Management
- `createDefaultConfig()` - Python factory function
- `updateFromDict()` - Update from Python dictionary
- `toDict()` - Convert to Python dictionary
- `validate()` - Configuration validation
- `getSchema()` - Get configuration schema

### Brain Convenience Methods
- `runEpisode()` - Run simulation episodes with callbacks
- `saveBrain()` / `loadBrain()` - Enhanced serialization
- `getEpisodeStats()` - Get brain statistics as Python dictionary
- `getDevelopmentStage()` - Get developmental stage information
- `enableDevelopment()` - Enable/disable development system

### Development System
- `getDevelopmentalStage()` - Get current developmental stage
- `setDevelopmentalStage()` - Set developmental stage
- `enableDevelopment()` - Enable/disable development

### Performance Monitoring
- `enablePerformanceMonitoring()` - Enable performance monitoring
- `getPerformanceStats()` - Get performance statistics

### Visualization
- `enableVisualization()` - Enable visualization support
- `exportForVisualization()` - Export brain state for visualization

### Error Handling
- `NLMRuntimeError` - Base runtime error
- `NLMValidationError` - Configuration validation errors
- `NLMImportError` - File import errors
- `NLMMemoryError` - Memory-related errors
- `NLMDevelopmentError` - Development system errors
- `NLMPerformanceError` - Performance-related errors
- `NLMVisualizationError` - Visualization errors
- `NLMExportError` - Export errors

### Integration Features
- Callback support for simulation hooks
- Integration with world simulation
- Agent brain interface

## Quick Start

Run the basic usage example to get started:

```bash
cd /workspace/app
python examples/basic_usage.py
```

This will demonstrate:
1. Creating and configuring a brain
2. Running a simulation episode
3. Monitoring performance
4. Exploring available features

## Advanced Features

For more advanced usage, refer to the individual example files or the
`examples/advanced_features.py` which demonstrates all major features together.

## Testing

Run the test suite to verify all examples work correctly:

```bash
python -m pytest examples/test_examples.py -v
```

This will run all examples and check for any errors or issues.

## Troubleshooting

### Common Issues

1. **Module not found**: Ensure the NLM Python bindings are built and installed:
   ```bash
   cd /workspace/app
   mkdir build && cd build
   cmake ..
   make -j4
   ```

2. **Import errors**: Check that pynlm module is available in Python path
3. **Callback errors**: Ensure callback functions are properly defined and handle exceptions

### Error Handling

The examples demonstrate comprehensive error handling with custom exception types.
Catch specific exception types for better error recovery:

```python
try:
    config.validate()
except pynlm.ValidationError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMRuntimeError as e:
    print(f"Runtime error: {e}")
```

## Future Enhancements

This directory will be expanded with more examples as new features are added:

- Real-world simulation scenarios
- Performance optimization examples
- Research use cases
- Machine learning integration examples
- Custom callback implementations
- Visualization and plotting integration

## License

This project is licensed under the MIT License. See LICENSE file for details.