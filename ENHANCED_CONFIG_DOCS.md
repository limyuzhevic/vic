# Enhanced Configuration System - Documentation

## Overview

The NLM (Neural Learning Machine) enhanced configuration system provides:

1. **Comprehensive Type Support**: Full support for basic types (int, double, string, bool) and complex vector types (vector<int>, vector<double>, vector<string>)

2. **Automatic Type Detection**: The `getAuto()` method automatically detects the type of stored values and returns them as the appropriate C++ type

3. **Fallback Support**: The `getOrAuto()` method provides safe fallback values when configuration keys are missing

4. **Improved File I/O**: Enhanced save/load functionality with proper vector formatting and parsing

5. **Better Error Handling**: Robust type conversion with automatic error handling

## Key Features

### 1. Type Conversion Utilities

The configuration system includes comprehensive type conversion utilities:

```cpp
// Basic type conversion
auto value = config->stringToInt("key");
auto value = config->stringToDouble("key");
auto value = config->stringToBool("key");
auto value = config->stringToString("key");

// Vector type conversion
auto values = config->stringToVectorInt("key");
auto values = config->stringToVectorDouble("key");
auto values = config->stringToVectorString("key");
```

### 2. Enhanced Get Methods

```cpp
// Traditional get() - requires exact type matching
auto value = config->get<int>("learning_rate");

// Enhanced getAuto() - automatic type detection
auto value = config->getAuto<double>("learning_rate");  // Returns std::optional<double>
auto value = config->getAuto<std::vector<std::string>("neuron_types");  // Returns std::optional<vector<string>>

// Enhanced getOr() - traditional fallback
auto value = config->getOr<double>("learning_rate", 0.01);

// Enhanced getOrAuto() - automatic type detection with fallback
auto value = config->getOrAuto<double>("learning_rate", 0.01);
auto value = config->getOrAuto<std::vector<int>("layers", std::vector<int>{1, 2, 3});
```

### 3. Vector Support

The configuration system now fully supports vector types:

```cpp
// Store vector values
config->set("learning_rates", std::vector<double>{0.01, 0.001, 0.0001});
config->set("neuron_types", std::vector<std::string>{"excitatory", "inhibitory", "sensory"});
config->set("connection_layers", std::vector<int>{1, 2, 3, 4, 5});

// Retrieve vector values
auto rates = config->getOr<std::vector<double>("learning_rates", std::vector<double>{0.01});
auto types = config->getOrAuto<std::vector<std::string>("neuron_types", std::vector<std::string>{"default"});
```

### 4. File I/O Improvements

Configuration files now properly support vector formats:

```plaintext
# Example configuration file
# Learning rates for different layers
learning_rates = [0.01, 0.001, 0.0001]

# Neuron type definitions
neuron_types = ["excitatory", "inhibitory", "sensory", "modulatory"]

# Connection layer indices
connection_layers = [1, 2, 3, 4, 5]

# Scalar values
integer_value = 42
double_value = 3.14159
string_value = "Hello World"
bool_value = true
```

### 5. Enhanced Summary

The summary() method now displays all value types correctly:

```
Configuration (7 entries):
  integer_value = [42] (3)
  double_value = [3.14159] (3)
  string_value = ["Hello World"] (3)
  bool_value = [true] (3)
  learning_rates = [[0.01, 0.001, 0.0001]] (4)
  neuron_types = [["excitatory", "beta", "gamma"]] (4)
  connection_layers = [[1, 2, 3, 4, 5]] (4)
```

## Migration Guide

### From Old to New API

**Old API (Limited Type Support)**:
```cpp
// Only basic types supported
auto value = config->get<int>("key");
auto value = config->getOr<double>("key", 0.0);
```

**New API (Enhanced Type Support)**:
```cpp
// Basic types with enhanced error handling
auto value = config->getAuto<int>("key");  // Automatic type detection
auto value = config->getOrAuto<double>("key", 0.0);  // With fallback

// Vector types (NEW!)
auto values = config->getAuto<std::vector<int>("key");
auto values = config->getOrAuto<std::vector<std::string>("key", std::vector<std::string>{"default"});
```

## Benefits

1. **Type Safety**: Automatic type detection reduces compilation errors
2. **Flexibility**: Support for complex data structures
3. **Backward Compatibility**: Existing code continues to work
4. **Ease of Use**: Simplified API for common operations
5. **Better Error Handling**: Graceful handling of missing or malformed data
6. **Improved File I/O**: Proper vector formatting and parsing

## Use Cases

### 1. Machine Learning Configuration
```cpp
config->set("learning_rate", 0.01);
config->set("batch_size", 32);
config->set("hidden_layers", std::vector<int>{128, 64, 32});
config->set("activation_functions", std::vector<std::string>{"relu", "relu", "softmax"});
```

### 2. Neural Network Architecture
```cpp
config->set("input_size", 784);
config->set("output_size", 10);
config->set("layer_sizes", std::vector<int>{256, 128, 64, 32});
config->set("dropout_rates", std::vector<double>{0.2, 0.3, 0.4, 0.5});
```

### 3. Simulation Parameters
```cpp
config->set("time_step", 0.001);
config->set("simulation_steps", 10000);
config->set("neuron_parameters", std::vector<double>{-70.0, -55.0, -50.0, -45.0});
config->set("synapse_weights", std::vector<double>{0.1, 0.2, 0.3, 0.4, 0.5});
```

## Future Enhancements

The enhanced configuration system provides a foundation for:

1. **JSON/YAML Support**: Next phase will implement proper JSON/YAML parsing
2. **Configuration Templates**: Common configuration profiles for different use cases
3. **Validation**: Type validation and constraint checking
4. **Inheritance**: Configuration inheritance and merging
5. **Environment Variables**: Support for environment variable configuration
6. **Database Integration**: Configuration storage in databases

## Testing

The enhanced configuration system has been tested with:

1. **Basic Type Operations**: int, double, string, bool
2. **Vector Type Operations**: vector<int>, vector<double>, vector<string>
3. **File I/O**: Save and load with vector support
4. **Error Handling**: Missing keys, type conversion errors
5. **Memory Management**: Proper cleanup and resource handling

All tests pass successfully, demonstrating the robustness of the enhanced configuration system.