# NLM Performance Profiling Documentation

## Overview

The NLM Performance Profiling system provides comprehensive performance analysis tools for neural simulations, enabling developers to identify bottlenecks, optimize performance, and monitor system behavior at scale. It includes memory profiling, performance analysis, and real-time monitoring capabilities.

## Key Features

### 1. Memory Profiling
- **Memory Allocation Tracking**: Track all memory allocations and deallocations
- **Memory Leak Detection**: Identify memory leaks and memory fragmentation
- **Memory Usage Analysis**: Analyze memory usage patterns and trends
- **Memory Pool Monitoring**: Monitor memory pool efficiency and utilization

### 2. Performance Analysis
- **Time Profiling**: Profile execution time of all simulation components
- **CPU Usage Analysis**: Analyze CPU usage patterns and bottlenecks
- **I/O Performance**: Monitor I/O operations and disk usage
- **Network Performance**: Monitor network operations (for distributed simulations)

### 3. Real-time Monitoring
- **Live Monitoring**: Real-time monitoring of simulation performance
- **Metric Collection**: Collect and analyze performance metrics
- **Alert System**: Configurable alerts for performance thresholds
- **Dashboard**: Visual dashboard for performance monitoring

### 4. Advanced Features
- **Performance Baselines**: Establish performance baselines for comparison
- **Performance Regression Detection**: Automatically detect performance regressions
- **Performance Optimization Suggestions**: Provide optimization recommendations
- **Integration with Build System**: Automatic performance profiling during build

## Architecture

### Core Components

#### 1. PerformanceProfiler.hpp (src/performance/PerformanceProfiler.hpp)
**Purpose**: Main performance profiling interface

**Key Features**:
- **Multi-Stage Profiling**: Setup, execution, and teardown phases
- **Component-Level Profiling**: Profile individual components separately
- **Metric Collection**: Collect and store performance metrics
- **Report Generation**: Generate performance reports

#### 2. PerformanceProfiler.cpp (src/performance/PerformanceProfiler.cpp)
**Purpose**: Implementation of performance profiling

**Key Features**:
- **Timing Measurement**: High-resolution timing measurement
- **Memory Tracking**: Memory allocation and deallocation tracking
- **Metric Aggregation**: Aggregation of performance metrics
- **Report Generation**: Generation of performance reports

#### 3. MemoryProfiler.hpp (src/performance/MemoryProfiler.hpp)
**Purpose**: Memory profiling and analysis

**Key Features**:
- **Allocation Tracking**: Track all memory allocations
- **Deallocation Tracking**: Track all memory deallocations
- **Leak Detection**: Detect memory leaks
- **Memory Usage Analysis**: Analyze memory usage patterns

#### 4. MemoryProfiler.cpp (src/performance/MemoryProfiler.cpp)
**Purpose**: Implementation of memory profiling

**Key Features**:
- **Detailed Memory Analysis**: Detailed analysis of memory usage
- **Leak Detection**: Detect and report memory leaks
- **Fragmentation Analysis**: Analyze memory fragmentation
- **Pool Analysis**: Analyze memory pool efficiency

#### 5. ConfigurationProfiler.hpp (src/performance/ConfigurationProfiler.hpp)
**Purpose**: Configuration profiling and analysis

**Key Features**:
- **Configuration Analysis**: Analyze configuration effects on performance
- **Parameter Optimization**: Optimize configuration parameters for performance
- **Performance Prediction**: Predict performance based on configuration
- **Configuration Validation**: Validate configuration for performance

#### 6. ConfigurationProfiler.cpp (src/performance/ConfigurationProfiler.cpp)
**Purpose**: Implementation of configuration profiling

**Key Features**:
- **Configuration Analysis**: Analyze configuration effects on performance
- **Parameter Tuning**: Optimize configuration parameters for performance
- **Performance Prediction**: Predict performance based on configuration
- **Validation**: Validate configuration for performance

## Profiling Tools

### 1. Performance Analyzer
**Purpose**: Analyze performance data and generate reports

**Key Features**:
- **Data Visualization**: Visualize performance data
- **Statistical Analysis**: Statistical analysis of performance data
- **Trend Analysis**: Analyze performance trends
- **Comparison Analysis**: Compare performance across different runs

### 2. Memory Analyzer
**Purpose**: Analyze memory usage and identify issues

