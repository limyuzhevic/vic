# NLM Performance Monitoring and Debugging Tools

## Overview

This project provides comprehensive performance monitoring and debugging tools for NLM (Neural Learning Machine) neural simulations. The monitoring system enables advanced users to optimize, monitor, and debug neural simulations with real-time metrics, memory leak detection, performance profiling, bottleneck analysis, and neural activity monitoring.

## Project Structure

### Core Monitoring Module
- `src/monitor/PerformanceMonitor.hpp` - Performance monitoring class
- `src/monitor/PerformanceMonitor.cpp` - Performance monitoring implementation
- `src/monitor/MemoryMonitor.hpp` - Memory monitoring class
- `src/monitor/MemoryMonitor.cpp` - Memory monitoring implementation
- `src/monitor/NeuralMonitor.hpp` - Neural activity monitoring class
- `src/monitor/NeuralMonitor.cpp` - Neural activity monitoring implementation
- `src/monitor/NeuromodulationMonitor.hpp` - Neuromodulation monitoring class
- `src/monitor/NeuromodulationMonitor.cpp` - Neuromodulation monitoring implementation

### Monitoring Tools
- `src/monitor/tools/MemoryLeakDetector.hpp` - Memory leak detection
- `src/monitor/tools/MemoryLeakDetector.cpp` - Memory leak detection implementation
- `src/monitor/tools/MemoryPoolManager.hpp` - Memory pool management
- `src/monitor/tools/MemoryPoolManager.cpp` - Memory pool management implementation
- `src/monitor/tools/PerformanceProfiler.hpp` - Performance profiling
- `src/monitor/tools/PerformanceProfiler.cpp` - Performance profiling implementation
- `src/monitor/tools/BottleneckAnalyzer.hpp` - Bottleneck analysis
- `src/monitor/tools/BottleneckAnalyzer.cpp` - Bottleneck analysis implementation

### Python Bindings
- `python/monitor_bindings.cpp` - Python bindings for all monitoring classes

### Examples
- `examples/monitor/real_time_monitor.py` - Real-time monitoring example
- `examples/monitor/memory_leak_detection.py` - Memory leak detection example
- `examples/monitor/performance_profiling.py` - Performance profiling example
- `examples/monitor/bottleneck_analysis.py` - Bottleneck analysis example
- `examples/monitor/neural_activity_monitor.py` - Neural activity monitoring example

### Documentation
- `docs/performance_monitoring.md` - Comprehensive documentation

### Build Configuration
- `src/monitor/CMakeLists.txt` - CMake configuration for monitoring tools
- `CMakeLists.txt` - Main project CMake configuration
- `python/CMakeLists.txt` - Python bindings CMake configuration

## Key Features

### 1. Performance Monitoring
- Real-time metrics collection (neural activity, memory usage, neuromodulation, predictions, cognitive performance)
- Performance benchmarking (steps per second, memory allocation rates, I/O performance, CPU utilization)
- Real-time monitoring interface (dashboard, alerts, historical data, export)
- Alert system with configurable thresholds

### 2. Memory Leak Detection
- Allocation tracking for leak detection
- Memory block tracking for heap analysis
- Circular reference detection
- Memory pool integration
- Detailed leak reporting with stack traces

### 3. Performance Profiling
- Function-level profiling
- System metrics tracking
- Timeline data collection
- Hotspot detection
- Bottleneck analysis
- Comparative analysis across different profiles

### 4. Bottleneck Analysis
- CPU bottleneck detection
- Memory bottleneck detection
- I/O bottleneck detection
- Network bottleneck detection
- Algorithm bottleneck detection
- Time-based trend analysis
- Performance prediction

### 5. Neural Activity Monitoring
- Real-time spike tracking and analysis
- Population-level activity monitoring
- Synaptic activity tracking
- Network topology analysis
- Synchronized group detection
- Burst neuron detection

### 6. Neuromodulation Monitoring
- Real-time neuromodulator signal monitoring (dopamine, curiosity, novelty, etc.)
- Signal timing and effectiveness analysis
- Effect tracking on neural activity
- Signal quality analysis
- Pattern detection

### 7. Python Integration
- Full Python bindings for all monitoring classes
- Seamless integration with existing NLM Python API
- Example scripts for quick start
- Comprehensive documentation

## Building and Installation

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Python 3.8 or higher
- pybind11
- CMake 3.16 or higher

### Building with CMake

