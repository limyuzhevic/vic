# Logger API Documentation

## Overview
The Logger component provides comprehensive, thread-safe logging infrastructure for NLM with structured metadata support, log rotation, and validation capabilities.

## Key Features
- **Thread Safety**: Uses mutexes to protect all logging operations
- **Structured Logging**: Support for metadata and structured log entries
- **Log Rotation**: Automatic file rotation based on size and count
- **Validation**: Entry validation with configurable rules
- **Multi-sink Support**: Log to both console and files simultaneously
- **Color Output**: ANSI color support for terminal output
- **Performance**: Minimal overhead for production use

## Usage Examples

### Basic Console Logging
```cpp
#include <nlm/core/Logger.hpp>

int main() {
    // Configure global logger
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    
    // Use different log levels
    nlm::Logger::getGlobal().debug("Debug information for developers");
    nlm::Logger::getGlobal().info("Informational messages");
    nlm::Logger::getGlobal().warning("Warnings about potential issues");
    nlm::Logger::getGlobal().error("Error conditions that need attention");
    nlm::Logger::getGlobal().critical("Critical errors requiring immediate action");
    
    // Log with location information
    nlm::Logger::getGlobal().error("Database connection failed", 
                                   __FILE__, __LINE__, __func__);
    
    return 0;
}
```

### Structured Logging with Metadata
```cpp
#include <nlm/core/Logger.hpp>

int main() {
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    
    // Log with metadata
    std::unordered_map<std::string, std::string> metadata;
    metadata["neuron_id"] = "1234";
    metadata["layer"] = "cortical";
    metadata["weight"] = "0.75";
    
    nlm::Logger::getGlobal().logWithMetadata(
        nlm::LogLevel::Info,
        "Synapse formed between neurons",
        metadata,
        __FILE__, __LINE__, __func__
    );
    
    // Template-based logging for metrics
    nlm::Logger::getGlobal().logValue(nlm::LogLevel::Info, "firing_rate", 42.5);
    nlm::Logger::getGlobal().logValue(nlm::LogLevel::Warning, "loss", 0.85);
    
    // Stream-style logging
    nlm::LogStream debugStream(nlm::LogLevel::Debug, __FILE__, __LINE__, __func__);
    debugStream << "Processing neuron " << 1234 << " in layer " << "cortical";
    debugStream.addMetadata("processing_step", "1");
    debugStream.addMetadata("thread_id", std::to_string(std::hash<std::thread::id>{}()));
    
    return 0;
}
```

### Advanced Logger Configuration
```cpp
#include <nlm/core/Logger.hpp>

int main() {
    // Get logger instance and configure
    auto& logger = nlm::Logger::getGlobal();
    
    // Set up multiple sinks
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setUseColors(true);
    consoleLogger->setOutputFile("console.log");
    
    // File logger with rotation
    auto fileLogger = std::make_shared<nlm::FileLogger>("nlm.log", nlm::LogLevel::Debug);
    fileLogger->enableLogRotation(10 * 1024 * 1024,  // 10MB
                                   5);              // 5 files
    
    // Add loggers to main logger
    logger.addLogger(consoleLogger);
    logger.addLogger(fileLogger);
    
    // Configure validation rules
    auto validator = [](const std::string& key, const nlm::LogLevel& level,
                       std::vector<nlm::LogValidationError>& errors) {
        if (key == "neuron_id" && level >= nlm::LogLevel::Warning) {
            errors.push_back({key, "Neuron ID required for warnings/errors", 
                             "neuron_id provided", "missing"});
        }
        return errors.empty();
    };
    
    logger.addLogValidator(validator);
    
    // Set forbidden patterns (case-insensitive)
    logger.setForbiddenPatterns({"password", "secret", "token"});
    
    // Set required metadata
    std::unordered_map<std::string, std::string> requiredMeta;
    requiredMeta["component"] = "brain";
    requiredMeta["session"] = "default";
    logger.setRequiredMetadata(requiredMeta);
    
    return 0;
}
```

### Log Validation and Filtering
```cpp
#include <nlm/core/Logger.hpp>

int main() {
    auto& logger = nlm::Logger::getGlobal();
    
    // Log entry that will be validated
    nlm::LogEntry entry;
    entry.level = nlm::LogLevel::Error;
    entry.message = "Failed to process neuron";
    entry.file = "brain.cpp";
    entry.line = 123;
    entry.function = "processNeuron";
    
    // Missing required metadata will be detected
    entry.metadata["neuron_id"] = "1234"; // But missing "component"
    
    // Log the entry (validation will occur)
    logger.logWithMetadata(nlm::LogLevel::Error, 
                          "Failed to process neuron",
                          entry.metadata,
                          __FILE__, __LINE__, __func__);
    
    // Check validation results
    if (!logger.validateEntry(entry)) {
        auto errors = logger.getValidationErrors(entry);
        for (const auto& error : errors) {
            std::cout << "Validation error: " << error.message << std::endl;
        }
    }
    
    return 0;
}
```

