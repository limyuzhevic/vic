#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <chrono>
#include <optional>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <set>
#include <deque>
#include <fstream>
#include <iostream>
#include "Performance.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Advanced performance analysis types
enum class PerformanceAnalysisType {
    Bottleneck,
    Regression,
    Optimization,
    Comparative,
    Predictive,
    SystemWide
};

enum class PerformanceMetricType {
    Allocation,
    Deallocation,
    LockContention,
    MemoryUsage,
    CPUUsage,
    SIMDEfficiency,
    Throughput,
    Latency,
    CacheHitRate,
    ThreadUtilization,
    IOWait
};

enum class PerformanceTrend {
    Improving,
    Degrading,
    Stable,
    Volatile,
    Unknown
};

// Performance regression detection
struct PerformanceRegression {
    std::string metricName;
    double baselineValue;
    double currentValue;
    double regressionPercentage;
    PerformanceTrend trend;
    std::chrono::high_resolution_clock::time_point detectedTime;
    std::string regressionType;
    std::string severity;
    std::string affectedComponent;
    std::string fixSuggestion;
    
    PerformanceRegression() : baselineValue(0.0), currentValue(0.0), 
                             regressionPercentage(0.0), detectedTime(), severity("medium") {}
    
    PerformanceRegression(const std::string& name, double baseline, double current,
                         const std::string& type, const std::string& component)
        : metricName(name), baselineValue(baseline), currentValue(current),
          detectedTime(), regressionType(type), affectedComponent(component),
          severity("medium") {
        regressionPercentage = (current - baseline) / baseline * 100.0;
        
        if (regressionPercentage < -5.0) trend = PerformanceTrend::Improving;
        else if (regressionPercentage < -1.0) trend = PerformanceTrend::Degrading;
        else if (regressionPercentage < 1.0) trend = PerformanceTrend::Stable;
        else trend = PerformanceTrend::Volatile;
        
        if (std::abs(regressionPercentage) > 20.0) severity = "high";
        else if (std::abs(regressionPercentage) > 10.0) severity = "medium";
        else severity = "low";
    }
};

// Performance bottleneck detection
struct PerformanceBottleneck {
    std::string componentName;
    PerformanceMetricType metricType;
    double currentValue;
    double thresholdValue;
    std::string bottleneckType;
    std::chrono::high_resolution_clock::time_point detectedTime;
    std::vector<std::string> affectedFunctions;
    std::string recommendation;
    bool isResolved;
    
    PerformanceBottleneck() : currentValue(0.0), thresholdValue(0.0), isResolved(false) {}
    
    PerformanceBottleneck(const std::string& component, PerformanceMetricType metric,
                          double current, double threshold, const std::string& type)
        : componentName(component), metricType(metric), currentValue(current),
          thresholdValue(threshold), bottleneckType(type), isResolved(false) {
        detectedTime = std::chrono::high_resolution_clock::now();
    }
    
    bool isActive() const {
        return currentValue > thresholdValue;
    }
};

// Performance optimization recommendation
struct PerformanceOptimization {
    std::string optimizationId;
    std::string description;
    std::string component;
    double estimatedGainPercentage;
    double implementationComplexity; // 1-5 scale
    double riskLevel; // 1-5 scale
    std::vector<std::string> requiredChanges;
    std::string implementationGuide;
    std::chrono::high_resolution_clock::time_point suggestedTime;
    bool isImplemented;
    bool isVerified;
    
    PerformanceOptimization() : estimatedGainPercentage(0.0), implementationComplexity(3.0),
                                riskLevel(2.0), isImplemented(false), isVerified(false) {}
    
    PerformanceOptimization(const std::string& id, const std::string& desc, 
                           const std::string& component, double gain, 
                           double complexity, double risk)
        : optimizationId(id), description(desc), component(component),
          estimatedGainPercentage(gain), implementationComplexity(complexity),
          riskLevel(risk), isImplemented(false), isVerified(false) {
        suggestedTime = std::chrono::high_resolution_clock::now();
    }
    
    std::string getPriorityLabel() const {
        if (estimatedGainPercentage > 20.0) return "Critical";
        if (estimatedGainPercentage > 10.0) return "High";
        if (estimatedGainPercentage > 5.0) return "Medium";
        return "Low";
    }
};

// Performance profiling session
struct PerformanceProfileSession {
    std::string sessionId;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::string description;
    std::string scenario; // "startup", "steady_state", "shutdown", "stress_test"
    std::map<std::string, double> baselineMetrics;
    std::map<std::string, double> currentMetrics;
    std::vector<PerfSample> samples;
    std::vector<PerfEvent> events;
    std::vector<PerformanceRegression> regressions;
    std::vector<PerformanceBottleneck> bottlenecks;
    std::vector<PerformanceOptimization> recommendations;
    bool isActive;
    
    PerformanceProfileSession() : isActive(false) {}
    
