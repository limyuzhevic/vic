# Performance Monitoring Component Headers

## Performance Monitor Header

```cpp
// PerformanceMonitor.hpp - Core performance monitoring class
#ifndef NLM_PERFORMANCE_MONITOR_HPP
#define NLM_PERFORMANCE_MONITOR_HPP

#include <chrono>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

// Performance metrics structure
struct PerformanceMetrics {
    // Timing metrics
    double simulationTime;      // Total simulation time
    double cpuTime;             // CPU time used
    double wallClockTime;       // Wall clock time
    double avgStepTime;         // Average time per simulation step
    double maxStepTime;         // Maximum time per simulation step
    double minStepTime;         // Minimum time per simulation step
    
    // Activity metrics
    size_t totalSpikes;         // Total spikes generated
    size_t totalPlasticityUpdates; // Total plasticity updates
    size_t totalSynapseUpdates;     // Total synapse weight updates
    size_t totalMemoryAccesses;    // Total memory accesses
    size_t neuronsActive;       // Number of active neurons
    size_t synapsesActive;      // Number of active synapses
    
    // Performance metrics
    double spikeRate;           // Spikes per second
    double plasticityRate;      // Plasticity updates per second
    double synapseUpdateRate;   // Synapse updates per second
    double memoryAccessRate;    // Memory accesses per second
    double computationEfficiency; // CPU efficiency ratio
    double simulationEfficiency;   // Wall clock efficiency
    
    // Resource usage
    size_t memoryUsage;         // Current memory usage in MB
    size_t peakMemoryUsage;     // Peak memory usage in MB
    double cpuUsage;            // CPU usage percentage
    double gpuUsage;            // GPU usage percentage (if available)
    
    PerformanceMetrics()
        : simulationTime(0.0), cpuTime(0.0), wallClockTime(0.0),
          avgStepTime(0.0), maxStepTime(0.0), minStepTime(0.0),
          totalSpikes(0), totalPlasticityUpdates(0), totalSynapseUpdates(0),
          totalMemoryAccesses(0), neuronsActive(0), synapsesActive(0),
          spikeRate(0.0), plasticityRate(0.0), synapseUpdateRate(0.0),
          memoryAccessRate(0.0), computationEfficiency(0.0),
          simulationEfficiency(0.0), memoryUsage(0), peakMemoryUsage(0),
          cpuUsage(0.0), gpuUsage(0.0) {}
};

// Performance monitor configuration
struct PerformanceMonitorConfig {
    double monitoringInterval;      // Monitoring interval in seconds
    bool metricsCollection;          // Enable metrics collection
    bool bottleneckDetection;        // Enable bottleneck detection
    std::string exportFormat;        // Export format (JSON, CSV, etc.)
    bool realTimeReporting;          // Enable real-time reporting
    double cpuThreshold;             // CPU usage threshold for alerts
    double memoryThreshold;          // Memory usage threshold for alerts
    double ioThreshold;              // I/O usage threshold for alerts
    double networkThreshold;         // Network usage threshold for alerts
    
    PerformanceMonitorConfig()
        : monitoringInterval(0.001), metricsCollection(true),
          bottleneckDetection(true), exportFormat("JSON"),
          realTimeReporting(true), cpuThreshold(0.8), memoryThreshold(0.9),
          ioThreshold(0.7), networkThreshold(0.6) {}
};

// Performance monitor class
class PerformanceMonitor {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    PerformanceMonitor();
    explicit PerformanceMonitor(const PerformanceMonitorConfig& config);
    ~PerformanceMonitor();
    
    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;
    
    // Configuration
    void applyConfig(const PerformanceMonitorConfig& config);
    PerformanceMonitorConfig getConfig() const;
    
    // Start/stop monitoring
    void start();
    void stop();
    bool isRunning() const;
    
    // Record various events
    void recordStepStart();
    void recordStepEnd();
    void recordNeuronSpike(size_t neuronId, double spikeTime);
    void recordSynapseUpdate(SynapseId synapseId, double weightChange);
    void recordPlasticityUpdate(PlasticityType type, double strength);
    void recordMemoryAccess(size_t accessSize, bool cacheHit);
    void recordInputInjection(NeuronType type, double current);
    void recordActionProduction(ActionType action, double confidence);
    
    // Performance metrics
    PerformanceMetrics getMetrics() const;
    double getSimulationTime() const;
    double getCPUTime() const;
    double getWallClockTime() const;
    size_t getTotalSpikes() const;
    size_t getTotalPlasticityUpdates() const;
    size_t getTotalSynapseUpdates() const;
    size_t getTotalMemoryAccesses() const;
    
    // Profiling data
    struct ProfilingData {
        std::vector<double> stepTimes;           // Time per simulation step
        std::vector<size_t> spikeCounts;         // Spike counts per step
        std::vector<size_t> plasticityUpdates;   // Plasticity updates per step
        std::vector<size_t> synapseUpdates;      // Synapse updates per step
        std::vector<double> memoryUsage;        // Memory usage per step
        std::vector<double> cpuUsage;           // CPU usage per step
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        
        ProfilingData() : startTime(std::chrono::high_resolution_clock::now()) {}
    };
    
    ProfilingData getProfilingData() const;
    
    // Performance analysis
    double calculateComputationEfficiency() const;
    double calculateSimulationEfficiency() const;
    double calculateMemoryEfficiency() const;
    double calculateNeuralEfficiency(size_t neurons, size_t spikes) const;
    
    // Bottleneck detection
    std::vector<std::string> detectBottlenecks() const;
    bool hasPerformanceIssues() const;
    std::string getPrimaryBottleneck() const;
    
    // Export and import
    bool exportToFile(const std::string& filepath) const;
    bool importFromFile(const std::string& filepath);
    
    // Reset monitoring
    void reset();
    
    // Configuration setters
    void setMonitoringInterval(double interval);
    void setMetricsCollection(bool collect);
    void setBottleneckDetection(bool detect);
    void setExportFormat(const std::string& format);
    void setRealTimeReporting(bool report);
    void setCPThreshold(double threshold);
    void setMemoryThreshold(double threshold);
    void setIOThreshold(double threshold);
    void setNetworkThreshold(double threshold);
    
    // Configuration getters
    double getMonitoringInterval() const;
    bool getMetricsCollection() const;
    bool getBottleneckDetection() const;
    std::string getExportFormat() const;
    bool getRealTimeReporting() const;
    double getCPThreshold() const;
    double getMemoryThreshold() const;
    double getIOThreshold() const;
    double getNetworkThreshold() const;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITOR_HPP
```

## Bottleneck Detector Header