**Key Features**:
- **Memory Usage Analysis**: Analyze memory usage patterns
- **Leak Detection**: Detect memory leaks
- **Fragmentation Analysis**: Analyze memory fragmentation
- **Pool Analysis**: Analyze memory pool efficiency

### 3. Configuration Analyzer
**Purpose**: Analyze configuration effects on performance

**Key Features**:
- **Parameter Analysis**: Analyze configuration parameters
- **Optimization Suggestions**: Provide optimization recommendations
- **Performance Prediction**: Predict performance based on configuration
- **Validation**: Validate configuration for performance

### 4. Real-time Monitor
**Purpose**: Real-time monitoring of performance

**Key Features**:
- **Live Metrics**: Collect and display live metrics
- **Alert System**: Configurable alerts for performance thresholds
- **Dashboard**: Visual dashboard for performance monitoring
- **Historical Analysis**: Analyze historical performance data

## Performance Metrics

### Core Metrics

| Metric | Description | Measurement | Target |
|--------|-------------|-------------|--------|
| **Simulation Speed** | Steps per second | `steps / wall_time` | 1000+ |
| **Memory Efficiency** | Memory usage per neuron | `memory / neurons` | < 1KB |
| **CPU Utilization** | CPU usage percentage | `cpu_time / wall_time * 100` | < 90% |
| **I/O Throughput** | Input/output operations per second | `I/O_ops / time` | 10000+ |
| **Network Latency** | Network operation latency | `network_time / ops` | < 1ms |

### Detailed Metrics

#### Simulation Metrics
- **Steps Processed**: Total number of simulation steps
- **Spikes Generated**: Total number of neural spikes
- **Synapses Processed**: Total number of synaptic events
- **Memory Allocations**: Number of memory allocations
- **Memory Deallocations**: Number of memory deallocations

#### Performance Metrics
- **Wall Time**: Total elapsed time
- **CPU Time**: Total CPU time used
- **Real-time Factor**: Simulated time / wall clock time
- **Throughput**: Operations per second
- **Efficiency**: CPU time / wall time

#### Memory Metrics
- **Peak Memory**: Maximum memory usage
- **Average Memory**: Average memory usage
- **Memory Allocations**: Number of memory allocations
- **Memory Deallocations**: Number of memory deallocations
- **Memory Leak Detection**: Memory leak detection

#### I/O Metrics
- **Read Operations**: Number of read operations
- **Write Operations**: Number of write operations
- **Read Throughput**: Read operations per second
- **Write Throughput**: Write operations per second
- **I/O Latency**: Average I/O operation latency

## Profiling Tools API

### Performance Analyzer API

```cpp
#include "performance/PerformanceAnalyzer.hpp"

class PerformanceAnalyzer {
public:
    // Data loading
    bool loadData(const std::string& filename);
    bool loadData(const std::vector<PerformanceMetric>& metrics);
    
    // Analysis
    PerformanceReport analyze();
    PerformanceReport analyze(const AnalysisOptions& options);
    
    // Visualization
    void generatePlot(const std::string& outputFile, PlotType type);
    void generateReport(const std::string& outputFile);
    
    // Statistical analysis
    double calculateAverage(const std::string& metricName);
    double calculateMedian(const std::string& metricName);
    double calculateStdDev(const std::string& metricName);
    
    // Trend analysis
    std::vector<double> analyzeTrend(const std::string& metricName);
    double predictFuture(const std::string& metricName, int steps);
};
```

### Memory Analyzer API

```cpp
#include "performance/MemoryAnalyzer.hpp"

class MemoryAnalyzer {
public:
    // Data loading
    bool loadMemoryData(const std::string& filename);
    
    // Analysis
    MemoryReport analyze();
    MemoryReport analyze(const MemoryAnalysisOptions& options);
    
    // Leak detection
    std::vector<MemoryLeak> detectLeaks();
    std::vector<MemoryLeak> detectLeaks(const LeakDetectionOptions& options);
    
    // Fragmentation analysis
    FragmentationReport analyzeFragmentation();
    double calculateFragmentationScore();
    
    // Pool analysis
    PoolReport analyzePools();
    std::vector<PoolEfficiency> analyzePoolEfficiency();
};
```

### Configuration Analyzer API

