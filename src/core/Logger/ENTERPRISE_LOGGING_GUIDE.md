# NLM Enterprise Logging and Error Handling System

This document describes the comprehensive enterprise-grade logging and error handling system implemented for NLM (Neural Learning Machine).

## Overview

The NLM enterprise logging system transforms the basic logging infrastructure into a sophisticated platform capable of handling research, production, and mission-critical applications. It provides:

### Advanced Error Handling Framework
- Custom exception hierarchy for NLM with specific error categories
- Error severity levels (DEBUG, INFO, WARNING, ERROR, CRITICAL, ALERT, EMERGENCY)
- Exception chaining and context preservation
- Error recovery mechanisms and fallback strategies
- Thread-safe error reporting and collection
- Error statistics and analytics

### Enterprise Logging System
- Multiple output handlers (console, file, network, database)
- Structured logging with JSON/XML/CSV output
- Log levels with filtering and thresholding
- Performance logging with timing and profiling
- Memory usage logging and garbage collection tracking
- Distributed logging with correlation IDs
- Log rotation and log compression

### Real-time Monitoring and Alerting
- Performance metrics collection and real-time monitoring
- System health checks and health monitoring
- Alert threshold configuration and notification systems
- Resource monitoring (CPU, memory, I/O)
- Application-specific health indicators

### Integration and Customization
- Integration with existing NLM systems (brain, neuromodulation, commands)
- Plugin architecture for custom log handlers
- Configuration-based log management
- External service integration (ELK stack, Graylog, etc.)
- Custom log formatters and serializers

## Technical Architecture

### Error Handling System

#### Exception Hierarchy
```cpp
// Base NLM exception
class NLMException : public std::exception {
public:
    std::string message;
    ExceptionCategory category;
    ExceptionSeverity severity;
    std::string file, function;
    int line;
    std::chrono::system_clock::time_point timestamp;
    std::vector<ContextInfo> contextStack;
};

// Specialized exception types
class NLMInitException : public NLMException;  // Initialization failures
class NLMMemoryException : public NLMException;  // Memory allocation errors
class NLMConfigException : public NLMException;  // Configuration errors
class NLMNetworkException : public NLMException;  // Network errors
```

#### Exception Categories
- **Core System**: INITIALIZATION_FAILURE, CONFIGURATION_ERROR, MEMORY_ALLOCATION_FAILURE, NETWORK_ERROR
- **Neural Network**: NEURON_ERROR, SYNAPSE_ERROR, BRAIN_ERROR, PLASTICITY_ERROR, DYNAMICS_ERROR
- **Cognitive**: CONCEPT_FORMATION_ERROR, PLANNING_ERROR, PERCEPTION_ERROR, ACTION_ERROR
- **Neuromodulation**: DOPAMINE_ERROR, REWARD_ERROR, NOVELTY_ERROR, CURIOUSITY_ERROR, PREDICTION_ERROR
- **Memory**: WORKING_MEMORY_ERROR, EPISODIC_MEMORY_ERROR, SEMANTIC_MEMORY_ERROR, PROCEDURAL_MEMORY_ERROR, ASSOCIATIVE_MEMORY_ERROR
- **System**: THREADING_ERROR, RESOURCE_EXHAUSTION, TIMEOUT_ERROR, PLUGIN_LOAD_ERROR, VALIDATION_ERROR, UNKNOWN_ERROR

#### Error Reporting
The ErrorReporter class provides:
- Thread-safe error collection and reporting
- Automatic error context preservation
- Recovery mechanism support
- Error statistics and analytics
- Callback-based error notification

### Logging System

#### Enhanced LogEntry Structure
```cpp
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string processId;
    std::string module;
    uint64_t correlationId;
    
    // Performance metrics
    std::chrono::microseconds executionTime;
    uint64_t memoryUsage;
    
    // Structured data
    std::unordered_map<std::string, std::string> fields;
    std::unordered_map<std::string, double> numericFields;
    
    // Custom context
    std::unordered_map<std::string, std::string> context;
    
    // Exception information
    std::string exceptionType;
    std::string exceptionMessage;
    std::string stackTrace;
};
```

