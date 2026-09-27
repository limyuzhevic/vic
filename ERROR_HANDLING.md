# Error Handling and Robustness Improvements

This document describes the error handling and robustness improvements implemented in the NLM framework. These improvements enhance the reliability and maintainability of the codebase by providing comprehensive error detection, handling, and recovery mechanisms.

## Overview

The NLM framework now includes a robust error handling system that provides:

- **Comprehensive error detection** across all components
- **Type-safe error handling** with detailed error messages
- **Graceful error recovery** where possible
- **Detailed logging and diagnostics**
- **Thread-safe error reporting**
- **Performance considerations for error handling**
- **Consistent error interface across all components**

## Error Handling Architecture

### 1. Error Classification

Errors are classified into the following categories:

```cpp
enum class ErrorCategory {
    // Internal errors
    Internal = 0,           // Programming errors, logic bugs
    Memory = 1,             // Memory allocation/deallocation issues
    FileSystem = 2,         // File I/O operations
    Network = 3,            // Network communication issues
    Configuration = 4,      // Configuration errors
    Plugin = 5,             // Plugin loading/execution errors
    Sensor = 6,             // Sensor-specific errors
    NeuralNetwork = 7,      // Neural network computation errors
    World = 8,              // World simulation errors
    Developmental = 9,      // Development system errors
    Neuromodulation = 10,   // Neuromodulation errors
    Performance = 11,       // Performance-related errors
    UserInput = 12,         // Invalid user input
    Timeout = 13,           // Operation timeouts
    ResourceLimit = 14,     // Resource limit exceeded
    
    // External errors
    External = 100,         // External system failures
    Dependency = 101,       // Missing dependencies
    Compatibility = 102,    // Version/compatibility issues
    Permission = 103,       // Access permission issues
};
```

### 2. Error Severity Levels

```cpp
enum class ErrorSeverity {
    Debug = 0,             // Debug information, not an error
    Info = 1,              // Informational messages
    Warning = 2,           // Non-critical issues that may cause problems
    Error = 3,             // Critical errors requiring immediate attention
    Critical = 4,          // Severe errors that may crash the application
    Fatal = 5,             // Unrecoverable errors that crash the application
};
```

### 3. Error Structure

```cpp
struct NLMError {
    // Error identification
    ErrorCategory category;          //!< Error category
    ErrorSeverity severity;          //!< Error severity level
    std::string code;                //!< Error code (e.g., "E001", "N001")
    std::string message;             //!< Human-readable error message
    
    // Context information
    std::string component;           //!< Component where error occurred
    std::string function;            //!< Function where error occurred
    int line;                        //!< Line number in source code
    std::string file;                //!< File where error occurred
    
    // Error details
    std::unordered_map<std::string, std::string> details; //!< Additional error details
    std::vector<std::string> context; //!< Context information for debugging
    
    // System information
    std::string timestamp;           //!< Error occurrence time
    std::string threadId;            //!< ID of thread that reported the error
    
    // Recovery information
    bool recoverable;                //!< Whether error can be recovered from
    std::vector<std::string> recoveryActions; //!< Possible recovery actions
    
    // Original exception
    std::exception_ptr originalException; //!< Original exception if available
    
    NLMError() : category(ErrorCategory::Internal), severity(ErrorSeverity::Error),
                line(0), recoverable(false) {}
    
    NLMError(ErrorCategory cat, ErrorSeverity sev, const std::string& msg,
             const std::string& comp = "", const std::string& func = "",
             int ln = 0, const std::string& file = "")
        : category(cat), severity(sev), message(msg), component(comp),
          function(func), line(ln), file(file), recoverable(false) {}
};
```

## Error Handling Classes

### 1. Error Handler Interface

```cpp
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    // Error reporting
    virtual void handleError(const NLMError& error) = 0;
    virtual void handleWarning(const NLMError& error) = 0;
    virtual void handleInfo(const NLMError& error) = 0;
    virtual void handleDebug(const NLMError& error) = 0;
    
    // Configuration
    virtual void setErrorLevel(ErrorSeverity level) = 0;
    virtual ErrorSeverity getErrorLevel() const = 0;
    virtual void setMaxErrors(size_t max) = 0;
    virtual size_t getMaxErrors() const = 0;
    
    // Filtering
    virtual void addErrorFilter(const std::string& component) = 0;
    virtual void removeErrorFilter(const std::string& component) = 0;
    virtual bool shouldHandle(const NLMError& error) const = 0;
    
    // Statistics
    virtual size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const = 0;
    virtual std::chrono::system_clock::time_point getLastErrorTime() const = 0;
};
```

