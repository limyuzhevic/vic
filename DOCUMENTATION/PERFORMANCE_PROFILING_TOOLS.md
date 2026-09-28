# Performance Profiling and Benchmarking Tools for NLM

## Overview
This document describes the performance profiling and benchmarking tools for NLM (Neural Learning Machine), including comprehensive performance monitoring, benchmarking capabilities, and analysis tools for optimizing neural simulation performance.

## Components

### 1. Performance Profiler
- **File**: `src/performance/PerformanceProfiler.cpp`
- **Purpose**: Comprehensive performance monitoring and real-time metrics collection
- **Features**:
  - Real-time performance monitoring
  - Memory usage tracking
  - CPU profiling
  - Spike performance analysis
  - Network performance metrics

### 2. Benchmarking Suite
- **File**: `src/performance/BenchmarkingSuite.cpp`
- **Purpose**: Standardized performance benchmarking across different configurations
- **Features**:
  - Scalability testing
  - Performance comparison
  - Configuration optimization
  - Performance regression testing
  - Resource utilization analysis

### 3. Performance Analyzer
- **File**: `src/performance/PerformanceAnalyzer.cpp`
- **Purpose**: Advanced performance analysis and optimization recommendations
- **Features**:
  - Performance trend analysis
  - Bottleneck identification
  - Optimization recommendations
  - Performance prediction
  - Continuous performance monitoring

### 4. Resource Monitor
- **File**: `src/performance/ResourceMonitor.cpp`
- **Purpose**: System resource monitoring and management
- **Features**:
  - Memory usage monitoring
  - CPU usage monitoring
  - Network I/O monitoring
  - Disk I/O monitoring
  - Process monitoring

### 5. Performance Reports
- **File**: `src/performance/PerformanceReports.cpp`
- **Purpose**: Comprehensive performance reporting and visualization
- **Features**:
  - Performance report generation
  - Metrics visualization
  - Trend analysis
  - Performance comparison
  - Automated reporting

## Performance Profiler Implementation

### Real-Time Performance Monitoring
```cpp
namespace nlm {

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start performance monitoring
    void startMonitoring(const Brain& brain);
    
    // Stop performance monitoring
    void stopMonitoring();
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Get performance history
    std::vector<PerformanceMetrics> getPerformanceHistory() const;
    
    // Set monitoring parameters
    void setMonitoringParameters(const MonitoringParameters& params);
    
    // Enable/disable monitoring subsystems
    void enableMonitoring(MonitoringSubsystem subsystem, bool enable);
    
    // Get system resources
    SystemResources getSystemResources() const;
    
    // Check performance thresholds
    PerformanceStatus checkPerformanceThresholds() const;
};

} // namespace nlm
```

### Memory Profiling
```cpp
namespace nlm {

class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Start memory profiling
    void startMemoryProfiling(const Brain& brain);
    
    // Stop memory profiling
    void stopMemoryProfiling();
    
    // Get memory usage statistics
    MemoryStatistics getMemoryStatistics() const;
    
    // Get memory allocation history
    std::vector<MemoryAllocation> getMemoryAllocationHistory() const;
    
    // Set memory profiling parameters
    void setMemoryProfilingParameters(const MemoryProfilingParameters& params);
    
    // Detect memory leaks
    MemoryLeakReport detectMemoryLeaks() const;
    
    // Analyze memory fragmentation
    MemoryFragmentationReport analyzeFragmentation() const;
};

} // namespace nlm
```

### CPU Performance Profiling
```cpp
namespace nlm {

class CPUProfiler {
public:
    CPUProfiler();
    ~CPUProfiler();
    
    // Start CPU profiling
    void startCPUProfiling();
    
    // Stop CPU profiling
    void stopCPUProfiling();
    
    // Get CPU performance metrics
    CPUMetrics getCPUMetrics() const;
    
    // Get thread performance data
    std::vector<ThreadMetrics> getThreadMetrics() const;
    
    // Set CPU profiling parameters
    void setCPUProfilingParameters(const CPUProfilingParameters& params);
    
    // Identify performance bottlenecks
    BottleneckReport identifyCPUBottlenecks() const;
    
    // Analyze cache performance
    CachePerformanceReport analyzeCachePerformance() const;
};

} // namespace nlm
```

### Spike Performance Analysis
```cpp
namespace nlm {

class SpikePerformanceAnalyzer {
public:
    SpikePerformanceAnalyzer();
    ~SpikePerformanceAnalyzer();
    
    // Analyze spike generation performance
    SpikePerformanceMetrics analyzeSpikePerformance(const Brain& brain);
    
    // Analyze spike propagation
    SpikePropagationMetrics analyzeSpikePropagation(const Brain& brain);
    
    // Analyze spike timing
    SpikeTimingMetrics analyzeSpikeTiming(const Brain& brain);
    
    // Analyze spike correlations
    SpikeCorrelationMetrics analyzeSpikeCorrelations(const Brain& brain);
    
    // Set spike performance parameters
    void setSpikePerformanceParameters(const SpikePerformanceParameters& params);
    
    // Get spike performance trends
    SpikePerformanceTrends getSpikePerformanceTrends() const;
};

} // namespace nlm
```