#### Log Handlers
1. **ConsoleLogHandler**: Enhanced console output with colors, emojis, and formatting
2. **FileLogHandler**: File-based logging with rotation and compression
3. **JSONLogHandler**: Structured JSON logging for external systems
4. **MemoryLogHandler**: Specialized memory and garbage collection logging

#### Formatters
1. **JSONLogFormatter**: JSON-structured output
2. **XMLLogFormatter**: XML-structured output
3. **CSVLogFormatter**: CSV-structured output
4. **PlainTextFormatter**: Human-readable plain text output

### Monitoring and Analytics

#### Performance Monitoring
- **PerformanceLogger**: Automatic timing and profiling of operations
- **MemoryTracker**: Memory allocation/deallocation tracking
- **SystemProfiler**: Comprehensive system resource monitoring

#### Health Monitoring
- **System Health Checks**: CPU, memory, I/O, and application-specific checks
- **Error Rate Monitoring**: Track error frequencies and patterns
- **Recovery Metrics**: Monitor error recovery success rates

#### Correlation IDs
- **Distributed Tracing**: Maintain request context across systems
- **Correlation Context**: Nested correlation contexts for complex operations
- **Thread Safety**: Thread-safe correlation ID management

### Integration Features

#### Configuration
- **Environment-based Configuration**: Load settings from environment variables
- **File-based Configuration**: JSON, YAML, TOML, and custom formats
- **Runtime Configuration**: Dynamic configuration updates

#### Plugin Architecture
- **Handler Plugins**: Custom log handlers for external systems
- **Formatter Plugins**: Custom log formatters for different output formats
- **Extension Points**: Extensible architecture for custom functionality

#### External Integrations
- **ELK Stack**: Integration with Elasticsearch, Logstash, and Kibana
- **Graylog**: Integration with centralized logging infrastructure
- **Splunk**: Integration with Splunk logging platform
- **Custom Services**: Generic integration framework for external services

## API Documentation

### Core Classes

#### Logger
```cpp
class Logger {
public:
    // Basic logging methods
    void debug(const std::string& message, ...);
    void info(const std::string& message, ...);
    void warning(const std::string& message, ...);
    void error(const std::string& message, ...);
    void critical(const std::string& message, ...);
    
    // Structured logging
    void log(LogLevel level, const std::string& message, 
             const std::string& file, int line,
             const std::string& function,
             const std::string& module = "",
             const std::unordered_map<std::string, std::string>& fields = {});
    
    // Performance logging
    PerformanceLogger startPerformanceLog(const std::string& module);
    
    // Memory tracking
    void logMemoryStats(const std::string& context = "");
    
    // Health checks
    bool performHealthCheck();
    
    // Correlation IDs
    void startCorrelation(const std::string& context);
    void endCorrelation();
    uint64_t getCurrentCorrelationId() const;
    
    // Enterprise mode
    void setEnterpriseMode(bool enabled);
    bool isEnterpriseMode() const;
    
    // Static methods
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
};
```

#### NLMException
```cpp
class NLMException : public std::exception {
public:
    NLMException(const std::string& message, ExceptionCategory category,
                 ExceptionSeverity severity = ExceptionSeverity::ERROR,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    
    // Exception chaining
    template<typename... Args>
    NLMException& addContext(const std::string& context, const Args&... args);
    NLMException& operator<<(const std::string& context);
    
    // Accessors
    const std::string& getMessage() const;
    ExceptionCategory getCategory() const;
    ExceptionSeverity getSeverity() const;
    const std::string& getFile() const;
    int getLine() const;
    const std::string& getFunction() const;
    bool canRecover() const;
};
```