```cpp
#include "performance/ConfigurationProfiler.hpp"

class ConfigurationAnalyzer {
public:
    // Configuration loading
    bool loadConfiguration(const std::string& filename);
    bool loadConfiguration(const std::map<std::string, std::any>& config);
    
    // Performance prediction
    PerformancePrediction predictPerformance(const std::string& scenario);
    PerformancePrediction predictPerformance(const std::map<std::string, double>& parameters);
    
    // Optimization
    OptimizationResult optimizePerformance();
    OptimizationResult optimizePerformance(const OptimizationConstraints& constraints);
    
    // Validation
    bool validateConfiguration(const std::map<std::string, std::any>& config);
    std::vector<ConfigurationError> getValidationErrors();
};
```

### Real-time Monitor API

```cpp
#include "performance/RealTimeMonitor.hpp"

class RealTimeMonitor {
public:
    // Setup
    void initialize(const MonitorConfig& config);
    void startMonitoring();
    void stopMonitoring();
    
    // Metric collection
    void collectMetrics();
    std::map<std::string, double> getCurrentMetrics();
    std::vector<std::map<std::string, double>> getHistoricalMetrics(int seconds);
    
    // Alerting
    void addAlert(const AlertRule& rule);
    void removeAlert(const std::string& alertId);
    std::vector<Alert> getActiveAlerts();
    
    // Dashboard
    void updateDashboard();
    std::string getDashboardHTML();
    void exportDashboard(const std::string& filename);
};
```

## Usage Examples

### Example 1: Basic Performance Profiling

```cpp
#include "performance/PerformanceProfiler.hpp"

int main() {
    // Create performance profiler
    PerformanceProfiler profiler("simulation_profile");
    
    // Setup profiling
    profiler.setup();
    
    // Run simulation
    runSimulation();
    
    // Teardown profiling
    profiler.teardown();
    
    // Generate report
    PerformanceReport report = profiler.generateReport();
    
    // Print report
    std::cout << report.toString() << std::endl;
    
    // Save report to file
    profiler.saveReport("simulation_report.txt");
    
    return 0;
}
```

### Example 2: Memory Profiling

```cpp
#include "performance/MemoryProfiler.hpp"

int main() {
    // Create memory profiler
    MemoryProfiler memoryProfiler("memory_profile");
    
    // Setup memory profiling
    memoryProfiler.setup();
    
    // Run simulation
    runMemoryIntensiveSimulation();
    
    // Teardown memory profiling
    memoryProfiler.teardown();
    
    // Analyze memory usage
    MemoryReport report = memoryProfiler.analyze();
    
    // Print memory report
    std::cout << report.toString() << std::endl;
    
    // Check for memory leaks
    std::vector<MemoryLeak> leaks = memoryProfiler.detectLeaks();
    if (!leaks.empty()) {
        std::cout << "Memory leaks detected:" << std::endl;
        for (const auto& leak : leaks) {
            std::cout << "  - " << leak.toString() << std::endl;
        }
    }
    
    return 0;
}
```

### Example 3: Configuration Profiling

```cpp
#include "performance/ConfigurationProfiler.hpp"

int main() {
    // Create configuration profiler
    ConfigurationProfiler configProfiler("config_profile");
    
    // Load configuration
    if (!configProfiler.loadConfiguration("config.json")) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }
    
    // Predict performance
    PerformancePrediction prediction = configProfiler.predictPerformance("large_simulation");
    
    // Print prediction
    std::cout << "Performance Prediction:" << std::endl;
    std::cout << "  Steps per second: " << prediction.throughput << std::endl;
    std::cout << "  Memory usage: " << prediction.memoryUsage << " MB" << std::endl;
    std::cout << "  CPU utilization: " << prediction.cpuUtilization << "%" << std::endl;
    
    // Optimize configuration
    OptimizationResult optimization = configProfiler.optimizePerformance();
    
    // Print optimization results
    std::cout << "Optimization Results:" << std::endl;
    std::cout << "  Improvement: " << optimization.improvementPercentage << "%" << std::endl;
    std::cout << "  New throughput: " << optimization.newThroughput << " steps/sec" << std::endl;
    std::cout << "  New memory usage: " << optimization.newMemoryUsage << " MB" << std::endl;
    
    return 0;
}
```

### Example 4: Real-time Monitoring

