# NLM Error Handling and Robustness Documentation

## Overview

NLM features a comprehensive error handling system that provides robust error management, validation, recovery, and reporting capabilities across all components. The system is designed to help developers write more reliable and resilient code while maintaining performance and ease of use.

## Error Handling Architecture

### Core Error System

```mermaid
graph TD
    A[NLM Error] --> A1[Specific Error Types]
    A1 --> A2[ConfigError]
    A1 --> A3[MemoryError]
    A1 --> A4[BrainError]
    A1 --> A5[LoggerError]
    A1 --> A6[WorldError]
    A1 --> A7[ValidationError]
    
    A --> A8[ErrorContext Management]
    A --> A9[Error Reporting]
    A --> A10[Error Recovery]
    A --> A11[Input Validation]
    
    A8 --> A12[Thread-Safe Context Tracking]
    A9 --> A13[ErrorReporter Interface]
    A9 --> A14[Error Monitor]
    A9 --> A15[Health Monitoring]
    A10 --> A16[Automatic Recovery]
    A10 --> A17[Retry Logic]
    A11 --> A18[Comprehensive Validation]
end
```

### Error Hierarchy

The error handling system provides a hierarchical exception structure:

```cpp
// Base error class
class NLMError : public std::exception {
public:
    std::string message_;
    std::string context_;
    int code_;
    std::string sourceFile_;
    int sourceLine_;
    std::chrono::system_clock::time_point timestamp_;
    
    NLMError(const std::string& message, const std::string& context = "", 
             int code = 0, const std::string& file = "", int line = 0);
};

// Specific error types
class ConfigError : public NLMError {
    enum class Code {
        InvalidKey = 1000,
        MissingRequiredValue = 1001,
        TypeMismatch = 1002,
        FileNotFound = 1003,
        ParseError = 1004,
        ValidationFailed = 1005
    };
};

class MemoryError : public NLMError {
    enum class Code {
        AllocationFailed = 2000,
        InvalidPointer = 2001,
        BufferOverflow = 2002,
        ResourceExhausted = 2003
    };
};

class BrainError : public NLMError {
    enum class Code {
        InitializationFailed = 3000,
        SimulationError = 3001,
        InvalidConfiguration = 3002,
        MemorySystemError = 3003,
        IntegrationError = 3004
    };
};

class LoggerError : public NLMError {
    enum class Code {
        WriteFailed = 4000,
        FileError = 4001,
        InitializationFailed = 4002
    };
};

class WorldError : public NLMError {
    enum class Code {
        BoundaryViolation = 5000,
        CollisionError = 5001,
        InvalidState = 5002
    };
};

class ValidationError : public NLMError {
    enum class Code {
        InvalidInput = 6000,
        RangeViolation = 6001,
        MissingDependency = 6002,
        CrossValidationFailed = 6003
    };
};
```

## Error Handling Principles

### 1. Fail Fast with Detailed Information

Never silently fail. Always throw informative exceptions with:

- **Clear messages**: What went wrong
- **Context**: Where it happened
- **Codes**: Why it failed
- **Location**: Source file and line number
- **Timestamp**: When it occurred

```cpp
// ✅ Good
void criticalOperation() {
    if (someCondition) {
        throw ConfigError("Configuration invalid", "criticalOperation", ConfigError::Code::ValidationFailed);
    }
}

// ❌ Bad
void criticalOperation() {
    if (someCondition) {
        // Silent failure - bad!
    }
}
```

### 2. Validate Inputs Before Use

Validate all inputs before using them in your code:

```cpp
bool processInput(const std::string& input) {
    // Validate input before processing
    if (!InputValidator::isNonEmpty("input", input)) {
        throw ValidationError("Input cannot be empty", "processInput");
    }
    
    if (!InputValidator::matchesPattern("input", input, "^[a-zA-Z0-9_]+$")) {
        throw ValidationError("Input contains invalid characters", "processInput");
    }
    
    // ... process input
    return true;
}
```

### 3. Use Try-Catch Blocks

Always wrap operations that can fail in try-catch blocks:

```cpp
void riskyOperation() {
    try {
        // Operation that might throw
        performRiskyAction();
    } catch (const ConfigError& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        // Handle configuration error
    } catch (const MemoryError& e) {
        std::cerr << "Memory error: " << e.what() << std::endl;
        // Handle memory error
    } catch (const NLMError& e) {
        std::cerr << "NLM error: " << e.what() << std::endl;
        // Handle other NLM errors
    }
}
```