#### ErrorReporter
```cpp
class ErrorReporter {
public:
    struct ErrorReport {
        std::string id;
        std::string message;
        ExceptionCategory category;
        ExceptionSeverity severity;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        std::vector<std::string> stackTrace;
        bool recovered;
        std::string recoveryAction;
    };
    
    using ErrorCallback = std::function<void(const ErrorReport&)>;
    
    // Error reporting
    template<typename TException>
    void report(const TException& exception, bool autoRecover = false);
    
    // Recovery
    bool attemptRecovery(const ErrorReport& report, const std::string& strategy);
    
    // Statistics
    SystemErrorStats getSystemStats() const;
    
    // Callbacks
    void addErrorCallback(ErrorCallback callback);
    
    // Configuration
    void setRecoveryEnabled(bool enabled);
    void setMaxHistorySize(size_t size);
    
    // Accessors
    std::vector<ErrorReport> getRecentErrors(size_t limit = 100) const;
};
```

### Macros and Convenience Functions

#### Logging Macros
```cpp
// Original macros (backward compatibility)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream logging
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Structured logging
#define NLM_LOG_WITH_FIELDS(level, message, ...) \
    do { \
        std::unordered_map<std::string, std::string> fields = {__VA_ARGS__}; \
        nlm::Logger::getGlobal().log(level, message, __FILE__, __LINE__, __func__, "", fields); \
    } while(0)

// Enterprise macros
#define NLM_PERFORMANCE_LOG(module) nlm::PerformanceLogger performanceLogger(module)
#define NLM_PERFORMANCE_START(module, operation) performanceLogger.startTiming(operation)
#define NLM_PERFORMANCE_END(operation) performanceLogger.endTiming(operation)

#define NLM_CORRELATION_START(context) nlm::getCorrelationManager().startCorrelation(context)
#define NLM_CORRELATION_END() nlm::getCorrelationManager().endCorrelation()

#define NLM_HEALTH_CHECK() nlm::Logger::getGlobal().performHealthCheck()
```

#### Exception Macros
```cpp
#define NLM_THROW(category, severity, message) \
    do { \
        nlm::NLMException exc(message, category, severity, __FILE__, __LINE__, __func__); \
        nlm::reportError(exc); \
        throw exc; \
    } while(0)

#define NLM_THROW_WITH_CONTEXT(category, severity, message, context) \
    do { \
        nlm::NLMException exc(message, category, severity, __FILE__, __LINE__, __func__); \
        exc << context; \
        nlm::reportError(exc); \
        throw exc; \
    } while(0)

#define NLM_RECOVERABLE_THROW(category, severity, message) \
    do { \
        nlm::NLMException exc(message, category, severity, __FILE__, __LINE__, __func__); \
        if (exc.canRecover()) { \
            nlm::reportError(exc, true); \
            return exc; \
        } else { \
            nlm::reportError(exc); \
            throw exc; \
        } \
    } while(0)
```

### Configuration

#### Log Handler Configuration
```cpp
// Console handler configuration
{
    "level": "INFO",
    "colors": "true",
    "emoji": "true",
    "file": "nlm.log"
}

// File handler configuration
{
    "level": "DEBUG",
    "max_file_size": "10485760",  // 10 MB
    "max_files": "5",
    "format": "json",
    "compression": "gzip"
}

// Performance monitoring configuration
{
    "enabled": "true",
    "sampling_rate": "0.1",
    "min_duration": "1000",  // microseconds
    "track_memory": "true"
}
```

## Usage Examples

