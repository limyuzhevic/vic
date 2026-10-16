# Performance Monitoring System Configuration

This document provides configuration options for the NLM Performance Monitoring System.

## Overview

The NLM Performance Monitoring System provides comprehensive performance monitoring and profiling capabilities for the NLM framework. It includes:

- Real-time performance metrics collection
- Call stack profiling
- Memory allocation profiling
- Event-driven profiling
- Performance bottleneck detection
- Flame graph generation
- Performance analysis and reporting
- Real-time monitoring dashboards

## Configuration Options

### Global Performance Settings

```
performance.enabled = true
performance.monitoring_level = "Detailed"
performance.max_profile_history = 1000
performance.update_interval_ms = 100
performance.cpu_threshold = 80.0
performance.memory_threshold_mb = 1024
performance.event_threshold = 10000
performance.bottleneck_threshold = 0.9
performance.enable_flame_graphs = true
performance.enable_regression_detection = true
performance.enable_dashboard = true
```

### Performance Monitoring Levels

- **Silent (0)**: No performance monitoring
- **Basic (1)**: Essential metrics only
- **Detailed (2)**: Detailed profiling
- **Comprehensive (3)**: Full profiling with analysis

### Performance Categories

The system monitors the following performance categories:

1. **Neural Computation**: Neuron step time, spike propagation, synaptic updates
2. **Memory Management**: Allocation, deallocation, memory fragmentation
3. **Connectivity**: Synapse creation, plasticity updates, region management
4. **Integration**: Inter-system communication, state synchronization
5. **Development**: Developmental changes, adaptation, plasticity
6. **Overall**: Total simulation performance, efficiency metrics

### Output Formats

The performance monitoring system supports multiple output formats:

- **Text**: Human-readable text format
- **JSON**: Machine-readable JSON format
- **CSV**: Comma-separated values format
- **HTML**: Web-based HTML format with visualizations

### Performance Thresholds

The system monitors performance thresholds and generates alerts:

- **CPU Usage**: Alerts when CPU usage exceeds the configured threshold
- **Memory Usage**: Alerts when memory usage exceeds the configured threshold
- **Event Count**: Alerts when event count exceeds the configured threshold

### Configuration Files

Performance monitoring configuration can be specified in:

1. **Config File**: `/tmp/performance_config.json`
2. **Command Line Arguments**: `--performance-level=Detailed --performance-update-interval=100`
3. **Runtime Configuration**: Through the Config class API

### Usage Examples

#### Basic Usage

```cpp
#include "performance_monitoring/PerformanceMonitor.hpp"

int main() {
    // Initialize performance monitoring
    auto performanceMonitor = &nlm::PerformanceMonitor::getInstance();
    performanceMonitor->initialize();
    
    // Start monitoring
    performanceMonitor->startMonitoring();
    
    // Perform simulation
    // ...
    
    // Stop monitoring
    performanceMonitor->stopMonitoring();
    
    // Generate report
    performanceMonitor->generateDetailedReport("performance_report.txt");
    
    return 0;
}
```

#### Advanced Usage

```cpp
#include "performance_monitoring/PerformanceMonitor.hpp"
#include "performance_monitoring/Profiler.hpp"
#include "performance_monitoring/PerformanceDashboard.hpp"

int main() {
    // Configure performance monitoring
    auto config = std::make_shared<Config>();
    config->set("performance.enabled", true);
    config->set("performance.monitoring_level", "Comprehensive");
    config->set("performance.enable_flame_graphs", true);
    config->set("performance.enable_dashboard", true);
    
    // Initialize performance monitoring
    auto performanceMonitor = std::make_shared<nlm::PerformanceMonitor>(config);
    nlm::PerformanceMonitor::setInstance(performanceMonitor);
    
    auto profiler = std::make_shared<nlm::Profiler>(config);
    nlm::Profiler::setInstance(profiler);
    
    // Configure dashboard
    auto dashboard = std::make_shared<nlm::PerformanceDashboard>(performanceMonitor, &nlm::PerformanceMetrics::getInstance());
    nlm::PerformanceDashboard::setInstance(dashboard);
    
    // Start monitoring
    performanceMonitor->startMonitoring();
    profiler->startCallStackProfiling();
    profiler->startMemoryProfiling();
    
    // Perform simulation
    // ...
    
    // Generate comprehensive reports
    performanceMonitor->generateDetailedReport("detailed_report.html");
    profiler->generateFlameGraph("flame_graph.html");
    
    // Start dashboard
    dashboard->startDashboard();
    
    // Stop all monitoring
    performanceMonitor->stopMonitoring();
    profiler->stopCallStackProfiling();
    profiler->stopMemoryProfiling();
    dashboard->stopDashboard();
    
    return 0;
}
```

### Performance Monitoring API

The performance monitoring system provides the following API:

