// NLM Performance Monitoring System
// Real-time performance monitoring and metrics collection for the NLM artificial brain

#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/Metrics.hpp"
#include <chrono>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <mutex>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>

namespace nlm {

// Performance metrics structure
struct PerformanceMetrics {
    // Brain performance metrics
    double brainMemoryUsage = 0.0;          // MB
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t firingNeuronCount = 0;
    double averageFiringRate = 0.0;
    double simulationSpeed = 0.0;          // steps/second
    double memoryEfficiency = 0.0;         // synapses/neuron ratio
    
    // System resource metrics
    double cpuUsage = 0.0;                  // %
    double memoryUsage = 0.0;              // % of system memory
    double gpuUsage = 0.0;                  // % (if GPU available)
    size_t threadCount = 0;
    
    // Simulation metrics
    size_t simulationStep = 0;
    double wallClockTime = 0.0;            // seconds
    double simulationTime = 0.0;           // seconds (simulated time)
    double timestep = 0.0;
    
    // Cognitive performance metrics
    float workingMemoryUsage = 0.0;        // Active traces
    float episodicMemoryUsage = 0.0;      // Episodes stored
    float predictionAccuracy = 0.0;        // Error rate
    float noveltyLevel = 0.0;              // Novelty detection
    float curiosityLevel = 0.0;            // Exploration drive
    float dopamineLevel = 0.0;             // Neuromodulation
    
    // Bottleneck detection
    std::vector<std::string> detectedBottlenecks;
    bool hasPerformanceIssues = false;
    
    // Timestamps
    uint64_t timestamp = 0;
    std::chrono::steady_clock::time_point lastUpdate;
};

// System resource monitor
class SystemResourceMonitor {
public:
    SystemResourceMonitor();
    ~SystemResourceMonitor();
    
    void update();
    const PerformanceMetrics& getCurrentMetrics() const { return currentMetrics_; }
    
    // Configuration
    void setMonitoringInterval(double interval) { monitoringInterval_ = interval; }
    void enableCPUMonitoring(bool enable) { monitorCPU_ = enable; }
    void enableMemoryMonitoring(bool enable) { monitorMemory_ = enable; }
    
private:
    double monitoringInterval_;
    bool monitorCPU_;
    bool monitorMemory_;
    bool monitorGPU_;
    PerformanceMetrics currentMetrics_;
    
    void updateCPUUsage();
    void updateMemoryUsage();
    void updateGPUUsage();
    void updateThreadCount();
};

// Brain performance monitor
class BrainPerformanceMonitor {
public:
    BrainPerformanceMonitor();
    ~BrainPerformanceMonitor();
    
    void initialize(std::shared_ptr<Brain> brain);
    void update(double timestep, size_t step);
    
    const PerformanceMetrics& getCurrentMetrics() const { return currentMetrics_; }
    
    // Cognitive metrics
    void updateCognitiveMetrics();
    void checkMemorySystems();
    void checkNeuromodulation();
    void checkPredictionSystem();
    void checkDevelopmentSystem();
    
    // Bottleneck detection
    void detectPerformanceBottlenecks();
    void analyzeMemoryUsage();
    void analyzeConnectivity();
    
    // Configuration
    void setRealTimeMode(bool enable) { realTimeMode_ = enable; }
    void setMetricCollectionInterval(double interval) { metricInterval_ = interval; }
    void addMetricCollector(const std::string& name, std::function<double()> collector);
    
private:
    std::shared_ptr<Brain> brain_;
    PerformanceMetrics currentMetrics_;
    bool realTimeMode_;
    double metricInterval_;
    std::chrono::steady_clock::time_point lastMetricUpdate_;
    
    std::unordered_map<std::string, std::function<double()>> metricCollectors_;
    std::mutex metricsMutex_;
    
    // Historical data
    std::vector<PerformanceMetrics> metricHistory_;
};

// Experiment progress tracker
class ExperimentProgressTracker {
public:
    ExperimentProgressTracker();
    ~ExperimentProgressTracker();
    
    void startExperiment(const std::string& name);
    void updateProgress(double progress, const std::string& status = "");
    void setCurrentStage(const std::string& stage);
    void recordMetric(const std::string& name, double value);
    