### 2. Error Reporting Handler

```cpp
class ConsoleErrorHandler : public IErrorHandler {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ConsoleErrorHandler();
    explicit ConsoleErrorHandler(ErrorSeverity level);
    ~ConsoleErrorHandler() override;
    
    void handleError(const NLMError& error) override;
    void handleWarning(const NLMError& error) override;
    void handleInfo(const NLMError& error) override;
    void handleDebug(const NLMError& error) override;
    
    void setErrorLevel(ErrorSeverity level) override;
    ErrorSeverity getErrorLevel() const override;
    void setMaxErrors(size_t max) override;
    size_t getMaxErrors() const override;
    
    void addErrorFilter(const std::string& component) override;
    void removeErrorFilter(const std::string& component) override;
    bool shouldHandle(const NLMError& error) const override;
    
    size_t getErrorCount(ErrorSeverity severity = ErrorSeverity::Error) const override;
    std::chrono::system_clock::time_point getLastErrorTime() const override;
    
    // Additional methods
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Error formatting
    std::string formatError(const NLMError& error) const;
    std::string formatErrorDetails(const NLMError& error) const;
    std::string formatErrorContext(const NLMError& error) const;
};
```

### 3. Error Recovery Handler

```cpp
class ErrorRecoveryHandler {
public:
    ErrorRecoveryHandler();
    ~ErrorRecoveryHandler();
    
    // Error recovery registration
    template<typename ErrorType, typename RecoveryFunc>
    void registerRecoveryHandler(ErrorType errorType, RecoveryFunc recoveryFunc);
    
    // Error recovery execution
    bool attemptRecovery(const NLMError& error);
    bool attemptRecovery(ErrorCategory category, ErrorSeverity severity);
    
    // Recovery configuration
    void setAutoRecovery(bool enabled);
    bool isAutoRecoveryEnabled() const;
    void setMaxRecoveryAttempts(size_t max);
    size_t getMaxRecoveryAttempts() const;
    
    // Recovery statistics
    size_t getRecoveryCount() const;
    size_t getRecoverySuccessCount() const;
    double getRecoverySuccessRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### 4. Error Monitoring Handler

```cpp
class ErrorMonitoringHandler {
public:
    ErrorMonitoringHandler();
    ~ErrorMonitoringHandler();
    
    // Monitoring configuration
    void setMonitoringInterval(std::chrono::seconds interval);
    void setAlertThreshold(size_t threshold);
    void setAlertEmail(const std::string& email);
    
    // Monitoring statistics
    struct ErrorStats {
        size_t totalErrors;
        size_t errorsByCategory[13];          // By ErrorCategory
        size_t errorsBySeverity[6];            // By ErrorSeverity
        size_t errorsByComponent[std::numeric_limits<size_t>::max()]; // By component
        std::chrono::system_clock::time_point lastErrorTime;
        std::chrono::system_clock::time_point firstErrorTime;
        double errorRatePerSecond;
        
        ErrorStats() : totalErrors(0), lastErrorTime(), firstErrorTime() {
            std::fill(std::begin(errorsByCategory), std::end(errorsByCategory), 0);
            std::fill(std::begin(errorsBySeverity), std::end(errorsBySeverity), 0);
        }
    };
    
    ErrorStats getErrorStatistics() const;
    std::vector<NLMError> getRecentErrors(size_t count = 100) const;
    
