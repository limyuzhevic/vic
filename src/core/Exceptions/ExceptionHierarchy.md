# NLM Exception Hierarchy Integration Guide

This document provides comprehensive information about the NLM exception hierarchy implementation, including error handling patterns and integration instructions.

## Overview

The NLM (Neural Learning Machine) project now includes a comprehensive exception hierarchy designed to provide consistent, context-rich error handling across all neural system components. The hierarchy includes:

### Base Exception Classes

#### NLMException (Base Class)
- **Location**: `src/core/Exceptions/NLMException.hpp`
- **Description**: Base exception class for all NLM-specific exceptions
- **Key Features**:
  - Error messages with context information
  - Error codes for categorized error tracking
  - Timestamps for debugging temporal issues
  - Optional stack traces for detailed error analysis
  - Rich context map for debugging environment

#### Derived Exception Classes

1. **ConfigurationError** (`ConfigurationError.hpp`)
   - Purpose: Configuration-related errors
   - Error Code: `CONFIG_ERROR`

2. **MemoryError** (`MemoryError.hpp`)
   - Purpose: Memory allocation and management errors
   - Error Code: `MEMORY_ERROR`

3. **NeuralError** (`NeuralError.hpp`)
   - Purpose: Neural computation and dynamics errors
   - Error Code: `NEURAL_ERROR`

4. **PlasticityError** (`PlasticityError.hpp`)
   - Purpose: Synaptic plasticity and learning errors
   - Error Code: `PLASTICITY_ERROR`

5. **IntegrationError** (`IntegrationError.hpp`)
   - Purpose: Component integration errors
   - Error Code: `INTEGRATION_ERROR`

6. **DevelopmentError** (`DevelopmentError.hpp`)
   - Purpose: Developmental stage errors
   - Error Code: `DEVELOPMENT_ERROR`

7. **NeuromodulationError** (`NeuromodulationError.hpp`)
   - Purpose: Neuromodulator signaling errors
   - Error Code: `NEUROMODULATION_ERROR`

8. **MemorySystemError** (`MemorySystemError.hpp`)
   - Purpose: Memory system operational errors
   - Error Code: `MEMORY_SYSTEM_ERROR`

9. **PredictionError** (`PredictionError.hpp`)
   - Purpose: Prediction system errors
   - Error Code: `PREDICTION_ERROR`

10. **CognitiveError** (`CognitiveError.hpp`)
    - Purpose: Cognitive processing errors
    - Error Code: `COGNITIVE_ERROR`

11. **SensoryError** (`SensoryError.hpp`)
    - Purpose: Sensory processing errors
    - Error Code: `SENSORY_ERROR`

12. **MotorError** (`MotorError.hpp`)
    - Purpose: Motor control errors
    - Error Code: `MOTOR_ERROR`

13. **WorldError** (`WorldError.hpp`)
    - Purpose: Environment/world interaction errors
    - Error Code: `WORLD_ERROR`

14. **AgentError** (`AgentError.hpp`)
    - Purpose: Agent-level errors
    - Error Code: `AGENT_ERROR`

### Utility Classes and Macros

#### ExceptionUtils (`ExceptionUtils.hpp`)
- **Location**: `src/core/Exceptions/ExceptionUtils.hpp`
- **Description**: Utility functions for exception handling
- **Key Features**:
  - Exception throwing macros: `NLM_THROW`, `NLM_THROW_WITH_CONTEXT`
  - Exception handling macros: `NLM_TRY_CATCH`, `NLM_CATCH_AND_LOG`
  - Exception factory functions for each exception type
  - Exception formatting utilities

#### Demonstration Example (`DemonstrationExample.cpp`)
- **Location**: `src/core/Exceptions/DemonstrationExample.cpp`
- **Description**: Comprehensive example demonstrating exception hierarchy usage
- **Features**:
  - All 14 exception types demonstrated
  - Production code patterns
  - Error codes and context information
  - Exception serialization examples

### Exception Hierarchy Benefits

1. **Consistent Error Handling**: All components use the same error handling patterns
2. **Rich Context Information**: Errors include detailed context for debugging
3. **Temporal Tracking**: Timestamps help track error progression
4. **Hierarchical Classification**: Easy error categorization and filtering
5. **Debug Support**: Stack traces and detailed error information
6. **Extensibility**: Easy to add new exception types
7. **Integration**: Works with existing NLM logging system

## Integration Instructions

### 1. Include Headers in Existing Code

Add `#include "NLMException.hpp"` to files that need exception handling:

```cpp
#include "NLMException.hpp"
#include "ExceptionUtils.hpp"
```

### 2. Update Exception Handling Patterns

#### Before (Standard C++ exceptions):

```cpp
try {
    config->loadFromFile(filepath);
} catch (const std::exception& e) {
    NLM_LOG_ERROR("Failed to load config: " + std::string(e.what()));
}
```

#### After (NLM exceptions):

