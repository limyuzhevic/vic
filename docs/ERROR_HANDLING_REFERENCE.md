# NLM Error Handling Reference

## Overview
This comprehensive reference document provides detailed information about error handling in the NLM (熙然) neural learning machine system, including the enhanced ErrorHandler utility class, error categorization, and error handling best practices.

## Error Handling Architecture

The NLM error handling system is built around the ErrorHandler utility class, which provides:

1. **Comprehensive Error Categorization**: Errors are categorized by severity and type
2. **Consistent Error Reporting**: Errors are reported consistently across all components
3. **Configurable Error Callbacks**: Integration with existing error logging systems
4. **Platform-Specific Stack Traces**: Enhanced debugging capabilities
5. **Error History Tracking**: Tracking of error occurrences and patterns

## Error Severity Levels

### ErrorHandler::Severity Enumeration

```cpp
enum class ErrorHandler::Severity {
    DEBUG,        // Debug information
    INFO,         // General information
    WARNING,      // Warning conditions
    ERROR,        // Error conditions
    CRITICAL,     // Critical errors
    FATAL         // Fatal errors
};
```

### Severity Levels and Descriptions

| Severity | Description | Typical Use Case |
|----------|-------------|------------------|
| DEBUG | Debug information for development | Development and debugging |
| INFO | General information messages | Normal operation logging |
| WARNING | Warning conditions | Non-critical issues |
| ERROR | Error conditions | Recoverable errors |
| CRITICAL | Critical errors | Serious issues requiring attention |
| FATAL | Fatal errors | Unrecoverable errors |

## Error Categories

### ErrorHandler::Category Enumeration

```cpp
enum class ErrorHandler::Category {
    CONFIG,        // Configuration errors
    MEMORY,        // Memory management errors
    NETWORK,       // Network communication errors
    FILE,          // File system errors
    NEURAL,        // Neural computation errors
    PLASTICITY,    // Plasticity rule errors
    PREDICTION,    // Prediction system errors
    MEMORY_SYSTEM, // Memory system errors
    NEUROMODULATION, // Neuromodulation errors
    DEVELOPMENT,   // Development system errors
    ENVIRONMENT,   // Environment interface errors
    SYSTEM,        // System-level errors
    UNKNOWN        // Unknown or uncategorized errors
};
```

## Error Structure

### ErrorHandler::Error Structure

```cpp
struct ErrorHandler::Error {
    std::chrono::system_clock::time_point timestamp;  // Error occurrence time
    ErrorHandler::Severity severity;                 // Error severity
    ErrorHandler::Category category;                 // Error category
    std::string message;                             // Error message
    std::string details;                             // Detailed error information
    std::string source;                              // Source of the error
    std::vector<std::string> stackTrace;            // Stack trace information
    std::map<std::string, std::string> context;     // Error context information
    std::map<std::string, std::any> metadata;       // Additional error metadata
};
```

## ErrorHandler Class Interface

### Core Methods

#### ErrorHandler::report()

```cpp
void report(Severity severity, const std::string& message, 
           Category category = Category::UNKNOWN,
           const std::string& details = "",
           const std::string& source = "",
           const std::vector<std::string>& stackTrace = {});
```

**Parameters**:
- `severity`: Error severity level
- `message`: Error message
- `category`: Error category (default: UNKNOWN)
- `details`: Detailed error information
- `source`: Source of the error
- `stackTrace`: Stack trace information

#### ErrorHandler::getErrors()

```cpp
std::vector<Error> getErrors(Severity minSeverity = Severity::DEBUG) const;
```

**Parameters**:
- `minSeverity`: Minimum severity level for returned errors

#### ErrorHandler::clearErrors()

```cpp
void clearErrors();
```

Clears all errors from the error handler.

#### ErrorHandler::hasErrors()

```cpp
bool hasErrors() const;
```

Returns true if there are any errors in the error handler.

#### ErrorHandler::getErrorCount()

```cpp
size_t getErrorCount(Severity severity = Severity::DEBUG) const;
```

Returns the number of errors with the specified severity level.

### Configuration Methods

#### ErrorHandler::setLogLevel()