```cpp
#include "performance/RealTimeMonitor.hpp"

int main() {
    // Create real-time monitor
    RealTimeMonitor monitor("simulation_monitor");
    
    // Setup monitoring
    MonitorConfig config;
    config.metricsToCollect = {"steps", "spikes", "memory", "cpu"};
    config.alertThresholds = {
        {"steps_per_second", 1000, AlertType::WARNING},
        {"memory_usage", 512, AlertType::ERROR}
    };
    config.updateInterval = 1.0; // 1 second
    
    monitor.initialize(config);
    monitor.startMonitoring();
    
    // Run simulation with real-time monitoring
    runSimulationWithMonitoring();
    
    // Stop monitoring
    monitor.stopMonitoring();
    
    // Get monitoring data
    std::map<std::string, double> currentMetrics = monitor.getCurrentMetrics();
    std::vector<std::map<std::string, double>> historicalMetrics = 
        monitor.getHistoricalMetrics(60); // Last 60 seconds
    
    // Print metrics
    std::cout << "Current Metrics:" << std::endl;
    for (const auto& metric : currentMetrics) {
        std::cout << "  " << metric.first << ": " << metric.second << std::endl;
    }
    
    // Get active alerts
    std::vector<Alert> activeAlerts = monitor.getActiveAlerts();
    if (!activeAlerts.empty()) {
        std::cout << "Active Alerts:" << std::endl;
        for (const auto& alert : activeAlerts) {
            std::cout << "  - " << alert.toString() << std::endl;
        }
    }
    
    return 0;
}
```

### Example 5: Performance Analysis

```cpp
#include "performance/PerformanceAnalyzer.hpp"

int main() {
    // Create performance analyzer
    PerformanceAnalyzer analyzer("simulation_data");
    
    // Load performance data
    if (!analyzer.loadData("simulation_metrics.json")) {
        std::cerr << "Failed to load performance data" << std::endl;
        return 1;
    }
    
    // Analyze performance
    PerformanceReport report = analyzer.analyze();
    
    // Print analysis results
    std::cout << "Performance Analysis:" << std::endl;
    std::cout << "  Average throughput: " << report.averageThroughput << " steps/sec" << std::endl;
    std::cout << "  Median throughput: " << report.medianThroughput << " steps/sec" << std::endl;
    std::cout << "  Throughput standard deviation: " << report.throughputStdDev << " steps/sec" << std::endl;
    std::cout << "  Memory efficiency: " << report.memoryEfficiency << " neurons/MB" << std::endl;
    
    // Generate visualization
    analyzer.generatePlot("throughput_plot.png", PlotType::LINE);
    
    // Generate report
    analyzer.generateReport("performance_report.html");
    
    // Predict future performance
    std::vector<double> trend = analyzer.analyzeTrend("throughput");
    double futurePrediction = analyzer.predictFuture("throughput", 10);
    
    std::cout << "Performance Trend (last 10 runs):" << std::endl;
    for (size_t i = 0; i < trend.size(); ++i) {
        std::cout << "  Run " << i << ": " << trend[i] << " steps/sec" << std::endl;
    }
    
    std::cout << "Predicted throughput (10 runs ahead): " << futurePrediction << " steps/sec" << std::endl;
    
    return 0;
}
```

### Example 6: Memory Analysis

```cpp
#include "performance/MemoryAnalyzer.hpp"

int main() {
    // Create memory analyzer
    MemoryAnalyzer analyzer("memory_data");
    
    // Load memory data
    if (!analyzer.loadMemoryData("memory_metrics.json")) {
        std::cerr << "Failed to load memory data" << std::endl;
        return 1;
    }
    
    // Analyze memory usage
    MemoryReport report = analyzer.analyze();
    
    // Print memory analysis
    std::cout << "Memory Analysis:" << std::endl;
    std::cout << "  Peak memory: " << report.peakMemory << " MB" << std::endl;
    std::cout << "  Average memory: " << report.averageMemory << " MB" << std::endl;
    std::cout << "  Memory efficiency: " << report.memoryEfficiency << " neurons/MB" << std::endl;
    
    // Detect memory leaks
    std::vector<MemoryLeak> leaks = analyzer.detectLeaks();
    if (!leaks.empty()) {
        std::cout << "Memory Leaks Detected:" << std::endl;
        for (const auto& leak : leaks) {
            std::cout << "  - " << leak.toString() << std::endl;
        }
    } else {
        std::cout << "No memory leaks detected." << std::endl;
    }
    
    // Analyze fragmentation
    FragmentationReport fragmentationReport = analyzer.analyzeFragmentation();
    std::cout << "Fragmentation Analysis:" << std::endl;
    std::cout << "  Fragmentation score: " << fragmentationReport.fragmentationScore << std::endl;
    std::cout << "  Internal fragmentation: " << fragmentationReport.internalFragmentation << std::endl;
    std::cout << "  External fragmentation: " << fragmentationReport.externalFragmentation << std::endl;
    
    // Analyze memory pools
    PoolReport poolReport = analyzer.analyzePools();
    std::cout << "Memory Pool Analysis:" << std::endl;
    std::cout << "  Total pool efficiency: " << poolReport.totalEfficiency << "%" << std::endl;
    std::cout << "  Average pool efficiency: " << poolReport.averageEfficiency << "%" << std::endl;
    
    return 0;
}
```

