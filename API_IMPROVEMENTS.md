# NLM Python Bindings - API Improvements

This document describes the improvements made to the NLM (Neural Learning Machine) Python bindings to enhance API consistency, completeness, and developer experience.

## Overview

The NLM Python bindings have been significantly improved with the following enhancements:

- **Convenience Methods**: Added type-specific getter methods for common configuration parameters
- **Error Handling**: Improved error handling to prevent Python interpreter crashes
- **Exception Translation**: Added proper exception translation from C++ to Python
- **Class Enhancements**: Added missing methods to SensoryInput, Vision, Audio, and Action classes
- **Type Conversions**: Enhanced NeuronId and SynapseId interfaces with conversion methods
- **Factory Functions**: Improved existing factory functions and added specialized ones
- **Utility Functions**: Added utility functions for common operations
- **Documentation**: Comprehensive documentation and examples

## Config Class Improvements

### New Convenience Methods

The Config class now includes the following convenience getter methods:

- `getFloat(key)`: Get a float value from configuration
- `getInt(key)`: Get an integer value from configuration
- `getBool(key)`: Get a boolean value from configuration
- `getString(key)`: Get a string value from configuration
- `getFloatArray(key)`: Get a float array value from configuration
- `getIntArray(key)`: Get an integer array value from configuration

### Type-Specific Getters

Added type-specific getter methods for common configuration parameters:

- `getLearningRate()`: Get learning rate parameter
- `getDecayRate()`: Get decay rate parameter
- `getPopulationSize()`: Get population size parameter
- `getConnectionDensity()`: Get connection density parameter
- `getMaxSynapticAge()`: Get max synaptic age parameter
- `getEnablePlasticity()`: Get plasticity enable flag
- `getEnableNeuromodulation()`: Get neuromodulation enable flag

### Error Handling

Improved error handling in `loadFromArgs()` to prevent Python interpreter crashes. The method now:

- Wraps C++ exceptions in Python exceptions
- Provides meaningful error messages
- Gracefully handles invalid command line arguments

## SensoryInput Class Improvements

### Virtual Destructor and Copy Support

Added:
- Virtual destructor for proper polymorphic cleanup
- Copy constructor and assignment operator
- Error handling methods (`hasError()`, `getErrorMessage()`)

### Enhanced Error Handling

The base `SensoryInput` class now includes:
- Error state tracking
- Error message storage
- Error reporting methods

## Vision Class Improvements

### New Vision Processing Methods

Added comprehensive vision processing methods:

- `getChannelData(channel)`: Get specific color channel data
- `applyFilter(kernel)`: Apply a processing filter to the image
- `normalize(target_mean, target_stddev)`: Normalize image data
- `scale(factor)`: Scale image dimensions by a factor
- `resize(new_width, new_height)`: Resize image to specified dimensions

### Improved Error Handling

All vision processing methods include proper error handling with:
- Exception wrapping
- Meaningful error messages
- Graceful failure handling

## Audio Class Improvements

### New Audio Processing Methods

Added audio processing capabilities:

- `applyGain(gainDb)`: Apply gain in dB to audio signal
- `getDuration()`: Get audio signal duration in seconds
- `getFrequency()`: Get frequency content of audio signal

### Enhanced Sample Rate Support

Improved `setSampleRate()` method with:
- Better validation
- Error handling
- Return value support

## Action Class Improvements

### String Representation Methods

Added string representation capabilities:

- `toString()`: Get string representation of action
- `fromString(str)`: Parse action from string representation

### Convenience Methods

Enhanced action creation with:
- Factory function `createAction(type, parameters)`
- Specialized factory functions for common actions:
  - `createMoveAction(dx, dy)`
  - `createLookAction(angleX, angleY)`

## Type Conversion Improvements

### NeuronId and SynapseId Enhancements

Improved the Python interface for identifier classes:

- Added conversion methods between different types
- Enhanced equality and comparison operators
- Added type conversion utilities:
  - `neuronIdToInt(neuron_id)`
  - `intToNeuronId(value)`
  - `synapseIdToInt(synapse_id)`
  - `intToSynapseId(value)`

### Fixed NeuronType Enum Ambiguity