### Basic Usage
```cpp
#include "core/Logger/Logger.hpp"
#include "core/ErrorHandling/NLMExceptions.hpp"

int main() {
    // Initialize enterprise logging
    Logger::getGlobal().setEnterpriseMode(true);
    
    // Log basic messages
    NLM_LOG_INFO("Starting NLM simulation");
    NLM_LOG_DEBUG("Entering neural computation phase");
    
    // Start performance logging
    NLM_PERFORMANCE_LOG("NeuralComputation");
    NLM_PERFORMANCE_START("NeuralComputation", "SynapticUpdate");
    
    // Perform computation
    double result = performComputation();
    
    NLM_PERFORMANCE_END("SynapticUpdate");
    
    // Log with structured data
    NLM_LOG_INFO_WITH_FIELDS(
        "Computation completed",
        ("result", std::to_string(result)),
        ("duration", "123.45"),
        ("neuron_count", "500")
    );
    
    // Use correlation IDs for distributed tracing
    NLM_CORRELATION_START("SimulationRequest");
    NLM_LOG_INFO("Processing simulation with correlation ID");
    NLM_CORRELATION_END();
    
    // Error handling
    try {
        // Some operation that might fail
        riskyOperation();
    } catch (const NLMException& e) {
        NLM_LOG_ERROR("Operation failed: " + std::string(e.what()),
                     e.getFile(), e.getLine(), e.getFunction());
        
        if (e.canRecover()) {
            NLM_LOG_INFO("Attempting recovery...");
            recoverFromError();
        }
    }
    
    return 0;
}
```

### Performance Monitoring
```cpp
void neuralNetworkComputation(Brain& brain) {
    NLM_PERFORMANCE_LOG("NeuralNetwork");
    
    NLM_PERFORMANCE_START("NeuralNetwork", "SynapseActivation");
    brain.activateSynapses();
    NLM_PERFORMANCE_END("SynapseActivation");
    
    NLM_PERFORMANCE_START("NeuralNetwork", "SpikePropagation");
    brain.propagateSpikes();
    NLM_PERFORMANCE_END("SpikePropagation");
    
    NLM_PERFORMANCE_START("NeuralNetwork", "PlasticityUpdate");
    brain.updatePlasticity();
    NLM_PERFORMANCE_END("PlasticityUpdate");
    
    // Log memory statistics
    NLM_LOG_INFO("Memory usage: ", getMemoryUsage());
}
```

### Distributed Tracing
```cpp
code
++
// Process an incoming request with correlation ID
void processRequest(const std::string& requestId) {
    NLM_CORRELATION_START("HTTP Request: " + requestId);
    
    NLM_LOG_INFO_WITH_FIELDS(
        "Processing request",
        ("request_id", requestId),
        ("service", "neural-computation"),
        ("version", "1.0.0")
    );
    
    try {
        // Call another service
        auto response = externalService.call(requestId);
        
        NLM_LOG_INFO_WITH_FIELDS(
            "Request completed successfully",
            ("response_time", response.duration),
            ("neuron_count", response.neurons),
            ("learning_effect", response.learningEffect)
        );
        
    } catch (const NLMException& e) {
        NLM_LOG_ERROR_WITH_FIELDS(
            "Request processing failed",
            ("request_id", requestId),
            ("error_type", exceptionType(e)),
            ("error_category", exceptionCategory(e)),
            ("severity", exceptionSeverity(e))
        );
        
        throw; // Re-throw for upstream handling
    }
    
    NLM_CORRELATION_END();
}
```

### Health Monitoring
```cpp
bool isSystemHealthy() {
    Logger& logger = Logger::getGlobal();
    
    // Enable enterprise mode for health checks
    logger.setEnterpriseMode(true);
    
    // Perform comprehensive health check
    bool healthy = logger.performHealthCheck();
    
    if (!healthy) {
        NLM_LOG_WARNING("System health check failed", "", 0, "", "HealthMonitor");
        
        // Log detailed diagnostics
        NLM_LOG_INFO_WITH_FIELDS(
            "Health check details",
            ("error_rate", errorStats.errorRate),
            ("memory_usage", memoryStats.usage),
            ("cpu_usage", systemStats.cpuUsage),
            ("gc_collections", memoryStats.gcCollections)
        );
    }
    
    return healthy;
}
```

## Configuration and Customization

### Adding Custom Log Handlers
```cpp
class CustomLogHandler : public ILogHandler {
public:
    void handleLog(const LogEntry& entry) override {
        // Custom logging logic here
        sendToCustomSystem(entry);
    }
    
    void setLevel(LogLevel level) override { /* Implementation */ }
    LogLevel getLevel() const override { /* Implementation */ }
    void flush() override { /* Implementation */ }
    void configure(const std::unordered_map<std::string, std::string>& config) override { /* Implementation */ }
};

// Usage
Logger::getGlobal().addHandler(std::make_shared<CustomLogHandler>());
```