### Network Performance Analysis
```cpp
namespace nlm {

class NetworkPerformanceAnalyzer {
public:
    NetworkPerformanceAnalyzer();
    ~NetworkPerformanceAnalyzer();
    
    // Analyze network throughput
    NetworkThroughputMetrics analyzeNetworkThroughput(const Brain& brain);
    
    // Analyze network efficiency
    NetworkEfficiencyMetrics analyzeNetworkEfficiency(const Brain& brain);
    
    // Analyze network scalability
    NetworkScalabilityMetrics analyzeNetworkScalability(const Brain& brain);
    
    // Analyze network connectivity
    NetworkConnectivityMetrics analyzeNetworkConnectivity(const Brain& brain);
    
    // Set network performance parameters
    void setNetworkPerformanceParameters(const NetworkPerformanceParameters& params);
    
    // Get network performance metrics
    NetworkPerformanceMetrics getNetworkPerformanceMetrics(const Brain& brain) const;
};

} // namespace nlm
```

## Benchmarking Suite Implementation

### Standard Benchmark Suite
```cpp
namespace nlm {

class BenchmarkingSuite {
public:
    BenchmarkingSuite();
    ~BenchmarkingSuite();
    
    // Run scalability benchmark
    ScalabilityResults runScalabilityBenchmark(const ScalabilityConfig& config);
    
    // Run configuration benchmark
    ConfigurationResults runConfigurationBenchmark(const ConfigurationConfig& config);
    
    // Run performance benchmark
    PerformanceResults runPerformanceBenchmark(const PerformanceConfig& config);
    
    // Run memory benchmark
    MemoryResults runMemoryBenchmark(const MemoryConfig& config);
    
    // Run CPU benchmark
    CPUResults runCPUBenchmark(const CPUConfig& config);
    
    // Set benchmarking parameters
    void setBenchmarkingParameters(const BenchmarkingParameters& params);
    
    // Get benchmark results
    BenchmarkResults getBenchmarkResults() const;
    
    // Export benchmark results
    void exportBenchmarkResults(const std::string& filepath);
    
    // Generate benchmark report
    std::string generateBenchmarkReport() const;
};

} // namespace nlm
```

### Scalability Benchmarking
```cpp
namespace nlm {

class ScalabilityBenchmarker {
public:
    ScalabilityBenchmarker();
    ~ScalabilityBenchmarker();
    
    // Test neuron scalability
    NeuronScalabilityResults testNeuronScalability(const std::vector<size_t>& neuronCounts);
    
    // Test region scalability
    RegionScalabilityResults testRegionScalability(const std::vector<size_t>& regionCounts);
    
    // Test connection scalability
    ConnectionScalabilityResults testConnectionScalability(const std::vector<float>& connectionProbabilities);
    
    // Set scalability parameters
    void setScalabilityParameters(const ScalabilityParameters& params);
    
    // Get scalability results
    ScalabilityResults getScalabilityResults() const;
};

} // namespace nlm
```

### Configuration Benchmarking
```cpp
namespace nlm {

class ConfigurationBenchmarker {
public:
    ConfigurationBenchmarker();
    ~ConfigurationBenchmarker();
    
    // Test different connectivity patterns
    ConnectivityResults testConnectivityPatterns(const std::vector<float>& connectionProbabilities);
    
    // Test different plasticity settings
    PlasticityResults testPlasticitySettings(const std::vector<PlasticitySettings>& plasticitySettings);
    
    // Test different neuromodulation settings
    NeuromodulationResults testNeuromodulationSettings(const std::vector<NeuromodulationSettings>& neuromodulationSettings);
    
    // Test different development settings
    DevelopmentResults testDevelopmentSettings(const std::vector<DevelopmentSettings>& developmentSettings);
    
    // Set configuration benchmarking parameters
    void setConfigurationBenchmarkingParameters(const ConfigurationBenchmarkingParameters& params);
    
    // Get configuration benchmarking results
    ConfigurationBenchmarkingResults getConfigurationBenchmarkingResults() const;
};

} // namespace nlm
```

### Performance Benchmarking
```cpp
namespace nlm {

class PerformanceBenchmarker {
public:
    PerformanceBenchmarker();
    ~PerformanceBenchmarker();
    
    // Run throughput benchmark
    ThroughputResults runThroughputBenchmark(const Brain& brain, const PerformanceConfig& config);
    
    // Run latency benchmark
    LatencyResults runLatencyBenchmark(const Brain& brain, const PerformanceConfig& config);
    
    // Run efficiency benchmark
    EfficiencyResults runEfficiencyBenchmark(const Brain& brain, const PerformanceConfig& config);
    
    // Run accuracy benchmark
    AccuracyResults runAccuracyBenchmark(const Brain& brain, const PerformanceConfig& config);
    
    // Set performance benchmarking parameters
    void setPerformanceBenchmarkingParameters(const PerformanceBenchmarkingParameters& params);
    
    // Get performance benchmarking results
    PerformanceBenchmarkingResults getPerformanceBenchmarkingResults() const;
};

} // namespace nlm
```

