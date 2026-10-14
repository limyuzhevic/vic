# NLM Performance Monitoring and Metrics Collection

## Overview

This document describes the performance monitoring and metrics collection system for the NLM (Neural Learning Machine) project.

## Overview

The performance monitoring system provides:

- **Real-time monitoring**: Track system performance during execution
- **Metrics collection**: Collect and analyze various performance metrics
- **Performance profiling**: Identify bottlenecks and optimize performance
- **Resource monitoring**: Track CPU, memory, and I/O usage
- **System health monitoring**: Monitor system health and detect issues
- **Alerting**: Generate alerts for performance thresholds

## Metrics Categories

### 1. Computational Metrics
- **Simulation speed**: Steps per second, simulation duration
- **Neuron activity**: Firing rates, spike counts, membrane potential statistics
- **Synaptic activity**: Connection formation, weight changes, plasticity rates
- **Memory usage**: Working memory utilization, episodic memory growth

### 2. Resource Metrics
- **CPU usage**: Processor utilization, multi-core efficiency
- **Memory usage**: RAM consumption, memory allocation patterns
- **I/O operations**: File read/write performance, network I/O
- **Storage usage**: Checkpoint file sizes, disk space consumption

### 3. System Metrics
- **Network latency**: Inter-region communication delays
- **Data throughput**: Information processing rates
- **Error rates**: Failure rates, recovery times
- **Availability**: System uptime, service continuity

### 4. User Experience Metrics
- **Response time**: User interaction response times
- **Throughput**: Number of concurrent operations
- **Satisfaction**: User experience ratings
- **Usability**: Interface efficiency and ease of use

## Metrics Collection Infrastructure

### Core Components

1. **Metrics Collector**: Collects metrics from various sources
2. **Metrics Storage**: Stores metrics data for analysis
3. **Metrics Processor**: Processes and analyzes metrics data
4. **Metrics Visualizer**: Generates visual representations of metrics
5. **Alert System**: Generates alerts based on metric thresholds

### Metrics Data Models

#### Base Metric Class

```cpp
class Metric {
public:
    virtual ~Metric() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getUnit() const = 0;
    virtual double getValue() const = 0;
    virtual std::string getTimestamp() const = 0;
    virtual MetricType getType() const = 0;
    
    virtual std::string toString() const = 0;
    virtual nlohmann::json toJSON() const = 0;
};
```

#### Metric Types

- **Counter**: Cumulative metric (e.g., total spikes)
- **Gauge**: Current value metric (e.g., active neurons)
- **Rate**: Rate metric (e.g., spikes per second)
- **Histogram**: Distribution metric (e.g., spike timing)
- **Summary**: Summarized metric (e.g., average firing rate)

### Implementation

#### Metrics Collector

```cpp
class MetricsCollector {
public:
    void registerMetric(const std::string& name, std::shared_ptr<Metric> metric);
    void unregisterMetric(const std::string& name);
    void startCollecting();
    void stopCollecting();
    std::vector<std::shared_ptr<Metric>> getMetrics() const;
    std::shared_ptr<Metric> getMetric(const std::string& name) const;
    
    void setCollectionInterval(double interval); // seconds
    void setRetentionPeriod(double period); // days
    
    virtual bool collect();
    virtual bool saveToFile(const std::string& filepath);
    virtual bool saveToDatabase(const std::string& connectionString);
};
```

#### Metrics Storage

```cpp
class MetricsStorage {
public:
    virtual ~MetricsStorage() = default;
    
    virtual bool store(const std::vector<std::shared_ptr<Metric>>& metrics) = 0;
    virtual std::vector<std::shared_ptr<Metric>> load(const std::string& query) = 0;
    virtual bool clear() = 0;
    virtual size_t size() const = 0;
    
    virtual std::string getStorageType() const = 0;
    virtual std::string getStorageInfo() const = 0;
};
```

#### Metrics Processor

```cpp
class MetricsProcessor {
public:
    virtual ~MetricsProcessor() = default;
    
    virtual double calculateMean(const std::vector<double>& values) const = 0;
    virtual double calculateStandardDeviation(const std::vector<double>& values) const = 0;
    virtual double calculateMedian(const std::vector<double>& values) const = 0;
    virtual std::vector<double> calculatePercentiles(const std::vector<double>& values, 
                                                      const std::vector<double>& percentiles) const = 0;
    virtual std::pair<double, double> calculateMinMax(const std::vector<double>& values) const = 0;
    
    virtual std::string generateReport(const std::vector<std::shared_ptr<Metric>>& metrics,
                                       const std::string& format = "text") const = 0;
    virtual std::string generateTrendAnalysis(const std::vector<std::shared_ptr<Metric>>& metrics) const = 0;
};
```

### Specialized Metrics

#### Brain Performance Metrics

