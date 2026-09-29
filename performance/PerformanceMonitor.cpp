// Performance Monitoring and Profiling Tools
//
// This module provides comprehensive performance monitoring, profiling,
// and benchmarking capabilities for the NLM system. It includes:
//
// 1. Performance Profiling Infrastructure
// 2. Benchmarking Tools
// 3. Performance Metrics Collection
// 4. Profiling Data Analysis
// 5. Performance Comparison Capabilities
// 6. Performance Alerting
//
// These tools help users monitor system performance, identify bottlenecks,
// optimize neural simulations, and ensure optimal operation.

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <memory>
#include <unordered_map>
#include <deque>
#include <functional>
#include <iostream>
#include <iomanip>

// Include NLM headers for integration
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "visualization/VisualizationInterface.hpp"

namespace nlm {

// Performance measurement types
enum class MeasurementUnit {
    Seconds,
    Milliseconds,
    Microseconds,
    Nanoseconds,
    Hertz,
    Bytes,
    BytesPerSecond,
    Percent,
    Count,
    OperationsPerSecond,
    MegaOperationsPerSecond
};

// Performance measurement result
struct PerformanceMeasurement {
    std::string name;
    double value;
    MeasurementUnit unit;
    std::chrono::system_clock::time_point timestamp;
    
    // Comparisons
    bool operator<(const PerformanceMeasurement& other) const {
        return value < other.value;
    }
    
    bool operator>(const PerformanceMeasurement& other) const {
        return value > other.value;
    }
    
    bool operator==(const PerformanceMeasurement& other) const {
        return std::abs(value - other.value) < 1e-9;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3);
        oss << name << ": " << value;
        
        switch (unit) {
            case MeasurementUnit::Seconds: oss << " s"; break;
            case MeasurementUnit::Milliseconds: oss << " ms"; break;
            case MeasurementUnit::Microseconds: oss << " μs"; break;
            case MeasurementUnit::Nanoseconds: oss << " ns"; break;
            case MeasurementUnit::Hertz: oss << " Hz"; break;
            case MeasurementUnit::Bytes: oss << " B"; break;
            case MeasurementUnit::BytesPerSecond: oss << " B/s"; break;
            case MeasurementUnit::Percent: oss << " %"; break;
            case MeasurementUnit::Count: oss << " count"; break;
            case MeasurementUnit::OperationsPerSecond: oss << " ops/s"; break;
            case MeasurementUnit::MegaOperationsPerSecond: oss << " MOPS"; break;
        }
        
        return oss.str();
    }
};

// Performance profile - collection of measurements over time
class PerformanceProfile {
public:
    PerformanceProfile(const std::string& name);
    
    // Add a measurement
    void addMeasurement(const std::string& metricName, double value, MeasurementUnit unit);
    
    // Add measurement with timing
    void addTimedMeasurement(const std::string& metricName, 
                            const std::function<void()>& operation);
    
    // Get measurements
    std::vector<PerformanceMeasurement> getMeasurements() const;
    std::vector<PerformanceMeasurement> getMeasurements(const std::string& metricName) const;
    
    // Statistical analysis
    double getMean(const std::string& metricName) const;
    double getMedian(const std::string& metricName) const;
    double getMin(const std::string& metricName) const;
    double getMax(const std::string& metricName) const;
    double getStdDev(const std::string& metricName) const;
    double getPercentile(const std::string& metricName, double percentile) const;
    
    // Profile statistics
    size_t getMeasurementCount(const std::string& metricName = "") const;
    std::string getProfileSummary() const;
    
    // Export/import
    bool exportProfile(const std::string& filename) const;
    bool importProfile(const std::string& filename);
    
    // Profile management
    void clear();
    void reset();
    std::string getName() const;
    
    // Time series analysis
    std::vector<std::pair<double, double>> getTimeSeries(const std::string& metricName) const;
    std::string getTimeSeriesAsCSV(const std::string& metricName) const;
    
    // Comparison with other profiles
    double compareWith(const PerformanceProfile& other, const std::string& metricName) const;
    std::string getComparisonReport(const PerformanceProfile& other) const;
    
    // Alerting
    void setThresholdAlert(const std::string& metricName, double threshold, bool isUpperBound = true);
    void removeThresholdAlert(const std::string& metricName);
    std::vector<std::string> getActiveAlerts() const;
    bool hasAlerts() const;
    
    // Benchmarking
    struct BenchmarkResult {
        std::string benchmarkName;
        double executionTime;
        size_t iterations;
        double overheadTime;
        std::string report() const;
    };
    
    BenchmarkResult benchmark(const std::string& benchmarkName, 
                              const std::function<void()>& operation,
                              size_t iterations = 100);
    
    // Performance analysis
    struct PerformanceAnalysis {
        std::string profileName;
        std::map<std::string, double> keyMetrics;
        std::string getRecommendations() const;
        std::string getDetailedReport() const;
    };
    
    PerformanceAnalysis analyzePerformance() const;
    
    // Integration with other systems
    void integrateWithBrain(const Brain& brain);
    void integrateWithAgent(const AgentBrain& agent);
    void integrateWithWorld(const SimpleWorld& world);
    void integrateWithVisualization(const VisualizationInterface& viz);
    
