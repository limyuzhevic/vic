# NLM Error Handling System

This document provides an overview of the comprehensive error handling system implemented for the Neural Language Model (NLM) project.

## Overview

The error handling system provides:

1. **Complete exception hierarchy** with base and specific error types
2. **Error utilities** for validation, formatting, and recovery
3. **Validation framework** for parameters, configuration, state, and resources
4. **Error recovery mechanisms** with retry, degradation, and cleanup strategies
5. **Integration with existing logging system**
6. **User-friendly error reporting** with diagnostic information

## Error Hierarchy

### Base Exception
- `NLMException` - Base class for all NLM-specific exceptions

### Specific Exception Types

#### ValidationError
- `ParameterValidation` - Parameter validation failures
- `StateValidation` - State validation failures
- `ResourceValidation` - Resource validation failures
- `IntegrationValidation` - Integration validation failures

#### MemoryError
- `AllocationFailed` - Memory allocation failures
- `Exhaustion` - Memory exhaustion
- `Corruption` - Memory corruption
- `AccessViolation` - Invalid memory access

#### ConfigurationError
- `FileNotFound` - Configuration file not found
- `InvalidFormat` - Invalid configuration format
- `InvalidValue` - Invalid configuration values
- `MissingKey` - Missing required configuration keys

#### SimulationError
- `NeuralError` - Neural network errors
- `NetworkUnconnected` - Disconnected neural networks
- `InvalidStep` - Invalid simulation steps
- `Timeout` - Simulation timeouts

#### PerformanceError
- `ComputationOverload` - Computation overload
- `BandwidthExceeded` - Bandwidth exceeded
- `CpuLimitExceeded` - CPU limit exceeded
- `LatencyExceeded` - Latency exceeded

#### PluginError
- `NotFound` - Plugin not found
- `VersionMismatch` - Version mismatch
- `LoadFailed` - Plugin load failed
- `Unsafe` - Unsafe plugin behavior

#### IntegrationError
- `DependencyMissing` - Missing dependencies
- `ApiVersionMismatch` - API version mismatch
- `InterfaceError` - Interface errors
- `ProtocolError` - Protocol errors

## Error Types and Codes

The error system uses a comprehensive enumeration of error codes:

```cpp
enum class ErrorCode : uint32_t {
    OK = 0,
    UNKNOWN_ERROR = 1,
    NOT_IMPLEMENTED = 2,
    OUT_OF_MEMORY = 3,
    INVALID_ARGUMENT = 4,
    INVALID_STATE = 5,
    TIMEOUT = 6,
    VALIDATION_ERROR = 100,
    PARAMETER_VALIDATION_FAILED = 101,
    STATE_VALIDATION_FAILED = 102,
    RESOURCE_VALIDATION_FAILED = 103,
    INTEGRATION_VALIDATION_FAILED = 104,
    MEMORY_ALLOCATION_FAILED = 200,
    MEMORY_EXHAUSTION = 201,
    MEMORY_CORRUPTION = 202,
    MEMORY_ACCESS_VIOLATION = 203,
    CONFIGURATION_ERROR = 300,
    CONFIG_FILE_NOT_FOUND = 301,
    CONFIG_INVALID_FORMAT = 302,
    CONFIG_VALUE_INVALID = 303,
    CONFIG_KEY_MISSING = 304,
    SIMULATION_ERROR = 400,
    NEURAL_ERROR = 401,
    NETWORK_UNCONNECTED = 402,
    INVALID_SIMULATION_STEP = 403,
    SIMULATION_TIMEOUT = 404,
    PERFORMANCE_ERROR = 500,
    COMPUTATION_OVERLOADED = 501,
    BANDWIDTH_EXCEEDED = 502,
    CPU_LIMIT_EXCEEDED = 503,
    LATENCY_EXCEEDED = 504,
    PLUGIN_ERROR = 600,
    PLUGIN_NOT_FOUND = 601,
    PLUGIN_VERSION_MISMATCH = 602,
    PLUGIN_LOAD_FAILED = 603,
    PLUGIN_UNSAFE = 604,
    INTEGRATION_ERROR = 700,
    DEPENDENCY_MISSING = 701,
    API_VERSION_MISMATCH = 702,
    INTERFACE_ERROR = 703,
    PROTOCOL_ERROR = 704,
};
```