```cpp
class BrainPerformanceMetrics {
public:
    static std::shared_ptr<Metric> createTotalSpikeCount(const std::shared_ptr<Brain>& brain);
    static std::shared_ptr<Metric> createFiringRate(const std::shared_ptr<Brain>& brain);
    static std::shared_ptr<Metric> createSynapseChanges(const std::shared_ptr<Brain>& brain);
    static std::shared_ptr<Metric> createMemoryUsage(const std::shared_ptr<Brain>& brain);
    static std::shared_ptr<Metric> createProcessingTime(const std::shared_ptr<Brain>& brain);
};
```

#### System Resource Metrics

```cpp
class SystemResourceMetrics {
public:
    static std::shared_ptr<Metric> createCPUUsage();
    static std::shared_ptr<Metric> createMemoryUsage();
    static std::shared_ptr<Metric> createDiskIO();
    static std::shared_ptr<Metric> createNetworkIO();
};
```

#### Configuration Metrics

```cpp
class ConfigurationMetrics {
public:
    static std::shared_ptr<Metric> createConfigLoadTime(const std::shared_ptr<Config>& config);
    static std::shared_ptr<Metric> createConfigValidationTime(const std::shared_ptr<Config>& config);
    static std::shared_ptr<Metric> createConfigMemoryUsage(const std::shared_ptr<Config>& config);
};
```

## Monitoring Integration

### Integration with Brain System

```cpp
class BrainMonitor {
public:
    void startMonitoring(std::shared_ptr<Brain> brain);
    void stopMonitoring();
    void addMetricCollector(std::shared_ptr<MetricsCollector> collector);
    
    void recordBrainMetrics();
    void recordSystemMetrics();
    void recordConfigurationMetrics();
    
    std::vector<std::shared_ptr<Metric>> getCurrentMetrics() const;
    std::string generateReport() const;
};
```

### Integration with CLI

```cpp
class CLIMonitor {
public:
    void setupMonitoring(const std::vector<std::string>& args);
    void startMonitoring();
    void stopMonitoring();
    
    void handleCommand(const std::string& command, const std::vector<std::string>& args);
    void showMonitoringStatus();
    void showPerformanceReport();
};
```

### Integration with Test Framework

```cpp
class TestMetricsCollector {
public:
    void startCollecting(const std::vector<TestCase>& tests);
    void stopCollecting();
    
    void recordTestStart(const TestCase& test);
    void recordTestEnd(const TestCase& test, TestResult result);
    void recordTestMetrics(const TestCase& test, const std::vector<std::shared_ptr<Metric>>& metrics);
    
    std::vector<std::shared_ptr<Metric>> getTestMetrics() const;
    std::string generateTestReport() const;
};
```

## Visualization and Reporting

### Report Types

1. **Text Reports**: Plain text summaries
2. **HTML Reports**: Web-based reports with charts
3. **JSON Reports**: Structured data for integration
4. **XML Reports**: Structured data for enterprise systems
5. **CSV Reports**: Spreadsheet-compatible data

### Visualization Tools

```cpp
class MetricsVisualizer {
public:
    virtual ~MetricsVisualizer() = default;
    
    virtual void createTimeSeriesChart(const std::string& title,
                                       const std::vector<std::shared_ptr<Metric>>& metrics,
                                       const std::string& outputFile) = 0;
    
    virtual void createHistogram(const std::string& title,
                                 const std::vector<double>& values,
                                 const std::string& outputFile) = 0;
    
    virtual void createComparisonChart(const std::string& title,
                                       const std::vector<std::vector<std::shared_ptr<Metric>>>& datasets,
                                       const std::vector<std::string>& labels,
                                       const std::string& outputFile) = 0;
    
    virtual void createDashboard(const std::string& title,
                                 const std::vector<std::shared_ptr<Metric>>& metrics,
                                 const std::string& outputFile) = 0;
};
```

### Export Formats

```cpp
class ReportExporter {
public:
    virtual ~ReportExporter() = default;
    
    virtual bool exportToText(const std::vector<std::shared_ptr<Metric>>& metrics,
                             const std::string& outputFile) = 0;
    
    virtual bool exportToJSON(const std::vector<std::shared_ptr<Metric>>& metrics,
                             const std::string& outputFile) = 0;
    
    virtual bool exportToCSV(const std::vector<std::shared_ptr<Metric>>& metrics,
                            const std::string& outputFile) = 0;
    
    virtual bool exportToXML(const std::vector<std::shared_ptr<Metric>>& metrics,
                            const std::string& outputFile) = 0;
};
```

## Alerting System

### Alert Types

1. **Threshold Alerts**: Trigger when metrics exceed thresholds
2. **Trend Alerts**: Trigger based on metric trends
3. **Anomaly Detection**: Trigger based on statistical anomalies
4. **Resource Alerts**: Trigger based on resource consumption
5. **Custom Alerts**: User-defined alert conditions

