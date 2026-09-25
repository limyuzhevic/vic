# NLM Enhanced Logging and Monitoring System

## Overview

This document provides comprehensive information about the enhanced logging and monitoring system integrated into the NLM (Neural Learning Machine) project. The system provides real-time monitoring, performance tracking, and comprehensive log management capabilities for the neural network simulation.

## Features

### Core Logging Components

#### 1. Advanced Logger Base Class
- **Multi-level logging**: INFO, WARNING, ERROR, DEBUG, TRACE, CRITICAL, FATAL
- **Structured logging**: Context-aware log messages with structured data
- **Performance metrics integration**: Real-time performance monitoring
- **Thread-safe operations**: Safe for multi-threaded environments
- **Log filtering and routing**: Flexible filtering by level, category, and custom criteria
- **Asynchronous logging**: Background logging to prevent performance impact

#### 2. Performance Monitoring
- **Detailed timing metrics**: Microsecond precision timing for all operations
- **Memory usage tracking**: Real-time memory monitoring with alerts
- **Network I/O metrics**: Network latency and throughput monitoring
- **CPU profiling**: CPU usage and performance analysis
- **Custom metrics collection**: User-defined metric tracking
- **Performance baselines**: Comparison against historical performance

#### 3. Runtime Metrics Collection
- **Brain state monitoring**: Neural network state tracking
- **Simulation metrics**: Step timing, spike counts, firing rates
- **Memory pool tracking**: Memory allocation/deallocation tracking
- **Event system monitoring**: Event queue depth and processing times
- **Neuromodulator tracking**: Neuromodulator level changes and effects

#### 4. Advanced Log Formatting
- **JSON logging**: Structured JSON output for machine parsing
- **CSV logging**: Comma-separated values for data analysis
- **HTML logging**: Interactive HTML log reports
- **Log visualization**: Chart generation from log data
- **Color-coded output**: Terminal color coding for better readability

#### 5. Real-time Monitoring Interface
- **Web-based dashboard**: Real-time web interface for monitoring
- **REST API**: Programmatic access to monitoring data
- **Graph generation**: Real-time graph visualization
- **Alert system**: Configurable alerts for threshold breaches

## Usage Examples

### Basic Logging

```cpp
#include "src/core/Logger/AdvancedLogger.hpp"

auto& logger = AdvancedLogger::getInstance();
logger.initialize("nlm.log", LogLevel::Info);

logger.info("Brain simulation started");
logger.warning("High memory usage detected");
logger.error("Failed to allocate memory pool");
```

### Structured Logging

```cpp
logger.logStructured(
    LogLevel::Info,
    "SimulationStep",
    std::map<std::string, std::string>{
        {"step", "100"},
        {"spike_count", "1024"},
        {"firing_rate", "5.2"},
        {"neurons_active", "128"}
    },
    "Simulation step completed successfully"
);
```

### Performance Monitoring

```cpp
#include "src/core/Logger/PerformanceMonitor.hpp"

auto& perfMonitor = PerformanceMonitor::getInstance();

// Start profiling an operation
perfMonitor.startProfiling("brain_initialization", {
    {"component", "brain"},
    {"stage", "initialization"}
});

// Simulate work
// ... perform operations ...

// Stop profiling
perfMonitor.stopProfiling("brain_initialization");

// Record custom metrics
perfMonitor.recordMetric("neuron_count", 1024);
perfMonitor.recordMetric("firing_rate", 5.2);
perfMonitor.recordTiming("decision_making", 0.15);
```

### Runtime Metrics Collection

```cpp
#include "src/core/Logger/RuntimeMetricsCollector.hpp"

auto& metricsCollector = RuntimeMetricsCollector::getInstance();

// Record brain state metrics
BrainStateMetrics brainMetrics;
brainMetrics.totalNeuronCount = 1024;
brainMetrics.activeNeuronCount = 128;
brainMetrics.firingRate = 5.2f;
metricsCollector.recordBrainMetrics(brainMetrics);

// Record simulation metrics
SimulationMetrics simMetrics;
simMetrics.step = 100;
simMetrics.stepDuration = std::chrono::milliseconds(50);
simMetrics.spikeCount = 1024;
simMetrics.firingRate = 5.2f;
metricsCollector.recordSimulationMetrics(simMetrics);
```

### Log Formatting and Export

```cpp
#include "src/core/Logger/LogFormatter.hpp"

auto& formatter = LogFormatter::getInstance();

// Export logs to different formats
formatter.exportToFile("simulation_logs", "logs.json", "json");
formatter.exportToFile("simulation_logs", "logs.csv", "csv");
formatter.exportToFile("simulation_logs", "logs.html", "html");

// Generate report
std::string report = formatter.generateReport("logs.json");
```

