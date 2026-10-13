# NLM Exception Hierarchy Implementation

## Overview

This document provides a comprehensive overview of the NLM (Neural Learning Machine) exception hierarchy implementation. The exception system was designed to provide a robust, extensible framework for error handling throughout the NLM project.

## Architecture

### Base Exception Class: NLMException

The `NLMException` class serves as the abstract base class for all NLM-specific exceptions. It provides:

#### Core Features
- **Error Codes**: Enumerated error codes organized by category
- **Error Messages**: Detailed, human-readable error descriptions
- **Source Location**: Automatic tracking of where exceptions were thrown (file, line, function)
- **Timestamps**: Automatic recording of when exceptions were created
- **Exception Chaining**: Support for nested exceptions (exception causes)
- **Metadata**: Key-value pairs for additional context
- **Serialization**: Conversion to string format for logging and debugging
- **Cloning**: Ability to copy exceptions for error handling

#### Error Code Categories

The error codes are organized into the following categories:

1. **Generic Errors (0-9)**: Common error types
   - `Unknown = 0`
   - `NotImplemented = 1`
   - `InvalidArgument = 2`
   - `OutOfRange = 3`
   - `NotFound = 4`
   - `AlreadyExists = 5`
   - `AccessDenied = 6`
   - `PermissionDenied = 7`
   - `Timeout = 8`
   - `Interrupted = 9`

2. **Configuration Errors (100-105)**: Configuration loading/parameter errors
   - `FileNotFound = 100`
   - `ParseError = 101`
   - `InvalidParameter = 102`
   - `MissingRequiredParameter = 103`
   - `InvalidValue = 104`
   - `ValidationFailed = 105`

3. **Initialization Errors (200-204)**: Initialization failures
   - `Failed = 200`
   - `ResourceNotAvailable = 201`
   - `InvalidState = 202`
   - `DependencyFailed = 203`
   - `Timeout = 204`

4. **Simulation Errors (300-304)**: Runtime simulation errors
   - `StepFailed = 300`
   - `InvalidStep = 301`
   - `ConvergenceFailed = 302`
   - `TimeStepTooSmall = 303`
   - `NumericalInstability = 304`

5. **Memory Errors (400-404)**: Memory allocation/management errors
   - `AllocationFailed = 400`
   - `Corruption = 401`
   - `OutOfMemory = 402`
   - `InvalidDeallocation = 403`
   - `MemoryLeak = 404`

6. **I/O Errors (500-506)**: File I/O errors
   - `FileNotFound = 500`
   - `FilePermissionDenied = 501`
   - `FileReadError = 502`
   - `FileWriteError = 503`
   - `DirectoryNotFound = 504`
   - `PathNotFound = 505`
   - `DeviceError = 506`

7. **Neural Computation Errors (600-605)**: Neural computation errors
   - `NetworkError = 600`
   - `InvalidTopology = 601`
   - `InvalidWeights = 602`
   - `ConvergenceError = 603`
   - `ActivationError = 604`
   - `BackpropagationError = 605`

8. **System Errors (700-705)**: System-level errors
   - `SystemError = 700`
   - `SystemCallFailed = 701`
   - `ResourceExhausted = 702`
   - `InvalidOperation = 703`
   - `NotSupported = 704`
   - `OutOfBounds = 705`

### Specialized Exception Classes

Each category is represented by a dedicated exception class:

#### ConfigurationException
- **Purpose**: Configuration loading and parameter errors
- **Use Case**: Invalid config files, missing parameters, validation failures

#### InitializationException
- **Purpose**: Initialization failures
- **Use Case**: Resource unavailable, dependency issues, timeouts

#### SimulationException
- **Purpose**: Runtime simulation errors
- **Use Case**: Convergence failures, numerical instability, invalid steps

#### MemoryException
- **Purpose**: Memory allocation/management errors
- **Use Case**: Allocation failures, corruption, out of memory

#### IOException
- **Purpose**: File I/O errors
- **Use Case**: File not found, permission denied, read/write errors

#### NeuralException
- **Purpose**: Neural computation errors
- **Use Case**: Network topology issues, weight problems, convergence errors

#### SystemException
- **Purpose**: System-level errors
- **Use Case**: System call failures, resource exhaustion, invalid operations

## API Reference

### Constructor Usage

All exception classes provide the following constructor signatures:

```cpp
// Standard constructor
ExceptionType(ErrorTypeCode code, const std::string& message,
             const std::string& file = "",
             int line = 0,
             const std::string& function = "");
```

**Parameters:**
- `code`: Error code from the exception-specific enum
- `message`: Human-readable error description
- `file`: Source file where the exception was thrown (auto-filled if empty)
- `line`: Source line where the exception was thrown (auto-filled if 0)
- `function`: Source function where the exception was thrown (auto-filled if empty)

### Factory Functions

The exception hierarchy provides helper functions for creating and throwing exceptions:

