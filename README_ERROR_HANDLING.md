# NLM Error Handling System

## Overview

This is a comprehensive error handling system for the NLM (Neural Learning Machine) project. The system provides robust, type-safe error handling with configurable strategies, recovery mechanisms, and detailed reporting.

## Features

### 1. Error Classification
The system defines comprehensive error categories:

- **Configuration Errors** (1000-1003):
  - Invalid values in configuration
  - Missing configuration files
  - JSON parsing errors
  - Configuration validation failures

- **Neural System Errors** (2000-2003):
  - Neuron not found
  - Invalid synapses
  - Simulation errors
  - Brain initialization failures

- **IO Errors** (3000-3003):
  - File read/write errors
  - Network errors
  - File not found

- **Resource Errors** (4000-4003):
  - Out of memory
  - Invalid parameters
  - Timeouts
  - Resource quotas exceeded

- **System Errors** (5000-5003):
  - Initialization failures
  - Not implemented features
  - Internal system errors
  - System unavailable

- **Additional Categories** (6000-6003):
  - Parameter validation errors
  - Memory allocation errors
  - Threading and concurrency errors
  - Random number generation errors

### 2. Error Hierarchy

The system uses a type-safe error hierarchy:

```
NLMError (base)
├── ConfigurationError
│   ├── InvalidConfigValueError
│   ├── MissingConfigFileError
│   ├── JSONParsingError
│   └── ValidationError
├── NeuralSystemError
│   ├── NeuronNotFoundError
│   ├── InvalidSynapseError
│   └── BrainInitializationError
├── IOError
│   ├── FileReadError
│   ├── FileWriteError
│   └── NetworkError
├── ResourceError
│   ├── OutOfMemoryError
│   ├── TimeoutError
│   └── QuotaExceededError
├── SystemError
│   ├── InitializationError
│   ├── NotImplementedError
│   └── InternalError
├── ValidationError (additional)
├── MemoryError (additional)
├── ThreadingError (additional)
└── RandomError (additional)
```

### 3. Error Handling Strategies

The system supports 8 configurable error handling strategies:

- **THROW**: Re-throw the error for caller to handle
- **LOG_AND_THROW**: Log the error then re-throw
- **RECOVER**: Attempt automatic recovery
- **RETRY**: Retry the operation
- **FALLBACK**: Use fallback values/strategies
- **IGNORE**: Ignore the error (use defaults)
- **SILENT**: Only log internally, don't propagate

### 4. Recovery Mechanisms

The system includes recovery handlers for common error types:

- **MemoryAllocationRecoveryHandler**: Handles out-of-memory errors
- **ConfigValidationRecoveryHandler**: Handles configuration validation errors
- **SimulationRecoveryHandler**: Handles simulation errors

### 5. Error Reporting

The system provides comprehensive error reporting:

- **Human-readable error messages**
- **Structured error information for logging**
- **Error context tracking** (component, operation, file, line, timestamp)
- **Error severity levels** (INFO, WARNING, ERROR, CRITICAL, FATAL)

### 6. Integration

The error handling system is designed to be integrated throughout the NLM codebase:

- Wrap existing error-prone operations with try-catch blocks
- Convert existing error codes/exceptions to NLMError
- Add error handling to critical paths (brain initialization, file operations, etc.)
- Provide consistent error handling across all modules

## Usage Examples

### Basic Error Handling

```cpp
#include "src/core/Error/NLMError.hpp"
#include "src/core/Error/ErrorHandler.hpp"

// Create and throw an error
try {
    throw InvalidConfigValueError(
        "max_neurons",
        "positive integer between 1 and 100",
        "-5"
    );
} catch (const InvalidConfigValueError& e) {
    std::cout << "Error: " << e.what() << std::endl;
    std::cout << "Category: " << e.getCategoryString() << std::endl;
}
```

### Error Handling with Recovery

```cpp
// Configure error handler to use recovery strategy
ErrorHandler::getInstance().setStrategy(
    ErrorCategory::RESOURCE_OUT_OF_MEMORY,
    ErrorStrategy::RECOVER
);

// Add recovery handlers
ErrorHandler::getInstance().addRecoveryHandler(
    ErrorCategory::RESOURCE_OUT_OF_MEMORY,
    std::make_shared<MemoryAllocationRecoveryHandler>()
);
```

### Error Reporting

