# NLM Performance Monitoring & Profiling System

This document provides comprehensive information about the NLM (Neural Learning Machine) performance monitoring and profiling system.

## Overview

The NLM performance monitoring and profiling system provides comprehensive tools for understanding, analyzing, and optimizing simulation performance. It includes:

- **Core Performance Monitoring**: Timing, memory, CPU, I/O, and event profiling
- **Simulation Performance Metrics**: Step performance, spike processing, synaptic updates, memory operations, neuromodulation, and development system performance
- **System Resource Monitoring**: Memory usage, CPU utilization, disk I/O, network operations, and thermal monitoring
- **Advanced Profiling**: Call stack profiling, memory leak detection, garbage collection profiling, thread profiling, and real-time monitoring
- **Performance Analysis Tools**: Performance reports, benchmark suites, performance comparison, optimization recommendations, and performance regression detection
- **Configuration-Based Profiling**: Command-line profiling scripts, profile duration control, output formats, profile triggers, and sampling control
- **Integration with Existing Systems**: CLI integration, Python API integration, web interface (optional), and export capabilities
- **Performance Bottleneck Detection**: Resource starvation detection, algorithmic inefficiencies, data structure analysis, network bottlenecks, and hardware utilization
- **Performance Optimization Tools**: Automatic optimization, configuration optimization, algorithm selection, hardware recommendations, and code optimization
- **Real-time Performance Visualization**: Live graphs, performance dashboards, alerting system, trend analysis, and anomaly detection

## Architecture

The performance monitoring system is organized into several key components:

### 1. Performance Monitor Class
- **Purpose**: Core performance monitoring class that tracks timing, memory, CPU, I/O, and events
- **Key Methods**:
  - `startTimer(name)`: Start timing a named operation
  - `stopTimer(name)`: Stop timing a named operation
  - `getElapsedTime(name)`: Get elapsed time for a named operation
  - `trackMemoryAllocation(size)`: Track memory allocation
  - `trackMemoryDeallocation(size)`: Track memory deallocation
  - `getCurrentMemoryUsage()`: Get current memory usage
  - `startCPUMeasurement()`: Start CPU measurement
  - `stopCPUMeasurement()`: Stop CPU measurement
  - `getCPUUtilization()`: Get CPU utilization
  - `trackRead(bytes)`: Track read I/O
  - `trackWrite(bytes)`: Track write I/O
  - `recordEvent(eventName)`: Record an event
  - `getEventCount(eventName)`: Get event count
  - `generatePerformanceReport(filename)`: Generate performance report
  - `generateBenchmarkReport(filename)`: Generate benchmark report
  - `optimizePerformance()`: Run performance optimization

### 2. Profiler Class
- **Purpose**: Main profiling framework for simulation steps and operations
- **Configuration**: ProfilerConfig with options for sampling interval, tracking, output format, etc.
- **Key Methods**:
  - `configure(config)`: Configure profiler
  - `startProfiling()`: Start profiling
  - `stopProfiling()`: Stop profiling
  - `profileStep(step, metrics)`: Profile a simulation step
  - `profileEvent(eventName, duration)`: Profile an event
  - `profileMemory(size, allocation)`: Profile memory operations
  - `profileCPU(time)`: Profile CPU usage
  - `profileIO(bytes, isRead)`: Profile I/O operations
  - `getProfileData()`: Get profile data
  - `getLastMetric()`: Get last metric
  - `getAverageStepTime()`: Get average step time
  - `getMedianStepTime()`: Get median step time
  - `exportProfile(filename)`: Export profile
  - `generateTextReport(output)`: Generate text report
  - `generateJSONReport(output)`: Generate JSON report
  - `generateCSVReport(output)`: Generate CSV report
  - `generateHTMLReport(output)`: Generate HTML report

### 3. Benchmark Suite
- **Purpose**: Benchmarking suite for performance testing
- **Key Components**:
  - Benchmark: Individual benchmark definition
  - BenchmarkResult: Results from running a benchmark
  - Preset benchmarks: Memory, CPU, I/O, simulation benchmarks
- **Key Methods**:
  - `addBenchmark(benchmark)`: Add a benchmark
  - `runBenchmark(name)`: Run a benchmark
  - `runAllBenchmarks()`: Run all benchmarks
  - `generateBenchmarkReport(filename)`: Generate benchmark report
  - `addDefaultBenchmarks()`: Add default benchmarks
  - `loadBenchmarksFromFile(filename)`: Load benchmarks from file

### 4. Performance Report Generator
- **Purpose**: Generate comprehensive performance reports in multiple formats
- **Output Formats**:
  - Text: Plain text report
  - JSON: JSON-formatted report
  - HTML: HTML-formatted report
  - CSV: CSV-formatted report