### Alert Configuration

```cpp
struct AlertCondition {
    std::string metricName;
    AlertType type;
    double threshold;
    std::string severity;
    std::string message;
    std::chrono::system_clock::time_point lastTriggered;
    int triggerCount;
};
```

### Alert Management

```cpp
class AlertManager {
public:
    void addAlertCondition(const AlertCondition& condition);
    void removeAlertCondition(const std::string& metricName);
    void updateAlertCondition(const AlertCondition& condition);
    
    void checkAlerts(const std::vector<std::shared_ptr<Metric>>& metrics);
    std::vector<AlertCondition> getActiveAlerts() const;
    void acknowledgeAlert(const std::string& metricName);
    
    void setAlertCallback(std::function<void(const AlertCondition&)> callback);
    void setAlertSeverityThreshold(const std::string& severity, double threshold);
};
```

## Performance Benchmarks

### Standard Benchmarks

1. **Configuration Loading**: Time to load configuration from file
2. **Brain Initialization**: Time to initialize brain with given configuration
3. **Single Step Processing**: Time for one simulation step
4. **Large Network Performance**: Performance with 1000+ neurons
5. **Memory Efficiency**: Memory usage per neuron
6. **I/O Performance**: File read/write speeds

### Benchmark Framework

```cpp
class PerformanceBenchmarker {
public:
    struct BenchmarkResult {
        std::string benchmarkName;
        double duration;
        double throughput;
        double efficiency;
        std::string details;
        std::vector<double> samples;
    };
    
    BenchmarkResult runBenchmark(const std::string& name,
                                 std::function<void()> benchmarkFunction,
                                 int iterations = 10);
    
    std::vector<BenchmarkResult> runAllBenchmarks();
    std::string generateBenchmarkReport() const;
    bool saveBenchmarkResults(const std::string& filepath) const;
};
```

## Integration with External Systems

### Database Integration

```cpp
class MetricsDatabase {
public:
    virtual ~MetricsDatabase() = default;
    
    virtual bool connect(const std::string& connectionString) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    virtual bool storeMetrics(const std::vector<std::shared_ptr<Metric>>& metrics) = 0;
    virtual std::vector<std::shared_ptr<Metric>> queryMetrics(const std::string& query) = 0;
    
    virtual bool createTable(const std::string& tableName) = 0;
    virtual bool dropTable(const std::string& tableName) = 0;
};
```

### Message Queue Integration

```cpp
class MetricsMessageQueue {
public:
    virtual ~MetricsMessageQueue() = default;
    
    virtual bool connect(const std::string& connectionString) = 0;
    virtual void disconnect() = 0;
    
    virtual bool publish(const std::shared_ptr<Metric>& metric) = 0;
    virtual bool subscribe(std::function<void(std::shared_ptr<Metric>)> callback) = 0;
};
```

### Visualization Integration

```cpp
class MetricsVisualization {
public:
    virtual ~MetricsVisualization() = default;
    
    virtual bool createRealTimeDashboard(const std::vector<std::shared_ptr<Metric>>& metrics) = 0;
    virtual bool createHistoricalCharts(const std::string& query, const std::string& outputFile) = 0;
    virtual bool createTrendAnalysis(const std::string& title, const std::string& dataFile) = 0;
};
```

## Configuration

### Metrics Configuration

```ini
[metrics]
collection_interval = 1.0
retention_period = 30
storage_type = sqlite
storage_file = metrics.db
enable_alerting = true
enable_visualization = true

[alerts]
threshold_cpu_usage = 90.0
threshold_memory_usage = 80.0
threshold_response_time = 1000.0

[export]
text_format = true
json_format = true
csv_format = true
xml_format = true

[visualization]
chart_type = line
refresh_interval = 5
```

### Sample Configuration File

```ini
# NLM Performance Monitoring Configuration

[collection]
interval = 1.0
retention_days = 30
workers = 4
batch_size = 100

[storage]
type = sqlite
file = metrics.db
table_prefix = nlm_

[alerts]
enabled = true

[cpu]
threshold_usage = 90.0
alert_severity = high

[memory]
threshold_usage = 80.0
alert_severity = medium

[simulation]
threshold_processing_time = 1000.0
alert_severity = low

[export]
text = true
json = true
csv = true
html = true
xml = true

[visualization]
enabled = true
dashboard_refresh = 5
```

## Usage Examples

### Basic Usage