#### PerformanceMonitor Class

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize the performance monitoring system |
| `startMonitoring()` | Start performance monitoring |
| `stopMonitoring()` | Stop performance monitoring |
| `isMonitoring()` | Check if monitoring is active |
| `updateMetrics()` | Update performance metrics |
| `recordMetric()` | Record a performance metric |
| `recordMemoryUsage()` | Record memory usage |
| `recordCPUTime()` | Record CPU time |
| `recordIOOperation()` | Record I/O operation |
| `startProfile()` | Start profiling |
| `stopProfile()` | Stop profiling |
| `recordAllocation()` | Record memory allocation |
| `recordDeallocation()` | Record memory deallocation |
| `recordEvent()` | Record an event |
| `getProfileHistory()` | Get profile history |
| `getCurrentProfile()` | Get current profile |
| `getAlerts()` | Get performance alerts |
| `acknowledgeAlert()` | Acknowledge an alert |
| `checkThresholds()` | Check performance thresholds |
| `analyzePerformance()` | Analyze performance |
| `detectRegression()` | Detect performance regression |
| `getOptimizationRecommendations()` | Get optimization recommendations |
| `exportPerformanceData()` | Export performance data |
| `importPerformanceData()` | Import performance data |
| `generateFlameGraph()` | Generate flame graph |
| `generateDetailedReport()` | Generate detailed report |

#### Profiler Class

| Method | Description |
|--------|-------------|
| `setMonitoringLevel()` | Set monitoring level |
| `getMonitoringLevel()` | Get monitoring level |
| `startCallStackProfiling()` | Start call stack profiling |
| `stopCallStackProfiling()` | Stop call stack profiling |
| `recordCallStack()` | Record call stack |
| `getCallStackCounts()` | Get call stack counts |
| `generateCallStackReport()` | Generate call stack report |
| `startMemoryProfiling()` | Start memory profiling |
| `stopMemoryProfiling()` | Stop memory profiling |
| `recordAllocation()` | Record memory allocation |
| `recordDeallocation()` | Record memory deallocation |
| `getAllocationCounts()` | Get allocation counts |
| `getAllocationSizes()` | Get allocation sizes |
| `generateMemoryReport()` | Generate memory report |
| `startEventProfiling()` | Start event profiling |
| `stopEventProfiling()` | Stop event profiling |
| `recordEvent()` | Record event |
| `recordTimedEvent()` | Record timed event |
| `getEventTimeline()` | Get event timeline |
| `generateEventReport()` | Generate event report |
| `updateBottleneckData()` | Update bottleneck data |
| `detectBottlenecks()` | Detect bottlenecks |
| `identifyHotPaths()` | Identify hot paths |
| `generateBottleneckReport()` | Generate bottleneck report |
| `generateFlameGraph()` | Generate flame graph |

#### PerformanceMetrics Class

| Method | Description |
|--------|-------------|
| `recordMetric()` | Record a performance metric |
| `getMetric()` | Get a performance metric |
| `getMetrics()` | Get metrics by category |
| `getAllMetrics()` | Get all metrics |
| `startTimer()` | Start a timer |
| `stopTimer()` | Stop a timer |
| `getTimerDuration()` | Get timer duration |
| `getAllTimers()` | Get all timers |
| `recordMemorySnapshot()` | Record memory snapshot |
| `getCurrentMemoryUsage()` | Get current memory usage |
| `getPeakMemoryUsage()` | Get peak memory usage |
| `recordCPUSample()` | Record CPU sample |
| `recordIOSample()` | Record I/O sample |
| `getAverageCPUUsage()` | Get average CPU usage |
| `getIOHistory()` | Get I/O history |
| `exportMetrics()` | Export metrics |
| `importMetrics()` | Import metrics |

#### PerformanceAnalysis Class

| Method | Description |
|--------|-------------|
| `analyzePerformance()` | Analyze performance |
| `detectAnomalies()` | Detect anomalies |
| `detectRegression()` | Detect regression |
| `getOptimizationRecommendations()` | Get optimization recommendations |
| `identifyBottlenecks()` | Identify bottlenecks |
| `identifyHotPaths()` | Identify hot paths |
| `calculatePerformanceScore()` | Calculate performance score |
| `getCategoryScores()` | Get category scores |
| `getOverallPerformanceGrade()` | Get overall performance grade |

#### PerformanceDashboard Class

| Method | Description |
|--------|-------------|
| `update()` | Update dashboard |
| `startDashboard()` | Start dashboard |
| `stopDashboard()` | Stop dashboard |
| `isDashboardRunning()` | Check if dashboard is running |
| `getDashboardDisplay()` | Get dashboard display |
| `display()` | Display dashboard |
| `exportDashboard()` | Export dashboard |
| `addWidget()` | Add widget |
| `removeWidget()` | Remove widget |
| `updateWidget()` | Update widget |
| `getWidget()` | Get widget |
| `getAllWidgets()` | Get all widgets |
| `addLayout()` | Add layout |
| `setActiveLayout()` | Set active layout |

#### PerformanceReporter Class

