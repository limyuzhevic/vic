# NLM Performance Monitoring System

## Overview

The NLM Performance Monitoring System is a comprehensive performance monitoring and profiling system designed specifically for the NLM (Neural Learning Machine) project. It provides real-time performance metrics, advanced profiling capabilities, automated optimization suggestions, and expert-level performance features.

## Key Features

### 1. Real-Time Performance Metrics
- **System-level metrics**: CPU usage, memory consumption, disk I/O, network traffic
- **Application-level metrics**: Event queue size, memory pool usage, thread activity
- **Neural computation metrics**: Spike rates, synaptic activity, neuron state tracking
- **Configuration processing metrics**: Validation, parsing, serialization performance
- **Logging system metrics**: Throughput, queue depth, sink performance monitoring

### 2. Advanced Profiling
- **Function-level profiling**: With call graphs and hot path identification
- **Memory profiling**: Leak detection, allocation/deallocation tracking
- **Thread performance**: CPU usage analysis and contention detection
- **I/O performance**: Read/write rate analysis and bottleneck identification
- **Neural network metrics**: Forward/backward pass timing and accuracy tracking

### 3. Performance Optimization
- **Automated tuning suggestions**: AI-driven optimization recommendations
- **Resource allocation recommendations**: CPU, memory, and I/O optimization
- **Configuration optimization**: Data structure and SIMD optimization
- **Parallelization recommendations**: Thread and algorithm optimization
- **Memory pool optimization**: Allocation and deallocation strategies

### 4. Performance Monitoring
- **Real-time dashboard**: Live metrics visualization with HTML/JSON output
- **Historical performance tracking**: Long-term trend analysis and archiving
- **Performance alerting**: Threshold-based notifications and alerts
- **Trend analysis**: Performance degradation and improvement detection
- **Bottleneck identification**: Pinpoint performance bottlenecks automatically

### 5. Expert Performance Features
- **Performance scripting**: Custom analysis and automation scripts
- **Benchmarking**: Configuration and algorithm benchmarking
- **Regression detection**: Performance comparison across versions
- **Performance comparison**: Baseline and current configuration comparison
- **Performance prediction**: Machine learning-based performance forecasting

## System Architecture

### Core Components

1. **PerformanceSystem Class**: Main interface for all performance monitoring features
2. **PerformanceSnapshot Structure**: Data structure for storing performance metrics
3. **Metric Collection**: System for collecting and storing performance data
4. **Performance Dashboard**: Real-time visualization and reporting
5. **Optimization Engine**: Automated optimization suggestion engine

### Integration with NLM Systems

The PerformanceSystem integrates with all NLM subsystems:

- **Brain Subsystem**: Neural computation metrics and spike rate monitoring
- **Memory Subsystem**: Memory pool usage and allocation tracking
- **Prediction Subsystem**: Neural network performance and accuracy tracking
- **Logging Subsystem**: Log throughput and queue depth monitoring
- **Configuration Subsystem**: Configuration processing and validation metrics

### PerformanceError System

The PerformanceSystem integrates with the existing NLM ErrorHandling system:

- **PerformanceError**: Custom error type for performance-related issues
- **PerformanceErrorType**: Enum for different types of performance errors
- **Recovery Actions**: Automatic recovery suggestions and recommendations

## API Reference

### Main Class