```cpp
// BottleneckDetector.hpp - Advanced bottleneck detection class
#ifndef NLM_BOTTLENECK_DETECTOR_HPP
#define NLM_BOTTLENECK_DETECTOR_HPP

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <functional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class PerformanceMonitor;

// Bottleneck analysis result
struct BottleneckAnalysis {
    enum class Type {
        CPUBound,
        MemoryBound,
        IOBound,
        NetworkBound,
        AlgorithmicComplex,
        Synchronization,
        ResourceContention,
        CacheMisses,
        BranchMisprediction,
        ThermalThrottling
    };
    
    Type type;
    std::string description;
    double severity;  // 0.0 to 1.0
    std::string impact; // "low", "medium", "high", "critical"
    std::vector<std::string> affectedComponents;
    std::vector<std::string> mitigationStrategies;
    double estimatedTimeSavings;
    std::chrono::system_clock::time_point detectedTime;
    
    BottleneckAnalysis() : type(Type::CPUBound), severity(0.0), impact("low"),
                         estimatedTimeSavings(0.0) {}
};

// Bottleneck detector configuration
struct BottleneckDetectorConfig {
    double sensitivity;              // Sensitivity for bottleneck detection (0.0 to 1.0)
    size_t timeWindow;               // Time window for analysis (number of steps)
    double minSeverity;              // Minimum severity for reporting
    bool enableRealTimeDetection;    // Enable real-time bottleneck detection
    std::chrono::milliseconds realTimeInterval; // Real-time detection interval
    bool exportAnalysis;              // Export analysis to file
    std::string analysisFilepath;    // Filepath for analysis export
    
    BottleneckDetectorConfig()
        : sensitivity(0.8), timeWindow(100), minSeverity(0.3),
          enableRealTimeDetection(true), realTimeInterval(std::chrono::milliseconds(100)),
          exportAnalysis(true), analysisFilepath("bottleneck_analysis.json") {}
};

// Bottleneck detector class
class BottleneckDetector {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    BottleneckDetector();
    explicit BottleneckDetector(const BottleneckDetectorConfig& config);
    ~BottleneckDetector();
    
    // Disable copying, enable moving
    BottleneckDetector(const BottleneckDetector&) = delete;
    BottleneckDetector& operator=(const BottleneckDetector&) = delete;
    BottleneckDetector(BottleneckDetector&&) noexcept;
    BottleneckDetector& operator=(BottleneckDetector&&) noexcept;
    
    // Configuration
    void applyConfig(const BottleneckDetectorConfig& config);
    BottleneckDetectorConfig getConfig() const;
    
    // Analysis methods
    std::vector<BottleneckAnalysis> analyzePerformance(const PerformanceMonitor::Metrics& metrics);
    std::vector<BottleneckAnalysis> analyzeHistoricalData(const std::vector<PerformanceMonitor::Metrics>& historicalData);
    
    // Real-time detection
    void startRealTimeDetection(std::chrono::milliseconds interval = std::chrono::milliseconds(100));
    void stopRealTimeDetection();
    bool isRealTimeDetectionActive() const;
    
    std::vector<BottleneckAnalysis> getCurrentBottlenecks() const;
    
    // Prediction
    bool predictFutureBottlenecks(const std::vector<PerformanceMonitor::Metrics>& recentData);
    std::vector<BottleneckAnalysis> getPredictedBottlenecks() const;
    
    // Mitigation
    std::vector<std::string> getMitigationRecommendations(const BottleneckAnalysis& bottleneck) const;
    bool optimizeConfiguration(std::shared_ptr<Config>& config) const;
    bool suggestParallelization(std::shared_ptr<Config>& config) const;
    bool suggestCaching(std::shared_ptr<Config>& config) const;
    
    // Export/import
    bool exportAnalysis(const std::string& filepath) const;
    bool importAnalysis(const std::string& filepath);
    
    // Configuration setters
    void setSensitivity(double sensitivity);
    void setTimeWindow(size_t timeWindow);
    void setMinSeverity(double minSeverity);
    void setRealTimeDetection(bool enable);
    void setRealTimeDetectionInterval(std::chrono::milliseconds interval);
    
    // Configuration getters
    double getSensitivity() const;
    size_t getTimeWindow() const;
    double getMinSeverity() const;
    bool isRealTimeDetectionActive() const;
    std::chrono::milliseconds getRealTimeDetectionInterval() const;
};

} // namespace nlm

#endif // NLM_BOTTLENECK_DETECTOR_HPP
```

## Resource Usage Analyzer Header