    // Real-time monitoring
    void startRealTimeMonitoring(const std::vector<std::string>& metrics, double interval = 1.0);
    void stopRealTimeMonitoring();
    bool isMonitoring() const;
    std::map<std::string, double> getCurrentMetrics() const;
    
    // Configuration
    struct PerformanceConfig {
        double samplingInterval = 0.1; // seconds
        size_t maxMeasurements = 10000;
        bool enableRealTimeMonitoring = false;
        bool enableAutomatedBenchmarking = false;
        double benchmarkInterval = 60.0; // seconds
        std::string outputDirectory = "./performance_data";
        bool exportToCSV = true;
        bool exportToJSON = true;
        bool enableAlerts = true;
    };
    
    void setConfiguration(const PerformanceConfig& config);
    PerformanceConfig getConfiguration() const;
    
    // Thread pool for concurrent measurements
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads);
        ~ThreadPool();
        
        template<typename F>
        void submitTask(const std::string& name, const F& task);
        
        std::vector<PerformanceMeasurement> getResults() const;
        void clearResults();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Start profiling thread pool
    void startThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    void stopThreadPool();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Benchmarking utilities
namespace benchmarking {
    
    // Simple benchmark runner
    class BenchmarkRunner {
    public:
        BenchmarkRunner();
        
        // Add benchmark
        void addBenchmark(const std::string& name, const std::function<void()>& operation);
        
        // Run all benchmarks
        std::vector<PerformanceProfile::BenchmarkResult> runAllBenchmarks(size_t iterations = 100);
        
        // Run specific benchmark
        PerformanceProfile::BenchmarkResult runBenchmark(const std::string& name, 
                                                         size_t iterations = 100);
        
        // Get benchmark results
        std::vector<std::string> getBenchmarkNames() const;
        PerformanceProfile::BenchmarkResult getBenchmarkResult(const std::string& name) const;
        
        // Configuration
        void setWarmupIterations(size_t warmup);
        void setMeasurementIterations(size_t measurements);
        
        // Export results
        bool exportResults(const std::string& filename) const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Performance regression testing
    class RegressionTester {
    public:
        RegressionTester();
        
        // Add baseline profile
        void setBaselineProfile(const PerformanceProfile& baseline);
        
        // Add new profile to test
        void addTestProfile(const PerformanceProfile& testProfile, const std::string& testName);
        
        // Run regression test
        struct RegressionResult {
            std::string testName;
            bool passed;
            std::map<std::string, double> regressions;
            std::string report() const;
        };
        
        RegressionResult runRegressionTest();
        
        // Get test results
        std::vector<RegressionResult> getTestResults() const;
        std::string getRegressionReport() const;
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
    // Performance profiler
    class Profiler {
    public:
        Profiler();
        
        // Start/stop profiling
        void startProfiling(const std::string& profileName);
        PerformanceProfile stopProfiling();
        
        // Get current profile
        PerformanceProfile getCurrentProfile() const;
        
        // Profile management
        void clearCurrentProfile();
        std::vector<std::string> getProfileNames() const;
        
        // Integration with NLM systems
        void integrateWithBrain(const Brain& brain);
        void integrateWithAgent(const AgentBrain& agent);
        void integrateWithWorld(const SimpleWorld& world);
        void integrateWithVisualization(const VisualizationInterface& viz);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
}

// Performance metrics collector
class PerformanceMetricsCollector {
public:
    PerformanceMetricsCollector();
    
    // Start/stop collection
    void startCollection();
    void stopCollection();
    bool isCollecting() const;
    
    // Add metric
    void addMetric(const std::string& name, double value, MeasurementUnit unit);
    void addMetric(const PerformanceMeasurement& measurement);
    
    // Get metrics
    std::vector<PerformanceMeasurement> getMetrics(const std::string& name = "") const;
    std::map<std::string, std::vector<PerformanceMeasurement>> getAllMetrics() const;
    
    // Statistics
    double getAverage(const std::string& metricName) const;
    double getMin(const std::string& metricName) const;
    double getMax(const std::string& metricName) const;
    double getStdDev(const std::string& metricName) const;
    
    // Export
    bool exportMetrics(const std::string& filename) const;
    std::string getMetricsAsCSV(const std::string& metricName = "") const;
    std::string getMetricsAsJSON(const std::string& metricName = "") const;
    
    // Configuration
    void setCollectionInterval(double interval);
    void setMaxMetrics(size_t maxMetrics);
    
    // Real-time monitoring
    void startRealTimeMonitoring(const std::vector<std::string>& metrics);
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoring() const;
    std::map<std::string, double> getCurrentMetricValues() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance alerting system
class PerformanceAlertSystem {
public:
    PerformanceAlertSystem();
    
    // Alert configuration
    struct Alert {
        std::string name;
        std::string metricName;
        double threshold;
        bool isUpperBound;
        std::string severity; // "low", "medium", "high", "critical"
        std::string message;
        std::chrono::system_clock::time_point lastTriggered;
        size_t triggerCount;
        
        bool isTriggered(double value) const;
        std::string getStatus() const;
    };
    
