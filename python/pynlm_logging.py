# NLM Logging System

This module provides comprehensive logging for NLM Python bindings,
including structured logging, different log levels, and various output formats.

## Overview

The logging system provides:

- **Multiple Log Levels**: Debug, Info, Warning, Error, Critical
- **Structured Logging**: JSON and text log formats
- **Multiple Outputs**: Console, file, and network logging
- **Log Levels**: Configurable log levels per module or component
- **Performance Monitoring**: Performance logging and metrics
- **Debugging Support**: Stack traces and detailed debug information
- **Asynchronous Logging**: Non-blocking log operations
- **Log Rotation**: Automatic log file rotation

## Log Levels

### DEBUG (0)
Detailed debug information, useful for development and troubleshooting.

### INFO (1)
General information about system operation.

### WARNING (2)
Warning messages that may indicate potential issues.

### ERROR (3)
Error messages indicating failures or unexpected behavior.

### CRITICAL (4)
Critical errors that may cause system failure.

## Log Formats

### Text Format
Human-readable log format:

```
2023-10-15 14:30:45 INFO  [Module] Starting NLM simulation
2023-10-15 14:30:46 DEBUG [Brain] Created brain with 500 neurons
2023-10-15 14:30:47 WARNING [Plasticity] Synaptogenesis rate too high
2023-10-15 14:30:48 ERROR   [Memory] Failed to load checkpoint
2023-10-15 14:30:49 CRITICAL [System] Simulation crashed
```

### JSON Format
Structured log format:

```json
{
  "timestamp": "2023-10-15T14:30:45.123Z",
  "level": "INFO",
  "module": "Brain",
  "message": "Created brain with 500 neurons",
  "metadata": {
    "neuron_count": 500,
    "region_count": 1
  },
  "trace_id": "abc123"
}
```

## Logger Class

The main `Logger` class provides logging functionality:

```python
from pynlm.logging import Logger

# Create logger
logger = Logger()

# Set log level
logger.set_level("INFO")

# Log messages
logger.debug("Debug message")
logger.info("Information message")
logger.warning("Warning message")
logger.error("Error message")
logger.critical("Critical message")
```

## Specialized Loggers

### ModuleLogger

Logger specific to a module or component:

```python
from pynlm.logging import ModuleLogger

# Create module logger
brain_logger = ModuleLogger("Brain")
brain_logger.info("Brain initialized")

# Create system logger
system_logger = ModuleLogger("System")
system_logger.error("System error")
```

### AsyncLogger

Asynchronous logger for non-blocking log operations:

```python
from pynlm.logging import AsyncLogger

# Create async logger
async_logger = AsyncLogger()
async_logger.info("This log operation is asynchronous")
```

### PerformanceLogger

Logger for performance monitoring:

```python
from pynlm.logging import PerformanceLogger

# Create performance logger
perf_logger = PerformanceLogger()
perf_logger.start_timing("simulation_step")

# Run simulation
brain.step(step, dt)

# Log performance
perf_logger.end_timing("simulation_step")
perf_logger.log_performance("simulation_step", 0.001)
```

## Log Output

### Console Output

Logs are printed to console:

```python
logger.set_level("INFO")
logger.add_handler("console")
```

### File Output

Logs are written to files:

```python
logger.add_handler("file", filename="nlm.log", max_size=10*1024*1024)
```

### JSON Output

Logs are written in JSON format:

```python
logger.add_handler("json", filename="nlm.json", pretty_print=True)
```

## Log Configuration

### Basic Configuration

```python
from pynlm.logging import Logger

# Create logger
logger = Logger()

# Configure console output
logger.add_console_handler(
    level="INFO",
    format="%(asctime)s %(levelname)s %(name)s %(message)s"
)

# Configure file output
logger.add_file_handler(
    filename="nlm.log",
    level="DEBUG",
    max_size=10*1024*1024,  # 10 MB
    backup_count=5
)

# Configure JSON output
logger.add_json_handler(
    filename="nlm.json",
    level="INFO",
    pretty_print=True
)
```

### Advanced Configuration