```cpp
// ResourceUsageAnalyzer.hpp - Resource utilization analysis class
#ifndef NLM_RESOURCE_USAGE_ANALYZER_HPP
#define NLM_RESOURCE_USAGE_ANALYZER_HPP

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Forward declarations
class PerformanceMonitor;

// Memory analysis result
struct MemoryAnalysis {
    size_t totalMemoryUsage;      // Total memory usage in bytes
    size_t peakMemoryUsage;       // Peak memory usage in bytes
    size_t memoryAllocationCount; // Number of memory allocations
    size_t memoryDeallocationCount; // Number of memory deallocations
    double memoryEfficiency;     // Memory efficiency ratio (0.0 to 1.0)
    std::vector<std::pair<std::string, size_t>> memoryBreakdown; // Component-wise breakdown
    std::vector<std::pair<size_t, double>> allocationPatterns;  // Size vs. frequency
    
    MemoryAnalysis() : totalMemoryUsage(0), peakMemoryUsage(0), memoryAllocationCount(0),
                     memoryDeallocationCount(0), memoryEfficiency(0.0) {}
};

// CPU analysis result
struct CPUAnalysis {
    double cpuUsage;                    // Overall CPU usage percentage
    double cpuTime;                     // CPU time used
    double userCPUTime;                 // User CPU time
    double systemCPUTime;               // System CPU time
    size_t cpuCycles;                   // CPU cycles
    size_t instructionsRetired;        // Instructions retired
    double ipc;                         // Instructions per cycle
    std::vector<std::pair<std::string, double>> cpuBreakdown; // Component-wise breakdown
    std::vector<std::pair<size_t, double>> threadUsage; // Thread usage patterns
    
    CPUAnalysis() : cpuUsage(0.0), cpuTime(0.0), userCPUTime(0.0), systemCPUTime(0.0),
                   cpuCycles(0), instructionsRetired(0), ipc(0.0) {}
};

// GPU analysis result
struct GPUAnalysis {
    bool available;                    // Whether GPU is available
    double gpuUsage;                   // GPU usage percentage
    double gpuMemoryUsage;             // GPU memory usage in MB
    double gpuMemoryTotal;             // Total GPU memory in MB
    size_t gpuComputeUnits;            // Number of compute units
    size_t gpuClockSpeed;              // GPU clock speed in MHz
    std::vector<std::pair<std::string, double>> kernelUsage; // Kernel execution times
    std::vector<std::pair<size_t, double>> memoryTransfers; // Memory transfer patterns
    
    GPUAnalysis() : available(false), gpuUsage(0.0), gpuMemoryUsage(0.0),
                   gpuMemoryTotal(0.0), gpuComputeUnits(0), gpuClockSpeed(0) {}
};

// I/O analysis result
struct IOAnalysis {
    double diskReadSpeed;              // Disk read speed in MB/s
    double diskWriteSpeed;             // Disk write speed in MB/s
    double networkUploadSpeed;         // Network upload speed in Mbps
    double networkDownloadSpeed;       // Network download speed in Mbps
    size_t diskReadOperations;         // Number of disk read operations
    size_t diskWriteOperations;        // Number of disk write operations
    size_t networkPacketsSent;         // Number of network packets sent
    size_t networkPacketsReceived;    // Number of network packets received
    double ioWaitTime;                 // I/O wait time
    double ioThroughput;              // I/O throughput
    std::vector<std::pair<std::string, double>> ioBreakdown; // Component-wise breakdown
    
    IOAnalysis() : diskReadSpeed(0.0), diskWriteSpeed(0.0), networkUploadSpeed(0.0),
                  networkDownloadSpeed(0.0), diskReadOperations(0), diskWriteOperations(0),
                  networkPacketsSent(0), networkPacketsReceived(0), ioWaitTime(0.0),
                  ioThroughput(0.0) {}
};

// Network analysis result
struct NetworkAnalysis {
    double networkUploadUsage;         // Network upload usage in Mbps
    double networkDownloadUsage;       // Network download usage in Mbps
    size_t networkPacketsSent;         // Number of network packets sent
    size_t networkPacketsReceived;    // Number of network packets received
    double packetLossRate;             // Packet loss rate (0.0 to 1.0)
    double networkLatency;            // Network latency in ms
    std::vector<std::pair<std::string, double>> protocolUsage; // Protocol-wise usage
    std::vector<std::pair<size_t, double>> packetSizeDistribution; // Packet size distribution
    
    NetworkAnalysis() : networkUploadUsage(0.0), networkDownloadUsage(0.0),
                      networkPacketsSent(0), networkPacketsReceived(0),
                      packetLossRate(0.0), networkLatency(0.0) {}
};

// Resource utilization result
struct ResourceUtilization {
    double cpuUtilization;             // Overall CPU utilization (0.0 to 1.0)
    double memoryUtilization;         // Memory utilization (0.0 to 1.0)
    double ioUtilization;             // I/O utilization (0.0 to 1.0)
    double networkUtilization;        // Network utilization (0.0 to 1.0)
    double gpuUtilization;             // GPU utilization (0.0 to 1.0)
    double energyConsumption;         // Energy consumption in watts
    double powerUsage;                 // Power usage in watts
    std::chrono::system_clock::time_point timestamp; // Collection timestamp
    
    ResourceUtilization() : cpuUtilization(0.0), memoryUtilization(0.0),
                          ioUtilization(0.0), networkUtilization(0.0),
                          gpuUtilization(0.0), energyConsumption(0.0), powerUsage(0.0) {}
};

// Resource usage analyzer configuration
struct ResourceUsageAnalyzerConfig {
    double cpuThreshold;             // CPU usage threshold for alerts
    double memoryThreshold;          // Memory usage threshold for alerts
    double ioThreshold;              // I/O usage threshold for alerts
    double networkThreshold;         // Network usage threshold for alerts
    bool enableRealTimeMonitoring;   // Enable real-time resource monitoring
    std::chrono::milliseconds monitoringInterval; // Resource monitoring interval
    bool exportAnalysis;              // Export analysis to file
    std::string analysisFilepath;    // Filepath for analysis export
    
    ResourceUsageAnalyzerConfig()
        : cpuThreshold(0.8), memoryThreshold(0.9), ioThreshold(0.7), networkThreshold(0.6),
          enableRealTimeMonitoring(true), monitoringInterval(std::chrono::milliseconds(1000)),
          exportAnalysis(true), analysisFilepath("resource_analysis.json") {}
};

// Resource usage analyzer class
class ResourceUsageAnalyzer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ResourceUsageAnalyzer();
    explicit ResourceUsageAnalyzer(const ResourceUsageAnalyzerConfig& config);
    ~ResourceUsageAnalyzer();
    
    // Disable copying, enable moving
    ResourceUsageAnalyzer(const ResourceUsageAnalyzer&) = delete;
    ResourceUsageAnalyzer& operator=(const ResourceUsageAnalyzer&) = delete;
    ResourceUsageAnalyzer(ResourceUsageAnalyzer&&) noexcept;
    ResourceUsageAnalyzer& operator=(ResourceUsageAnalyzer&&) noexcept;
    
    // Configuration
    void applyConfig(const ResourceUsageAnalyzerConfig& config);
    ResourceUsageAnalyzerConfig getConfig() const;
    
    // Analysis methods
    MemoryAnalysis analyzeMemoryUsage() const;
    CPUAnalysis analyzeCPUUsage() const;
    GPUAnalysis analyzeGPUUsage() const;
    IOAnalysis analyzeIOUsage() const;
    NetworkAnalysis analyzeNetworkUsage() const;
    ResourceUtilization analyzeResourceUtilization() const;
    
    // Performance score calculation
    double calculatePerformanceScore(const PerformanceMonitor::Metrics& metrics,
                                    const MemoryAnalysis& memory,
                                    const CPUAnalysis& cpu,
                                    const GPUAnalysis& gpu,
                                    const IOAnalysis& io,
                                    const NetworkAnalysis& network) const;
    
    // Optimization
    bool optimizeForCPU(const std::shared_ptr<Config>& config) const;
    bool optimizeForMemory(const std::shared_ptr<Config>& config) const;
    bool optimizeForGPU(const std::shared_ptr<Config>& config) const;
    bool optimizeForNetwork(const std::shared_ptr<Config>& config) const;
    
    // Export/import
    bool exportAnalysis(const std::string& filepath) const;
    bool importAnalysis(const std::string& filepath);
    
    // Real-time monitoring
    void startResourceMonitoring(std::chrono::milliseconds interval = std::chrono::milliseconds(1000));
    void stopResourceMonitoring();
    bool isResourceMonitoringActive() const;
    
    std::vector<ResourceUtilization> getResourceUtilizationHistory() const;
    
    // Alert configuration
    void setResourceThresholds(double cpuThreshold, double memoryThreshold,
                              double ioThreshold, double networkThreshold);
    void setAlertCallback(std::function<void(ResourceUtilization)>> callback);
    
    double getCPUThreshold() const;
    double getMemoryThreshold() const;
    double getIOThreshold() const;
    double getNetworkThreshold() const;
};

} // namespace nlm

#endif // NLM_RESOURCE_USAGE_ANALYZER_HPP
```

## Benchmark Suite Header

