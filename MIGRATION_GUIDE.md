# Enhanced Python Bindings for NLM - Migration Guide

## Overview

This migration guide documents the improvements made to the Python bindings in `python/bindings.cpp`. It explains the changes from the old API to the new enhanced API, provides migration examples, and highlights the benefits of the new implementation.

## Summary of Changes

### 1. Action Class Enhancements
- **Old API**: Multiple constructors requiring manual ActionType enum selection
- **New API**: Pythonic factory methods with fluent builder pattern

### 2. Vision and Audio Class Improvements
- **Old API**: Basic constructors with limited factory support
- **New API**: Enhanced factory methods with validation and better attribute access

### 3. Enhanced Error Handling
- **Old API**: Basic exception handling with limited validation
- **New API**: Comprehensive error mapping with descriptive error messages

### 4. Smart Pointer Integration
- **Old API**: Raw pointer returns and potential memory leaks
- **New API**: Smart pointer integration with proper memory management

### 5. Pythonic Factory Methods
- **Old API**: Limited factory functions
- **New API**: Enhanced factory methods with validation and options

### 6. Enhanced API Design
- **Old API**: Basic class definitions
- **New API**: Context managers, type conversion utilities, serialization support

## Migration Examples

### Action Class Migration

#### OLD API:
```python
# Create actions using constructor
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
action2 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])

# Set properties manually
action1.setType(pynlm.ActionType.TurnLeft)
action1.setParameters([0.5, 0.3])

# Get properties
print(action1.getType())
print(action1.getParameters())
```

#### NEW API:
```python
# Create actions using Pythonic factory methods
action1 = pynlm.Action.move_forward()
action2 = pynlm.Action.custom([1.0, 2.0, 3.0])

# Builder pattern with fluent interface
action3 = pynlm.Action.turn_left().with_parameters([0.5, 0.3])

# Alternative static factory methods
action4 = pynlm.Action("MoveForward")  # Type constructor
action5 = pynlm.Action("Custom", [1.0, 2.0, 3.0])

# Get properties (same API)
print(action1.get_type())  # New Pythonic naming
print(action1.get_parameters())
```

### Vision Class Migration

#### OLD API:
```python
# Create vision objects
vision1 = pynlm.Vision()
vision2 = pynlm.Vision(640, 480, 3)

# Set data
vision2.setData([1.0, 2.0, 3.0])

# Get properties
print(vision2.getWidth())
print(vision2.getHeight())
```

#### NEW API:
```python
# Create vision objects using Pythonic factory methods
vision1 = pynlm.Vision.from_image(640, 480)
vision2 = pynlm.Vision.rgb(800, 600)

# Additional factory methods
vision3 = pynlm.Vision.grayscale(320, 240)
vision4 = pynlm.Vision.rgba(1024, 768)

# Enhanced property access
print(vision2.get_width())   # New Pythonic naming
print(vision2.get_height())
print(vision2.get_channels())

# Validation-aware factory
vision5 = pynlm.Vision.from_data(data, 640, 480, 3)

# Type checking helpers
if vision2.is_rgb():
    print("RGB image")
if vision2.is_grayscale():
    print("Grayscale image")
```

### Audio Class Migration

#### OLD API:
```python
# Create audio objects
audio1 = pynlm.Audio()
audio2 = pynlm.Audio(44100, 44100)

# Set data and properties
audio2.setData([1.0, 2.0, 3.0])
audio2.setSampleRate(48000)

# Get properties
print(audio2.getSampleRate())
print(audio2.getNumSamples())
```

#### NEW API:
```python
# Create audio objects using Pythonic factory methods
audio1 = pynlm.Audio.from_samples(44100, 44100)

# Enhanced factory methods
audio2 = pynlm.Audio.create_default_audio(2.5)  # 2.5 seconds at default rate
audio3 = pynlm.Audio.from_samples_with_data(44100, 44100, audio_data)

# Enhanced property access
print(audio2.get_sample_rate())   # New Pythonic naming
print(audio2.get_num_samples())
print(audio2.get_duration())      # New: get duration in seconds

# Duration calculation
duration = audio2.get_sample_rate() / audio2.get_num_samples()
```