### Integration with NLM Systems

```cpp
void setupLoggingSystem() {
    auto& logger = AdvancedLogger::getInstance();
    auto& perfMonitor = PerformanceMonitor::getInstance();
    auto& metricsCollector = RuntimeMetricsCollector::getInstance();
    
    // Configure logging
    logger.initialize("nlm.log", LogLevel::Info);
    logger.enableAsyncLogging(true);
    logger.enablePerformanceLogging(true);
    
    // Configure performance monitoring
    perfMonitor.setCollectionInterval(std::chrono::milliseconds(100));
    perfMonitor.enableSystemMonitoring(true);
    
    // Configure runtime metrics
    metricsCollector.setCollectionInterval(std::chrono::milliseconds(100));
    metricsCollector.setMaxHistorySize(10000);
}

void integrateWithBrain(Brain& brain) {
    auto& logger = AdvancedLogger::getInstance();
    auto& perfMonitor = PerformanceMonitor::getInstance();
    
    // Monitor brain configuration
    auto config = brain.getConfig();
    if (config) {
        auto neuronCount = config->getOr<int>("neuron_count", 0);
        perfMonitor.recordMetric("config_neuron_count", neuronCount);
    }
    
    // Monitor brain state
    perfMonitor.recordBrainMetrics(brain);
    
    logger.info("Brain integration completed");
}
```

## Configuration

### Logging Configuration

```cpp
struct LoggingConfig {
    std::string name;
    LogLevel level;
    std::string outputPath;
    std::string format;
    bool useColors;
    bool asyncLogging;
    size_t maxFileSize;
    int maxFiles;
    bool enablePerformanceLogging;
    std::map<std::string, std::string> customSettings;
};
```

### Performance Monitoring Configuration

```cpp
struct PerformanceConfig {
    bool enableMemoryTracking;
    bool enableCPUTracking;
    bool enableNetworkTracking;
    double memoryThreshold;
    double cpuThreshold;
    double networkThreshold;
    std::chrono::seconds performanceBaselineInterval;
};
```

### Monitoring Configuration

```cpp
struct MonitoringConfig {
    std::string dashboardUrl;
    std::string dashboardApiKey;
    bool enableWebDashboard;
    bool enableRealTimeGraphs;
    std::chrono::seconds alertCheckInterval;
    std::chrono::seconds metricsCollectionInterval;
};
```

### System Configuration

```cpp
struct LoggingSystemConfig {
    LoggingConfig logging;
    PerformanceConfig performance;
    MonitoringConfig monitoring;
    
    bool validate() const {
        return !logging.name.empty();
    }
};
```

## Advanced Features

### Alert System

The logging system includes a comprehensive alert system that can notify when performance metrics exceed thresholds:

```cpp
// Add alert thresholds
perfMonitor.addAlert({
    "memory_usage_alert",
    "memory.used_percent",
    LogLevel::Warning,
    0.8,
    "Memory usage exceeded 80% threshold"
});

perfMonitor.addAlert({
    "cpu_usage_alert",
    "cpu.usage_percent", 
    LogLevel::Critical,
    0.95,
    "CPU usage exceeded 95% threshold"
});
```

### Log Filtering

Configure log filtering based on level, category, and custom criteria:

```cpp
LogFilter filter;
filter.minLevel = LogLevel::Warning;
filter.categories = {"brain", "simulation", "error"};
filter.includePerformanceLogs = false;

logger.setFilter(filter);
```

### Performance Baselines

Track and compare performance against historical baselines:

```cpp
auto& perfMonitor = PerformanceMonitor::getInstance();

// Update baseline
perfMonitor.updateBaseline("simulation_step");

// Compare with baseline
auto comparison = perfMonitor.compareWithBaseline("simulation_step", currentDuration);
```

### Real-time Monitoring

Access real-time metrics through the monitoring dashboard:

```cpp
auto& dashboard = MonitorDashboard::getInstance();

// Get current system metrics
auto metrics = dashboard.getCurrentMetrics();

// Get brain state
auto brainState = dashboard.getBrainState();

// Get performance data
auto performanceData = dashboard.getPerformanceData("1h");
```

## Integration Examples

### Integration with Brain Module

The logging system integrates seamlessly with the Brain module to monitor:

- Neuron counts and activity levels
- Spike generation patterns
- Synaptic activity and plasticity
- Simulation timing and performance

### Integration with Memory Systems

Monitor memory pool performance:

- Working memory usage and allocation
- Episodic memory tracking
- Associative memory metrics
- Memory pool utilization

### Integration with Neuromodulation