```python
from pynlm.logging import Logger, ModuleLogger

# Create structured logger
logger = Logger()

# Add custom formatter
class CustomFormatter:
    def format(self, record):
        return f"[{record.levelname}] {record.name}: {record.message}"

logger.add_handler("console", formatter=CustomFormatter())

# Configure log levels for different modules
brain_logger = ModuleLogger("Brain")
brain_logger.set_level("DEBUG")

memory_logger = ModuleLogger("Memory")
memory_logger.set_level("INFO")

plasticity_logger = ModuleLogger("Plasticity")
plasticity_logger.set_level("WARNING")
```

## Log Monitoring

### Log Monitoring

Monitor log activity:

```python
from pynlm.logging import LogMonitor

# Create log monitor
monitor = LogMonitor()

# Start monitoring
monitor.start_monitoring("nlm.log")

# Get statistics
stats = monitor.get_statistics()
print(f"Log rate: {stats.log_rate} logs/second")
print(f"Error count: {stats.error_count}")
```

### Log Analysis

Analyze log files:

```python
from pynlm.logging import LogAnalyzer

# Create log analyzer
analyzer = LogAnalyzer()

# Analyze log file
results = analyzer.analyze_file("nlm.log")
print(f"Errors: {results.error_count}")
print(f"Warnings: {results.warning_count}")
print(f"Most common messages: {results.most_common_messages}")
```

## Performance Logging

### Performance Metrics

Log performance metrics:

```python
from pynlm.logging import PerformanceLogger

# Create performance logger
perf_logger = PerformanceLogger()

# Log simulation performance
perf_logger.start_timing("brain_step")
brain.step(step, dt)
perf_logger.end_timing("brain_step")

# Log memory usage
perf_logger.log_memory_usage()

# Log CPU usage
perf_logger.log_cpu_usage()
```

### Performance Monitoring

Monitor performance in real-time:

```python
from pynlm.logging import PerformanceMonitor

# Create performance monitor
monitor = PerformanceMonitor()

# Start monitoring
monitor.start_monitoring()

# Run simulation
for step in range(1000):
    brain.step(step, dt)

# Stop monitoring
monitor.stop_monitoring()

# Get performance report
report = monitor.get_report()
print(f"Average step time: {report.avg_step_time} seconds")
print(f"Memory usage: {report.memory_usage} MB")
print(f"CPU usage: {report.cpu_usage}%")
```

## Debugging Support

### Stack Trace Logging

Log stack traces:

```python
from pynlm.logging import Logger

logger = Logger()

try:
    # Some operation that might fail
    brain.step(step, dt)
except Exception as e:
    logger.error("Simulation failed", exc_info=True)
```

### Debug Mode

Enable debug mode:

```python
from pynlm.logging import Logger

logger = Logger()
logger.set_debug_mode(True)

# Log debug information
logger.debug("Debug information about brain state")
```

## Log Rotation

### Log Rotation

Automatic log file rotation:

```python
from pynlm.logging import Logger

logger = Logger()
logger.add_file_handler(
    filename="nlm.log",
    max_size=10*1024*1024,  # 10 MB
    backup_count=5,          # Keep 5 backup files
    rotate_on_startup=True
)
```

## Log Analysis API

### Log Analysis

Analyze log files programmatically:

```python
from pynlm.logging import LogAnalyzer

analyzer = LogAnalyzer()

# Analyze log file
results = analyzer.analyze_file("nlm.log")

# Get error statistics
error_stats = results.error_statistics
print(f"Total errors: {error_stats.total}")
print(f"Errors by module: {error_stats.by_module}")

# Get performance statistics
perf_stats = results.performance_statistics
print(f"Average step time: {perf_stats.avg_step_time}")
print(f"Step time standard deviation: {perf_stats.step_time_std}")
```

### Log Search

Search log files:

```python
from pynlm.logging import LogSearch

search = LogSearch()

# Search for errors
errors = search.search_file("nlm.log", level="ERROR")

# Search for specific messages
messages = search.search_file("nlm.log", message_pattern="Failed to load")

# Search in time range
from datetime import datetime, timedelta
start_time = datetime.now() - timedelta(hours=1)
end_time = datetime.now()
timeline = search.search_time_range("nlm.log", start_time, end_time)
```

