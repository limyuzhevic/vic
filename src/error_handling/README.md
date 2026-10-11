# NLM Error Handling System

## Overview

This document describes the comprehensive error handling system implemented for the NLM (Neural Learning Machine) codebase. The system provides robust error handling, validation, recovery, and reporting capabilities across all components including Brain, Config, Logger, World, and integration systems.

## Key Features

### 1. Standardized Error Types

The error handling system provides a hierarchical exception structure:

- **NLMError** (Base class)
  - ConfigError
  - MemoryError
  - BrainError
  - LoggerError
  - WorldError
  - ValidationError

Each error type includes specific error codes and contextual information for precise error diagnosis.

### 2. Comprehensive Input Validation

- **InputValidator** class with extensive validation capabilities
- Type validation for strings, integers, floats, doubles, booleans
- Range validation, pattern matching, choice validation
- Container validation for vectors and lists
- Neural system specific validation (neuron count, region count, etc.)
- File and path validation
- Network and connection validation
- State and system validation

### 3. Thread-Safe Error Reporting

- **ErrorReporter** interface with multiple implementations
- **ErrorManager** for coordinating reporters, handlers, and monitors
- **ErrorMonitor** for system health monitoring
- **ErrorHandler** for error recovery and mitigation
- Thread-safe operations with mutex protection

### 4. Enhanced Logging System

- **Logger** class with comprehensive error handling
- **ConsoleLogger** with file output and error recovery
- **ErrorAwareLogger** for integration with error reporting
- Support for error context and severity filtering
- Graceful degradation when logging fails

### 5. Error Recovery Mechanisms

- **ErrorRecovery** class for automatic error recovery
- Retry logic with exponential backoff
- Graceful degradation for non-critical operations
- Automatic recovery from I/O errors
- System health monitoring and recovery

### 6. Integration with Existing Components

- **Brain.cpp**: Enhanced with error handling, validation, and recovery
- **Config.cpp**: Comprehensive error handling for file operations and validation
- **Logger.cpp**: Enhanced with error reporting and recovery
- **Phase6IntegratedExperiment.cpp**: Integration verification with error handling
- **SimpleWorld.cpp**: World boundary and collision error handling

### 7. Testing and Validation

- Comprehensive test suite for error handling system
- Unit tests for individual components
- Integration tests for system-wide functionality
- Performance tests for error reporting under load
- Memory usage tests for error handling objects