- **Key Methods**:
  - `addMetric(name, value, unit, category, operation)`: Add a metric
  - `generateTextReport(filename)`: Generate text report
  - `generateJSONReport(filename)`: Generate JSON report
  - `generateHTMLReport(filename)`: Generate HTML report
  - `generateCSVReport(filename)`: Generate CSV report
  - `generateOptimizationRecommendations()`: Get optimization recommendations

### 5. Resource Monitor
- **Purpose**: Monitor system resources (CPU, memory, disk, network, temperature)
- **ResourceUsage Structure**:
  - `cpuUsage`: CPU utilization (0.0 - 1.0)
  - `memoryUsage`: Current memory usage (bytes)
  - `memoryPeak`: Peak memory usage (bytes)
  - `diskRead`: Disk read bytes
  - `diskWrite`: Disk write bytes
  - `networkUpload`: Network upload (bytes/sec)
  - `networkDownload`: Network download (bytes/sec)
  - `temperature`: CPU temperature (Celsius)
  - `timestamp`: Timestamp
- **Key Methods**:
  - `startMonitoring()`: Start monitoring
  - `stopMonitoring()`: Stop monitoring
  - `getCurrentResourceUsage()`: Get current resource usage
  - `getResourceUsageHistory()`: Get resource usage history
  - `generateResourceReport(filename)`: Generate resource report
  - `identifyResourceBottlenecks()`: Identify resource bottlenecks

## Integration with NLM

### CLI Integration
The performance monitoring system integrates with the NLM CLI through the following commands:

- `--profile`: Enable performance profiling
- `--benchmark`: Run performance benchmarks
- `--benchmark-suite`: Run complete benchmark suite
- `--performance-report`: Generate performance report
- `--performance-stats`: Show performance statistics
- `--resource-monitor`: Enable resource monitoring
- `--resource-report`: Generate resource usage report
- `--performance-optimize`: Run performance optimization

### Python API Integration
The performance monitoring system provides Python bindings through `pybind11`, allowing users to:

- Create and configure performance monitors
- Start and stop profiling
- Access profiling data
- Generate reports
- Run benchmarks

### Web Interface (Optional)
An optional web-based performance dashboard can be implemented to provide real-time monitoring and visualization.

## Usage Examples

### Basic Performance Monitoring

```cpp
#include "performance_monitor.h"
#include "profiler.h"

int main() {
    // Create performance monitor and profiler
    PerformanceMonitor monitor;
    Profiler profiler("simulation_profiler", 0.1, 10000);
    
    // Start profiling
    profiler.startProfiling();
    
    // Profile simulation steps
    for (size_t step = 0; step < 1000; ++step) {
        // Start timers for key operations
        monitor.startTimer("plasticity_computation");
        monitor.startTimer("memory_operations");
        monitor.startTimer("neuromodulation");
        
        // Run simulation step
        brain->step(step, step * 0.001);
        
        // Stop timers
        monitor.stopTimer("plasticity_computation");
        monitor.stopTimer("memory_operations");
        monitor.stopTimer("neuromodulation");
        
        // Profile I/O
        monitor.trackRead(1024);
        monitor.trackWrite(512);
    }
    
    // Stop profiling
    profiler.stopProfiling();
    
    // Generate performance report
    monitor.generatePerformanceReport("performance_report.html");
    
    return 0;
}
```

### Benchmark Suite

```cpp
#include "benchmark.h"

int main() {
    BenchmarkSuite suite;
    
    // Add default benchmarks
    suite.addDefaultBenchmarks();
    suite.addSimulationBenchmarks();
    suite.addMemoryBenchmarks();
    suite.addCPUBenchmarks();
    
    // Run all benchmarks
    auto results = suite.runAllBenchmarks();
    
    // Generate benchmark report
    suite.generateBenchmarkReport("benchmark_report.html");
    
    return 0;
}
```

### Resource Monitoring

```cpp
#include "resources_monitor.h"

int main() {
    ResourceMonitor monitor;
    
    // Start monitoring
    monitor.startMonitoring();
    
    // Wait for a while
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    // Stop monitoring
    monitor.stopMonitoring();
    
    // Generate resource report
    monitor.generateResourceReport("resource_report.html");
    
    return 0;
}
```

### CLI Integration

```bash
# Enable performance profiling and run simulation
nlm --profile --steps 1000

# Run performance benchmarks
nlm --benchmark --benchmark-suite

# Generate performance report
nlm --performance-report

# Enable resource monitoring
nlm --resource-monitor

# Run performance optimization
nlm --performance-optimize
```

## Benefits