```cpp
void setLogLevel(Severity severity);
```

Sets the minimum log level for errors.

#### ErrorHandler::getLogLevel()

```cpp
Severity getLogLevel() const;
```

Gets the current log level.

#### ErrorHandler::setCallback()

```cpp
callback.setCallback(std::function<void(const Error&)> callback);
```

Sets the error callback function.

#### ErrorHandler::getCallback()

```cpp
std::function<void(const Error&)> getCallback() const;
```

Gets the current error callback function.

### Error Management Methods

#### ErrorHandler::filterErrors()

```cpp
std::vector<Error> filterErrors(std::function<bool(const Error&)> predicate) const;
```

Filters errors based on a predicate function.

#### ErrorHandler::aggregateErrors()

```cpp
std::map<Category, std::vector<Error>> aggregateErrors() const;
```

Aggregates errors by category.

#### ErrorHandler::getRecentErrors()

```cpp
std::vector<Error> getRecentErrors(std::chrono::system_clock::duration duration) const;
```

Gets errors that occurred within the specified duration.

### Utility Methods

#### ErrorHandler::validateError()

```cpp
bool validateError(const Error& error) const;
```

Validates an error structure.

#### ErrorHandler::formatError()

```cpp
std::string formatError(const Error& error) const;
```

Formats an error for display.

#### ErrorHandler::formatErrorReport()

```cpp
std::string formatErrorReport(const std::vector<Error>& errors) const;
```

Formats a report of multiple errors.

## Error Handler Usage Examples

### Example 1: Basic Error Handling

```cpp
#include "core/utilities/ErrorHandler.hpp"

class ExampleClass {
public:
    void performOperation() {
        ErrorHandler errorHandler;
        errorHandler.setLogLevel(ErrorHandler::Severity::WARNING);
        
        try {
            // Perform operation
            if (someCondition) {
                errorHandler.report(ErrorHandler::Severity::ERROR,
                                  "Operation failed: invalid input",
                                  ErrorHandler::Category::NEURAL);
                return;
            }
            
            // Successful operation
            errorHandler.report(ErrorHandler::Severity::INFO,
                              "Operation completed successfully",
                              ErrorHandler::Category::NEURAL);
            
        } catch (const std::exception& e) {
            errorHandler.report(ErrorHandler::Severity::ERROR,
                              "Exception during operation: " + std::string(e.what()),
                              ErrorHandler::Category::SYSTEM);
        }
        
        // Check for errors
        if (errorHandler.hasErrors()) {
            auto errors = errorHandler.getErrors();
            for (const auto& error : errors) {
                std::cout << formatError(error) << std::endl;
            }
        }
    }
};
```

### Example 2: Error Callback

```cpp
#include "core/utilities/ErrorHandler.hpp"

class ErrorTrackingClass {
public:
    ErrorTrackingClass() {
        errorHandler.setCallback([this](const ErrorHandler::Error& error) {
            onErrorOccurred(error);
        });
    }
    
    void performComplexOperation() {
        try {
            // Perform complex operation
            if (complexOperationFails()) {
                errorHandler.report(ErrorHandler::Severity::ERROR,
                                  "Complex operation failed",
                                  ErrorHandler::Category::SYSTEM,
                                  "Operation timed out after 10 seconds");
            }
            
        } catch (const std::exception& e) {
            errorHandler.report(ErrorHandler::Severity::CRITICAL,
                              "Critical exception in complex operation",
                              ErrorHandler::Category::SYSTEM,
                              std::string(e.what()));
        }
    }
    
private:
    ErrorHandler errorHandler;
    
    void onErrorOccurred(const ErrorHandler::Error& error) {
        // Handle error asynchronously
        std::cout << "Error occurred: " << error.message << std::endl;
        
        // Take appropriate action based on error type
        switch (error.category) {
            case ErrorHandler::Category::MEMORY:
                handleMemoryError(error);
                break;
            case ErrorHandler::Category::NEURAL:
                handleNeuralError(error);
                break;
            case ErrorHandler::Category::CONFIG:
                handleConfigError(error);
                break;
            default:
                handleGenericError(error);
                break;
        }
    }
    
    void handleMemoryError(const ErrorHandler::Error& error) {
        // Handle memory-related errors
        std::cout << "Handling memory error" << std::endl;
        // Implement memory error recovery
    }
    
    void handleNeuralError(const ErrorHandler::Error& error) {
        // Handle neural computation errors
        std::cout << "Handling neural error" << std::endl;
        // Implement neural error recovery
    }
    
    void handleConfigError(const ErrorHandler::Error& error) {
        // Handle configuration errors
        std::cout << "Handling configuration error" << std::endl;
        // Implement configuration error recovery
    }
    
    void handleGenericError(const ErrorHandler::Error& error) {
        // Handle generic errors
        std::cout << "Handling generic error" << std::endl;
        // Implement generic error recovery
    }
    
    bool complexOperationFails() {
        // Simulate complex operation failure
        return false;
    }
};
```

