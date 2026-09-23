# Comprehensive Logging Infrastructure

## Overview

The NLM (Neural Learning Machine) now features a comprehensive logging infrastructure designed to provide detailed insights into system operation, debugging information, and performance monitoring.

## Architecture

### Core Components

1. **Logger Interface**
   - Abstract base class for all logging implementations
   - Supports multiple logging levels
   - Provides thread-safe logging operations

2. **Console Logger**
   - Outputs to standard console/stdout
   - Supports different log levels (DEBUG, INFO, WARNING, ERROR)
   - Configurable formatting and output options

3. **File Logger**
   - Writes log entries to files
   - Supports rotation and size-based management
   - Automatic file creation and permission handling

4. **Performance Logger**
   - Specialized logging for performance metrics
   - Tracks execution times and resource usage
   - Generates performance reports

### Log Levels

| Level | Priority | Description |
|-------|----------|-------------|
| DEBUG | 1 | Detailed debugging information |
| INFO | 2 | General information messages |
| WARNING | 3 | Warning conditions that don't prevent operation |
| ERROR | 4 | Error conditions that prevent specific operations |

## Configuration

### Log Configuration Schema

```json
{
  "log_level": "INFO",
  "log_to_file": false,
  "log_filename": "nlm.log",
  "log_rotation_size": 10485760,
  "log_max_files": 5,
  "console_color": true,
  "log_timestamp": true,
  "log_thread_id": true,
  "log_category": true
}
```

### Configuration Options

- **log_level**: Minimum log level to output
- **log_to_file**: Enable file logging
- **log_filename**: Output file name
- **log_rotation_size**: File size before rotation (bytes)
- **log_max_files**: Maximum number of log files to keep
- **console_color**: Enable colored console output
- **log_timestamp**: Include timestamps in log entries
- **log_thread_id**: Include thread IDs in log entries
- **log_category**: Include category/category name in log entries

## Usage Examples

### Basic Console Logging

```cpp
#include "core/Logger/Logger.hpp"

int main() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("System initialized");
    NLM_LOG_WARNING("Deprecated feature used");
    NLM_LOG_ERROR("Failed to load configuration");
}
```

### File Logging

```cpp
auto fileLogger = std::make_shared<FileLogger>("nlm.log");
if (fileLogger) {
    logger->addLogger(fileLogger);
    NLM_LOG_INFO("File logging enabled");
}
```

### Performance Logging

```cpp
class PerformanceLogger {
public:
    PerformanceLogger() {
        NLM_PERF_LOG("Memory allocation", getCurrentMemoryUsage());
        NLM_PERF_LOG("Computation time", getComputationTime());
    }
    
    void logStep(const std::string& stepName, double duration) {
        NLM_PERF_LOG(stepName, duration);
    }
};
```

## Log Categories

The logging system supports categorization for better log management:

- **SYSTEM**: Core system initialization and shutdown
- **CONFIG**: Configuration loading and validation
- **BRAIN**: Neural network operations
- **MEMORY**: Memory management and allocation
- **PLASTICITY**: Synaptic plasticity and learning
- **PERFORMANCE**: Performance metrics and timing
- **ERROR**: Error conditions and exceptions
- **DEBUG**: Detailed debugging information

## Example Log Output

```
[2025-01-01 12:00:00.000] [INFO] [SYSTEM] System initialized successfully
[2025-01-01 12:00:01.234] [DEBUG] [CONFIG] Loading configuration from configs/default.cfg
[2025-01-01 12:00:02.456] [WARNING] [SYSTEM] Deprecated configuration format detected
[2025-01-01 12:00:03.789] [ERROR] [BRAIN] Failed to initialize neural network: Insufficient memory
[2025-01-01 12:00:05.123] [PERF] Memory allocation: 1.2MB (1.5s)
[2025-01-01 12:00:06.456] [INFO] [SYSTEM] System shutdown completed
```

## Integration with Existing Code

