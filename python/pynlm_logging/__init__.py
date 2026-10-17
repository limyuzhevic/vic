# NLM Logging Package

This is the package initialization file for the NLM logging system.

## Overview

The NLM logging package provides comprehensive logging functionality for the NLM
(Neural Learning Machine) Python bindings. It includes loggers, performance monitors,
log analyzers, and various logging utilities.

## Package Structure

### Core Modules

- **`__init__.py`**: Package initialization
- **`logger.py`**: Main logger implementation
- **`performance_logger.py`**: Performance monitoring
- **`log_monitor.py`**: Log file monitoring
- **`log_analyzer.py`**: Log file analysis
- **`log_search.py`**: Log file search

### Utility Modules

- **`formats.py`**: Log formats and formatters
- **`handlers.py`**: Log handlers
- **`configuration.py`**: Log configuration
- **`exceptions.py`**: Log-related exceptions

### Testing Modules

- **`test_logger.py`**: Logger unit tests
- **`test_performance_logger.py`**: Performance logger unit tests
- **`test_log_monitor.py`**: Log monitor unit tests
- **`test_log_analyzer.py`**: Log analyzer unit tests
- **`test_log_search.py`**: Log search unit tests
- **`test_logging_integration.py`**: Integration tests

## Package Features

### 1. Comprehensive Logging

The logging system provides:

- **Multiple Log Levels**: DEBUG, INFO, WARNING, ERROR, CRITICAL
- **Structured Logging**: JSON and text formats
- **Multiple Outputs**: Console, file, and network logging
- **Configurable Log Levels**: Per-module and global log levels

### 2. Performance Monitoring

Performance monitoring includes:

- **Timing Operations**: Start/end timing for operations
- **Performance Metrics**: Logging of performance metrics
- **Memory Usage**: Memory usage tracking
- **CPU Usage**: CPU usage tracking
- **Performance Analysis**: Performance analysis and reporting

### 3. Log Monitoring

Log monitoring includes:

- **File Monitoring**: Real-time monitoring of log files
- **Statistics**: Log file statistics
- **Alerting**: Alerting based on log content
- **Monitoring**: Continuous monitoring of log activity

### 4. Log Analysis

Log analysis includes:

- **File Analysis**: Analysis of log files
- **Error Statistics**: Statistics about errors in logs
- **Performance Statistics**: Statistics about performance in logs
- **Pattern Detection**: Detection of patterns in logs
- **Anomaly Detection**: Detection of anomalies in logs

### 5. Log Search

Log search includes:

- **Content Search**: Search for content in log files
- **Time Range Search**: Search for logs in time ranges
- **Level Search**: Search for logs at specific levels
- **Pattern Search**: Search for patterns in logs

## Installation

### Prerequisites

```bash
pip install -r requirements.txt
```

### Installation

```bash
pip install -e .
```

## Usage

### Basic Usage

```python
from nlm.logging import Logger

# Create logger
logger = Logger()

# Set log level
logger.set_level("INFO")

# Add handlers
logger.add_console_handler()
logger.add_file_handler("nlm.log")

# Log messages
logger.info("Starting NLM simulation")
logger.debug(f"Created brain with {brain.getTotalNeuronCount()} neurons")
brain.initialize()
logger.info("Brain initialized successfully")
```

### Performance Logging

```python
from nlm.logging import PerformanceLogger

perf_logger = PerformanceLogger()

# Log performance
for step in range(1000):
    perf_logger.start_timing("brain_step")
    brain.step(step, 0.001)
    perf_logger.end_timing("brain_step")
    
    if step % 100 == 0:
        perf_logger.log_performance("brain_step", 0.001)
        perf_logger.log_memory_usage()
```

### Log Monitoring

```python
from nlm.logging import LogMonitor

monitor = LogMonitor()
monitor.start_monitoring("nlm.log")

# Wait for monitoring
time.sleep(1)

# Get statistics
stats = monitor.get_statistics()
print(f"Log rate: {stats.log_rate} logs/second")
print(f"Error count: {stats.error_count}")

monitor.stop_monitoring()
```

### Log Analysis

```python
from nlm.logging import LogAnalyzer

analyzer = LogAnalyzer()
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

```python
from nlm.logging import LogSearch

search = LogSearch()

# Search for errors
errors = search.search_file("nlm.log", level="ERROR")

# Search for messages
messages = search.search_file("nlm.log", message_pattern="Test")

# Search in time range
from datetime import datetime, timedelta
start_time = datetime.now() - timedelta(hours=1)
end_time = datetime.now()
timeline = search.search_time_range("nlm.log", start_time, end_time)
```

## Configuration

### Configuration File

Log configuration can be stored in `config.json`:

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
        }
    ],
    "modules": {
        "Brain": "INFO",
        "Memory": "DEBUG",
        "Plasticity": "WARNING"
    }
}
```