### Example 3: Error Filtering and Aggregation

```cpp
#include "core/utilities/ErrorHandler.hpp"

class ErrorAnalyzer {
public:
    ErrorAnalyzer(const ErrorHandler& errorHandler) : errorHandler(errorHandler) {}
    
    void analyzeErrors() {
        auto errors = errorHandler.getErrors();
        
        // Filter high-severity errors
        auto highSeverityErrors = filterBySeverity(errors, ErrorHandler::Severity::ERROR);
        auto criticalErrors = filterBySeverity(errors, ErrorHandler::Severity::CRITICAL);
        auto fatalErrors = filterBySeverity(errors, ErrorHandler::Severity::FATAL);
        
        // Aggregate by category
        auto aggregatedErrors = errorHandler.aggregateErrors();
        
        // Analyze error patterns
        analyzeErrorPatterns(errors);
        
        // Generate error report
        generateErrorReport(errors);
    }
    
private:
    const ErrorHandler& errorHandler;
    
    std::vector<ErrorHandler::Error> filterBySeverity(
        const std::vector<ErrorHandler::Error>& errors,
        ErrorHandler::Severity severity) {
        std::vector<ErrorHandler::Error> filteredErrors;
        for (const auto& error : errors) {
            if (error.severity == severity) {
                filteredErrors.push_back(error);
            }
        }
        return filteredErrors;
    }
    
    void analyzeErrorPatterns(const std::vector<ErrorHandler::Error>& errors) {
        // Analyze error patterns
        std::map<ErrorHandler::Category, size_t> errorCounts;
        std::map<std::string, size_t> sourceCounts;
        
        for (const auto& error : errors) {
            errorCounts[error.category]++;
            sourceCounts[error.source]++;
        }
        
        std::cout << "Error pattern analysis:" << std::endl;
        for (const auto& pair : errorCounts) {
            std::cout << "  " << errorCategoryToString(pair.first) << ": " << pair.second << std::endl;
        }
        
        for (const auto& pair : sourceCounts) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    void generateErrorReport(const std::vector<ErrorHandler::Error>& errors) {
        // Generate formatted error report
        std::string report = errorHandler.formatErrorReport(errors);
        std::cout << report << std::endl;
    }
    
    std::string errorCategoryToString(ErrorHandler::Category category) {
        // Convert error category to string
        switch (category) {
            case ErrorHandler::Category::CONFIG: return "CONFIG";
            case ErrorHandler::Category::MEMORY: return "MEMORY";
            case ErrorHandler::Category::NETWORK: return "NETWORK";
            case ErrorHandler::Category::FILE: return "FILE";
            case ErrorHandler::Category::NEURAL: return "NEURAL";
            case ErrorHandler::Category::PLASTICITY: return "PLASTICITY";
            case ErrorHandler::Category::PREDICTION: return "PREDICTION";
            case ErrorHandler::Category::MEMORY_SYSTEM: return "MEMORY_SYSTEM";
            case ErrorHandler::Category::NEUROMODULATION: return "NEUROMODULATION";
            case ErrorHandler::Category::DEVELOPMENT: return "DEVELOPMENT";
            case ErrorHandler::Category::ENVIRONMENT: return "ENVIRONMENT";
            case ErrorHandler::Category::SYSTEM: return "SYSTEM";
            case ErrorHandler::Category::UNKNOWN: return "UNKNOWN";
            default: return "UNKNOWN";
        }
    }
};
```

