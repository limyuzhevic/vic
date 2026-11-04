#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace nlm {

/**
 * Performance metrics collection and reporting
 * Monitors key performance indicators during neural simulation
 */
class PerformanceMetrics {
public:
    // Performance metrics categories
    enum class MetricCategory {
        NeuronActivity,
        SynapticActivity,
        MemoryUsage,
        ProcessingSpeed,
        Plasticity,
        Development,
        Neuromodulation,
        Total
    };
    
    // Individual metric data point
    struct MetricPoint {
        std::string name;
        MetricCategory category;
        double value;
        double timestamp;
        std::unordered_map<std::string, std::string> tags;
    };
    
    PerformanceMetrics();
    ~PerformanceMetrics();
    
    // Disable copying
    PerformanceMetrics(const PerformanceMetrics&) = delete;
    PerformanceMetrics& operator=(const PerformanceMetrics&) = delete;
    
    // Record a new metric value
    void recordMetric(const std::string& name, MetricCategory category, double value, 
                     double timestamp = 0.0, const std::unordered_map<std::string, std::string>& tags = {});
    
    // Record common performance metrics
    void recordNeuronMetrics(size_t neuronCount, size_t activeNeurons, size_t firingNeurons);
    void recordSynapticMetrics(size_t synapseCount, float excitationInhibitionRatio);
    void recordMemoryMetrics(size_t workingMemorySize, size_t episodicMemorySize);
    void recordProcessingMetrics(double processingTime, size_t stepsProcessed);
    void recordPlasticityMetrics(float stdpWeightChange, float hebbianWeightChange);
    void recordDevelopmentMetrics(float developmentStage);
    void recordNeuromodulationMetrics(float dopamineLevel, float curiosityLevel);
    
    // Query metrics
    std::vector<MetricPoint> getMetrics(MetricCategory category) const;
    std::vector<MetricPoint> getMetrics(const std::string& name) const;
    double getAverageValue(const std::string& name, MetricCategory category, 
                          double startTime = 0.0, double endTime = 0.0) const;
    double getMaximumValue(const std::string& name, MetricCategory category,
                          double startTime = 0.0, double endTime = 0.0) const;
    double getMinimumValue(const std::string& name, MetricCategory category,
                          double startTime = 0.0, double endTime = 0.0) const;
    
    // Performance analysis
    struct PerformanceReport {
        double duration;
        size_t totalSteps;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t peakNeuronActivity;
        double peakProcessingTime;
        std::unordered_map<std::string, double> categoryStats;
    };
    
    PerformanceReport generateReport(double startTime = 0.0, double endTime = 0.0) const;
    
    // Configuration
    void setSamplingInterval(double interval) { samplingInterval = interval; }
    void setMaxDataPoints(size_t maxPoints) { maxDataPoints = maxPoints; }
    void enableRealTimeMonitoring(bool enable) { realTimeMonitoring = enable; }
    
    // Reset
    void clear();
    void resetTime(double timestamp = 0.0);
    
    // Statistics
    size_t getMetricCount() const { return metrics.size(); }
    size_t getMetricCount(MetricCategory category) const;
    double getStartTime() const { return startTime; }
    double getEndTime() const { return endTime; }
    
private:
    // Internal data structures
    std::vector<MetricPoint> metrics;
    std::unordered_map<std::string, std::vector<MetricPoint>> metricsByName;
    std::unordered_map<MetricCategory, std::vector<MetricPoint>> metricsByCategory;
    
    // Performance tracking
    double startTime;
    double endTime;
    double currentTime;
    double samplingInterval;
    size_t maxDataPoints;
    bool realTimeMonitoring;
    
    // Thread safety
    mutable std::mutex metricsMutex;
    
    // Helper methods
    void addMetric(const MetricPoint& point);
    void cleanupOldMetrics(double cutoffTime);
    void updateTime(double timestamp);
    
    // Statistical calculations
    static double calculateMean(const std::vector<double>& values);
    static double calculateVariance(const std::vector<double>& values, double mean);
    static double calculateStdDev(const std::vector<double>& values, double mean);
    
    // Friend classes
    friend class BrainMetricsCollector;
    friend class PerformanceProfiler;
};

/**
 * Performance profiler for detailed timing analysis
 */
class PerformanceProfiler {
public:
    struct ProfilePoint {
        std::string operation;
        double duration;
        double timestamp;
        std::unordered_map<std::string, std::string> context;
    };
    
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    // Start/stop profiling
    void startProfile(const std::string& operation, const std::unordered_map<std::string, std::string>& context = {});
    ProfilePoint stopProfile(const std::string& operation);
    
    // Profile helper functions
    template<typename F>
    auto profile(const std::string& operation, F&& func) {
        startProfile(operation);
        auto result = func();
        stopProfile(operation);
        return result;
    }
    
    // Query profiling data
    std::vector<ProfilePoint> getProfile(const std::string& operation) const;
    double getTotalProfileTime(const std::string& operation) const;
    double getAverageProfileTime(const std::string& operation) const;
    
    // Report generation
    std::string generateProfileReport() const;
    void clearProfileData();
    
    // Configuration
    void setProfileEnabled(bool enable) { profileEnabled = enable; }
    void setDetailedProfiling(bool detailed) { detailedProfiling = detailed; }
    
private:
    std::vector<ProfilePoint> profileData;
    std::unordered_map<std::string, std::vector<ProfilePoint>> profileDataByOperation;
    double currentProfileStartTime;
    std::string currentProfileOperation;
    std::unordered_map<std::string, std::string> currentProfileContext;
    bool profileEnabled;
    bool detailedProfiling;
    
    mutable std::mutex profileMutex;
    
    ProfilePoint createProfilePoint(const std::string& operation, double duration,
                                   const std::unordered_map<std::string, std::string>& context);
};

/**
 * Brain metrics collector - collects performance metrics during simulation
 */
class BrainMetricsCollector {
public:
    BrainMetricsCollector();
    ~BrainMetricsCollector();
    
    // Disable copying
    BrainMetricsCollector(const BrainMetricsCollector&) = delete;
    BrainMetricsCollector& operator=(const BrainMetricsCollector&) = delete;
    
    // Initialize with brain reference
    bool initialize(BrainCore* core);
    
    // Collect metrics during simulation
    void collectMetrics(double timestamp);
    void collectNeuronMetrics(BrainCore* core);
    void collectSynapticMetrics(BrainCore* core);
    void collectMemoryMetrics(BrainCore* core);
    void collectProcessingMetrics(BrainCore* core, double deltaTime);
    
    // Performance profiling
    void startProfiling(const std::string& operation);
    void stopProfiling(const std::string& operation);
    
    // Metrics access
    std::shared_ptr<PerformanceMetrics> getPerformanceMetrics() { return metrics; }
    std::shared_ptr<PerformanceProfiler> getPerformanceProfiler() { return profiler; }
    
    // Report generation
    std::string generateFullReport() const;
    
private:
    std::shared_ptr<PerformanceMetrics> metrics;
    std::shared_ptr<PerformanceProfiler> profiler;
    
    BrainCore* core = nullptr;
    double lastCollectTime;
    size_t stepsSinceLastCollect;
};

} // namespace nlm
