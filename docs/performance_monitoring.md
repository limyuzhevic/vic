# NLM Performance Monitoring and Debugging Tools

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Performance Monitoring and Debugging Tools. These tools enable advanced users to optimize, monitor, and debug neural simulations with real-time metrics, memory leak detection, performance profiling, and bottleneck analysis.

## Table of Contents

1. [Introduction](#introduction)
2. [Core Monitoring Classes](#core-monitoring-classes)
3. [Monitoring Tools](#monitoring-tools)
4. [Python Integration](#python-integration)
5. [Examples](#examples)
6. [API Reference](#api-reference)
7. [Performance Considerations](#performance-considerations)
8. [Configuration](#configuration)
9. [Troubleshooting](#troubleshooting)

## Introduction

The NLM Performance Monitoring and Debugging Tools provide a comprehensive suite of monitoring and debugging capabilities for neural simulations. These tools are designed to help users:

- Monitor real-time performance metrics
- Detect and analyze memory leaks
- Profile and optimize performance
- Identify and resolve bottlenecks
- Analyze neural activity patterns
- Track neuromodulation signals
- Generate detailed reports and visualizations

The monitoring system is modular and extensible, allowing users to enable only the components they need.

## Core Monitoring Classes

### 1. PerformanceMonitor

The PerformanceMonitor class provides real-time performance monitoring and metrics collection.

**Features:**
- Real-time metrics collection for neural activity, memory usage, neuromodulation, predictions, and cognitive performance
- Performance benchmarking (steps per second, memory allocation rates, I/O performance, CPU utilization)
- Real-time monitoring interface with dashboard, alerts, historical data, and export capabilities
- Alert system with configurable thresholds
- Historical data storage and trend analysis

**Key Methods:**
- `startMonitoring()`: Start performance monitoring
- `stopMonitoring()`: Stop performance monitoring
- `recordMetric(name, value)`: Record a performance metric
- `getMetrics(name, startTime, endTime)`: Retrieve metrics for a specific time range
- `setAlertThreshold(name, threshold)`: Set alert thresholds
- `addAlertListener(listener)`: Add an alert listener
- `generateDashboardHTML()`: Generate HTML dashboard
- `exportMetrics(filepath, format)`: Export metrics to file

**Usage Example:**
```cpp
#include "monitor.hpp"

nlm::monitor::NLMMonitor monitor;
monitor.configure({
    .enableRealTimeMonitoring = true,
    .enableMemoryTracking = true,
    .enableNeuralMonitoring = true,
    .enableNeuromodulationMonitoring = true,
    .enableLeakDetection = true,
    .enablePerformanceProfiling = true,
    .enableBottleneckAnalysis = true
});

monitor.startAllMonitoring();

// Record performance metrics
monitor.getPerformanceMonitor().recordMetric("cpu_usage", 0.5);
monitor.getPerformanceMonitor().recordMetric("memory_usage", 1024.0);

// Get current metrics
auto metrics = monitor.collectGlobalMetrics();
```

### 2. MemoryMonitor

The MemoryMonitor class provides comprehensive memory tracking and analysis.

**Features:**
- Memory tracking and analysis for working memory, episodic memory, and total memory footprint
- Allocation and deallocation tracking
- Memory pool management
- Memory leak detection
- Garbage collection analysis
- Memory efficiency analysis

**Key Methods:**
- `startTracking()`: Start memory tracking
- `stopTracking()`: Stop memory tracking
- `trackAllocation(size, source)`: Track memory allocation
- `trackDeallocation(size, source)`: Track memory deallocation
- `getTotalMemoryUsage()`: Get total memory usage
- `getCurrentMemoryUsage()`: Get current memory usage
- `detectMemoryLeaks()`: Detect memory leaks
- `getMemoryStats()`: Get memory statistics

**Usage Example:**
```cpp
auto& memoryMonitor = monitor.getMemoryMonitor();
memoryMonitor.startTracking();

// Track memory allocations
memoryMonitor.trackAllocation(1024, "simulation_step");
memoryMonitor.trackAllocation(512, "neural_computation");

// Check for leaks
if (memoryMonitor.hasMemoryLeaks()) {
    std::cout << "Memory leaks detected!" << std::endl;
}

memoryMonitor.stopTracking();
```

### 3. NeuralMonitor

The NeuralMonitor class provides neural activity monitoring and analysis.

**Features:**
- Real-time spike tracking and analysis
- Population-level activity monitoring
- Synaptic activity tracking
- Network topology analysis
- Synchronized group detection
- Burst neuron detection

**Key Methods:**
- `startMonitoring()`: Start neural monitoring
- `stopMonitoring()`: Stop neural monitoring
- `recordSpike(neuronId, timestamp)`: Record a neural spike
- `recordPopulationActivity(neurons, activity)`: Record population activity
- `recordSynapticActivity(activity)`: Record synaptic activity
- `getTotalSpikes()`: Get total spike count
- `getFiringNeurons()`: Get firing neuron count
- `getAverageFiringRate()`: Get average firing rate
- `analyzeNetworkTopology()`: Analyze network topology
- `detectSynchronizedGroups(threshold)`: Detect synchronized neuron groups

**Usage Example:**
```cpp
auto& neuralMonitor = monitor.getNeuralMonitor();
neuralMonitor.startMonitoring();

// Record neural activity
neuralMonitor.recordSpike(42, 123.45);
neuralMonitor.recordPopulationActivity({1, 2, 3, 4, 5}, 0.8);

// Get statistics
std::cout << "Total spikes: " << neuralMonitor.getTotalSpikes() << std::endl;
std::cout << "Average firing rate: " << neuralMonitor.getAverageFiringRate() << " Hz" << std::endl;

neuralMonitor.stopMonitoring();
```

### 4. NeuromodulationMonitor

The NeuromodulationMonitor class provides neuromodulation signal monitoring and analysis.

**Features:**
- Real-time neuromodulator signal monitoring (dopamine, curiosity, novelty, etc.)
- Signal timing and effectiveness analysis
- Effect tracking on neural activity
- Signal quality analysis
- Pattern detection

**Key Methods:**
- `startMonitoring()`: Start neuromodulation monitoring
- `stopMonitoring()`: Stop neuromodulation monitoring
- `recordNeuromodulatorSignal(name, level, timestamp)`: Record neuromodulator signal
- `getDopamineLevel()`: Get dopamine level
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error
- `analyzeSignalPatterns(name)`: Analyze signal patterns
- `getSignalToNoiseRatio(name)`: Get signal-to-noise ratio

**Usage Example:**
```cpp
auto& neuroModMonitor = monitor.getNeuromodulationMonitor();
neuroModMonitor.startMonitoring();

// Record neuromodulator signals
neuroModMonitor.recordNeuromodulatorSignal("dopamine", 0.8, 123.45);
neuroModMonitor.recordNeuromodulatorSignal("curiosity", 0.6, 123.46);
neuroModMonitor.recordNeuromodulatorSignal("novelty", 0.9, 123.47);

// Get current state
auto state = neuroModMonitor.getCurrentState();
std::cout << "Dopamine level: " << state.dopamine << std::endl;
std::cout << "Curiosity level: " << state.curiosity << std::endl;

neuroModMonitor.stopMonitoring();
```

## Monitoring Tools

### 1. MemoryLeakDetector

The MemoryLeakDetector class provides comprehensive memory leak detection and analysis.

**Features:**
- Allocation tracking for leak detection
- Memory block tracking for heap analysis
- Circular reference detection
- Memory pool integration
- Detailed leak reporting
- Stack trace capture

**Key Methods:**
- `startDetection()`: Start leak detection
- `stopDetection()`: Stop leak detection
- `trackAllocation(size, sourceFile, sourceLine, functionName)`: Track allocation
- `trackDeallocation(allocationId)`: Track deallocation
- `detectMemoryLeaks()`: Detect memory leaks
- `detectMemoryLeaksByType(typeName)`: Detect leaks by type
- `hasMemoryLeaks()`: Check for memory leaks
- `getTotalLeakedMemory()`: Get total leaked memory
- `generateLeakReport()`: Generate leak report
- `generateDetailedLeakReport()`: Generate detailed leak report

### 2. MemoryPoolManager

The MemoryPoolManager class provides memory pool management and optimization.

**Features:**
- Memory pool creation and management
- Allocation and deallocation from pools
- Fragmentation analysis and recovery
- Pool optimization and resizing
- Pool statistics and reporting
- Garbage collection integration

**Key Methods:**
- `createPool(poolName, blockSize, initialCapacity)`: Create a memory pool
- `allocate(poolName, size, sourceFile, sourceLine, functionName)`: Allocate from pool
- `deallocate(poolName, address)`: Deallocate from pool
- `deallocate(poolName, allocationId)`: Deallocate by allocation ID
- `getPoolStats(poolName)`: Get pool statistics
- `getAllPoolStats()`: Get all pool statistics
- `optimizePool(poolName)`: Optimize pool
- `resizePool(poolName, newCapacity)`: Resize pool
- `consolidatePool(poolName)`: Consolidate pool
- `generatePoolReport(poolName)`: Generate pool report

### 3. PerformanceProfiler

The PerformanceProfiler class provides performance profiling and analysis.

**Features:**
- Function-level profiling
- System metrics tracking
- Timeline data collection
- Hotspot detection
- Bottleneck analysis
- Comparative analysis

**Key Methods:**
- `startProfiling(profileId)`: Start profiling
- `stopProfiling()`: Stop profiling
- `startFunctionProfile(functionName, fileName, lineNumber, moduleName)`: Start function profiling
- `stopFunctionProfile()`: Stop function profiling
- `recordFunctionTime(time)`: Record function execution time
- `recordFunctionMemory(memory)`: Record function memory usage
- `recordSystemMetric(name, value)`: Record system metric
- `getFunctionProfile(functionName)`: Get function profile
- `getCurrentProfile()`: Get current profile
- `identifyHotspots()`: Identify hotspots
- `identifyBottlenecks()`: Identify bottlenecks
- `generateProfileReport()`: Generate profile report

### 4. BottleneckAnalyzer

The BottleneckAnalyzer class provides bottleneck analysis and optimization recommendations.

**Features:**
- CPU bottleneck detection
- Memory bottleneck detection
- I/O bottleneck detection
- Network bottleneck detection
- Algorithm bottleneck detection
- Time-based trend analysis
- Performance prediction

**Key Methods:**
- `startAnalysis()`: Start bottleneck analysis
- `stopAnalysis()`: Stop bottleneck analysis
- `recordResourceUsage(profile)`: Record resource usage
- `recordExecutionTrace(trace)`: Record execution trace
- `analyzeBottlenecks()`: Analyze bottlenecks
- `analyzeBottlenecksByType(type)`: Analyze bottlenecks by type
- `hasBottlenecks()`: Check for bottlenecks
- `analyzeCpuUsagePatterns()`: Analyze CPU usage patterns
- `analyzeMemoryUsagePatterns()`: Analyze memory usage patterns
- `identifySlowFunctions(thresholdTime)`: Identify slow functions
- `generateBottleneckReport()`: Generate bottleneck report

## Python Integration

The monitoring tools are fully integrated with Python through Pybind11 bindings. All C++ classes are exposed as Python classes, allowing users to use them directly from Python scripts.

### Installation

The Python bindings are automatically built as part of the NLM build system. After building NLM, you can use the monitoring tools in Python:

```python
import pynlm
from monitor import PerformanceMonitor, MemoryMonitor, NeuralMonitor, NeuromodulationMonitor

# Create monitor
monitor = nlm.Monitor()

# Configure monitoring
monitor.configure({
    'enableRealTimeMonitoring': True,
    'enableMemoryTracking': True,
    'enableNeuralMonitoring': True,
    'enableNeuromodulationMonitoring': True,
    'enableLeakDetection': True,
    'enablePerformanceProfiling': True,
    'enableBottleneckAnalysis': True
})

# Start monitoring
monitor.startAllMonitoring()

# Run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)
    # Monitoring is done automatically

# Stop monitoring
monitor.stopAllMonitoring()

# Generate reports
report = monitor.generateComprehensiveReport()
with open("report.txt", "w") as f:
    f.write(report)
```

### Available Python Classes

The following classes are available in Python:

1. **PerformanceMonitor**: Real-time performance monitoring
2. **MemoryMonitor**: Memory tracking and analysis
3. **NeuralMonitor**: Neural activity monitoring
4. **NeuromodulationMonitor**: Neuromodulation signal monitoring
5. **MemoryLeakDetector**: Memory leak detection
6. **MemoryPoolManager**: Memory pool management
7. **PerformanceProfiler**: Performance profiling
8. **BottleneckAnalyzer**: Bottleneck analysis
9. **NLMMonitor**: Main monitor that coordinates all components

### Python Examples

The `examples/monitor/` directory contains comprehensive Python examples:

1. **real_time_monitor.py**: Real-time monitoring example
2. **memory_leak_detection.py**: Memory leak detection example
3. **performance_profiling.py**: Performance profiling example
4. **bottleneck_analysis.py**: Bottleneck analysis example
5. **neural_activity_monitor.py**: Neural activity monitoring example

## Examples

### 1. Real-time Monitoring Example

The `real_time_monitor.py` example demonstrates real-time monitoring of neural simulations, including performance metrics, neural activity, and memory usage.

### 2. Memory Leak Detection Example

The `memory_leak_detection.py` example demonstrates memory leak detection and analysis, showing how to track allocations and detect memory leaks.

### 3. Performance Profiling Example

The `performance_profiling.py` example demonstrates performance profiling, including function-level analysis and hotspot detection.

### 4. Bottleneck Analysis Example

The `bottleneck_analysis.py` example demonstrates bottleneck analysis, including resource usage patterns and optimization recommendations.

### 5. Neural Activity Monitoring Example

The `neural_activity_monitor.py` example demonstrates neural activity monitoring, including spike detection, population activity analysis, and network topology analysis.

## API Reference

### Configuration Structure

The `MonitoringConfig` structure defines the configuration for the NLM monitor:

```cpp
struct MonitoringConfig {
    bool enableRealTimeMonitoring;
    bool enableMemoryTracking;
    bool enableNeuralMonitoring;
    bool enableNeuromodulationMonitoring;
    bool enableLeakDetection;
    bool enablePerformanceProfiling;
    bool enableBottleneckAnalysis;
    Timestamp retentionTime;
    double alertThreshold;
    std::string exportFormat;
};
```

### Global Metrics Structure

The `MonitoringMetrics` structure contains global monitoring metrics:

```cpp
struct MonitoringMetrics {
    double cpuUsage;
    double memoryUsage;
    double firingRate;
    double predictionError;
    double noveltyLevel;
    double curiosityLevel;
    double dopamineLevel;
    Timestamp timestamp;
};
```

## Performance Considerations

### 1. Minimal Overhead

The monitoring tools are designed to have minimal overhead when not in use. Components are only enabled when explicitly requested.

### 2. Efficient Data Structures

The monitoring system uses efficient data structures to minimize memory usage and maximize performance.

### 3. Configurable Sampling

Users can configure sampling rates to balance between performance and detail.

### 4. Parallel Processing

Where applicable, the monitoring tools use parallel processing to minimize impact on simulation performance.

## Configuration

### CMake Options

The NLM monitoring system is controlled by the following CMake options:

- `NLM_ENABLE_MONITORING`: Enable the monitoring system (default: ON)
- `NLM_MONITORING_TYPE`: Monitoring implementation type (default: "comprehensive")

### Configuration File

Users can create a `monitor_config.json` file to configure the monitoring system:

```json
{
    "enableRealTimeMonitoring": true,
    "enableMemoryTracking": true,
    "enableNeuralMonitoring": true,
    "enableNeuromodulationMonitoring": true,
    "enableLeakDetection": true,
    "enablePerformanceProfiling": true,
    "enableBottleneckAnalysis": true,
    "retentionTime": 3600.0,
    "alertThreshold": 0.8,
    "exportFormat": "json"
}
```

## Troubleshooting

### Common Issues

1. **No monitoring data captured**
   - Ensure monitoring is started with `startAllMonitoring()`
   - Check that the correct components are enabled in the configuration
   - Verify that the simulation is actually running

2. **Memory leaks detected**
   - Check for proper deallocation in the simulation code
   - Verify that memory tracking is configured correctly
   - Consider increasing the allocation threshold

3. **Performance bottlenecks**
   - Profile the simulation to identify specific bottlenecks
   - Use the optimization recommendations from the analysis reports
   - Consider upgrading hardware or optimizing algorithms

4. **High memory usage**
   - Check for memory leaks or inefficient data structures
   - Consider using memory pools for frequent allocations
   - Monitor memory usage patterns over time

### Debugging Tips

1. **Enable verbose logging** to see detailed monitoring information
2. **Use smaller simulation runs** to isolate issues
3. **Check component configurations** to ensure all needed components are enabled
4. **Review export files** to analyze captured data
5. **Use the examples** as starting points for custom monitoring scripts

## Conclusion

The NLM Performance Monitoring and Debugging Tools provide a comprehensive solution for monitoring and debugging neural simulations. With real-time metrics, memory leak detection, performance profiling, and bottleneck analysis, users can optimize their simulations for maximum performance and reliability.

The modular design allows users to enable only the components they need, minimizing overhead while providing maximum flexibility and insight into simulation performance.

For more information, refer to the API documentation, examples, and configuration files included with this system.