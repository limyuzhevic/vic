# Performance Monitoring and Statistics

This document describes the performance monitoring and statistics system in the NLM framework, including how to access and use performance data.

## Overview

The NLM framework includes comprehensive performance monitoring and statistics collection to help users understand system performance, identify bottlenecks, and optimize simulations.

## Key Components

### 1. PerformanceProfiler

The `PerformanceProfiler` class allows users to measure the execution time of specific operations or code sections.

**Usage:**
```cpp
#include "performance/Profiler.hpp"

// Method 1: Object-based profiling
PerformanceProfiler profiler("neural_step");
profiler.start();
// ... perform neural computation ...
profiler.stop();

// Method 2: One-shot profiling
PerformanceProfiler::profileSection("simulation_loop", []() {
    // ... simulation code ...
});

// Method 3: Time measurement
double elapsed = PerformanceProfiler::measureTime([]() {
    // ... code to measure ...
});
```

**Output:** Automatic logging to the global logger with duration in milliseconds.

### 2. MemoryTracker

The `MemoryTracker` class provides detailed memory usage statistics and leak detection.

**Usage:**
```cpp
#include "performance/MemoryTracker.hpp"

MemoryTracker& tracker = MemoryTracker::getGlobal();

// Track memory allocations
void* ptr = malloc(size);
tracker.trackAllocation(size, "file.cpp", 123, "function_name");

// Check for memory leaks
if (tracker.hasLeaks()) {
    auto leaks = tracker.getLeakInfo();
    // ... report leaks ...
}

// Get statistics
std::cout << "Total allocated: " << tracker.getTotalAllocated() << " bytes" << std::endl;
std::cout << "Current usage: " << tracker.getCurrentAllocated() << " bytes" << std::endl;
```

**Features:**
- Automatic tracking of memory allocations
- Leak detection and reporting
- Per-allocation source tracking (file, line, function)
- Cumulative statistics

### 3. TimelineProfiler

The `TimelineProfiler` class records event sequences for timeline analysis and debugging.

**Usage:**
```cpp
#include "performance/TimelineProfiler.hpp"

TimelineProfiler& timeline = TimelineProfiler::getGlobal();

// Record timeline events
timeline.startEvent("brain_initialization");
// ... brain initialization ...
timeline.endEvent("brain_initialization");

timeline.startEvent("simulation_phase_1");
// ... first phase ...
timeline.endEvent("simulation_phase_1", "completed successfully");

// Export timeline
timeline.saveToFile("simulation_timeline.json");
```

**Features:**
- Event timestamp recording
- Context information
- Thread-aware tracking
- Export to JSON and file formats

### 4. Statistics Collection

The framework includes automatic statistics collection for:

#### Neural Network Statistics
- **Neuron Counts**: Total, active, firing neurons
- **Synapse Statistics**: Total synapses, excitatory/inhibitory ratio
- **Activity Metrics**: Firing rates, spike counts
- **Memory Usage**: Working memory traces, episodic memory episodes

#### Simulation Statistics
- **Performance**: Steps per second, real-time factor
- **Accuracy**: Prediction errors, reward accumulation
- **Resource Usage**: Memory consumption, CPU time

## Accessing Performance Data

### Through the Advanced CLI

The Advanced CLI provides commands to access performance data:

```bash
# Show memory statistics
nlm> memory

# Show timeline events  
nlm> timeline

# Run performance benchmark
nlm> benchmark neural_computation

# Show system status
nlm> debug
```

### Through C++ API

```cpp
#include "performance/PerformanceMonitor.hpp"

PerformanceMonitor& monitor = PerformanceMonitor::getGlobal();

// Get current performance statistics
auto stats = monitor.getCurrentStats();

// Get performance history
auto history = monitor.getPerformanceHistory(timePoint);

// Reset statistics
monitor.reset();
```

### Real-time Monitoring

The framework supports real-time monitoring through log files and console output:

```bash
# Enable real-time logging
nlm> debug start

# Monitor memory usage in real-time
# Check log files: nlm_memory.log
```

## Performance Analysis

### 1. Neural Performance

Measure neural computation efficiency:

```cpp
PerformanceProfiler neuralProfiler("neural_step");
neuralProfiler.start();
brain->step(step, time);
neuralProfiler.stop();

std::cout << "Neural step time: " << neuralProfiler.getDuration() * 1000 << " ms" << std::endl;
```

### 2. Memory Efficiency

Track memory usage patterns:

```cpp
MemoryTracker& tracker = MemoryTracker::getGlobal();

// Simulate memory allocation
std::vector<float> data(1000000, 0.0f);
tracker.trackAllocation(data.size() * sizeof(float), __FILE__, __LINE__, __func__);

// Check for memory leaks
if (tracker.hasLeaks()) {
    std::cout << "Memory leak detected!" << std::endl;
    auto leaks = tracker.getLeakInfo();
    for (const auto& leak : leaks) {
        std::cout << "  " << leak << std::endl;
    }
}
```

### 3. Simulation Performance

Track overall simulation efficiency:

```cpp
PerformanceProfiler simulationProfiler("full_simulation");
simulationProfiler.start();

// Run complete simulation
Phase6Config config;
config.maxSteps = 10000;
auto result = experiment.run(config);

simulationProfiler.stop();

std::cout << "Simulation performance:" << std::endl;
std::cout << "  Duration: " << simulationProfiler.getDuration() * 1000 << " ms" << std::endl;
std::cout << "  Steps per second: " << config.maxSteps / simulationProfiler.getDuration() << std::endl;
```

