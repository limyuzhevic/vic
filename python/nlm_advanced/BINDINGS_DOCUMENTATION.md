# Enhanced Python Bindings Documentation

## Overview

The Enhanced Python Bindings provide a comprehensive interface for NLM neural simulations with advanced features including type safety, error handling, and comprehensive validation. They build upon the original pynlm bindings while adding new capabilities for expert users.

## Key Features

### Type Safety

The enhanced bindings provide robust type safety with automatic conversion between Python and NLM types:

1. **NLMType System**: Comprehensive type system with automatic conversion
2. **Type Validation**: Automatic type checking and validation
3. **Error Handling**: Detailed error messages with type information
4. **Backward Compatibility**: Maintains compatibility with existing pynlm

### Enhanced Error Handling

The enhanced bindings provide comprehensive error handling:

1. **Custom Exception Types**: Detailed exception hierarchy for different error types
2. **Error Information**: Comprehensive error information including context and suggestions
3. **Error Recovery**: Strategies for recovering from errors
4. **Error Reporting**: Detailed error reporting with stack traces

### Pythonic Interfaces

The enhanced bindings provide intuitive Python interfaces:

1. **Context Managers**: Safe resource management with context managers
2. **Iterator Support**: Easy iteration over configuration and simulation data
3. **Property Access**: Intuitive property access with automatic conversion
4. **Method Chaining**: Method chaining for complex operations

### Performance Optimizations

The enhanced bindings include performance optimizations:

1. **Memory Management**: Efficient memory management and garbage collection
2. **Parallel Processing**: Support for parallel processing of simulations
3. **Lazy Evaluation**: Lazy evaluation for complex operations
4. **Caching**: Intelligent caching of frequently accessed data

## Bindings API

### Core Classes

#### EnhancedConfig

Enhanced configuration management with type safety.

```python
from nlm_enhanced import EnhancedConfig

# Create enhanced configuration
config = EnhancedConfig()

# Set values with automatic type conversion
config.set("learning_rate", 0.01)
config.set("batch_size", 32)
config.set("use_gpu", True)
config.set("model_name", "neural_network")

# Access values with automatic type conversion
learning_rate = config["learning_rate"]  # Returns float
batch_size = config["batch_size"]  # Returns int
use_gpu = config["use_gpu"]  # Returns bool
model_name = config["model_name"]  # Returns string

print(f"Learning rate: {learning_rate} (type: {type(learning_rate).__name__})")
print(f"Batch size: {batch_size} (type: {type(batch_size).__name__})")
print(f"Use GPU: {use_gpu} (type: {type(use_gpu).__name__})")
print(f"Model name: {model_name} (type: {type(model_name).__name__})")
```

#### EnhancedBrain

Enhanced brain management with advanced features.

```python
from nlm_enhanced import EnhancedBrain, EnhancedAgentBrain

# Create enhanced configuration
config = EnhancedConfig()
config.set("network_size", 1000)
config.set("learning_rate", 0.01)

# Create enhanced brain
brain = EnhancedBrain(config)

# Initialize brain
brain.initialize()

# Run simulation
for step in range(100):
    time = step * 0.01
    brain.step(step, time)

# Get brain statistics
stats = brain.get_statistics()
print(f"Average firing rate: {stats['average_firing_rate']}")
print(f"Total spikes: {stats['total_spikes']}")
```

#### EnhancedAgentBrain

Enhanced agent brain with advanced capabilities.

```python
from nlm_enhanced import EnhancedAgentBrain

# Create enhanced configuration
config = EnhancedConfig()
config.set("network_size", 1000)
config.set("learning_rate", 0.01)

# Create enhanced brain and agent brain
brain = EnhancedBrain(config)
agent_brain = EnhancedAgentBrain(brain)

# Initialize agent brain
agent_brain.initialize()

# Enable agent features
agent_brain.enable_feature("curiosity")
agent_brain.disable_feature("structural_plasticity")

# Apply neuromodulation
agent_brain.apply_reward_modulation(reward=1.0, predicted_reward=0.8)

# Update development
agent_brain.update_development(timestep=0.1)

# Get feature status
feature_status = agent_brain.get_feature_status()
print(f"Feature status: {feature_status}")
```