## Error Handling Best Practices

### 1. Use Appropriate Error Severity

- **DEBUG**: Use for development and debugging information
- **INFO**: Use for general information messages
- **WARNING**: Use for non-critical issues that may require attention
- **ERROR**: Use for recoverable errors
- **CRITICAL**: Use for serious issues that may affect system stability
- **FATAL**: Use for unrecoverable errors that cause system failure

### 2. Provide Meaningful Error Messages

- Include specific details about the error
- Provide context about what went wrong
- Include actionable information for recovery
- Use consistent error message format

### 3. Handle Errors at Appropriate Levels

- **Handle errors at the lowest possible level**: Catch and handle specific errors early
- **Propagate errors appropriately**: Allow errors to propagate to higher levels for handling
- **Avoid swallowing errors**: Never ignore errors without handling

### 4. Use Error Callbacks for Asynchronous Handling

- Use error callbacks for asynchronous error handling
- Implement proper error callback management
- Ensure error callbacks are thread-safe

### 5. Implement Error Recovery Mechanisms

- Implement recovery mechanisms for different error types
- Use retry logic for transient errors
- Implement fallback behavior for critical errors

### 6. Monitor Error Patterns

- Track error frequencies and patterns
- Implement error alerting for unusual patterns
- Use error analytics to improve system reliability

### 7. Ensure Thread Safety

- Make error handling operations thread-safe
- Use appropriate synchronization mechanisms
- Avoid race conditions in error handling

## Common Error Handling Patterns

### Pattern 1: Try-Catch with Error Reporting

```cpp
void performOperation() {
    ErrorHandler errorHandler;
    
    try {
        // Perform operation
        if (operationFails()) {
            throw std::runtime_error("Operation failed");
        }
        
    } catch (const std::exception& e) {
        errorHandler.report(ErrorHandler::Severity::ERROR,
                          "Exception during operation: " + std::string(e.what()),
                          ErrorHandler::Category::SYSTEM);
    }
    
    // Check and handle errors
    if (errorHandler.hasErrors()) {
        handleErrors(errorHandler.getErrors());
    }
}
```

### Pattern 2: Error Handling with Recovery

```cpp
bool performOperationWithRecovery() {
    ErrorHandler errorHandler;
    
    try {
        // Perform operation
        if (operationFails()) {
            errorHandler.report(ErrorHandler::Severity::ERROR,
                              "Operation failed, attempting recovery",
                              ErrorHandler::Category::SYSTEM);
            
            // Attempt recovery
            if (!attemptRecovery()) {
                errorHandler.report(ErrorHandler::Severity::CRITICAL,
                                  "Recovery failed",
                                  ErrorHandler::Category::SYSTEM);
                return false;
            }
        }
        
    } catch (const std::exception& e) {
        errorHandler.report(ErrorHandler::Severity::ERROR,
                          "Exception during operation: " + std::string(e.what()),
                          ErrorHandler::Category::SYSTEM);
        return false;
    }
    
    return true;
}
```

### Pattern 3: Error Aggregation

```cpp
class MultiOperationExecutor {
public:
    std::vector<ErrorHandler::Error> executeMultipleOperations() {
        ErrorHandler errorHandler;
        std::vector<ErrorHandler::Error> allErrors;
        
        // Execute multiple operations
        if (!executeOperation1(errorHandler)) {
            allErrors = errorHandler.getErrors();
        }
        
        errorHandler.clearErrors();
        
        if (!executeOperation2(errorHandler)) {
            auto newErrors = errorHandler.getErrors();
            allErrors.insert(allErrors.end(), newErrors.begin(), newErrors.end());
        }
        
        errorHandler.clearErrors();
        
        if (!executeOperation3(errorHandler)) {
            auto newErrors = errorHandler.getErrors();
            allErrors.insert(allErrors.end(), newErrors.begin(), newErrors.end());
        }
        
        return allErrors;
    }
    
private:
    bool executeOperation1(ErrorHandler& errorHandler) {
        // Implement operation 1
        return true;
    }
    
    bool executeOperation2(ErrorHandler& errorHandler) {
        // Implement operation 2
        return true;
    }
    
    bool executeOperation3(ErrorHandler& errorHandler) {
        // Implement operation 3
        return true;
    }
};
```