## Error Categories

```cpp
enum class ErrorCategory : uint8_t {
    None = 0,
    Generic = 1,
    Validation = 2,
    Memory = 3,
    Configuration = 4,
    Simulation = 5,
    Performance = 6,
    Plugin = 7,
    Integration = 8,
    System = 9,
};
```

## Error Severity Levels

```cpp
enum class ErrorSeverity : uint8_t {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};
```

## Error Context Tracking

The system captures comprehensive context information for each error:

- File name and line number
- Function name where error occurred
- Thread ID
- Timestamp
- Custom metadata

Example context capture:

```cpp
struct ErrorContext {
    std::string file;
    int line;
    std::string function;
    std::string thread_id;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::any> data;
};
```

## Error Formatting

Errors can be formatted in human-readable or machine-readable formats:

```cpp
std::string errorMessage = e.formatMessage(); // Machine-readable
std::string errorReport = ErrorFormatter::format(e, true); // Human-readable with stack trace
```

## Validation Framework

### Parameter Validation

Register validation functions for parameter types:

```cpp
nlm::ParameterValidator validator;
validator.registerValidator<int>("int_type", [](int value, const std::string& context) {
    nlm::ValidationResult result;
    if (value < 0) {
        result.passed = false;
        // Add error details
    }
    return result;
});
```

### Configuration Validation

Validate configuration values against templates:

```cpp
nlm::ConfigValidator validator;
nlm::ConfigValidator::ConfigValidationRule rule;
rule.key = "learning_rate";
rule.type = "float";
rule.minValue = "0.0";
rule.maxValue = "1.0";
validator.addRule(rule);

std::map<std::string, std::string> config;
config["learning_rate"] = "0.05";
auto result = validator.validate(config);
```

### State Validation

Validate neural network and simulation states:

```cpp
nlm::StateValidator stateValidator;
stateValidator.registerValidator<NeuralState>("NeuralState", [](const NeuralState& state, const std::string& context) {
    // Validate neural state
});
```

### Resource Validation

Validate system resources:

```cpp
nlm::ResourceValidator resourceValidator;
resourceValidator.addConstraint(nlm::ResourceValidator::ResourceConstraint(
    "cpu", 0.9, 1.0, nlm::ValidationSeverity::Warning, "Optimize performance"));
```

## Error Recovery Mechanisms

### Retry with Exponential Backoff

Execute operations with automatic retry:

```cpp
nlm::RetryConfig retryConfig;
retryConfig.maxRetries = 3;
retryConfig.initialDelay = std::chrono::milliseconds(100);

auto operation = []() { /* operation that might fail */ };
auto result = nlm::ErrorRecovery::executeWithRetry(operation, retryConfig, "Operation");
```

### Graceful Degradation

Execute operations with fallback behaviors:

```cpp
auto mainOperation = []() { /* main operation */ };
auto fallback1 = []() { /* fallback 1 */ };
auto fallback2 = []() { /* fallback 2 */ };

auto result = nlm::ErrorRecovery::executeWithDegradation(
    mainOperation, {fallback1, fallback2}, "Main Operation");
```

### Resource Cleanup

Ensure resources are cleaned up even when errors occur:

```cpp
auto operation = []() { /* operation that might fail */ };
auto cleanup = []() { /* cleanup resources */ };

auto result = nlm::ErrorRecovery::executeWithCleanup(operation, cleanup, "Operation with Cleanup");
```

## Integration with Existing Logging

The error system integrates seamlessly with the existing NLM logging system:

