# NLM Performance Monitoring Infrastructure

## Overview

The NLM Performance Monitoring Infrastructure provides comprehensive real-time monitoring capabilities for neural network simulations. This system enables developers to track, analyze, and optimize simulation performance across multiple dimensions.

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                   Performance Monitoring System                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐ │
│  │ Performance     │  │ Performance     │  │ Performance     │ │
│  │ Monitor (Main)  │  │ Profiler       │  │ Reporter        │ │
│  │ Singleton       │  │ High-resolution │  │ Multi-format    │ │
│  │                 │  │ timing & stack │  │ output         │ │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘ │
│                                                                 │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐ │
│  │  Metric        │  │  Metric         │  │  Export/        │ │
│  │  Collector     │  │  Collector      │  │  Visualization  │ │
│  │  Time-series   │  │  Analytics      │  │  Dashboard      │ │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘ │
│                                                                 │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐ │
│  │   Component     │  │   Component     │  │   Performance   │ │
│  │   Adapters     │  │   Adapters      │  │   Analysis     │ │
│  │   (Brain,      │  │   (Memory,      │  │   Utilities     │ │
│  │   Memory, Env) │  │   Environment)   │  │   (Stats, Timers│ │
│  └─────────────────┘  └─────────────────┘  │   Counters)     │ │
│                                             └─────────────────┘ │
│                                                                 │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐ │
│  │  Real-time      │  │  Live Dashboard │  │  Alerting       │ │
│  │  API Access     │  │  Visualization  │  │  Thresholds     │ │
│  │                 │  │                 │  │                 │ │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘ │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## Core Components

### 1. Performance Monitor (Main Interface)

**Location**: `src/core/Monitoring.hpp` / `Monitoring.cpp`

The PerformanceMonitor is a singleton class that provides the main interface for performance monitoring:

```cpp
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance();
    
    // Metric recording
    void recordMetric(const std::string& name, double value, 
                     MetricType type = MetricType::Gauge);
    void recordTiming(const std::string& name, std::chrono::duration<double> duration);
    
    // Component registration
    void registerComponent(const std::string& name, 
                          ComponentType type = ComponentType::Core);
    void unregisterComponent(const std::string& name);
    
    // Performance profiling
    ProfilingScope profile(const std::string& operation);
    
    // Real-time access
    std::vector<MetricSample> getMetrics(const std::string& name, 
                                       std::chrono::duration<double> range);
    double getCurrentMetric(const std::string& name);
    
    // Reporting
    std::string generateReport(ReportFormat format = ReportFormat::Text);
    void exportMetrics(const std::string& filepath);
};
```

#### Features:
- **Singleton pattern**: Single instance throughout the application
- **Thread-safe**: Mutex protection for concurrent access
- **Component-based**: Track metrics per component
- **Time-series storage**: Historical metric collection
- **Multi-format reporting**: Text, JSON, CSV, HTML outputs

### 2. Performance Profiler

**Location**: `src/core/PerformanceProfiler.hpp` / `PerformanceProfiler.cpp`

High-resolution performance profiling with call stack tracking:

```cpp
class PerformanceProfiler {
public:
    template<typename Func>
    auto profile(const std::string& operation, Func&& func);
    
    ManualProfiler createManualProfile(const std::string& operation);
    
    // Configuration
    void setCaptureStackTrace(bool enable);
    void setTrackMemory(bool enable);
    void setThreadAware(bool enable);
    
    // Data access
    std::vector<ProfileData> getProfileData(const std::string& functionName = "");
    std::string generateProfileReport();
};
```

#### Features:
- **High-resolution timers**: Nanosecond precision
- **Call stack tracking**: Stack traces for function profiling
- **Thread-aware profiling**: Per-thread metrics
- **Memory tracking**: Memory allocation/deallocation tracking
- **Template-based API**: Easy to use with lambdas

### 3. Metric Collector

**Location**: `src/core/MetricCollector.hpp` / `MetricCollector.cpp`

Centralized metric collection with time-series storage and analytics:

```cpp
class MetricCollector {
public:
    void addMetric(const std::string& name, double value,
                  const std::string& component = "core",
                  const std::unordered_map<std::string, std::string>& tags = {});
    
    std::vector<TimeSeriesPoint> getTimeSeries(const std::string& name,
                                             std::chrono::duration<double> startTime,
                                             std::chrono::duration<double> endTime);
    
    MetricStats calculateStats(const std::string& name,
                               std::chrono::duration<double> range);
    
    std::vector<AnomalyInfo> detectAnomalies(const std::string& name,
                                            double threshold = 2.0,
                                            AnomalyDetectionMethod method = StandardDeviation);
};
```

#### Features:
- **Time-series storage**: Efficient storage for large datasets
- **Rolling window**: Automatic data expiration
- **Statistical analysis**: Mean, median, standard deviation
- **Anomaly detection**: Statistical outlier detection
- **Memory-efficient**: Optimized for large-scale simulations

### 4. Performance Reporter

**Location**: `src/core/PerformanceReporter.hpp` / `PerformanceReporter.cpp`

