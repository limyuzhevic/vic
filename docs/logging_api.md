# NLM Logging System API

## Overview

The Logging System provides comprehensive logging and debugging capabilities for the NLM neural simulation framework. It supports multiple log levels, output formats, and integration with various logging backends while maintaining high performance and minimal overhead.

## Key Features

- **Multi-level logging**: Debug, Info, Warning, Error, Critical
- **Multiple output sinks**: Console, file, custom implementations
- **Performance optimized**: Minimal overhead with configurable logging levels
- **Thread-safe operations**: Safe concurrent logging from multiple threads
- **Structured logging**: Rich metadata with file, line, and function information
- **Global and local loggers**: Flexible logging configuration
- **Stream-style logging**: Efficient formatted output generation

## Logging Architecture

### Logger Hierarchy

```
                          +-------------------+
                          |    Logger         |
                          | (Base Class)      |
                          +-------------------+
                                 |
         +------------------------+------------------------+
         |                        |                        |
+---------------+          +---------------+          +---------------+
|  ConsoleLogger |          |   FileLogger  |          | CustomLogger  |
| (Implementation)|          | (Implementation)|          | (Implementation)|
+---------------+          +---------------+          +---------------+
```

### Log Processing Pipeline

1. **Event Generation**: Log statements create LogEntry objects
2. **Level Filtering**: LogLevel checks determine if log should be processed
3. **Sink Dispatch**: LogEntry sent to registered sinks
4. **Output Generation**: Each sink formats and outputs the log
5. **Asynchronous Processing**: Optional background thread for performance

## Core Classes

### LogLevel Enum

```cpp
enum class LogLevel {
    Debug = 0,      // Detailed debugging information
    Info = 1,        // General information messages
    Warning = 2,     // Warning conditions
    Error = 3,       // Error conditions
    Critical = 4     // Critical errors requiring immediate attention
};
```

### LogEntry Structure

```cpp
struct LogEntry {
    LogLevel level;                           // Severity level
    std::string message;                      // Log message
    std::string file;                         // Source file path
    int line;                                 // Line number in source file
    std::string function;                     // Function name where log was created
    std::chrono::system_clock::time_point timestamp; // Entry creation time
};
```

### ILogger Interface

Abstract base class for all logger implementations.

```cpp
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Core logging interface
    virtual void log(const LogEntry& entry) = 0;
    
    // Configuration interface
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};
```

### ConsoleLogger Class

Console output implementation with advanced formatting.

```cpp
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Core interface implementation
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Console-specific configuration
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};
```

### Logger Class

Main logger class with global and local instances.

```cpp
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Sink management
    void addLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Configuration
    void setLevel(LogLevel level);
    
    // Core logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", 
              int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", 
                 int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", 
                  int line = 0, const std::string& function = "");
    
    // Query interface
    bool isEnabled(LogLevel level) const;
    
    // Global logger access
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### LogStream Class

Stream-style logging helper for formatted output.

```cpp
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, 
              const std::string& function);
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

## Logging Levels

### Level Hierarchy

Log levels are processed in order of increasing severity:
1. **Debug** (0): Detailed debugging information
2. **Info** (1): General information messages
3. **Warning** (2): Warning conditions
4. **Error** (3): Error conditions
5. **Critical** (4): Critical errors requiring immediate attention

### Level Filtering

```cpp
// Set global log level to filter messages
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Warning);

// Only messages with Warning, Error, or Critical level will be processed
logger.debug("Debug message");    // Not logged
logger.info("Info message");      // Not logged
logger.warning("Warning message"); // Logged
logger.error("Error message");    // Logged
logger.critical("Critical message"); // Logged
```

## Logging Methods

### Basic Logging

```cpp
// Basic logging with message only
nlm::Logger::getGlobal().info("Simulation started");

// Logging with source location information
nlm::Logger::getGlobal().debug("Processing neuron", __FILE__, __LINE__, __func__);

// Stream-style formatted logging
NLM_LOG_STREAM(nlm::LogLevel::Info) << "Processing neuron " << neuronId 
                                      << " at step " << step;
```

### Convenience Macros

```cpp
// Macro definitions for common logging patterns
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Usage examples
NLM_LOG_INFO("Brain initialization completed");
NLM_LOG_ERROR("Failed to initialize brain");
```

## Usage Examples

### Basic Logging Setup

```cpp
#include <nlm/core/Logger.hpp>

int main() {
    // Initialize global logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Log different levels of messages
    NLM_LOG_INFO("Starting NLM simulation");
    NLM_LOG_DEBUG("Initializing neural components");
    NLM_LOG_WARNING("High memory usage detected");
    NLM_LOG_ERROR("Failed to load configuration");
    
    return 0;
}
```

