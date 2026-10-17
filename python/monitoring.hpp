#pragma once

// Real-time monitoring and visualization for NLM
// Provides live metrics monitoring and streaming, real-time brain state visualization,
// performance profiling, and activity patterns analysis

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <deque>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"

namespace nlm {
namespace python {

// Streaming metrics data
struct StreamingMetric {
    std::string name;
    std::string unit;
    double value;
    double minValue;
    double maxValue;
    double averageValue;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
};

// Real-time visualization data
struct RealTimeVisualizationData {
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> regionalActivity;
    std::vector<std::pair<double, double>> firingRateHistory;
    std::vector<std::vector<double>> connectivityMatrix;
    std::map<std::string, std::vector<double>> neuromodulatorLevels;
    std::map<std::string, std::vector<double>> populationActivity;
    std::vector<std::pair<double, double>> synapticStrengthHistory;
};

// Performance profiling data
struct PerformanceProfile {
    double totalSimulationTime;
    double averageStepTime;
    double minStepTime;
    double maxStepTime;
    std::map<std::string, double> componentTimes;
    std::map<std::string, size_t> componentCallCounts;
    std::vector<std::pair<std::string, double>> sortedComponents;
    std::chrono::system_clock::time_point profileStartTime;
    std::chrono::system_clock::time_point profileEndTime;
};

// Activity pattern analysis results
struct ActivityPatternAnalysis {
    std::chrono::system_clock::time_point analysisTime;
    std::map<std::string, double> regionalCoherence;
    double globalCoherenceIndex;
    std::vector<std::pair<double, double>> burstPatterns;
    std::vector<std::pair<double, double>> silencePatterns;
    std::map<std::string, double> firingRateStatistics;
    std::map<std::string, std::vector<double>> populationFiringRates;
    std::map<std::string, std::vector<double>> neuralFiringRates;
    double totalSpikeCount;
    double averageFiringRate;
    double firingRateCV; // Coefficient of variation
    bool isBurstingActive;
    bool isGammaOscillationsPresent;
    bool isThetaOscillationsPresent;
};

// Monitoring configuration
struct MonitoringConfig {
    bool enableMetricsStreaming;
    double metricsStreamIntervalMs;
    bool enableRealTimeVisualization;
    std::string visualizationBackend;
    int visualizationServerPort;
    bool enablePerformanceProfiling;
    double profilingIntervalSeconds;
    bool enableActivityPatternAnalysis;
    double analysisWindowSeconds;
    bool enableAnomalyDetection;
    double anomalyThreshold;
    std::vector<std::string> customMetrics;
    std::map<std::string, std::variant<int, float, bool, std::string>> metricParameters;
};

// Real-time monitoring and visualization system
class MonitoringSystem {
public:
    // Live metrics monitoring and streaming
    static void initializeMonitoring(const MonitoringConfig& config);
    static void shutdownMonitoring();
    static bool isMonitoringActive();
    
    // Start/stop metrics streaming
    static void startMetricsStreaming(std::shared_ptr<Brain> brain, double intervalMs = 100.0);
    static void stopMetricsStreaming();
    static bool isMetricsStreamingActive();
    static std::vector<StreamingMetric> getCurrentMetrics(std::shared_ptr<Brain> brain);
    static void registerCustomMetric(const std::string& name, 
                                     const std::function<StreamingMetric(std::shared_ptr<Brain>)>& collector);
    static void unregisterMetric(const std::string& name);
    static std::map<std::string, StreamingMetric> getAllMetrics(std::shared_ptr<Brain> brain);
    
    // Real-time brain state visualization
    static RealTimeVisualizationData getBrainStateVisualization(std::shared_ptr<Brain> brain);
    static std::string generateBrainStateVisualizationHTML(const RealTimeVisualizationData& data);
    static std::string generateBrainStateVisualizationJSON(const RealTimeVisualizationData& data);
    static void startVisualizationServer(std::shared_ptr<Brain> brain, int port = 8080);
    static void stopVisualizationServer();
    static bool isVisualizationServerActive();
    static void setVisualizationBackend(const std::string& backend);
    
    // Performance profiling
    static void enableProfiling(std::shared_ptr<Brain> brain, bool enable = true);
    static void startProfiling(std::shared_ptr<Brain> brain);
    static void stopProfiling(std::shared_ptr<Brain> brain);
    static bool isProfilingActive(std::shared_ptr<Brain> brain);
    static PerformanceProfile getPerformanceProfile(std::shared_ptr<Brain> brain);
    static std::vector<std::pair<std::string, double>> getCurrentPerformanceStats(std::shared_ptr<Brain> brain);
    static double getCurrentStepTime(std::shared_ptr<Brain> brain);
    static size_t getCurrentStepCount(std::shared_ptr<Brain> brain);
    