Multi-format report generation and export capabilities:

```cpp
class PerformanceReporter {
public:
    std::string generateDetailedReport(const std::string& metricName = "",
                                       std::chrono::duration<double> range = std::chrono::hours(24));
    
    std::string generateJSONReport(const std::string& metricName = "",
                                  std::chrono::duration<double> range = std::chrono::hours(24));
    
    std::string generateCSVReport(const std::string& metricName = "",
                                  std::chrono::duration<double> range = std::chrono::hours(24));
    
    std::string generateHTMLReport(const std::string& metricName = "",
                                  std::chrono::duration<double> range = std::chrono::hours(24));
    
    void exportMetrics(const std::string& filepath, ReportFormat format = ReportFormat::Text,
                      const std::string& metricName = "");
    
    void exportAllMetrics(const std::string& directory);
};
```

#### Features:
- **Multiple formats**: Text, JSON, CSV, HTML
- **Detailed analysis**: Statistical summaries and visualizations
- **Export capabilities**: Save reports to files
- **Configuration options**: Control report content and format

## Metric Categories

### 1. System-Level Metrics
- CPU utilization and thread activity
- Memory usage (heap, stack, pools)
- I/O operations and file access patterns
- Network activity (for distributed components)

### 2. Simulation-Level Metrics
- Neuron firing rates and patterns
- Synapse activity statistics
- Memory system utilization (working, episodic)
- Neuromodulation levels
- Plasticity update frequency

### 3. Application-Level Metrics
- Experiment progress tracking
- Resource allocation efficiency
- Error rates and exception statistics
- Throughput and latency measurements

### 4. Specialized Metrics
- SIMD utilization
- Cache hit rates
- Memory bandwidth
- Parallel processing efficiency
- Event processing performance

## Component Integration Adapters

### 1. Brain Performance Adapter
**Location**: `src/core/integrations/BrainPerformanceAdapter.hpp`

Collects performance metrics from the Brain class:
- Neuron firing rates and activity
- Synapse connection statistics
- Plasticity update metrics
- Development progress tracking

### 2. Memory Performance Adapter
**Location**: `src/core/integrations/MemoryPerformanceAdapter.hpp`

Collects performance metrics from Memory systems:
- Working memory utilization
- Episodic memory growth
- Capacity and efficiency metrics
- Access pattern analysis

### 3. Environment Performance Adapter
**Location**: `src/core/integrations/EnvironmentPerformanceAdapter.hpp`

Collects performance metrics from Environment:
- Simulation step execution time
- Environment interaction rates
- State change metrics
- Resource utilization

## Utility Components

### 1. Timer Utilities
**Location**: `src/core/utils/Timer.hpp`

High-resolution timing utilities:
- `HighResTimer`: High-resolution timer with pause/resume support
- `ManualTimer`: Convenience class for timing function execution

### 2. Performance Counter
**Location**: `src/core/utils/PerformanceCounter.hpp`

Atomic performance counters:
- Thread-safe counter operations
- Component-based counter categorization
- Batch operations and statistics
- JSON report generation

### 3. Statistical Calculator
**Location**: `src/core/utils/StatsCalculator.hpp`

Statistical analysis utilities:
- Basic statistics (mean, median, stddev)
- Percentiles (P25, P50, P75, P90, P95, P99)
- Distribution analysis (skewness, kurtosis)
- Anomaly detection
- Optimization suggestions

## Integration Points

### Core Components
- **Brain Class**: Neuron firing rates, synapse activity, plasticity metrics
- **Memory Systems**: Working memory utilization, episodic memory growth
- **Development System**: Learning progress, maturation stages
- **Neuromodulation**: Signal levels, error signals

### Simulation Components
- **Environment**: Interaction rates, reward patterns
- **Agent**: Decision speed, action selection efficiency
- **World**: State changes, event processing

### Performance Infrastructure
- **Memory Pools**: Allocation efficiency, fragmentation
- **Event Queue**: Processing latency, queue depth
- **Multithreading**: Thread utilization, synchronization overhead
- **SIMD**: Instruction throughput, vector utilization

## Real-Time Monitoring Features

### Live Dashboard
- Real-time metric display with auto-refresh
- Historical trend visualization
- Component-based metric filtering
- Alerting system with thresholds

### Automated Analysis
- Performance anomaly detection
- Bottleneck identification
- Scalability analysis
- Regression testing capabilities

### Export and Analysis
- Export to common formats (JSON, CSV, PNG)
- Integration with visualization tools
- Comparison analysis across runs
- Statistical analysis capabilities

## Usage Examples

### Basic Usage
```cpp
#include "PerformanceMonitor.hpp"

// Get singleton instance
auto& monitor = PerformanceMonitor::getInstance();

// Record a metric
monitor.recordMetric("neuron_firing_rate", 42.5, MetricType::Gauge, "brain");

// Profile a function
auto scope = monitor.profile("neural_update");
// ... do work ...
scope.complete();

// Get current metric value
double currentRate = monitor.getCurrentMetric("neuron_firing_rate");
```