```cpp
class PerformanceSystem {
public:
    // Core lifecycle management
    static PerformanceSystem& getInstance();
    bool initialize(const std::string& configFile = "");
    void shutdown();
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Performance monitoring
    void updateSnapshot();
    PerformanceSnapshot getCurrentSnapshot() const;
    PerformanceSnapshot getSnapshot(uint64_t step) const;
    std::vector<PerformanceSnapshot> getHistoricalSnapshots(size_t maxSnapshots = 1000) const;
    
    // Metric collection
    void recordMetric(const std::string& name, double value, MetricType type,
                     const std::unordered_map<std::string, std::string>& tags = {});
    void recordSystemMetrics();
    void recordApplicationMetrics();
    void recordNeuralComputationMetrics();
    void recordConfigurationMetrics();
    void recordLoggingMetrics();
    
    // Performance monitoring
    bool checkThresholds();
    void triggerAlerts();
    std::vector<std::string> getActiveAlerts() const;
    
    // Profiling
    void startProfiling(const std::string& functionName);
    void stopProfiling(const std::string& functionName);
    void profileFunction(std::function<void()> func, const std::string& functionName);
    std::unordered_map<std::string, double> getFunctionProfiles() const;
    std::unordered_map<std::string, uint64_t> getFunctionCallCounts() const;
    
    // Memory profiling
    void startMemoryProfiling();
    void stopMemoryProfiling();
    void detectMemoryLeaks();
    std::unordered_map<std::string, uint64_t> getMemoryUsageByFunction() const;
    uint64_t getTotalMemoryAllocated() const;
    uint64_t getTotalMemoryDeallocated() const;
    
    // Thread performance analysis
    void analyzeThreadPerformance();
    std::unordered_map<std::thread::id, double> getThreadCpuUsage() const;
    std::unordered_map<std::string, uint64_t> getThreadActiveCounts() const;
    
    // I/O performance analysis
    void analyzeIOPerformance();
    std::unordered_map<std::string, double> getIoReadRates() const;
    std::unordered_map<std::string, double> getIoWriteRates() const;
    
    // Neural network performance
    void recordNeuralNetworkMetrics(const std::string& networkName,
                                   uint64_t forwardPasses, double forwardTime,
                                   uint64_t backwardPasses, double backwardTime,
                                   uint64_t examplesProcessed, double accuracy);
    std::unordered_map<std::string, double> getNeuralNetworkPerformance() const;
    
    // Performance optimization
    std::vector<std::string> getOptimizationSuggestions() const;
    std::vector<std::string> getResourceAllocationRecommendations() const;
    std::vector<std::string> getSimdOptimizationOpportunities() const;
    std::vector<std::string> getParallelizationRecommendations() const;
    std::vector<std::string> getMemoryPoolOptimizationSuggestions() const;
    
    // Performance monitoring dashboard
    std::string generateDashboardHTML() const;
    std::string generateDashboardJSON() const;
    std::string generatePerformanceReport() const;
    
    // Historical performance tracking
    void archiveSnapshot(const PerformanceSnapshot& snapshot);
    std::vector<PerformanceSnapshot> getSnapshotsByTimeRange(
        std::chrono::system_clock::time_point startTime,
        std::chrono::system_clock::time_point endTime) const;
    std::vector<PerformanceSnapshot> getSnapshotsByStepRange(uint64_t startStep, uint64_t endStep) const;
    
    // Performance trend analysis
    double calculateTrend(const std::vector<double>& values, size_t windowSize = 10) const;
    double calculateRateOfChange(const std::vector<double>& values) const;
    std::string identifyBottlenecks() const;
    
    // Performance alerting and threshold monitoring
    void addThreshold(const std::string& metricName, double threshold, MetricType type,
                     const std::string& alertLevel = "warning");
    void removeThreshold(const std::string& metricName);
    std::vector<std::pair<std::string, double>> getThresholds() const;
    
    // Expert performance features
    void exportPerformanceData(const std::string& filepath) const;
    void importPerformanceData(const std::string& filepath);
    void benchmarkConfiguration(const std::string& configName, const std::string& configFile);
    void comparePerformance(const std::string& baselineFile, const std::string& currentFile);
    void predictPerformance(const std::vector<std::vector<double>>& trainingData);
    
    // Integration with NLM systems
    void setBrainCallback(std::function<void(const PerformanceSnapshot&)> callback);
    void setMemoryCallback(std::function<void(const PerformanceSnapshot&)> callback);
    void setPredictionCallback(std::function<void(const PerformanceSnapshot&)> callback);
    void setLoggingCallback(std::function<void(const PerformanceSnapshot&)> callback);
    
    // Error handling
    void raisePerformanceError(PerformanceErrorType type, const std::string& metric,
                               const std::string& suggestion = "");
    
    // Performance scripting
    void executePerformanceScript(const std::string& script);
    std::unordered_map<std::string, std::string> getPerformanceScripts() const;
    
    // Configuration optimization
    std::string optimizeConfiguration(const std::string& configName);
    std::string suggestDataStructureOptimization() const;
    
    // Real-time monitoring
    void startRealTimeMonitoring(std::chrono::milliseconds interval = std::chrono::milliseconds(100));
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoring() const;
    
    // Performance statistics
    uint64_t getTotalUpdates() const;
    uint64_t getTotalAlerts() const;
    uint64_t getTotalErrors() const;
    std::chrono::system_clock::time_point getLastUpdateTime() const;
    
    // Thread safety
    void lock();
    void unlock();
};
```