Resolved naming inconsistencies in the NeuronType enum by:

- Ensuring consistent naming across all enum values
- Adding proper documentation for each enum value
- Improving enum value mapping to strings

## Factory Function Improvements

### Enhanced Existing Functions

Improved consistency with C++ API patterns:

- `createDefaultConfig()`: Now sets sensible default values
- `createBrain(config)`: Enhanced parameter validation
- `createSimpleWorld()`: Added basic configuration

### Specialized Factory Functions

Added factory functions for common use cases:

- `createSimpleRobotBrain()`: Create a basic robot brain
- `createLearningAgentWorld()`: Create a world suitable for learning agents
- `createVisionSensor(width, height, channels)`: Create a vision sensor
- `createAudioSensor(sampleRate, numSamples)`: Create an audio sensor

## Utility Functions

### Data Processing Utilities

Added utility functions for common operations:

- `normalizeData(data, target_mean, target_stddev)`: Normalize data to specified mean and standard deviation
- `scaleData(data, factor)`: Scale all data values by a factor
- `clipData(data, min, max)`: Clip data values to specified range

### Neural ID Utilities

Added utilities for working with neural identifiers:

- `getRandomNeuronId()`: Get a random neuron ID
- `getRandomSynapseId()`: Get a random synapse ID
- `getRandomRegionId()`: Get a random region ID

### Type Conversion Utilities

Comprehensive type conversion utilities:

- Neuron ID conversions
- Synapse ID conversions
- Region ID conversions

## Exception Handling and Error Translation

### Python Exception Integration

All C++ exceptions are now properly translated to Python exceptions:

- `std::runtime_error` → `RuntimeError`
- `std::invalid_argument` → `ValueError`
- `std::out_of_range` → `IndexError`
- `std::bad_variant_access` → `KeyError`

### Error Messages

Error messages are now:

- More descriptive and user-friendly
- Consistent across all methods
- Include context about what went wrong

## Testing and Examples

### Example Usage File

The `examples/api_usage.py` file provides comprehensive examples of:

- All new convenience methods
- Vision and audio processing
- Action string representations
- Utility functions
- Factory functions

### Testing Approach

The improvements are tested through:

- Example usage demonstrations
- Unit tests for individual methods
- Integration tests for complex workflows
- Error handling validation

## Migration Guide

### Breaking Changes

This update introduces no breaking changes. All existing code will continue to work as before.

### New Capabilities

Users can now:

- Use more intuitive getter methods
- Benefit from better error handling
- Access enhanced processing capabilities
- Leverage improved factory functions
- Utilize comprehensive utility functions

### Performance Considerations

The improvements maintain the same performance characteristics:

- All new methods are optional and not used unless needed
- Error handling only impacts code paths that encounter errors
- Memory usage remains the same for existing functionality

## Conclusion

The NLM Python bindings have been significantly enhanced with:

- **Improved API consistency**: All classes now follow similar patterns
- **Enhanced completeness**: Missing methods have been added
- **Better error handling**: Prevents crashes and provides helpful error messages
- **Comprehensive documentation**: Includes examples and usage guides
- **Developer-friendly utilities**: Makes common tasks easier

These improvements make the NLM Python bindings more robust, user-friendly, and suitable for production use.

## Build Instructions

To build and use the improved bindings:

```bash
# Install the package
pip install -e .

# Or build from source
python setup.py build_ext --inplace

# Run examples
python examples/api_usage.py
```

Note: The Python bindings need to be built before use. The build process requires:

- Python development headers
- pybind11
- scikit-build-core
- C++ compiler with C++20 support

## Support

For questions or issues:

- Check the examples for usage patterns
- Review the documentation for method details
- Report issues through the NLM GitHub repository
- Refer to the individual class documentation in the source code

## See Also

- `src/core/Config/Config.hpp`: Config class header
- `src/sensory/SensoryInput.hpp`: SensoryInput base class header
- `src/sensory/Vision.hpp`: Vision class header
- `src/sensory/Audio.hpp`: Audio class header
- `src/motor/Action.hpp`: Action class header
- `python/bindings.cpp`: Python bindings implementation