## Performance Analyzer Implementation

### Performance Analysis
```cpp
namespace nlm {

class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    // Analyze performance trends
    PerformanceTrends analyzePerformanceTrends(const PerformanceHistory& history);
    
    // Identify performance bottlenecks
    BottleneckAnalysis identifyPerformanceBottlenecks(const Brain& brain);
    
    // Generate optimization recommendations
    OptimizationRecommendations generateOptimizationRecommendations(const Brain& brain);
    
    // Predict performance
    PerformancePrediction predictPerformance(const Brain& brain, const PredictionParameters& params);
    
    // Set performance analysis parameters
    void setPerformanceAnalysisParameters(const PerformanceAnalysisParameters& params);
    
    // Analyze performance patterns
    PerformancePatternAnalysis analyzePerformancePatterns(const Brain& brain);
};

} // namespace nlm
```

### Bottleneck Analysis
```cpp
namespace nlm {

class BottleneckAnalyzer {
public:
    BottleneckAnalyzer();
    ~BottleneckAnalyzer();
    
    // Analyze memory bottlenecks
    MemoryBottleneckAnalysis analyzeMemoryBottlenecks(const Brain& brain);
    
    // Analyze CPU bottlenecks
    CPUBottleneckAnalysis analyzeCPUBottlenecks(const Brain& brain);
    
    // Analyze network bottlenecks
    NetworkBottleneckAnalysis analyzeNetworkBottlenecks(const Brain& brain);
    
    // Analyze algorithm bottlenecks
    AlgorithmBottleneckAnalysis analyzeAlgorithmBottlenecks(const Brain& brain);
    
    // Set bottleneck analysis parameters
    void setBottleneckAnalysisParameters(const BottleneckAnalysisParameters& params);
    
    // Get bottleneck analysis results
    BottleneckAnalysisResults getBottleneckAnalysisResults() const;
};

} // namespace nlm
```

### Optimization Recommendations
```cpp
namespace nlm {

class OptimizationRecommender {
public:
    OptimizationRecommender();
    ~OptimizationRecommender();
    
    // Recommend memory optimizations
    MemoryOptimizationRecommendations recommendMemoryOptimizations(const Brain& brain);
    
    // Recommend CPU optimizations
    CPUOptimizationRecommendations recommendCPOptimizations(const Brain& brain);
    
    // Recommend network optimizations
    NetworkOptimizationRecommendations recommendNetworkOptimizations(const Brain& brain);
    
    // Recommend algorithm optimizations
    AlgorithmOptimizationRecommendations recommendAlgorithmOptimizations(const Brain& brain);
    
    // Set optimization recommendation parameters
    void setOptimizationRecommendationParameters(const OptimizationRecommendationParameters& params);
    
    // Get optimization recommendations
    OptimizationRecommendations getOptimizationRecommendations() const;
};

} // namespace nlm
```

## Resource Monitor Implementation

### Resource Monitoring
```cpp
namespace nlm {

class ResourceMonitor {
public:
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Start resource monitoring
    void startResourceMonitoring(const Brain& brain);
    
    // Stop resource monitoring
    void stopResourceMonitoring();
    
    // Get system resources
    SystemResources getSystemResources() const;
    
    // Get process resources
    ProcessResources getProcessResources() const;
    
    // Get network resources
    NetworkResources getNetworkResources() const;
    
    // Get disk resources
    DiskResources getDiskResources() const;
    
    // Set resource monitoring parameters
    void setResourceMonitoringParameters(const ResourceMonitoringParameters& params);
    
    // Check resource thresholds
    ResourceStatus checkResourceThresholds() const;
};

} // namespace nlm
```

### Memory Monitoring
```cpp
namespace nlm {

class MemoryMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor();
    
    // Start memory monitoring
    void startMemoryMonitoring(const Brain& brain);
    
    // Stop memory monitoring
    void stopMemoryMonitoring();
    
    // Get memory usage
    MemoryUsage getMemoryUsage() const;
    
    // Get memory allocation history
    std::vector<MemoryAllocation> getMemoryAllocationHistory() const;
    
    // Set memory monitoring parameters
    void setMemoryMonitoringParameters(const MemoryMonitoringParameters& params);
    
    // Detect memory leaks
    MemoryLeakReport detectMemoryLeaks() const;
    
    // Analyze memory usage patterns
    MemoryUsagePatterns analyzeMemoryUsagePatterns() const;
};

} // namespace nlm
```

