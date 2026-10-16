# NLM Error Handling and Recovery System - Usage Guide

## Overview

This document provides comprehensive information about the NLM Error Handling and Recovery System, including installation, configuration, usage examples, and integration guidelines.

## Introduction

The NLM Error Handling and Recovery System provides robust exception handling, error recovery mechanisms, and integration support for the NLM (Neural Language Model) framework. The system addresses the following requirements:

- Complete exception hierarchy for all NLM components
- Error reporting infrastructure with configurable levels
- Recovery mechanisms including retry logic, graceful degradation, and checkpointing
- Integration with existing NLM APIs
- Backward compatibility with existing error handling patterns
- Configuration for error handling behavior

## Installation

### Prerequisites

- C++20 compiler
- CMake 3.16 or higher
- Standard NLM development environment

### Building the Error Handling System

The error handling system is integrated into the main NLM build system and requires no separate installation.

#### Standard Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

#### Building with Error Handling Tests

```bash
mkdir build_tests
cd build_tests
cmake -DTESTING=ON ..
cmake --build .
./nlm_test  # Run NLM tests
```

## Configuration

### Global Error Handling Configuration

The error handling system can be configured globally using the `nlm::initializeErrorHandling()` function:

```cpp
#include "core/ErrorHandling/ErrorHandling.hpp"

// Initialize with warning level and catch strategy
nlm::initializeErrorHandling(nlm::ErrorReportLevel::Warning, 
                              nlm::ExceptionStrategy::Catch);

// Initialize with critical level and throw strategy
nlm::initializeErrorHandling(nlm::ErrorReportLevel::Critical, 
                              nlm::ExceptionStrategy::Throw);
```

### Error Reporting Levels

- `ErrorReportLevel::Silent` - No error reporting
- `ErrorReportLevel::Warning` - Report warnings only
- `ErrorReportLevel::Error` - Report errors and warnings
- `ErrorReportLevel::Critical` - Report all errors, warnings, and critical issues

### Exception Handling Strategies

- `ExceptionStrategy::Throw` - Throw exceptions for errors (default)
- `ExceptionStrategy::Catch` - Catch and handle internally
- `ExceptionStrategy::Ignore` - Ignore errors, continue operation

## Usage Examples

### Basic Error Handling

```cpp
#include "core/ErrorHandling/ErrorHandling.hpp"

void riskyOperation() {
    try {
        // Critical operation that may throw
        performRiskyAction();
    } catch (const nlm::NLMException& e) {
        nlm::logError(e.getMessage(), e.getContext());
        // Handle error or rethrow
    }
}
```

### Error Handling with Recovery

```cpp
#include "core/ErrorHandling/ErrorHandling.hpp"

void safeOperation() {
    nlm::GlobalErrorRecovery& recovery = nlm::getGlobalErrorRecovery();
    
    auto result = recovery.executeSafely("ComplexOperation", []() {
        return performComplexOperation();
    });
    
    if (result.isDegradedValue()) {
        nlm::logWarning("Operation completed in degraded mode: " + 
                       result.getDegradationReason());
    }
}
```

### Configuration Error Handling

```cpp
#include "core/ErrorHandling/ErrorHandling.hpp"

int main() {
    nlm::initializeErrorHandling(nlm::ErrorReportLevel::Warning);
    
    nlm::GlobalErrorRecovery& recovery = nlm::getGlobalErrorRecovery();
    
    auto config = recovery.getConfigCheckpoint().load("last");
    if (!config) {
        config = recovery.getRetryManager().executeWithRetry("LoadConfig", []() {
            return nlm::ErrorHandling::ConfigErrorHandler::loadFromFile("config.cfg");
        });
    }
    
    if (config) {
        // Use configuration
    } else {
        // Handle missing configuration
    }
    
    return 0;
}
```

### Application Integration