    PerformanceProfileSession(const std::string& id, const std::string& desc, const std::string& scenario)
        : sessionId(id), description(desc), scenario(scenario), isActive(false) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double getDurationSeconds() const {
        auto end = isActive ? std::chrono::high_resolution_clock::now() : endTime;
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - startTime);
        return static_cast<double>(duration.count());
    }
    
    void end() {
        endTime = std::chrono::high_resolution_clock::now();
        isActive = false;
    }
};

// Comparative performance analysis
struct ComparativePerformanceData {
    std::string identifier;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::map<std::string, double> metrics;
    std::string configuration;
    std::string gitCommit;
    std::string buildType;
    double totalMemoryUsage;
    double peakMemoryUsage;
    double cpuUsage;
    
    ComparativePerformanceData() : totalMemoryUsage(0.0), peakMemoryUsage(0.0), cpuUsage(0.0) {}
    
    ComparativePerformanceData(const std::string& id, const std::map<std::string, double>& metrics,
                              const std::string& config, const std::string& commit)
        : identifier(id), metrics(metrics), configuration(config), gitCommit(commit),
          totalMemoryUsage(0.0), peakMemoryUsage(0.0), cpuUsage(0.0) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Advanced performance analyzer
class AdvancedPerformance {
public:
    AdvancedPerformance();
    ~AdvancedPerformance();
    
    // Disable copying
    AdvancedPerformance(const AdvancedPerformance&) = delete;
    AdvancedPerformance& operator=(const AdvancedPerformance&) = delete;
    
    // Singleton access
    static AdvancedPerformance& getInstance() {
        static AdvancedPerformance instance;
        return instance;
    }
    
    // Performance profiling sessions
    std::string startProfileSession(const std::string& description = "", 
                                   const std::string& scenario = "steady_state");
    void endProfileSession(const std::string& sessionId);
    PerformanceProfileSession* getActiveSession(const std::string& sessionId);
    const PerformanceProfileSession* getActiveSession(const std::string& sessionId) const;
    std::vector<PerformanceProfileSession> getAllSessions() const;
    
    // Real-time performance monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoringActive() const;
    
    // Performance regression detection
    std::vector<PerformanceRegression> detectPerformanceRegressions(
        const std::string& baselineSessionId, const std::string& currentSessionId) const;
    std::vector<PerformanceRegression> detectPerformanceRegressions(
        const PerformanceProfileSession& baseline, const PerformanceProfileSession& current) const;
    
    // Bottleneck detection
    std::vector<PerformanceBottleneck> detectPerformanceBottlenecks(
        const PerformanceProfileSession& session) const;
    std::vector<PerformanceBottleneck> detectPerformanceBottlenecks() const;
    
    // Performance optimization recommendations
    std::vector<PerformanceOptimization> generateOptimizationRecommendations(
        const PerformanceProfileSession& session) const;
    std::vector<PerformanceOptimization> generateOptimizationRecommendations() const;
    
    // Performance comparison and analysis
    ComparativePerformanceData createComparativeDataPoint(const std::string& identifier,
                                                         const std::map<std::string, double>& metrics,
                                                         const Config& config = Config());
    std::vector<ComparativePerformanceData> getComparativeData() const;
    void clearComparativeData();
    
    // Performance regression testing
    bool runRegressionTest(const std::string& testName, const std::function<void()>& testFunction,
                         const std::map<std::string, double>& baselineMetrics);
    bool runRegressionTest(const std::string& testName, const std::function<void()>& testFunction,
                         const Config& baselineConfig);
    std::vector<std::string> getRegressionTestResults() const;
    void clearRegressionTestResults();
    
    // Performance analysis
    PerformanceAnalysisType analyzePerformanceType(const PerformanceProfileSession& session) const;
    PerformanceTrend analyzeMetricTrend(const std::vector<double>& values) const;
    double calculateMetricChangeRate(const std::vector<double>& values) const;
    
    // Configuration integration
    void setPerformanceConfig(const Config& config);
    Config getPerformanceConfig() const;
    
    // Export and reporting
    std::string exportSessionToJSON(const std::string& sessionId) const;
    std::string exportSessionToCSV(const std::string& sessionId) const;
    std::string generatePerformanceReport(const std::string& sessionId) const;
    std::string generatePerformanceReport() const;
    
    // Performance validation
    bool validatePerformanceAgainstThresholds(const PerformanceProfileSession& session) const;
    bool validatePerformanceAgainstThresholds() const;
    std::vector<std::string> getPerformanceValidationErrors() const;
    
    // Automated optimization
    bool runAutomatedOptimization(const std::string& targetSessionId);
    bool runAutomatedOptimization();
    bool applyOptimization(const PerformanceOptimization& optimization);
    