## Log Configuration File

### Configuration File

Log configuration can be stored in files:

```json
{
    "log_level": "INFO",
    "handlers": [
        {
            "type": "console",
            "format": "%(asctime)s %(levelname)s %(name)s %(message)s"
        },
        {
            "type": "file",
            "filename": "nlm.log",
            "max_size": 10485760,
            "backup_count": 5
        },
        {
            "type": "json",
            "filename": "nlm.json",
            "pretty_print": true
        }
    ],
    "modules": {
        "Brain": "INFO",
        "Memory": "DEBUG",
        "Plasticity": "WARNING"
    }
}
```

## Log Usage Examples

### Example 1: Basic Logging

```python
from pynlm.logging import Logger

logger = Logger()
logger.set_level("INFO")

# Configure handlers
logger.add_console_handler()
logger.add_file_handler("nlm.log")

# Log simulation events
logger.info("Starting NLM simulation")
logger.debug(f"Created brain with {brain.getTotalNeuronCount()} neurons")
brain.initialize()
logger.info("Brain initialized successfully")

for step in range(100):
    brain.step(step, 0.001)
    if step % 10 == 0:
        logger.info(f"Completed {step} steps")
```

### Example 2: Performance Logging

```python
from pynlm.logging import PerformanceLogger

perf_logger = PerformanceLogger()

# Log simulation performance
for step in range(1000):
    perf_logger.start_timing("brain_step")
    brain.step(step, 0.001)
    perf_logger.end_timing("brain_step")
    
    if step % 100 == 0:
        perf_logger.log_performance("brain_step", 0.001)
        perf_logger.log_memory_usage()
```

### Example 3: Error Logging

```python
from pynlm.logging import Logger

logger = Logger()
logger.add_console_handler()
logger.add_file_handler("nlm.log")

try:
    # Some operation that might fail
    brain.save("brain_checkpoint.bin")
except Exception as e:
    logger.error(f"Failed to save brain checkpoint: {e}", exc_info=True)
```

## Log Best Practices

### 1. Use Appropriate Log Levels

- Use DEBUG for detailed information useful for development
- Use INFO for general system operation
- Use WARNING for potential issues
- Use ERROR for actual problems
- Use CRITICAL for serious errors that may cause system failure

### 2. Include Relevant Information

Include relevant information in log messages:

```python
logger.info(f"Created brain with {brain.getTotalNeuronCount()} neurons "
            f"in {brain.getRegionCount()} regions")
```

### 3. Use Structured Logging

Use structured logging for machine-readable logs:

```python
logger.info(
    "Simulation completed",
    steps_completed=1000,
    final_firing_rate=brain.getAverageFiringRate(),
    total_spikes=brain.getTotalSpikeCount()
)
```

### 4. Log Performance Metrics

Log performance metrics:

```python
perf_logger.start_timing("simulation")
brain.step(step, dt)
perf_logger.end_timing("simulation")
perf_logger.log_performance("simulation", brain.getAverageFiringRate())
```

### 5. Handle Errors Properly

Handle errors and log them:

```python
try:
    # Operation that might fail
    result = some_operation()
except Exception as e:
    logger.error(f"Operation failed: {e}", exc_info=True)
    raise  # Re-raise the exception
```

### 6. Use Log Levels Consistently

Use log levels consistently:

```python
# Good
if invalid_parameter:
    logger.error(f"Invalid parameter: {parameter}")

# Bad
if invalid_parameter:
    print(f"Error: Invalid parameter: {parameter}")
    logger.error(f"Invalid parameter: {parameter}")
```

### 7. Configure Log Levels Appropriately

Configure log levels appropriately:

```python
# Development environment
logger.set_level("DEBUG")

# Production environment
logger.set_level("INFO")
```

### 8. Use Log Monitoring

Use log monitoring:

```python
from pynlm.logging import LogMonitor

monitor = LogMonitor()
monitor.start_monitoring("nlm.log")
```