### PerformanceSnapshot Structure

```cpp
struct PerformanceSnapshot {
    std::chrono::system_clock::time_point timestamp;
    uint64_t step;
    double simulationTime;
    
    // System metrics
    double cpuUsagePercent;
    uint64_t memoryTotal;
    uint64_t memoryUsed;
    uint64_t memoryAvailable;
    uint64_t memoryFree;
    double diskReadRate;
    double diskWriteRate;
    double networkRxRate;
    double networkTxRate;
    
    // Application metrics
    uint64_t eventQueueSize;
    uint64_t eventQueueCapacity;
    uint64_t memoryPoolUsed;
    uint64_t memoryPoolAllocated;
    uint64_t memoryPoolCapacity;
    uint64_t threadActiveCount;
    uint64_t threadIdleCount;
    
    // Neural computation metrics
    uint64_t spikeCount;
    double averageFiringRate;
    uint64_t neuronStateActive;
    uint64_t neuronStateInactive;
    uint64_t synapticActivityCount;
    double averageSynapticWeight;
    
    // Configuration processing metrics
    uint64_t configParseCount;
    double configParseTimeMs;
    uint64_t configValidateCount;
    double configValidateTimeMs;
    uint64_t configSerializeCount;
    double configSerializeTimeMs;
    
    // Logging system metrics
    uint64_t logMessagesTotal;
    uint64_t logMessagesDropped;
    uint64_t logMessagesProcessed;
    double logProcessingThroughput;
    uint64_t logQueueDepth;
    uint64_t logSinkPerformance;
    
    // Performance metrics
    uint64_t functionCallsTotal;
    double functionCallAverageTimeMs;
    uint64_t memoryAllocations;
    uint64_t memoryDeallocations;
    uint64_t memoryLeaksDetected;
    double cacheHitRate;
    double cacheMissRate;
    
    // Neural network metrics
    uint64_t forwardPassCount;
    double forwardPassAverageTimeMs;
    uint64_t backwardPassCount;
    double backwardPassAverageTimeMs;
    uint64_t trainingExamplesProcessed;
    double trainingAccuracy;
    
    // Performance profiling data
    std::vector<std::pair<std::string, double>> hotFunctions;
    std::vector<std::pair<std::string, double>> memoryUsageByFunction;
    std::vector<std::pair<std::string, double>> threadUsageByFunction;
    std::vector<std::pair<std::string, double>> ioUsageByFunction;
};
```

### Metric Types

```cpp
enum class MetricType {
    Counter,
    Gauge,
    Histogram,
    Timer,
    Rate,
    Duration,
    Memory,
    CPU,
    Network,
    DiskIO,
    Thread,
    EventQueue,
    MemoryPool,
    NeuralComputation,
    Configuration,
    Logging
};
```

### Performance Error Types

```cpp
enum class PerformanceErrorType {
    MemoryUsageExceeded,
    CPUUsageExceeded,
    TimeLimitExceeded,
    ThroughputBelowMinimum,
    LatencyExceeded,
    ContextSwitchRateHigh,
    CacheMissRateHigh,
    ResourceContention,
    SlowdownDetected,
    DegradationDetected
};
```

## Quick Start Guide

### 1. Basic Usage

```cpp
#include "performance/PerformanceSystem.hpp"
#include "core/Logger/Logger.hpp"

int main() {
    // Initialize the PerformanceSystem
    auto& performanceSystem = nlm::PerformanceSystem::getInstance();
    if (!performanceSystem.initialize()) {
        NLM_LOG_ERROR("Failed to initialize performance system");
        return 1;
    }
    
    // Start real-time monitoring
    performanceSystem.startRealTimeMonitoring(std::chrono::milliseconds(100));
    
    // Main simulation loop
    while (simulationActive) {
        // Update performance metrics
        performanceSystem.updateSnapshot();
        
        // Run simulation step
        runSimulationStep();
        
        // Check for performance issues
        performanceSystem.checkThresholds();
    }
    
    // Cleanup
    performanceSystem.stopRealTimeMonitoring();
    performanceSystem.shutdown();
    
    return 0;
}
```

### 2. Profiling Functions