### Component Integration
```cpp
#include "BrainPerformanceAdapter.hpp"

// Create adapter for Brain class
Brain brain;
BrainPerformanceAdapter brainAdapter(&brain);

// Update metrics periodically
void updatePerformanceMetrics() {
    brainAdapter.updateMetrics();
}
```

### Custom Metric Collection
```cpp
#include "MetricCollector.hpp"

auto& monitor = PerformanceMonitor::getInstance();
auto& collector = MetricCollector::getInstance();

// Collect time-series data
collector.addMetric("cpu_usage", 45.2, "system");
collector.addMetric("memory_usage", 1024.0, "system");

// Get statistics for last hour
MetricStats stats = collector.calculateStats("cpu_usage", std::chrono::hours(1));
```

### Report Generation
```cpp
#include "PerformanceReporter.hpp"

PerformanceReporter reporter;

// Generate detailed report
std::string report = reporter.generateDetailedReport("neuron_firing_rate");

// Export to JSON
reporter.exportMetrics("./reports/neural_performance.json", ReportFormat::JSON, "neuron_firing_rate");
```

## Configuration

### Performance Monitoring Configuration

The performance monitoring system can be configured through the NLM configuration file:

```ini
[performance]
enabled = true
log_level = "info"

[performance.profiler]
capture_stack_trace = true
track_memory = true
thread_aware = true

[performance.collector]
max_data_points = 1000000
retention_hours = 24
rolling_window = true

[performance.reporter]
export_directory = "./reports"
include_timestamps = true
include_components = true
export_raw_data = true
export_statistics = true
export_anomalies = true
```

### Enabling Performance Monitoring

1. **Build with performance flags**:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)
```

2. **Enable in code**:
```cpp
PerformanceMonitor::initialize();
```

3. **Register components**:
```cpp
monitor.registerComponent("brain", ComponentType::Brain);
monitor.registerComponent("memory", ComponentType::Memory);
monitor.registerComponent("environment", ComponentType::Environment);
```

## Performance Considerations

### Non-Intrusive Design

The performance monitoring system is designed to be non-intrusive:
- **Minimal overhead**: Timing measurements use high-resolution clocks
- **Configurable granularity**: Enable/disable features as needed
- **Adaptive sampling**: Adjust sampling frequency based on system load

### Memory Efficiency

For large-scale simulations:
- **Rolling window**: Automatic data expiration to prevent memory bloat
- **Compression**: Optional metric compression for long-term storage
- **Sampling**: Sub-sample long-running metrics for historical analysis

### Scalability

The monitoring system scales with simulation size:
- **Thread-safe**: Multiple threads can record metrics concurrently
- **Hierarchical storage**: Metrics organized by component and time
- **Asynchronous processing**: Background processing for heavy operations

## Testing and Validation

### Test Coverage

1. **Unit Tests**: Each component tested independently
2. **Integration Tests**: Component interactions verified
3. **Performance Tests**: Overhead and accuracy measured
4. **Load Tests**: System behavior under heavy metric collection

### Validation Metrics

- **Timing accuracy**: Measurement precision verified
- **Memory usage**: No memory leaks or excessive consumption
- **Thread safety**: Race conditions prevented
- **Export formats**: Generated files validated

## Future Enhancements

### Phase 2: Advanced Features

1. **Live Dashboard**: Web-based real-time visualization
2. **Alerting System**: Configurable performance thresholds
3. **Machine Learning**: Predictive performance analysis
4. **Distributed Monitoring**: Multi-machine coordination

### Phase 3: Extended Capabilities

1. **GPU Monitoring**: CUDA performance metrics
2. **Network Monitoring**: Distributed system metrics
3. **Hardware Counters**: CPU cycle and cache statistics
4. **Automatic Instrumentation**: Code-based metric discovery

## Performance Monitoring Files

### Generated Files

- `./reports/` - Directory for exported reports
- `./logs/performance.log` - Performance monitoring logs
- `./metrics/current.json` - Current metric snapshot

### Configuration Files

- `./config/performance.ini` - Performance monitoring configuration
- `./performance.json` - Generated performance profile

## Troubleshooting

### Common Issues

**Issue**: No metrics being recorded
**Solution**: Check if PerformanceMonitor::initialize() was called

**Issue**: High memory usage
**Solution**: Verify rolling window is enabled and retention period is appropriate

**Issue**: Thread synchronization problems
**Solution**: Ensure all metric access is properly locked

**Issue**: Export failures
**Solution**: Check file permissions and disk space

### Debugging

Enable debug logging for detailed troubleshooting:
```cpp
monitor.setDebugMode(true);
```

## Conclusion

The NLM Performance Monitoring Infrastructure provides comprehensive visibility into neural network simulation performance. It enables developers to:

- Monitor system resources in real-time
- Analyze simulation performance across multiple dimensions
- Identify performance bottlenecks and anomalies
- Generate detailed reports and exports
- Optimize simulation performance based on data

This infrastructure is essential for scaling NLM to millions of neurons and maintaining high performance across complex neural network simulations.