## Error Handling Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                 Error Reporting System                        │
│                                                             │
│  ┌───────────────────────┐ ┌──────────────────────────────┐ │
│  │     ErrorReporter     │ │      ErrorMonitor          │ │
│  │                     │ │                     │     │ │
│  │  ┌──────────────┐  │ │  ┌──────────────────────┐  │ │ │
│  │  │DefaultReport│  │ │  │   HealthMonitor       │  │ │ │ │
│  │  │              │  │ │                      │  │ │ │ │ │
│  │  └──────────────┘  │ │  └──────────────────────┘  │ │ │ │ │
│  │                   │ │                           │ │ │ │ │
│  └──────────────────────┘                           │ │ │ │ │
│                                                     │ │ │ │ │
│  ┌───────────────────────┐ ┌──────────────────────────────┐ │ │ │
│  │     ErrorHandler      │ │      ErrorContext        │ │ │ │ │
│  │                     │ │                     │     │ │ │ │ │
│  │  ┌──────────────┐  │ │  │  ┌──────────────────┐  │ │ │ │ │
│  │  │DefaultHandler│  │ │  │ ErrorContext     │  │ │ │ │ │ │
│  │  │              │  │ │  │ Manager          │  │ │ │ │ │ │
│  │  └──────────────┘  │ │  └──────────────────┘  │ │ │ │ │ │
│  │                   │ │                       │ │ │ │ │ │
│  └──────────────────────┘                       │ │ │ │ │
│                                                │ │ │ │ │
│  ┌─────────────────────────────────────────────────┐ │ │ │ │
│  │                 ErrorContext Manager          │ │ │ │ │
│  │                                             │ │ │ │ │
│  │  ┌─────────────────┐ ┌─────────────────────┐ │ │ │ │ │
│  │  │   ErrorContext │ │    ThreadSafety   │ │ │ │ │ │
│  │  │   Stack        │ │    Manager        │ │ │ │ │ │ │ │
│  │  └─────────────────┘ └─────────────────────┘ │ │ │ │ │
│  │                                            │ │ │ │ │
│  └─────────────────────────────────────────────────┘ │ │ │ │
│                                                      │ │ │ │
└──────────────────────────────────────────────────────┘ │ │ │
                                                        │ │ │
  ┌───────────────────────────────────────────────────────┐ │ │
  │                 Component Error Handling             │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   Config.Error │ │   Logger.Error       │   │ │ │ │
  │  │   Handling     │ │   Handling           │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   Brain.Error  │ │   World.Error       │   │ │ │ │
  │  │   Handling     │ │   Handling           │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │  Validation.   │ │   Integration.       │   │ │ │ │
  │  │   Error        │ │   Error              │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  └───────────────────────────────────────────────────────┘ │ │
                                                            │ │
  ┌───────────────────────────────────────────────────────┐ │ │
  │                 Application Components                │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │    Brain       │ │    Phase6Integrated │   │ │ │ │
  │  │    System     │ │    Experiment      │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │    Config      │ │    SimpleWorld      │   │ │ │ │
  │  │    System     │ │    System           │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  └───────────────────────────────────────────────────────┘ │ │
                                                            │ │
  ┌───────────────────────────────────────────────────────┐ │ │
  │                 Infrastructure Components            │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   ThreadPool   │ │   ResourceManager   │   │ │ │ │
  │  │   Safety      │ │                     │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   MemoryPool   │ │   CheckpointSystem │   │ │ │ │
  │  │   Management  │ │                     │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  └───────────────────────────────────────────────────────┘ │ │
                                                            │ │
  ┌───────────────────────────────────────────────────────┐ │ │
  │                 Supporting Utilities                  │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   FileUtils    │ │   NetworkUtils      │   │ │ │ │
  │  │   SystemUtils  │ │   MathUtils         │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   DebugUtils   │ │   PerformanceUtils │   │ │ │ │
  │  │   Profiling    │ │                     │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  └───────────────────────────────────────────────────────┘ │ │
                                                            │ │
  ┌───────────────────────────────────────────────────────┐ │ │
  │                 Configuration and Schema              │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   ConfigSchema │ │   ErrorSchema      │   │ │ │ │
  │  │   Management  │ │                     │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  └───────────────────────────────────────────────────────┘ │ │
                                                            │ │
  ┌───────────────────────────────────────────────────────┐ │ │
  │                 Testing and Validation               │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   UnitTests    │ │   IntegrationTests  │   │ │ │ │
  │  │               │ │                     │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   Performance  │ │   StressTests      │   │ │ │ │
  │  │   Tests        │ │                     │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  └───────────────────────────────────────────────────────┘ │ │
                                                            │ │
  ┌───────────────────────────────────────────────────────┐ │ │
  │                 Documentation and Help                │ │ │
  │                                                     │ │ │
  │  ┌─────────────────┐ ┌──────────────────────────┐   │ │ │
  │  │   UserGuide    │ │   APIReference   │   │ │ │ │
  │  │               │ │                     │   │ │ │ │
  │  └─────────────────┘ └──────────────────────────┘   │ │ │
  │                                                     │ │ │
  └───────────────────────────────────────────────────────┘ │ │
                                                            │ │
  └─────────────────────────────────────────────────────────────┘

```

## Usage Examples

### Basic Error Handling

```cpp
#include "Error.hpp"