### NLMType System

```python
from nlm_enhanced import NLMType

# Create NLMType values
null_value = NLMType()
int_value = NLMType(42)
float_value = NLMType(3.14)
string_value = NLMType("hello")
bool_value = NLMType(True)

# Access type information
print(f"Null value: {null_value} (is null: {null_value.is_null()})")
print(f"Int value: {int_value} (type: {int_value.type()})")
print(f"Float value: {float_value} (type: {float_value.type()})")
print(f"String value: {string_value} (type: {string_value.type()})")
print(f"Bool value: {bool_value} (type: {bool_value.type()})")

# Type conversion
try:
    converted_int = int_value.as_int()
    print(f"Converted int: {converted_int}")
except Exception as e:
    print(f"Error converting int: {e}")

# Type checking
if int_value.is_int():
    print(f"int_value is int: {int_value.as_int()}")

if float_value.is_float():
    print(f"float_value is float: {float_value.as_float()}")

if string_value.is_string():
    print(f"string_value is string: {string_value.as_string()}")
```

### Exception Types

```python
from nlm_enhanced import (
    NLMException,
    ConfigurationError,
    BrainError,
    AgentError,
    WorldError
)

try:
    # Create configuration
    config = createDefaultConfig()
    
    # Set invalid value
    config.set("learning_rate", "invalid")  # String instead of float
    
    # Access invalid key
    value = config["nonexistent_key"]
    
except ConfigurationError as e:
    print(f"ConfigurationError: {e}")
    print(f"Error type: {e.type()}")
    print(f"Error details: {e.get_error_details()}")

except BrainError as e:
    print(f"BrainError: {e}")
    print(f"Error type: {e.type()}")

except AgentError as e:
    print(f"AgentError: {e}")
    print(f"Error type: {e.type()}")

except WorldError as e:
    print(f"WorldError: {e}")
    print(f"Error type: {e.type()}")
```

### Utility Functions

```python
from nlm_enhanced import (
    py_to_nlm,
    nlm_to_py,
    validate_config,
    validate_brain,
    get_error_type,
    get_error_details,
    log_error,
    trim,
    starts_with,
    to_lower,
    mean,
    stddev,
    median,
    get_current_directory
)

# Type conversion utilities
py_list = [1, 2, 3, 4, 5]
nlm_list = py_to_nlm(py_list)
back_to_py = nlm_to_py(nlm_list)

# Validation utilities
config = createDefaultConfig()
config.set("learning_rate", 0.01)
config.set("batch_size", 32)

if validate_config(config):
    print("Configuration validation: OK")
else:
    print("Configuration validation: FAILED")

# String utilities
test_string = "  HELLO WORLD  "
trimmed = trim(test_string)
print(f"Original: '{test_string}'")
print(f"Trimmed: '{trimmed}'")
print(f"Starts with 'HELLO': {starts_with(trimmed, 'HELLO')}")
print(f"To lowercase: {to_lower(trimmed)}")

# Statistical utilities
values = [1.0, 2.0, 3.0, 4.0, 5.0]
mean_val = mean(values)
stddev_val = stddev(values)
median_val = median(values)

print(f"Values: {values}")
print(f"Mean: {mean_val}")
print(f"Standard deviation: {stddev_val}")
print(f"Median: {median_val}")

# Error utilities
try:
    raise ConfigurationError("Test configuration error")
except ConfigurationError as e:
    error_type = get_error_type(e)
    error_details = get_error_details(e)
    print(f"Error type: {error_type}")
    print(f"Error details: {error_details}")
    
    # Log the error
    log_error(e)
    print(f"Error logged successfully")
```

### Context Managers