    // Add alert
    void addAlert(const Alert& alert);
    void addAlert(const std::string& name, const std::string& metricName, 
                  double threshold, bool isUpperBound, const std::string& severity = "medium");
    
    // Remove alert
    void removeAlert(const std::string& name);
    void clearAlerts();
    
    // Get alerts
    std::vector<Alert> getAlerts() const;
    std::vector<Alert> getAlerts(const std::string& metricName) const;
    Alert getAlert(const std::string& name) const;
    
    // Check and trigger alerts
    std::vector<Alert> checkAlerts(const std::map<std::string, double>& metrics);
    bool hasAlerts() const;
    std::vector<std::string> getActiveAlertNames() const;
    
    // Alert management
    void acknowledgeAlert(const std::string& name);
    void acknowledgeAllAlerts();
    
    // Configuration
    void setAlertCooldown(double cooldown); // seconds
    void setMaxAlerts(size_t maxAlerts);
    
    // Export/import
    bool exportAlerts(const std::string& filename) const;
    bool importAlerts(const std::string& filename);
    
    // Statistics
    std::string getAlertStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance analysis engine
class PerformanceAnalysisEngine {
public:
    PerformanceAnalysisEngine();
    
    // Add profile for analysis
    void addProfile(const PerformanceProfile& profile);
    
    // Get analysis results
    struct AnalysisResult {
        std::string profileName;
        std::map<std::string, double> metrics;
        std::string optimizationSuggestions;
        std::vector<std::string> bottlenecks;
        double performanceScore; // 0-100
        std::string getDetailedReport() const;
    };
    
    AnalysisResult analyzeProfile(const std::string& profileName) const;
    std::vector<AnalysisResult> analyzeAllProfiles() const;
    
    // Comparative analysis
    struct ComparativeAnalysis {
        std::map<std::string, double> differences;
        std::vector<std::string> recommendations;
        double improvementPercent;
        std::string getReport() const;
    };
    
    ComparativeAnalysis compareProfiles(const std::string& profile1, const std::string& profile2) const;
    
    // Performance prediction
    double predictPerformance(const std::string& profileName, 
                             const std::map<std::string, double>& loadFactors) const;
    
    // Bottleneck detection
    std::vector<std::string> detectBottlenecks(const PerformanceProfile& profile) const;
    std::map<std::string, double> identifyImprovementAreas(const PerformanceProfile& profile) const;
    
    // Configuration
    void setAnalysisDepth(double depth); // 0.0 to 1.0
    void setIncludeHistoricalData(bool include);
    
    // Export
    bool exportAnalysis(const std::string& filename) const;
    std::string getAnalysisAsJSON() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main performance monitoring interface
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Get profiles
    PerformanceProfile getCurrentProfile() const;
    std::vector<std::string> getProfileNames() const;
    PerformanceProfile getProfile(const std::string& name) const;
    
    // Configuration
    void setConfiguration(const PerformanceProfile::PerformanceConfig& config);
    PerformanceProfile::PerformanceConfig getConfiguration() const;
    
    // Integration with NLM systems
    void integrateWithBrain(const Brain& brain);
    void integrateWithAgent(const AgentBrain& agent);
    void integrateWithWorld(const SimpleWorld& world);
    void integrateWithVisualization(const VisualizationInterface& viz);
    
    // Performance metrics
    PerformanceMetricsCollector getMetricsCollector() const;
    void setMetricsCollector(const PerformanceMetricsCollector& collector);
    
    // Alert system
    PerformanceAlertSystem getAlertSystem() const;
    void setAlertSystem(const PerformanceAlertSystem& alertSystem);
    
    // Analysis engine
    PerformanceAnalysisEngine getAnalysisEngine() const;
    void setAnalysisEngine(const PerformanceAnalysisEngine& analysisEngine);
    
    // Benchmarking
    benchmarking::BenchmarkRunner getBenchmarkRunner() const;
    benchmarking::Profiler getProfiler() const;
    
    // Real-time monitoring
    void startRealTimeMonitoring(const std::vector<std::string>& metrics, double interval = 1.0);
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoring() const;
    std::map<std::string, double> getCurrentMetrics() const;
    
    // Export/import
    bool exportPerformanceData(const std::string& filename) const;
    bool importPerformanceData(const std::string& filename);
    
    // Health check
    bool isHealthy() const;
    std::string getHealthReport() const;
    
    // Cleanup
    void cleanup();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global performance monitor instance
PerformanceMonitor& getPerformanceMonitor();

// Convenience functions for common performance tasks
namespace performance {
    
    // Benchmark a specific operation
    PerformanceProfile::BenchmarkResult benchmark(const std::string& name,
                                                  const std::function<void()>& operation,
                                                  size_t iterations = 100);
    
    // Profile a specific function
    PerformanceProfile profile(const std::string& name,
                              const std::function<void()>& operation,
                              double timeout = 60.0);
    
    // Measure performance of NLM operations
    struct BrainPerformance {
        double initializeTime;
        double stepTime;
        double totalTime;
        size_t stepsCompleted;
        std::string report() const;
    };
    
    BrainPerformance measureBrainPerformance(const Brain& brain, size_t steps = 1000);
    