### 4. Implement Error Recovery for Non-Critical Failures

For non-critical failures, implement automatic recovery:

```cpp
void networkOperation() {
    try {
        connectToNetwork();
    } catch (const NLMError& e) {
        std::cerr << "Network error: " << e.what() << std::endl;
        
        // Attempt recovery
        if (ErrorRecovery::attemptRecovery("networkOperation", 3.0f)) {
            std::cout << "Recovered from network error" << std::endl;
            return;
        }
        
        // If recovery fails, throw or handle appropriately
        throw;
    }
}
```

### 5. Use Error Context for Debugging

Track operation context to help debug errors:

```cpp
void complexOperation() {
    ErrorContext::getInstance().pushContext("DatabaseConnection");
    ErrorContext::getInstance().pushContext("QueryExecution");
    
    try {
        executeDatabaseQuery("SELECT * FROM table");
    } catch (const NLMError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Context: " << e.getContext() << std::endl;
    }
    
    finally {
        ErrorContext::getInstance().popContext();
        ErrorContext::getInstance().popContext();
    }
}
```

## Error Reporting Mechanisms

### Error Reporting Interface

```cpp
class ErrorReporter {
public:
    virtual ~ErrorReporter() = default;
    
    virtual void reportError(const NLMError& error) = 0;
    virtual void reportWarning(const std::string& message, const std::string& context = "") = 0;
};
```

### Global Error Reporter

```cpp
// Set global error reporter
void setErrorReporter(std::unique_ptr<ErrorReporter> reporter);

// Get global error reporter
ErrorReporter& getErrorReporter();
```

### Error Types and Levels

```cpp
enum ErrorLevel {
    Debug,      // Debug information
    Info,       // General information
    Warning,    // Warning that might indicate a problem
    Error,      // Error that prevented an operation
    Critical,   // Critical error that might crash the system
    Fatal       // Fatal error that requires immediate attention
};
```

### Error Context Management

```cpp
// Push context onto stack
ErrorContext::getInstance().pushContext("OperationName");

// Get current context
std::string context = ErrorContext::getInstance().getCurrentContext();

// Clear all contexts
ErrorContext::getInstance().clear();
```

## Error Recovery Strategies

### Automatic Recovery

```cpp
class ErrorRecovery {
public:
    // Attempt automatic recovery with retries
    static bool attemptRecovery(const std::string& operation, float maxRetries = 3.0f);
    
    // Set retry delay between attempts
    static void setRetryDelay(double seconds);
    
    // Check if operation is critical (should not auto-recover)
    static bool isOperationCritical(const std::string& operation);
};
```

### Recovery Examples

```cpp
void networkService() {
    try {
        connectToDatabase();
    } catch (const NLMError& e) {
        std::cerr << "Database connection failed: " << e.what() << std::endl;
        
        // Attempt recovery
        if (ErrorRecovery::attemptRecovery("database_connection", 5.0f)) {
            std::cout << "Successfully reconnected to database" << std::endl;
            return;
        }
        
        // Recovery failed
        std::cerr << "Failed to recover from database connection error" << std::endl;
        throw; // Re-throw the error
    }
}

void fileOperation() {
    try {
        writeToFile("data.txt", "important data");
    } catch (const NLMError& e) {
        std::cerr << "File write failed: " << e.what() << std::endl;
        
        // Attempt to write to backup location
        if (ErrorRecovery::attemptRecovery("file_write_backup", 2.0f)) {
            std::cout << "Successfully wrote to backup file" << std::endl;
            return;
        }
        
        throw;
    }
}
```

## Debugging Techniques

### 1. Use Error Context

Always use error context to track where errors occur:

```cpp
void performComplexTask() {
    ErrorContext::getInstance().pushContext("TaskStart");
    ErrorContext::getInstance().pushContext("DataLoading");
    
    try {
        loadData("file.txt");
    } catch (const NLMError& e) {
        std::cerr << "Error loading data: " << e.what() << std::endl;
        std::cerr << "Context: " << e.getContext() << std::endl;
    }
    
    ErrorContext::getInstance().popContext();
    ErrorContext::getInstance().popContext();
}
```

### 2. Log Errors

Always log errors for debugging:

```cpp
void riskyFunction() {
    try {
        doSomethingRisky();
    } catch (const NLMError& e) {
        // Log the error
        logError(e);
        
        // Also throw to stop execution
        throw;
    }
}
```

### 3. Use Safe Pointers

Use safe pointers to prevent null pointer dereferences:

