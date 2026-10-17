# NLM Error Handling and Validation System

This document describes the comprehensive error handling and validation system for the NLM (Neural Learning Machine) framework.

## Overview

The NLM error handling system provides:

1. **Hierarchical exception types** with error codes and source locations
2. **Comprehensive validation** for parameters, state, and resources
3. **Detailed error reporting** with context and metrics collection
4. **Graceful recovery** mechanisms for long-running processes
5. **Integration hooks** for easy adoption across the codebase

## Core Components

### 1. Error Handling Classes

**Error.hpp**
- Base exception class `NLMException` with error codes and source locations
- Specific exception types for different error categories:
  - `ConfigurationException` - Configuration-related errors
  - `NeuralComputationException` - Neural computation errors
  - `MemoryException` - Memory management errors
  - `IOError` - File I/O errors
  - `ValidationException` - Parameter and state validation errors
  - `ResourceException` - Resource exhaustion errors
  - `IntegrationException` - Component integration errors
  - `RecoveryException` - Recovery mechanism errors

**Error Reporting Infrastructure**
- Multiple error handlers: `ConsoleErrorHandler`, `FileErrorHandler`
- Metrics collector for error statistics
- Error recovery manager with graceful degradation support
- Global error reporting infrastructure

### 2. Validation Utilities

**Validation.hpp**
- **Parameter validation**: Type checking, range validation, constraint checking
- **State validation**: Precondition/postcondition checking
- **Resource validation**: Memory, file handles, system resources
- **Integration validation**: Component compatibility and data flow validation

**ValidationUtils**
- `isValid()` - Generic parameter validation
- `isInRange()` - Range checking with inclusive/exclusive options
- `isPositive()` - Positive value validation
- `isFinite()` - NaN/Inf checking
- `matchesPattern()` - String pattern matching
- `isMonotonic()` - Sequence monotonicity validation
- `validateFilePath()` - File path validation
- `validateDirectoryPath()` - Directory path validation

**StateValidator**
- `isValidState()` - State validation using object's isValid() method
- `checkCondition()` - Conditional validation with custom predicates
- `validateMemoryState()` - Memory integrity checking

**ResourceValidator**
- `validateMemory()` - Available memory checking
- `validateFileHandle()` - File handle validation
- `validateNetworkResource()` - Network resource validation
- `validateSystemResource()` - System resource usage validation

### 3. Error Reporting Infrastructure

**ErrorReporting.hpp**
- `IErrorHandler` - Abstract base for error handlers
- `ConsoleErrorHandler` - Console-based error reporting with colors
- `FileErrorHandler` - File-based error reporting with formatting options
- `MetricsCollector` - Error statistics and metrics collection
- `ErrorRecoveryManager` - Recovery mechanism management
- `ErrorReportingInfrastructure` - Global error reporting infrastructure

### 4. Error Handling System

**ErrorHandlingSystem.hpp**
- Singleton pattern for global error handling
- Automatic initialization and shutdown
- Centralized error reporting and recovery
- Integration with existing logging systems

## Key Features

### 1. Error Code Categorization

Errors are categorized by:
- **ErrorCategory**: High-level categories (Configuration, NeuralComputation, Memory, etc.)
- **ErrorCode**: Specific error codes within each category
- **ErrorContext**: Detailed context information including source location, timestamps, metadata

### 2. Hierarchical Exception Handling

```cpp
// Configuration exception
try {
    config.set("", "value");
} catch (const nlm::ConfigurationException& e) {
    // Handle configuration-specific errors
}

// Validation exception
try {
    neuron.validateId(invalidId);
} catch (const nlm::ValidationException& e) {
    // Handle validation errors
}
```

### 3. Validation Macros

```cpp
// Configuration validation
NLM_THROW_CONFIG("Invalid configuration value")

// Neural computation validation
NLM_THROW_NEURAL("Invalid neural state")

// Memory validation
NLM_THROW_MEMORY("Memory allocation failed")

// I/O validation
NLM_THROW_IO("File not found")

// Parameter validation
NLM_THROW_VALIDATION("Invalid parameter value")
```

### 4. Graceful Degradation