### Adding Custom Formatters
```cpp
class CustomFormatter : public ILogFormatter {
public:
    std::string format(const LogEntry& entry) override {
        // Custom formatting logic
        return formatAsCustomProtocol(entry);
    }
    
    std::string getSupportedFormat() const override { return "custom"; }
};

// Register custom formatter
// (Implementation depends on the plugin system)
```

### Monitoring Integration
```cpp
// Add custom error callback
void logToMonitoring(const ErrorReporter::ErrorReport& report) {
    // Send error to monitoring system
    monitoringSystem.logError(report);
}

void customLogHandler::registerCallbacks() {
    auto& errorReporter = getErrorReporter();
    errorReporter.addErrorCallback(logToMonitoring);
}
```

## Performance Considerations

### Memory Efficiency
- The logging system uses a circular buffer for error history
- Memory tracking uses efficient data structures to minimize overhead
- Performance logging is sampled to reduce overhead

### Thread Safety
- All logging operations are thread-safe
- Error reporting uses mutex-protected data structures
- Correlation IDs are thread-local with proper synchronization

### Performance Optimizations
- Log level filtering prevents expensive formatting when not needed
- Structured logging uses efficient data structures
- Performance monitoring can be enabled/disabled at runtime

## Compliance and Security

### GDPR Compliance
- Personal data in logs can be redacted using field filtering
- Log retention policies can be configured
- Access logging can be configured for audit trails

### Security Features
- Log rotation prevents log file exhaustion attacks
- Sanitization of sensitive data in log output
- Secure transmission for remote logging (TLS encryption)

## Testing

### Unit Tests
The enterprise logging system includes comprehensive unit tests:
- Basic logging functionality
- Exception handling and chaining
- Performance monitoring
- Memory tracking
- Health checks
- Correlation ID management
- Multi-threaded logging
- Log rotation
- Error reporting and recovery

### Integration Tests
- Integration with existing NLM components
- Performance under load
- Distributed tracing across multiple services
- Error recovery scenarios

## Future Enhancements

### Planned Features
1. **Network Logging**: Native support for remote syslog, TCP, and UDP logging
2. **Database Logging**: Integration with relational databases for structured storage
3. **Real-time Alerts**: Integration with alerting systems (Prometheus, Nagios)
4. **Machine Learning**: Automated log analysis and anomaly detection
5. **Log Analytics**: Built-in log aggregation and analysis capabilities

### Extension Points
- **Handler Plugins**: Easy addition of new log handlers
- **Formatter Plugins**: Support for custom log formats
- **Filter Plugins**: Advanced log filtering and routing
- **Backend Plugins**: Integration with various backend systems

## Build and Deployment

### Building
```bash
# Build the NLM library
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Build with enterprise logging
-DNLM_ENTERPRISE_LOGGING=ON
cmake -B build -DCMAKE_BUILD_TYPE=Release -DNLM_ENTERPRISE_LOGGING=ON
```

### Installation
```bash
# Install the NLM library and tools
cmake --install build --prefix /usr/local
```

### Configuration
The logging system can be configured through:
- **Environment Variables**: Set `NLM_LOG_LEVEL`, `NLM_LOG_FILE`, etc.
- **Configuration Files**: Use the NLM configuration system
- **Command Line Arguments**: `--log-level`, `--log-file`, etc.
- **Runtime Configuration**: Use the Logger API to configure at runtime

## Conclusion

The NLM enterprise logging and error handling system provides a comprehensive, production-ready platform for NLM applications. It combines advanced error handling, structured logging, performance monitoring, and real-time analytics into a unified system that can be easily extended and customized for specific requirements.

The system maintains backward compatibility while providing powerful new features for modern, distributed applications. It is designed to be secure, efficient, and compliant with industry standards while remaining flexible enough to adapt to the evolving needs of NLM research and production systems.
