# Enhanced Logger with Comprehensive Error Handling and Context Support

This document outlines the enhanced logging system for NLM Phase 6, providing comprehensive error handling, context-aware logging, and performance monitoring.

## Enhanced Logger Features

### 1. Context-Aware Logging

The enhanced logger provides detailed context information for all log entries:

- **Thread Identification**: Automatic thread tracking for concurrent operations
- **Operation Context**: Hierarchical context management for complex operations
- **Global Context**: Application-level context sharing across components
- **Performance Monitoring**: Timing and duration tracking for operations

### 2. Enhanced Error Handling

Improved error handling capabilities:

- **Structured Error Reports**: Consistent error format with severity levels
- **Recovery Mechanisms**: Automatic error recovery and health monitoring
- **Exception Handling**: Comprehensive exception logging with stack traces
- **Error Classification**: Error categorization for better analysis

### 3. Performance Monitoring

Built-in performance monitoring:

- **Operation Timing**: Precise timing of critical operations
- **Performance Thresholds**: Configurable performance alerts
- **Resource Monitoring**: Memory, CPU, and I/O usage tracking
- **Performance Reports**: Detailed performance analysis and reporting

### 4. Advanced Logging Macros

Enhanced macro system for better logging:

```cpp
// Context-aware logging
NLM_LOG_ERROR_WITH_CONTEXT("Database connection failed", "DatabaseManager::connect")
NLM_LOG_WARNING_WITH_CONTEXT("High memory usage", "MemoryManager::allocate")
NLM_LOG_INFO_WITH_CONTEXT("Processing complete", "DataProcessor::transform")

// Stream-based logging
NLM_LOG_STREAM_ERROR() << "Failed to open file: " << filename;
NLM_LOG_STREAM_INFO() << "Starting processing of " << count << " items";
```

## Implementation Details

### Core Classes

#### LogEntry Structure
```cpp
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;      // Thread identification
    std::string context;       // Operation context
    std::string exceptionType; // Exception type (for error logging)
    std::string stackTrace;    // Exception stack trace
    double duration;           // Operation duration
    std::string performanceData;// Additional performance metrics
};
```

#### Logger Class
Enhanced with:
- **Context Management**: Hierarchical context system
- **Error Recovery**: Automatic error detection and recovery
- **Performance Monitoring**: Built-in performance tracking
- **Health Monitoring**: System health checks and alerts

#### ConsoleLogger Class
Enhanced with:
- **Color Coding**: Enhanced terminal output with colors
- **File Output**: Separate log file writing
- **Structured Formatting**: Consistent log format
- **Error Highlighting**: Special formatting for errors

### Key Features

#### 1. Thread-Local Context
```cpp
class ThreadLocalContext {
public:
    static std::string getCurrentContext();
    static void setCurrentContext(const std::string& context);
    
private:
    static thread_local std::string threadContext_;
};
```

#### 2. Global Context Management
```cpp
class LoggingContextManager {
public:
    static void setGlobalContext(const std::string& context);
    static std::string getGlobalContext();
    static void addContext(const std::string& context);
    static std::string getFullContext();
};
```

#### 3. Performance Monitoring
```cpp
class PerformanceMonitor {
public:
    struct Scope {
        Scope(const std::string& name);
        ~Scope();
        double getDuration() const;
    };
    
    static void setLogLevel(LogLevel level);
    static std::vector<std::string> getPerformanceReport();
    static void reset();
};
```

## Usage Examples

### Basic Context-Aware Logging
```cpp
// Set global context
LoggingContextManager::setGlobalContext("Application");

// Set thread-specific context
ThreadLocalContext::setCurrentContext("Database");

// Log with context
NLM_LOG_INFO_WITH_CONTEXT("Processing request", "RequestHandler::process");

// Stream-based logging with context
NLM_LOG_STREAM_ERROR() << "Failed to connect to database"
                      << " (Context: " << ThreadLocalContext::getCurrentContext() << ")";
```

### Error Handling with Exception Information
```cpp
try {
    // Code that might throw exceptions
    performDatabaseOperation();
} catch (const std::exception& e) {
    NLM_LOG_ERROR_WITH_CONTEXT(
        std::string("Operation failed: ") + e.what(),
        "ErrorHandler::handleException"
    );
    
    // Log exception with stack trace
    ConsoleLogger::logException(e, "DatabaseOperation::execute");
}
```

### Performance Monitoring
```cpp
// Monitor operation performance
NLM_PERFORMANCE_SCOPE("DatabaseQuery");

// Perform database operation
result = database.query("SELECT * FROM users");

// Log performance results
PerformanceMonitor::getPerformanceReport();
```

### Error Recovery and Health Monitoring
```cpp
// Check system health
if (!Logger::getGlobal().isHealthy()) {
    std::vector<std::string> issues = Logger::getGlobal().getHealthIssues();
    for (const auto& issue : issues) {
        NLM_LOG_ERROR_WITH_CONTEXT(issue, "HealthMonitor::check");
    }
}
```

## Error Handling Strategies

### 1. Graceful Degradation
- **Component Failure**: Continue with reduced functionality
- **Resource Exhaustion**: Fallback to lower-quality operations
- **Network Issues**: Implement retry mechanisms

### 2. Automatic Recovery
- **Log File Rotation**: Automatic log file management
- **Memory Management**: Automatic cleanup of resources
- **Connection Management**: Automatic reconnection attempts