## Log Troubleshooting

### Common Issues

1. **Log Messages Not Appearing**:
   - Check log level
   - Check handler configuration
   - Check file permissions

2. **Log Files Too Large**:
   - Configure log rotation
   - Delete old log files

3. **Log Messages Not Formatted Correctly**:
   - Check formatter configuration
   - Check log format string

### Solutions

1. **Check Log Configuration**:

```python
from pynlm.logging import Logger

logger = Logger()
print("Log level:", logger.get_level())
print("Handlers:", logger.get_handlers())
```

2. **Check Log Files**:

```python
import os

if os.path.exists("nlm.log"):
    with open("nlm.log", "r") as f:
        print(f.read())
else:
    print("Log file not found")
```

3. **Check Environment Variables**:

```bash
export NLM_LOG_LEVEL=DEBUG
export NLM_LOG_FILE=nlm.log
```

## Future Enhancements

### Advanced Features

- **Distributed Logging**: Log to multiple remote servers
- **Log Aggregation**: Aggregate logs from multiple sources
- **Real-time Log Analysis**: Analyze logs in real-time
- **Log-Based Monitoring**: Use logs for system monitoring
- **ML-based Log Analysis**: Use machine learning to analyze logs

### Integration

- **Integration with Monitoring Systems**: Integrate with Prometheus, Grafana
- **Integration with Alerting Systems**: Send alerts based on log messages
- **Integration with SIEM Systems**: Send logs to Security Information and Event Management systems

## API Reference

### Logger Class

- `set_level(level)`: Set log level
- `get_level()`: Get current log level
- `add_console_handler(format=None, level=None, formatter=None)`: Add console handler
- `add_file_handler(filename, max_size=None, backup_count=None, level=None, formatter=None)`: Add file handler
- `add_json_handler(filename, pretty_print=False, level=None, formatter=None)`: Add JSON handler
- `add_handler(handler_type, **kwargs)`: Add handler
- `debug(message, **kwargs)`: Log debug message
- `info(message, **kwargs)`: Log info message
- `warning(message, **kwargs)`: Log warning message
- `error(message, **kwargs)`: Log error message
- `critical(message, **kwargs)`: Log critical message
- `log(level, message, **kwargs)`: Log message at specified level
- `get_handlers()`: Get list of handlers
- `remove_handler(handler)`: Remove handler
- `clear_handlers()`: Remove all handlers

### ModuleLogger Class

- `set_level(level)`: Set log level for module
- `get_level()`: Get log level for module
- `debug(message, **kwargs)`: Log debug message for module
- `info(message, **kwargs)`: Log info message for module
- `warning(message, **kwargs)`: Log warning message for module
- `error(message, **kwargs)`: Log error message for module
- `critical(message, **kwargs)`: Log critical message for module

### AsyncLogger Class

- `log(message, level=None, **kwargs)`: Log message asynchronously
- `wait_for_completion()`: Wait for all log operations to complete

### PerformanceLogger Class

- `start_timing(name)`: Start timing for operation
- `end_timing(name)`: End timing for operation
- `log_performance(name, value)`: Log performance metric
- `log_memory_usage()`: Log memory usage
- `log_cpu_usage()`: Log CPU usage
- `get_performance_stats()`: Get performance statistics

### LogMonitor Class

- `start_monitoring(filename)`: Start monitoring log file
- `stop_monitoring()`: Stop monitoring
- `get_statistics()`: Get log statistics
- `get_log_lines(level=None, limit=None)`: Get log lines

### LogAnalyzer Class

- `analyze_file(filename)`: Analyze log file
- `analyze_string(log_string)`: Analyze log string
- `get_error_statistics()`: Get error statistics
- `get_performance_statistics()`: Get performance statistics

### LogSearch Class

- `search_file(filename, level=None, message_pattern=None)`: Search log file
- `search_time_range(filename, start_time, end_time)`: Search log file in time range

---
*This logging system provides comprehensive logging for NLM Python bindings, ensuring robust logging for development, testing, and production environments.*