### File Logging

```cpp
// Add file logging to existing logger
auto fileLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
fileLogger->setOutputFile("nlm_simulation.log");

nlm::Logger::getGlobal().addLogger(fileLogger);
```

### Colored Console Output

```cpp
// Enable colored console output
auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
consoleLogger->setUseColors(true);

nlm::Logger::getGlobal().addLogger(consoleLogger);
```

### Multi-Logger Setup

```cpp
int main() {
    // Create multiple logger instances
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    auto fileLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    
    // Configure file logger
    fileLogger->setOutputFile("debug.log");
    fileLogger->setUseColors(false);
    
    // Set up main logger
    auto logger = std::make_shared<nlm::Logger>();
    logger->addLogger(consoleLogger);  // Console: Info and above
    logger->addLogger(fileLogger);     // File: Warning and above
    nlm::Logger::setGlobal(logger);
    
    return 0;
}
```

## Configuration and Management

### Log Level Management

```cpp
// Set global log level
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);

// Set individual logger levels
auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Debug);
consoleLogger->setLevel(nlm::LogLevel::Info);  // Only Info and above
```

### Logger Lifecycle

```cpp
// Create and configure logger
auto logger = std::make_shared<nlm::Logger>();

// Add sinks
logger->addLogger(std::make_shared<nlm::ConsoleLogger>());

// Configure sinks
if (auto consoleLogger = 
    std::dynamic_pointer_cast<nlm::ConsoleLogger>(logger->getLogger(0))) {
    consoleLogger->setUseColors(true);
    consoleLogger->setOutputFile("console.log");
}

// Set as global logger
nlm::Logger::setGlobal(logger);
```

### Log Stream Usage

```cpp
// Create and use log stream
NLM_LOG_STREAM(nlm::LogLevel::Info) << "Simulation step " << step 
                                      << " completed in " << duration << "ms";

// Complex formatting
NLM_LOG_STREAM(nlm::LogLevel::Warning) << "Neuron " << neuronId 
                                         << " exceeded threshold: " 
                                         << neuron->getMembranePotential() << "mV";
```

## Performance Considerations

### Log Level Filtering

```cpp
// Efficient level checking
bool isLogEnabled = logger.isEnabled(nlm::LogLevel::Debug);
if (isLogEnabled) {
    logger.debug("Expensive computation details");
}
```

### Minimal Overhead

```cpp
// Even when logging is disabled, function calls are cheap
logger.debug("Debug message");  // Still function call overhead
// Better: check first
if (logger.isEnabled(nlm::LogLevel::Debug)) {
    logger.debug("Expensive debug message");
}
```

### Stream Buffer Management

```cpp
// Stream objects are created on demand
NLM_LOG_STREAM(nlm::LogLevel::Info) << "Processing " << itemCount << " items";

// Automatic cleanup when stream goes out of scope
```

## Thread Safety

### Concurrent Logging

```cpp
// Global logger is thread-safe for read operations
void threadFunction() {
    nlm::Logger::getGlobal().info("Thread message");
}

// Multiple threads can log safely
std::thread t1([]() { nlm::Logger::getGlobal().info("Thread 1"); });
std::thread t2([]() { nlm::Logger::getGlobal().info("Thread 2"); });

t1.join();
t2.join();
```

### Sink Thread Safety

```cpp
// Each logger instance should be accessed from a single thread
// or with proper synchronization
auto logger = std::make_shared<nlm::Logger>();

// Thread-safe operations
logger->addLogger(std::make_shared<nlm::ConsoleLogger>());  // Multiple threads
logger->setLevel(nlm::LogLevel::Info);  // Single thread recommended
```

## Error Handling

### Logging Errors

```cpp
// Handle potential logging failures
auto logger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);

try {
    logger->log(LogEntry{nlm::LogLevel::Info, "Test message", "file.cpp", 123, "test()"});
} catch (const std::exception& e) {
    std::cerr << "Logging error: " << e.what() << std::endl;
}
```

## Integration Examples

### With Brain Simulation

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Logger.hpp>