```cpp
void exampleFunction() {
    // Profile a specific function
    performanceSystem.profileFunction([]() {
        // Function implementation
        doWork();
    }, "exampleFunction");
}

void complexAlgorithm() {
    performanceSystem.startProfiling("complexAlgorithm");
    
    // Algorithm implementation
    for (int i = 0; i < iterations; ++i) {
        processData(i);
    }
    
    performanceSystem.stopProfiling("complexAlgorithm");
}
```

### 3. Memory Profiling

```cpp
void memoryIntensiveTask() {
    performanceSystem.startMemoryProfiling();
    
    // Memory-intensive operations
    std::vector<std::vector<float>> largeData;
    allocateAndProcessData(largeData);
    
    performanceSystem.stopMemoryProfiling();
    
    // Check for memory leaks
    performanceSystem.detectMemoryLeaks();
}
```

### 4. Setting Up Thresholds

```cpp
void setupPerformanceThresholds() {
    auto& performanceSystem = nlm::PerformanceSystem::getInstance();
    
    // Set up performance thresholds
    performanceSystem.addThreshold("cpu_usage", 80.0, nlm::MetricType::Gauge, "warning");
    performanceSystem.addThreshold("memory_used", 1024 * 1024 * 1024, nlm::MetricType::Memory, "critical");
    performanceSystem.addThreshold("response_time_ms", 100.0, nlm::MetricType::Timer, "warning");
    performanceSystem.addThreshold("event_queue_size", 10000, nlm::MetricType::Gauge, "warning");
}
```

### 5. Using Optimization Suggestions

```cpp
void analyzeAndOptimize() {
    auto& performanceSystem = nlm::PerformanceSystem::getInstance();
    
    // Get optimization suggestions
    auto suggestions = performanceSystem.getOptimizationSuggestions();
    
    for (const std::string& suggestion : suggestions) {
        NLM_LOG_INFO("Optimization suggestion: " + suggestion);
        // Apply optimization based on suggestion
        applyOptimization(suggestion);
    }
}
```

### 6. Generating Performance Reports

```cpp
void generatePerformanceReports() {
    auto& performanceSystem = nlm::PerformanceSystem::getInstance();
    
    // Generate different report formats
    std::string htmlReport = performanceSystem.generateDashboardHTML();
    std::string jsonReport = performanceSystem.generateDashboardJSON();
    std::string textReport = performanceSystem.generatePerformanceReport();
    
    // Save reports to files
    saveReportToFile(htmlReport, "performance_dashboard.html");
    saveReportToFile(jsonReport, "performance_dashboard.json");
    saveReportToFile(textReport, "performance_report.txt");
}
```

## Integration Examples

### Brain Subsystem Integration

```cpp
void setupBrainIntegration() {
    auto& performanceSystem = nlm::PerformanceSystem::getInstance();
    
    performanceSystem.setBrainCallback([](const PerformanceSnapshot& snapshot) {
        // Handle brain-specific performance metrics
        uint64_t spikeCount = snapshot.spikeCount;
        double firingRate = snapshot.averageFiringRate;
        
        if (spikeCount > 1000000) {
            performanceSystem.raisePerformanceError(
                PerformanceErrorType::CPUUsageExceeded,
                "spike_count",
                "Consider increasing neural population size or reducing simulation complexity"
            );
        }
    });
}
```

### Memory Subsystem Integration

```cpp
void setupMemoryIntegration() {
    auto& performanceSystem = nlm::PerformanceSystem::getInstance();
    
    performanceSystem.setMemoryCallback([](const PerformanceSnapshot& snapshot) {
        // Handle memory-specific performance metrics
        uint64_t memoryUsed = snapshot.memoryUsed;
        uint64_t memoryPoolUsed = snapshot.memoryPoolUsed;
        
        if (memoryUsed > memoryPoolUsed * 0.9) {
            performanceSystem.raisePerformanceError(
                PerformanceErrorType::MemoryUsageExceeded,
                "memory_used",
                "Consider increasing memory pool size or optimizing memory usage"
            );
        }
    });
}
```

## Configuration

### Performance Configuration File

