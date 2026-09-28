# NLM Enhanced Logging and Debugging System

This document provides a comprehensive overview of the enhanced logging and debugging system for the NLM (Neural Learning Machine) project.

## Overview

The enhanced logging system provides:
- **Structured logging** with JSON output
- **Performance metrics tracking**
- **Memory usage monitoring and leak detection**
- **Real-time debug console** for system monitoring
- **Asynchronous logging** for performance
- **Correlation ID tracking** for distributed tracing
- **Log level filtering and hierarchies**
- **File rotation and management**
- **Integration with all NLM components**

## Architecture

### Core Components

1. **Advanced Logger**
   - JSON-based structured logging
   - Multiple log levels with configurable filtering
   - Correlation ID tracking
   - Performance metrics integration

2. **Logger Manager**
   - Centralized logging configuration
   - Log level hierarchies and filtering
   - Performance thresholds and alerting
   - File rotation management
   - System monitoring integration

3. **Debug Console**
   - Real-time system monitoring interface
   - Visual representation of system metrics
   - Log filtering and display
   - Export functionality for reports

4. **System Monitor**
   - CPU, memory, and thread usage tracking
   - Component status monitoring
   - Performance metric collection
   - Health checks and alerting

5. **Performance Profiler**
   - Detailed timing measurements
   - Profile categorization and comparison
   - Threshold-based alerting
   - Export for analysis

6. **Memory Tracker**
   - Allocation/deallocation tracking
   - Memory leak detection
   - Peak usage monitoring
   - Context-based analysis

### Integration Points

The enhanced logging system integrates with all NLM components:

- **Brain Simulation**: Neuron activation, synapse formation, plasticity
- **Environment Simulation**: Step timing, state changes
- **Experiment Components**: Run metrics, data collection
- **Memory Systems**: Working memory, episodic memory
- **Development Systems**: Synaptogenesis, pruning, maturation

## Configuration

### Basic Configuration

```cpp
// Initialize the enhanced logging system
nlm::integration::initializeEnhancedLogging();

// Configure brain component logging
auto brainLogger = nlm::LoggerManager::getInstance().getLogger("brain");
brainLogger->setLevel(nlm::LogLevel::Debug);
brainLogger->setFormat("json");
```

### Performance Configuration

```cpp
// Set performance thresholds
auto& loggerManager = nlm::LoggerManager::getInstance();
loggerManager.setPerformanceThreshold("neuron_step", 100.0); // ms
loggerManager.setMemoryThreshold(512 * 1024 * 1024); // 512MB
loggerManager.setCpuThreshold(80.0); // 80%
```

### Debug Console Configuration

```cpp
// Configure and show debug console
auto debugConsole = nlm::DebugConsole::getInstance();
debugConsole->show();
debugConsole->setPerformanceMode(true);
debugConsole->setDebugMode(true);
```

## Usage Examples

### Basic Logging

```cpp
#include "core/Logger/Logger.hpp"
#include "core/Integration.hpp"

int main() {
    // Initialize enhanced logging
    nlm::integration::initializeEnhancedLogging();
    
    // Basic logging
    NLM_LOG_INFO("Simulation started");
    NLM_LOG_DEBUG("Initializing brain components");
    
    // Integration macros
    NLM_LOG_PERFORMANCE("neuron_activation", 45.2);
    NLM_LOG_MEMORY("neuron_allocation", 1024);
    
    return 0;
}
```

### Structured Logging

```cpp
#include "core/Logger/Logger.hpp"

void logNeuronEvent(nlm::NeuronId neuronId, const std::string& eventType, const nlohmann::json& data) {
    auto logger = nlm::LoggerManager::getInstance().getLogger("neuron");
    logger->logStructuredEvent("neuron", neuronId, eventType, data);
}
```

### Performance Monitoring

```cpp
#include "core/Utilities/PerformanceProfiler.hpp"

void simulate() {
    auto profiler = nlm::PerformanceProfiler::getInstance();
    
    profiler->startProfile("network_update");
    // Perform network update
    profiler->stopProfile("network_update");
    
    double duration = profiler->getLastMeasurement("network_update");
    if (duration > 100.0) { // 100ms threshold
        NLM_LOG_WARNING("Network update took " + std::to_string(duration) + "ms");
    }
}
```

### Memory Leak Detection

```cpp
#include "core/Utilities/MemoryTracker.hpp"

void allocateMemory() {
    auto memoryTracker = nlm::MemoryTracker::getInstance();
    memoryTracker->setLeakDetectionEnabled(true);
    
    int* ptr = new int[100];
    memoryTracker->trackAllocation(ptr, 100 * sizeof(int), "simulation_temp");
    
    // Simulate leak
    // delete[] ptr; // Comment out to simulate leak
    
    if (memoryTracker->detectMemoryLeaks()) {
        NLM_LOG_WARNING("Memory leak detected in simulation");
    }
}
```