## Error Handling Integration

### Integration with Neural Activity Calculator

```cpp
#include "core/utilities/NeuralActivityCalculator.hpp"
#include "core/utilities/ErrorHandler.hpp"

class NeuralActivityAnalyzer {
public:
    NeuralActivityAnalyzer(ErrorHandler& errorHandler) : errorHandler(errorHandler) {}
    
    std::vector<double> analyzeNeuralActivity(const std::vector<double>& voltages) {
        NeuralActivityCalculator calculator;
        std::vector<double> activities;
        
        try {
            for (size_t i = 1; i < voltages.size(); ++i) {
                // Calculate neural activity
                double activity = calculator.calculate(voltages[i-1], voltages[i]);
                
                // Validate activity
                if (std::isnan(activity) || std::isinf(activity)) {
                    errorHandler.report(ErrorHandler::Severity::ERROR,
                                      "Invalid neural activity: " + std::to_string(activity),
                                      ErrorHandler::Category::NEURAL,
                                      "Voltage values: " + std::to_string(voltages[i-1]) + ", " + std::to_string(voltages[i]));
                    continue;
                }
                
                activities.push_back(activity);
            }
            
        } catch (const std::exception& e) {
            errorHandler.report(ErrorHandler::Severity::ERROR,
                              "Exception during neural activity analysis: " + std::string(e.what()),
                              ErrorHandler::Category::NEURAL);
        }
        
        return activities;
    }
    
private:
    ErrorHandler& errorHandler;
};
```

### Integration with Spike Detector

```cpp
#include "core/utilities/SpikeDetector.hpp"
#include "core/utilities/ErrorHandler.hpp"

class SpikeAnalyzer {
public:
    SpikeAnalyzer(ErrorHandler& errorHandler) : errorHandler(errorHandler) {}
    
    std::vector<double> analyzeSpikes(const std::vector<double>& voltages, double timestep) {
        SpikeDetector detector;
        detector.setThreshold(20.0);
        detector.setRefractoryPeriod(5.0);
        
        std::vector<double> spikeTimes;
        
        try {
            for (size_t i = 0; i < voltages.size(); ++i) {
                double voltage = voltages[i];
                
                // Detect spikes
                if (detector.detectSpike(voltage)) {
                    double spikeTime = i * timestep;
                    spikeTimes.push_back(spikeTime);
                }
                
                // Validate spike times
                if (!validateSpikeTime(spikeTime)) {
                    errorHandler.report(ErrorHandler::Severity::WARNING,
                                      "Invalid spike time: " + std::to_string(spikeTime),
                                      ErrorHandler::Category::NEURAL,
                                      "Voltage: " + std::to_string(voltage));
                }
            }
            
        } catch (const std::exception& e) {
            errorHandler.report(ErrorHandler::Severity::ERROR,
                              "Exception during spike analysis: " + std::string(e.what()),
                              ErrorHandler::Category::NEURAL);
        }
        
        return spikeTimes;
    }
    
private:
    ErrorHandler& errorHandler;
    
    bool validateSpikeTime(double spikeTime) {
        // Validate spike time
        return spikeTime >= 0.0 && spikeTime < 3600.0;  // Spike time must be within 1 hour
    }
};
```

## Error Handling Testing

### Testing Error Handling