#### Standard Build
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

#### Build with Options
```bash
mkdir build
cd build
cmake -DNLM_ENABLE_MONITORING=ON -DNLM_ENABLE_PYTHON_BINDINGS=ON ..
cmake --build . -j$(nproc)
```

#### Build for Development
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_MONITORING=ON -DNLM_ENABLE_PYTHON_BINDINGS=ON ..
cmake --build . -j$(nproc)
```

### Installing

#### System Installation
```bash
cd build
sudo cmake --install ..
```

#### Local Installation
```bash
cd build
cmake --install . --prefix ~/.local
```

### Building Python Bindings

The Python bindings are automatically built as part of the main project. After building, the Python modules will be available in the build directory.

#### Build and Install Python Package
```bash
cd build
pip install -e ../
```

#### Test Python Bindings
```bash
cd build
python3 -c "import sys; sys.path.insert(0, '.'); import pynlm; print('NLM Python bindings loaded successfully')"
```

## Using the Monitoring Tools

### Basic Usage

#### C++ Integration
```cpp
#include <pynlm.h>
#include <monitor.hpp>

int main() {
    // Create monitor
    nlm::monitor::NLMMonitor monitor;
    
    // Configure monitoring
    nlm::monitor::MonitoringConfig config;
    config.enableRealTimeMonitoring = true;
    config.enableMemoryTracking = true;
    config.enableNeuralMonitoring = true;
    config.enableNeuromodulationMonitoring = true;
    config.enableLeakDetection = true;
    config.enablePerformanceProfiling = true;
    config.enableBottleneckAnalysis = true;
    
    monitor.configure(config);
    
    // Start monitoring
    monitor.startAllMonitoring();
    
    // Run simulation
    // ... (your simulation code here)
    
    // Stop monitoring
    monitor.stopAllMonitoring();
    
    // Generate reports
    std::string report = monitor.generateComprehensiveReport();
    std::ofstream("report.txt") << report;
    
    return 0;
}
```

#### Python Usage
```python
import pynlm
from monitor import PerformanceMonitor, MemoryMonitor, NeuralMonitor

# Create monitor
monitor = pynlm.Monitor()

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
# ... (your simulation code here)

# Stop monitoring
monitor.stopAllMonitoring()

# Generate reports
report = monitor.generateComprehensiveReport()
with open("report.txt", "w") as f:
    f.write(report)
