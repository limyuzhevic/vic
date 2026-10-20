# NLM Enhanced Logging System

## Overview

The NLM (Neural Learning Machine) logging system has been significantly enhanced to provide:

- **Structured JSON Logging**: Support for JSON-formatted log entries
- **Multiple Output Formats**: Traditional text and structured JSON output
- **Advanced Filtering**: Log level filtering and structured filtering
- **Rich Metadata**: Enhanced log entries with file, line, function, and timestamp information
- **Integration**: Seamless integration with the configuration system
- **Color Support**: ANSI color codes for terminal output
- **File Output**: Dual console and file logging capabilities

## Log Levels

The logging system uses a standardized severity hierarchy:

```cpp
enum class LogLevel {
    Debug = 0,      // Detailed debugging information
    Info = 1,       // General information messages
    Warning = 2,    // Warning conditions
    Error = 3,      // Error conditions
    Critical = 4    // Critical errors (system-threatening)
};
```

## Log Entry Structure

Each log entry contains comprehensive metadata for debugging and analysis:

```cpp
struct LogEntry {
    LogLevel level;                                    // Severity level
    std::string message;                               // Log message
    std::string file;                                  // Source file name
    int line;                                          // Line number in file
    std::string function;                             // Function name
    std::chrono::system_clock::time_point timestamp;  // Precise timestamp
};
```

## Logger Interface

### Abstract Base Class

```cpp
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};
```

## Console Logger Implementation

### Key Features

The ConsoleLogger provides:

1. **Dual Output Modes**: Text and JSON output
2. **Color Support**: ANSI color codes for different log levels
3. **File Output**: Optional file logging alongside console output
4. **Structured Formatting**: Configurable log message formatting
5. **Performance**: Fast filtering and efficient output

### Class: nlm::ConsoleLogger

```cpp
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();                                    // Default constructor
    explicit ConsoleLogger(LogLevel level);             // Constructor with level
    ~ConsoleLogger() override;
    
    // ILogger interface implementation
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Configuration methods
    void setUseColors(bool useColors);                  // Enable/disable colors
    bool getUseColors() const;
    
    // JSON logging configuration
    void setUseJSON(bool useJSON);                      // Enable JSON output
    bool getUseJSON() const;
    void setJSONFormat(const std::string& format);     // Set JSON format string
    std::string getJSONFormat() const;
    
    // File output methods
    void setOutputFile(const std::string& filepath);    // Set output file
    void closeOutputFile();                            // Close output file
    
private:
    struct Impl;                                       // Private implementation
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatJSONEntry(const LogEntry& entry) const;
};
```

### JSON Logging Format

When JSON logging is enabled, entries are formatted as:

```json
{
    "timestamp": "2026-10-20 14:30:45.123",
    "level": "INFO",
    "message": "Simulation started",
    "file": "src/main.cpp",
    "line": 42,
    "function": "main"
}
```

### Text Logging Format

Traditional text logging format:

```
[INFO] 2026-10-20 14:30:45.123 Simulation started (src/main.cpp:42)
```

### Color Coding

ANSI color codes for different log levels:

```
[DEBUG] Cyan
[INFO] Green
[WARNING] Yellow
[ERROR] Red
[CRITICAL] Magenta
```

## Main Logger Class

### Class: nlm::Logger

```cpp
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Global configuration
    void setLevel(LogLevel level);
    
    // Logging interface
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Query methods
    bool isEnabled(LogLevel level) const;
    
    // Global instance management
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Logging Macros

### Convenience Macros

```cpp
// Basic logging macros
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)
```

### Usage Examples

```cpp
// Basic logging
NLM_LOG_INFO("Starting simulation");
NLM_LOG_WARNING("Low memory available");
NLM_LOG_ERROR("Failed to load configuration");

// Debug logging
NLM_LOG_DEBUG("Neuron state: V=\" + std::to_string(neuron->getMembranePotential()) + " mV, threshold=\" + std::to_string(neuron->getThreshold()) + " mV");

// Stream-style logging
NLM_LOG_STREAM(nlm::LogLevel::Info) << "Neurons initialized: " << brain->getTotalNeuronCount();
```

## LogStream Class

### Stream-Style Logging

```cpp
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};
```

## Configuration Integration

### Logger Configuration via Environment

The logging system can be configured through environment variables:

```bash
# Set log level via environment
export NLM_LOG_LEVEL=INFO

# Enable JSON logging
export NLM_LOG_JSON=true

# Set log file
export NLM_LOG_FILE=nlm.log
```

### CLI Integration

Command-line options for logging:

```bash
--log-level LEVEL           Set minimum log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
--json-logging              Enable JSON logging format
--log-file FILE             Set log output file
--no-colors                 Disable colored output
```

## Performance Considerations

### Memory Usage

- Each LogEntry contains timestamp, metadata, and message
- Logger instances maintain lists of active sinks
- Memory usage scales with number of concurrent loggers

### Output Performance

- Color codes are cached for performance
- JSON serialization uses optimized string building
- File output is buffered for efficiency
- Log level filtering happens before allocation

### Memory Pool Optimization

```cpp
// For high-performance scenarios, consider implementing a memory pool
class LogMemoryPool {
    static LogEntry* allocate();
    static void deallocate(LogEntry* entry);
};
```

## Integration with Other Components

### Configuration System Integration

The logging system integrates with the configuration system for:

- **Log Level Configuration**: Set via config file or CLI
- **File Output Configuration**: Set log file location and rotation
- **JSON Formatting Options**: Customize JSON output format
- **Performance Settings**: Enable/disable features based on configuration

### Brain System Integration

The brain system uses logging for:

- **Initialization**: Brain creation and setup logging
- **Simulation**: Step progress and activity logging
- **Errors**: Neural dynamics and plasticity errors
- **Debug**: Complex state transitions

## Thread Safety

### Concurrent Logging

The logging system is thread-safe:

```cpp
// Multiple threads can log simultaneously
std::thread t1([&]() { NLM_LOG_INFO("Thread 1 work"); });
std::thread t2([&]() { NLM_LOG_INFO("Thread 2 work"); });