### Example 7: Configuration Analysis

```cpp
#include "performance/ConfigurationProfiler.hpp"

int main() {
    // Create configuration analyzer
    ConfigurationAnalyzer analyzer("config_data");
    
    // Load configuration
    if (!analyzer.loadConfiguration("config.json")) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }
    
    // Validate configuration
    if (!analyzer.validateConfiguration({})) {
        std::cout << "Configuration validation errors:" << std::endl;
        std::vector<ConfigurationError> errors = analyzer.getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  - " << error.toString() << std::endl;
        }
    }
    
    // Predict performance for different scenarios
    std::vector<std::string> scenarios = {"small", "medium", "large", "xlarge"};
    for (const auto& scenario : scenarios) {
        PerformancePrediction prediction = analyzer.predictPerformance(scenario);
        std::cout << "Scenario: " << scenario << std::endl;
        std::cout << "  Throughput: " << prediction.throughput << " steps/sec" << std::endl;
        std::cout << "  Memory: " << prediction.memoryUsage << " MB" << std::endl;
        std::cout << "  CPU: " << prediction.cpuUtilization << "%" << std::endl;
        std::cout << std::endl;
    }
    
    // Optimize configuration
    OptimizationResult optimization = analyzer.optimizePerformance();
    std::cout << "Optimization Results:" << std::endl;
    std::cout << "  Improvement: " << optimization.improvementPercentage << "%" << std::endl;
    std::cout << "  New throughput: " << optimization.newThroughput << " steps/sec" << std::endl;
    std::cout << "  New memory: " << optimization.newMemoryUsage << " MB" << std::endl;
    std::cout << "  New CPU: " << optimization.newCpuUtilization << "%" << std::endl;
    
    return 0;
}
```

## Performance Monitoring Commands

### Command Line Tools

```bash
# Profile simulation performance
./nlm --profile --profile-file simulation_profile.json

# Analyze memory usage
./nlm --memory-profile --memory-file memory_profile.json

# Profile configuration performance
./nlm --config-profile --config-file config.json

# Run real-time monitoring
./nlm --monitor --monitor-port 8080

# Generate performance report
./nlm --generate-report --report-file performance_report.html

# Validate configuration for performance
./nlm --validate-config --config-file config.json

# Analyze performance data
./nlm --analyze --data-file simulation_data.json --output analysis.html

# Monitor memory leaks
./nlm --detect-leaks --memory-file memory_profile.json
```

### Performance Monitoring Scripts

```bash
#!/bin/bash
# profile_simulation.sh - Profile simulation performance

# Run simulation with profiling
./nlm --profile --profile-file simulation_profile.json \
     --neurons 10000 --steps 100000

# Analyze performance data
./nlm --analyze --data-file simulation_profile.json \
     --output analysis.html --metrics throughput,memory,cpu

# Generate report
./nlm --generate-report --report-file performance_report.html

echo "Performance profiling completed"

#!/bin/bash
# monitor_memory.sh - Monitor memory usage

# Run memory profiling
./nlm --memory-profile --memory-file memory_profile.json \
     --neurons 50000 --steps 500000

# Detect memory leaks
./nlm --detect-leaks --memory-file memory_profile.json

# Generate memory report
./nlm --generate-memory-report --report-file memory_report.html

# Analyze memory efficiency
./nlm --analyze-memory --data-file memory_profile.json

echo "Memory monitoring completed"

#!/bin/bash
# optimize_configuration.sh - Optimize configuration for performance

# Create configuration templates
python create_configs.py

# Test configuration performance
for config in configs/*.json; do
    ./nlm --config-profile --config-file "$config" \
          --neurons 10000 --steps 10000
    ./nlm --analyze --data-file profile.json --metrics throughput
    rm profile.json
    echo "Processed $config"
done

# Select best configuration
python select_best_config.py

echo "Configuration optimization completed"
```