void exampleFunction() {
    try {
        // Code that might throw errors
        if (someCondition) {
            NLM_THROW_ERROR("Error occurred", "Context: exampleFunction", 1001);
        }
        // ...
    } catch (const NLMError& e) {
        // Handle error
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
```

### Configuration Error Handling

```cpp
#include "Config.hpp"

void loadConfiguration(const std::string& filepath) {
    try {
        auto config = std::make_shared<Config>();
        
        if (!config->loadFromFile(filepath)) {
            throw ConfigError("Failed to load configuration file", "loadConfiguration");
        }
        
        // Use validated configuration values
        auto neuronCount = config->get<size_t>("neuron_count");
        auto regionCount = config->get<size_t>("region_count");
        
        // Apply validation
        InputValidator::validateNeuronCount("neuron_count", *neuronCount);
        InputValidator::validateRegionCount("region_count", *regionCount);
        
    } catch (const ConfigError& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        // Attempt recovery
        config->initializeDefaults({
            {"neuron_count", 1000},
            {"region_count", 4}
        });
    }
}
```

### Logger Error Handling

```cpp
#include "Logger.hpp"

void logWithErrorHandling(const std::string& message) {
    try {
        Logger::getGlobal().info(message, __FILE__, __LINE__, __func__);
        
        // Check if logging was successful
        if (!Logger::getGlobal().isEnabled(LogLevel::Info)) {
            throw LoggerError("Logging not enabled", "logWithErrorHandling");
        }
        
    } catch (const LoggerError& e) {
        std::cerr << "Logging error: " << e.what() << std::endl;
        // Try to recover
        Logger::getGlobal().attemptRecovery("logWithErrorHandling", 3.0f);
    }
}
```

### Input Validation

```cpp
#include "InputValidator.hpp"

bool validateInput(const std::string& input) {
    // Validate string input
    auto result = InputValidator::validateString("input", input, "positive_number");
    
    if (!result.valid) {
        ErrorReporting::report(ErrorCategory::Validation, "VALIDATION_FAILED",
                            "Input validation failed: " + result.errorMessage,
                            "validateInput");
        return false;
    }
    
    return true;
}
```

### Error Context Management

```cpp
#include "Error.hpp"

void complexOperation() {
    // Set up error context
    ErrorContext::getInstance().pushContext("DatabaseConnection");
    ErrorContext::getInstance().pushContext("QueryExecution");
    
    try {
        // Perform operation
        executeDatabaseQuery("SELECT * FROM table");
        
    } catch (const NLMError& e) {
        // Error includes context
        std::cerr << "Error: " << e.what() << std::endl;
        
    } finally {
        // Clean up context
        ErrorContext::getInstance().popContext();
        ErrorContext::getInstance().popContext();
    }
}
```

## Error Reporting

The error reporting system provides:

- **Error Levels**: Debug, Info, Warning, Error, Critical, Fatal
- **Error Categories**: Configuration, Memory, Brain, Logger, World, Validation, etc.
- **Error Statistics**: Counts, trends, performance metrics
- **Health Monitoring**: System health checks and alerts
- **Error Recovery**: Automatic recovery and retry mechanisms

## Testing

Run the test suite with:

```bash
cd /path/to/nlm/source
./test/TestErrorHandling
```

The test suite includes:

- Basic error handling tests
- Logger error handling tests
- Config error handling tests
- Brain error handling tests
- Validation error handling tests
- Integration tests
- Performance tests
- Memory usage tests
- Stress tests

## Configuration

The error handling system can be configured using the `configure()` method:

```cpp
std::map<std::string, std::string> config = {
    {"report_level", "ALL"},
    {"filter_critical", "true"},
    {"output_file", "./logs/nlm_errors.log"},
    {"max_retries", "5"},
    {"retry_delay", "0.1"},
    {"strict_mode", "true"}
};

ErrorReporting::configure(config);
```

## Performance Considerations

The error handling system is designed for performance:

- **Minimal overhead**: Error reporting is conditional and can be disabled
- **Efficient error handling**: Exceptions are lightweight and catch-specific
- **Thread-safe operations**: Proper mutex usage to avoid contention
- **Configurable filtering**: Only high-priority errors are reported
- **Memory management**: Proper cleanup of error handling resources

## Best Practices

1. **Always validate inputs** before using them
2. **Use try-catch blocks** around operations that can fail
3. **Provide meaningful error messages** with context
4. **Implement error recovery** for non-critical failures
5. **Use error context** to track the source of errors
6. **Test error handling** thoroughly
7. **Monitor error rates** and system health
8. **Log errors** for debugging and analysis
9. **Implement graceful degradation** for partial failures
10. **Document error conditions** and recovery procedures

## Integration with Existing Code

The error handling system is designed to integrate seamlessly with existing code:

- **Backward compatible**: Existing code continues to work without changes
- **Optional**: Error handling features can be enabled/disabled as needed
- **Configurable**: Error handling behavior can be customized
- **Extensible**: New error types and handlers can be added easily

## Conclusion

The NLM error handling system provides comprehensive error handling, validation, recovery, and reporting capabilities. It enables robust error handling across all components while maintaining performance and ease of use. The system is designed to help developers write more reliable and resilient code by providing proper error handling infrastructure.

Key benefits:

- **Improved reliability**: Better error handling prevents crashes and data corruption
- **Enhanced debugging**: Detailed error messages and context aid in debugging
- **Better user experience**: Graceful error recovery and informative messages
- **Easier maintenance**: Centralized error handling and reporting
- **Performance**: Efficient error handling with minimal overhead
- **Scalability**: Extensible error handling system that can grow with the application

The error handling system is production-ready and can be used immediately in any NLM project.