## Debug Console Features

### System Status Dashboard

The debug console provides real-time visualization of:

- **Memory Usage**: Current, peak, and percentage
- **CPU Usage**: Overall and per-core usage
- **Active Threads**: Thread count and state
- **Component Status**: OK, warning, error, critical
- **Performance Metrics**: Real-time measurements
- **Recent Logs**: Filtered and searchable log messages

### Export Functionality

```cpp
// Export all debug reports
NLM_EXPORT_DEBUG_REPORTS();

// Show system status in debug console
NLM_SHOW_SYSTEM_STATUS();
```

## Integration with NLM Components

### Brain Component Integration

```cpp
void nlm::integration::integrateBrainComponents() {
    auto& loggerManager = LoggerManager::getInstance();
    
    // Integrate neuron logging
    auto neuronLogger = loggerManager.getLogger("neuron");
    neuronLogger->setLevel(LogLevel::Debug);
    neuronLogger->setFormat("json");
    
    // Integrate synapse logging
    auto synapseLogger = loggerManager.getLogger("synapse");
    synapseLogger->setLevel(LogLevel::Info);
    
    // Set up performance tracking
    auto performanceProfiler = PerformanceProfiler::getInstance();
    performanceProfiler->startProfile("neuron_activation");
    performanceProfiler->startProfile("synapse_formation");
    
    // Configure memory tracking
    auto memoryTracker = MemoryTracker::getInstance();
    memoryTracker->setAllocationTrackingEnabled(true);
}
```

### Environment Component Integration

```cpp
void nlm::integration::integrateEnvironmentComponents() {
    auto& loggerManager = LoggerManager::getInstance();
    
    auto envLogger = loggerManager.getLogger("environment");
    envLogger->setLevel(LogLevel::Info);
    envLogger->setFormat("json");
    
    auto systemMonitor = SystemMonitor::getInstance();
    systemMonitor->updateComponentStatus("environment", "ok", "Environment simulation running");
}
```

## Configuration File Support

### JSON Configuration File

Create a `nlm_logging_config.json` file:

```json
{
    "loggers": {
        "brain": {
            "level": "debug",
            "format": "json",
            "output": "brain_logs.json"
        },
        "environment": {
            "level": "info",
            "format": "json",
            "output": "environment_logs.json"
        }
    },
    "performance": {
        "thresholds": {
            "neuron_step": 100.0,
            "network_update": 50.0
        }
    },
    "memory": {
        "track_leaks": true,
        "leak_threshold": 1048576
    }
}
```

### Loading Configuration

```cpp
#include "core/Logging/LoggerManager.hpp"

void loadLoggingConfig(const std::string& configFile) {
    // Implementation would parse JSON and configure loggers
    // This is a placeholder for future implementation
}
```

## Performance Considerations

### Async Logging

Async logging is enabled by default for performance:

```cpp
// Configure async logging
loggerManager.setGlobalConfig("json", true, 10000); // 10K queue size
```

### File Rotation

File rotation is enabled by default:

```cpp
// Configure file rotation
loggerManager.setLogFileRotation("logs/nlm_debug", 10 * 1024 * 1024, 5);
```

### Memory Usage

The memory tracker provides:
- Real-time memory usage monitoring
- Peak memory usage tracking
- Leak detection
- Allocation/deallocation tracking

## Debugging Tips

### Common Issues and Solutions

1. **No logs appearing**: Check log level configuration
2. **Debug console not visible**: Call `show()` method
3. **Memory leaks detected**: Check for missing `delete[]` or `delete`
4. **Performance alerts**: Check debug console performance metrics
5. **File rotation not working**: Check file permissions and disk space

### Debugging Techniques

1. **Enable debug mode**: `debugConsole->setDebugMode(true)`
2. **Filter logs**: Use module and level filters
3. **Export reports**: Use export functionality for analysis
4. **Monitor system**: Use system monitor for resource tracking
5. **Profile performance**: Use performance profiler for timing

## Migration Guide

### From Basic to Enhanced Logging

1. **Add integration header**:
   ```cpp
   #include "core/Integration.hpp"
   ```

2. **Initialize enhanced logging**:
   ```cpp
   nlm::integration::initializeEnhancedLogging();
   ```

3. **Replace basic macros** with enhanced macros:
   ```cpp
   // Before
   NLM_LOG_INFO("message");
   
   // After (same syntax, enhanced functionality)
   NLM_LOG_INFO("message");
   ```

4. **Integrate with components**:
   ```cpp
   nlm::integration::integrateBrainComponents();
   nlm::integration::integrateEnvironmentComponents();
   ```

## API Reference

### Core Classes

- **LoggerManager**: Centralized logging configuration
- **DebugConsole**: Real-time monitoring interface
- **SystemMonitor**: System resource monitoring
- **PerformanceProfiler**: Performance timing and analysis
- **MemoryTracker**: Memory usage and leak detection