```

### Examples

The `examples/monitor/` directory contains comprehensive Python examples:

1. **real_time_monitor.py**: Real-time monitoring of neural simulations
2. **memory_leak_detection.py**: Memory leak detection and analysis
3. **performance_profiling.py**: Performance profiling and analysis
4. **bottleneck_analysis.py**: Bottleneck analysis and optimization recommendations
5. **neural_activity_monitor.py**: Neural activity monitoring and analysis

To run an example:
```bash
python3 examples/monitor/real_time_monitor.py
```

## Features and Capabilities

### 1. Real-time Metrics Collection
- **Neural Activity Metrics**: Spike rates, firing patterns, population activity
- **Memory Usage Metrics**: Working memory utilization, episodic memory load, total memory footprint
- **Neuromodulation Metrics**: Signal levels, timing, effectiveness
- **Prediction Accuracy Metrics**: Prediction error, confidence, accuracy
- **Cognitive Performance Metrics**: Planning success, concept formation rate, decision quality

### 2. Performance Benchmarking
- **Simulation Performance**: Steps per second, simulation time
- **Memory Performance**: Memory allocation/deallocation rates
- **I/O Performance**: File read/write speeds, data transfer rates
- **CPU Performance**: CPU utilization, thread performance
- **Network Performance**: Inter-process communication (if applicable)

### 3. Real-time Monitoring Interface
- **Dashboard**: Real-time visualization of system metrics
- **Alerts**: Threshold-based alerts for performance issues
- **Historical Data**: Performance history and trend analysis
- **Export**: Export metrics for analysis

### 4. Memory Leak Detection
- **Allocation Tracking**: Track memory allocations and deallocations
- **Leak Detection**: Identify memory leaks in Python bindings
- **Circular Reference Detection**: Detect circular references
- **Memory Pool Monitoring**: Monitor memory pool usage
- **Garbage Collection Analysis**: Analyze garbage collection efficiency

### 5. Neural Activity Monitoring
- **Spike Monitoring**: Real-time spike detection and analysis
- **Population Analysis**: Population-level activity analysis
- **Synaptic Tracking**: Connection strength and activity monitoring
- **Network Topology**: Real-time network structure analysis

### 6. Performance Analysis
- **Bottleneck Identification**: Identify performance bottlenecks
- **Hotspot Detection**: Identify hotspots in code execution
- **Resource Profiling**: Profile CPU, memory, I/O resource usage
- **Comparative Analysis**: Compare performance across configurations

### 7. Memory Management Tools
- **Memory Pool Optimization**: Optimize memory pool usage
- **Allocation Strategy**: Analyze and optimize allocation strategies
- **Deallocation Optimization**: Optimize cleanup strategies
- **Memory Fragmentation Analysis**: Analyze memory fragmentation

### 8. Parallel Processing Tools
- **Thread Pool Monitoring**: Monitor thread pool performance
- **Task Scheduling Analysis**: Analyze task scheduling efficiency
- **Load Balancing**: Monitor and optimize load balancing
- **Concurrent Access Control**: Ensure thread-safe access

### 9. Simulation Scaling Tools
- **Large-scale Simulation**: Tools for large-scale neural simulations
- **Distributed Computing**: Tools for distributed simulation (if applicable)
- **Memory-efficient Algorithms**: Tools for memory-efficient implementations
- **Time-efficient Algorithms**: Tools for time-efficient implementations

## Configuration Options

The monitoring system can be configured with the following options:

### CMake Options
- `NLM_ENABLE_MONITORING`: Enable performance monitoring (default: ON)
- `NLM_MONITORING_TYPE`: Monitoring implementation type (default: "comprehensive")
- `NLM_ENABLE_PYTHON_BINDINGS`: Build Python bindings (default: ON)
- `NLM_ENABLE_MONITORS_DOC`: Generate monitoring documentation (default: ON)

### Monitoring Configuration
The `NLM::Monitor` class can be configured with:
- `enableRealTimeMonitoring`: Enable real-time performance monitoring
- `enableMemoryTracking`: Enable memory tracking and analysis
- `enableNeuralMonitoring`: Enable neural activity monitoring
- `enableNeuromodulationMonitoring`: Enable neuromodulation signal monitoring
- `enableLeakDetection`: Enable memory leak detection
- `enablePerformanceProfiling`: Enable performance profiling
- `enableBottleneckAnalysis`: Enable bottleneck analysis
- `retentionTime`: Data retention time in seconds (default: 3600.0)
- `alertThreshold`: Alert threshold (default: 0.8)
- `exportFormat`: Export format (default: "json")

## Performance Considerations

### 1. Minimal Overhead
- Monitoring components are only enabled when explicitly requested
- Minimal performance impact when monitoring is disabled
- Efficient data structures for optimal memory usage

### 2. Configurable Sampling
- Users can configure sampling rates to balance performance and detail
- Support for different sampling strategies
- Configurable retention times for historical data

### 3. Parallel Processing
- Where applicable, monitoring uses parallel processing to minimize impact on simulations
- Thread-safe implementation for multi-threaded environments

### 4. Extensible Design
- New monitoring components can be easily added
- Plugin architecture for custom monitoring tools
- Support for user-defined metrics and alerts

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

## Technical Details

### Thread Safety
- All monitoring components are thread-safe
- No global state is shared between components
- Monitoring operations are atomic where necessary

### Memory Management
- Memory usage is carefully managed to minimize overhead
- Memory pools are used for efficient allocation/deallocation
- Memory leaks are actively detected and reported

### Performance Monitoring
- Real-time metrics are collected with minimal overhead
- Sampling rates can be configured based on performance requirements
- Metrics are stored in memory for efficient access

### Data Export
- Multiple export formats are supported (JSON, CSV, HTML)
- Export can be configured based on user needs
- Exported data includes all relevant metrics and statistics

## Acknowledgements

This monitoring system is built on top of the following NLM components:
- NLM Core Library
- NLM Brain Module
- NLM Memory System
- NLM Neuromodulation System
- NLM Cognition System
- NLM Prediction System
- NLM Development System
- NLM Visualization System
- NLM Performance Optimizations

The monitoring system follows the same design principles and coding standards as the core NLM libraries.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more information.

## Copyright

Copyright (c) 2024 NLM Team. All rights reserved.

## Version

Version: 0.1.0

For more information, documentation, and support, please visit the NLM project repository or contact the NLM development team.