    // Error trends
    std::vector<double> getErrorTrend(size_t windowSize = 100) const;
    std::unordered_map<std::string, double> getComponentErrorRates() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Error Handling Integration

### 1. Global Error Handler

```cpp
class GlobalErrorHandler {
public:
    static GlobalErrorHandler& getInstance();
    
    // Error handler management
    void setErrorHandler(std::unique_ptr<IErrorHandler> handler);
    IErrorHandler* getErrorHandler();
    
    // Error reporting
    void reportError(const NLMError& error);
    void reportWarning(const std::string& message, const std::string& component = "");
    void reportInfo(const std::string& message, const std::string& component = "");
    void reportDebug(const std::string& message, const std::string& component = "");
    
    // Error recovery
    void setRecoveryHandler(std::unique_ptr<ErrorRecoveryHandler> recoveryHandler);
    bool attemptRecovery(const NLMError& error);
    
    // Monitoring
    void setMonitoringHandler(std::unique_ptr<ErrorMonitoringHandler> monitoringHandler);
    ErrorMonitoringHandler::ErrorStats getErrorStatistics() const;
    
    // Configuration
    void initialize(const Config& config);
    void shutdown();
    
    // Error filtering
    void addGlobalFilter(const std::string& component);
    void removeGlobalFilter(const std::string& component);
    
private:
    GlobalErrorHandler();
    ~GlobalErrorHandler();
    GlobalErrorHandler(const GlobalErrorHandler&) = delete;
    GlobalErrorHandler& operator=(const GlobalErrorHandler&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### 2. Macro-Based Error Reporting

```cpp
// Basic error reporting macro
#define NLM_THROW_ERROR(category, severity, message, component) \
    nlm::GlobalErrorHandler::getInstance().reportError( \
        nlm::NLMError( \
            category, severity, message, component, __func__, __LINE__, __FILE__ \
        ) \
    )

// Error reporting with recovery attempt
#define NLM_ERROR(category, severity, message, component) \
    do { \
        nlm::NLMError error(category, severity, message, component, __func__, __LINE__, __FILE__); \
        nlm::GlobalErrorHandler::getInstance().reportError(error); \
        if (nlm::GlobalErrorHandler::getInstance().attemptRecovery(error)) { \
            break; \
        } \
    } while(0)

// Warning macro
#define NLM_WARNING(severity, message, component) \
    nlm::GlobalErrorHandler::getInstance().reportWarning( \
        nlm::NLMError( \
            ErrorCategory::Internal, severity, message, component, __func__, __LINE__, __FILE__ \
        ) \
    )

// Exception wrapper for C++ exceptions
#define NLM_TRY_CATCH(component) \
    try { \

#define NLM_CATCH_ALL \
    } catch (const std::exception& e) { \
        nlm::NLMError error(ErrorCategory::Internal, ErrorSeverity::Error, \
                          std::string("Exception: ") + e.what(), component, __func__, __LINE__, __FILE__); \
        error.originalException = std::current_exception(); \
        nlm::GlobalErrorHandler::getInstance().reportError(error); \
    } \

// Recovery wrapper
#define NLM_RECOVERABLE_ERROR(category, severity, message, component, recoveryFunc) \
    do { \
        nlm::NLMError error(category, severity, message, component, __func__, __LINE__, __FILE__); \
        if (nlm::GlobalErrorHandler::getInstance().attemptRecovery(error)) { \
            break; \
        } \
        recoveryFunc(); \
    } while(0)
```

### 3. Component-Specific Error Handling

```cpp
// Example component error handling
class SensorComponent {
public:
    SensorComponent() : errorCount_(0) {}
    
    bool readSensorData() {
        try {
            // Sensor reading logic
            if (!sensorInitialized_) {
                NLM_ERROR(ErrorCategory::Sensor, ErrorSeverity::Warning, \
                         "Sensor not initialized", "SensorComponent");
                return false;
            }
            
            if (sensorFailed_) {
                NLM_ERROR(ErrorCategory::Sensor, ErrorSeverity::Error, \
                         "Sensor hardware failure", "SensorComponent");
                return false;
            }
            
            // Read sensor data
            auto data = readData();
            
            // Validate data
            if (!validateData(data)) {
                NLM_ERROR(ErrorCategory::Sensor, ErrorSeverity::Warning, \
                         "Sensor data validation failed", "SensorComponent");
                return false;
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_ERROR(ErrorCategory::Sensor, ErrorSeverity::Error,
                     std::string("Sensor reading exception: ") + e.what(), "SensorComponent");
            return false;
        }
    }
    
private:
    bool sensorInitialized_;
    bool sensorFailed_;
    size_t errorCount_;
};
```

## Error Handling Best Practices

### 1. Error Handling Guidelines

#### Do's
- Always handle exceptions and report errors
- Use specific error categories and severities
- Provide detailed error messages with context
- Log errors for debugging
- Attempt recovery where possible
- Validate input parameters
- Check resource availability before use
- Use defensive programming practices

#### Don'ts
- Ignore exceptions or errors
- Use generic error messages without context
- Crash the application without proper cleanup
- Report errors in development mode to users
- Suppress errors without understanding them
- Use global error variables
- Assume external resources will always be available

### 2. Performance Considerations

```cpp
// Fast error path (critical for performance)
inline void logErrorDebug(const NLMError& error) {
    // Minimal error handling for debug builds
    if (error.severity == ErrorSeverity::Debug) {
        return;  // Skip debug logging in production
    }
    
    // Fast error reporting
    if (error.severity <= ErrorSeverity::Warning) {
        return;  // Skip non-critical errors
    }
    
    // Log critical errors
    reportCriticalError(error);
}
```

### 3. Testing Error Handling

```cpp
// Error injection for testing
class ErrorInjector {
public:
    static void injectError(ErrorCategory category, ErrorSeverity severity,
                           const std::string& message, const std::string& component) {
        nlm::NLMError error(category, severity, message, component, "", 0, "");
        nlm::GlobalErrorHandler::getInstance().reportError(error);
    }
    