### Performance-Optimized Logging
```cpp
#include <nlm/core/Logger.hpp>

int main() {
    auto& logger = nlm::Logger::getGlobal();
    
    // Disable logging at runtime for performance
    if (logger.isEnabled(nlm::LogLevel::Debug)) {
        nlm::LogStream debugStream(nlm::LogLevel::Debug, __FILE__, __LINE__, __func__);
        // Only expensive operations here
        debugStream << "Expensive computation result: " << expensiveCalculation();
        debugStream.addMetadata("computation_time", std::to_string(computeTime()));
    }
    
    // Use efficient level checking
    if (nlm::Logger::getGlobal().isEnabled(nlm::LogLevel::Info)) {
        // Less critical but still useful information
        nlm::Logger::getGlobal().info("Step completed");
    }
    
    return 0;
}
```

## Thread Safety Features
The Logger is fully thread-safe:
- All log operations are protected by mutexes
- Concurrent writes from multiple threads are serialized
- Reading recent logs is thread-safe
- Metadata operations are thread-safe
- Validation operations are thread-safe

## Performance Optimizations
1. **Conditional Logging**: Use `isEnabled()` to skip expensive operations
2. **Stream-style Logging**: Reduce string concatenation overhead
3. **Batch Metadata**: Group related metadata into single log entries
4. **Log Levels**: Use appropriate log levels to filter at configuration

## API Reference

### Class: ILogger

#### Pure Virtual Methods
- `virtual void log(const LogEntry& entry) = 0`
- `virtual void setLevel(LogLevel level) = 0`
- `virtual LogLevel getLevel() const = 0`

#### Metadata Methods
- `virtual void setMetadata(const std::string& key, const std::string& value) = 0`
- `virtual std::string getMetadata(const std::string& key) const = 0`
- `virtual void clearMetadata() = 0`
- `virtual size_t getLogCount() const = 0`
- `virtual std::vector<LogEntry> getRecentLogs(size_t limit = 100) const = 0`

#### Validation Methods
- `virtual bool validateEntry(const LogEntry& entry) const = 0`
- `virtual std::vector<LogValidationError> getValidationErrors(const LogEntry& entry) const = 0`

### Class: ConsoleLogger

#### Constructors
- `ConsoleLogger()` - Default constructor (Info level)
- `ConsoleLogger(LogLevel level)` - Constructor with level

#### Core Methods
- `void log(const LogEntry& entry) override`
- `void setLevel(LogLevel level) override`
- `LogLevel getLevel() const override`

#### Configuration Methods
- `void setUseColors(bool useColors)`
- `bool getUseColors() const`
- `void setOutputFile(const std::string& filepath)`
- `void closeOutputFile()`

#### Log Rotation
- `void enableLogRotation(size_t maxFileSize = 10 * 1024 * 1024, size_t maxFiles = 5)`
- `void disableLogRotation()`
- `bool isLogRotationEnabled() const`
- `size_t getLogFileSize() const`

#### Metadata
- `void setMetadata(const std::string& key, const std::string& value) override`
- `std::string getMetadata(const std::string& key) const override`
- `void clearMetadata() override`
- `size_t getLogCount() const override`
- `std::vector<LogEntry> getRecentLogs(size_t limit = 100) const override`

#### Validation
- `bool validateEntry(const LogEntry& entry) const override`
- `std::vector<LogValidationError> getValidationErrors(const LogEntry& entry) const override`

#### Log Configuration
- `void addLogValidator(const LogValidator& validator)`
- `void setForbiddenPatterns(const std::vector<std::string>& patterns)`
- `void setRequiredPatterns(const std::vector<std::string>& patterns)`
- `void setRequiredMetadata(const std::unordered_map<std::string, std::string>& metadata)`

### Class: Logger

#### Constructors/Destructors
- `Logger()` - Constructor
- `~Logger()` - Destructor

#### Core Operations
- `void addLogger(std::shared_ptr<ILogger> logger)`
- `void clearLoggers()`
- `void setLevel(LogLevel level)`

#### Global Logging Functions
- `void log(LogLevel level, const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")`
- `void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")`
- `void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")`
- `void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")`
- `void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")`
- `void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "")`

#### Query Operations
- `bool isEnabled(LogLevel level) const`

#### Global Logger Management
- `static Logger& getGlobal()` - Get singleton instance
- `static void setGlobal(std::shared_ptr<Logger> logger)`

#### Structured Logging
- `void logWithMetadata(LogLevel level, const std::string& message, const std::unordered_map<std::string, std::string>& metadata, ...)`

