// PerformanceExperiment.hpp - Performance experiments
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <unordered_map>

#include "experiments/Experiment.hpp"
#include "experiments/Metrics.hpp"
#include "profiler/ProfilerIntegration.hpp"

namespace nlm {

/**
 * PerformanceExperiment - Performance experiments for NLM
 * 
 * Provides comprehensive performance experiments including:
 * - Performance benchmarking and testing
 * - Performance regression testing
 * - Scalability analysis
 * - Performance optimization experiments
 * - Performance comparison experiments
 * - Performance monitoring experiments
 * 
 * Features:
 * - Configurable performance experiments
 * - Integration with existing experiment system
 * - Integration with profiler integration
 * - Performance data collection and analysis
 * - Statistical analysis of performance data
 * - Performance comparison and validation
 * 
 * Integration with existing systems:
 * - Existing experiment system
 * - Profiler integration
 * - Configuration system
 * - Performance profiler
 * 
 * @see Experiment
 * @see ProfilerIntegration
 * @see Metrics
 */
class PerformanceExperiment : public Experiment {
public:
    /**
     * Performance experiment type enumeration
     */
    enum class ExperimentType {
        BASIC_PERFORMANCE,
        MEMORY_PERFORMANCE,
        THREAD_PERFORMANCE,
        SCALABILITY,
        REGRESSION_TESTING,
        OPTIMIZATION,
        BENCHMARKING,
        REAL_TIME_MONITORING,
        THROUGHTPUT_ANALYSIS,
        LATENCY_ANALYSIS
    };

    /**
     * Performance experiment configuration
     */
    struct PerformanceExperimentConfig {
        std::string name;
        ExperimentType type;
        int64_t duration; // milliseconds
        int64_t warmupDuration; // milliseconds
        int64_t cooldownDuration; // milliseconds
        int64_t samplingInterval; // milliseconds
        int64_t warmupSamplingInterval; // milliseconds
        
        // Experiment parameters
        size_t numThreads; // for thread performance experiments
        size_t numNeurons; // for scalability experiments
        double loadFactor; // for performance experiments
        bool enableRealTimeMonitoring; // for real-time monitoring experiments
        bool enableDetailedMetrics; // for detailed metrics collection
        
        // Performance targets
        double targetThroughput; // ops/sec
        double targetLatency; // milliseconds
        double targetMemoryUsage; // bytes
        
        // Experiment validation
        bool validateRegression; // for regression testing
        bool validateOptimization; // for optimization experiments
        double regressionThreshold; // for regression detection
        double optimizationThreshold; // for optimization detection
        
        // Output configuration
        bool enableFileOutput; // for experiment results
        std::string outputFile; // for experiment results
        bool enableConsoleOutput; // for experiment results
        
        // Analysis configuration
        bool enableStatisticalAnalysis; // for statistical analysis
        bool enableTrendAnalysis; // for trend analysis
        bool enableBottleneckDetection; // for bottleneck detection
        bool enableRecommendationGeneration; // for recommendation generation
    };

    /**
     * Performance experiment result
     */
    struct PerformanceExperimentResult {
        std::string experimentName;
        ExperimentType type;
        int64_t duration; // milliseconds
        
        // Performance metrics
        double throughput; // ops/sec
        double avgLatency; // milliseconds
        double p95Latency; // 95th percentile latency
        double p99Latency; // 99th percentile latency
        double throughputVariation; // coefficient of variation
        
        // Resource usage
        double avgMemoryUsage; // bytes
        double peakMemoryUsage; // bytes
        double cpuUtilization; // percentage
        
        // Thread performance
        size_t activeThreads;
        double threadSynchronizationOverhead; // milliseconds
        size_t contextSwitches;
        
        // Experiment results
        bool passed; // true if experiment passed validation
        std::string errorMessage; // error message if experiment failed
        std::string validationResults; // validation results
        
        // Statistical analysis
        double meanThroughput;
        double stdDevThroughput;
        double meanLatency;
        double stdDevLatency;
        double meanMemoryUsage;
        double stdDevMemoryUsage;
        
        // Experiment metadata
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        std::vector<std::pair<int64_t, double>> throughputTimeline;
        std::vector<std::pair<int64_t, double>> latencyTimeline;
        std::vector<std::pair<int64_t, double>> memoryUsageTimeline;
        
        // Comparison with baseline
        double throughputChangeRatio; // change from baseline
        double latencyChangeRatio; // change from baseline
        double memoryChangeRatio; // change from baseline
        bool hasRegression; // true if regression detected
        bool hasOptimization; // true if optimization detected
        
        // Recommendations
        std::vector<std::string> recommendations;
        std::vector<std::string> bottlenecks;
        std::vector<std::string> warnings;
    };

    /**
     * PerformanceExperiment constructor
     */
    PerformanceExperiment();
    
    /**
     * PerformanceExperiment constructor with configuration
     */
    PerformanceExperiment(const PerformanceExperimentConfig& config);
    
    /**
     * PerformanceExperiment destructor
     */
    ~PerformanceExperiment();
    
