// NLM Performance Monitor Header
// Provides comprehensive system performance monitoring capabilities
// Includes CPU, memory, and application metrics collection

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <nlohmann/json.hpp>

namespace nlm {

class SystemPerformanceMonitor {
public:
    struct SystemMetrics {
        double cpuUsagePercent;
        size_t memoryTotalKB;
        size_t memoryUsedKB;
        size_t memoryAvailableKB;
        size_t memoryCachedKB;
        size_t swapTotalKB;
        size_t swapUsedKB;
        size_t threadCount;
        size_t processCount;
        double systemLoadAverage;
        std::chrono::system_clock::time_point timestamp;
    };
    
    struct ApplicationMetrics {
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeuronCount;
        size_t activeSynapseCount;
        double spikeRate;
        double simulationTimeMs;
        size_t processingSteps;
        double efficiencyScore;
        std::chrono::system_clock::time_point timestamp;
    };
    
    struct CombinedMetrics {
        SystemMetrics system;
        ApplicationMetrics application;
        std::vector<PerformanceSample> performanceHistory;
        std::map<std::string, double> customMetrics;
    };
    
    struct PerformanceAnalysis {
        double cpuUtilization;
        size_t memoryEfficiency;
        double spikeEfficiency;
        double synapticEfficiency;
        double computationalEfficiency;
        std::vector<std::string> recommendations;
    };
    
    struct PerformanceComparison {
        std::string baselineMetricName;
        double baselineValue;
        double currentValue;
        double improvementPercent;
        std::string trend;
    };
    
    struct BenchmarkResult {
        std::string benchmarkName;
        double executionTimeMs;
        size_t memoryUsageKB;
        double spikesPerSecond;
        double synapsesPerMs;
        double efficiencyScore;
        std::chrono::system_clock::time_point timestamp;
    };
    
    SystemPerformanceMonitor();
    ~SystemPerformanceMonitor();
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_; }
    
    // Get metrics
    SystemMetrics getSystemMetrics() const;
    ApplicationMetrics getApplicationMetrics() const;
    CombinedMetrics getCombinedMetrics() const;
    
    // Performance analysis
    PerformanceAnalysis analyzePerformance() const;
    
    // Performance comparison
    std::vector<PerformanceComparison> compareWithBaseline(const std::string& baselineFile) const;
    
    // Optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    
    // Performance benchmarking
    BenchmarkResult runBenchmark(const std::string& benchmarkName,
                                const std::shared_ptr<Config>& config) const;
    
    // Performance profiling
    bool profilePerformance(const std::string& profileName,
                           const std::function<void()>& workload,
                           int durationSeconds = 60) const;
    
    // Performance analysis
    bool analyzePerformanceProfile(const std::string& profileFile,
                                  const std::string& outputFile = "") const;
    
private:
    bool monitoring_ = false;
    std::thread monitorThread_;
    std::atomic<bool> monitorStopFlag_ = false;
    
    mutable std::mutex mutex_;
    
    SystemMetrics getSystemMetricsInternal() const;
    ApplicationMetrics getApplicationMetricsInternal() const;
    
    void updateSystemMetrics();
    void updateApplicationMetrics();
    void checkPerformanceThresholds();
    void generatePerformanceAlerts();
    
    // Helper functions
    double getCurrentCPUUsage() const;
    size_t getCurrentMemoryUsage() const;
    size_t getCurrentMemoryPeak() const;
    size_t getCurrentThreadCount() const;
    size_t getCurrentActiveNeuronCount() const;
    size_t getCurrentActiveSynapseCount() const;
    double getCurrentSpikeRate() const;
    double getCurrentSimulationTime() const;
    
    bool loadPerformanceData(const std::string& filename,
                            std::vector<PerformanceSample>& samples) const;
    double calculateAverageExecutionTime(const std::vector<PerformanceSample>& samples) const;
}; 

} // namespace nlm