    static void simulateSensorFailure() {
        injectError(ErrorCategory::Sensor, ErrorSeverity::Error,
                   "Sensor simulated failure", "SensorComponent");
    }
    
    static void simulateMemoryError() {
        injectError(ErrorCategory::Memory, ErrorSeverity::Critical,
                   "Memory allocation failure", "Core");
    }
    
    static void reset() {
        // Clear all injected errors
    }
};
```

## Error Handling in Specific Components

### 1. Plugin System Error Handling

```cpp
bool PluginManager::loadPlugin(const std::string& pluginPath) {
    try {
        // Check if plugin is already loaded
        std::string pluginName = extractPluginName(pluginPath);
        if (isPluginLoaded(pluginName)) {
            NLM_WARNING(ErrorSeverity::Warning, std::string("Plugin already loaded: ") + pluginName, "PluginManager");
            return true;
        }
        
        // Load plugin library
        void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!handle) {
            std::string errorMsg = dlerror();
            NLM_ERROR(ErrorCategory::Plugin, ErrorSeverity::Error,
                     std::string("Failed to load plugin library: ") + errorMsg, "PluginManager");
            
            // Attempt to save error for recovery
            return false;
        }
        
        // Get plugin factory function
        auto factory = reinterpret_cast<PluginFactory>(dlsym(handle, "createPlugin"));
        if (!factory) {
            dlclose(handle);
            NLM_ERROR(ErrorCategory::Plugin, ErrorSeverity::Error,
                     std::string("Plugin factory not found: ") + dlerror(), "PluginManager");
            return false;
        }
        
        // Create plugin instance
        auto plugin = factory();
        if (!plugin) {
            dlclose(handle);
            NLM_ERROR(ErrorCategory::Plugin, ErrorSeverity::Error,
                     "Plugin factory returned null", "PluginManager");
            return false;
        }
        
        // Initialize plugin
        if (!plugin->initialize(getPluginConfig(pluginName))) {
            dlclose(handle);
            NLM_ERROR(ErrorCategory::Plugin, ErrorSeverity::Error,
                     "Plugin initialization failed", "PluginManager");
            return false;
        }
        
        // Store plugin
        pluginRegistry_[pluginName] = getPluginMetadata(plugin);
        pluginInstances_[pluginName] = std::make_pair(handle, plugin);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_ERROR(ErrorCategory::Plugin, ErrorSeverity::Critical,
                 std::string("Plugin loading exception: ") + e.what(), "PluginManager");
        return false;
    }
}
```

### 2. Configuration Error Handling

```cpp
bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Validate file path
        if (filepath.empty()) {
            NLM_ERROR(ErrorCategory::Configuration, ErrorSeverity::Error,
                     "Empty file path provided", "Config");
            return false;
        }
        
        // Check if file exists and is readable
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_ERROR(ErrorCategory::Configuration, ErrorSeverity::Error,
                     std::string("Cannot open configuration file: ") + filepath, "Config");
            return false;
        }
        
        // Parse JSON
        std::vector<ConfigEntry> entries;
        if (!parseJSONFile(filepath, entries)) {
            NLM_ERROR(ErrorCategory::Configuration, ErrorSeverity::Error,
                     std::string("Failed to parse JSON file: ") + filepath, "Config");
            return false;
        }
        
        // Apply configuration
        for (const auto& entry : entries) {
            set(entry.key, entry.value, entry.source);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_ERROR(ErrorCategory::Configuration, ErrorSeverity::Error,
                 std::string("Configuration loading exception: ") + e.what(), "Config");
        return false;
    }
}
```

## Error Handling Configuration

### 1. Error Handler Configuration

```ini
[error_handling]
# Global error settings
error_level = "warning"  # debug, info, warning, error, critical, fatal
max_errors = 1000
auto_recovery = true
monitoring_enabled = true

