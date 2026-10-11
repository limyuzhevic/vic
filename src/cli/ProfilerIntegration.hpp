// ProfilerIntegration.hpp - Enhanced CLI profiler integration
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>

// Core profiler headers
#include "profiler/PerformanceProfiler.hpp"
#include "profiler/MemoryProfiler.hpp"
#include "profiler/ThreadProfiler.hpp"
#include "profiler/Benchmark.hpp"
#include "profiler/Visualization.hpp"

namespace nlm {

/**
 * ProfilerIntegration - Enhanced performance profiler integration
 * 
 * Provides comprehensive performance profiler integration including:
 * - Core performance profiling with PerformanceProfiler
 * - Memory profiling with MemoryProfiler
 * - Thread-level profiling with ThreadProfiler
 * - Performance benchmarking with Benchmark
 * - Performance visualization with Visualization
 * - Integration with batch processing for comparative analysis
 * - Integration with existing logging system for profile output
 * - Integration with configuration system for profiler settings
 * - Real-time performance monitoring during execution
 * - Post-execution performance analysis
 * 
 * Features:
 * - Multi-tier profiling architecture
 * - Real-time monitoring with configurable sampling
 * - Automatic performance bottleneck detection
 * - Performance comparison and regression testing
 * - Scalability analysis
 * - Multi-format output (JSON, CSV, HTML)
 * - Web-based performance dashboards
 * 
 * Integration with existing systems:
 * - Batch processing for comparative analysis
 * - Existing logging system for profile output
 * - Configuration system for profiler settings
 * 
 * @see PerformanceProfiler
 * @see MemoryProfiler
 * @see ThreadProfiler
 * @see Benchmark
 * @see Visualization
 */
class ProfilerIntegration {
public:
    /**
     * Performance profiling configuration
     */
    struct PerformanceConfig {
        bool enableCoreProfiler = true;
        bool enableMemoryProfiler = true;
        bool enableThreadProfiler = true;
        bool enableBenchmarking = true;
        bool enableVisualization = true;
        
        // Core profiler settings
        PerformanceProfiler::ProfilerConfig coreProfilerConfig;
        
        // Memory profiler settings
        MemoryProfiler::MemoryProfilerConfig memoryProfilerConfig;
        
        // Thread profiler settings
        ThreadProfiler::ThreadProfilerConfig threadProfilerConfig;
        
        // Benchmarking settings
        Benchmark::BenchmarkConfig benchmarkConfig;
        
        // Visualization settings
        Visualization::VisualizationConfig visualizationConfig;
        
        // Comparative analysis settings
        bool enableBaselineComparison = true;
        std::string baselineFile;
        bool enableRegressionTesting = true;
        double regressionThreshold = 0.1; // 10% regression threshold
        
        // Output settings
        bool enableFileOutput = true;
        bool enableConsoleOutput = true;
        std::string outputDirectory;
        std::string sessionName;
        
        // Real-time monitoring
        bool enableRealTimeMonitoring = true;
        double samplingIntervalMs = 100.0; // milliseconds
        
        // Analysis settings
        bool enableBottleneckDetection = true;
        bool enableScalabilityAnalysis = true;
        bool enableOptimizationRecommendations = true;
    };

    /**
     * Performance integration status
     */
    struct IntegrationStatus {
        bool coreProfilerActive = false;
        bool memoryProfilerActive = false;
        bool threadProfilerActive = false;
        bool benchmarkProfilerActive = false;
        bool visualizationActive = false;
        
        int64_t totalProfileDuration = 0;
        size_t totalSamplesCollected = 0;
        size_t bottlenecksDetected = 0;
        size_t regressionsDetected = 0;
        bool analysisComplete = false;
    };

    /**
     * ProfilerIntegration constructor
     */
    ProfilerIntegration();
    
    /**
     * ProfilerIntegration constructor with configuration
     */
    ProfilerIntegration(const PerformanceConfig& config);
    
    /**
     * ProfilerIntegration destructor
     */
    ~ProfilerIntegration();
    
    /**
     * Configure profiler integration
     */
    void configure(const PerformanceConfig& config) { performanceConfig = config; }
    
    /**
     * Start all profilers
     */
    void startProfiling(const std::string& sessionName = "nlm_performance");
    
    /**
     * Stop all profilers
     */
    void stopProfiling();
    
    /**
     * Check if profiling is active
     */
    bool isProfilingActive() const;
    
    /**
     * Start individual profiler
     */
    void startProfiler(const std::string& profilerName);
    
    /**
     * Stop individual profiler
     */
    void stopProfiler(const std::string& profilerName);
    
    /**
     * Check if specific profiler is active
     */
    bool isProfilerActive(const std::string& profilerName) const;
    