```cpp
// Automatic error logging
nlm::Exception::report(e);

// Exception with custom severity
nlm::Exception::logException(e, nlm::ErrorSeverity::Error);

// Batch error reporting
std::vector<std::unique_ptr<nlm::NLMException>> errors;
errors.push_back(std::make_unique<nlm::ValidationError>("Error 1"));
nlm::ErrorUtilities::reportBatch(errors);
```

## Usage Examples

### Basic Error Usage

```cpp
try {
    // Code that might throw errors
    throw ValidationError("Invalid input", "param1");
} catch (const NLMException& e) {
    nlm::Exception::report(e);
}
```

### Error Recovery with Retry

```cpp
auto riskyOperation = []() {
    // Operation that might temporarily fail
};

nlm::RetryConfig config;
auto result = nlm::ErrorRecovery::executeWithRetry(riskyOperation, config);

if (result.success) {
    // Operation succeeded after retries
} else {
    // Operation failed after all retries
}
```

### Parameter Validation

```cpp
nlm::ParameterValidator validator;
validator.registerValidator<std::string>("string", [](const std::string& value, const std::string& context) {
    nlm::ValidationResult result;
    if (value.empty()) {
        result.passed = false;
        // Add error
    }
    return result;
});

auto validationResults = validator.validateAll(params);
```

### Configuration Validation

```cpp
nlm::ConfigValidator configValidator;
nlm::ConfigValidator::ConfigValidationRule rule;
rule.key = "max_iterations";
rule.type = "int";
rule.minValue = "1";
rule.maxValue = "1000";
rule.required = true;
configValidator.addRule(rule);

std::map<std::string, std::string> config;
auto validationResult = configValidator.validate(config);
```

## Best Practices

1. **Use specific error types** - Catch specific exception types when possible
2. **Include context information** - Always include file, line, and function information
3. **Implement proper error recovery** - Use retry, degradation, and cleanup mechanisms
4. **Validate inputs** - Use the validation framework for parameter validation
5. **Log errors appropriately** - Use the error reporting utilities for consistent logging
6. **Provide user-friendly messages** - Include suggested fixes and diagnostic information
7. **Handle exceptions at appropriate levels** - Don't catch generic exceptions when you can handle specific types
8. **Clean up resources** - Always ensure resources are cleaned up in error handling code

## Building and Running Examples

To build and run the examples:

```bash
mkdir build
cd build
cmake ..
make
./examples/ErrorHandlingExamples
```

Or if using your preferred build system:

```bash
cd examples
g++ -std=c++20 -I../src -I. ErrorHandlingExamples.cpp ../src/core/Error/NLMErrors.cpp ../src/core/ErrorUtils/ErrorUtils.cpp ../src/core/Validation/Validation.cpp ../src/core/Recovery/Recovery.cpp ../src/core/Logger/Logger.cpp -o ErrorHandlingExamples
./ErrorHandlingExamples
```

## File Structure

The error handling system is organized as follows:

```
src/core/
├── Error/
│   ├── NLMErrors.hpp
│   └── NLMErrors.cpp
├── ErrorUtils/
│   ├── ErrorUtils.hpp
│   └── ErrorUtils.cpp
├── Validation/
│   ├── Validation.hpp
│   └── Validation.cpp
├── Recovery/
│   ├── Recovery.hpp
│   └── Recovery.cpp
├── Logger/
│   ├── Logger.hpp
│   └── Logger.cpp
├── Types/
│   ├── Types.hpp
│   └── Types.cpp
├── Config/
│   ├── Config.hpp
│   └── Config.cpp
└── ... (other core modules)

examples/
└── ErrorHandlingExamples.cpp
```

## Integration with Existing Code

To integrate the error handling system with existing NLM code:

1. Include the appropriate headers:
   ```cpp
   #include "NLMErrors.hpp"
   #include "ErrorUtils.hpp"
   #include "Validation.hpp"
   ```

2. Replace `std::runtime_error` with specific NLM exception types
3. Use the validation framework for parameter and configuration validation
4. Implement error recovery where appropriate
5. Use the error reporting utilities for consistent error logging

The system is designed to be backward compatible while providing a comprehensive set of error handling capabilities for the NLM project.