```cpp
class NLMApplication {
private:
    std::unique_ptr<nlm::Config> config;
    std::unique_ptr<nlm::ErrorHandling::GlobalErrorRecovery> recovery;
    
public:
    NLMApplication() {
        // Configure error handling
        nlm::ErrorHandlingStrategy::Configuration errorConfig;
        errorConfig.mode = nlm::ErrorHandlingStrategy::Mode::Resilient;
        errorConfig.reportLevel = nlm::ErrorReportLevel::Warning;
        errorConfig.strategy = nlm::ExceptionStrategy::Catch;
        
        recovery = std::make_unique<nlm::ErrorHandling::GlobalErrorRecovery>(errorConfig);
        
        // Load configuration with error handling
        auto loadedConfig = nlm::ErrorHandling::ConfigErrorHandler::safeLoadConfig(
            "config.cfg", recovery->getConfigCheckpoint(), recovery->getRetryManager(), true
        );
        
        if (loadedConfig) {
            config = std::move(loadedConfig);
        } else {
            nlm::logError("Failed to load configuration");
        }
    }
    
    bool runStep(nlm::SimulationStep step) {
        nlm::ErrorHandling::ErrorContext context("app_step");
        context.addStep(std::string("Running step ") + std::to_string(step));
        
        try {
            // Run simulation with error handling
            return simulationSystem->runStep(step);
        } catch (const nlm::NLMException& e) {
            nlm::logError("Step failed: " + e.getMessage(), "app_step");
            
            // Attempt recovery
            if (attemptRecovery()) {
                nlm::logWarning("Recovery attempted");
                return true;
            }
            
            return false;
        }
    }
};
```

## Integration with Existing Components

### Configuration Component

The Configuration component integrates error handling as follows:

```cpp
nlm::Config config;

// Enable error handling
config.setErrorHandlingEnabled(true);
config.setErrorStrategy(nlm::ExceptionStrategy::Catch);

// Load configuration with error handling
bool success = config.loadFromFile("config.cfg");
if (!success) {
    nlm::logError("Failed to load configuration");
}

// Validate configuration with error handling
auto result = config.validate();
if (!result.isValid) {
    for (const auto& error : result.errors) {
        nlm::logError(error->what(), "ConfigValidation");
    }
}
```

### Brain Component

The Brain component integrates error handling for simulation steps:

```cpp
nlm::Brain brain(config);

bool success = brain.initialize();
if (!success) {
    nlm::logError("Brain initialization failed");
    return 1;
}

for (nlm::SimulationStep step = 0; step < totalSteps; ++step) {
    bool stepSuccess = brain.runStep(step);
    if (!stepSuccess) {
        nlm::logWarning("Step " + std::to_string(step) + " completed with warnings");
    }
}
```

### Neuron Component

The Neuron component integrates error handling for spike generation:

```cpp
nlm::Neuron neuron(neuronId);

bool firing = neuron.stepLIF(currentTime, dt);
if (firing) {
    // Handle spike
} else {
    // Handle sub-threshold operation
}
```

## Error Recovery Mechanisms

### Retry Logic

The system provides retry logic for transient errors:

```cpp
nlm::ErrorHandling::TransientErrorRecovery::RetryConfig retryConfig;
retryConfig.maxAttempts = 3;
retryConfig.initialDelay = std::chrono::milliseconds(100);
retryConfig.backoffMultiplier = 2.0;

nlm::ErrorHandling::TransientErrorRecovery retryManager(retryConfig);

int result = retryManager.executeWithRetry("Operation", []() {
    return performOperation();
});
```

### Checkpoint Recovery

The system supports checkpoint-based recovery:

```cpp
nlm::ErrorHandling::CheckpointManager checkpoint("checkpoints");

// Save state
checkpoint.save("step_1000", simulationState);

// Load state
auto loadedState = checkpoint.load("step_1000");
if (loadedState) {
    // Restore state
}
```

### Graceful Degradation

The system provides graceful degradation for non-critical failures:

```cpp
auto degradedConfig = nlm::ErrorHandling::GracefulDegradation::degradedConfig("Config unavailable");

if (degradedConfig.isDegradedValue()) {
    nlm::logWarning("Using degraded configuration: " + degradedConfig.getDegradationReason());
    // Use minimal configuration
} else {
    // Use full configuration
}
```

## Migration Guide

### From Early Returns to Exceptions

If your code currently uses early returns for error handling, you can gradually migrate to exceptions:

```cpp
// Old pattern
bool result = config.loadFromFile("config.cfg");
if (!result) {
    std::cerr << "Failed to load configuration" << std::endl;
    return 1;
}

// New pattern with error handling enabled
bool result;
try {
    result = config.loadFromFile("config.cfg");
} catch (const nlm::ConfigException& e) {
    std::cerr << "Failed to load configuration: " << e.what() << std::endl;
    return 1;
}
```

### Gradual Migration Steps