    /**
     * Record performance data
     */
    void recordPerformanceData(const std::string& profilerName,
                              const PerformanceProfiler::PerformanceData& data);
    
    /**
     * Record memory allocation
     */
    void recordMemoryAllocation(const std::string& profilerName,
                                void* address, size_t size,
                                const std::string& allocationSite = "",
                                const std::string& callStack = "",
                                int threadId = -1);
    
    /**
     * Record thread execution
     */
    void recordThreadExecution(const std::string& profilerName,
                               int64_t durationNs);
    
    /**
     * Record synchronization event
     */
    void recordSynchronizationEvent(const std::string& profilerName,
                                    const ThreadProfiler::SynchronizationEvent& event);
    
    /**
     * Record benchmark
     */
    void recordBenchmark(const std::string& profilerName,
                         const Benchmark::BenchmarkResult& result);
    
    /**
     * Get current performance data
     */
    PerformanceProfiler::PerformanceData getCurrentPerformanceData() const;
    
    /**
     * Get current memory stats
     */
    MemoryProfiler::MemoryStats getCurrentMemoryStats() const;
    
    /**
     * Get current thread stats
     */
    ThreadProfiler::ThreadStats getCurrentThreadStats() const;
    
    /**
     * Get performance comparison report
     */
    std::string getPerformanceComparisonReport() const;
    
    /**
     * Get performance regression report
     */
    std::string getPerformanceRegressionReport() const;
    
    /**
     * Get scalability analysis report
     */
    std::string getScalabilityAnalysisReport() const;
    
    /**
     * Get optimization recommendations
     */
    std::vector<std::string> getOptimizationRecommendations() const;
    
    /**
     * Get performance bottlenecks
     */
    std::vector<std::string> getPerformanceBottlenecks() const;
    
    /**
     * Generate comprehensive performance report
     */
    std::string generateComprehensiveReport(const std::string& format = "auto") const;
    
    /**
     * Export performance data to file
     */
    bool exportPerformanceData(const std::string& filename, const std::string& format = "auto") const;
    
    /**
     * Generate performance dashboard
     */
    bool generatePerformanceDashboard(const std::string& filename) const;
    
    /**
     * Load baseline performance data
     */
    bool loadBaselineData(const std::string& filename);
    
    /**
     * Set baseline data
     */
    void setBaselineData(const std::vector<PerformanceProfiler::PerformanceData>& baseline);
    
    /**
     * Compare with baseline
     */
    std::string compareWithBaseline(const std::vector<PerformanceProfiler::PerformanceData>& current) const;
    
    /**
     * Detect performance regressions
     */
    std::vector<std::string> detectPerformanceRegressions(
        const std::vector<PerformanceProfiler::PerformanceData>& current) const;
    
    /**
     * Analyze scalability
     */
    std::string analyzeScalability(const std::vector<PerformanceProfiler::PerformanceData>& data) const;
    
    /**
     * Get integration status
     */
    IntegrationStatus getStatus() const;
    
    /**
     * Reset profiler integration
     */
    void reset();
    
    /**
     * Clean up old data
     */
    void cleanupOldData();

private:
    PerformanceConfig performanceConfig;
    IntegrationStatus status;
    
    mutable std::mutex dataMutex;
    
    // Core profiler
    std::unique_ptr<PerformanceProfiler> coreProfiler;
    
    // Memory profiler
    std::unique_ptr<MemoryProfiler> memoryProfiler;
    
    // Thread profiler
    std::unique_ptr<ThreadProfiler> threadProfiler;
    
    // Benchmarking
    std::unique_ptr<Benchmark> benchmark;
    
    // Visualization
    std::unique_ptr<Visualization> visualizer;
    
    // Baseline data for comparison
    std::vector<PerformanceProfiler::PerformanceData> baselineData;
    
    // Performance history
    std::vector<PerformanceProfiler::PerformanceData> performanceHistory;
    
    // Bottlenecks detected
    std::vector<std::string> detectedBottlenecks;
    
    // Regressions detected
    std::vector<std::string> detectedRegressions;
    
    // Optimization recommendations
    std::vector<std::string> optimizationRecommendations;
    
    // Session management
    std::chrono::steady_clock::time_point startTime;
    
    void initializeProfilers();
    void updateStatus();
    void analyzePerformanceData();
    void detectBottlenecks();
    void detectRegressions();
    void generateRecommendations();
    void updatePerformanceHistory();
    
    // Suppress warnings about unused private members
    ProfilerIntegration(const ProfilerIntegration&) = delete;
    ProfilerIntegration& operator=(const ProfilerIntegration&) = delete;
};

} // namespace nlm