    struct AgentPerformance {
        double processInputTime;
        double decodeCommandTime;
        double applyModulationTime;
        double updateDevelopmentTime;
        double totalTime;
        size_t stepsCompleted;
        std::string report() const;
    };
    
    AgentPerformance measureAgentPerformance(const AgentBrain& agent, const SimpleWorld& world, size_t steps = 100);
    
    // Configuration
    void setPerformanceLogging(bool enable);
    bool isPerformanceLoggingEnabled();
    
    // Output
    void printPerformanceReport(const PerformanceProfile& profile);
    bool savePerformanceReport(const PerformanceProfile& profile, const std::string& filename);
    
}

// Implementation details for PerformanceProfile
namespace nlm {
    
    // PerformanceProfile implementation details
    class PerformanceProfile::Impl {
    public:
        std::string name;
        std::vector<PerformanceMeasurement> measurements;
        std::chrono::system_clock::time_point startTime;
        bool isMonitoring;
        double monitoringInterval;
        size_t maxMeasurements;
        
        // Thread pool
        std::vector<std::thread> threadPool;
        bool threadPoolRunning;
        std::mutex resultsMutex;
        
        // Alerts
        std::vector<Alert> alerts;
        std::mutex alertsMutex;
        
        // Configuration
        PerformanceConfig config;
        
        // Integration
        const Brain* brain;
        const AgentBrain* agent;
        const SimpleWorld* world;
        const VisualizationInterface* viz;
        
        // Real-time monitoring
        bool realTimeMonitoring;
        std::vector<std::string> monitoredMetrics;
        std::chrono::steady_clock::time_point lastUpdate;
        std::mutex metricsMutex;
        
        Impl(const std::string& profileName, const PerformanceConfig& config);
        ~Impl();
        
        // Methods implementation
        void addMeasurementInternal(const std::string& metricName, double value, MeasurementUnit unit);
        void addTimedMeasurementInternal(const std::string& metricName, 
                                         const std::function<void()>& operation);
        std::vector<PerformanceMeasurement> getMeasurementsInternal(const std::string& metricName = "") const;
        std::vector<PerformanceMeasurement> getAllMeasurementsInternal() const;
        
        void cleanupOldMeasurements();
        void updateRealTimeMetrics();
        bool checkAlerts(const std::map<std::string, double>& metricValues);
        
        void exportProfileInternal(const std::string& filename, bool includeComments = true) const;
        bool importProfileInternal(const std::string& filename);
        
        void startThreadPoolInternal(size_t numThreads);
        void stopThreadPoolInternal();
        void submitTaskInternal(const std::string& name, const std::function<void()>& task);
        
        PerformanceProfile::BenchmarkResult benchmarkInternal(const std::string& benchmarkName,
                                                              const std::function<void()>& operation,
                                                              size_t iterations);
        
        PerformanceAnalysis analyzePerformanceInternal() const;
        void integrateWithBrainInternal(const Brain& brain);
        void integrateWithAgentInternal(const AgentBrain& agent);
        void integrateWithWorldInternal(const SimpleWorld& world);
        void integrateWithVisualizationInternal(const VisualizationInterface& viz);
        
        std::string getNameInternal() const;
        bool isMonitoringInternal() const;
        std::vector<PerformanceMeasurement> getMeasurementsInternal() const;
        
        void setConfigurationInternal(const PerformanceConfig& config);
        PerformanceConfig getConfigurationInternal() const;
        
        void startRealTimeMonitoringInternal(const std::vector<std::string>& metrics, double interval);
        void stopRealTimeMonitoringInternal();
        bool isRealTimeMonitoringInternal() const;
        std::map<std::string, double> getCurrentMetricsInternal() const;
        
        std::string getProfileSummaryInternal() const;
        double getMeanInternal(const std::string& metricName) const;
        double getMedianInternal(const std::string& metricName) const;
        double getMinInternal(const std::string& metricName) const;
        double getMaxInternal(const std::string& metricName) const;
        double getStdDevInternal(const std::string& metricName) const;
        double getPercentileInternal(const std::string& metricName, double percentile) const;
        size_t getMeasurementCountInternal(const std::string& metricName = "") const;
        bool exportProfileInternal(const std::string& filename, bool includeComments = true) const;
        bool importProfileInternal(const std::string& filename);
        std::vector<std::pair<double, double>> getTimeSeriesInternal(const std::string& metricName) const;
        std::string getTimeSeriesAsCSVInternal(const std::string& metricName) const;
        double compareWithInternal(const PerformanceProfile& other, const std::string& metricName) const;
        std::string getComparisonReportInternal(const PerformanceProfile& other) const;
        void setThresholdAlertInternal(const std::string& metricName, double threshold, bool isUpperBound);
        void removeThresholdAlertInternal(const std::string& metricName);
        std::vector<std::string> getActiveAlertsInternal() const;
        bool hasAlertsInternal() const;
        std::string getProfileSummaryInternal(bool detailed = false) const;
        PerformanceAnalysis analyzePerformanceInternal() const;
        PerformanceProfile::BenchmarkResult benchmarkInternal(const std::string& benchmarkName,
                                                              const std::function<void()>& operation,
                                                              size_t iterations,
                                                              bool warmup);
    };
    