1. **Enable error handling** in components
2. **Configure error strategy** (Throw, Catch, or Ignore)
3. **Add recovery actions** for common errors
4. **Implement graceful degradation** for non-critical components
5. **Test thoroughly** with existing test suite

## Testing

The error handling system includes comprehensive tests:

### Unit Tests

Run the NLM test suite:

```bash
cd build
./nlm_test
```

### Integration Tests

The system includes integration tests that verify:

- Exception hierarchy and error codes
- Error reporting and statistics
- Recovery mechanisms (retry, checkpoints, graceful degradation)
- Component integration (Config, Brain, Neuron, Synapse)
- Global error recovery system

### Custom Tests

You can create custom tests for your error handling integration:

```cpp
// test_custom_error_handling.cpp
#include "core/ErrorHandling/ErrorHandling.hpp"

TEST(CustomErrorHandling, ConfigurationLoad) {
    nlm::GlobalErrorRecovery& recovery = nlm::getGlobalErrorRecovery();
    
    auto config = recovery.getConfigCheckpoint().load("last");
    ASSERT_TRUE(config != nullptr);
}

TEST(CustomErrorHandling, RecoveryActions) {
    nlm::ErrorHandler::getInstance().registerRecoveryAction(
        "test_error",
        []() { return true; }
    );
    
    bool result = nlm::ErrorHandler::getInstance().executeRecovery("test_error");
    ASSERT_TRUE(result);
}
```

## Troubleshooting

### Common Issues

#### 1. Error Handling Not Enabled

**Problem:** Error handling is not active in components.

**Solution:** Ensure error handling is initialized and enabled:

```cpp
nlm::initializeErrorHandling(nlm::ErrorReportLevel::Warning);
// OR
config.setErrorHandlingEnabled(true);
```

#### 2. No Exception Reporting

**Problem:** Exceptions are not being reported or logged.

**Solution:** Check error handler configuration:

```cpp
nlm::ErrorHandler::initialize(nlm::ErrorReportLevel::Error);
// Add console reporter
nlm::ErrorHandler::getInstance().addReporter(
    std::make_shared<nlm::ConsoleErrorReporter>(
        nlm::ErrorReportLevel::Error, true
    )
);
```

#### 3. Recovery Not Working

**Problem:** Recovery actions are not being executed.

**Solution:** Verify recovery configuration and action registration:

```cpp
nlm::ErrorHandler::getInstance().registerRecoveryAction(
    "specific_error",
    []() {
        // Recovery logic
        return true;
    }
);

// Verify registration
bool result = nlm::ErrorHandler::getInstance().executeRecovery("specific_error");
```

### Debugging

#### Error Context

Use error context to debug issues:

```cpp
nlm::ErrorHandling::ErrorContext context("DebugOperation");
context.addStep("Step 1");
context.addStep("Step 2");
context.addError("Debug error");

std::cout << "Context: " << context.getContext() << std::endl;
```

#### Error Statistics

Monitor error statistics:

```cpp
const auto& stats = nlm::ErrorHandler::getInstance().getStatistics();
std::cout << "Total errors: " << stats.totalErrors << std::endl;
std::cout << "Errors handled: " << stats.errorsHandled << std::endl;
std::cout << "Last error: " << stats.lastErrorMessage << std::endl;
```

## Best Practices

### 1. Use Appropriate Error Levels

- Use `Warning` for non-critical issues
- Use `Error` for critical failures
- Use `Critical` for system-level failures

### 2. Implement Recovery Actions

- Register recovery actions for common errors
- Implement retry logic for transient errors
- Provide graceful degradation for non-critical components

### 3. Use Error Context

- Add context information to errors
- Use error context for debugging and logging
- Include operation names in error contexts

### 4. Configure Error Handling Appropriately

- Configure error handling based on application requirements
- Balance between strict error checking and performance
- Consider application availability requirements

### 5. Test Error Handling

- Include error handling tests in your test suite
- Test recovery mechanisms under different conditions
- Verify graceful degradation behavior

## Conclusion

The NLM Error Handling and Recovery System provides a comprehensive solution for error handling in the NLM framework. It offers:

- Complete exception hierarchy with specific error types
- Robust error reporting infrastructure
- Multiple recovery mechanisms (retry, checkpoints, graceful degradation)
- Integration with existing NLM components
- Backward compatibility with existing error handling patterns
- Extensive configuration options

The system is designed to be flexible, configurable, and easy to integrate while providing the reliability and robustness needed for production NLM applications.