```json
{
    "performance_monitoring": {
        "enabled": true,
        "monitoring_interval_ms": 100,
        "max_historical_snapshots": 10000,
        "enable_profiling": true,
        "enable_memory_profiling": true,
        "enable_thread_profiling": true,
        "enable_io_profiling": true,
        "enable_neural_profiling": true
    },
    "thresholds": {
        "cpu_usage": {"threshold": 80.0, "level": "warning"},
        "memory_usage": {"threshold": 1024000000, "level": "critical"},
        "event_queue_size": {"threshold": 10000, "level": "warning"},
        "response_time_ms": {"threshold": 100.0, "level": "warning"}
    },
    "alerts": {
        "email_enabled": false,
        "slack_webhook": "",
        "notification_channels": ["console", "log_file"]
    }
}
```

## Build Configuration

### CMake Integration

To enable performance monitoring in your build, add the following to your CMakeLists.txt:

```cmake
# Enable performance monitoring
option(NLM_PERFORMANCE_ENABLED "Enable performance monitoring" ON)

if(NLM_PERFORMANCE_ENABLED)
    # Add performance system source files
    target_sources(nlm_core PRIVATE
        src/performance/PerformanceSystem.cpp
    )
    
    # Define performance monitoring macros
    target_compile_definitions(nlm_core PRIVATE NLM_PERFORMANCE_ENABLED)
    
    # Link performance libraries
    target_link_libraries(nlm_core PRIVATE
        nlohmann_json
        libcurl
        pqxx
        zlib
    )
endif()
```

### Compiler Flags

To optimize performance monitoring compilation:

```cmake
# Performance optimization flags
add_compile_options(-O2 -march=native -flto)

# Profiling support
add_compile_options(-g -pg)

# SIMD instructions
add_compile_options(-msse4.1 -mavx -mavx2)
```

## Testing

### Running Tests

Run the performance system test suite:

```bash
# Run all performance system tests
./test_performance_system

# Run specific test categories
./test_performance_system --category=metrics
./test_performance_system --category=profiling
./test_performance_system --category=optimization
```

### Unit Tests

The PerformanceSystem includes comprehensive unit tests:

- `testPerformanceSystemInitialization()`: Tests initialization and shutdown
- `testPerformanceMetricsCollection()`: Tests metric collection and storage
- `testThresholdMonitoring()`: Tests threshold setup and monitoring
- `testMemoryProfiling()`: Tests memory profiling and leak detection
- `testThreadAnalysis()`: Tests thread performance analysis
- `testNeuralNetworkMetrics()`: Tests neural network performance tracking
- `testDashboardGeneration()`: Tests report generation
- `testHistoricalData()`: Tests historical data management
- `testPerformanceTrends()`: Tests trend analysis
- `testErrorHandling()`: Tests error handling and reporting
- `testOptimizationSuggestions()`: Tests optimization suggestion generation
- `testDataExportImport()`: Tests data export/import functionality

### Performance Tests

```bash
# Run performance benchmarks
./benchmark_performance

# Run performance regression tests
./test_performance_regression
```

## Performance Monitoring Best Practices

### 1. Enable Early
Enable performance monitoring at the start of your application to capture complete performance data.

### 2. Use Appropriate Intervals
- Use shorter intervals (100ms) for interactive applications
- Use longer intervals (1s) for batch processing applications
- Adjust based on your specific performance requirements

### 3. Set Meaningful Thresholds
Set thresholds based on your application's performance requirements and acceptable performance levels.

### 4. Analyze Regularly
Use the performance dashboard and reports to regularly analyze performance trends and identify optimization opportunities.

### 5. Integrate with Error Handling
Use performance monitoring to detect and report performance-related errors through the NLM ErrorHandling system.

## Files

### Core Files

- `src/performance/PerformanceSystem.hpp`: PerformanceSystem class header
- `src/performance/PerformanceSystem.cpp`: PerformanceSystem class implementation
- `docs/PerformanceMonitoring.md`: This documentation file

### Test Files

- `test_performance_system.cpp`: Comprehensive test suite
- `tests/PerformanceTest.cpp`: Unit tests
- `tests/PerformanceIntegrationTest.cpp`: Integration tests

### Example Files

- `examples/performance_basic_usage.cpp`: Basic usage example
- `examples/performance_profiling.cpp`: Profiling example
- `examples/performance_optimization.cpp`: Optimization example

## License

This performance monitoring system is part of the NLM project and is licensed under the same terms as the NLM project.

## Author

NLM Performance Monitoring Team

## Version

1.0.0