### Factory Methods Migration

#### OLD API:
```python
# Limited factory functions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
```

#### NEW API:
```python
# Enhanced factory methods with validation
config = pynlm.createDefaultConfig()

# Brain creation options
brain = pynlm.createBrainFromConfig(config)
brain = pynlm.createBrainFromFile("config.json")
brain = pynlm.createBrainFromArgs(2, ["prog", "--neurons=100"])

# Agent brain creation (Pythonic factories)
agent = pynlm.createAgentBrainFromConfig(config)
agent = pynlm.createAgentBrainFromBrain(brain)

# World creation options
world = pynlm.createSimpleWorldWithSize(20, 20, 8, 8)
world = pynlm.createSimpleWorldWithVision(10, 10, 5, 5)

# Error handling with validation
try:
    brain = pynlm.createBrainFromFile("invalid_config.json")
except Exception as e:
    print(f"Failed to create brain: {e}")
```

### Enhanced API Migration

#### OLD API:
```python
# Basic usage
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
percept = world.getSensoryPercept()
action = agent.decodeMotorCommand()
```

#### NEW API:
```python
# With context managers (recommended)
with pynlm.BrainContext(config) as brain:
    brain.initialize()
    brain.step(0)
    print(f"Brain stats: {brain.getTotalNeuronCount()} neurons")

# With world context manager
with pynlm.WorldContext() as world:
    world.configure(10, 10, 8, 8)
    world.reset()
    percept = world.getSensoryPercept()

# Type conversion utilities
action_str = pynlm.to_string(action)
vision_data = pynlm.to_vector(vision)
audio_data = pynlm.to_vector(audio)

# Action comparison
are_equal = pynlm.compare_actions(action1, action2)

# Comprehensive brain stats
stats = pynlm.calculate_brain_stats(brain)
print(f"Neurons: {stats['total_neurons']}")
print(f"Active: {stats['active_neurons']}")
```

## Error Handling Improvements

### Exception Types

The enhanced bindings now support comprehensive exception types:

- `RuntimeError`: For runtime errors
- `InvalidArgumentError`: For invalid function arguments
- `OutOfRangeError`: For index out of bounds
- `LogicError`: For logical errors
- `DomainError`: For domain-specific errors
- `LengthError`: For size mismatches
- `RangeError`: For value range errors
- `OverflowError`: For overflow conditions
- `UnderflowError`: For underflow conditions
- `NLMError`: Generic NLM errors

### Error Handling Examples

#### OLD API:
```python
# Basic error handling
try:
    brain = pynlm.createBrainFromFile("config.json")
except Exception as e:
    print(f"Error: {e}")
```

#### NEW API:
```python
# Enhanced error handling with specific exception types
try:
    brain = pynlm.createBrainFromFile("config.json")
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except pynlm.FileNotFoundError as e:
    print(f"File not found: {e}")
except pynlm.InvalidConfigError as e:
    print(f"Invalid configuration: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

### Validation Examples

#### OLD API:
```python
# Manual validation
vision = pynlm.Vision(640, 480, 3)
data = [1.0, 2.0, 3.0]  # Too small for 640x480 RGB
if len(data) != vision.getWidth() * vision.getHeight() * vision.getChannels():
    print("Data size mismatch")
    # Fallback to smaller image
    vision = pynlm.Vision(3, 1, 1)
vision.setData(data)  # Might cause issues
```

#### NEW API:
```python
# Automatic validation with descriptive errors
try:
    vision = pynlm.Vision.from_data(data, 640, 480, 3)
except Exception as e:
    print(f"Validation error: {e}")
    # Try with correct data size
    correct_data = [0.0] * (640 * 480 * 3)
    vision = pynlm.Vision.from_data(correct_data, 640, 480, 3)
```

## Smart Pointer Integration

### Memory Management Improvements

#### OLD API:
```python
# Raw pointer returns - potential memory leaks
region = brain.getRegion(id)  # Raw pointer
regions = brain.getRegions()   # Raw pointer
brain_ptr = agent.getBrain()   # Raw pointer

# Manual memory management needed
if region:
    delete region  # Must remember to delete