```cpp
// Add error reporters
ErrorHandler::getInstance().addReporter(
    std::make_shared<ConsoleErrorReporter>(ErrorSeverity::INFO)
);

ErrorHandler::getInstance().addReporter(
    std::make_shared<FileErrorReporter>("/tmp/nlm_errors.log")
);
```

### Error Context

```cpp
// Create error with context
ErrorContext context("Config", "load_config", "config.cpp", 42);

try {
    throw InvalidConfigValueError(
        "max_neurons",
        "positive integer",
        "-5",
        context
    );
} catch (const NLMError& e) {
    std::cout << "Component: " << e.getContext().component << std::endl;
    std::cout << "Operation: " << e.getContext().operation << std::endl;
    std::cout << "File: " << e.getContext().file << std::endl;
    std::cout << "Line: " << e.getContext().line << std::endl;
}
```

## Integration Throughout NLM

### 1. Config Module

All configuration operations should be wrapped with error handling:

```cpp
bool loadConfig(const std::string& filename) {
    try {
        auto config = nlm::Config::create();
        config->loadFromFile(filename);
        return true;
    } catch (const JSONParsingError& e) {
        // Handle JSON parsing errors
        ErrorHandler::getInstance().handle(
            std::make_unique<NLMError>(e)
        );
        return false;
    } catch (const MissingConfigFileError& e) {
        // Handle missing file errors
        ErrorHandler::getInstance().handle(
            std::make_unique<NLMError>(e)
        );
        return false;
    }
}
```

### 2. Random Module

```cpp
double generateRandomValue(double min, double max) {
    try {
        auto rng = nlm::RandomGenerator::create(42);
        return rng->uniformDouble(min, max);
    } catch (const RandomError& e) {
        ErrorHandler::getInstance().handle(
            std::make_unique<NLMError>(e)
        );
        // Fallback value
        return (min + max) / 2.0;
    }
}
```

### 3. Simulation Module

```cpp
bool runSimulation(const SimulationParams& params) {
    try {
        auto brain = nlm::Brain::create(params);
        brain->initialize();
        return true;
    } catch (const BrainInitializationError& e) {
        ErrorHandler::getInstance().handle(
            std::make_unique<NLMError>(e)
        );
        // Recovery strategy may attempt to fix initialization
        return false;
    }
}
```

## Files

### Core Error Handling Files

- `src/core/Error/ErrorCategory.hpp` - Error categories and severity levels
- `src/core/Error/NLMError.hpp` - Base error class and all error subclasses
- `src/core/Error/ErrorHandler.hpp` - Error handler with strategy management
- `src/core/Error/ErrorIntegration.cpp` - Integration examples

### Supporting Files

- `demo_error_handling.cpp` - Demonstration program
- `tests/test_error_handling_integration.cpp` - Test suite
- `CMakeLists.txt` - CMake configuration

## Building and Testing

### Build with CMake

```bash
mkdir build
cd build
cmake ..
make
```

### Run Tests

```bash
./test_error_handling
```

### Run Demonstration

```bash
./demo_error_handling
```

## Design Principles

### 1. Type Safety

All errors inherit from appropriate base classes, allowing for specific error handling while maintaining type safety.

### 2. Configurability

Error handling strategies can be configured per error category, allowing for different handling approaches based on error type.

### 3. Recoverability

The system provides recovery mechanisms for common error types, allowing for automatic error recovery where possible.

### 4. Reportability

The system provides comprehensive error reporting with human-readable and structured information for debugging.

### 5. Integration

The system is designed to be easily integrated throughout the NLM codebase, providing consistent error handling across all modules.

## Benefits

1. **Consistent Error Handling**: All errors in the NLM system use the same error handling framework
2. **Type Safety**: Specific error types allow for precise error handling
3. **Configurability**: Different error strategies for different error categories
4. **Recovery**: Automatic recovery for common error types
5. **Reporting**: Detailed error information for debugging
6. **Integration**: Easy to integrate throughout the NLM codebase

## Future Enhancements

1. **Custom Recovery Handlers**: Allow users to register custom recovery handlers
2. **Error Aggregation**: Aggregate multiple related errors
3. **Error Persistence**: Store errors in a persistent log
4. **Error Analytics**: Analyze error patterns for system improvement
5. **Async Error Handling**: Support for asynchronous error handling

## Conclusion

The NLM error handling system provides a comprehensive, type-safe, and configurable error handling framework for the NLM project. It includes error classification, hierarchy, handling strategies, recovery mechanisms, and reporting. The system is designed to be easily integrated throughout the NLM codebase, providing consistent error handling across all modules.