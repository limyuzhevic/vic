// NLM Phase 6 Performance Monitoring
// 
// Comprehensive performance monitoring and profiling for Phase 6
// integrated cognitive systems. Tracks brain performance, cognitive system
// efficiency, and learning progress.

#ifndef NLM_PERFORMANCE_MONITORING_HPP
#define NLM_PERFORMANCE_MONITORING_HPP

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <thread>
#include <mutex>

namespace nlm {

// Performance monitoring data structures
struct PerformanceSnapshot {
    double timestamp;
    
    // Brain performance
    struct {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float avgFiringRate;
        float eToIRatio;
    } brain;
    
    // Cognitive system performance
    struct {
        size_t workingMemoryUsage;
        size_t episodicMemoryUsage;
        float predictionAccuracy;
        float conceptStability;
        float attentionFocus;
        float planningConfidence;
    } cognitive;
    
    // Neuromodulation
    struct {
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
    } neuromodulation;
    
    // Learning metrics
    struct {
        float learningRate;
        float retentionRate;
        float generalizationScore;
        std::vector<float> rewardHistory;
        std::vector<float> predictionErrorHistory;
    } learning;
    
    // Performance indicators
    float integrationScore;
    float systemEfficiency;
    float emergenceMetrics;
    std::string integrationStatus;
};

struct PerformanceMetrics {
    // Configuration
    bool enabled;
    bool verboseLogging;
    double samplingInterval;
    size_t maxHistorySize;
    bool trackLearningProgress;
    bool trackSystemIntegration;
    bool trackResourceUtilization;
    
    // Performance tracking
    std::vector<PerformanceSnapshot> history;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastSampleTime;
    
    // System performance
    struct {
        double avgStepTime;
        double maxStepTime;
        double minStepTime;
        size_t totalSteps;
        size_t successfulSteps;
        size_t failedSteps;
    } stepPerformance;
    
    // Cognitive system performance
    struct {
        std::unordered_map<std::string, double> systemMetrics;
        std::unordered_map<std::string, std::vector<double>> systemHistory;
    } cognitive;
    
    // Learning performance
    struct {
        double learningEfficiency;
        double retentionEfficiency;
        double generalizationScore;
        std::vector<double> learningProgress;
    } learning;
    
    // Resource utilization
    struct {
        size_t memoryUsage;
        size_t cpuUsage;
        size_t networkUsage;
        double energyConsumption;
    } resources;
    
    // Integration metrics
    struct {
        double integrationScore;
        double systemCohesion;
        double emergenceScore;
        std::vector<std::string> integrationIssues;
    } integration;
    
    // Runtime info
    struct {
        std::string platform;
        std::string compiler;
        std::string buildType;
        std::vector<std::string> enabledFeatures;
    } runtime;
};

class PerformanceMonitor {
public:
    PerformanceMonitor() : enabled(false), samplingInterval(0.1), 
                          maxHistorySize(10000), verboseLogging(false) {
        setupRuntimeInfo();
    }
    
    ~PerformanceMonitor() {
        if (enabled) {
            generatePerformanceReport();
        }
    }
    
    // Configuration
    void enable() { enabled = true; startTime = std::chrono::steady_clock::now(); }
    void disable() { enabled = false; }
    
    void setSamplingInterval(double interval) { samplingInterval = interval; }
    void setMaxHistorySize(size_t size) { maxHistorySize = size; }
    void setVerboseLogging(bool verbose) { verboseLogging = verbose; }
    
    // Performance tracking
    void recordPerformanceSnapshot(const PerformanceSnapshot& snapshot);
    PerformanceSnapshot getCurrentSnapshot();
    
    // Brain performance tracking
    void recordStepPerformance(double stepTime, bool success);
    void updateBrainMetrics(size_t neurons, size_t synapses, size_t activeNeurons);
    
    // Cognitive system tracking
    void recordCognitiveMetric(const std::string& system, double value);
    void recordCognitiveHistory(const std::string& system, const std::vector<double>& history);
    
    // Learning tracking
    void recordLearningProgress(double efficiency, double retention, double generalization);
    void updateLearningHistory(double step, double reward, double error);
    
    // Integration tracking
    void updateIntegrationScore(double score, const std::string& status);
    void addIntegrationIssue(const std::string& issue);
    
    // Resource tracking
    void updateResourceUsage(size_t memory, size_t cpu, size_t network, double energy);
    
    // Analysis and reporting
    double calculateIntegrationEfficiency() const;
    double calculateLearningEfficiency() const;
    double calculateSystemEfficiency() const;
    
    std::string generatePerformanceReport() const;
    std::string generateIntegrationReport() const;
    std::string generateLearningReport() const;
    std::string generateResourceReport() const;
    
    // Status and statistics
    bool isEnabled() const { return enabled; }
    size_t getHistorySize() const { return metrics.history.size(); }
    double getAverageStepTime() const { return metrics.stepPerformance.avgStepTime; }
    double getIntegrationScore() const { return metrics.integration.integrationScore; }
    std::string getIntegrationStatus() const { return metrics.integration.integrationStatus; }
    
    // Configuration for Phase 6
    void configureForPhase6();
    void setupPhase6Monitoring();
    
    // Performance validation
    bool validatePerformanceThresholds() const;
    std::vector<std::string> getPerformanceIssues() const;
    
    // Export and import
    bool exportPerformanceData(const std::string& filepath) const;
    bool importPerformanceData(const std::string& filepath);
    
private:
    PerformanceMetrics metrics;
    std::mutex monitorMutex;
    
    void setupRuntimeInfo();
    void updateSampling();
    void cleanupOldHistory();
    double calculateAverage(const std::vector<double>& values) const;
    double calculateStdDev(const std::vector<double>& values) const;
    std::string formatPerformanceValue(double value, const std::string& unit) const;
    std::string getCurrentTimestamp() const;
};

// Global performance monitor instance
extern PerformanceMonitor& getPerformanceMonitor();

// Convenience functions for Phase 6 integration
inline void initializePerformanceMonitoring() {
    getPerformanceMonitor().enable();
    getPerformanceMonitor().configureForPhase6();
}

inline void recordBrainPerformance(size_t neurons, size_t synapses, size_t activeNeurons) {
    getPerformanceMonitor().updateBrainMetrics(neurons, synapses, activeNeurons);
}

inline void recordStepPerformance(double stepTime, bool success) {
    getPerformanceMonitor().recordStepPerformance(stepTime, success);
}

inline void recordCognitivePerformance(const std::string& system, double value) {
    getPerformanceMonitor().recordCognitiveMetric(system, value);
}

inline void updateIntegrationProgress(double score, const std::string& status) {
    getPerformanceMonitor().updateIntegrationScore(score, status);
}

inline std::string getPerformanceReport() {
    return getPerformanceMonitor().generatePerformanceReport();
}

inline std::string getIntegrationReport() {
    return getPerformanceMonitor().generateIntegrationReport();
}

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_HPP
