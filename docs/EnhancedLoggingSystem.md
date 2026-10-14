# NLM Enhanced Logging System

## Overview

This document describes the enhanced logging and metrics collection system for NLM (Neural Learning Machine). The system provides comprehensive performance monitoring, structured output, and detailed metrics for neural simulations.

## Key Components

### 1. PerformanceMetricsLogger
Tracks simulation performance, neural dynamics, and system resources.

**Features:**
- Simulation timing and progress tracking
- Neural activity monitoring (firing rates, synaptic weights)
- Memory usage and system resource monitoring
- Performance analysis and reporting
- Context-aware logging with correlation IDs

**Usage:**
```cpp
auto& logger = nlm::Logger::getGlobal();
auto perfLogger = std::make_shared<nlm::PerformanceMetricsLogger>();
logger.addLogger(perfLogger);

// Record performance metrics
perfLogger->recordStepStart(context);
perfLogger->recordSpike(context, firingRate);
perfLogger->recordMemoryUsage(context, memoryMB);
perfLogger->recordStepEnd(context);
```

### 2. EnhancedConsoleLogger
Advanced logging with filtering, structured output, and progress indicators.

**Features:**
- Log levels filtering by module/component
- Structured output formats (JSON, CSV, human-readable)
- Log rotation and file management
- Progress indicators and dashboards
- Color-coded output

**Usage:**
```cpp
auto consoleLogger = std::make_shared<nlm::EnhancedConsoleLogger>();
consoleLogger->setLevel(nlm::LogLevel::Info);
consoleLogger->setFilter("brain");  // Filter by component
consoleLogger->setOutputFormat("json");
consoleLogger->setLogRotation(true, 10 * 1024 * 1024, 5);
```

### 3. SystemMonitor
Tracks CPU, memory, disk I/O, and network activity.

**Features:**
- Real-time CPU and memory monitoring
- Disk I/O tracking (read/write bytes, rates)
- Network activity monitoring
- Performance benchmarking
- Resource usage alerts

**Usage:**
```cpp
nlm::SystemMonitor systemMonitor;
systemMonitor.startMonitoring();

double cpuUsage = systemMonitor.getCpuUsage();
double memoryUsage = systemMonitor.getMemoryUsage();
double memoryAvailable = systemMonitor.getMemoryAvailable();
```

### 4. NeuralPerformanceTracker
Specific metrics for neural simulation including firing rates and network dynamics.

**Features:**
- Neuron firing pattern tracking
- Synaptic weight change monitoring
- Network connectivity metrics
- Learning progress tracking
- Detailed neural analysis reports

**Usage:**
```cpp
nlm::NeuralPerformanceTracker neuralTracker;

neuralTracker.recordNeuronFiring(context, neuronId, firingRate, type, spikeTime);
neuralTracker.recordSynapticWeightChange(context, synapseId, oldWeight, newWeight, changeMagnitude);
neuralTracker.recordPopulationActivity(context, populationId, avgFiringRate, variance);
```

### 5. LogAggregator
Collects and analyzes logs across multiple components.

**Features:**
- Cross-component log collection
- Pattern analysis and anomaly detection
- Report generation
- Log level aggregation
- Contextual analysis

**Usage:**
```cpp
nlm::LogAggregator aggregator;

aggregator.addLogEntry(logEntry);
aggregator.addPerformanceMetrics(metrics);
aggregator.addNeuralMetrics(neuralMetrics);

std::cout << aggregator.generateLogSummary() << std::endl;
```

### 6. StructuredLogOutput
JSON and other structured formats for machine parsing.

**Features:**
- JSON output format for machine parsing
- CSV format for spreadsheet analysis
- Configurable output destinations
- Structured logging with context
- Custom formatting options

**Usage:**
```cpp
nlm::StructuredLogOutput structuredOutput;

structuredOutput.setJsonOutput(std::cout);
structuredOutput.setCsvOutput(std::cout, headers);
structuredOutput.setHumanReadableOutput(std::cout);

structuredOutput.logEntry(entry, context);
structuredOutput.logPerformanceMetrics(metrics, context);
structuredOutput.logNeuralMetrics(neuralMetrics, context);
```