### Environment Variables

```bash
export NLM_LOG_LEVEL=INFO
export NLM_LOG_FILE=nlm.log
export NLM_LOG_FORMAT=text
```

## Testing

### Running Tests

```bash
pytest
```

### Running Specific Tests

```bash
pytest nlm_logging/tests/test_logger.py
pytest nlm_logging/tests/test_performance_logger.py
pytest nlm_logging/tests/test_log_monitor.py
pytest nlm_logging/tests/test_log_analyzer.py
pytest nlm_logging/tests/test_log_search.py
```

### Running Tests with Coverage

```bash
pytest --cov=nlm_logging --cov-report=html
```

## Testing

### Test Coverage

The testing package includes comprehensive tests:

- **Unit Tests**: Tests for individual components
- **Integration Tests**: Tests for integrated functionality
- **Performance Tests**: Tests for performance monitoring
- **End-to-End Tests**: Tests for complete workflows

### Test Fixtures

Test fixtures include:

- **Log Fixtures**: Fixtures for testing loggers
- **Performance Fixtures**: Fixtures for testing performance logging
- **Monitor Fixtures**: Fixtures for testing log monitoring
- **Analyzer Fixtures**: Fixtures for testing log analysis

### Test Examples

#### Basic Logger Test

```python
def test_logger_basic_functionality():
    """Test basic logger functionality."""
    logger = Logger()
    logger.set_level("INFO")
    
    logger.debug("Debug message")
    logger.info("Info message")
    logger.warning("Warning message")
    logger.error("Error message")
    logger.critical("Critical message")
    
    assert logger.get_level() == "INFO"
```

#### Performance Logger Test

```python
def test_performance_logger():
    """Test performance logger functionality."""
    perf_logger = PerformanceLogger()
    
    perf_logger.start_timing("test_operation")
    time.sleep(0.01)
    perf_logger.end_timing("test_operation")
    
    perf_logger.log_performance("test_operation", 0.001)
    
    stats = perf_logger.get_performance_stats()
    assert "test_operation" in stats
```

#### Log Monitor Test

```python
def test_log_monitor():
    """Test log monitor functionality."""
    monitor = LogMonitor()
    
    with open("test_monitor.log", "w") as f:
        f.write("2023-10-15 14:30:45 INFO Test message\n")
    
    monitor.start_monitoring("test_monitor.log")
    time.sleep(0.1)
    
    stats = monitor.get_statistics()
    assert stats["test_monitor.log"]["info_count"] == 1
    
    monitor.stop_monitoring()
    os.remove("test_monitor.log")
```

## Performance

### Performance Considerations

1. **Asynchronous Logging**: Use `AsyncLogger` for non-blocking log operations
2. **Log Rotation**: Configure log rotation to prevent disk space issues
3. **Log Level Filtering**: Use appropriate log levels to reduce log volume
4. **Memory Usage**: Monitor memory usage when logging large volumes

### Performance Metrics

Performance metrics include:

- **Log Rate**: Number of logs per second
- **Log Size**: Size of log files
- **Memory Usage**: Memory usage for logging
- **CPU Usage**: CPU usage for logging

## Error Handling

### Error Handling

The logging system includes error handling for:

- **Invalid Log Levels**: Validation of log level values
- **File Errors**: Errors when writing to log files
- **Memory Errors**: Errors when memory is exhausted
- **Configuration Errors**: Errors in log configuration

### Error Examples

```python
try:
    logger = Logger()
    logger.add_file_handler("invalid/path/log.log")
except Exception as e:
    print(f"Error adding file handler: {e}")
```

## Future Enhancements

### Planned Features

1. **Distributed Logging**: Log to multiple remote servers
2. **Log Aggregation**: Aggregate logs from multiple sources
3. **Real-time Log Analysis**: Analyze logs in real-time
4. **ML-based Log Analysis**: Use machine learning to analyze logs
5. **Log-based Monitoring**: Use logs for system monitoring

### Integration

1. **Integration with Monitoring Systems**: Integrate with Prometheus, Grafana
2. **Integration with Alerting Systems**: Send alerts based on log messages
3. **Integration with SIEM Systems**: Send logs to Security Information and Event Management systems

## Dependencies

### Python Dependencies

- `pytest`: Testing framework
- `time`: Time module for timing operations
- `os`: Operating system module for file operations
- `json`: JSON module for JSON logging
- `datetime`: Date and time module for time range searches

### System Dependencies

- **File System**: Read/write access to log files
- **Memory**: Sufficient memory for logging
- **CPU**: CPU resources for logging operations

## License

MIT License

## Copyright

Copyright 2026 NLM Project

---
*This logging package provides comprehensive logging functionality for the NLM Python bindings.*