Track neuromodulator levels and effects:

- Dopamine levels and impact
- Curiosity and exploration behavior
- Novelty detection and response
- Reward prediction errors

### Integration with Development

Monitor developmental stages and changes:

- Developmental stage progression
- Plasticity rate changes
- Excitatory/inhibitory balance
- Developmental milestones

## Building and Running

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_LOGGING=ON \
    -DNLM_ENABLE_PERFORMANCE=ON \
    -DNLM_ENABLE_DEMO=ON

# Build the project
make -j$(nproc)
```

### Running Tests

```bash
# Run logging system tests
./test_logging_integration

# Run error handling tests
./test_error_handling

# Run all tests
ctest
```

### Running Demo Programs

```bash
# Run NLM Phase 6 demo
./nlm_phase6_demo

# Run logging system demo
./nlm_logging_demo
```

## Configuration Files

The logging system supports configuration files for:

### JSON Configuration
```json
{
    "logging": {
        "name": "nlm",
        "level": "info",
        "outputPath": "logs/nlm.log",
        "format": "console",
        "useColors": true,
        "asyncLogging": true,
        "maxFileSize": 10485760,
        "maxFiles": 5
    },
    "performance": {
        "enableMemoryTracking": true,
        "enableCPUTracking": true,
        "memoryThreshold": 0.8,
        "cpuThreshold": 0.9
    },
    "monitoring": {
        "enableWebDashboard": false,
        "updateInterval": 1000,
        "alertCheckInterval": 5000
    }
}
```

### Command Line Arguments

The logging system accepts the following command line arguments:

- `--log-level <level>`: Set minimum log level
- `--log-file <file>`: Set log output file
- `--log-format <format>`: Set log format (console, json, csv, html)
- `--performance-monitoring`: Enable performance monitoring
- `--real-time-dashboard`: Enable web dashboard

Example:
```bash
./nlm_main --log-level warning --log-file logs/nlm.log --log-format json
```

## Performance Considerations

### Asynchronous Logging

Asynchronous logging is enabled by default to minimize performance impact:

- Log entries are queued and processed in background threads
- Console output is buffered and flushed periodically
- File I/O operations are asynchronous

### Memory Management

The logging system includes memory management features:

- Automatic memory pool monitoring
- Memory usage alerts and thresholds
- Memory leak detection capabilities

### CPU Usage

Performance monitoring is designed to have minimal overhead:

- Sampling-based metrics collection
- Conditional logging based on levels
- Thread-safe operations with minimal lock contention

## Troubleshooting

### Common Issues and Solutions

#### 1. No logs appearing in output
- Check log level configuration
- Verify log output files are writable
- Ensure logging outputs are properly initialized

#### 2. Performance degradation
- Disable performance logging in production
- Reduce log file size and rotation frequency
- Use async logging for better performance

#### 3. Memory usage spikes
- Monitor memory pool usage
- Check for memory leaks in logging outputs
- Use file logging instead of console logging

#### 4. Web dashboard not working
- Check if all required dependencies are installed
- Verify network connectivity
- Check firewall settings

### Debugging Tips

1. **Enable debug logging**: Set log level to DEBUG to see all messages
2. **Check file permissions**: Ensure log files can be written to
3. **Monitor memory usage**: Use system monitoring tools
4. **Check disk space**: Ensure sufficient space for log files
5. **Review alert thresholds**: Adjust based on system performance

## Future Enhancements

### Planned Features

1. **Distributed Logging**: Support for distributed logging across multiple machines
2. **Log Analytics**: Advanced log analysis and pattern detection
3. **Machine Learning Integration**: ML-based anomaly detection in logs
4. **Cloud Integration**: Integration with cloud monitoring services
5. **Mobile Monitoring**: Support for mobile device monitoring
6. **Container Integration**: Native support for container environments

### Extension Points

The logging system is designed to be extensible:

- **Custom Log Outputs**: Implement custom log output formats
- **Custom Metrics**: Add new metric types and collectors
- **Alert Systems**: Create custom alert handlers
- **Dashboard Plugins**: Extend web dashboard functionality
- **Log Parsers**: Add support for new log file formats

## References

- [NLM Documentation](https://github.com/Kilo-Org/nlm)
- [C++ Standard Library](https://en.cppreference.com/w/cpp)
- [JSON for Modern C++](https://github.com/nlohmann/json)
- [WebSocket++](https://github.com/zaphoyd/websocketpp)

## License

This logging and monitoring system is part of the NLM project and is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

The enhanced logging and monitoring system was developed as part of the NLM Phase 6 integration effort, building upon the existing error handling system and adding comprehensive observability capabilities for the neural network simulation.