    // Performance analysis helpers
    double calculateThroughput(const PerfSample& sample) const;
    double calculateMemoryEfficiency(const PerfSample& sample) const;
    double calculateSIMDEfficiency(const PerfSample& sample) const;
    double calculateCacheEfficiency(const PerfSample& sample) const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Statistics and monitoring
    struct PerformanceStats {
        size_t totalProfiles{0};
        size_t activeProfiles{0};
        size_t regressionTestsPassed{0};
        size_t regressionTestsFailed{0};
        uint64_t totalProfileTime{0};
        double averageProfileThroughput{0.0};
        std::map<std::string, size_t> regressionCountsByType;
        std::map<std::string, size_t> bottleneckCountsByComponent;
        std::map<std::string, size_t> optimizationCountsByPriority;
        
        void reset() {
            totalProfiles = activeProfiles = 0;
            regressionTestsPassed = regressionTestsFailed = 0;
            totalProfileTime = 0;
            averageProfileThroughput = 0.0;
            regressionCountsByType.clear();
            bottleneckCountsByComponent.clear();
            optimizationCountsByPriority.clear();
        }
    };
    
    PerformanceStats getStats() const;
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    void setLastError(const std::string& error) { lastError_ = error; }
    
    // Configuration integration helpers
    bool isMetricEnabled(const std::string& metricName) const;
    double getMetricThreshold(const std::string& metricName) const;
    
private:
    // Internal performance analysis
    std::vector<PerfSample> collectPerformanceSamples(size_t maxSamples = 1000) const;
    std::vector<PerfEvent> collectPerformanceEvents(size_t maxEvents = 10000) const;
    std::map<std::string, double> extractMetricsFromSamples(
        const std::vector<PerfSample>& samples) const;
    std::map<std::string, double> extractMetricsFromEvents(
        const std::vector<PerfEvent>& events) const;
    
    // Performance regression detection
    std::vector<PerformanceRegression> compareSessions(
        const PerformanceProfileSession& baseline, 
        const PerformanceProfileSession& current) const;
    std::vector<PerformanceBottleneck> analyzeForBottlenecks(
        const PerformanceProfileSession& session) const;
    std::vector<PerformanceOptimization> generateRecommendations(
        const PerformanceProfileSession& session) const;
    
    // Performance comparison
    ComparativePerformanceData calculateComparativeMetrics(
        const std::vector<ComparativePerformanceData>& dataPoints) const;
    std::vector<std::string> validateComparativeData(
        const ComparativePerformanceData& dataPoint) const;
    
    // Thread synchronization
    mutable std::mutex systemMutex_;
    std::atomic<bool> isLocked_;
    
    // Performance profiling
    std::unordered_map<std::string, PerformanceProfileSession> activeSessions_;
    std::vector<PerformanceProfileSession> completedSessions_;
    mutable std::mutex sessionsMutex_;
    
    // Real-time monitoring
    bool realTimeMonitoringActive_;
    mutable std::mutex monitoringMutex_;
    
    // Performance regression testing
    std::vector<std::string> regressionTestResults_;
    mutable std::mutex regressionMutex_;
    
    // Comparative performance data
    std::vector<ComparativePerformanceData> comparativeData_;
    mutable std::mutex comparativeMutex_;
    
    // Performance optimization
    std::vector<PerformanceOptimization> optimizationHistory_;
    mutable std::mutex optimizationMutex_;
    
    // Configuration
    Config performanceConfig_;
    mutable std::mutex configMutex_;
    
    // Statistics
    PerformanceStats stats_;
    mutable std::mutex statsMutex_;
    
    // Error handling
    std::string lastError_;
    mutable std::mutex errorMutex_;
    
    // Performance thresholds
    std::map<std::string, double> performanceThresholds_;
    mutable std::mutex thresholdsMutex_;
    
    // Performance validation
    std::vector<std::string> validationErrors_;
    mutable std::mutex validationMutex_;
};

// Performance analysis helper functions
inline PerformanceTrend analyzeTrend(const std::vector<double>& values) {
    if (values.size() < 3) return PerformanceTrend::Unknown;
    
    std::vector<double> differences;
    for (size_t i = 1; i < values.size(); ++i) {
        differences.push_back(values[i] - values[i-1]);
    }
    
    double avgChange = std::accumulate(differences.begin(), differences.end(), 0.0) / differences.size();
    
    if (avgChange < -0.05) return PerformanceTrend::Improving;
    else if (avgChange > 0.05) return PerformanceTrend::Degrading;
    else if (std::abs(avgChange) < 0.01) return PerformanceTrend::Stable;
    else return PerformanceTrend::Volatile;
}

inline double calculateChangeRate(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    double first = values.front();
    double last = values.back();
    
    if (first == 0.0) return (last > 0.0) ? 1.0 : 0.0;
    
    return (last - first) / first;
}

inline std::string trendToString(PerformanceTrend trend) {
    switch (trend) {
        case PerformanceTrend::Improving: return "Improving";
        case PerformanceTrend::Degrading: return "Degrading";
        case PerformanceTrend::Stable: return "Stable";
        case PerformanceTrend::Volatile: return "Volatile";
        default: return "Unknown";
    }
}

} // namespace nlm