### 7. PerformanceBenchmark
Detailed timing and performance measurements.

**Features:**
- Precise timing measurements
- Benchmark execution and comparison
- Detailed benchmark reports
- Export capabilities
- Iteration tracking

**Usage:**
```cpp
nlm::PerformanceBenchmark benchmark;

benchmark.runBenchmark("neural-computation", []() {
    // Benchmark code here
});

std::cout << benchmark.generateBenchmarkReport() << std::endl;
```

### 8. Context-Aware Logging
Track correlation IDs and context across components.

**Features:**
- Correlation ID tracking
- Component and simulation identification
- Step and time tracking
- Custom context fields
- Cross-component correlation

**Usage:**
```cpp
nlm::LoggingContext context;
context.correlationId = "sim-001";
context.componentId = "brain-simulator";
context.simulationId = "test-simulation";
context.stepNumber = 100;
context.simulationTime = 1500.0;

// Use context in logging operations
perfLogger->setCurrentContext(context);
```

## Integration

### System Initialization
```cpp
#include "core/Logger/EnhancedLogging.hpp"

int main() {
    // Initialize the enhanced logging system
    nlm::initializeLoggingSystem();
    
    // Your simulation code here
    
    // Shutdown the logging system
    nlm::shutdownLoggingSystem();
    
    return 0;
}
```

### Configuration
The enhanced logging system integrates seamlessly with the existing Logger framework. You can:

1. Add custom loggers alongside existing ones
2. Configure log levels and filtering
3. Enable structured output formats
4. Set up log rotation for file output
5. Monitor system resources in real-time

## File Structure

```
src/core/Logging/
├── Logging.hpp          # Main header with all logging classes
├── Logging.cpp          # Implementation of all logging classes

/src/core/Logger/
├── Logger.hpp           # Original NLM Logger interface
├── Logger.cpp           # Original NLM Logger implementation
├── EnhancedLogging.hpp   # Enhanced logging system interface
├── EnhancedLogging.cpp   # Enhanced logging system implementation

src/demo/
├── EnhancedLoggingDemo.cpp  # Demonstration of all features
```

## Build Instructions

The enhanced logging system is automatically included in the NLM build process:

1. The `Logging.cpp` and `EnhancedLogging.cpp` files are added to the `nlm_core` library in `CMakeLists.txt`
2. Include `#include "core/Logger/EnhancedLogging.hpp"` in your source files
3. Link against the `nlm_core` library

## Examples

See `src/demo/EnhancedLoggingDemo.cpp` for a complete demonstration of all features.

## Performance Considerations

The enhanced logging system is designed to be lightweight and minimally invasive:

- Logging operations are conditional and check log levels before performing work
- Thread-safe operations with minimal locking
- Context tracking is lightweight and memory-efficient
- Benchmark timers are optimized for low overhead

## Customization

You can extend the logging system by:

1. Creating custom logger implementations that inherit from `ILogger`
2. Adding custom metrics tracking to `PerformanceMetrics`
3. Implementing custom context classes
4. Extending the structured output formats

## Troubleshooting

### Common Issues

1. **Log files not created**: Ensure the output file path is valid and writable
2. **Performance degradation**: Disable logging at high log levels in production
3. **Memory usage**: Monitor memory usage with `SystemMonitor`
4. **Thread safety**: All logging operations are thread-safe

### Getting Help

- Check the demo code for usage examples
- Refer to the NLM documentation for additional information
- Report issues to the NLM development team

## Version History

- **v0.1.0**: Initial implementation of enhanced logging system
- **Features**: All 8 components implemented
- **Performance**: Optimized for neural simulation workloads
- **Integration**: Seams with existing NLM Logger framework

## License

This enhanced logging system is part of the NLM project under the MIT License.
