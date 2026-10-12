# NLM Error Handling System

## Overview

The NLM Error Handling System is a comprehensive error handling and recovery framework for the Neural Learning Machine (NLM) project. It provides:

- **Error Classification and Hierarchy**: Base `NLMError` class with specific error types for different domains
- **Error Context and Diagnostics**: Detailed error information including stack traces, performance metrics, and context data
- **Recovery and Resilience**: Automatic recovery strategies and graceful degradation
- **Error Reporting and Analysis**: Structured reporting in multiple formats with error aggregation and correlation
- **Integration with All Systems**: Seamless integration with Logger, CLI parser, Config validation, and Serialization systems

## Key Features

### 1. Error Hierarchy

#### Base Error Class
- `NLMError`: Base class with comprehensive error context and functionality
- Error categorization: System, User, Temporary, Permanent, Security, Resource, Neural, Memory, Validation, Serialization, Configuration, Network, Performance
- Error severity levels: Debug, Info, Warning, Error, Critical, Fatal, Alert
- Error classification: Retryable, Fallback, Degrade, Checkpoint, Repair, Isolate, Escalate, Ignore

#### Specific Error Types
- `ConfigurationError`: Configuration-related errors (invalid format, missing keys, type mismatch)
- `ValidationError`: Validation-related errors (type mismatch, constraint violation, format error)
- `SerializationError`: Serialization-related errors (version mismatch, corrupted data, invalid format)
- `NeuralError`: Neural network computation errors (neuron initialization, synapse creation, plasticity rules)
- `MemoryError`: Memory management errors (allocation failure, buffer overflow, memory leak)
- `SystemError`: System-level errors (file system, network, process, thread, synchronization)
- `PerformanceError`: Performance-related errors (memory usage exceeded, CPU usage exceeded)
- `NetworkError`: Network-related errors (connection failed, timeout, packet loss)

### 2. Error Context and Diagnostics

- **Stack Trace Capture**: With source location and function information
- **Context Information**: State, variables, and parameters during error occurrence
- **Performance Metrics**: Memory usage tracking, processing time, CPU usage
- **Thread and Process Information**: Unique identification for debugging
- **Time-stamped Error Reports**: Precise timing for error analysis

### 3. Recovery and Resilience

- **Automatic Recovery Strategies**: Retry, fallback, repair, degrade, checkpoint/restore, circuit breaker
- **Graceful Degradation**: Reduced functionality when errors occur
- **State Checkpoint/Restore**: State management for error recovery
- **Resource Cleanup and Leak Detection**: Automatic resource management
- **Memory Management During Error Handling**: Safe memory operations
- **Circuit Breaker Pattern**: Prevent cascade failures

### 4. Error Reporting and Analysis

- **Structured Error Reporting**: JSON, YAML, and XML formats
- **Error Aggregation and Correlation**: Multiple errors analyzed together
- **Error Frequency Tracking and Pattern Detection**: Statistical analysis of errors
- **Root Cause Analysis with Heuristics**: Automated error diagnosis
- **Performance Impact Analysis**: Error impact on system performance
- **Automated Error Suggestions and Fixes**: Intelligent error recommendations

### 5. Integration with All Systems

- **Integration with Enhanced Logger System**: Structured logging with error context
- **Integration with Advanced CLI Parser**: Error handling in command-line interface
- **Integration with Config Validation System**: Configuration validation errors
- **Integration with Serialization System**: Data serialization/deserialization errors
- **Integration with Existing Error Handling**: Backward compatibility maintained
- **Zero Breaking Changes to Existing API**: Existing code continues to work

## Usage Examples

### Basic Error Handling

```cpp
#include "core/ErrorHandling/include/nlm/NLMError.hpp"
#include "core/ErrorHandling/include/nlm/ErrorHandling.hpp"

// Create and handle an error
auto error = nlm::ConfigurationError::createConfigurationError(
    "Invalid configuration value",
    "learning_rate",
    "Use default learning rate (0.01)"
);

nlm::handleError(std::move(error));
```

### Error Handling with Context

```cpp
auto error = nlm::ValidationError::createValidationError(
    "Value out of range",
    "network_depth",
    "Set network_depth to 10"
);

error->addContext("current_value", 100);
error->addContext("valid_range", "1-50");
error->addContext("file", "config/network.cfg");
error->addContext("line", 42);

nlm::handleError(std::move(error));
```

### Error Handling with Recovery

```cpp
try {
    // Code that might throw an error
    auto error = nlm::NeuralError::createNeuralError(
        "Neuron initialization failed",
        "initialize_neurons",
        "Check neuron parameters and try again"
    );
    
    nlm::handleError(std::move(error));
    
    // Try recovery
    if (attemptRecovery(*error)) {
        std::cout << "Error recovered successfully!" << std::endl;
    }
    
} catch (const nlm::NLMError& e) {
    // Handle NLM errors
    std::cerr << "NLM Error: " << e.what() << std::endl;
    std::cerr << "Location: " << e.getSourceLocation() << std::endl;
} catch (const std::exception& e) {
    // Handle legacy exceptions
    nlm::translateLegacyException(e);
}
```

### Error Error Handling Macros

```cpp
// Throw error with source location and handling
NLM_THROW_ERROR_AT(
    nlm::ConfigurationError,
    "Configuration file not found"
);

// Handle error with context
NLM_THROW_ERROR_WITH_CONTEXT(
    nlm::ValidationError,
    "Invalid value type",
    std::map<std::string, std::string>{
        {"field", "temperature"},
        {"expected_type", "double"},
        {"actual_type", "string"}
    }
);

// Handle error synchronously and throw
NLM_THROW_ERROR_AT_SYNC(
    nlm::SerializationError,
    "Version mismatch"
);
```