```cpp
// BenchmarkSuite.hpp - Comprehensive benchmarking suite
#ifndef NLM_BENCHMARK_SUITE_HPP
#define NLM_BENCHMARK_SUITE_HPP

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;
class PerformanceMonitor;

// Benchmark scenarios
enum class BenchmarkScenario {
    SimpleConnectivity,
    PlasticityLearning,
    STDPVerification,
    PerformanceTest,
    MemoryUsage,
    Scalability,
    MultiModalProcessing,
    HierarchicalProcessing,
    AttentionMechanisms,
    Custom
};

// Benchmark result
struct BenchmarkResult {
    BenchmarkScenario scenario;
    std::string description;
    double executionTime;
    size_t neuronsUsed;
    size_t synapsesUsed;
    size_t spikesGenerated;
    double spikeRate;
    double plasticityEfficiency;
    double memoryEfficiency;
    std::vector<std::string> bottlenecks;
    double successRate;
    std::string notes;
    std::chrono::system_clock::time_point timestamp;
    
    BenchmarkResult()
        : executionTime(0.0), neuronsUsed(0), synapsesUsed(0),
          spikesGenerated(0), spikeRate(0.0), plasticityEfficiency(0.0),
          memoryEfficiency(0.0), successRate(0.0) {}
};

// Benchmark configuration
struct BenchmarkSuiteConfig {
    std::vector<BenchmarkScenario> scenarios;           // Benchmark scenarios to run
    std::shared_ptr<Config> baselineConfig;             // Baseline configuration
    std::shared_ptr<Config> performanceConfig;          // Performance configuration
    std::shared_ptr<Config> learningConfig;             // Learning configuration
    std::shared_ptr<Config> scalabilityConfig;          // Scalability configuration
    double performanceRegressionTolerance;             // Tolerance for performance regression detection
    bool enablePerformanceRegressionTesting;           // Enable performance regression testing
    bool exportResults;                                  // Export results to file
    std::string resultsFilepath;                        // Filepath for results export
    
    BenchmarkSuiteConfig()
        : performanceRegressionTolerance(0.1),
          enablePerformanceRegressionTesting(true),
          exportResults(true), resultsFilepath("benchmark_results.json") {}
};

// Benchmark suite class
class BenchmarkSuite {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    BenchmarkSuite();
    explicit BenchmarkSuite(const BenchmarkSuiteConfig& config);
    ~BenchmarkSuite();
    
    // Disable copying, enable moving
    BenchmarkSuite(const BenchmarkSuite&) = delete;
    BenchmarkSuite& operator=(const BenchmarkSuite&) = delete;
    BenchmarkSuite(BenchmarkSuite&&) noexcept;
    BenchmarkSuite& operator=(BenchmarkSuite&&) noexcept;
    
    // Configuration
    void applyConfig(const BenchmarkSuiteConfig& config);
    BenchmarkSuiteConfig getConfig() const;
    
    // Benchmark methods
    BenchmarkResult runScenario(BenchmarkScenario scenario,
                               const std::shared_ptr<Config>& config = nullptr,
                               const std::map<std::string, std::string>& parameters = {});
    
    std::vector<BenchmarkResult> runBenchmarkSuite(const std::vector<BenchmarkScenario>& scenarios,
                                                  const std::shared_ptr<Config>& config = nullptr);
    
    // Configuration comparison
    struct ComparisonResult {
        std::string configName;
        std::vector<BenchmarkResult> results;
        double totalTime;
        double averageSpikeRate;
        double averagePlasticityEfficiency;
        
        ComparisonResult() : totalTime(0.0), averageSpikeRate(0.0), averagePlasticityEfficiency(0.0) {}
    };
    
    ComparisonResult compareConfigurations(const std::vector<std::pair<std::string, std::shared_ptr<Config>>>& configs);
    
    // Performance regression testing
    bool checkPerformanceRegression(const std::string& baselineConfigName,
                                   const std::shared_ptr<Config>& currentConfig,
                                   double tolerance = 0.1); // 10% tolerance
    
    // Benchmark statistics
    struct BenchmarkStatistics {
        size_t totalScenarios;
        double averageExecutionTime;
        double medianExecutionTime;
        double standardDeviation;
        size_t minNeurons;
        size_t maxNeurons;
        double minSpikeRate;
        double maxSpikeRate;
        std::vector<std::string> commonBottlenecks;
        
        BenchmarkStatistics() : totalScenarios(0), averageExecutionTime(0.0),
                              medianExecutionTime(0.0), standardDeviation(0.0),
                              minNeurons(0), maxNeurons(0), minSpikeRate(0.0),
                              maxSpikeRate(0.0) {}
    };
    
    BenchmarkStatistics getStatistics() const;
    
    // Export/import
    bool exportResults(const std::string& filepath) const;
    bool importResults(const std::string& filepath);
    
    // Configuration presets
    static std::shared_ptr<Config> createSimpleConfig(size_t neuronCount = 100);
    static std::shared_ptr<Config> createPerformanceConfig(size_t neuronCount = 1000);
    static std::shared_ptr<Config> createLearningConfig(size_t neuronCount = 500);
    static std::shared_ptr<Config> createScalabilityConfig(size_t neuronCount = 10000);
    
    // Scenario descriptions
    static std::string getScenarioDescription(BenchmarkScenario scenario);
    static std::vector<BenchmarkScenario> getDefaultScenarioOrder();
};

} // namespace nlm

#endif // NLM_BENCHMARK_SUITE_HPP
```

## Performance Monitoring Implementation Headers

```cpp
// PerformanceMonitorImpl.hpp - Performance monitor implementation details
#ifndef NLM_PERFORMANCE_MONITOR_IMPL_HPP
#define NLM_PERFORMANCE_MONITOR_IMPL_HPP

#include "PerformanceMonitor.hpp"
#include <chrono>
#include <unordered_map>

namespace nlm {

struct PerformanceMonitor::Impl {
    PerformanceMonitorConfig config;
    bool isRunningFlag;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    // Performance data
    struct StepData {
        std::chrono::high_resolution_clock::time_point stepStart;
        std::chrono::high_resolution_clock::time_point stepEnd;
        size_t spikeCount;
        size_t plasticityUpdates;
        size_t synapseUpdates;
        size_t memoryAccesses;
        size_t neuronsActive;
        size_t synapsesActive;
        double cpuUsage;
        double memoryUsage;
        double ioUsage;
        
        StepData() : stepCount(0), spikeCount(0), plasticityUpdates(0),
                   synapseUpdates(0), memoryAccesses(0), neuronsActive(0),
                   synapsesActive(0), cpuUsage(0.0), memoryUsage(0.0),
                   ioUsage(0.0) {}
    };
    
    std::vector<StepData> stepData;
    PerformanceMetrics currentMetrics;
    
    // Event tracking
    std::vector<std::pair<size_t, double>> spikeEvents;        // neuronId, time
    std::vector<std::pair<SynapseId, double>> synapseEvents;   // synapseId, weightChange
    std::vector<std::pair<PlasticityType, double>> plasticityEvents; // type, strength
    std::vector<std::pair<size_t, bool>> memoryEvents;         // accessSize, cacheHit
    
    Impl() : isRunningFlag(false) {}
};

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITOR_IMPL_HPP
```

```cpp
// BottleneckDetectorImpl.hpp - Bottleneck detector implementation details
#ifndef NLM_BOTTLENECK_DETECTOR_IMPL_HPP
#define NLM_BOTTLENECK_DETECTOR_IMPL_HPP

#include "BottleneckDetector.hpp"
#include <chrono>
#include <algorithm>
#include <cmath>

namespace nlm {

struct BottleneckDetector::Impl {
    BottleneckDetectorConfig config;
    bool realTimeDetectionActive;
    std::chrono::milliseconds realTimeInterval;
    std::chrono::steady_clock::time_point lastRealTimeCheck;
    
    // Historical data
    std::vector<PerformanceMonitor::Metrics> historicalData;
    std::vector<BottleneckAnalysis> currentBottlenecks;
    
    // Prediction data
    std::vector<PerformanceMonitor::Metrics> recentData;
    std::vector<BottleneckAnalysis> predictedBottlenecks;
    
    Impl() : realTimeDetectionActive(false) {}
};

} // namespace nlm

#endif // NLM_BOTTLENECK_DETECTOR_IMPL_HPP
```