### 3. Health Monitoring
- **System Checks**: Regular health checks of all components
- **Alert Management**: Configurable alert thresholds
- **Recovery Actions**: Automatic recovery actions

## Performance Considerations

### 1. Logging Overhead
- **Conditional Logging**: Only log messages at or above the configured level
- **Asynchronous Logging**: Separate thread for log writing
- **Buffer Management**: Efficient buffer usage for log entries

### 2. Context Management
- **Hierarchical Contexts**: Efficient context hierarchy management
- **Context Caching**: Cache frequently used context combinations
- **Memory Efficiency**: Minimal memory overhead for context management

### 3. Performance Monitoring
- **Lightweight Timing**: Fast timing operations
- **Configurable Monitoring**: Only monitor when needed
- **Aggregated Reporting**: Batch reporting to reduce overhead

## Configuration Options

### Logging Configuration
```cpp
// Set global log level
Logger::getGlobal().setLevel(LogLevel::Info);

// Enable performance logging
Logger::getGlobal().enablePerformanceLogging(true);

// Set performance log level
Logger::getGlobal().setPerformanceLogLevel(LogLevel::Warning);

// Set log file
Logger::getGlobal().setLogFile("/var/log/nlm.log");
```

### Context Configuration
```cpp
// Set global context
LoggingContextManager::setGlobalContext("NLM");

// Add operation context
LoggingContextManager::addContext("Simulation");
LoggingContextManager::addContext("Phase6");

// Get full context for logging
std::string context = LoggingContextManager::getFullContext();
```

## Integration with NLM Systems

### 1. Configuration Validation
```cpp
// Enhanced configuration validation with logging
bool Config::validate() const {
    bool isValid = true;
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            errors.push_back("Invalid key format: '" + entry.key + "'");
            isValid = false;
        }
        
        if (!validateValue(entry.key, entry.value)) {
            errors.push_back("Invalid value for key '" + entry.key + "'");
            isValid = false;
        }
    }
    
    // Log validation errors
    if (!isValid && !errors.empty()) {
        NLM_LOG_ERROR_WITH_CONTEXT(
            "Configuration validation failed: " + errors[0],
            "Config::validate"
        );
    }
    
    return isValid;
}
```

### 2. Reward Computation Logging
```cpp
// Enhanced reward computation with logging
float Reward::computeReward(const Observation& observation, 
                          std::shared_ptr<const Config> config) {
    float totalReward = 0.0f;
    
    // Log computation start
    NLM_LOG_INFO_WITH_CONTEXT(
        "Computing reward with observation dimensions: " + 
        std::to_string(observation.getData().size()),
        "Reward::computeReward"
    );
    
    // Component calculations with logging
    float survivalReward = computeSurvivalReward(observation, config);
    NLM_LOG_DEBUG_WITH_CONTEXT(
        std::string("Survival reward: ") + std::to_string(survivalReward),
        "Reward::computeReward"
    );
    
    // Continue with other components...
    
    // Log final result
    NLM_LOG_INFO_WITH_CONTEXT(
        std::string("Total reward: ") + std::to_string(totalReward),
        "Reward::computeReward"
    );
    
    return totalReward;
}
```

## Testing and Validation

### 1. Unit Testing
- **Component Testing**: Test each logging component in isolation
- **Integration Testing**: Test logging integration with other components
- **Performance Testing**: Test performance characteristics
- **Error Testing**: Test error handling and recovery

### 2. Validation
```cpp
// Example unit test for enhanced logging
void testEnhancedLogging() {
    // Set up logging context
    LoggingContextManager::setGlobalContext("Test");
    ThreadLocalContext::setCurrentContext("UnitTest");
    
    // Test context-aware logging
    NLM_LOG_INFO_WITH_CONTEXT("Test message", "TestLogger::test");
    
    // Test error logging
    NLM_LOG_ERROR_WITH_CONTEXT("Test error", "TestLogger::testError");
    
    // Test performance monitoring
    NLM_PERFORMANCE_SCOPE("PerformanceTest");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Verify log output
    // ... assertion logic ...
}
```

## Benefits of Enhanced Logging

### 1. Better Debugging
- **Detailed Context**: Rich context information for debugging
- **Structured Logs**: Consistent log format for easy parsing
- **Error Classification**: Categorized errors for better analysis

### 2. Improved Monitoring
- **Real-time Monitoring**: Real-time monitoring of system health
- **Performance Tracking**: Continuous performance monitoring
- **Resource Management**: Efficient resource usage monitoring

### 3. Enhanced Reliability
- **Error Recovery**: Automatic error detection and recovery
- **Health Checks**: Regular health checks of system components
- **Graceful Degradation**: Continuation with reduced functionality

### 4. Better User Experience
- **Clear Messages**: Human-readable log messages
- **Contextual Information**: Relevant context for each log entry
- **Actionable Insights**: Log messages that suggest actions

## Conclusion

The enhanced logging system in NLM Phase 6 provides:

1. **Comprehensive Error Handling**: Graceful error handling with recovery mechanisms
2. **Context-Aware Logging**: Rich context information for better debugging
3. **Performance Monitoring**: Built-in performance monitoring and analysis
4. **Health Monitoring**: System health checks and automatic recovery
5. **Better User Experience**: Clear, actionable log messages

These improvements make NLM more robust, maintainable, and easier to debug and monitor, while providing valuable insights into system performance and behavior.