#### createException
```cpp
template<typename ExceptionType, typename ErrorCodeType>
std::unique_ptr<ExceptionType> createException(
    ErrorCodeType code, 
    const std::string& message,
    std::source_location location = std::source_location::current() // C++20
);
```

#### throwException
```cpp
template<typename ExceptionType, typename ErrorCodeType>
void throwException(
    ErrorCodeType code, 
    const std::string& message,
    std::shared_ptr<NLMException> cause = nullptr,
    std::source_location location = std::source_location::current() // C++20
);
```

### Core Methods

#### NLMException Public Interface

| Method | Description |
|--------|-------------|
| `what()` | Returns the error message as C-string |
| `getErrorCode()` | Returns the error code |
| `getMessage()` | Returns the error message |
| `getFile()` | Returns the source file |
| `getLine()` | Returns the source line |
| `getFunction()` | Returns the source function |
| `getTimestamp()` | Returns the creation timestamp |
| `getCause()` | Returns the cause exception (if any) |
| `hasCause()` | Checks if the exception has a cause |
| `addMetadata(key, value)` | Adds metadata for additional context |
| `getMetadata(key)` | Retrieves metadata value |
| `getMetadata()` | Retrieves all metadata |
| `serialize()` | Returns serialized string representation |
| `formatWithCause()` | Returns formatted string with cause information |
| `is<T>()` | Checks if exception is of specific type |
| `getCauseAs<T>()` | Gets cause as specific exception type |

#### Type-Specific Methods

Each exception class provides a type-specific method:

```cpp
// For ConfigurationException
ConfigurationErrorCode getConfigurationErrorCode() const;

// For InitializationException
InitializationErrorCode getInitializationErrorCode() const;

// For SimulationException
SimulationErrorCode getSimulationErrorCode() const;

// For MemoryException
MemoryErrorCode getMemoryErrorCode() const;

// For IOException
IOErrorCode getIOErrorCode() const;

// For NeuralException
NeuralErrorCode getNeuralErrorCode() const;

// For SystemException
SystemErrorCode getSystemErrorCode() const;
```

### Exception Chaining

The exception hierarchy supports nesting exceptions to provide detailed error context:

```cpp
try {
    // Some operation that fails
    throw ConfigurationException(...);
} catch (const ConfigurationException& config_exc) {
    try {
        // Another operation that depends on the first
        throw IOException(...);
    } catch (const IOException& io_exc) {
        // Chain the exceptions
        io_exc.setCause(std::make_shared<ConfigurationException>(config_exc));
        throw; // Re-throw the chained exception
    }
}
```

**Chaining Benefits:**
- Complete error context from root cause to immediate failure
- Detailed debugging information
- Proper exception propagation
- Enhanced error reporting

### Serialization

Exceptions can be serialized to string format for logging and debugging:

```cpp
ConfigurationException exc(ConfigurationErrorCode::InvalidParameter, "Invalid parameter");
exc.addMetadata("param_name", "learning_rate");
std::string serialized = exc.serialize();
// Result: [ConfigErrorCode=102][ErrorCode=102][Message=Invalid parameter]...
```

**Serialized Format:**
- `[ErrorCode=X]` - Error code
- `[Message=...]` - Error message
- `[File=...]` - Source file
- `[Line=X]` - Source line
- `[Function=...]` - Source function
- `[Timestamp=X]` - Creation timestamp
- `[Cause=...]` - Serialized cause (if present)
- `[Key=Value]` - Metadata entries (if present)

### Type Checking and Casting

The exception hierarchy provides methods for type checking and safe casting:

```cpp
try {
    throwSomeException();
} catch (const NLMException& exc) {
    if (exc.is<ConfigurationException>()) {
        auto config_exc = std::dynamic_pointer_cast<ConfigurationException>(&exc);
        // Use configuration-specific methods
    }
    
    if (exc.is<MemoryException>()) {
        auto mem_exc = exc.getCauseAs<MemoryException>();
        // Safely access memory exception details
    }
}
```

## Best Practices

### 1. Using the Exception Hierarchy

**For C++17 and earlier:**
```cpp
#include "NLMErrors.hpp"

void loadConfiguration(const std::string& filename) {
    try {
        // Configuration loading logic
        if (/* file not found */) {
            throw ConfigurationException(
                ConfigurationException::ConfigurationErrorCode::FileNotFound,
                "Configuration file not found: " + filename,
                __FILE__, __LINE__, __func__
            );
        }
    } catch (const ConfigurationException& exc) {
        // Handle configuration error
        std::cerr << "Configuration error: " << exc.what() << std::endl;
        // Add context
        exc.addMetadata("filename", filename);
        throw; // Re-throw with additional context
    }
}
```