```cpp
class SafePtr {
public:
    SafePtr(T* ptr = nullptr, const std::string& operation = "")
        : ptr_(ptr), operation_(operation) {}
    
    T* operator->() const {
        if (!ptr_) {
            throw NLMError("Null pointer access", operation_ + " -> SafePtr dereference");
        }
        return ptr_;
    }
    
    T& operator*() const {
        if (!ptr_) {
            throw NLMError("Null pointer dereference", operation_ + " -> SafePtr dereference");
        }
        return *ptr_;
    }
    
    operator bool() const { return ptr_ != nullptr; }
};
```

### 4. Memory Safety

Use safe memory allocation/deallocation:

```cpp
void* safe_malloc(size_t size, const std::string& context = "") {
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw MemoryError("Memory allocation failed", context);
    }
    return ptr;
}

template<typename T, typename... Args>
T* safe_new(Args&&... args) {
    void* ptr = std::malloc(sizeof(T));
    if (!ptr) {
        throw MemoryError("Memory allocation failed", "safe_new");
    }
    try {
        return new(ptr) T(std::forward<Args>(args)...);
    } catch (...) {
        std::free(ptr);
        throw;
    }
}
```

## Input Validation

### Comprehensive Input Validation

```cpp
class InputValidator {
public:
    // Basic validation
    static bool isPositive(const std::string& key, float value);
    static bool isWithinRange(const std::string& key, float value, float min, float max);
    static bool isNonEmpty(const std::string& key, const std::string& value);
    static bool isValidIndex(const std::string& key, size_t index, size_t maxSize);
    static bool isFinite(const std::string& key, double value);
    
    // Pattern validation
    static bool matchesPattern(const std::string& key, const std::string& value, 
                               const std::string& pattern);
    
    // Neural system specific validation
    static bool validateNeuronCount(const std::string& key, size_t count);
    static bool validateRegionCount(const std::string& key, size_t count);
    static bool validateConnectionProbability(const std::string& key, float prob);
    static bool validateTimestep(const std::string& key, float timestep);
    
    // Validation with detailed error messages
    static ValidationResult validate(const std::string& key, const ConfigValue& value,
                                     const std::string& validationRule = "");
};
```

### Validation Examples

```cpp
bool validateConfiguration(const std::shared_ptr<Configuration>& config) {
    bool allValid = true;
    
    // Validate neuron count
    if (config->has("brain.neuron_count")) {
        int64_t neuronCount = config->getTyped<int64_t>("brain.neuron_count");
        if (!InputValidator::validateNeuronCount("neuron_count", neuronCount)) {
            throw ValidationError("Invalid neuron count", "validateConfiguration",
                                ValidationError::Code::RangeViolation);
        }
    }
    
    // Validate connection probability
    if (config->has("brain.connection_probability")) {
        double prob = config->getTyped<double>("brain.connection_probability");
        if (!InputValidator::isWithinRange("connection_probability", prob, 0.0, 1.0)) {
            throw ValidationError("Connection probability out of range", "validateConfiguration",
                                ValidationError::Code::RangeViolation);
        }
    }
    
    // Validate timestep
    if (config->has("simulation.timestep")) {
        double timestep = config->getTyped<double>("simulation.timestep");
        if (!InputValidator::isWithinRange("timestep", timestep, 0.0001, 0.1)) {
            throw ValidationError("Timestep out of valid range", "validateConfiguration",
                                ValidationError::Code::RangeViolation);
        }
    }
    
    return allValid;
}
```

## Thread Safety

### Thread-Safe Error Handling

```cpp
class ThreadSafeErrorHandler {
public:
    static void setThreadSafetyEnabled(bool enabled);
    static bool isThreadSafetyEnabled();
    static void lockForErrorHandling();
    static void unlockFromErrorHandling();
    static std::mutex& getErrorMutex();
};
```

### Thread-Safe Error Context

```cpp
// Error context is thread-safe
ErrorContext::getInstance().pushContext("Operation");
// Can be called from multiple threads safely
```

### Thread-Safe Error Reporting

```cpp
// Error reporting is thread-safe
nlm::getErrorReporter().reportError(someError);
nlm::getErrorReporter().reportWarning("Warning message");
```

## Performance Considerations

### Minimal Overhead

Error reporting is conditional and can be disabled for performance:

```cpp
// Error reporting can be disabled
void disableErrorReporting() {
    setErrorReporter(nullptr);
}

// Error reporting is conditional
void reportErrorIfEnabled(const NLMError& error) {
    if (isErrorReportingEnabled()) {
        nlm::getErrorReporter().reportError(error);
    }
}
```

### Efficient Exception Handling

Exception handling is designed for performance:

```cpp
// Use specific exception types for better performance
void specificOperation() {
    try {
        // Perform operation
        doWork();
    } catch (const ConfigError& e) {
        // Handle config-specific error
    } catch (const MemoryError& e) {
        // Handle memory-specific error
    } catch (const NLMError& e) {
        // Handle other NLM errors
    }
}
```

## Testing and Validation

### Error Handling Tests

```bash
# Run error handling tests
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

### Test Example

```cpp
// TestErrorHandling.cpp - Example test structure
class TestErrorHandling {
public:
    bool runAllTests();
    
    bool testConfigErrorHandling();
    bool testMemoryErrorHandling();
    bool testBrainErrorHandling();
    bool testLoggerErrorHandling();
    bool testValidationErrorHandling();
    bool testErrorReporting();
    bool testErrorRecovery();
    bool testThreadSafety();
    bool testPerformance();
};
```

## Integration with Existing Code

### Brain Error Handling

The Brain class has comprehensive error handling:

```cpp
class Brain {
public:
    bool initialize();                    // Can throw BrainError
    void step(SimulationStep currentStep); // Can throw BrainError
    void receiveSensoryInput(const SensoryInput& input); // Can throw BrainError
    bool save(const std::string& filepath) const; // Can throw BrainError
    bool load(const std::string& filepath);       // Can throw BrainError
};
```

### Configuration Error Handling

The Configuration system has comprehensive error handling:

```cpp
// Configuration error handling
auto config = nlm::createConfiguration();