The system supports graceful degradation for recoverable errors:

```cpp
enableGracefulDegradation(true);

// Errors will attempt recovery automatically
// If recovery fails, system continues with degraded functionality
```

### 5. Metrics and Statistics

The system collects comprehensive error statistics:

```cpp
nlm::ErrorStats stats = getErrorStats();
std::cout << stats.getStatsSummary() << std::endl;
```

### 6. Integration with Existing Code

To integrate with existing code:

1. **Replace `throw std::invalid_argument` with appropriate macros**
2. **Replace `throw std::runtime_error` with appropriate macros**
3. **Add `#include "ErrorHandling/Error.hpp"` to relevant headers**
4. **Initialize error handling system in main()**

### 7. Performance Considerations

- **Minimal overhead in production**: Error reporting can be disabled
- **Small object optimizations**: Used `isSmallObject()` for efficient storage
- **Lazy validation**: Validation only when explicitly requested
- **Efficient error context**: Only collect necessary metadata

## Usage Examples

### 1. Basic Error Handling

```cpp
#include "ErrorHandling/ErrorHandlingSystem.hpp"

int main() {
    // Initialize error handling system
    initializeErrorHandling();
    
    try {
        nlm::Config config;
        config.set("key", "value");
    } catch (const nlm::NLMException& e) {
        reportError(e.getContext());
    }
    
    shutdownErrorHandling();
    return 0;
}
```

### 2. Custom Recovery Handler

```cpp
// Register recovery handler for configuration errors
registerRecoveryHandler(
    nlm::ErrorCategory::Configuration,
    [](const nlm::ErrorContext& context) {
        // Implement recovery logic here
        if (context.message.find("missing file") != std::string::npos) {
            // Try to load default configuration
            return loadDefaultConfig();
        }
        return false;
    }
);
```

### 3. File-based Error Logging

```cpp
#include "ErrorHandling/ErrorReporting.hpp"

int main() {
    initializeErrorHandling("", true);
    
    // Add file error handler
    auto fileHandler = std::make_shared<nlm::FileErrorHandler>("nlm_errors.log");
    fileHandler->setIncludeTimestamp(true);
    fileHandler->setIncludeSourceInfo(true);
    
    ErrorReportingInfrastructure::getInstance().addErrorHandler(fileHandler);
    
    // Now errors will be written to file as well
    try {
        // Code that may generate errors
    } catch (...) {
        // Errors will be reported to both console and file
    }
    
    return 0;
}
```

## Migration Guide

### From Standard Exceptions to NLM Exceptions

Replace:

```cpp
throw std::invalid_argument("Invalid parameter");
throw std::runtime_error("Initialization failed");
throw std::out_of_range("Value out of range");
```

With:

```cpp
NLM_THROW_VALIDATION("Invalid parameter");
NLM_THROW_NEURAL("Initialization failed");
NLM_THROW_VALIDATION("Value out of range");
```

### Files to Update

1. **Config.hpp/Config.cpp** - Already updated
2. **Neuron.hpp/Neuron.cpp** - Already updated  
3. **Synapse.hpp/Synapse.cpp** - Already updated
4. **Brain.hpp/Brain.cpp** - Already updated
5. **Logger.hpp/Logger.cpp** - Already updated
6. **Random.hpp/Random.cpp** - Already updated

## Build Instructions

The error handling system is part of the main NLM build. No additional build steps are required.

```bash
cd /workspace/app
echo "Build commands will be determined by the build system (CMake, pyproject.toml, etc.)" && ls -la
```

## Testing

The system includes comprehensive test examples in `example.cpp`. Compile and run:

```bash
g++ -std=c++20 example.cpp src/core/Config/Config.cpp src/core/Logger/Logger.cpp src/brain/Neuron.cpp -o example
./example
```

## Conclusion

The NLM error handling and validation system provides a robust, extensible framework for error management throughout the neural simulation codebase. It enables developers to:

- Quickly identify and handle errors
- Maintain detailed error logs for debugging
- Implement recovery strategies for long-running processes
- Ensure data integrity through comprehensive validation
- Monitor system health through error statistics

The system is designed to be lightweight in production while providing rich debugging information during development.