### Modifying Existing Log Statements

```cpp
// Before
NLM_LOG_INFO("Initializing brain");

// After
NLM_LOG_INFO("Initializing brain", LogCategory::SYSTEM);
```

### Adding New Log Categories

```cpp
enum class LogCategory {
    SYSTEM,
    CONFIG,
    BRAIN,
    MEMORY,
    PLASTICITY,
    PERFORMANCE,
    ERROR,
    DEBUG,
    WORLD,
    AGENT,
    SENSOR,
    MOTOR
};
```

## Performance Considerations

### Log Level Optimization

- Set appropriate log levels for production environments
- Use DEBUG level only during development
- Consider log level filtering at system level

### File Logging Best Practices

- Enable file logging for production debugging
- Configure log rotation to prevent disk space issues
- Use compressed logs for long-running simulations

### Memory Usage

- Log streaming: Consider log streaming for large-scale simulations
- Log aggregation: Implement log aggregation for distributed systems
- Log sampling: Sample logs for high-frequency events

## Testing and Validation

### Unit Tests

```bash
# Run logging infrastructure tests
./test_logging --verbose

# Test different log levels
./test_logging --levels=DEBUG,INFO,WARNING,ERROR

# Test file logging
./test_logging --file_logging
```

### Integration Tests

```bash
# Test end-to-end logging in simulation
./test_simulation --logging=true --output_log=simulation.log
```

### Performance Tests

```bash
# Test logging performance under load
./performance_test --duration=300 --log_level=INFO
```

## Migration Guide

### From Legacy Logging to Enhanced Logging

1. **Replace Direct std::cout/cerr Usage**
   ```cpp
   // Old way
   std::cout << "Initializing system" << std::endl;
   
   // New way
   NLM_LOG_INFO("Initializing system");
   ```

2. **Add Log Categories**
   ```cpp
   // Old way (no categories)
   NLM_LOG_WARNING("This feature is deprecated");
   
   // New way (with categories)
   NLM_LOG_WARNING("This feature is deprecated", LogCategory::SYSTEM);
   ```

3. **Enable File Logging**
   ```cpp
   auto fileLogger = std::make_shared<FileLogger>("nlm.log");
   logger->addLogger(fileLogger);
   ```

## Troubleshooting

### Common Issues and Solutions

**Issue: No log output appears**
- Solution: Check log level configuration and ensure logger is properly initialized

**Issue: Log files grow too large**
- Solution: Configure log rotation and set appropriate file size limits

**Issue: Performance degradation due to logging**
- Solution: Reduce log verbosity, disable file logging for performance-critical sections

**Issue: Log entries are not formatted correctly**
- Solution: Check log configuration and ensure proper initialization sequence

## Future Enhancements

### Planned Improvements

1. **Structured Logging**: Implement JSON-formatted log entries
2. **Distributed Logging**: Support for centralized logging across multiple processes
3. **Log Analytics**: Built-in log analysis and visualization tools
4. **Log Streaming**: Real-time log streaming for remote monitoring
5. **Adaptive Logging**: Dynamic log level adjustment based on system state

### Research and Development

- Implement structured logging with correlation IDs
- Develop log aggregation for distributed systems
- Create log-based anomaly detection
- Integrate with existing monitoring systems (Grafana, Prometheus)

## Conclusion

The enhanced logging infrastructure provides comprehensive visibility into NLM operations, supports multiple output formats, and offers flexible configuration options. It enables effective debugging, performance monitoring, and system diagnostics while maintaining high performance characteristics.

The logging system is designed to be:
- **Comprehensive**: Covers all aspects of system operation
- **Flexible**: Supports multiple output formats and configurations
- **Performant**: Minimizes overhead while providing detailed information
- **Extensible**: Easy to extend with new log categories and output formats
- **Robust**: Handles errors gracefully and provides fallback mechanisms

This infrastructure will significantly improve the debugging experience and operational visibility of the NLM system.