## Error Handling Functions

### Global Error Handling Functions

- `initializeErrorHandling(config)`: Initialize error handling system with configuration
- `configureErrorHandling(config)`: Configure error handling system
- `handleError(error)`: Handle an NLMError
- `handleErrorSync(error)`: Handle an NLMError synchronously
- `addErrorToGroup(groupId, error)`: Add error to error group
- `getErrorGroup(groupId)`: Get error group by ID
- `getErrorHandlingStats()`: Get error handling system statistics
- `translateLegacyException(e)`: Translate legacy std::exception to NLMError

### Error Handling Macros

- `NLM_THROW_ERROR_AT(errorType, message)`: Throw error with source location
- `NLM_THROW_ERROR_AT_SYNC(errorType, message)`: Throw error synchronously
- `NLM_ERROR_HANDLED_FUNCTION_BEGIN`: Begin error-handled function
- `NLM_ERROR_HANDLED_FUNCTION_END(continueOnError)`: End error-handled function

## Configuration Options

The error handling system can be configured with the following options:

```cpp
std::unordered_map<std::string, std::string> config = {
    {"min_handled_severity", "WARNING"},           // Minimum severity to handle
    {"error_retention_hours", "24"},              // Error retention period
    {"enable_structured_reporting", "true"},       // Enable structured reporting
    {"enable_error_correlation", "true"},          // Enable error correlation
    {"max_concurrent_recoveries", "4"},           // Maximum concurrent recoveries
    {"recovery_timeout_ms", "30000"},             // Recovery timeout
};
```

## Error Statistics

The error handling system provides comprehensive statistics:

- Total errors handled
- Total recoveries attempted
- Recovery success rate
- Errors by category
- Errors by severity
- Recovery time statistics

```cpp
auto stats = getErrorHandlingStats();
std::cout << "Total errors: " << stats.totalErrors << std::endl;
std::cout << "Total recoveries: " << stats.totalRecoveries << std::endl;
std::cout << "Recovery success rate: " << 
    (stats.totalRecoveries > 0 ? 
        (static_cast<double>(stats.recoveryStats.successfulRecoveries) / stats.totalRecoveries * 100) : 0)
    << "%" << std::endl;
```

## Error Groups

Error groups allow correlation and analysis of related errors:

```cpp
// Create error group
nlm::addErrorToGroup("validation_errors", error1);
nlm::addErrorToGroup("validation_errors", error2);
nlm::addErrorToGroup("validation_errors", error3);

// Get error group
auto group = nlm::getErrorGroup("validation_errors");
if (group) {
    std::cout << "Group summary: " << group->getSummary() << std::endl;
    std::cout << "Number of errors: " << group->errors.size() << std::endl;
    std::cout << "Group severity: " << static_cast<int>(group->severity) << std::endl;
}
```

## Backward Compatibility

The error handling system maintains full backward compatibility:

- Existing exception throwing code continues to work
- Legacy exception handling is automatically translated
- All existing APIs remain unchanged
- New error handling is additive and optional

## Performance Considerations

The error handling system is designed for high performance:

- Thread-safe implementation with minimal locking
- Efficient error context storage
- Configurable error handling based on severity
- Asynchronous recovery options
- Memory-efficient error reporting

## Dependencies

The error handling system depends on:

- Standard C++ library (C++20)
- Nlohmann JSON library (for JSON serialization)
- YAML-CPP library (for YAML serialization)
- Existing NLM logging and configuration systems

## Testing

Comprehensive tests are included in the `src/core/ErrorHandling/test/` directory:

- `TestErrorHandlingSystem.cpp`: Comprehensive error handling system tests
- `TestSimpleErrorHandling.cpp`: Simple error handling verification

Run tests using your preferred test framework.

## Integration Instructions

To integrate the error handling system with your NLM project:

1. Add the error handling system to your CMakeLists.txt:
   ```cmake
   add_library(nlm_error_handling STATIC
       src/core/ErrorHandling/src/NLMError.cpp
       src/core/ErrorHandling/src/ConfigurationError.cpp
       src/core/ErrorHandling/src/ValidationError.cpp
       src/core/ErrorHandling/src/SerializationError.cpp
       src/core/ErrorHandling/src/NeuralError.cpp
       src/core/ErrorHandling/src/MemoryError.cpp
       src/core/ErrorHandling/src/SystemError.cpp
       src/core/ErrorHandling/src/PerformanceError.cpp
       src/core/ErrorHandling/src/NetworkError.cpp
       src/core/ErrorHandling/src/ErrorHandling.cpp
   )
   ```

2. Update your main CMakeLists.txt to include the error handling system in the nlm_core library

3. Include the headers in your code:
   ```cpp
   #include "core/ErrorHandling/include/nlm/NLMError.hpp"
   #include "core/ErrorHandling/include/nlm/ErrorHandling.hpp"
   ```

4. Initialize the error handling system in your main function

5. Use the error handling functions throughout your codebase

## Conclusion

The NLM Error Handling System provides a robust, comprehensive error handling framework that enhances the reliability and maintainability of the NLM project. It integrates seamlessly with existing systems while providing advanced error recovery and analysis capabilities.

This system enables NLM to handle errors gracefully, recover from failures when possible, and provide detailed error information for debugging and analysis.