    // Benchmarking implementation details
    class benchmarking::BenchmarkRunner::Impl {
    public:
        std::map<std::string, std::function<void()>> benchmarks;
        size_t warmupIterations;
        size_t measurementIterations;
        
        BenchmarkRunner::BenchmarkResult runBenchmarkInternal(const std::string& name, 
                                                              const std::function<void()>& operation,
                                                              size_t iterations);
        
        Impl();
    };
    
    class benchmarking::Profiler::Impl {
    public:
        std::map<std::string, PerformanceProfile> profiles;
        std::string currentProfileName;
        
        Impl();
        
        PerformanceProfile profileInternal(const std::string& name,
                                           const std::function<void()>& operation,
                                           double timeout);
        
        void integrateWithBrainInternal(const Brain& brain);
        void integrateWithAgentInternal(const AgentBrain& agent);
        void integrateWithWorldInternal(const SimpleWorld& world);
        void integrateWithVisualizationInternal(const VisualizationInterface& viz);
    };
    
    // PerformanceMetricsCollector implementation details
    class PerformanceMetricsCollector::Impl {
    public:
        bool isCollecting;
        double collectionInterval;
        size_t maxMetrics;
        
        std::vector<PerformanceMeasurement> metrics;
        std::mutex metricsMutex;
        
        // Real-time monitoring
        bool realTimeMonitoring;
        std::vector<std::string> monitoredMetrics;
        std::map<std::string, double> currentValues;
        std::mutex currentValuesMutex;
        
        Impl();
        
        void addMetricInternal(const std::string& name, double value, MeasurementUnit unit);
        void addMetricInternal(const PerformanceMeasurement& measurement);
        
        std::vector<PerformanceMeasurement> getMetricsInternal(const std::string& name = "") const;
        std::map<std::string, std::vector<PerformanceMeasurement>> getAllMetricsInternal() const;
        
        double getAverageInternal(const std::string& metricName) const;
        double getMinInternal(const std::string& metricName) const;
        double getMaxInternal(const std::string& metricName) const;
        double getStdDevInternal(const std::string& metricName) const;
        
        bool exportMetricsInternal(const std::string& filename) const;
        std::string getMetricsAsCSVInternal(const std::string& metricName = "") const;
        std::string getMetricsAsJSONInternal(const std::string& metricName = "") const;
        
        void startRealTimeMonitoringInternal(const std::vector<std::string>& metrics);
        void stopRealTimeMonitoringInternal();
        bool isRealTimeMonitoringInternal() const;
        std::map<std::string, double> getCurrentMetricValuesInternal() const;
    };
    
    // PerformanceAlertSystem implementation details
    class PerformanceAlertSystem::Impl {
    public:
        std::vector<Alert> alerts;
        std::mutex alertsMutex;
        double cooldownPeriod;
        size_t maxAlerts;
        
        Impl();
        
        void addAlertInternal(const Alert& alert);
        void removeAlertInternal(const std::string& name);
        std::vector<Alert> getAlertsInternal() const;
        std::vector<Alert> getAlertsInternal(const std::string& metricName) const;
        Alert getAlertInternal(const std::string& name) const;
        
        std::vector<Alert> checkAlertsInternal(const std::map<std::string, double>& metrics);
        bool hasAlertsInternal() const;
        std::vector<std::string> getActiveAlertNamesInternal() const;
        
        void acknowledgeAlertInternal(const std::string& name);
        void acknowledgeAllAlertsInternal();
        
        bool exportAlertsInternal(const std::string& filename) const;
        bool importAlertsInternal(const std::string& filename);
        
        std::string getAlertStatisticsInternal() const;
    };
    
    // PerformanceAnalysisEngine implementation details
    class PerformanceAnalysisEngine::Impl {
    public:
        std::map<std::string, PerformanceProfile> profiles;
        double analysisDepth;
        bool includeHistoricalData;
        
        Impl();
        
        PerformanceAnalysis analyzeProfileInternal(const std::string& profileName) const;
        std::vector<AnalysisResult> analyzeAllProfilesInternal() const;
        ComparativeAnalysis compareProfilesInternal(const std::string& profile1, const std::string& profile2) const;
        double predictPerformanceInternal(const std::string& profileName,
                                          const std::map<std::string, double>& loadFactors) const;
        std::vector<std::string> detectBottlenecksInternal(const PerformanceProfile& profile) const;
        std::map<std::string, double> identifyImprovementAreasInternal(const PerformanceProfile& profile) const;
        
        bool exportAnalysisInternal(const std::string& filename) const;
        std::string getAnalysisAsJSONInternal() const;
    };
    
    // PerformanceMonitor implementation details
    class PerformanceMonitor::Impl {
    public:
        bool isMonitoring;
        PerformanceProfile currentProfile;
        PerformanceMetricsCollector metricsCollector;
        PerformanceAlertSystem alertSystem;
        PerformanceAnalysisEngine analysisEngine;
        benchmarking::BenchmarkRunner benchmarkRunner;
        benchmarking::Profiler profiler;
        
        PerformanceConfig config;
        
        // Integration
        const Brain* brain;
        const AgentBrain* agent;
        const SimpleWorld* world;
        const VisualizationInterface* viz;
        