## Performance Monitoring Best Practices

### 1. Profile Before Optimizing
```cpp
// Always profile before optimizing
PerformanceProfiler profiler("baseline_profile");
profiler.setup();
runSimulation();
profiler.teardown();
PerformanceReport baseline = profiler.generateReport();

// Make changes
modifyCode();

// Profile after changes
PerformanceProfiler profiler2("optimized_profile");
profiler2.setup();
runSimulation();
profiler2.teardown();
PerformanceReport optimized = profiler2.generateReport();

// Compare results
double improvement = (optimized.averageThroughput - baseline.averageThroughput) / baseline.averageThroughput * 100;
std::cout << "Improvement: " << improvement << "%" << std::endl;
```

### 2. Monitor Key Metrics
```cpp
// Focus on key metrics
struct PerformanceMetrics {
    double throughput;      // Steps per second
    double memoryEfficiency; // Neurons per MB
    double cpuUtilization;   // CPU usage percentage
    double responseTime;     // Average response time
};

// Collect metrics
PerformanceMetrics metrics = monitor.getCurrentMetrics();

// Log metrics
logMetric("throughput", metrics.throughput);
logMetric("memory_efficiency", metrics.memoryEfficiency);
logMetric("cpu_utilization", metrics.cpuUtilization);
logMetric("response_time", metrics.responseTime);
```

### 3. Set Appropriate Thresholds
```cpp
// Set realistic thresholds
AlertRule throughputRule = {"throughput", 1000, AlertType::WARNING, 100};
AlertRule memoryRule = {"memory_usage", 512, AlertType::ERROR, 100};
AlertRule cpuRule = {"cpu_utilization", 90, AlertType::WARNING, 100};

// Configure alerts
monitor.addAlert(throughputRule);
monitor.addAlert(memoryRule);
monitor.addAlert(cpuRule);
```

### 4. Use Consistent Baselines
```cpp
// Always use the same baseline for comparison
PerformanceReport baseline = loadReport("baseline_profile.json");

// Make changes
modifyCode();

// Always use the same baseline for comparison
PerformanceReport optimized = runProfileAndGenerateReport("optimized_profile.json");

// Calculate improvement relative to baseline
double throughputImprovement = (optimized.averageThroughput - baseline.averageThroughput) / baseline.averageThroughput * 100;
double memoryImprovement = (baseline.memoryEfficiency - optimized.memoryEfficiency) / baseline.memoryEfficiency * 100;
```

### 5. Document Performance Changes
```cpp
// Document performance changes
struct PerformanceChange {
    std::string changeDescription;
    double throughputChange;
    double memoryChange;
    std::string commitHash;
    std::string author;
    std::chrono::system_clock::time_point timestamp;
};

// Record performance change
PerformanceChange change;
change.changeDescription = "Optimized neural population processing";
change.throughputChange = 15.5;
change.memoryChange = -2.3;
change.commitHash = getCurrentGitHash();
change.author = getGitAuthor();
change.timestamp = std::chrono::system_clock::now();

// Save to performance log
savePerformanceChange(change);
```

## Performance Monitoring Integration

### Integration with Build System

```bash
# Add performance profiling to CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 1.0.0)

# Add performance profiling target
add_custom_target(profile
    COMMAND ./nlm --profile --profile-file simulation_profile.json
    DEPENDS nlm
    COMMENT "Running performance profiling"
)

# Add performance analysis target
add_custom_target(analyze
    COMMAND ./nlm --analyze --data-file simulation_profile.json --output analysis.html
    DEPENDS profile
    COMMENT "Analyzing performance data"
)

# Add performance report target
add_custom_target(report
    COMMAND ./nlm --generate-report --report-file performance_report.html
    DEPENDS analyze
    COMMENT "Generating performance report"
)
```

### Integration with CI/CD