### Performance Improvements
- Identify and eliminate performance bottlenecks
- Optimize simulation performance
- Reduce resource consumption
- Improve system responsiveness

### Better Understanding
- Understand performance characteristics
- Analyze resource usage patterns
- Identify optimization opportunities
- Track performance over time

### Enhanced Development
- Performance-aware development
- Early performance detection
- Automated performance optimization
- Continuous performance monitoring

### Production Support
- Performance monitoring in production
- Performance alerting
- Performance trend analysis
- Performance regression detection

## Installation and Setup

### Prerequisites
- C++20 compiler
- CMake 3.16 or higher
- Python 3.6+ (for Python bindings)
- pybind11

### Building

```bash
cmake -B build
cmake --build build
```

### Running

```bash
./build/nlm --help
```

## Configuration

### Performance Configuration File
Create a configuration file `performance.cfg` with the following options:

```ini
[performance]
# Profiling settings
profile_enabled = true
profile_interval = 0.1
sampling_interval = 1.0
max_samples = 10000
output_format = json
output_file = performance_profile.json
real_time_monitoring = true

# Resource monitoring
resource_monitoring_enabled = true
cpu_monitoring = true
memory_monitoring = true
disk_monitoring = true
network_monitoring = true
temperature_monitoring = true

# Benchmark settings
benchmark_iterations = 1000
benchmark_timeout = 30.0
warmup_enabled = true
warmup_iterations = 100

# Optimization
auto_optimization = true
optimization_threshold = 0.5
```

### CLI Configuration Overrides
Use `--set` option to override performance configuration:

```bash
nlm --set performance.profile_enabled=false --set performance.output_format=text
```

## File Structure

```
/workspace/app/src/
├── performance_monitor.h          # Core performance monitoring
├── performance_monitor.cpp        # Implementation
├── profiler.h                   # Main profiling framework
├── profiler.cpp                 # Implementation
├── benchmark.h                  # Benchmarking suite
├── benchmark.cpp                # Implementation
├── performance_report.h          # Performance report generator
├── performance_report.cpp        # Implementation
├── resources_monitor.h           # System resource monitoring
├── resources_monitor.cpp         # Implementation
├── performance_monitor_integration.h  # NLM integration
├── performance_monitor_integration.cpp# Integration implementation
├── performance_examples.cpp      # Usage examples
└── performance/
    ├── Performance.hpp          # Existing performance optimization
    └── ... (other performance files)
```

## Testing

### Performance Testing
Run performance tests with:

```bash
cd build
ctest --output-on-failure
```

### Benchmark Testing
Run benchmarks with:

```bash
./build/benchmark
```

## Troubleshooting

### Common Issues

1. **Performance Monitoring Overhead**
   - Solution: Use lower sampling intervals for production
   - Configuration: Set `sampling_interval` in `performance.cfg`

2. **Memory Usage Tracking**
   - Solution: Enable memory profiling with `--profile`
   - Configuration: Set `track_memory = true` in profiler config

3. **CPU Usage Tracking**
   - Solution: Enable CPU profiling with `--profile`
   - Configuration: Set `track_cpu = true` in profiler config

4. **Resource Monitoring**
   - Solution: Enable resource monitoring with `--resource-monitor`
   - Configuration: Set `resource_monitoring_enabled = true` in `performance.cfg`

5. **Report Generation**
   - Solution: Check file permissions and disk space
   - Configuration: Ensure write permissions to output directory

## Future Enhancements

### Planned Features
1. **GPU Monitoring**: Add GPU monitoring capabilities for CUDA/OpenCL systems
2. **Network Monitoring**: Enhanced network monitoring with bandwidth analysis
3. **File System Monitoring**: Detailed file system monitoring
4. **Application-Level Profiling**: Automatic profiling of NLM components
5. **Performance Prediction**: Machine learning-based performance prediction
6. **Auto-tuning**: Automatic configuration optimization
7. **Distributed Profiling**: Profiling across multiple nodes
8. **Integration with External Tools**: Integration with Valgrind, perf, etc.

### Performance Goals
1. **Low Overhead**: Minimize performance impact of monitoring
2. **Real-time Monitoring**: Provide real-time performance feedback
3. **Scalability**: Support for large-scale simulations (100M+ neurons)
4. **Accuracy**: Accurate measurement of performance metrics
5. **Usability**: Easy-to-use API and comprehensive documentation

## Conclusion

The NLM performance monitoring and profiling system provides comprehensive tools for understanding, analyzing, and optimizing simulation performance. It integrates seamlessly with the NLM framework and provides extensive capabilities for performance monitoring, analysis, and optimization. The system is designed to be flexible, extensible, and user-friendly, making it an essential component for performance-aware development and deployment of NLM simulations.