    /**
     * Configure experiment
     */
    void configure(const PerformanceExperimentConfig& config) { experimentConfig = config; }
    
    /**
     * Set profiler integration
     */
    void setProfilerIntegration(std::shared_ptr<ProfilerIntegration> profiler) {
        this->profilerIntegration = profiler;
    }
    
    /**
     * Start performance experiment
     */
    bool start() override;
    
    /**
     * Execute performance experiment
     */
    bool execute() override;
    
    /**
     * Stop performance experiment
     */
    void stop() override;
    
    /**
     * Check if experiment is active
     */
    bool isActive() const override { return isActive; }
    
    /**
     * Get experiment name
     */
    const std::string& getName() const override { return experimentConfig.name; }
    
    /**
     * Get experiment type
     */
    ExperimentType getType() const { return experimentConfig.type; }
    
    /**
     * Get experiment result
     */
    const PerformanceExperimentResult& getResult() const { return result; }
    
    /**
     * Get experiment metrics
     */
    const std::vector<std::pair<std::string, double>>& getMetrics() const override;
    
    /**
     * Get experiment notes
     */
    const std::vector<std::string>& getNotes() const override { return notes; }
    
    /**
     * Get experiment duration
     */
    int64_t getDuration() const override { return result.duration; }
    
    /**
     * Get experiment throughput
     */
    double getThroughput() const { return result.throughput; }
    
    /**
     * Get experiment latency
     */
    double getLatency() const { return result.avgLatency; }
    
    /**
     * Get experiment memory usage
     */
    double getMemoryUsage() const { return result.avgMemoryUsage; }
    
    /**
     * Get experiment status
     */
    bool getStatus() const { return result.passed; }
    
    /**
     * Validate experiment configuration
     */
    bool validateConfiguration() const;
    
    /**
     * Run basic performance experiment
     */
    bool runBasicPerformanceExperiment();
    
    /**
     * Run memory performance experiment
     */
    bool runMemoryPerformanceExperiment();
    
    /**
     * Run thread performance experiment
     */
     */
    bool runThreadPerformanceExperiment();
    
    /**
     * Run scalability experiment
     */
    bool runScalabilityExperiment();
    
    /**
     * Run regression testing experiment
     */
    bool runRegressionTestingExperiment();
    
    /**
     * Run optimization experiment
     */
    bool runOptimizationExperiment();
    
    /**
     * Run benchmarking experiment
     */
    bool runBenchmarkingExperiment();
    
    /**
     * Run real-time monitoring experiment
     */
    bool runRealTimeMonitoringExperiment();
    
    /**
     * Run throughput analysis experiment
     */
    bool runThroughputAnalysisExperiment();
    
    /**
     * Run latency analysis experiment
     */
    bool runLatencyAnalysisExperiment();
    
    /**
     * Analyze experiment results
     */
    void analyzeResults();
    
    /**
     * Validate experiment results
     */
    bool validateResults() const;
    
    /**
     * Generate experiment report
     */
    std::string generateReport(const std::string& format = "auto") const;
    
    /**
     * Export experiment results
     */
    bool exportResults(const std::string& filename, const std::string& format = "auto") const;
    
    /**
     * Compare with baseline
     */
    std::string compareWithBaseline(const PerformanceExperimentResult& baseline) const;
    
    /**
     * Detect regressions
     */
    bool detectRegressions(const PerformanceExperimentResult& baseline) const;
    
    /**
     * Detect optimizations
     */
    bool detectOptimizations(const PerformanceExperimentResult& baseline) const;

private:
    PerformanceExperimentConfig experimentConfig;
    PerformanceExperimentResult result;
    bool isActive;
    
    std::shared_ptr<ProfilerIntegration> profilerIntegration;
    std::vector<std::string> notes;
    
    std::chrono::steady_clock::time_point experimentStartTime;
    std::chrono::steady_clock::time_point experimentEndTime;
    
    mutable std::mutex dataMutex;
    
    void initializeExperiment();
    void setupExperiment();
    void executeBasicPerformance();
    void executeMemoryPerformance();
    void executeThreadPerformance();
    void executeScalability();
    void executeRegressionTesting();
    void executeOptimization();
    void executeBenchmarking();
    void executeRealTimeMonitoring();
    void executeThroughputAnalysis();
    void executeLatencyAnalysis();
    
    void collectPerformanceData();
    void analyzePerformanceData();
    void validateExperimentResults();
    void generateRecommendations();
    void generateBottlenecks();
    void generateWarnings();
    
    double calculateThroughput(const std::vector<std::pair<int64_t, double>>& timeline) const;
    double calculateLatency(const std::vector<std::pair<int64_t, double>>& timeline) const;
    double calculateMemoryUsage(const std::vector<std::pair<int64_t, double>>& timeline) const;
    
    // Suppress warnings about unused private members
    PerformanceExperiment(const PerformanceExperiment&) = delete;
    PerformanceExperiment& operator=(const PerformanceExperiment&) = delete;
};

} // namespace nlm