### CPU Monitoring
```cpp
namespace nlm {

class CPUMonitor {
public:
    CPUMonitor();
    ~CPUMonitor();
    
    // Start CPU monitoring
    void startCPUMonitoring();
    
    // Stop CPU monitoring
    void stopCPUMonitoring();
    
    // Get CPU usage
    CPUUsage getCPUUsage() const;
    
    // Get core usage
    CoreUsage getCoreUsage() const;
    
    // Get process CPU usage
    ProcessCPUUsage getProcessCPUUsage() const;
    
    // Set CPU monitoring parameters
    void setCPUMonitoringParameters(const CPUMonitoringParameters& params);
    
    // Check CPU thresholds
    CPUStatus checkCPUThresholds() const;
};

} // namespace nlm
```

### Network Monitoring
```cpp
namespace nlm {

class NetworkMonitor {
public:
    NetworkMonitor();
    ~NetworkMonitor();
    
    // Start network monitoring
    void startNetworkMonitoring();
    
    // Stop network monitoring
    void stopNetworkMonitoring();
    
    // Get network usage
    NetworkUsage getNetworkUsage() const;
    
    // Get I/O statistics
    IOStatistics getIOStatistics() const;
    
    // Set network monitoring parameters
    void setNetworkMonitoringParameters(const NetworkMonitoringParameters& params);
    
    // Check network thresholds
    NetworkStatus checkNetworkThresholds() const;
};

} // namespace nlm
```

### Disk Monitoring
```cpp
namespace nlm {

class DiskMonitor {
public:
    DiskMonitor();
    ~DiskMonitor();
    
    // Start disk monitoring
    void startDiskMonitoring();
    
    // Stop disk monitoring
    void stopDiskMonitoring();
    
    // Get disk usage
    DiskUsage getDiskUsage() const;
    
    // Get I/O statistics
    DiskIOStatistics getDiskIOStatistics() const;
    
    // Set disk monitoring parameters
    void setDiskMonitoringParameters(const DiskMonitoringParameters& params);
    
    // Check disk thresholds
    DiskStatus checkDiskThresholds() const;
};

} // namespace nlm
```

## Performance Reports Implementation

### Performance Report Generation
```cpp
namespace nlm {

class PerformanceReporter {
public:
    PerformanceReporter();
    ~PerformanceReporter();
    
    // Generate performance report
    PerformanceReport generateReport(const PerformanceData& data);
    
    // Generate benchmark report
    BenchmarkReport generateBenchmarkReport(const BenchmarkResults& results);
    
    // Generate scalability report
    ScalabilityReport generateScalabilityReport(const ScalabilityResults& results);
    
    // Generate configuration report
    ConfigurationReport generateConfigurationReport(const ConfigurationResults& results);
    
    // Generate memory report
    MemoryReport generateMemoryReport(const MemoryResults& results);
    
    // Generate CPU report
    CPUReport generateCPUReport(const CPUResults& results);
    
    // Set report generation parameters
    void setReportGenerationParameters(const ReportGenerationParameters& params);
    
    // Export report
    void exportReport(const PerformanceReport& report, const std::string& filepath);
    
    // Generate HTML report
    std::string generateHTMLReport(const PerformanceReport& report) const;
    
    // Generate JSON report
    std::string generateJSONReport(const PerformanceReport& report) const;
    
    // Generate CSV report
    std::string generateCSVReport(const PerformanceReport& report) const;
};

} // namespace nlm
```

### Metrics Visualization
```cpp
namespace nlm {

class MetricsVisualizer {
public:
    MetricsVisualizer();
    ~MetricsVisualizer();
    
    // Create performance dashboard
    PerformanceDashboard createPerformanceDashboard(const PerformanceData& data);
    
    // Create metrics chart
    MetricsChart createMetricsChart(const PerformanceData& data, const MetricsChartConfig& config);
    
    // Create trend chart
    TrendChart createTrendChart(const PerformanceData& data, const TrendChartConfig& config);
    
    // Create comparison chart
    ComparisonChart createComparisonChart(const std::vector<PerformanceData>& data, const ComparisonChartConfig& config);
    
    // Set visualization parameters
    void setVisualizationParameters(const VisualizationParameters& params);
    
    // Export visualization
    void exportVisualization(const PerformanceDashboard& dashboard, const std::string& filepath);
    
    // Generate interactive visualization
    std::string generateInteractiveVisualization(const PerformanceDashboard& dashboard) const;
};

} // namespace nlm
```

## Configuration Files