# Error reporting
console_output = true
file_output = "nlm_errors.log"
log_level = "error"
use_colors = true

# Error recovery
max_recovery_attempts = 3
recovery_timeout_seconds = 30

# Error monitoring
alert_threshold = 100
alert_email = "admin@nlm-framework.org"
monitoring_interval_seconds = 60
```

### 2. Component-Specific Error Settings

```ini
[error_handling.components]
# Plugin error settings
plugin.error_level = "error"
plugin.max_errors = 100
plugin.auto_recovery = true

# Sensor error settings
sensor.error_level = "warning"
sensor.max_errors = 500

# Neural network error settings
neural_network.error_level = "error"
neural_network.max_errors = 200

# World simulation error settings
world.error_level = "warning"
world.max_errors = 300

# Configuration error settings
config.error_level = "error"
config.max_errors = 50
```

## Error Handling Tools and Utilities

### 1. Error Reporting Tool

```cpp
class ErrorReporter {
public:
    static void reportError(const NLMError& error);
    static void reportError(ErrorCategory category, ErrorSeverity severity,
                           const std::string& message, const std::string& component = "");
    static void generateErrorReport(const std::string& outputFile);
    static void analyzeErrorPatterns(const std::string& outputFile);
    static void compareErrorStats(const ErrorReporter& otherReporter);
};
```

### 2. Error Recovery Test Tool

```cpp
class ErrorRecoveryTester {
public:
    static void runErrorRecoveryTests();
    static void testRecoveryHandlers();
    static void testErrorScenarios();
    static void validateErrorRecovery();
    static void stressTestErrorHandling();
};
```

### 3. Error Monitoring Dashboard

```cpp
class ErrorMonitoringDashboard {
public:
    void startMonitoring();
    void stopMonitoring();
    void displayErrorStatistics();
    void showErrorTrends();
    void showComponentErrors();
    void showRecentErrors();
    void exportErrorData(const std::string& filename);
    void importErrorData(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Migration Guide

### From Legacy Error Handling to New System

#### 1. Update Existing Error Code

Old code:
```cpp
if (sensorFailed) {
    std::cerr << "Sensor failed!" << std::endl;
    return false;
}
```

New code:
```cpp
if (sensorFailed) {
    NLM_ERROR(ErrorCategory::Sensor, ErrorSeverity::Error,
             "Sensor hardware failure", "SensorComponent");
    return false;
}
```

#### 2. Add Error Handling for Exceptions

Old code:
```cpp
try {
    readSensorData();
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return false;
}
```

New code:
```cpp
try {
    readSensorData();
} catch (const std::exception& e) {
    NLM_ERROR(ErrorCategory::Sensor, ErrorSeverity::Error,
             std::string("Sensor reading exception: ") + e.what(), "SensorComponent");
    return false;
}
```

#### 3. Add Error Recovery

Old code:
```cpp
bool result = readSensorData();
if (!result) {
    // Try to recover
    initializeSensor();
    result = readSensorData();
}
return result;
```

New code:
```cpp
bool result = readSensorData();
if (!result) {
    // Attempt automatic recovery
    if (GlobalErrorHandler::getInstance().attemptRecovery(
        NLMError(ErrorCategory::Sensor, ErrorSeverity::Error,
                "Sensor reading failed", "SensorComponent"))) {
        return true;
    }
    
    // Manual recovery
    initializeSensor();
    result = readSensorData();
}
return result;
```

## Conclusion

The NLM framework now includes a comprehensive error handling system that provides:

1. **Robust error detection** across all components
2. **Type-safe error handling** with detailed error messages
3. **Graceful error recovery** where possible
4. **Detailed logging and diagnostics**
5. **Thread-safe error reporting**
6. **Performance considerations for error handling**
7. **Consistent error interface across all components**

This error handling system significantly improves the reliability, maintainability, and debuggability of the NLM framework, making it more robust for production use and easier to develop with.