```python
from nlm_enhanced import EnhancedConfig

# Context manager for safe configuration management
with EnhancedConfig() as config:
    config.set("learning_rate", 0.01)
    config.set("batch_size", 32)
    print(f"Learning rate: {config['learning_rate']}")
    print(f"Batch size: {config['batch_size']}")

# Configuration is automatically cleaned up after context exit
```

### Iterator Support

```python
from nlm_enhanced import EnhancedConfig

# Iterator support for configuration iteration
config = EnhancedConfig()
config.set("learning_rate", 0.01)
config.set("batch_size", 32)
config.set("hidden_layers", 2)

# Iterate over configuration
for key, value in config.items():
    print(f"{key}: {value}")

# Check if configuration contains specific key
print(f"Contains 'learning_rate': {'learning_rate' in config}")
print(f"Contains 'nonexistent_key': {'nonexistent_key' in config}")
```

## Advanced Features

### Simulation with Error Handling

```python
from nlm_enhanced import createBrain, EnhancedBrain

# Create enhanced configuration
config = createDefaultConfig()

# Create enhanced brain
brain = createBrain(config)

# Initialize brain with error handling
try:
    brain.initialize()
    print("Brain initialized successfully")
except BrainError as e:
    print(f"Error initializing brain: {e}")
    sys.exit(1)

# Run simulation with error handling
for step in range(100):
    time = step * 0.01
    try:
        brain.step(step, time)
        if (step + 1) % 10 == 0:
            print(f"Step {step + 1}/100 completed")
    except BrainError as e:
        print(f"Error at step {step}: {e}")
        break

# Get brain statistics
try:
    stats = brain.get_statistics()
    print("Brain statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
except BrainError as e:
    print(f"Error getting statistics: {e}")
```

### Agent Brain with Advanced Features

```python
from nlm_enhanced import createBrain, EnhancedAgentBrain

# Create enhanced configuration
config = createDefaultConfig()

# Create enhanced brain and agent brain
brain = createBrain(config)
agent_brain = EnhancedAgentBrain(brain)

# Initialize agent brain
agent_brain.initialize()

# Enable agent features
agent_brain.enable_feature("curiosity")
agent_brain.disable_feature("structural_plasticity")

# Apply neuromodulation
agent_brain.apply_reward_modulation(reward=1.0, predicted_reward=0.8)
agent_brain.apply_prediction_error_modulation(prediction_error=0.5)

# Update development
for timestep in range(0, 10, 1):
    agent_brain.update_development(timestep=timestep)
    print(f"Development timestep {timestep} completed")

# Get feature status
feature_status = agent_brain.get_feature_status()
print(f"Feature status: {feature_status}")

# Get agent statistics
agent_stats = agent_brain.get_statistics()
print(f"Agent statistics: {agent_stats}")
```

### Type Conversion and Validation

```python
from nlm_enhanced import EnhancedConfig, NLMType, ConfigurationError

# Create enhanced configuration
config = EnhancedConfig()

# Test type conversion
print("Testing type conversion:")

# Set values with different types
config.set("learning_rate", 0.01)  # Float
config.set("batch_size", 32)  # Int
config.set("use_gpu", True)  # Bool
config.set("model_name", "neural_network")  # String

# Access with automatic type conversion
print(f"Learning rate: {config['learning_rate']} (type: {type(config['learning_rate']).__name__})")
print(f"Batch size: {config['batch_size']} (type: {type(config['batch_size']).__name__})")
print(f"Use GPU: {config['use_gpu']} (type: {type(config['use_gpu']).__name__})")
print(f"Model name: {config['model_name']} (type: {type(config['model_name']).__name__})")

# Test validation
print("\nTesting validation:")
if config.validate():
    print("Configuration is valid!")
else:
    print("Configuration is invalid!")

# Test invalid configuration
print("\nTesting invalid configuration:")
try:
    config.set("invalid_param", "value")
    if config.validate():
        print("Configuration is valid!")
    else:
        print("Configuration is invalid!")
except Exception as e:
    print(f"Error: {e}")
```