### Performance Profiler Configuration
**File**: `configs/performance_profiler.cfg`
```ini
[PerformanceProfiler]
enabled = true
sampling_interval = 0.1
performance_monitoring = true
memory_profiling = true
cpu_profiling = true
network_profiling = true
spike_analysis = true
network_analysis = true

[RealTimeMonitoring]
update_interval = 0.5
metrics_thresholds = 0.90
memory_threshold_mb = 2048
cpu_threshold_percent = 80.0

[Benchmarking]
enable_benchmarks = true
benchmark_duration = 300.0
benchmark_steps = 1000
warmup_steps = 100
repeats = 3

[ResourceMonitoring]
enable_system_monitoring = true
enable_memory_monitoring = true
enable_cpu_monitoring = true
enable_network_monitoring = true
enable_disk_monitoring = true

[Reporting]
generate_reports = true
report_interval = 300.0
export_format = "json"
include_charts = true
include_trends = true
```

### Benchmarking Suite Configuration
**File**: `configs/benchmarking_suite.cfg`
```ini
[BenchmarkingSuite]
enabled = true
scalability_testing = true
configuration_testing = true
performance_testing = true
memory_testing = true
cpu_testing = true

[Scalability]
test_neuron_scalability = true
test_region_scalability = true
test_connection_scalability = true
neuron_counts = [100, 500, 1000, 5000, 10000]
region_counts = [1, 2, 4, 8]
connection_probabilities = [0.01, 0.05, 0.1, 0.2, 0.5]

[Configuration]
test_connectivity_patterns = true
test_plasticity_settings = true
test_neuromodulation_settings = true
test_development_settings = true

[Performance]
test_throughput = true
test_latency = true
test_efficiency = true
test_accuracy = true

[ResourceUsage]
enable_memory_profiling = true
enable_cpu_profiling = true
enable_network_profiling = true
enable_disk_profiling = true
```

## Usage Examples

### 1. Performance Profiling
```cpp
#include "performance/PerformanceProfiler.hpp"

int main() {
    // Create performance profiler
    nlm::PerformanceProfiler profiler;
    
    // Start performance monitoring
    profiler.startMonitoring();
    
    // Create brain
    auto config = nlm::create_default_config();
    config->set("neuron_count", 1000);
    auto brain = nlm::create_brain(config);
    brain->initialize();
    
    // Run simulation with monitoring
    for (int step = 0; step < 5000; ++step) {
        brain->step(step);
    }
    
    // Stop monitoring
    profiler.stopMonitoring();
    
    // Get performance metrics
    auto metrics = profiler.getCurrentMetrics();
    
    std::cout << "Performance Report:" << std::endl;
    std::cout << "  Total duration: " << metrics.duration << "s" << std::endl;
    std::cout << "  Steps per second: " << metrics.stepsPerSecond << std::endl;
    std::cout << "  Memory usage: " << metrics.memoryUsage << "MB" << std::endl;
    std::cout << "  CPU usage: " << metrics.cpuUsage << "%" << std::endl;
    std::cout << "  Network I/O: " << metrics.networkIO << "MB" << std::endl;
    std::cout << "  Average firing rate: " << metrics.averageFiringRate << "Hz" << std::endl;
    
    // Check performance thresholds
    auto status = profiler.checkPerformanceThresholds();
    std::cout << "  Performance status: " << status.toString() << std::endl;
    
    return 0;
}
```

### 2. Memory Profiling
```cpp
#include "performance/MemoryProfiler.hpp"

int main() {
    // Create memory profiler
    nlm::MemoryProfiler memoryProfiler;
    
    // Start memory profiling
    memoryProfiler.startMemoryProfiling();
    
    // Create brain
    auto config = nlm::create_default_config();
    config->set("neuron_count", 2000);
    auto brain = nlm::create_brain(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 3000; ++step) {
        brain->step(step);
    }
    
    // Stop memory profiling
    memoryProfiler.stopMemoryProfiling();
    
    // Get memory statistics
    auto stats = memoryProfiler.getMemoryStatistics();
    
    std::cout << "Memory Report:" << std::endl;
    std::cout << "  Peak memory usage: " << stats.peakMemoryUsage << "MB" << std::endl;
    std::cout << "  Average memory usage: " << stats.averageMemoryUsage << "MB" << std::endl;
    std::cout << "  Memory allocated: " << stats.totalMemoryAllocated << "MB" << std::endl;
    std::cout << "  Memory freed: " << stats.totalMemoryFreed << "MB" << std::endl;
    
    // Detect memory leaks
    auto leakReport = memoryProfiler.detectMemoryLeaks();
    std::cout << "  Memory leaks detected: " << leakReport.leakCount << std::endl;
    
    // Analyze memory fragmentation
    auto fragReport = memoryProfiler.analyzeFragmentation();
    std::cout << "  Memory fragmentation: " << fragReport.fragmentationScore << std::endl;
    
    return 0;
}
```