try {
    if (!config->loadFromFile("config.json")) {
        throw ConfigError("Failed to load configuration", "main");
    }
    
    auto builder = config->getBuilder();
    // ... configure
    
    auto finalConfig = builder->build();
    
} catch (const ConfigError& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    return 1;
}
```

### Logger Error Handling

```cpp
// Logger error handling
class Logger {
public:
    void info(const std::string& message, const std::string& file = "", 
              int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
};
```

## Best Practices

### 1. Always Validate Inputs

- Validate all function parameters
- Validate configuration values
- Validate file contents
- Validate network data

### 2. Use Specific Exception Types

- Use `ConfigError` for configuration issues
- Use `MemoryError` for memory problems
- Use `BrainError` for brain-specific issues
- Use `LoggerError` for logging issues
- Use `WorldError` for world/environment issues
- Use `ValidationError` for validation problems

### 3. Provide Meaningful Error Messages

Error messages should include:

- What went wrong
- Where it happened
- Why it happened
- How to fix it (if possible)

```cpp
throw ConfigError("Neuron count (500) is below minimum allowed (1000)",
                 "Brain::initialize", ConfigError::Code::InvalidParameter);
```

### 4. Implement Error Recovery

Implement recovery for non-critical failures:

- Retry failed operations
- Use fallback values
- Graceful degradation
- Circuit breakers

### 5. Test Error Handling

Test all error paths:

- Normal operation
- Missing input
- Invalid input
- Network failures
- File system errors
- Memory exhaustion
- Configuration errors
- Invalid state

### 6. Log Errors

Always log errors for debugging:

- Include error context
- Include timestamps
- Include stack traces (if available)
- Include system state

### 7. Graceful Degradation

Implement graceful degradation for partial failures:

```cpp
void criticalService() {
    try {
        primaryService();
    } catch (const NLMError& e) {
        logError(e);
        
        if (enableGracefulDegradation()) {
            fallbackService();
        } else {
            throw; // Re-throw if graceful degradation is disabled
        }
    }
}
```

### 8. Monitor Error Rates

Monitor error rates to detect issues:

```cpp
class ErrorMonitor {
public:
    void recordError(const NLMError& error);
    float getErrorRate(const std::string& operation);
    bool isErrorRateHigh(const std::string& operation, float threshold);
};
```

## Common Error Scenarios and Solutions

### 1. Configuration File Not Found

**Problem**: Configuration file cannot be loaded.

**Solution**:

```cpp
auto config = nlm::createConfiguration();

try {
    if (!config->loadFromFile("config.json")) {
        // Try alternative location
        if (!config->loadFromFile("config/default.json")) {
            // Use default configuration
            config->set("neuron_count", 1000);
        }
    }
} catch (const ConfigError& e) {
    // Handle configuration error
}
```

### 2. Invalid Configuration Values

**Problem**: Configuration values are invalid.

**Solution**:

```cpp
auto config = nlm::createConfiguration();
config->loadFromFile("config.json");

try {
    // Validate configuration
    auto validationResult = config->validate();
    if (!validationResult.valid) {
        throw ValidationError("Configuration validation failed: " + validationResult.errorMessage);
    }
    
    // Use validated values
    int64_t neuronCount = config->getTyped<int64_t>("neuron_count");
    
} catch (const ValidationError& e) {
    // Handle validation error
}
```

### 3. Memory Allocation Failure

**Problem**: Memory cannot be allocated.

**Solution**:

```cpp
void allocateMemory(size_t size) {
    try {
        void* ptr = safe_malloc(size, "allocateMemory");
        // Use allocated memory
        safe_free(ptr, "allocateMemory");
    } catch (const MemoryError& e) {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
        
        // Attempt recovery
        if (ErrorRecovery::attemptRecovery("memory_allocation", 2.0f)) {
            std::cout << "Successfully allocated memory after retry" << std::endl;
            return;
        }
        
        throw; // Re-throw if recovery fails
    }
}
```

### 4. File System Errors

**Problem**: File system operations fail.

**Solution**:

```cpp
void writeToFile(const std::string& filename, const std::string& content) {
    try {
        // Attempt to write to primary location
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw LoggerError("Cannot open file for writing", "writeToFile");
        }
        file << content;
        
    } catch (const LoggerError& e) {
        std::cerr << "File write failed: " << e.what() << std::endl;
        
        // Attempt to write to alternative location
        std::string backupFilename = "/tmp/" + filename;
        std::ofstream backupFile(backupFilename);
        if (backupFile.is_open()) {
            backupFile << content;
            std::cout << "Successfully wrote to backup file" << std::endl;
            return;
        }
        
        throw; // Re-throw if backup fails
    }
}
```

### 5. Network Communication Failures

**Problem**: Network operations fail.

**Solution**:

```cpp
void communicateWithServer(const std::string& host, int port) {
    try {
        connectToServer(host, port);
        sendData("Hello, Server!");
        receiveResponse();
        
    } catch (const NLMError& e) {
        std::cerr << "Network communication failed: " << e.what() << std::endl;
        
        // Attempt recovery with exponential backoff
        if (ErrorRecovery::attemptRecovery("network_communication", 5.0f)) {
            std::cout << "Successfully recovered network communication" << std::endl;
            return;
        }
        
        throw; // Re-throw if recovery fails
    }
}
```

## Error Handling Macros

### Thread-Safe Error Handling Macros

```cpp
#define NLM_THROW_ERROR(type, message, context, code) \
    do { \
        std::string ctx = nlm::ErrorContext::getInstance().getCurrentContext(); \
        if (!ctx.empty()) ctx = ctx + " -> " + context; else ctx = context; \
        nlm::getErrorReporter().reportError(type(message, ctx, code, __FILE__, __LINE__)); \
        throw type(message, ctx, code, __FILE__, __LINE__); \
    } while(0)

#define NLM_THROW_CONFIG_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::ConfigError, message, context, code)

#define NLM_THROW_BRAIN_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::BrainError, message, context, code)

#define NLM_THROW_MEMORY_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::MemoryError, message, context, code)

#define NLM_THROW_LOGGER_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::LoggerError, message, context, code)

#define NLM_THROW_WORLD_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::WorldError, message, context, code)

#define NLM_THROW_VALIDATION_ERROR(message, context, code) \
    NLM_THROW_ERROR(nlm::ValidationError, message, context, code)
```

### Logging Macros

```cpp
#define NLM_LOG_ERROR(message) nlm::logError(nlm::NLMError(message))
#define NLM_LOG_WARNING(message) nlm::logWarning(message)
#define NLM_LOG_INFO(message) nlm::getErrorReporter().reportWarning(message)
```

## Error Handling Configuration

### Configure Error Reporting

```cpp
// Configure error reporting
struct ErrorReportingConfig {
    bool enableReporting = true;        // Enable/disable error reporting
    ErrorLevel minLevel = ErrorLevel::Error; // Minimum error level to report
    bool enableContextTracking = true; // Enable error context tracking
    bool enableRecovery = true;        // Enable automatic recovery
    float retryDelay = 1.0f;            // Delay between retry attempts
    int maxRetries = 3;                 // Maximum number of retry attempts
};