```cpp
#include "ConfigurationError.hpp"
#include "ExceptionUtils.hpp"

NLM_TRY_CATCH(config->loadFromFile(filepath),
    [](const NLMException& e) {
        NLM_LOG_ERROR(formatException(e));
    })
```

### 3. Create Component-Specific Exceptions

For each major component, create specific exception classes that inherit from the appropriate base:

```cpp
// In src/core/Exceptions/ directory
class BrainError : public NeuralError {
    explicit BrainError(const std::string& message) 
        : NeuralError(message, "BRAIN_ERROR") {}
};
```

### 4. Integration Points for Exception Throwing

Add exception throwing throughout the codebase where appropriate:

#### Configuration System (`src/core/Config/Config.cpp`)
```cpp
if (key.empty()) {
    NLM_THROW(ConfigurationError, "Configuration key cannot be empty");
}
```

#### Memory System (`src/memory/NeuralWorkingMemory.cpp`)
```cpp
if (capacity > maxCapacity) {
    NLM_THROW(MemoryError, "Requested memory capacity exceeds available memory");
}
```

#### Neural Computation (`src/brain/Neuron.cpp`)
```cpp
if (state.membranePotential > threshold) {
    NLM_THROW(NeuralError, "Neuron threshold exceeded during computation");
}
```

### 5. Production Error Handling Examples

#### Exception Catching and Logging:

```cpp
try {
    performComplexOperation();
} catch (const ConfigurationError& e) {
    NLM_LOG_ERROR(formatException(e));
    // Retry or graceful degradation
} catch (const MemoryError& e) {
    NLM_LOG_ERROR("Memory system error: " + e.getMessage());
    // Emergency shutdown
} catch (const NLMException& e) {
    NLM_LOG_ERROR(formatException(e));
} catch (const std::exception& e) {
    NLM_THROW(ConfigurationError, "Unexpected error: " + std::string(e.what()));
}
```

#### Context-Rich Error Creation:

```cpp
// Create error with rich context for debugging
std::map<std::string, std::string> context = {
    {"operation", "load_configuration"},
    {"file", "brain_config.yaml"},
    {"line", "42"},
    {"timestamp", std::to_string(currentTime())},
    {"user_id", currentUserId()}
};

throw makeConfigurationError("Invalid configuration value", context);
```

### 6. Testing and Validation

#### Exception Testing:

```cpp
TEST(ConfigurationErrorTest, TestErrorMessage) {
    ConfigurationError e("Test error", "CONFIG_TEST");
    EXPECT_EQ(e.getMessage(), "Test error");
    EXPECT_EQ(e.getCode(), "CONFIG_TEST");
}

TEST(ExceptionContextTest, TestContextStorage) {
    std::map<std::string, std::string> context = {{ "key", "value" }};
    ConfigurationError e("Test", "CONFIG_TEST", context);
    
    auto retrievedContext = e.getContext();
    EXPECT_EQ(retrievedContext["key"], "value");
}
```

### 7. Migration Path

#### Legacy Exception Migration:

Convert existing std::exception throws to NLM exceptions:

```cpp
// Old
throw std::invalid_argument("Invalid neuron type");

// New
NLM_THROW(NeuralError, "Invalid neuron type");
```

### 8. Performance Considerations

- Exception objects are lightweight and copied efficiently
- Context storage is only allocated when needed
- Stack trace collection is optional
- Logging macros provide minimal overhead

### 9. Documentation and Examples

- Documentation: See `DemonstrationExample.cpp` for comprehensive examples
- Integration guide: This file
- Component-specific documentation: Check individual module documentation

## Best Practices

1. **Always use NLM exceptions**: Never throw std::exception directly for NLM-specific errors
2. **Provide context**: Always include relevant context information in errors
3. **Consistent error codes**: Use standardized error codes for each exception type
4. **Graceful degradation**: Handle exceptions appropriately in production
5. **Logging**: Use NLM logging system for exception reporting
6. **Testing**: Test exception handling paths thoroughly
7. **Serialization**: Consider serializing exception details for persistent logging

## Files Created

### Core Exception Classes
- `src/core/Exceptions/NLMException.hpp` - Base exception class
- `src/core/Exceptions/NLMException.cpp` - Base exception implementation
- Individual exception headers for all 14 exception types

### Utilities and Examples
- `src/core/Exceptions/ExceptionUtils.hpp` - Exception handling utilities
- `src/core/Exceptions/DemonstrationExample.cpp` - Usage examples
- `src/core/Exceptions/ExceptionHierarchy.md` - This integration guide

### Build System Updates
- CMakeLists.txt updated to include exceptions directory
- Python bindings may need updates for exception exposure

## Conclusion

The NLM exception hierarchy provides a robust, extensible foundation for error handling throughout the neural learning machine system. By implementing consistent error handling patterns and providing rich context information, developers can more easily diagnose, debug, and resolve issues in the complex neural system.

The hierarchy is designed to grow with the system, allowing for new exception types as the neural architecture evolves. Integration with existing logging and checkpoint systems ensures comprehensive error reporting and analysis capabilities.