### Error Recovery and Retry

```python
from nlm_enhanced import createBrain, EnhancedBrain, BrainError

# Create enhanced configuration
config = createDefaultConfig()

# Create enhanced brain
brain = createBrain(config)

# Attempt simulation with error recovery
max_retries = 3
retry_count = 0

while retry_count < max_retries:
    try:
        # Initialize brain
        brain.initialize()
        print("Brain initialized successfully")
        break
    except BrainError as e:
        retry_count += 1
        print(f"Brain initialization failed (attempt {retry_count}): {e}")
        
        if retry_count >= max_retries:
            print("Max retries reached. Giving up.")
            sys.exit(1)
        else:
            # Wait before retry
            import time
            time.sleep(1.0)
            print("Retrying...")

# Run simulation with error handling
for step in range(100):
    time = step * 0.01
    try:
        brain.step(step, time)
        print(f"Step {step + 1}/100 completed")
    except BrainError as e:
        print(f"Error at step {step}: {e}")
        # Try to recover from error
        if "memory" in str(e).lower():
            print("Memory error detected. Attempting cleanup...")
            import gc
            gc.collect()
            continue
        else:
            print("Unrecoverable error. Stopping simulation.")
            break
```

### Performance Monitoring

```python
from nlm_enhanced import createBrain, EnhancedBrain

# Create enhanced configuration
config = createDefaultConfig()

# Create enhanced brain
brain = createBrain(config)

# Initialize brain
brain.initialize()

# Run simulation with performance monitoring
import time
start_time = time.time()

for step in range(100):
    step_start = time.time()
    
    # Perform simulation step
    brain.step(step, step * 0.01)
    
    step_end = time.time()
    step_time = step_end - step_start
    
    # Log step performance
    print(f"Step {step + 1}/100 completed in {step_time:.4f}s")

# Calculate total simulation time
total_time = time.time() - start_time
steps_per_second = 100 / total_time if total_time > 0 else 0

print(f"\nSimulation completed in {total_time:.2f}s")
print(f"Steps per second: {steps_per_second:.2f}")
print(f"Average step time: {total_time / 100:.4f}s")
```

## Migration Guide

### From pynlm to Enhanced Bindings

The enhanced Python bindings maintain backward compatibility with the original pynlm while adding new features:

#### Original pynlm API

```python
# Original pynlm usage
import pynlm

# Create brain
brain = pynlm.createBrain()

# Initialize brain
brain.initialize()

# Run simulation
for step in range(100):
    time = step * 0.01
    brain.step(step, time)

# Get statistics
stats = brain.getStatistics()
```

#### Enhanced Python Bindings API

```python
# Enhanced Python bindings usage
from nlm_enhanced import createBrain, EnhancedBrain

# Create enhanced brain
brain = createBrain()

# Initialize brain
brain.initialize()

# Run simulation
for step in range(100):
    time = step * 0.01
    brain.step(step, time)

# Get enhanced statistics
stats = brain.get_statistics()
```

#### Key Differences

1. **Type Safety**: Enhanced bindings provide automatic type conversion
2. **Error Handling**: Enhanced bindings provide detailed error handling
3. **Validation**: Enhanced bindings provide configuration validation
4. **Performance**: Enhanced bindings provide performance monitoring
5. **Backward Compatibility**: Enhanced bindings maintain compatibility with pynlm

### Migration Examples

#### Example 1: Configuration

```python
# Old pynlm configuration
import pynlm

config = pynlm.createDefaultConfig()
config.set("learning_rate", 0.01)
config.set("batch_size", 32)

# Enhanced Python bindings configuration
from nlm_enhanced import EnhancedConfig

config = EnhancedConfig()
config.set("learning_rate", 0.01)
config.set("batch_size", 32)
```

#### Example 2: Brain Creation