if regions:
    delete[] regions
```

#### NEW API:
```python
# Smart pointer returns - automatic memory management
region = brain.get_region(id)  # std::shared_ptr
regions = brain.get_regions()   # std::shared_ptr
brain_ptr = agent.get_brain()   # std::shared_ptr

# No manual memory management needed
# Resources automatically cleaned up when no longer referenced
```

## Backward Compatibility

### Compatibility Notes

1. **Old API still works**: Most old code will continue to work with the new bindings.

2. **Deprecated methods**: Some old methods are deprecated but still functional.

3. **New naming conventions**: Some method names have changed for Pythonic style (snake_case instead of camelCase).

4. **Enhanced functionality**: New features are available but don't break existing code.

### Gradual Migration

For users who want to migrate gradually:

1. **Phase 1**: Use new factory methods for new code
2. **Phase 2**: Update existing code to use new factory methods
3. **Phase 3**: Adopt context managers and enhanced error handling
4. **Phase 4**: Use smart pointer integration throughout

## Performance Considerations

### Memory Management

- **Before**: Manual memory management with potential leaks
- **After**: Smart pointers with automatic garbage collection

### Validation Overhead

- **Before**: Minimal validation, potential runtime errors
- **After**: Comprehensive validation with early error detection

### API Design

- **Before**: Boilerplate factory functions
- **After**: Streamlined Pythonic API with less boilerplate

## Best Practices

### Error Handling

1. **Always use try-catch blocks**: The new bindings provide better error messages.
2. **Check for null pointers**: New APIs include null checks.
3. **Validate inputs**: New factory methods include automatic validation.
4. **Use context managers**: For resource cleanup and exception safety.

### Memory Management

1. **Prefer smart pointers**: New APIs use shared_ptr throughout.
2. **Don't manually delete**: Let smart pointers handle cleanup.
3. **Check reference counts**: If needed, but not required for most cases.

### API Usage

1. **Use factory methods**: For cleaner, more readable code.
2. **Use context managers**: For resource-intensive operations.
3. **Leverage validation**: For early error detection.
4. **Use Pythonic naming**: For better code readability.

## Troubleshooting

### Common Issues

1. **"ModuleNotFoundError: No module named pynlm"**
   - Ensure the bindings are compiled and installed correctly.
   - Run `python setup.py build_ext --inplace` if needed.

2. **"AttributeError: module 'pynlm' has no attribute 'Action'"**
   - The bindings module might not have been built correctly.
   - Check if there are any compilation errors in the bindings.

3. **"RuntimeError: Data size mismatch"**
   - This is a new validation error from the enhanced bindings.
   - Use the new factory methods with proper data size validation.

4. **Memory leaks**
   - Should not occur with smart pointer integration.
   - If occurring, check for circular references.

### Debugging Tips

1. **Enable verbose logging**: Use the enhanced logging methods in the bindings.
2. **Check exception types**: Catch specific exceptions for better error handling.
3. **Use context managers**: For automatic resource cleanup.
4. **Profile memory usage**: Use tools like valgrind or memory profilers.

## Next Steps

### For Users

1. **Update documentation**: Update your project documentation with new API.
2. **Test thoroughly**: Run all existing tests with the new bindings.
3. **Gradual migration**: Start with new code, migrate old code gradually.
4. **Training**: Provide training for users on new Pythonic patterns.

### For Developers

1. **Update examples**: Update example code in documentation.
2. **Add tests**: Add comprehensive tests for new functionality.
3. **Performance testing**: Test performance impact of changes.
4. **Documentation**: Update API documentation with new features.

## Conclusion

The enhanced Python bindings provide significant improvements over the original implementation:

- **Better API design**: Pythonic factory methods and fluent interface
- **Enhanced error handling**: Comprehensive validation and descriptive errors
- **Smart memory management**: Automatic cleanup with smart pointers
- **Context managers**: Automatic resource cleanup
- **Type conversion utilities**: Convenient data conversion methods
- **Serialization support**: Better data persistence options
- **Backward compatibility**: Existing code continues to work

Users can migrate at their own pace, starting with new code and gradually updating existing code to take advantage of the new features and improved API design.