### Core Methods

- **LoggerManager::getInstance()**: Get singleton instance
- **LoggerManager::configure()**: Configure logger
- **LoggerManager::setMinimumLogLevel()**: Set global log level
- **LoggerManager::setPerformanceThreshold()**: Set performance threshold
- **LoggerManager::startSystemMonitoring()**: Start system monitoring
- **LoggerManager::stopSystemMonitoring()**: Stop system monitoring
- **DebugConsole::show()**: Show debug console
- **DebugConsole::hide()**: Hide debug console
- **DebugConsole::toggleVisibility()**: Toggle debug console visibility
- **SystemMonitor::update()**: Update system metrics
- **PerformanceProfiler::startProfile()**: Start profiling
- **PerformanceProfiler::stopProfile()**: Stop profiling
- **MemoryTracker::trackAllocation()**: Track memory allocation
- **MemoryTracker::trackDeallocation()**: Track memory deallocation

## Installation and Usage

### Building with Enhanced Logging

1. **Ensure CMakeLists.txt includes new files**:
   ```cmake
   src/core/Logging/AdvancedLogger.cpp
   src/core/Logging/AsyncLogger.cpp
   src/core/Logging/StructuredLogger.cpp
   src/core/Logging/LoggerManager.cpp
   src/core/Debug/DebugConsole.cpp
   src/core/Debug/SystemMonitor.cpp
   src/core/Utilities/PerformanceProfiler.cpp
   src/core/Utilities/MemoryTracker.cpp
   ```

2. **Add nlohmann/json dependency**:
   ```cmake
   find_package(nlohmann_json REQUIRED)
   target_link_libraries(your_target PRIVATE nlohmann_json::nlohmann_json)
   ```

3. **Enable integration in main**:
   ```cpp
   #include "core/Integration.hpp"
   
   int main() {
       nlm::integration::initializeEnhancedLogging();
       // ... rest of your code
       return 0;
   }
   ```

### Running Debug Console

1. **Enable debug mode** in configuration
2. **Start simulation**
3. **Debug console will appear automatically**
4. **Use keyboard shortcuts**:
   - `F1`: Show/hide debug console
   - `F2`: Export reports
   - `F3`: Clear logs

### Performance Monitoring

1. **Enable performance mode**: `debugConsole->setPerformanceMode(true)`
2. **Monitor in real-time**: Debug console displays performance metrics
3. **Set thresholds**: Use `LoggerManager::setPerformanceThreshold()`
4. **Analyze reports**: Export performance data for analysis

## Testing

### Unit Tests

Run the enhanced logging tests:

```bash
cd /path/to/nlm_project
mkdir build
cd build
cmake ..
make
./nlm_test tests/test_logger.cpp
```

### Integration Tests

Test integration with NLM components:

```bash
./nlm_phase3_demo  # This will use integrated logging
```

## Troubleshooting

### Common Issues

1. **JSON library not found**: Install nlohmann/json or update CMakeLists.txt
2. **Async logging not working**: Check thread support
3. **File rotation not working**: Check file permissions
4. **Debug console not appearing**: Check GUI dependencies
5. **Memory tracker not working**: Check system memory APIs

### Debugging Commands

```bash
# Enable verbose logging
echo "export NLM_LOG_LEVEL=DEBUG" >> ~/.bashrc

# Check system resources
free -h
iostat -x 1

# Monitor memory usage
valgrind --tool=massif ./nlm_simulation

# Check for memory leaks
valgrind --leak-check=full ./nlm_simulation
```

## Future Enhancements

### Planned Features

1. **Distributed tracing** with OpenTelemetry integration
2. **Structured log aggregation** with Elasticsearch support
3. **Real-time visualization** with web interface
4. **Log analytics** with machine learning
5. **Custom log sinks** for database integration

### Contributing

To contribute to the enhanced logging system:

1. **Follow existing code style** and conventions
2. **Add comprehensive tests** for new features
3. **Update documentation** with examples and API references
4. **Maintain backward compatibility** where possible
5. **Add integration points** for new NLM components

## Conclusion

The enhanced logging and debugging system provides comprehensive support for:

- **Structured logging** with JSON output
- **Performance monitoring** with detailed metrics
- **Memory management** with leak detection
- **Real-time debugging** with interactive console
- **System monitoring** with resource tracking
- **Integration** with all NLM components

This system significantly improves the debugging capabilities of NLM simulations, providing developers with the tools needed to analyze performance, detect issues, and optimize their simulations.

## See Also

- **NLM Documentation**: General NLM project documentation
- **API Reference**: Complete API documentation
- **Examples**: Example configurations and usage patterns
- **Troubleshooting**: Common issues and solutions

The enhanced logging system is designed to be practical, immediately useful, and highly extensible for debugging complex NLM simulations.