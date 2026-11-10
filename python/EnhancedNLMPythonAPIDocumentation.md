# Enhanced NLM Python API Documentation

## Overview

The Enhanced NLM Python API provides improved interfaces to the Neural Learning Machine (NLM) framework with better error handling, validation, and Pythonic patterns.

## Key Improvements

### 1. Enhanced Error Handling
- Custom exception classes (NLMError, ValidationError, ConfigError, MemoryError, InitializationError)
- Type-specific error messages
- Validation error details

### 2. Configuration Validation
- Schema-based configuration validation
- Type checking and range validation
- Required key validation
- Invalid value detection

### 3. Pythonic API Design
- Context managers for resource management
- Type-safe value retrieval
- Validation on set operations
- Enhanced documentation

### 4. Performance Optimizations
- Memory pool integration
- Efficient validation rules
- Lazy validation where appropriate

## API Usage Examples

### Basic Usage

```python
import nlm

# Create a brain with validation
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### Configuration with Validation

```python
import nlm

# Create validated configuration
config = nlm.PythonConfig()

# Configure with validation
config.set("brain.neuron_count", 1000)  # Valid
config.set("brain.synapse_density", 0.1)   # Valid
config.set("enable_debug", True)         # Valid

# Attempt invalid value (will raise ValidationError)
try:
    config.set("brain.neuron_count", 100000)  # May be out of range
except nlm.ValidationError as e:
    print(f"Validation error: {e}")

# Get values safely
neuron_count = config.getInt("brain.neuron_count")
synapse_density = config.getFloat("brain.synapse_density")
debug_enabled = config.getBool("enable_debug")
```

### Enhanced Configuration Management

```python
import nlm

# Create configuration with schema
config = nlm.PythonConfig()

# Add validation rules
config.addIntValidation("brain.neuron_count", (100, 10000), required=True)
config.addFloatValidation("brain.synapse_density", (0.0, 1.0))
config.addStringValidation("test.mode", allowed_values=["train", "test", "eval"])
config.addBoolValidation("enable_checkpointing")

# Load from file with validation
config.loadFromFile("config.json")

# Configuration is automatically validated
if config.validate():
    print("Configuration is valid")
    
# Get validated configuration summary
summary = config.getSummary()
print(summary)
```

### Context Manager for Configuration

```python
import nlm

# Use configuration as a context manager
config = nlm.PythonConfig()
config.addIntValidation("param", (0, 10))

with config.createScope() as scope:
    # Safe configuration within context
    config.set("param", 5)
    
    # Access configuration
    print(f"Parameter value: {config.getInt('param')}")
    
    # Changes are committed when exiting context

# Configuration persists after context
print(f"Parameter value after context: {config.getInt('param')}")
```

### Error Handling Best Practices

```python
import nlm

config = nlm.PythonConfig()

# Configuration setup
config.addIntValidation("sleep_time", (0, 1000))
config.set("sleep_time", 5)

# Successful operations - no error
print("Configuration set successfully")

# Failed operations - catch specific exceptions
try:
    config.addIntValidation("sleep_time", (0, 1000))
    config.set("sleep_time", 500)  # Too large
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
    print("Configuration rejected")
or:
    # Could also be ConfigError for other issues
    pass

# Always validate after loading critical configuration
if config.validate():
    print("Configuration is valid and ready for use")
else:
    print("Configuration has validation errors")
```

## Advanced API Features

### 1. Memory Pool Integration

```python
import nlm

# Create world with memory optimization
world = nlm.createSimpleWorld()

# Configure with memory constraints
world.configure(
    width=20.0,
    height=20.0,
    visionWidth=8,
    visionHeight=8
)

world.setAgentStart(10.0, 10.0)

# Memory pools are automatically used for efficient allocation
print("World created with memory-optimized allocation")
```

### 2. Complex Brain Operations

```python
import nlm

# Create a complex brain with all subsystems
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complex simulation with memory management
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Memory management is handled automatically
    if step % 100 == 0:
        print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