**For C++20:**
```cpp
void loadConfiguration(const std::string& filename) {
    try {
        // Configuration loading logic
        if (/* file not found */) {
            throwException<ConfigurationException>(
                ConfigurationException::ConfigurationErrorCode::FileNotFound,
                "Configuration file not found: " + filename
            );
        }
    } catch (const ConfigurationException& exc) {
        // Handle configuration error
        std::cerr << "Configuration error: " << exc.what() << std::endl;
        // Add context
        exc.addMetadata("filename", filename);
        throw; // Re-throw with additional context
    }
}
```

### 2. Exception Chaining Best Practices

```cpp
void performNeuralTraining(const NeuralNetwork& network) {
    try {
        // Preprocessing step
        preProcessData(network);
    } catch (const ConfigurationException& config_exc) {
        try {
            // Training step that depends on preprocessing
            trainNetwork(network);
        } catch (const MemoryException& mem_exc) {
            // Chain the exceptions with context
            mem_exc.setCause(std::make_shared<ConfigurationException>(config_exc));
            throw ExceptionWithContext(
                "Neural training failed after preprocessing error",
                mem_exc
            );
        }
    }
}
```

### 3. Serialization for Logging

```cpp
void logError(const std::exception& exc) {
    // Convert to NLMException for detailed logging
    std::shared_ptr<NLMException> nlm_exc;
    
    if (const NLMException* nlm_ptr = dynamic_cast<const NLMException*>(&exc)) {
        nlm_exc = std::const_pointer_cast<NLMException>(std::shared_ptr<const NLMException>(nlm_ptr));
    }
    
    if (nlm_exc) {
        std::cerr << "Logged error: " << nlm_exc->serialize() << std::endl;
        std::cerr << "Formatted: " << nlm_exc->formatWithCause() << std::endl;
    }
}
```

### 4. Error Handling Patterns

**Basic Pattern:**
```cpp
try {
    criticalOperation();
} catch (const ConfigurationException& exc) {
    handleConfigurationError(exc);
} catch (const MemoryException& exc) {
    handleMemoryError(exc);
} catch (const IOException& exc) {
    handleIOError(exc);
} catch (const NLMException& exc) {
    handleGenericNLMError(exc);
}
```

**Chained Exception Pattern:**
```cpp
try {
    performOperation();
} catch (const NLMException& exc) {
    logError(exc); // Logs with full context
    // Additional error handling and recovery
}
```

## Integration Notes

### Header Inclusion

All NLM headers now include `NLMErrors.hpp`, enabling exception handling throughout the codebase:

```cpp
// Any NLM header file will now have:
#pragma once
#include "../core/Exceptions/NLMErrors.hpp"
// ... rest of header
```

### Build System Integration

The exception library is integrated into the build system:

1. **Source Files**: `src/core/Exceptions/NLMErrors.cpp` is included in `nlm_core` target
2. **Header Files**: All headers in the `src` directory include `NLMErrors.hpp`
3. **Compilation**: Uses C++20 standard with appropriate compiler flags

### Testing

The exception hierarchy includes comprehensive tests:

```bash
# Compile test program
./workspace/app$ g++ -std=c++20 -I./src -o test_exceptions tests/test_exceptions.cpp src/core/Exceptions/NLMErrors.cpp

# Run tests
./workspace/app$ ./test_exceptions

# Expected output: All tests passed successfully!
```

## Migration Guide

### From std::exception to NLMException

**Old Pattern:**
```cpp
try {
    // Some operation
    throw std::runtime_error("Operation failed");
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

**New Pattern:**
```cpp
try {
    // Some operation
    throw SystemException(
        SystemException::SystemErrorCode::SystemCallFailed,
        "Operation failed: " + std::string(e.what())
    );
} catch (const SystemException& exc) {
    std::cerr << "Error: " << exc.what() << " (Code: " 
              << static_cast<int>(exc.getErrorCode()) << ")" << std::endl;
    if (exc.hasCause()) {
        std::cerr << "Cause: " << exc.getCause()->what() << std::endl;
    }
}
```

### Adding New Exception Types

To add a new exception type:

1. **Define new error code enum** in `NLMErrors.hpp`
2. **Create new exception class** inheriting from `NLMException`
3. **Implement constructors** and type-specific methods
4. **Update `errorCodeToString`** mapping in `NLMErrors.cpp`
5. **Add to `createException` and `throwException`** templates if needed

### Version Compatibility

The exception hierarchy is designed to work with:
- **C++17**: Full functionality with fallback implementations
- **C++20**: Enhanced functionality with `std::source_location`
- **C++23**: Forward compatibility

## Conclusion

The NLM exception hierarchy provides a comprehensive, type-safe, and extensible framework for error handling in the NLM project. It supports:

- **Rich error information**: Error codes, messages, source location, and metadata
- **Exception chaining**: Detailed error context through nested exceptions
- **Serialization**: Conversion to string format for logging and debugging
- **Type safety**: Compile-time type checking and runtime type information
- **Backward compatibility**: Works with C++17 and C++20
- **Comprehensive coverage**: Covers all major error categories in the NLM system

This implementation enables robust error handling throughout the NLM codebase, providing developers with detailed error information for debugging and maintenance.