t1.join();
t2.join();
```

### Mutex Usage

```cpp
// Internal implementation uses mutexes for thread safety
// LogStream creation and destruction use internal locks
// File output is synchronized across threads
```

## Error Handling

### Log File Errors

```cpp
// File output errors are handled gracefully
// If file cannot be opened, logging continues to console
// Errors are reported at critical level
```

### JSON Formatting Errors

```cpp
// Invalid UTF-8 in messages are escaped
// Special characters are properly encoded
// Timestamp formatting uses ISO 8601 standard
```

## Testing and Validation

### Unit Testing

```cpp
// Test log level filtering
assert(!logger.isEnabled(nlm::LogLevel::Debug));  // Default is INFO

// Test message content
MockLogger mockLogger;
mockLogger.log(entry);
assert(mockLogger.lastMessage() == "Test message");

// Test JSON formatting
ConsoleLogger jsonLogger(nlm::LogLevel::Info);
jsonLogger.setUseJSON(true);
std::string jsonOutput = formatJSONEntry(testEntry);
assert(jsonOutput.find("timestamp") != std::string::npos);
```

### Integration Testing

```cpp
// Test logger integration with other components
Logger logger;
AutoLogger consoleLogger(nlm::LogLevel::Debug);
logger.addLogger(consoleLogger);

logger.debug("Integration test");
assert(consoleLogger.messages().contains("Integration test"));
```

## Migration Guide

### From Basic to Enhanced Logging

1. **Update includes**: `#include "Logger.hpp"`
2. **Replace macros**: Use NLM_LOG_* macros
3. **Configure logging**: Set log level and format
4. **Test functionality**: Verify all log levels work
5. **Performance tuning**: Adjust based on workload

### Upgrading from Previous Version

1. **New JSON support**: Enable via setUseJSON(true)
2. **New levels**: Use existing log levels
3. **New methods**: Implement custom LogStream operators
4. **New configuration**: Set via config or environment
5. **New formatting**: Customize JSON format

## Limitations

1. **JSON Dependencies**: Requires nlohmann/json for full JSON support
2. **Color Support**: ANSI colors may not work in all terminals
3. **Performance Overhead**: JSON formatting adds overhead
4. **Memory**: Log entries contain more metadata than simple logging
5. **Configuration**: Requires integration with configuration system

## Future Enhancements

1. **Log Levels**: Add custom log levels
2. **Asynchronous Logging**: Background thread for log output
3. **Log Rotation**: Automatic file rotation
4. **Structured Fields**: Typed log fields for structured logging
5. **Correlation ID**: Trace logs across system components
6. **Performance Metrics**: Log performance statistics

## Examples

### Basic Usage

```cpp
#include "Logger.hpp"

int main() {
    // Initialize logger
    auto logger = nlm::Logger::getGlobal();
    
    // Log various messages
    logger.info("Starting NLM simulation");
    logger.debug("Initial neuron configuration");
    
    // Enable JSON logging
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setUseJSON(true);
    logger.addLogger(consoleLogger);
    
    // More logging
    logger.warning("Simulation parameters outside optimal range");
    logger.error("Failed to load brain configuration");
    
    return 0;
}
```

### Advanced Usage

```cpp
#include "Logger.hpp"

void configureLogging() {
    auto logger = nlm::Logger::getGlobal();
    
    // Configure console logger
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    consoleLogger->setUseColors(true);
    consoleLogger->setUseJSON(false);
    consoleLogger->setOutputFile("nlm_simulation.log");
    
    logger->addLogger(consoleLogger);
    logger->setLevel(nlm::LogLevel::Debug);
}

void complexOperation() {
    // Complex stream-style logging
    NLM_LOG_STREAM(nlm::LogLevel::Info) 
        << "Processing " << 100 << " neurons in " 
        << 5 << " regions with " 
        << 0.15f << " connection probability";
    
    // File and console both get the message
    auto logger = nlm::Logger::getGlobal();
    logger->debug("Detailed neural state information");
}
```

## References

- [ANSI Escape Codes](https://en.wikipedia.org/wiki/ANSI_escape_code)
- [ISO 8601 Timestamp Format](https://en.wikipedia.org/wiki/ISO_8601)
- [nlohmann/json Documentation](https://github.com/nlohmann/json)
- [C++ Standard Library Thread Safety](https://en.cppreference.com/w/cpp/thread)

---
*This documentation is auto-generated from the NLM source code comments and logging system implementation.*
*Last updated: 2026-10-20*
*Version: 0.1.0*
*Author: NLM Research Team*