        // Real-time monitoring
        bool realTimeMonitoring;
        std::vector<std::string> realTimeMetrics;
        double realTimeInterval;
        
        Impl();
        ~Impl();
        
        void startMonitoringInternal();
        void stopMonitoringInternal();
        bool isMonitoringInternal() const;
        
        void setConfigurationInternal(const PerformanceConfig& config);
        PerformanceConfig getConfigurationInternal() const;
        
        void integrateWithBrainInternal(const Brain& brain);
        void integrateWithAgentInternal(const AgentBrain& agent);
        void integrateWithWorldInternal(const SimpleWorld& world);
        void integrateWithVisualizationInternal(const VisualizationInterface& viz);
        
        void setMetricsCollectorInternal(const PerformanceMetricsCollector& collector);
        void setAlertSystemInternal(const PerformanceAlertSystem& alertSystem);
        void setAnalysisEngineInternal(const PerformanceAnalysisEngine& analysisEngine);
        
        void setBenchmarkRunnerInternal(const benchmarking::BenchmarkRunner& benchmarkRunner);
        void setProfilerInternal(const benchmarking::Profiler& profiler);
        
        void startRealTimeMonitoringInternal(const std::vector<std::string>& metrics, double interval);
        void stopRealTimeMonitoringInternal();
        bool isRealTimeMonitoringInternal() const;
        std::map<std::string, double> getCurrentMetricsInternal() const;
        
        bool exportPerformanceDataInternal(const std::string& filename) const;
        bool importPerformanceDataInternal(const std::string& filename);
        
        bool isHealthyInternal() const;
        std::string getHealthReportInternal() const;
        
        void cleanupInternal();
    };
}

// Implementation of PerformanceProfile methods
nlm::PerformanceProfile::PerformanceProfile(const std::string& name) : pImpl(std::make_unique<Impl>(name, PerformanceConfig())) {}

void nlm::PerformanceProfile::addMeasurement(const std::string& metricName, double value, MeasurementUnit unit) {
    pImpl->addMeasurementInternal(metricName, value, unit);
}

void nlm::PerformanceProfile::addTimedMeasurement(const std::string& metricName, 
                                                 const std::function<void()>& operation) {
    pImpl->addTimedMeasurementInternal(metricName, operation);
}

std::vector<nlm::PerformanceMeasurement> nlm::PerformanceProfile::getMeasurements() const {
    return pImpl->getMeasurementsInternal();
}

std::vector<nlm::PerformanceMeasurement> nlm::PerformanceProfile::getMeasurements(const std::string& metricName) const {
    return pImpl->getMeasurementsInternal(metricName);
}

// Additional method implementations...

// Implementation of PerformanceMonitor
nlm::PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

nlm::PerformanceMonitor::~PerformanceMonitor() = default;

void nlm::PerformanceMonitor::startMonitoring() {
    pImpl->startMonitoringInternal();
}

void nlm::PerformanceMonitor::stopMonitoring() {
    pImpl->stopMonitoringInternal();
}

bool nlm::PerformanceMonitor::isMonitoring() const {
    return pImpl->isMonitoringInternal();
}

// Additional PerformanceMonitor methods...

// Global performance monitor instance
nlm::PerformanceMonitor& nlm::getPerformanceMonitor() {
    static PerformanceMonitor monitor;
    return monitor;
}

// Benchmarking namespace implementations
namespace nlm::benchmarking {
    BenchmarkRunner::BenchmarkRunner() : pImpl(std::make_unique<Impl>()) {}
    
    void BenchmarkRunner::addBenchmark(const std::string& name, const std::function<void()>& operation) {
        pImpl->benchmarks[name] = operation;
    }
    
    std::vector<PerformanceProfile::BenchmarkResult> BenchmarkRunner::runAllBenchmarks(size_t iterations) {
        std::vector<PerformanceProfile::BenchmarkResult> results;
        for (const auto& pair : pImpl->benchmarks) {
            results.push_back(runBenchmark(pair.first, iterations));
        }
        return results;
    }
    
    PerformanceProfile::BenchmarkResult BenchmarkRunner::runBenchmark(const std::string& name, 
                                                                       size_t iterations) {
        return pImpl->runBenchmarkInternal(name, pImpl->benchmarks[name], iterations);
    }
    
    // Additional BenchmarkRunner methods...
    
    Profiler::Profiler() : pImpl(std::make_unique<Impl>()) {}
    
    void Profiler::startProfiling(const std::string& profileName) {
        pImpl->currentProfileName = profileName;
    }
    
    PerformanceProfile Profiler::stopProfiling() {
        PerformanceProfile profile = pImpl->profiles[pImpl->currentProfileName];
        pImpl->profiles.erase(pImpl->currentProfileName);
        pImpl->currentProfileName.clear();
        return profile;
    }
    