### 3. CPU Performance Profiling
```cpp
#include "performance/CPUProfiler.hpp"

int main() {
    // Create CPU profiler
    nlm::CPUProfiler cpuProfiler;
    
    // Start CPU profiling
    cpuProfiler.startCPUProfiling();
    
    // Create brain
    auto config = nlm::create_default_config();
    config->set("neuron_count", 1500);
    auto brain = nlm::create_brain(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 4000; ++step) {
        brain->step(step);
    }
    
    // Stop CPU profiling
    cpuProfiler.stopCPUProfiling();
    
    // Get CPU metrics
    auto metrics = cpuProfiler.getCPUMetrics();
    
    std::cout << "CPU Performance Report:" << std::endl;
    std::cout << "  CPU usage: " << metrics.cpuUsage << "%" << std::endl;
    std::cout << "  User CPU time: " << metrics.userCPUTime << "s" << std::endl;
    std::cout << "  System CPU time: " << metrics.systemCPUTime << "s" << std::endl;
    std::cout << "  I/O wait time: " << metrics.iowaitTime << "s" << std::endl;
    
    // Get thread metrics
    auto threadMetrics = cpuProfiler.getThreadMetrics();
    std::cout << "  Number of threads: " << threadMetrics.size() << std::endl;
    
    // Identify CPU bottlenecks
    auto bottleneckReport = cpuProfiler.identifyCPUBottlenecks();
    std::cout << "  CPU bottlenecks: " << bottleneckReport.bottleneckCount << std::endl;
    
    // Analyze cache performance
    auto cacheReport = cpuProfiler.analyzeCachePerformance();
    std::cout << "  Cache efficiency: " << cacheReport.cacheEfficiency << "%" << std::endl;
    
    return 0;
}
```

### 4. Resource Monitoring
```cpp
#include "performance/ResourceMonitor.hpp"

int main() {
    // Create resource monitor
    nlm::ResourceMonitor resourceMonitor;
    
    // Start resource monitoring
    resourceMonitor.startResourceMonitoring();
    
    // Create brain
    auto config = nlm::create_default_config();
    config->set("neuron_count", 1000);
    auto brain = nlm::create_brain(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 3000; ++step) {
        brain->step(step);
    }
    
    // Stop resource monitoring
    resourceMonitor.stopResourceMonitoring();
    
    // Get system resources
    auto systemResources = resourceMonitor.getSystemResources();
    
    std::cout << "Resource Report:" << std::endl;
    std::cout << "  Total memory: " << systemResources.totalMemory << "MB" << std::endl;
    std::cout << "  Available memory: " << systemResources.availableMemory << "MB" << std::endl;
    std::cout << "  Memory usage: " << systemResources.memoryUsage << "MB" << std::endl;
    std::cout << "  CPU usage: " << systemResources.cpuUsage << "%" << std::endl;
    std::cout << "  Disk usage: " << systemResources.diskUsage << "%" << std::endl;
    
    // Check resource thresholds
    auto resourceStatus = resourceMonitor.checkResourceThresholds();
    std::cout << "  Resource status: " << resourceStatus.toString() << std::endl;
    
    return 0;
}
```

### 5. Benchmarking Suite
```cpp
#include "performance/BenchmarkingSuite.hpp"

int main() {
    // Create benchmarking suite
    nlm::BenchmarkingSuite benchmarkSuite;
    
    // Configure benchmarking parameters
    nlm::BenchmarkingParameters params;
    params.enableBenchmarks = true;
    params.benchmarkDuration = 300.0;
    params.benchmarkSteps = 1000;
    params.warmupSteps = 100;
    params.repeats = 3;
    
    benchmarkSuite.setBenchmarkingParameters(params);
    
    // Create brain
    auto config = nlm::create_default_config();
    config->set("neuron_count", 1000);
    auto brain = nlm::create_brain(config);
    brain->initialize();
    
    // Run performance benchmark
    auto performanceResults = benchmarkSuite.runPerformanceBenchmark(
        PerformanceConfig{brain, 1000, 100, 3});
    
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "  Throughput: " << performanceResults.throughput << " steps/sec" << std::endl;
    std::cout << "  Latency: " << performanceResults.latency << "s" << std::endl;
    std::cout << "  Efficiency: " << performanceResults.efficiency << "%" << std::endl;
    std::cout << "  Accuracy: " << performanceResults.accuracy << "%" << std::endl;
    
    // Export benchmark results
    benchmarkSuite.exportBenchmarkResults("benchmark_results.json");
    
    // Generate benchmark report
    std::string report = benchmarkSuite.generateBenchmarkReport();
    std::cout << "\nBenchmark Report:" << std::endl;
    std::cout << report;
    
    return 0;
}
```