| Method | Description |
|--------|-------------|
| `generateReport()` | Generate report |
| `generateSummaryReport()` | Generate summary report |
| `generateDetailedReport()` | Generate detailed report |
| `generateComparisonReport()` | Generate comparison report |
| `setReportConfig()` | Set report configuration |
| `getReportConfig()` | Get report configuration |
| `setReportTitle()` | Set report title |
| `setReportDescription()` | Set report description |
| `exportReport()` | Export report |
| `importReport()` | Import report |

### Performance Monitoring Statistics

The performance monitoring system collects the following statistics:

- **Steps**: Total number of simulation steps
- **Real Time**: Total real time elapsed
- **CPU Usage**: Average CPU usage percentage
- **Memory Usage**: Peak memory usage in MB
- **Events**: Total number of events
- **Functions**: Total number of unique functions
- **Performance Score**: Overall performance score (0-100)

### Performance Monitoring Commands

The performance monitoring system provides the following commands:

#### Command Line Options

```bash
--performance-level=<level>  Set performance monitoring level (Silent, Basic, Detailed, Comprehensive)
--performance-update-interval=<ms>  Set performance update interval in milliseconds
--performance-output-file=<file>  Set output file for performance data
--performance-enable-flame-graphs  Enable flame graph generation
--performance-enable-dashboard  Enable performance dashboard
--performance-enable-regression-detection  Enable regression detection
--performance-baseline=<name>  Set baseline name for comparison
```

#### Build Options

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)
```

### Performance Monitoring Files

The performance monitoring system generates the following files:

1. **Performance Data Files**: `/tmp/performance_data.json`
2. **Detailed Reports**: `/tmp/detailed_report.txt`, `/tmp/detailed_report.html`
3. **Flame Graphs**: `/tmp/flame_graph.html`
4. **Memory Reports**: `/tmp/memory_report.txt`
5. **Bottleneck Reports**: `/tmp/bottleneck_report.txt`
6. **Comparison Reports**: `/tmp/comparison_report.txt`
7. **Dashboard Files**: `/tmp/performance_dashboard.html`

### Troubleshooting

#### Common Issues

1. **Performance Monitoring Not Working**
   - Check if `performance.enabled` is set to `true`
   - Verify that `performance.monitoring_level` is not `Silent`
   - Ensure that `performanceMonitor->initialize()` has been called

2. **No Performance Data**
   - Check if `performanceMonitor->startMonitoring()` has been called
   - Verify that performance monitoring is not disabled in the config
   - Check if there are any errors in the performance monitoring initialization

3. **Report Generation Issues**
   - Ensure that the output directory is writable
   - Check if the required permissions are available
   - Verify that the report format is supported

4. **Dashboard Issues**
   - Check if `performanceMonitor->startMonitoring()` has been called
   - Verify that the dashboard configuration is correct
   - Ensure that all required dependencies are available

#### Error Messages

The performance monitoring system may generate the following error messages:

- `"Performance monitoring not initialized"`
- `"Performance monitoring not started"`
- `"Error generating report"`
- `"Error exporting performance data"`

### Performance Monitoring Integration

The performance monitoring system can be integrated with the NLM framework in the following ways:

1. **Core Integration**: Integrate with the core NLM components (Config, Brain, Neuron, Synapse)
2. **Plugin Integration**: Create plugins for specific performance monitoring scenarios
3. **Web Integration**: Integrate with web-based dashboards and visualizations
4. **Mobile Integration**: Integrate with mobile devices for field testing

### Performance Monitoring Examples

The performance monitoring system includes several examples:

1. **Integration Example**: `/performance_monitoring_examples/integration_example.cpp`
2. **Config Integration Example**: `/performance_monitoring_examples/config_integration_example.cpp`
3. **Brain Integration Example**: `/performance_monitoring_examples/brain_integration_example.cpp`

### Performance Monitoring Best Practices

1. **Always initialize the performance monitoring system before using it**
2. **Start monitoring before performing any operations**
3. **Stop monitoring after completing operations**
4. **Generate reports regularly to track performance over time**
5. **Use appropriate monitoring levels for different scenarios**
6. **Monitor performance thresholds and set appropriate alerts**
7. **Use flame graphs for detailed performance analysis**
8. **Compare performance against baselines to detect regressions**
9. **Use dashboards for real-time performance monitoring**
10. **Regularly review optimization recommendations**

### Performance Monitoring Future Enhancements

Future enhancements to the performance monitoring system may include:

1. **Machine Learning**: Use machine learning to predict performance issues
2. **Real-time Visualization**: Real-time visualization of performance metrics
3. **Cloud Integration**: Integration with cloud-based performance monitoring services
4. **Mobile Integration**: Integration with mobile devices for field testing
5. **IoT Integration**: Integration with IoT devices for distributed performance monitoring
6. **Advanced Analytics**: Advanced analytics for performance optimization
7. **Performance Prediction**: Predictive performance modeling
8. **Automated Optimization**: Automated performance optimization

### License

The NLM Performance Monitoring System is licensed under the NLM Framework License.