```cpp
#include "core/utilities/ErrorHandler.hpp"
#include <gtest/gtest.h>

class ErrorHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        errorHandler.setLogLevel(ErrorHandler::Severity::WARNING);
    }
    
    ErrorHandler errorHandler;
};

TEST_F(ErrorHandlerTest, ReportError) {
    errorHandler.report(ErrorHandler::Severity::ERROR, "Test error");
    EXPECT_TRUE(errorHandler.hasErrors());
    EXPECT_EQ(errorHandler.getErrorCount(), 1);
}

TEST_F(ErrorHandlerTest, ReportErrorWithCategory) {
    errorHandler.report(ErrorHandler::Severity::WARNING, "Test warning",
                       ErrorHandler::Category::NEURAL);
    EXPECT_TRUE(errorHandler.hasErrors());
    
    auto errors = errorHandler.getErrors();
    EXPECT_EQ(errors.size(), 1);
    EXPECT_EQ(errors[0].category, ErrorHandler::Category::NEURAL);
}

TEST_F(ErrorHandlerTest, ReportErrorWithCallback) {
    bool errorReported = false;
    errorHandler.setCallback([&errorReported](const ErrorHandler::Error& error) {
        errorReported = true;
    });
    
    errorHandler.report(ErrorHandler::Severity::INFO, "Test error with callback");
    EXPECT_TRUE(errorReported);
}

TEST_F(ErrorHandlerTest, FilterErrors) {
    errorHandler.report(ErrorHandler::Severity::ERROR, "Error 1");
    errorHandler.report(ErrorHandler::Severity::WARNING, "Warning 1");
    errorHandler.report(ErrorHandler::Severity::INFO, "Info 1");
    
    auto errors = errorHandler.getErrors(ErrorHandler::Severity::ERROR);
    EXPECT_EQ(errors.size(), 1);
    EXPECT_EQ(errors[0].severity, ErrorHandler::Severity::ERROR);
}

TEST_F(ErrorHandlerTest, ClearErrors) {
    errorHandler.report(ErrorHandler::Severity::ERROR, "Error 1");
    errorHandler.report(ErrorHandler::Severity::WARNING, "Warning 1");
    
    EXPECT_TRUE(errorHandler.hasErrors());
    errorHandler.clearErrors();
    EXPECT_FALSE(errorHandler.hasErrors());
    EXPECT_EQ(errorHandler.getErrorCount(), 0);
}

TEST_F(ErrorHandlerTest, ErrorValidation) {
    ErrorHandler::Error error;
    error.severity = ErrorHandler::Severity::ERROR;
    error.category = ErrorHandler::Category::NEURAL;
    error.message = "Test error";
    
    EXPECT_TRUE(errorHandler.validateError(error));
}
```

## Error Handling Utilities

### Error Handler Utilities

```cpp
namespace ErrorHandlerUtils {
    // Format error for display
    std::string formatError(const ErrorHandler::Error& error);
    
    // Format error report
    std::string formatErrorReport(const std::vector<ErrorHandler::Error>& errors);
    
    // Get error severity as string
    std::string severityToString(ErrorHandler::Severity severity);
    
    // Get error category as string
    std::string categoryToString(ErrorHandler::Category category);
    
    // Check if error is critical
    bool isCritical(const ErrorHandler::Error& error);
    
    // Check if error is fatal
    bool isFatal(const ErrorHandler::Error& error);
    
    // Get error timestamp as string
    std::string timestampToString(const ErrorHandler::Error& error);
    
    // Get error source
    std::string getErrorSource(const ErrorHandler::Error& error);
    
    // Get error details
    std::string getErrorDetails(const ErrorHandler::Error& error);
    
    // Get error stack trace
    std::string getErrorStackTrace(const ErrorHandler::Error& error);
}
```

## Conclusion

The NLM error handling system provides comprehensive error management capabilities. By following the best practices and using the provided utilities, you can ensure robust error handling in your NLM applications.

Key aspects of the error handling system:

1. **Comprehensive Error Categorization**: Errors are categorized by severity and type
2. **Consistent Error Reporting**: Errors are reported consistently across all components
3. **Configurable Error Callbacks**: Integration with existing error logging systems
4. **Platform-Specific Stack Traces**: Enhanced debugging capabilities
5. **Error History Tracking**: Tracking of error occurrences and patterns
6. **Flexible Error Filtering**: Filter errors by severity, category, and source
7. **Error Analysis**: Analyze error patterns and trends
8. **Error Reporting**: Generate formatted error reports

For more detailed information, refer to the other documentation files, particularly the usage guide, advanced usage guide, and plugin development guide.