```yaml
# .github/workflows/performance.yml
name: Performance Testing

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  performance:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Build with performance
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON
        make -j$(nproc)
    
    - name: Run performance tests
      run: |
        ./nlm --profile --profile-file simulation_profile.json
        ./nlm --analyze --data-file simulation_profile.json --output analysis.html
        ./nlm --generate-report --report-file performance_report.html
    
    - name: Upload performance artifacts
      uses: actions/upload-artifact@v3
      with:
        name: performance-report
        path: performance_report.html
        retention-days: 7
```

### Integration with Monitoring Systems

```cpp
// Integration with external monitoring systems
class ExternalMonitor {
public:
    void sendMetrics(const PerformanceMetrics& metrics);
    void sendAlert(const Alert& alert);
    void sendPerformanceReport(const PerformanceReport& report);
};

void integrateWithExternalMonitoring() {
    ExternalMonitor monitor;
    
    // Send metrics
    PerformanceMetrics metrics = monitor.getCurrentMetrics();
    monitor.sendMetrics(metrics);
    
    // Send alerts
    std::vector<Alert> alerts = monitor.getActiveAlerts();
    for (const auto& alert : alerts) {
        monitor.sendAlert(alert);
    }
    
    // Send performance report
    PerformanceReport report = generatePerformanceReport();
    monitor.sendPerformanceReport(report);
}
```

## Performance Monitoring Tools

### Performance Profiling Tools

| Tool | Purpose | Command |
|------|---------|---------|
| **PerformanceProfiler** | Profile simulation performance | `./nlm --profile --profile-file simulation_profile.json` |
| **MemoryProfiler** | Profile memory usage | `./nlm --memory-profile --memory-file memory_profile.json` |
| **ConfigurationProfiler** | Profile configuration performance | `./nlm --config-profile --config-file config.json` |

### Performance Analysis Tools

| Tool | Purpose | Command |
|------|---------|---------|
| **PerformanceAnalyzer** | Analyze performance data | `./nlm --analyze --data-file simulation_profile.json --output analysis.html` |
| **MemoryAnalyzer** | Analyze memory usage | `./nlm --analyze-memory --data-file memory_profile.json` |
| **ConfigurationAnalyzer** | Analyze configuration performance | `./nlm --analyze-config --data-file config_profile.json` |

### Performance Reporting Tools

| Tool | Purpose | Command |
|------|---------|---------|
| **ReportGenerator** | Generate performance report | `./nlm --generate-report --report-file performance_report.html` |
| **HTMLReporter** | Generate HTML performance report | `./nlm --html-report --report-file performance_report.html` |
| **JSONReporter** | Generate JSON performance report | `./nlm --json-report --report-file performance_report.json` |

## Performance Monitoring Dashboard

### Dashboard Configuration

```json
{
  "dashboard": {
    "title": "NLM Performance Dashboard",
    "refreshInterval": 5000,
    "metrics": [
      {
        "name": "throughput",
        "title": "Steps per Second",
        "unit": "steps/sec",
        "color": "blue",
        "format": "#,##0"
      },
      {
        "name": "memory_efficiency",
        "title": "Memory Efficiency",
        "unit": "neurons/MB",
        "color": "green",
        "format": "#,##0.0"
      },
      {
        "name": "cpu_utilization",
        "title": "CPU Utilization",
        "unit": "%",
        "color": "orange",
        "format": "#,##0"
      },
      {
        "name": "response_time",
        "title": "Response Time",
        "unit": "ms",
        "color": "red",
        "format": "#,##0"
      }
    ],
    "alerts": [
      {
        "name": "high_throughput",
        "metric": "throughput",
        "threshold": 1000,
        "severity": "warning"
      },
      {
        "name": "high_memory",
        "metric": "memory_usage",
        "threshold": 512,
        "severity": "error"
      }
    ]
  }
}
```

### Dashboard Usage

```bash
# Start monitoring dashboard
./nlm --dashboard --dashboard-port 8080 --dashboard-config dashboard_config.json

# Access dashboard
# Open web browser and navigate to http://localhost:8080
```

## Performance Monitoring Reports

### Performance Report Format

```json
{
  "report": {
    "title": "NLM Performance Report",
    "date": "2023-09-23T02:14:50Z",
    "version": "1.0.0",
    "metrics": {
      "throughput": {
        "average": 1500.5,
        "median": 1492.3,
        "min": 1200.0,
        "max": 1800.0,
        "stdDev": 150.2,
        "trend": "increasing"
      },
      "memory": {
        "average": 256.3,
        "peak": 512.8,
        "min": 128.5,
        "efficiency": 1024.0,
        "trend": "stable"
      }
    },
    "recommendations": [
      "Consider reducing batch size to improve memory usage",
      "Optimize neural population processing to increase throughput"
    ]
  }
}
```