## Advanced Features

### Performance Alerts

Configure automatic alerts for performance thresholds:

```cpp
#include "performance/PerformanceMonitor.hpp"

PerformanceMonitor::getGlobal().setThreshold(
    PerformanceMonitor::MetricType::MemoryUsage,
    1024 * 1024 * 1024, // 1 GB
    [](double value) {
        // Alert callback for high memory usage
        std::cout << "WARNING: Memory usage exceeded threshold!" << std::endl;
    }
);
```

### Custom Metrics

Add custom performance metrics:

```cpp
struct CustomMetrics {
    double neuralEfficiency;
    double memoryEfficiency;
    double simulationSpeed;
};

class CustomMetricCollector {
public:
    void collect(const CustomMetrics& metrics) {
        PerformanceMonitor::getGlobal().recordMetric(
            "custom_metrics",
            std::chrono::system_clock::now(),
            {metrics.neuralEfficiency, metrics.memoryEfficiency, metrics.simulationSpeed}
        );
    }
};
```

## Integration with Other Tools

### Logging Integration

Performance data is automatically integrated with the logging system:

```cpp
// Performance data is logged to the global logger
PerformanceProfiler profiler("operation");
profiler.start();
// ... operation ...
profiler.stop();  // Logs automatically
```

### Checkpoint Integration

Performance statistics are included in checkpoint metadata:

```cpp
CheckpointManager manager;
manager.setPerformanceCallback([](const PerformanceStats& stats) {
    // Store performance data with checkpoint
    // Can be used for analysis and optimization
});
```

## Configuration

### Environment Variables

Set performance monitoring configuration through environment variables:

```bash
export NLM_PERF_MONITORING=1        # Enable performance monitoring
export NLM_PERF_LOG_LEVEL=DEBUG     # Set log level (DEBUG, INFO, WARNING, ERROR)
export NLM_PERF_SAMPLE_INTERVAL=1.0 # Sampling interval in seconds
```

### Configuration File

Configuration can be set in the NLM configuration file:

```ini
[performance]
monitoring_enabled = true
log_level = debug
sample_interval = 1.0

[profiler]
enable_memory_tracking = true
enable_timeline_profiling = true
```

## Best Practices

### 1. Profile Early and Often

Profile your application from the beginning:

```cpp
// In your main function
PerformanceProfiler::profileSection("initialization", []() {
    // ... initialization code ...
});

// During development
PerformanceProfiler profiler("loop_analysis");
for (int i = 0; i < iterations; ++i) {
    profiler.start();
    // ... computation ...
    profiler.stop();
}
```

### 2. Use Appropriate Time Units

Use appropriate time units for different measurements:

```cpp
// For fast operations (milliseconds)
PerformanceProfiler::measureTime([]() {
    // ... fast operation ...
});

// For slow operations (seconds)
PerformanceProfiler profiler("simulation");
profiler.start();
// ... simulation ...
profiler.stop();
```

### 3. Profile Real Workloads

Profile realistic workloads, not synthetic benchmarks:

```cpp
PerformanceProfiler profiler("real_simulation");
profiler.start();
brain->step(step, time);
brain->applyAction(action);
timeline.startEvent("sensor_processing");
// ... process sensors ...
timeline.endEvent("sensor_processing");
profiler.stop();
```

### 4. Set Realistic Expectations

Understand performance characteristics:

```cpp
// Neural network has inherent variability
PerformanceProfiler profiler("neural_computation");
profiler.start();
for (int i = 0; i < num_neurons; ++i) {
    // Each neuron has variable firing patterns
    // This contributes to variability in execution time
    neuron->updateState();
}
profiler.stop();

// Multiple runs for statistical significance
std::vector<double> times;
for (int run = 0; run < num_runs; ++run) {
    PerformanceProfiler run_profiler("run");
    run_profiler.start();
    // ... run simulation ...
    run_profiler.stop();
    times.push_back(run_profiler.getDuration());
}

// Calculate statistics
double mean_time = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
double variance = 0.0;
for (double t : times) {
    variance += (t - mean_time) * (t - mean_time);
}
variance /= times.size();
```

## Conclusion

The NLM performance monitoring and statistics system provides comprehensive tools for:

1. **Performance Measurement**: Accurate timing of operations
2. **Memory Analysis**: Leak detection and usage tracking
3. **Timeline Analysis**: Event sequence recording
4. **Real-time Monitoring**: Live performance monitoring
5. **Statistical Analysis**: Data collection and analysis
6. **Integration**: Seamless integration with other NLM features

Use these tools to optimize your neural network simulations, identify performance bottlenecks, and ensure efficient resource usage.

## References

- [PerformanceProfiler Documentation](performance/Profiler.hpp)
- [MemoryTracker Documentation](performance/MemoryTracker.hpp)
- [TimelineProfiler Documentation](performance/TimelineProfiler.hpp)
- [Performance Monitor API](performance/PerformanceMonitor.hpp)

## See Also

- [Advanced Command Line Interface](core/AdvancedCLI.cpp) - Access performance data through CLI
- [Enhanced Checkpoint System](performance/CheckpointSystem.hpp) - Performance statistics in checkpoints
- [Debug and Profiling Tools](core/Logger/Logger.hpp) - Advanced debugging capabilities