### 6. Performance Analysis
```cpp
#include "performance/PerformanceAnalyzer.hpp"

int main() {
    // Create performance analyzer
    nlm::PerformanceAnalyzer analyzer;
    
    // Configure analysis parameters
    nlm::PerformanceAnalysisParameters params;
    params.analyzeTrends = true;
    params.identifyBottlenecks = true;
    params.generateRecommendations = true;
    params.predictPerformance = true;
    
    analyzer.setPerformanceAnalysisParameters(params);
    
    // Create brain
    auto config = nlm::create_default_config();
    config->set("neuron_count", 500);
    auto brain = nlm::create_brain(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 2000; ++step) {
        brain->step(step);
    }
    
    // Analyze performance trends
    auto performanceTrends = analyzer.analyzePerformanceTrends();
    
    std::cout << "Performance Trends:" << std::endl;
    std::cout << "  Firing rate trend: " << performanceTrends.firingRateTrend << "%" << std::endl;
    std::cout << "  Memory usage trend: " << performanceTrends.memoryUsageTrend << "%" << std::endl;
    std::cout << "  CPU usage trend: " << performanceTrends.cpuUsageTrend << "%" << std::endl;
    
    // Identify performance bottlenecks
    auto bottleneckAnalysis = analyzer.identifyPerformanceBottlenecks(brain);
    std::cout << "\nPerformance Bottlenecks:" << std::endl;
    std::cout << "  Memory bottlenecks: " << bottleneckAnalysis.memoryBottlenecks << std::endl;
    std::cout << "  CPU bottlenecks: " << bottleneckAnalysis.cpuBottlenecks << std::endl;
    std::cout << "  Network bottlenecks: " << bottleneckAnalysis.networkBottlenecks << std::endl;
    
    // Generate optimization recommendations
    auto optimizationRecommendations = analyzer.generateOptimizationRecommendations(brain);
    std::cout << "\nOptimization Recommendations:" << std::endl;
    for (const auto& recommendation : optimizationRecommendations.recommendations) {
        std::cout << "  - " << recommendation << std::endl;
    }
    
    return 0;
}
```

## Files Created

### Configuration Files
- `configs/performance_profiler.cfg` - Performance profiler configuration
- `configs/benchmarking_suite.cfg` - Benchmarking suite configuration

### Performance Profiling
- `src/performance/PerformanceProfiler.cpp` - Core performance monitoring
- `src/performance/PerformanceProfiler.hpp` - Performance profiler interface
- `src/performance/MemoryProfiler.cpp` - Memory profiling
- `src/performance/MemoryProfiler.hpp` - Memory profiler interface
- `src/performance/CPUProfiler.cpp` - CPU profiling
- `src/performance/CPUProfiler.hpp` - CPU profiler interface
- `src/performance/SpikePerformanceAnalyzer.cpp` - Spike performance analysis
- `src/performance/SpikePerformanceAnalyzer.hpp` - Spike performance analyzer interface
- `src/performance/NetworkPerformanceAnalyzer.cpp` - Network performance analysis
- `src/performance/NetworkPerformanceAnalyzer.hpp` - Network performance analyzer interface

### Benchmarking Suite
- `src/performance/BenchmarkingSuite.cpp` - Benchmarking suite
- `src/performance/BenchmarkingSuite.hpp` - Benchmarking suite interface
- `src/performance/ScalabilityBenchmarker.cpp` - Scalability benchmarking
- `src/performance/ScalabilityBenchmarker.hpp` - Scalability benchmarker interface
- `src/performance/ConfigurationBenchmarker.cpp` - Configuration benchmarking
- `src/performance/ConfigurationBenchmarker.hpp` - Configuration benchmarker interface
- `src/performance/PerformanceBenchmarker.cpp` - Performance benchmarking
- `src/performance/PerformanceBenchmarker.hpp` - Performance benchmarker interface

### Performance Analysis
- `src/performance/PerformanceAnalyzer.cpp` - Performance analysis
- `src/performance/PerformanceAnalyzer.hpp` - Performance analyzer interface
- `src/performance/BottleneckAnalyzer.cpp` - Bottleneck analysis
- `src/performance/BottleneckAnalyzer.hpp` - Bottleneck analyzer interface
- `src/performance/OptimizationRecommender.cpp` - Optimization recommendations
- `src/performance/OptimizationRecommender.hpp` - Optimization recommender interface

### Resource Monitoring
- `src/performance/ResourceMonitor.cpp` - Resource monitoring
- `src/performance/ResourceMonitor.hpp` - Resource monitor interface
- `src/performance/MemoryMonitor.cpp` - Memory monitoring
- `src/performance/MemoryMonitor.hpp` - Memory monitor interface
- `src/performance/CPUMonitor.cpp` - CPU monitoring
- `src/performance/CPUMonitor.hpp` - CPU monitor interface
- `src/performance/NetworkMonitor.cpp` - Network monitoring
- `src/performance/NetworkMonitor.hpp` - Network monitor interface
- `src/performance/DiskMonitor.cpp` - Disk monitoring
- `src/performance/DiskMonitor.hpp` - Disk monitor interface

### Performance Reports
- `src/performance/PerformanceReporter.cpp` - Performance report generation
- `src/performance/PerformanceReporter.hpp` - Performance reporter interface
- `src/performance/MetricsVisualizer.cpp` - Metrics visualization
- `src/performance/MetricsVisualizer.hpp` - Metrics visualizer interface