void configureErrorReporting(const ErrorReportingConfig& config);
```

### Global Error Handling Configuration

```cpp
void configureGlobalErrorHandling() {
    ErrorReportingConfig config;
    config.enableReporting = true;
    config.minLevel = ErrorLevel::Error;
    config.enableContextTracking = true;
    config.enableRecovery = true;
    config.retryDelay = 1.0f;
    config.maxRetries = 3;
    
    configureErrorReporting(config);
}
```

## Error Handling in Different Components

### Brain Error Handling

```cpp
class Brain {
public:
    bool initialize() {
        try {
            // Initialize brain components
            initializeNeurons();
            initializeSynapses();
            initializePlasticity();
            
            // Validate configuration
            if (!validateConfiguration()) {
                throw BrainError("Invalid configuration for brain initialization", 
                               "Brain::initialize", BrainError::Code::InvalidConfiguration);
            }
            
            return true;
            
        } catch (const BrainError& e) {
            // Handle brain error
            std::cerr << "Brain initialization failed: " << e.what() << std::endl;
            throw;
        }
    }
};
```

### Configuration Error Handling

```cpp
class Configuration {
public:
    bool loadFromFile(const std::string& filepath) {
        try {
            InputValidator::isNonEmpty("filepath", filepath);
            
            std::ifstream file(filepath);
            if (!file.is_open()) {
                throw ConfigError("Configuration file not found", "loadFromFile",
                                ConfigError::Code::FileNotFound);
            }
            
            // Parse file content
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            // Load configuration
            return loadFromString(content);
            
        } catch (const ConfigError& e) {
            // Handle configuration error
            std::cerr << "Configuration loading failed: " << e.what() << std::endl;
            throw;
        }
    }
};
```

### Logger Error Handling

```cpp
class Logger {
public:
    void log(const std::string& message, LogLevel level = LogLevel::Info) {
        try {
            // Ensure log directory exists
            ensureLogDirectoryExists();
            
            // Write log message
            writeLogMessage(message, level);
            
        } catch (const LoggerError& e) {
            // Handle logging error
            std::cerr << "Logging failed: " << e.what() << std::endl;
            
            // Attempt recovery
            if (ErrorRecovery::attemptRecovery("logging", 0.5f)) {
                std::cout << "Successfully logged message after recovery" << std::endl;
                return;
            }
            
            throw; // Re-throw if recovery fails
        }
    }
};
```

## Error Handling Testing

### Unit Tests

```cpp
// TestErrorHandling.cpp - Example unit test
class TestErrorHandling {
public:
    bool runAllTests() {
        bool allPassed = true;
        
        allPassed &= testConfigErrorHandling();
        allPassed &= testMemoryErrorHandling();
        allPassed &= testBrainErrorHandling();
        allPassed &= testLoggerErrorHandling();
        allPassed &= testValidationErrorHandling();
        allPassed &= testErrorReporting();
        allPassed &= testErrorRecovery();
        allPassed &= testThreadSafety();
        allPassed &= testPerformance();
        
        return allPassed;
    }
    
    bool testConfigErrorHandling() {
        try {
            auto config = nlm::createConfiguration();
            
            // Test missing file
            if (config->loadFromFile("nonexistent.txt")) {
                return false; // Should have thrown
            }
            
            // Test invalid configuration
            config->set("invalid_key", "invalid_value");
            
            return true;
            
        } catch (...) {
            return false; // Unexpected exception
        }
    }
};
```

### Integration Tests

```bash
# Run integration tests
./test/nlm_integration_test --config=integration_config.json

# Test error handling in integration
./test/nlm_integration_test --test=error_handling
```

## Conclusion

The NLM error handling system provides comprehensive error handling, validation, recovery, and reporting capabilities. It enables robust error handling across all components while maintaining performance and ease of use.

Key benefits:

1. **Improved Reliability**: Better error handling prevents crashes and data corruption
2. **Enhanced Debugging**: Detailed error messages and context aid in debugging
3. **Better User Experience**: Graceful error recovery and informative messages
4. **Easier Maintenance**: Centralized error handling and reporting
5. **Performance**: Efficient error handling with minimal overhead
6. **Thread Safety**: Thread-safe operations prevent race conditions
7. **Extensibility**: Easy to add new error types and handlers
8. **Testability**: Comprehensive testing ensures error handling works correctly

The error handling system is production-ready and can be used immediately in any NLM project. It follows best practices for error handling and provides a solid foundation for building reliable and robust NLM applications.