```cpp
// ResourceUsageAnalyzerImpl.hpp - Resource usage analyzer implementation details
#ifndef NLM_RESOURCE_USAGE_ANALYZER_IMPL_HPP
#define NLM_RESOURCE_USAGE_ANALYZER_IMPL_HPP

#include "ResourceUsageAnalyzer.hpp"
#include <chrono>
#include <algorithm>

namespace nlm {

struct ResourceUsageAnalyzer::Impl {
    ResourceUsageAnalyzerConfig config;
    bool realTimeMonitoringActive;
    std::chrono::milliseconds monitoringInterval;
    std::chrono::steady_clock::time_point lastMonitoringCheck;
    
    // Resource utilization history
    std::vector<ResourceUtilization> resourceUtilizationHistory;
    
    // Alert callbacks
    std::function<void(ResourceUtilization)> alertCallback;
    
    // Thresholds
    double cpuThreshold;
    double memoryThreshold;
    double ioThreshold;
    double networkThreshold;
    
    Impl() : realTimeMonitoringActive(false) {}
};

} // namespace nlm

#endif // NLM_RESOURCE_USAGE_ANALYZER_IMPL_HPP
```

```cpp
// BenchmarkSuiteImpl.hpp - Benchmark suite implementation details
#ifndef NLM_BENCHMARK_SUITE_IMPL_HPP
#define NLM_BENCHMARK_SUITE_IMPL_HPP

#include "BenchmarkSuite.hpp"
#include <algorithm>
#include <numeric>
#include <map>

namespace nlm {

struct BenchmarkSuite::Impl {
    BenchmarkSuiteConfig config;
    
    // Scenario descriptions
    static std::unordered_map<BenchmarkScenario, std::string> scenarioDescriptions;
    static std::vector<BenchmarkScenario> defaultScenarioOrder;
    
    // Configuration presets
    static std::unordered_map<std::string, std::shared_ptr<Config>> configPresets;
    
    Impl() {}
    
    // Initialize static members
    static void initializeStaticMembers();
};

} // namespace nlm

#endif // NLM_BENCHMARK_SUITE_IMPL_HPP
```

## Performance Monitoring CMake Configuration

```cmake
# CMake configuration for performance monitoring components

# Performance monitoring library
add_library(nlm_performance_monitoring STATIC
    src/performance/PerformanceMonitor.cpp
    src/performance/BottleneckDetector.cpp
    src/performance/ResourceUsageAnalyzer.cpp
    src/performance/BenchmarkSuite.cpp
)

target_include_directories(nlm_performance_monitoring PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_performance_monitoring PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link with core libraries
target_link_libraries(nlm_performance_monitoring PUBLIC nlm_core nlm_agent nlm_world)

# Add dependencies
add_dependencies(nlm_performance_monitoring nlm_core nlm_agent nlm_world)

# Install targets
install(TARGETS nlm_performance_monitoring
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Python bindings for performance monitoring
pybind11_add_module(pynlm_performance_monitoring performance_python_bindings.cpp)
target_link_libraries(pynlm_performance_monitoring PRIVATE nlm_performance_monitoring nlm_core nlm_agent nlm_world)
target_include_directories(pynlm_performance_monitoring PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(pynlm_performance_monitoring PRIVATE /permissive-)
endif()

install(TARGETS pynlm_performance_monitoring
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
```

## Performance Monitoring Python Bindings