    // Activity patterns analysis
    static void enableActivityAnalysis(std::shared_ptr<Brain> brain, bool enable = true);
    static bool isActivityAnalysisEnabled(std::shared_ptr<Brain> brain);
    static ActivityPatternAnalysis analyzeActivityPatterns(std::shared_ptr<Brain> brain, 
                                                         double windowSeconds = 60.0);
    static std::vector<std::pair<double, double>> getFiringRateHistory(std::shared_ptr<Brain> brain, 
                                                                      int windowSize = 1000);
    static double computeCoherenceIndex(std::shared_ptr<Brain> brain);
    static std::map<std::string, double> analyzeRegionalActivity(std::shared_ptr<Brain> brain);
    static std::vector<std::pair<double, double>> detectBursts(std::shared_ptr<Brain> brain, 
                                                              double threshold = 3.0);
    static std::vector<std::pair<double, double>> detectSilences(std::shared_ptr<Brain> brain, 
                                                                double threshold = 0.5);
    
    // Anomaly detection
    static void enableAnomalyDetection(std::shared_ptr<Brain> brain, bool enable = true);
    static std::vector<std::string> detectAnomalies(std::shared_ptr<Brain> brain);
    static double calculateAnomalyScore(std::shared_ptr<Brain> brain, const std::string& metricName);
    static void setAnomalyThreshold(double threshold);
    static double getAnomalyThreshold();
    
    // Data export and analysis
    static bool exportMetricsToFile(const std::string& filepath, const std::string& format = "json");
    static bool exportMetricsToCSV(const std::string& filepath);
    static bool exportMetricsToJSON(const std::string& filepath);
    static std::shared_ptr<std::vector<StreamingMetric>> getMetricsHistory(size_t historySize = 10000);
    static void clearMetricsHistory();
    static size_t getMetricsHistorySize();
    
    // Configuration management
    static void updateMonitoringConfig(const MonitoringConfig& config);
    static MonitoringConfig getMonitoringConfig();
    static void resetPerformanceProfile(std::shared_ptr<Brain> brain);
    static void clearAllData();
    
    // Utility functions
    static std::string generatePerformanceReport(std::shared_ptr<Brain> brain);
    static std::string generateActivityAnalysisReport(const ActivityPatternAnalysis& analysis);
    static double calculateMetricChangeRate(const std::string& metricName, double timeWindowSeconds = 60.0);
    static std::map<std::string, double> getMetricStatistics(std::shared_ptr<Brain> brain, 
                                                            const std::string& metricName);
    
    // Callbacks and events
    using MetricUpdateCallback = std::function<void(const std::vector<StreamingMetric>&)>;
    using AnomalyDetectedCallback = std::function<void(const std::string& anomaly, double severity)>;
    using VisualizationUpdateCallback = std::function<void(const RealTimeVisualizationData& data)>;
    
    static void addMetricUpdateCallback(const MetricUpdateCallback& callback);
    static void removeMetricUpdateCallback(const MetricUpdateCallback& callback);
    static void addAnomalyDetectedCallback(const AnomalyDetectedCallback& callback);
    static void removeAnomalyDetectedCallback(const AnomalyDetectedCallback& callback);
    static void addVisualizationUpdateCallback(const VisualizationUpdateCallback& callback);
    static void removeVisualizationUpdateCallback(const VisualizationUpdateCallback& callback);
    
    // Singleton instance management
    static MonitoringSystem& getInstance();
    
private:
    MonitoringSystem();
    ~MonitoringSystem();
    MonitoringSystem(const MonitoringSystem&) = delete;
    MonitoringSystem& operator=(const MonitoringSystem&) = delete;
    
    void updateMetrics(std::shared_ptr<Brain> brain);
    void streamMetrics();
    void updateVisualization();
    void profilePerformance(std::shared_ptr<Brain> brain);
    void analyzeActivity(std::shared_ptr<Brain> brain);
    void checkForAnomalies(std::shared_ptr<Brain> brain);
    
    std::atomic<bool> monitoringActive_;
    std::atomic<bool> metricsStreamingActive_;
    std::atomic<bool> visualizationServerActive_;
    std::atomic<bool> profilingActive_;
    std::atomic<bool> activityAnalysisEnabled_;
    std::atomic<bool> anomalyDetectionEnabled_;
    
    MonitoringConfig config_;
    PerformanceProfile currentProfile_;
    std::vector<StreamingMetric> metricsHistory_;
    std::vector<RealTimeVisualizationData> visualizationHistory_;
    std::vector<ActivityPatternAnalysis> activityAnalysisHistory_;
    
    std::thread metricsStreamThread_;
    std::thread visualizationUpdateThread_;
    std::mutex dataMutex_;
    
    std::vector<MetricUpdateCallback> metricUpdateCallbacks_;
    std::vector<AnomalyDetectedCallback> anomalyDetectedCallbacks_;
    std::vector<VisualizationUpdateCallback> visualizationUpdateCallbacks_;
};