#### Template Logging
- `template<typename T> void logValue(LogLevel level, const std::string& key, const T& value, ...)`

#### Validation
- `bool validateEntry(const LogEntry& entry) const`
- `std::vector<LogValidationError> getValidationErrors(const LogEntry& entry) const`

#### Log Configuration
- `void addLogValidator(const LogValidator& validator)`
- `void setForbiddenPatterns(const std::vector<std::string>& patterns)`
- `void setRequiredPatterns(const std::vector<std::string>& patterns)`
- `void setRequiredMetadata(const std::unordered_map<std::string, std::string>& metadata)`

### Class: FileLogger

#### Constructors
- `FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info)`

#### Core Methods
- `void log(const LogEntry& entry) override`
- `void setLevel(LogLevel level) override`
- `LogLevel getLevel() const override`

#### File Operations
- `void flush()`
- `void close()`
- `void setMetadata(const std::string& key, const std::string& value) override`
- `std::string getMetadata(const std::string& key) const override`
- `void clearMetadata() override`
- `size_t getLogCount() const override`
- `std::vector<LogEntry> getRecentLogs(size_t limit = 100) const override`

#### Validation
- `bool validateEntry(const LogEntry& entry) const override`
- `std::vector<LogValidationError> getValidationErrors(const LogEntry& entry) const override`

### Macros

#### Convenience Macros
- `#define NLM_LOG_DEBUG(msg)` - Debug level logging with location
- `#define NLM_LOG_INFO(msg)` - Info level logging with location
- `#define NLM_LOG_WARNING(msg)` - Warning level logging with location
- `#define NLM_LOG_ERROR(msg)` - Error level logging with location
- `#define NLM_LOG_CRITICAL(msg)` - Critical level logging with location

#### Stream Macro
- `#define NLM_LOG_STREAM(level)` - Stream-style logging

## Best Practices

### 1. Use Appropriate Log Levels
- DEBUG: Detailed debugging information
- INFO: General operational information
- WARNING: Warnings that may indicate problems
- ERROR: Error conditions that affect functionality
- CRITICAL: Severe errors that require immediate attention

### 2. Minimize Performance Impact
- Use `isEnabled()` to check log level before expensive operations
- Use stream-style logging for complex message construction
- Limit the size of metadata
- Avoid logging sensitive information (passwords, tokens, etc.)

### 3. Structured Logging
- Use metadata for structured log entries
- Include relevant context (neuron_id, layer, etc.)
- Use consistent metadata keys across the codebase

### 4. Validation Rules
- Define validation rules for critical log entries
- Use forbidden patterns to prevent sensitive data leaks
- Set required metadata for important log events

### 5. Log Rotation
- Enable log rotation in production
- Monitor log file sizes
- Rotate based on size or time

## Configuration File Examples

### JSON Configuration
```json
{
    "logger": {
        "level": "info",
        "use_colors": true,
        "console_output": true,
        "file_output": {
            "path": "logs/nlm.log",
            "max_size": 10485760,
            "max_files": 5
        },
        "validation": {
            "forbidden_patterns": ["password", "secret", "token"],
            "required_metadata": {"component": "brain", "session": "default"}
        }
    }
}
```

## Thread Safety in Detail
All logging operations are thread-safe:
1. **Log Methods**: Protected by mutexes
2. **Metadata Operations**: Protected by mutexes
3. **Log Rotation**: Thread-safe file operations
4. **Validation**: Thread-safe validation rules
5. **Recent Logs**: Thread-safe log retrieval
6. **Global Logger**: Thread-safe singleton access

## Performance Considerations
1. **Thread Contention**: High contention on single logger can impact performance
2. **Lock Granularity**: Fine-grained locks for different logger components
3. **Batch Operations**: Group multiple log entries when possible
4. **Disk I/O**: Minimize file operations in hot paths

## Error Handling
- FileLogger handles file I/O errors gracefully
- Log rotation handles disk full conditions
- Validation errors are logged but don't crash the application
- All exceptions in logging are caught and logged to prevent crashes

## Integration with NLM Components
The Logger is used throughout NLM for:
- Configuration changes
- Neural activity logging
- Memory system events
- Plasticity updates
- Development stage changes
- Simulation progress
- Error conditions
- Performance metrics

## Testing and Validation
- Log validation can be configured for unit tests
- Mock loggers can be injected for testing
- Structured logs can be easily parsed in tests
- Validation rules can be disabled during testing

## Migration Guide
If migrating from older versions of NLM:
1. Replace `Logger::log()` calls with `Logger::logWithMetadata()` for structured logging
2. Use the new macros (`NLM_LOG_DEBUG`, etc.) for simpler logging
3. Configure validation rules if you had custom logging validation
4. Set up log rotation for production deployments
5. Use metadata consistently across your codebase