```cpp
// performance_python_bindings.cpp - Python bindings for performance monitoring
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "PerformanceMonitor.hpp"
#include "BottleneckDetector.hpp"
#include "ResourceUsageAnalyzer.hpp"
#include "BenchmarkSuite.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pynlm_performance_monitoring, m) {
    m.doc() = R"pbdoc(
        NLM Performance Monitoring Python Bindings
        ---------------------------------------------
        Python bindings for NLM performance monitoring and benchmarking tools.
        
        Provides classes for real-time performance monitoring, bottleneck detection,
        resource analysis, and comprehensive benchmarking of neural simulations.
    )pbdoc";

    // PerformanceMonitor bindings
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring class)"pbdoc")
        .def(py::init<>(), R"pbdoc(Default constructor)"pbdoc")
        .def(py::init<const PerformanceMonitorConfig&>(), py::arg("config"),
             R"pbdoc(Constructor with configuration)"pbdoc")
        .def("apply_config", &PerformanceMonitor::applyConfig,
             py::arg("config"), R"pbdoc(Apply configuration)"pbdoc")
        .def("get_config", &PerformanceMonitor::getConfig,
             R"pbdoc(Get current configuration)"pbdoc")
        .def("start", &PerformanceMonitor::start,
             R"pbdoc(Start monitoring)"pbdoc")
        .def("stop", &PerformanceMonitor::stop,
             R"pbdoc(Stop monitoring)"pbdoc>)
        .def("is_running", &PerformanceMonitor::isRunning,
             R"pbdoc(Check if monitoring is running)"pbdoc")
        .def("record_step_start", &PerformanceMonitor::recordStepStart,
             R"pbdoc(Record step start)"pbdoc")
        .def("record_step_end", &PerformanceMonitor::recordStepEnd,
             R"pbdoc(Record step end)"pbdoc>)
        .def("record_neuron_spike", &PerformanceMonitor::recordNeuronSpike,
             py::arg("neuron_id"), py::arg("spike_time"),
             R"pbdoc(Record neuron spike event)"pbdoc")
        .def("record_synapse_update", &PerformanceMonitor::recordSynapseUpdate,
             py::arg("synapse_id"), py::arg("weight_change"),
             R"pbdoc(Record synapse update)"pbdoc")
        .def("record_plasticity_update", &PerformanceMonitor::recordPlasticityUpdate,
             py::arg("type"), py::arg("strength"),
             R"pbdoc(Record plasticity update)"pbdoc")
        .def("record_memory_access", &PerformanceMonitor::recordMemoryAccess,
             py::arg("access_size"), py::arg("cache_hit"),
             R"pbdoc(Record memory access)"pbdoc")
        .def("record_input_injection", &PerformanceMonitor::recordInputInjection,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Record input injection)"pbdoc")
        .def("record_action_production", &PerformanceMonitor::recordActionProduction,
             py::arg("action"), py::arg("confidence"),
             R"pbdoc(Record action production)"pbdoc")
        .def("get_metrics", &PerformanceMonitor::getMetrics,
             R"pbdoc(Get performance metrics)"pbdoc")
        .def("get_profiling_data", &PerformanceMonitor::getProfilingData,
             R"pbdoc(Get profiling data)"pbdoc>)
        .def("export_to_file", &PerformanceMonitor::exportToFile,
             py::arg("filepath"), R"pbdoc(Export to file)"pbdoc")
        .def("import_from_file", &PerformanceMonitor::importFromFile,
             py::arg("filepath"), R"pbdoc(Import from file)"pbdoc>)
        .def("reset", &PerformanceMonitor::reset,
             R"pbdoc(Reset monitoring)"pbdoc>)
        .def("get_monitoring_interval", &PerformanceMonitor::getMonitoringInterval,
             R"pbdoc(Get monitoring interval)"pbdoc")
        .def("set_monitoring_interval", &PerformanceMonitor::setMonitoringInterval,
             py::arg("interval"), R"pbdoc(Set monitoring interval)"pbdoc")
        .def("get_metrics_collection", &PerformanceMonitor::getMetricsCollection,
             R"pbdoc(Get metrics collection setting)"pbdoc")
        .def("set_metrics_collection", &PerformanceMonitor::setMetricsCollection,
             py::arg("collect"), R"pbdoc(Set metrics collection)"pbdoc")
        .def("get_bottleneck_detection", &PerformanceMonitor::getBottleneckDetection,
             R"pbdoc(Get bottleneck detection setting)"pbdoc")
        .def("set_bottleneck_detection", &PerformanceMonitor::setBottleneckDetection,
             py::arg("detect"), R"pbdoc(Set bottleneck detection)"pbdoc")
        .def("get_export_format", &PerformanceMonitor::getExportFormat,
             R"pbdoc(Get export format)"pbdoc")
        .def("set_export_format", &PerformanceMonitor::setExportFormat,
             py::arg("format"), R"pbdoc(Set export format)"pbdoc")
        .def("get_real_time_reporting", &PerformanceMonitor::getRealTimeReporting,
             R"pbdoc(Get real-time reporting setting)"pbdoc")
        .def("set_real_time_reporting", &PerformanceMonitor::setRealTimeReporting,
             py::arg("report"), R"pbdoc(Set real-time reporting)"pbdoc>)
        .def("calculate_computation_efficiency", &PerformanceMonitor::calculateComputationEfficiency,
             R"pbdoc(Calculate computation efficiency)"pbdoc")
        .def("calculate_simulation_efficiency", &PerformanceMonitor::calculateSimulationEfficiency,
             R"pbdoc(Calculate simulation efficiency)"pbdoc>)
        .def("calculate_memory_efficiency", &PerformanceMonitor::calculateMemoryEfficiency,
             R"pbdoc(Calculate memory efficiency)"pbdoc>)
        .def("detect_bottlenecks", &PerformanceMonitor::detectBottlenecks,
             R"pbdoc(Detect bottlenecks)"pbdoc")
        .def("has_performance_issues", &PerformanceMonitor::hasPerformanceIssues,
             R"pbdoc(Check for performance issues)"pbdoc>)
        .def("get_primary_bottleneck", &PerformanceMonitor::getPrimaryBottleneck,
             R"pbdoc(Get primary bottleneck)"pbdoc");

    // BottleneckDetector bindings
    py::class_<BottleneckDetector>(m, "BottleneckDetector", R"pbdoc(Bottleneck detection class)"pbdoc")
        .def(py::init<>(), R"pbdoc(Default constructor)"pbdoc")
        .def(py::init<const BottleneckDetectorConfig&>(), py::arg("config"),
             R"pbdoc(Constructor with configuration)"pbdoc")
        .def("apply_config", &BottleneckDetector::applyConfig,
             py::arg("config"), R"pbdoc(Apply configuration)"pbdoc")
        .def("get_config", &BottleneckDetector::getConfig,
             R"pbdoc(Get current configuration)"pbdoc")
        .def("analyze_performance", &BottleneckDetector::analyzePerformance,
             py::arg("metrics"), R"pbdoc(Analyze performance for bottlenecks)"pbdoc")
        .def("analyze_historical_data", &BottleneckDetector::analyzeHistoricalData,
             py::arg("historical_data"), R"pbdoc(Analyze historical data)"pbdoc")
        .def("start_real_time_detection", &BottleneckDetector::startRealTimeDetection,
             py::arg("interval") = std::chrono::milliseconds(100),
             R"pbdoc(Start real-time detection)"pbdoc")
        .def("stop_real_time_detection", &BottleneckDetector::stopRealTimeDetection,
             R"pbdoc(Stop real-time detection)"pbdoc>)
        .def("is_real_time_detection_active", &BottleneckDetector::isRealTimeDetectionActive,
             R"pbdoc(Check if real-time detection is active)"pbdoc>)
        .def("get_current_bottlenecks", &BottleneckDetector::getCurrentBottlenecks,
             R"pbdoc(Get current bottlenecks)"pbdoc")
        .def("predict_future_bottlenecks", &BottleneckDetector::predictFutureBottlenecks,
             py::arg("recent_data"), R"pbdoc(Predict future bottlenecks)"pbdoc")
        .def("get_predicted_bottlenecks", &BottleneckDetector::getPredictedBottlenecks,
             R"pbdoc(Get predicted bottlenecks)"pbdoc")
        .def("get_mitigation_recommendations", &BottleneckDetector::getMitigationRecommendations,
             py::arg("bottleneck"), R"pbdoc(Get mitigation recommendations)"pbdoc")
        .def("optimize_configuration", &BottleneckDetector::optimizeConfiguration,
             py::arg("config"), R"pbdoc(Optimize configuration)"pbdoc")
        .def("suggest_parallelization", &BottleneckDetector::suggestParallelization,
             py::arg("config"), R"pbdoc(Suggest parallelization)"pbdoc")
        .def("suggest_caching", &BottleneckDetector::suggestCaching,
             py::arg("config"), R"pbdoc(Suggest caching)"pbdoc")
        .def("export_analysis", &BottleneckDetector::exportAnalysis,
             py::arg("filepath"), R"pbdoc(Export analysis to file)"pbdoc")
        .def("import_analysis", &BottleneckDetector::importAnalysis,
             py::arg("filepath"), R"pbdoc(Import analysis from file)"pbdoc>)
        .def("set_sensitivity", &BottleneckDetector::setSensitivity,
             py::arg("sensitivity"), R"pbdoc(Set sensitivity)"pbdoc")
        .def("set_time_window", &BottleneckDetector::setTimeWindow,
             py::arg("time_window"), R"pbdoc(Set time window)"pbdoc")
        .def("set_min_severity", &BottleneckDetector::setMinSeverity,
             py::arg("min_severity"), R"pbdoc(Set minimum severity)"pbdoc")
        .def("set_real_time_detection", &BottleneckDetector::setRealTimeDetection,
             py::arg("enable"), R"pbdoc(Set real-time detection)"pbdoc>)
        .def("set_real_time_detection_interval", &BottleneckDetector::setRealTimeDetectionInterval,
             py::arg("interval"), R"pbdoc(Set real-time detection interval)"pbdoc");

    // ResourceUsageAnalyzer bindings
    py::class_<ResourceUsageAnalyzer>(m, "ResourceUsageAnalyzer", R"pbdoc(Resource usage analyzer class)"pbdoc")
        .def(py::init<>(), R"pbdoc(Default constructor)"pbdoc")
        .def(py::init<const ResourceUsageAnalyzerConfig&>(), py::arg("config"),
             R"pbdoc(Constructor with configuration)"pbdoc")
        .def("apply_config", &ResourceUsageAnalyzer::applyConfig,
             py::arg("config"), R"pbdoc(Apply configuration)"pbdoc")
        .def("get_config", &ResourceUsageAnalyzer::getConfig,
             R"pbdoc(Get current configuration)"pbdoc")
        .def("analyze_memory_usage", &ResourceUsageAnalyzer::analyzeMemoryUsage,
             R"pbdoc(Analyze memory usage)"pbdoc")
        .def("analyze_cpu_usage", &ResourceUsageAnalyzer::analyzeCPUUsage,
             R"pbdoc(Analyze CPU usage)"pbdoc")
        .def("analyze_gpu_usage", &ResourceUsageAnalyzer::analyzeGPUUsage,
             R"pbdoc(Analyze GPU usage)"pbdoc")
        .def("analyze_io_usage", &ResourceUsageAnalyzer::analyzeIOUsage,
             R"pbdoc(Analyze I/O usage)"pbdoc")
        .def("analyze_network_usage", &ResourceUsageAnalyzer::analyzeNetworkUsage,
             R"pbdoc(Analyze network usage)"pbdoc")
        .def("analyze_resource_utilization", &ResourceUsageAnalyzer::analyzeResourceUtilization,
             R"pbdoc(Analyze resource utilization)"pbdoc")
        .def("calculate_performance_score", &ResourceUsageAnalyzer::calculatePerformanceScore,
             py::arg("metrics"), py::arg("memory"), py::arg("cpu"), py::arg("gpu"),
             py::arg("io"), py::arg("network"),
             R"pbdoc(Calculate performance score)"pbdoc")
        .def("optimize_for_cpu", &ResourceUsageAnalyzer::optimizeForCPU,
             py::arg("config"), R"pbdoc(Optimize for CPU)"pbdoc")
        .def("optimize_for_memory", &ResourceUsageAnalyzer::optimizeForMemory,
             py::arg("config"), R"pbdoc(Optimize for memory)"pbdoc")
        .def("optimize_for_gpu", &ResourceUsageAnalyzer::optimizeForGPU,
             py::arg("config"), R"pbdoc(Optimize for GPU)"pbdoc")
        .def("optimize_for_network", &ResourceUsageAnalyzer::optimizeForNetwork,
             py::arg("config"), R"pbdoc(Optimize for network)"pbdoc")
        .def("export_analysis", &ResourceUsageAnalyzer::exportAnalysis,
             py::arg("filepath"), R"pbdoc(Export analysis to file)"pbdoc")
        .def("import_analysis", &ResourceUsageAnalyzer::importAnalysis,
             py::arg("filepath"), R"pbdoc(Import analysis from file)"pbdoc>)
        .def("start_resource_monitoring", &ResourceUsageAnalyzer::startResourceMonitoring,
             py::arg("interval") = std::chrono::milliseconds(1000),
             R"pbdoc(Start resource monitoring)"pbdoc")
        .def("stop_resource_monitoring", &ResourceUsageAnalyzer::stopResourceMonitoring,
             R"pbdoc(Stop resource monitoring)"pbdoc>)
        .def("is_resource_monitoring_active", &ResourceUsageAnalyzer::isResourceMonitoringActive,
             R"pbdoc(Check if resource monitoring is active)"pbdoc>)
        .def("get_resource_utilization_history", &ResourceUsageAnalyzer::getResourceUtilizationHistory,
             R"pbdoc(Get resource utilization history)"pbdoc>)
        .def("set_resource_thresholds", &ResourceUsageAnalyzer::setResourceThresholds,
             py::arg("cpu_threshold"), py::arg("memory_threshold"),
             py::arg("io_threshold"), py::arg("network_threshold"),
             R"pbdoc(Set resource thresholds)"pbdoc")
        .def("set_alert_callback", &ResourceUsageAnalyzer::setAlertCallback,
             py::arg("callback"), R"pbdoc(Set alert callback)"pbdoc>)
        .def("get_cpu_threshold", &ResourceUsageAnalyzer::getCPUThreshold,
             R"pbdoc(Get CPU threshold)"pbdoc")
        .def("get_memory_threshold", &ResourceUsageAnalyzer::getMemoryThreshold,
             R"pbdoc(Get memory threshold)"pbdoc")
        .def("get_io_threshold", &ResourceUsageAnalyzer::getIOThreshold,
             R"pbdoc(Get I/O threshold)"pbdoc")
        .def("get_network_threshold", &ResourceUsageAnalyzer::getNetworkThreshold,
             R"pbdoc(Get network threshold)"pbdoc");

    // BenchmarkSuite bindings
    py::class_<BenchmarkSuite>(m, "BenchmarkSuite", R"pbdoc(Benchmark suite class)"pbdoc")
        .def(py::init<>(), R"pbdoc(Default constructor)"pbdoc")
        .def(py::init<const BenchmarkSuiteConfig&>(), py::arg("config"),
             R"pbdoc(Constructor with configuration)"pbdoc")
        .def("apply_config", &BenchmarkSuite::applyConfig,
             py::arg("config"), R"pbdoc(Apply configuration)"pbdoc")
        .def("get_config", &BenchmarkSuite::getConfig,
             R"pbdoc(Get current configuration)"pbdoc")
        .def("run_scenario", &BenchmarkSuite::runScenario,
             py::arg("scenario"), py::arg("config") = nullptr,
             py::arg("parameters") = std::map<std::string, std::string>(),
             R"pbdoc(Run benchmark scenario)"pbdoc")
        .def("run_benchmark_suite", &BenchmarkSuite::runBenchmarkSuite,
             py::arg("scenarios"), py::arg("config") = nullptr,
             R"pbdoc(Run benchmark suite)"pbdoc")
        .def("compare_configurations", &BenchmarkSuite::compareConfigurations,
             py::arg("configs"), R"pbdoc(Compare configurations)"pbdoc")
        .def("check_performance_regression", &BenchmarkSuite::checkPerformanceRegression,
             py::arg("baseline_config_name"), py::arg("current_config"),
             py::arg("tolerance") = 0.1,
             R"pbdoc(Check performance regression)"pbdoc")
        .def("get_statistics", &BenchmarkSuite::getStatistics,
             R"pbdoc(Get benchmark statistics)"pbdoc")
        .def("export_results", &BenchmarkSuite::exportResults,
             py::arg("filepath"), R"pbdoc(Export results to file)"pbdoc")
        .def("import_results", &BenchmarkSuite::importResults,
             py::arg("filepath"), R"pbdoc(Import results from file)"pbdoc>);

    // Factory functions
    m.def("make_performance_monitor", &makePerformanceMonitor,
          py::arg("config") = PerformanceMonitorConfig(),
          R"pbdoc(Create performance monitor)"pbdoc");

    m.def("make_bottleneck_detector", &makeBottleneckDetector,
          py::arg("config") = BottleneckDetectorConfig(),
          R"pbdoc(Create bottleneck detector)"pbdoc");

    m.def("make_resource_usage_analyzer", &makeResourceUsageAnalyzer,
          py::arg("config") = ResourceUsageAnalyzerConfig(),
          R"pbdoc(Create resource usage analyzer)"pbdoc");

    m.def("make_benchmark_suite", &makeBenchmarkSuite,
          py::arg("config") = BenchmarkSuiteConfig(),
          R"pbdoc(Create benchmark suite)"pbdoc");

    // Performance configuration structures
    py::class_<PerformanceMonitorConfig>(m, "PerformanceMonitorConfig", R"pbdoc(Performance monitor configuration)"pbdoc")
        .def(py::init<>())
        .def_readwrite("monitoring_interval", &PerformanceMonitorConfig::monitoringInterval)
        .def_readwrite("metrics_collection", &PerformanceMonitorConfig::metricsCollection)
        .def_readwrite("bottleneck_detection", &PerformanceMonitorConfig::bottleneckDetection)
        .def_readwrite("export_format", &PerformanceMonitorConfig::exportFormat)
        .def_readwrite("real_time_reporting", &PerformanceMonitorConfig::realTimeReporting)
        .def_readwrite("cpu_threshold", &PerformanceMonitorConfig::cpuThreshold)
        .def_readwrite("memory_threshold", &PerformanceMonitorConfig::memoryThreshold)
        .def_readwrite("io_threshold", &PerformanceMonitorConfig::ioThreshold)
        .def_readwrite("network_threshold", &PerformanceMonitorConfig::networkThreshold);

    py::class_<BottleneckDetectorConfig>(m, "BottleneckDetectorConfig", R"pbdoc(Bottleneck detector configuration)"pbdoc")
        .def(py::init<>())
        .def_readwrite("sensitivity", &BottleneckDetectorConfig::sensitivity)
        .def_readwrite("time_window", &BottleneckDetectorConfig::timeWindow)
        .def_readwrite("min_severity", &BottleneckDetectorConfig::minSeverity)
        .def_readwrite("enable_real_time_detection", &BottleneckDetectorConfig::enableRealTimeDetection)
        .def_readwrite("real_time_interval", &BottleneckDetectorConfig::realTimeInterval)
        .def_readwrite("export_analysis", &BottleneckDetectorConfig::exportAnalysis)
        .def_readwrite("analysis_filepath", &BottleneckDetectorConfig::analysisFilepath);

    py::class_<ResourceUsageAnalyzerConfig>(m, "ResourceUsageAnalyzerConfig", R"pbdoc(Resource usage analyzer configuration)"pbdoc")
        .def(py::init<>())
        .def_readwrite("cpu_threshold", &ResourceUsageAnalyzerConfig::cpuThreshold)
        .def_readwrite("memory_threshold", &ResourceUsageAnalyzerConfig::memoryThreshold)
        .def_readwrite("io_threshold", &ResourceUsageAnalyzerConfig::ioThreshold)
        .def_readwrite("network_threshold", &ResourceUsageAnalyzerConfig::networkThreshold)
        .def_readwrite("enable_real_time_monitoring", &ResourceUsageAnalyzerConfig::enableRealTimeMonitoring)
        .def_readwrite("monitoring_interval", &ResourceUsageAnalyzerConfig::monitoringInterval)
        .def_readwrite("export_analysis", &ResourceUsageAnalyzerConfig::exportAnalysis)
        .def_readwrite("analysis_filepath", &ResourceUsageAnalyzerConfig::analysisFilepath);

    py::class_<BenchmarkSuiteConfig>(m, "BenchmarkSuiteConfig", R"pbdoc(Benchmark suite configuration)"pbdoc")
        .def(py::init<>())
        .def_readwrite("scenarios", &BenchmarkSuiteConfig::scenarios)
        .def_readwrite("baseline_config", &BenchmarkSuiteConfig::baselineConfig)
        .def_readwrite("performance_config", &BenchmarkSuiteConfig::performanceConfig)
        .def_readwrite("learning_config", &BenchmarkSuiteConfig::learningConfig)
        .def_readwrite("scalability_config", &BenchmarkSuiteConfig::scalabilityConfig)
        .def_readwrite("performance_regression_tolerance", &BenchmarkSuiteConfig::performanceRegressionTolerance)
        .def_readwrite("enable_performance_regression_testing", &BenchmarkSuiteConfig::enablePerformanceRegressionTesting)
        .def_readwrite("export_results", &BenchmarkSuiteConfig::exportResults)
        .def_readwrite("results_filepath", &BenchmarkSuiteConfig::resultsFilepath);

    // Benchmark scenario enum
    py::enum_<BenchmarkSuite::Scenario>(m, "BenchmarkScenario", R"pbdoc(Benchmark scenario)"pbdoc")
        .value("SimpleConnectivity", BenchmarkSuite::Scenario::SimpleConnectivity)
        .value("PlasticityLearning", BenchmarkSuite::Scenario::PlasticityLearning)
        .value("STDPVerification", BenchmarkSuite::Scenario::STDPVerification)
        .value("PerformanceTest", BenchmarkSuite::Scenario::PerformanceTest)
        .value("MemoryUsage", BenchmarkSuite::Scenario::MemoryUsage)
        .value("Scalability", BenchmarkSuite::Scenario::Scalability)
        .value("MultiModalProcessing", BenchmarkSuite::Scenario::MultiModalProcessing)
        .value("HierarchicalProcessing", BenchmarkSuite::Scenario::HierarchicalProcessing)
        .value("AttentionMechanisms", BenchmarkSuite::Scenario::AttentionMechanisms)
        .value("Custom", BenchmarkSuite::Scenario::Custom)
        .export_values();

    // Performance metric enum
    py::enum_<BottleneckAnalysis::Type>(m, "BottleneckType", R"pbdoc(Bottleneck type)"pbdoc")
        .value("CPUBound", BottleneckAnalysis::Type::CPUBound)
        .value("MemoryBound", BottleneckAnalysis::Type::MemoryBound)
        .value("IOBound", BottleneckAnalysis::Type::IOBound)
        .value("NetworkBound", BottleneckAnalysis::Type::NetworkBound)
        .value("AlgorithmicComplex", BottleneckAnalysis::Type::AlgorithmicComplex)
        .value("Synchronization", BottleneckAnalysis::Type::Synchronization)
        .value("ResourceContention", BottleneckAnalysis::Type::ResourceContention)
        .value("CacheMisses", BottleneckAnalysis::Type::CacheMisses)
        .value("BranchMisprediction", BottleneckAnalysis::Type::BranchMisprediction)
        .value("ThermalThrottling", BottleneckAnalysis::Type::ThermalThrottling)
        .export_values();

    // Performance constants
    m.attr("DEFAULT_MONITORING_INTERVAL") = 0.001;
    m.attr("DEFAULT_CPU_THRESHOLD") = 0.8;
    m.attr("DEFAULT_MEMORY_THRESHOLD") = 0.9;
    m.attr("DEFAULT_IO_THRESHOLD") = 0.7;
    m.attr("DEFAULT_NETWORK_THRESHOLD") = 0.6;
    m.attr("DEFAULT_PERFORMANCE_REGRESSION_TOLERANCE") = 0.1;

    // Error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
}