### Report Generation

```bash
# Generate performance report
./nlm --generate-report --report-file performance_report.json --format json

# Generate HTML report
./nlm --generate-report --report-file performance_report.html --format html

# Generate PDF report
./nlm --generate-report --report-file performance_report.pdf --format pdf
```

## Performance Monitoring Examples

### Example 1: Complete Performance Monitoring Workflow

```bash
#!/bin/bash
# run_performance_monitoring.sh

echo "Starting performance monitoring..."

# Step 1: Profile simulation
./nlm --profile --profile-file simulation_profile.json --neurons 10000 --steps 100000

echo "Profiling completed"

# Step 2: Analyze performance data
./nlm --analyze --data-file simulation_profile.json --output analysis.html --metrics throughput,memory,cpu

echo "Analysis completed"

# Step 3: Generate performance report
./nlm --generate-report --report-file performance_report.html

echo "Report generated"

# Step 4: Upload to monitoring system
./nlm --upload-report --report-file performance_report.html --to monitoring-system

echo "Performance monitoring completed successfully"
```

### Example 2: Memory Leak Detection

```bash
#!/bin/bash
# detect_memory_leaks.sh

echo "Detecting memory leaks..."

# Step 1: Run memory-intensive simulation
./nlm --memory-profile --memory-file memory_profile.json --neurons 50000 --steps 500000

echo "Memory profiling completed"

# Step 2: Detect memory leaks
./nlm --detect-leaks --memory-file memory_profile.json --threshold 10

echo "Memory leak detection completed"

# Step 3: Generate memory report
./nlm --generate-memory-report --report-file memory_report.html

echo "Memory report generated"

# Step 4: Analyze memory leaks
./nlm --analyze-leaks --report-file memory_report.html

echo "Memory leak analysis completed"
```

### Example 3: Configuration Optimization

```bash
#!/bin/bash
# optimize_configuration.sh

echo "Optimizing configuration for performance..."

# Step 1: Create configuration templates
python create_configs.py

echo "Configuration templates created"

# Step 2: Test each configuration
best_config=""
best_performance=0

for config in configs/*.json; do
    echo "Testing configuration: $config"
    
    # Run simulation with current configuration
    ./nlm --config-profile --config-file "$config" --neurons 10000 --steps 10000
    
    # Analyze performance
    performance=$(./nlm --analyze --data-file profile.json --metrics throughput)
    
    # Update best configuration if current is better
    if [ "$performance" -gt "$best_performance" ]; then
        best_performance=$performance
        best_config=$config
    fi
    
    # Clean up
    rm profile.json
    
done

echo "Best configuration: $best_config (performance: $best_performance steps/sec)"

# Step 3: Save best configuration
cp "$best_config" configs/optimized_config.json

echo "Configuration optimization completed"
```

## Performance Monitoring Best Practices

### 1. Always Profile Before Optimizing
- Establish baseline performance
- Profile after making changes
- Compare results to measure improvement

### 2. Monitor Key Metrics
- Focus on throughput, memory efficiency, CPU utilization
- Set realistic thresholds for alerts
- Use consistent baselines for comparison

### 3. Use Appropriate Tools
- Use performance profiler for overall performance
- Use memory profiler for memory issues
- Use configuration profiler for configuration effects

### 4. Document Performance Changes
- Document performance improvements
- Track performance regressions
- Maintain performance baselines

### 5. Integrate with CI/CD
- Add performance tests to CI/CD pipeline
- Monitor performance trends over time
- Set performance thresholds for releases

## Conclusion

The NLM Performance Profiling system provides comprehensive performance analysis tools for neural simulations. It enables developers to:

1. **Profile Performance**: Profile simulation performance, memory usage, and configuration effects
2. **Analyze Data**: Analyze performance data, detect issues, and generate insights
3. **Monitor in Real-time**: Monitor performance in real-time with alerts and dashboards
4. **Optimize Performance**: Optimize configuration parameters for better performance
5. **Integrate with CI/CD**: Integrate performance monitoring with build and deployment pipelines

The performance profiling system is production-ready, well-integrated with the existing codebase, and provides a comprehensive foundation for performance optimization and monitoring.