    // Additional Profiler methods...
}

// PerformanceAnalysisEngine implementation
nlm::PerformanceAnalysisEngine::PerformanceAnalysisEngine() : pImpl(std::make_unique<Impl>()) {}

// Additional PerformanceAnalysisEngine methods...

// Convenience namespace functions
namespace nlm::performance {
    PerformanceProfile::BenchmarkResult benchmark(const std::string& name,
                                                 const std::function<void()>& operation,
                                                 size_t iterations) {
        benchmarking::BenchmarkRunner runner;
        runner.addBenchmark(name, operation);
        return runner.runBenchmark(name, iterations);
    }
    
    PerformanceProfile profile(const std::string& name,
                              const std::function<void()>& operation,
                              double timeout) {
        benchmarking::Profiler profiler;
        profiler.startProfiling(name);
        operation();
        return profiler.stopProfiling();
    }
    
    // Additional convenience functions...
}

} // namespace nlm

// Test implementations for performance monitoring
namespace test_performance {
    
    void testPerformanceMonitorBasics() {
        std::cout << "    Testing performance monitor basics..." << std::endl;
        
        nlm::PerformanceMonitor& monitor = nlm::getPerformanceMonitor();
        
        // Start monitoring
        monitor.startMonitoring();
        assert(monitor.isMonitoring());
        
        // Get current profile
        auto profile = monitor.getCurrentProfile();
        assert(profile.getName() == "CurrentProfile");
        
        // Stop monitoring
        monitor.stopMonitoring();
        assert(!monitor.isMonitoring());
        
        std::cout << "    testPerformanceMonitorBasics passed" << std::endl;
    }
    
    void testPerformanceMetricsCollector() {
        std::cout << "    Testing performance metrics collector..." << std::endl;
        
        nlm::PerformanceMetricsCollector collector;
        
        // Start collection
        collector.startCollection();
        assert(collector.isCollecting());
        
        // Add metrics
        collector.addMetric("test_metric", 42.0, nlm::MeasurementUnit::Count);
        collector.addMetric("performance_metric", 0.95, nlm::MeasurementUnit::Percent);
        
        // Get metrics
        auto metrics = collector.getMetrics("test_metric");
        assert(metrics.size() == 1);
        assert(metrics[0].value == 42.0);
        
        auto perfMetrics = collector.getMetrics("performance_metric");
        assert(perfMetrics.size() == 1);
        assert(test_utils::nearEqual(perfMetrics[0].value, 0.95));
        
        // Stop collection
        collector.stopCollection();
        assert(!collector.isCollecting());
        
        std::cout << "    testPerformanceMetricsCollector passed" << std::endl;
    }
    
    void testPerformanceAlertSystem() {
        std::cout << "    Testing performance alert system..." << std::endl;
        
        nlm::PerformanceAlertSystem alertSystem;
        
        // Add alert
        alertSystem.addAlert("HighCPU", "cpu_usage", 90.0, true, "high");
        
        // Get alerts
        auto alerts = alertSystem.getAlerts();
        assert(alerts.size() == 1);
        assert(alerts[0].name == "HighCPU");
        
        // Check alerts (not triggered)
        auto triggered = alertSystem.checkAlerts({{"cpu_usage", 85.0}});
        assert(triggered.empty()); // 85.0 < 90.0, not triggered
        
        // Check alerts (triggered)
        triggered = alertSystem.checkAlerts({{"cpu_usage", 95.0}});
        assert(triggered.size() == 1);
        assert(triggered[0].name == "HighCPU");
        
        // Acknowledge alert
        alertSystem.acknowledgeAlert("HighCPU");
        
        std::cout << "    testPerformanceAlertSystem passed" << std::endl;
    }
    
    void testBenchmarkRunner() {
        std::cout << "    Testing benchmark runner..." << std::endl;
        
        nlm::benchmarking::BenchmarkRunner runner;
        
        // Add benchmark
        int counter = 0;
        runner.addBenchmark("test_bench", [&]() { counter++; });
        
        // Run benchmark
        auto result = runner.runBenchmark("test_bench", 100);
        assert(result.benchmarkName == "test_bench");
        assert(result.iterations == 100);
        assert(result.executionTime > 0.0);
        
        // Verify counter was incremented
        assert(counter == 100);
        
        std::cout << "    testBenchmarkRunner passed" << std::endl;
    }
    
    void testProfiler() {
        std::cout << "    Testing profiler..." << std::endl;
        
        nlm::benchmarking::Profiler profiler;
        
        // Profile a function
        int value = 0;
        profiler.startProfiling("sum_test");
        for (int i = 0; i < 1000; ++i) {
            value += i;
        }
        profiler.stopProfiling();
        
        // Get profile
        auto profile = profiler.getProfile("sum_test");
        assert(profile.getName() == "sum_test");
        assert(profile.getMeasurementCount() > 0);
        
        std::cout << "    testProfiler passed" << std::endl;
    }
    