    bool isRunning() const { return isRunning_; }
    double getProgress() const { return progress_; }
    std::string getCurrentStatus() const { return currentStatus_; }
    const Metrics& getMetrics() const { return metrics_; }
    
private:
    bool isRunning_;
    double progress_;
    std::string currentStatus_;
    std::string currentStage_;
    std::string experimentName_;
    std::chrono::steady_clock::time_point startTime_;
    Metrics metrics_;
    std::mutex progressMutex_;
};

// Performance alert system
class PerformanceAlertSystem {
public:
    PerformanceAlertSystem();
    ~PerformanceAlertSystem();
    
    void configure(const std::string& configFile = "");
    void checkAlerts(const PerformanceMetrics& metrics);
    
    void addAlert(const std::string& name, double threshold, const std::string& severity,
                  const std::string& message, std::function<bool(const PerformanceMetrics&)> condition);
    void enableAlert(const std::string& name, bool enable);
    void logAlert(const std::string& name, const PerformanceMetrics& metrics);
    
    std::vector<std::string> getActiveAlerts() const;
    
private:
    struct Alert {
        std::string name;
        double threshold;
        std::string severity; // "INFO", "WARNING", "ERROR", "CRITICAL"
        std::string message;
        bool enabled;
        std::function<bool(const PerformanceMetrics&)> condition;
        std::chrono::steady_clock::time_point lastTriggered;
        int triggerCount;
    };
    
    std::vector<Alert> alerts_;
    std::mutex alertsMutex_;
    std::ofstream alertLog_;
};

// Performance analysis engine
class PerformanceAnalysisEngine {
public:
    PerformanceAnalysisEngine();
    ~PerformanceAnalysisEngine();
    
    void analyze(const PerformanceMetrics& current, const std::vector<PerformanceMetrics>& history);
    void generateReport(const std::string& outputFile = "");
    void compareWithBaseline(const std::vector<PerformanceMetrics>& baseline);
    
    // Analysis types
    struct TrendAnalysis {
        std::string metricName;
        double currentValue;
        double trend; // slope
        double confidence; // 0-1
        std::string direction; // "increasing", "decreasing", "stable"
        std::string recommendation;
    };
    
    struct BottleneckAnalysis {
        std::string component;
        std::string bottleneckType;
        double impactScore;
        double severity; // 0-1
        std::string suggestedFix;
    };
    
    struct RegressionAnalysis {
        std::string regressionType;
        double pValue;
        bool isSignificant;
        double effectSize;
        std::string description;
    };
    
    std::vector<TrendAnalysis> getTrendAnalysis() const { return trendAnalysis_; }
    std::vector<BottleneckAnalysis> getBottleneckAnalysis() const { return bottleneckAnalysis_; }
    std::vector<RegressionAnalysis> getRegressionAnalysis() const { return regressionAnalysis_; }
    
private:
    std::vector<TrendAnalysis> trendAnalysis_;
    std::vector<BottleneckAnalysis> bottleneckAnalysis_;
    std::vector<RegressionAnalysis> regressionAnalysis_;
    std::mutex analysisMutex_;
};

// Main performance monitor class
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    void initialize(std::shared_ptr<Config> config);
    void startMonitoring();
    void stopMonitoring();
    
    void updateBrainMetrics(std::shared_ptr<Brain> brain, double timestep, size_t step);
    void updateSystemMetrics();
    
    const PerformanceMetrics& getCurrentMetrics() const { return currentMetrics_; }
    
    // Progress tracking
    void startExperiment(const std::string& name);
    void updateExperimentProgress(double progress, const std::string& status = "");
    
    // Alerting and analysis
    void checkAlerts();
    void performAnalysis();
    void generateReports(const std::string& outputDirectory = "performance_reports");
    
    // Configuration
    void configure(const std::string& configFile);
    
    // Access to sub-components
    PerformanceAlertSystem* getPerformanceAlertSystem() { return alertSystem_.get(); }
    
private:
    std::unique_ptr<SystemResourceMonitor> systemMonitor_;
    std::unique_ptr<BrainPerformanceMonitor> brainMonitor_;
    std::unique_ptr<ExperimentProgressTracker> progressTracker_;
    std::unique_ptr<PerformanceAlertSystem> alertSystem_;
    std::unique_ptr<PerformanceAnalysisEngine> analysisEngine_;
    
    PerformanceMetrics currentMetrics_;
    bool monitoringActive_;
    std::thread monitoringThread_;
    
    std::shared_ptr<Config> config_;
    std::mutex monitorMutex_;
    
    void monitoringLoop();
};

} // namespace nlm

