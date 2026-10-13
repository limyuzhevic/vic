#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <functional>

namespace nlm {

// Advanced monitoring and debugging tools for NLM brain
class BrainMonitor {
public:
    struct Metrics {
        // Neural activity metrics
        float averageFiringRate;
        float excitationInhibitionRatio;
        float memoryActivity;
        float predictionAccuracy;
        
        // Neuromodulation metrics
        float dopamineLevel;
        float acetylcholineLevel;
        float norepinephrineLevel;
        float serotoninLevel;
        float noveltyLevel;
        float curiosityLevel;
        float predictionError;
        
        // Learning metrics
        float plasticityRate;
        float learningEfficiency;
        float memoryRetention;
        
        // Cognitive metrics
        float conceptFormationRate;
        float planningConfidence;
        float attentionFocus;
        
        // Development metrics
        float developmentalProgress;
        float behavioralComplexity;
        
        // Simulation metrics
        double simulationTime;
        size_t totalSteps;
        float memoryUsage;
    };
    
    struct PerformanceIndicator {
        std::string name;
        std::string description;
        std::function<float()> metricExtractor;
        float targetValue;
        float thresholdLow;
        float thresholdHigh;
        bool critical;
    };
    
    struct AnomalyAlert {
        enum class Severity { Low, Medium, High, Critical };
        enum class Type { HighFiringRate, LowActivity, MemoryLeak, LearningPlateau, SystemStress };
        
        std::string message;
        Severity severity;
        Type type;
        double timestamp;
        std::string recommendation;
    };
    
    // Configuration
    void setMonitoringInterval(double seconds);
    void setAnomalyThresholds(float firingRateThreshold, float activityThreshold);
    void enableDetailedLogging(bool enable);
    void enablePerformanceIndicators(bool enable);
    void enableAnomalyDetection(bool enable);
    
    // Metric collection
    void collectMetrics(const Metrics& metrics);
    void updateMetricsFromBrain(std::shared_ptr<class Brain> brain);
    Metrics getCurrentMetrics() const;
    
    // Performance indicators
    void addPerformanceIndicator(const PerformanceIndicator& indicator);
    std::vector<PerformanceIndicator> getActiveIndicators() const;
    float getIndicatorScore(const std::string& indicatorName) const;
    
    // Anomaly detection
    std::vector<AnomalyAlert> getRecentAnomalies() const;
    std::vector<AnomalyAlert> getActiveAnomalies() const;
    
    // State tracking
    void resetEpisodeMetrics();
    void startEpisode();
    void endEpisode();
    bool isEpisodeActive() const;
    
    // Historical data
    std::vector<Metrics> getMetricHistory(size_t maxPoints = 1000) const;
    std::vector<AnomalyAlert> getAnomalyHistory(size_t maxPoints = 100) const;
    
    // Analysis
    float getLearningProgress() const;
    float getMemoryRetentionRate() const;
    float getAttentionEfficiency() const;
    float getBehavioralComplexity() const;
    
    // Reporting
    std::string generateReport() const;
    void exportMetricsToFile(const std::string& filename) const;
    
    // Alert callbacks
    using AlertCallback = std::function<void(const AnomalyAlert&)>
;    void setAlertCallback(AlertCallback callback);
    
    // Singleton interface
    static BrainMonitor& getInstance();
    static void initialize(std::shared_ptr<class Brain> brain);
    static void shutdown();
    
private:
    BrainMonitor();
    ~BrainMonitor();
    
    // Disable copying
    BrainMonitor(const BrainMonitor&) = delete;
    BrainMonitor& operator=(const BrainMonitor&) = delete;
    
    // Internal helpers
    void updateAnomalies(const Metrics& metrics);
    bool detectAnomalies(const Metrics& metrics);
    void processAnomalies();
    float computeLearningProgress() const;
    float computeMemoryRetention() const;
    
    // Data storage
    std::vector<Metrics> metricHistory_;
    std::vector<AnomalyAlert> anomalyHistory_;
    Metrics currentMetrics_;
    std::vector<PerformanceIndicator> activeIndicators_;
    
    // Configuration
    double monitoringInterval_;
    bool detailedLoggingEnabled_;
    bool performanceIndicatorsEnabled_;
    bool anomalyDetectionEnabled_;
    float firingRateThreshold_;
    float activityThreshold_;
    
    // State
    bool episodeActive_;
    size_t episodeStartStep_;
    double episodeStartTime_;
    
    // Callbacks
    AlertCallback alertCallback_;
    
    // External brain reference
    std::shared_ptr<class Brain> brain_;
};

} // namespace nlm