    void testPerformanceAnalysis() {
        std::cout << "    Testing performance analysis..." << std::endl;
        
        nlm::PerformanceAnalysisEngine analyzer;
        
        // Create test profiles
        nlm::PerformanceProfile profile1("test_profile_1");
        profile1.addMeasurement("metric1", 10.0, nlm::MeasurementUnit::Milliseconds);
        profile1.addMeasurement("metric2", 20.0, nlm::MeasurementUnit::Percent);
        
        nlm::PerformanceProfile profile2("test_profile_2");
        profile2.addMeasurement("metric1", 15.0, nlm::MeasurementUnit::Milliseconds);
        profile2.addMeasurement("metric2", 25.0, nlm::MeasurementUnit::Percent);
        
        // Add profiles to analyzer
        analyzer.addProfile(profile1);
        analyzer.addProfile(profile2);
        
        // Analyze profiles
        auto analysis1 = analyzer.analyzeProfile("test_profile_1");
        assert(analysis1.profileName == "test_profile_1");
        assert(analysis1.keyMetrics.find("metric1_mean") != analysis1.keyMetrics.end());
        
        auto analysis2 = analyzer.analyzeProfile("test_profile_2");
        assert(analysis2.profileName == "test_profile_2");
        
        // Compare profiles
        auto comparison = analyzer.compareProfiles("test_profile_1", "test_profile_2");
        assert(!comparison.differences.empty());
        
        std::cout << "    testPerformanceAnalysis passed" << std::endl;
    }
    
    void testRealTimeMonitoring() {
        std::cout << "    Testing real-time monitoring..." << std::endl;
        
        nlm::PerformanceMonitor& monitor = nlm::getPerformanceMonitor();
        
        // Start real-time monitoring
        monitor.startRealTimeMonitoring({"cpu_usage", "memory_usage", "performance_metric"}, 0.5);
        assert(monitor.isRealTimeMonitoring());
        
        // Get current metrics
        auto metrics = monitor.getCurrentMetrics();
        assert(metrics.find("cpu_usage") != metrics.end());
        
        // Stop real-time monitoring
        monitor.stopRealTimeMonitoring();
        assert(!monitor.isRealTimeMonitoring());
        
        std::cout << "    testRealTimeMonitoring passed" << std::endl;
    }
    
    void testPerformanceExportImport() {
        std::cout << "    Testing performance export/import..." << std::endl;
        
        nlm::PerformanceMonitor& monitor = nlm::getPerformanceMonitor();
        
        // Create test profile
        nlm::PerformanceProfile profile("test_export_profile");
        profile.addMeasurement("test_metric", 42.0, nlm::MeasurementUnit::Count);
        profile.addMeasurement("another_metric", 3.14, nlm::MeasurementUnit::Percent);
        
        // Export profile
        bool exportSuccess = profile.exportProfile("test_export.bin");
        assert(exportSuccess);
        
        // Import profile
        nlm::PerformanceProfile importedProfile("imported_profile");
        bool importSuccess = importedProfile.importProfile("test_export.bin");
        assert(importSuccess);
        
        // Verify imported profile
        auto metrics = importedProfile.getMeasurements();
        assert(metrics.size() == 2);
        
        // Cleanup
        std::remove("test_export.bin");
        
        std::cout << "    testPerformanceExportImport passed" << std::endl;
    }
    
    void testAlertSystemExportImport() {
        std::cout << "    Testing alert system export/import..." << std::endl;
        
        nlm::PerformanceAlertSystem alertSystem;
        
        // Add alerts
        alertSystem.addAlert("Alert1", "metric1", 100.0, true);
        alertSystem.addAlert("Alert2", "metric2", 50.0, false);
        
        // Export alerts
        bool exportSuccess = alertSystem.exportAlerts("test_alerts.json");
        assert(exportSuccess);
        
        // Create new alert system
        nlm::PerformanceAlertSystem newAlertSystem;
        
        // Import alerts
        bool importSuccess = newAlertSystem.importAlerts("test_alerts.json");
        assert(importSuccess);
        
        // Verify imported alerts
        auto alerts = newAlertSystem.getAlerts();
        assert(alerts.size() == 2);
        
        // Cleanup
        std::remove("test_alerts.json");
        
        std::cout << "    testAlertSystemExportImport passed" << std::endl;
    }
}

// Main test runner for performance monitoring
int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "NLM PERFORMANCE MONITORING TEST SUITE" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    try {
        // Performance monitoring tests
        std::cout << "\n=== PERFORMANCE MONITORING TESTS ===" << std::endl;
        test_performance::testPerformanceMonitorBasics(); passed++;
        test_performance::testPerformanceMetricsCollector(); passed++;
        test_performance::testPerformanceAlertSystem(); passed++;
        test_performance::testBenchmarkRunner(); passed++;
        test_performance::testProfiler(); passed++;
        test_performance::testPerformanceAnalysis(); passed++;
        test_performance::testRealTimeMonitoring(); passed++;
        test_performance::testPerformanceExportImport(); passed++;
        test_performance::testAlertSystemExportImport(); passed++;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Performance test suite failed with exception: " << e.what() << std::endl;
        failed++;
    } catch (...) {
        std::cerr << "ERROR: Performance test suite failed with unknown exception" << std::endl;
        failed++;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "PERFORMANCE TEST SUMMARY" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Tests passed: " << passed << std::endl;
    std::cout << "Tests failed: " << failed << std::endl;
    std::cout << "Total tests: " << (passed + failed) << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    if (failed > 0) {
        std::cout << "FAILURE: " << failed << " performance tests failed" << std::endl;
        return 1;
    }
    
    std::cout << "SUCCESS: All performance tests passed!" << std::endl;
    return 0;
}