#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/Experiment.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <chrono>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

class ExperimentMonitor {
public:
    struct ExperimentMetrics {
        SimulationStep step;
        Timestamp timestamp;
        float learningRate;
        float stabilityScore;
        float explorationScore;
        float predictionError;
        float rewardSignal;
        float spikeRate;
        float synapticActivity;
        float memoryUtilization;
        float energyConsumption;
        std::vector<float> weightDeltas;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        double elapsedTime;
    };

    struct ConvergenceInfo {
        bool converged;
        float convergenceThreshold;
        SimulationStep convergenceStep;
        float finalValue;
        std::string reason;
    };

    struct PerformanceStats {
        double totalTime;
        double cpuTime;
        double memoryUsage;
        SimulationStep stepsPerSecond;
        float operationsPerSecond;
    };

    using MetricCallback = std::function<void(const ExperimentMetrics&)';
    using ConvergenceCallback = std::function<void(const ConvergenceInfo&)';
    using ProgressCallback = std::function<void(float progress)>;

    ExperimentMonitor(std::shared_ptr<Brain> brain, std::shared_ptr<LearningExperiment> experiment);
    ~ExperimentMonitor() = default;

    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_; }

    // Metric collection
    ExperimentMetrics collectMetrics(SimulationStep step, Timestamp timestamp);
    void processMetrics(const ExperimentMetrics& metrics);

    // Convergence detection
    ConvergenceInfo detectConvergence(const std::vector<ExperimentMetrics>& metrics) const;
    bool checkConvergence(const ExperimentMetrics& current, const ExperimentMetrics& previous) const;

    // Progress tracking
    float calculateProgress(SimulationStep step, SimulationStep maxSteps) const;
    std::string getProgressStatus() const;

    // Performance statistics
    PerformanceStats calculatePerformanceStats() const;
    void resetPerformanceStats();

    // Callbacks
    void setMetricCallback(MetricCallback callback) { metricCallback_ = std::move(callback); }
    void setConvergenceCallback(ConvergenceCallback callback) { convergenceCallback_ = std::move(callback); }
    void setProgressCallback(ProgressCallback callback) { progressCallback_ = std::move(callback); }

    // Results and analysis
    std::vector<ExperimentMetrics> getMetricHistory() const { return metricHistory_; }
    ConvergenceInfo getConvergenceInfo() const { return convergenceInfo_; }
    PerformanceStats getPerformanceStats() const { return performanceStats_; }

    // Analysis utilities
    float calculateLearningCurve(SimulationStep startStep, SimulationStep endStep) const;
    float calculateStability(const std::vector<float>& values) const;
    float calculateExploration(const std::vector<ExperimentMetrics>& metrics) const;
    std::string generateReport() const;

    // Configuration
    void setConvergenceThreshold(float threshold) { convergenceThreshold_ = threshold; }
    void setStabilityWindow(size_t window) { stabilityWindow_ = window; }
    void setMetricCollectionInterval(SimulationStep interval) { metricCollectionInterval_ = interval; }
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }

    // Statistical analysis
    struct StatisticalSummary {
        float mean;
        float median;
        float standardDeviation;
        float min;
        float max;
        float variance;
        float percentile95;
        float percentile99;
    };

    StatisticalSummary calculateStatistics(const std::vector<float>& values) const;

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<LearningExperiment> experiment_;
    
    bool monitoring_;
    SimulationStep monitoringStep_;
    SimulationStep metricCollectionInterval_;
    size_t stabilityWindow_;
    float convergenceThreshold_;
    size_t maxHistorySize_;
    
    std::vector<ExperimentMetrics> metricHistory_;
    ConvergenceInfo convergenceInfo_;
    PerformanceStats performanceStats_;
    
    MetricCallback metricCallback_;
    ConvergenceCallback convergenceCallback_;
    ProgressCallback progressCallback_;
    
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point lastMetricCollection_;
    
    // For convergence detection
    SimulationStep lastConvergenceCheck_;
    float previousLearningRate_;
    float previousStabilityScore_;
    float previousExplorationScore_;
    
    // Performance measurement
    mutable std::chrono::steady_clock::time_point lastPerformanceMeasure_;
    mutable double totalCPUTime_;
    
    // Statistical tracking
    std::unordered_map<std::string, std::vector<float>> metricBuffers_;
};

// ExperimentSession class for managing complete experiment runs
class ExperimentSession {
public:
    struct SessionConfig {
        std::string name;
        SimulationStep maxSteps;
        float convergenceThreshold;
        float learningRateThreshold;
        bool enableEarlyTermination;
        std::string outputFormat;
        std::string outputPrefix;
        bool verbose;
        std::chrono::milliseconds timeout;
    };

    ExperimentSession(std::shared_ptr<Brain> brain, std::shared_ptr<LearningExperiment> experiment);
    ~ExperimentSession() = default;

    // Session management
    bool run(SimulationStep steps, const SessionConfig& config);
    void pause() { monitoringActive_ = false; }
    void resume() { monitoringActive_ = true; }
    void stop() { monitoringActive_ = false; running_ = false; }
    bool isRunning() const { return running_; }
    bool isPaused() const { return !monitoringActive_ && running_; }

    // Results and reporting
    const std::vector<ExperimentMonitor::ExperimentMetrics>& getMetrics() const { return monitor_->getMetricHistory(); }
    const ExperimentMonitor::ConvergenceInfo& getConvergenceInfo() const { return monitor_->getConvergenceInfo(); }
    const ExperimentMonitor::PerformanceStats& getPerformanceStats() const { return monitor_->getPerformanceStats(); }

    // Callbacks
    void setProgressCallback(ExperimentMonitor::ProgressCallback callback);
    void setConvergenceCallback(ExperimentMonitor::ConvergenceCallback callback);
    void setMetricCallback(ExperimentMonitor::MetricCallback callback);

    // Analysis and reporting
    std::string generateDetailedReport() const;
    std::string generateSummaryReport() const;
    void exportMetricsToCSV(const std::string& filepath) const;
    void exportMetricsToJSON(const std::string& filepath) const;

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<LearningExperiment> experiment_;
    std::unique_ptr<ExperimentMonitor> monitor_;

    bool running_;
    bool monitoringActive_;
    SimulationStep currentStep_;
    SessionConfig config_;
    
    // For timing and progress
    std::chrono::steady_clock::time_point sessionStart_;
    SimulationStep lastProgressUpdate_;
};

} // namespace nlm