```python
# Old pynlm brain creation
import pynlm

brain = pynlm.createBrain()
brain.initialize()

# Enhanced Python bindings brain creation
from nlm_enhanced import createBrain

brain = createBrain()
brain.initialize()
```

#### Example 3: Simulation

```python
# Old pynlm simulation
import pynlm

brain = pynlm.createBrain()
brain.initialize()

for step in range(100):
    time = step * 0.01
    brain.step(step, time)

stats = brain.getStatistics()

# Enhanced Python bindings simulation
from nlm_enhanced import createBrain, EnhancedBrain

brain = createBrain()
brain.initialize()

for step in range(100):
    time = step * 0.01
    brain.step(step, time)

stats = brain.get_statistics()
```

## Performance Considerations

### 1. Use Type Safety

```python
# Good: Use type safety for better error handling
config = EnhancedConfig()
config.set("learning_rate", 0.01)  # Automatic type conversion

learning_rate = config["learning_rate"]  # Returns float with automatic type checking
```

### 2. Validate Configuration

```python
# Good: Validate configuration before simulation
if not config.validate():
    print("Configuration is invalid. Exiting.")
    sys.exit(1)

run_simulation(config)
```

### 3. Use Error Handling

```python
# Good: Use error handling for robust simulations
try:
    brain.initialize()
except BrainError as e:
    print(f"Error initializing brain: {e}")
    sys.exit(1)
```

### 4. Monitor Performance

```python
# Good: Monitor simulation performance
import time
start_time = time.time()

for step in range(100):
    step_start = time.time()
    brain.step(step, step * 0.01)
    step_time = time.time() - step_start
    print(f"Step {step + 1}/100 completed in {step_time:.4f}s")

total_time = time.time() - start_time
print(f"Total simulation time: {total_time:.2f}s")
```

## Common Issues and Solutions

### Issue 1: Type Conversion Errors

**Symptoms**: Type conversion errors when accessing configuration values.

**Solution**: Use proper type conversion with enhanced bindings:

```python
from nlm_enhanced import EnhancedConfig

# Good: Use type-safe configuration
config = EnhancedConfig()
config.set("learning_rate", 0.01)  # Automatic type conversion

learning_rate = config["learning_rate"]  # Returns float with type checking
print(f"Learning rate: {learning_rate} (type: {type(learning_rate).__name__})")
```

### Issue 2: Validation Errors

**Symptoms**: Configuration validation fails with errors.

**Solution**: Use enhanced validation:

```python
from nlm_enhanced import EnhancedConfig

# Good: Use enhanced validation
config = EnhancedConfig()
config.set("learning_rate", 0.01)

if not config.validate():
    print("Configuration validation errors:")
    for error in config.get_validation_errors():
        print(f"  - {error}")
    sys.exit(1)
```

### Issue 3: Error Handling

**Symptoms**: Simulation fails with unclear error messages.

**Solution**: Use enhanced error handling:

```python
from nlm_enhanced import createBrain, EnhancedBrain, BrainError

# Good: Use enhanced error handling
brain = createBrain()

try:
    brain.initialize()
except BrainError as e:
    print(f"Brain initialization error: {e}")
    print(f"Error type: {e.type()}")
    sys.exit(1)
```

## Conclusion

The enhanced Python bindings provide powerful features for NLM neural simulations:

1. **Type Safety**: Automatic type conversion and validation
2. **Error Handling**: Comprehensive error handling with detailed error information
3. **Pythonic Interfaces**: Intuitive Python interfaces with context managers
4. **Performance Monitoring**: Performance monitoring and optimization
5. **Backward Compatibility**: Maintains compatibility with existing pynlm

By using the enhanced Python bindings, you can:

- Write cleaner, more maintainable simulation code
- Use type-safe configuration with automatic validation
- Handle errors gracefully with detailed error information
- Monitor simulation performance with built-in tools
- Maintain backward compatibility with existing code

Start using the enhanced Python bindings today to improve your NLM neural simulations!