## Build Instructions

### Building with Performance Profiling
```bash
# Configure CMake with performance profiling support
cmake .. -DNLM_ENABLE_PERFORMANCE_PROFILING=ON \
              -DNLM_ENABLE_BENCHMARKING=ON \
              -DNLM_ENABLE_PERFORMANCE_ANALYSIS=ON \
              -DNLM_ENABLE_RESOURCE_MONITORING=ON \
              -DNLM_ENABLE_PERFORMANCE_REPORTING=ON \
              -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Build specific performance targets
make performance_profiler
make benchmarking_suite
make performance_analyzer
make resource_monitor
make performance_reporter
```

### Running Performance Profiling
```bash
# Run performance profiling with detailed monitoring
./nlm --performance-profiling --perf-monitor --perf-sampling 100 --steps 5000

# Run comprehensive benchmarking
./nlm --benchmark --detailed --repeats 3 --output benchmark_results.json

# Run scalability analysis
./nlm --scalability-analysis --neuron-counts "100 500 1000 5000" --output scalability_results.json

# Run performance analysis
./nlm --performance-analysis --analyze-trends --identify-bottlenecks

# Run resource monitoring
./nlm --resource-monitoring --monitor-memory --monitor-cpu --monitor-network
```

## Usage Examples

### 1. Complete Performance Profiling Pipeline
```bash
#!/bin/bash

# Run performance profiling with comprehensive monitoring
./nlm --performance-profiling --perf-monitor --perf-sampling 100 \
     --benchmark --detailed --repeats 3 \
     --scalability-analysis --neuron-counts "100 500 1000 5000" \
     --output performance_results.json

# Generate performance report
./nlm --performance-reporting --generate-html-report --generate-json-report --generate-csv-report
```

### 2. Python Integration
```python
import pynlm
import time
import psutil
import numpy as np

# Create brain and run simulation
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Start performance monitoring
process = psutil.Process()
start_time = time.time()
start_memory = process.memory_info().rss / (1024 * 1024)  # MB

# Run simulation
for step in range(3000):
    brain.step(step)
    
    # Collect performance metrics
    current_memory = process.memory_info().rss / (1024 * 1024)
    cpu_percent = process.cpu_percent()

# Stop performance monitoring
end_time = time.time()
end_memory = process.memory_info().rss / (1024 * 1024)

# Calculate performance metrics
duration = end_time - start_time
memory_used = end_memory - start_memory
firing_rate = brain.get_average_firing_rate()
total_spikes = brain.get_total_spike_count()

print(f"Performance Results:")
print(f"  Duration: {duration:.2f}s")
print(f"  Steps per second: {3000/duration:.0f}")
print(f"  Memory usage: {memory_used:.2f} MB")
print(f"  CPU usage: {cpu_percent:.1f}%")
print(f"  Firing rate: {firing_rate:.2f} Hz")
print(f"  Total spikes: {total_spikes}")
```

### 3. Performance Comparison
```bash
#!/bin/bash

# Run performance comparison across different configurations
./nlm --performance-profiling --benchmark \
     --config "config_small.cfg" --output small_results.json \
     --config "config_medium.cfg" --output medium_results.json \
     --config "config_large.cfg" --output large_results.json \
     --compare-results --generate-comparison-chart

# Generate performance comparison report
./nlm --performance-reporting --generate-comparison-report \
     --input small_results.json medium_results.json large_results.json \
     --output performance_comparison.html
```

## Conclusion

The performance profiling and benchmarking tools provide a comprehensive framework for monitoring, analyzing, and optimizing NLM performance. These tools enable researchers and developers to:

1. **Monitor** system performance in real-time
2. **Benchmark** performance across different configurations
3. **Analyze** performance trends and bottlenecks
4. **Optimize** performance through detailed analysis
5. **Report** performance results with comprehensive visualization

The tools are designed to work seamlessly with the Phase 6 integration and provide comprehensive support for performance analysis and optimization.

## Files Created

This document corresponds to the following files:

1. `configs/performance_profiler.cfg` - Performance profiler configuration
2. `configs/benchmarking_suite.cfg` - Benchmarking suite configuration
3. `src/performance/PerformanceProfiler.cpp` - Core performance monitoring
4. `src/performance/PerformanceProfiler.hpp` - Performance profiler interface
5. `src/performance/MemoryProfiler.cpp` - Memory profiling
6. `src/performance/MemoryProfiler.hpp` - Memory profiler interface
7. `src/performance/CPUProfiler.cpp` - CPU profiling
8. `src/performance/CPUProfiler.hpp` - CPU profiler interface
9. `src/performance/SpikePerformanceAnalyzer.cpp` - Spike performance analysis
10. `src/performance/SpikePerformanceAnalyzer.hpp` - Spike performance analyzer interface

[Continue with remaining performance profiling files...]