```

## Migration Guide

### From Old API to Enhanced API

#### Configuration

**Old:**
```python
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)  # No validation
```

**New:**
```python
config = nlm.PythonConfig()
config.addIntValidation("brain.neuron_count", (100, 10000), required=True)
config.set("brain.neuron_count", 1000)  # With validation
```

#### Error Handling

**Old:**
```python
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
```

**New:**
```python
config = nlm.PythonConfig()
config.addIntValidation("brain.neuron_count", (100, 10000))
config.set("brain.neuron_count", 1000)  # Validation included
```

#### Configuration Loading

**Old:**
```python
config = nlm.createDefaultConfig()
config.loadFromFile("config.json")
```

**New:**
```python
config = nlm.PythonConfig()
config.loadFromFile("config.json")  # Automatic validation
```

## Best Practices

### 1. Always Validate Critical Parameters

```python
config = nlm.PythonConfig()

# Validate critical parameters
config.addIntValidation("brain.neuron_count", (100, 10000), required=True)
config.addFloatValidation("brain.synapse_density", (0.0, 1.0))

# Load configuration
config.loadFromFile("production_config.json")

if not config.validate():
    raise nlm.ValidationError("Configuration validation failed")
```

### 2. Use Context Managers for Temporary Configurations

```python
config = nlm.PythonConfig()
config.addIntValidation("temp_param", (0, 100))

with config.createScope():
    config.set("temp_param", 50)
    # Safe to modify temporarily
    print(f"Temporary parameter: {config.getInt('temp_param')}")

# Parameter reset after context
```

### 3. Handle Validation Errors Gracefully

```python
import nlm

def create_validated_config(filepath):
    config = nlm.PythonConfig()
    
    try:
        config.loadFromFile(filepath)
    except nlm.ConfigError as e:
        print(f"File loading error: {e}")
        return None
    
    if not config.validate():
        raise nlm.ValidationError("Configuration validation failed")
    
    return config

# Usage
config = create_validated_config("config.json")
if config:
    print("Configuration loaded and validated successfully")
```

### 4. Monitor Memory Usage

```python
import nlm

# Create components that use memory pools
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Check memory usage (if available in your Python bindings)
memory_usage = brain.getMemoryUsage()  # If implemented
print(f"Current memory usage: {memory_usage} bytes")
```

## Performance Considerations

### Validation Overhead
- Validation rules are compiled into efficient C++ functions
- Type checking is performed at compile time where possible
- Lazy validation - only validates when values are set
- Validation can be disabled for performance-critical code

### Memory Management
- Memory pools reduce allocation overhead
- Objects are allocated from pre-allocated pools
- Automatic memory cleanup
- Thread-safe memory management

### Configuration Validation
- Schema validation is performed during configuration load
- Rules are cached for efficient lookup
- Validation can be disabled for maximum performance

## Troubleshooting

### Common Issues and Solutions

1. **Configuration validation fails**
   - Solution: Check validation errors and fix configuration values
   - Check if validation schema is properly configured

2. **Memory errors**
   - Solution: Ensure memory pools are properly initialized
   - Check for memory leaks in custom code

3. **Python binding errors**
   - Solution: Verify that the enhanced bindings module is being used
   - Check for import errors in Python environment

### Validation Error Handling

```python
import nlm

config = nlm.PythonConfig()

# Add comprehensive validation
config.addIntValidation("param", (0, 100), required=True)
config.addFloatValidation("rate", (-1.0, 1.0))
config.addStringValidation("mode", allowed_values=["a", "b", "c"])

# Test various validation scenarios
test_cases = [
    ("param", 50, True, "valid"),
    ("param", 150, False, "out of range"),
    ("rate", 0.5, True, "valid"),
    ("mode", "a", True, "valid"),
]

for param, value, should_pass, description in test_cases:
    try:
        config.set(param, value if isinstance(param, int) else value)
        if should_pass:
            print(f"✓ {description}: {param}={value} accepted")
        else:
            print(f"✗ {description}: {param}={value} rejected")
    except nlm.ValidationError as e:
        print(f"✗ {description}: {param}={value} - {e}")
```

## Conclusion

The Enhanced NLM Python API provides a robust, validated interface to the NLM framework with comprehensive error handling, type safety, and validation. The improvements make the API more reliable, user-friendly, and suitable for production use while maintaining backward compatibility where possible.

Key benefits:
- **Robust validation**: Configuration values are validated against schema rules
- **Clear error messages**: Specific, actionable error messages
- **Type safety**: Type checking prevents type mismatches
- **Performance**: Efficient validation with minimal overhead
- **Maintainability**: Clear, well-documented API patterns
- **Backward compatibility**: Enhanced API maintains compatibility where possible

This enhanced API provides a solid foundation for reliable, production-ready NLM applications with improved developer experience and reduced runtime errors.