int main() {
    // Set up logging for brain simulation
    nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Info);
    
    // Log simulation events
    NLM_LOG_INFO("Creating brain simulation");
    
    auto brain = std::make_shared<nlm::Brain>(config);
    NLM_LOG_INFO("Brain created successfully");
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Brain initialization failed");
        return 1;
    }
    
    NLM_LOG_INFO("Brain initialization completed");
    
    // Log simulation progress
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("Step " << step << " completed, total spikes: " 
                          << brain->getTotalSpikeCount());
        }
    }
    
    NLM_LOG_INFO("Simulation completed successfully");
    
    return 0;
}
```

### Custom Logger Implementation

```cpp
#include <nlm/core/Logger.hpp>

class CustomLogger : public nlm::ILogger {
public:
    void log(const nlm::LogEntry& entry) override {
        // Custom logging implementation
        // For example, send to database, web service, etc.
        std::cout << "Custom: " << entry.message << std::endl;
    }
    
    void setLevel(nlm::LogLevel level) override {
        // Store level for filtering
        currentLevel_ = level;
    }
    
    nlm::LogLevel getLevel() const override {
        return currentLevel_;
    }
    
private:
    nlm::LogLevel currentLevel_ = nlm::LogLevel::Info;
};

int main() {
    // Add custom logger
    auto customLogger = std::make_shared<CustomLogger>();
    nlm::Logger::getGlobal().addLogger(customLogger);
    
    NLM_LOG_INFO("Message sent to custom logger");
    
    return 0;
}
```

## Testing and Debugging

### Unit Testing Logging

```cpp
#include <nlm/core/Logger.hpp>
#include <gtest/gtest.h>

TEST(LoggerTest, BasicLogging) {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    
    EXPECT_NO_THROW(NLM_LOG_INFO("Test message"));
}

TEST(LoggerTest, LevelFiltering) {
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Warning);
    logger->addLogger(consoleLogger);
    
    // Debug and info messages should not be logged
    // Warning and error messages should be logged
}
```

### Performance Testing

```bash
// Run logging performance tests
./benchmark --test logging --iterations 1000000

// Profile logging overhead
./profiler --target logger --operation debug_info
```

## Best Practices

### Logging Strategy

1. **Use appropriate log levels**: Don't log debug information in production
2. **Include context**: Add file, line, and function information for debugging
3. **Log early and often**: Log important events and state changes
4. **Avoid expensive operations in log messages**: Use streams for complex formatting
5. **Design for thread safety**: Use appropriate synchronization

### Performance Optimization

1. **Check log levels before expensive operations**: Use `isEnabled()`
2. **Use stream logging for complex messages**: Avoid string concatenation
3. **Limit log volume**: Set appropriate log levels
4. **Configure output sinks appropriately**: Balance performance and functionality

### Configuration Management

1. **Set appropriate log levels for different environments**:
   - **Development**: Debug level for comprehensive debugging
   - **Testing**: Info level for test tracking
   - **Production**: Warning/Error/Critical for system health
2. **Use structured logging**: Include relevant metadata in log messages
3. **Implement log rotation**: Manage log file sizes
4. **Monitor log performance**: Track overhead in performance-critical paths

## Common Use Cases

### Debug Mode

```cpp
// Development environment - detailed logging
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Debug);
```

### Production Mode

```cpp
// Production environment - minimal logging
nlm::Logger::getGlobal().setLevel(nlm::LogLevel::Error);
```

### Simulation Monitoring

```cpp
// Monitor simulation progress
for (int step = 0; step < maxSteps; ++step) {
    // ... simulation logic ...
    
    if (step % 100 == 0) {
        NLM_LOG_INFO("Step " << step << " completed");
    }
    
    // Check for potential issues
    if (brain->getFiringNeuronCount() > threshold) {
        NLM_LOG_WARNING("High firing rate detected at step " << step);
    }
}
```

## Migration Guide

### From Earlier Versions

Logging system changes:

- **Version 0.1.0**: Simple console output only
- **Version 0.2.0**: Added log levels and basic formatting
- **Version 0.3.0**: Added file logging and sink architecture
- **Version 0.4.0**: Added stream-style logging and macros
- **Version 0.5.0**: Enhanced thread safety and global logger
- **Version 0.6.0**: Improved performance and configuration options

## References

1. [C++ Logging Best Practices](https://stackoverflow.com/questions/5089761/best-practice-for-logging-in-c-applications)
2. [Structured Logging](https://go.dev/blog/structure-logging)
3. [Log Level Design](https://stackoverflow.com/questions/4228442/logging-levels)

## See Also

- [Config API](config_api.md) - Configuration system integration
- [Brain API](brain_api.md) - Brain simulation with logging
- [Visualization API](visualization_api.md) - Visualization system integration

**Last updated:** October 18, 2026
**Version:** 0.6.0