```cpp
#include <nlm/Metrics.hpp>
#include <nlm/Brain.hpp>

int main() {
    // Create metrics collector
    auto metricsCollector = std::make_shared<MetricsCollector>();
    
    // Create brain monitor
    auto brainMonitor = std::make_shared<BrainMonitor>();
    
    // Load configuration
    auto config = std::make_shared<Config>();
    config->loadFromFile("config.cfg");
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Start monitoring
    brainMonitor->startMonitoring(brain);
    brainMonitor->addMetricCollector(metricsCollector);
    
    // Run simulation
    for (int i = 0; i < 1000; ++i) {
        brain->step(i, i * 0.001);
        brainMonitor->recordBrainMetrics();
        brainMonitor->recordSystemMetrics();
    }
    
    // Stop monitoring
    brainMonitor->stopMonitoring();
    
    // Generate report
    std::string report = brainMonitor->generateReport();
    std::cout << report << std::endl;
    
    // Save metrics
    metricsCollector->saveToFile("metrics.json");
    
    return 0;
}
```

### Command-Line Monitoring

```bash
# Start NLM with monitoring
./nlm_cli --enable-monitoring --metrics-interval=1.0 --metrics-output=metrics.json

# Show current metrics
./nlm_cli metrics show

# Show performance report
./nlm_cli metrics report

# Configure alerts
./nlm_cli metrics alert add --metric=cpu_usage --threshold=90.0 --severity=high

# List alerts
./nlm_cli metrics alert list
```

### Test Framework Integration

```cpp
#include "TestFramework.hpp"
#include "Metrics.hpp"

void setupTestMetrics() {
    auto testMetrics = std::make_shared<TestMetricsCollector>();
    TestFramework::getInstance().setMetricsCollector(testMetrics);
}

void recordTestMetrics(const TestCase& test) {
    // Record test start
    testMetrics->recordTestStart(test);
    
    // Run test
    bool passed = test.testFunction();
    
    // Record test end
    testMetrics->recordTestEnd(test, passed ? TestResult::Pass : TestResult::Fail);
    
    // Record metrics
    auto metrics = testMetrics->getTestMetrics();
    testMetrics->recordTestMetrics(test, metrics);
}
```

## Performance Monitoring Scripts

### Python Script for Metrics Analysis

```python
#!/usr/bin/env python3

import json
import matplotlib.pyplot as plt
import pandas as pd
from datetime import datetime

class MetricsAnalyzer:
    def __init__(self, metrics_file):
        self.metrics_file = metrics_file
        self.data = self.load_metrics()
    
    def load_metrics(self):
        with open(self.metrics_file, 'r') as f:
            return json.load(f)
    
    def plot_time_series(self, metric_name):
        df = pd.DataFrame(self.data)
        df['timestamp'] = pd.to_datetime(df['timestamp'])
        
        plt.figure(figsize=(12, 6))
        plt.plot(df['timestamp'], df[metric_name])
        plt.title(f'{metric_name} Over Time')
        plt.xlabel('Time')
        plt.ylabel(metric_name)
        plt.grid(True)
        plt.savefig(f'{metric_name}_time_series.png')
        plt.close()
    
    def calculate_statistics(self, metric_name):
        df = pd.DataFrame(self.data)
        return {
            'mean': df[metric_name].mean(),
            'median': df[metric_name].median(),
            'std': df[metric_name].std(),
            'min': df[metric_name].min(),
            'max': df[metric_name].max(),
            'count': len(df[metric_name])
        }
    
    def detect_anomalies(self, metric_name, threshold=3):
        df = pd.DataFrame(self.data)
        mean = df[metric_name].mean()
        std = df[metric_name].std()
        
        anomalies = df[np.abs(df[metric_name] - mean) > threshold * std]
        return anomalies

if __name__ == "__main__":
    analyzer = MetricsAnalyzer('metrics.json')
    
    # Plot time series for key metrics
    for metric in ['cpu_usage', 'memory_usage', 'firing_rate', 'processing_time']:
        if metric in analyzer.data[0]:
            analyzer.plot_time_series(metric)
    
    # Calculate statistics
    for metric in ['cpu_usage', 'memory_usage', 'firing_rate', 'processing_time']:
        if metric in analyzer.data[0]:
            stats = analyzer.calculate_statistics(metric)
            print(f"{metric} Statistics:")
            for key, value in stats.items():
                print(f"  {key}: {value:.4f}")
            print()
    
    # Detect anomalies
    for metric in ['cpu_usage', 'memory_usage']:
        if metric in analyzer.data[0]:
            anomalies = analyzer.detect_anomalies(metric)
            print(f"{metric} anomalies detected: {len(anomalies)}")
```

## Conclusion

The performance monitoring and metrics collection system provides comprehensive visibility into NLM system performance, enabling:

- **Performance optimization**: Identify and eliminate bottlenecks
- **Resource management**: Monitor and optimize resource usage
- **System reliability**: Detect and respond to performance issues
- **Capacity planning**: Plan for future growth and scaling
- **Benchmarking**: Compare performance across different configurations
- **Quality assurance**: Ensure performance meets requirements

This system integrates seamlessly with the existing NLM infrastructure and provides actionable insights for performance improvement and system optimization.
