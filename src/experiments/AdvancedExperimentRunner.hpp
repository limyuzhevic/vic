// NLM Advanced Experiment Runner - Enhanced experiment management with advanced features
// Provides sophisticated experiment control, configuration, and analysis capabilities

#pragma once

#include "ExperimentRunner.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/Phase3Experiment.hpp"
#include "../experiments/Phase4Experiment.hpp"
#include "../experiments/Phase5Experiment.hpp"
#include "../experiments/Phase6IntegratedExperiment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <atomic>
#include <chrono>
#include <future>

namespace nlm {

// Experiment modes for advanced control
enum class AdvancedExperimentMode {
    Normal,
    Adaptive,      // Automatically adjusts parameters based on results
    Parallel,      // Runs multiple experiments in parallel
    CrossValidation, // Cross-validation for model evaluation
    HyperparameterOptimization, // Automated hyperparameter tuning
    AblationStudy  // Systematic removal of components to test importance
};

// Experiment configuration template
struct AdvancedExperimentConfig {
    std::string name;
    std::string description;
    AdvancedExperimentMode mode;
    std::string templateName;  // Reference to built-in experiment templates
    std::shared_ptr<Config> config;
    std::vector<std::string> parameterSweep;  // For hyperparameter optimization
    bool enableRealTimeMonitoring = false;
    bool enableAutoRecording = false;
    double timeLimit = -1.0;  // -1 for unlimited
    size_t maxConcurrentExperiments = 1;
    std::string outputFormat;  // csv, json, txt, binary
    bool enableStatisticalAnalysis = true;
    bool enableLearning = true;
    double learningRate = 0.01;
    
    AdvancedExperimentConfig() : mode(AdvancedExperimentMode::Normal), timeLimit(-1.0),
                                 maxConcurrentExperiments(1), learningRate(0.01) {}
};

// Advanced experiment result with enhanced metadata
struct AdvancedExperimentResult {
    std::string experimentName;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double duration;
    size_t stepsExecuted;
    double finalPerformance;
    std::vector<std::pair<std::string, double>> metrics;
    std::vector<std::string> notes;
    bool success;
    std::string errorMessage;
    std::map<std::string, double> parameterSettings;
    std::vector<double> learningCurve;  // Performance over time
    
    AdvancedExperimentResult() : duration(0.0), stepsExecuted(0), finalPerformance(0.0),
                                 success(false), learningCurve() {}
};

// Parallel experiment execution status
struct ParallelExperimentStatus {
    std::string experimentName;
    std::atomic<AdvancedExperimentMode> status;  // Pending, Running, Completed, Failed
    std::atomic<double> progress;  // 0.0 to 1.0
    std::shared_ptr<AdvancedExperimentResult> result;
    std::chrono::system_clock::time_point startTime;
    
    ParallelExperimentStatus() : status(AdvancedExperimentMode::Normal), progress(0.0) {}
};

// Advanced experiment runner with enhanced capabilities
class AdvancedExperimentRunner {
public:
    AdvancedExperimentRunner();
    ~AdvancedExperimentRunner();

    // Configuration
    void setConfig(std::shared_ptr<Config> config);
    std::shared_ptr<Config> getConfig() const;
    
    // Experiment management
    std::shared_ptr<AdvancedExperimentConfig> createExperiment(const std::string& name);
    std::shared_ptr<AdvancedExperimentConfig> createExperiment(const std::string& name, 
                                                              const std::string& templateName);
    bool runExperiment(const std::string& name, std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment);
    bool runExperiment(const std::shared_ptr<AdvancedExperimentConfig>& config,
                      std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment);
    
    // Batch operations
    std::vector<std::shared_ptr<AdvancedExperimentResult>> runBatch(
        const std::vector<std::string>& experimentNames,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate);
    
    // Parallel execution
    std::vector<std::shared_ptr<AdvancedExperimentResult>> runParallel(
        const std::vector<std::string>& experimentNames,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        size_t maxConcurrent = 4);
    
    // Experiment templates
    bool registerTemplate(const std::string& name, std::function<std::shared_ptr<Experiment>()> factory);
    std::shared_ptr<Experiment> createFromTemplate(const std::string& templateName);
    
    // Parameter sweep for hyperparameter optimization
    std::vector<std::shared_ptr<AdvancedExperimentResult>> runParameterSweep(
        const std::string& baseExperimentName,
        const std::map<std::string, std::vector<double>>& parameterGrid,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate);
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable);
    void setMonitoringInterval(double interval);  // seconds
    void startMonitoring() { monitoringEnabled_ = true; }
    void stopMonitoring() { monitoringEnabled_ = false; }
    
    // Learning and adaptation
    void enableAdaptiveLearning(bool enable, double learningRate = 0.01);
    void setLearningStrategy(const std::string& strategy);
    
    // Analysis and reporting
    std::string generateExperimentReport(const std::string& experimentName) const;
    std::string generateBatchReport(const std::vector<std::string>& experimentNames) const;
    std::string generateComparisonReport(const std::vector<std::string>& experimentNames) const;
    
    // Results management
    const std::map<std::string, std::shared_ptr<AdvancedExperimentResult>>& getResults() const;
    std::shared_ptr<AdvancedExperimentResult> getResult(const std::string& experimentName) const;
    void clearResults();
    
    // Checkpoint and persistence
    bool saveExperimentState(const std::string& filepath) const;
    bool loadExperimentState(const std::string& filepath);
    
    // Analysis utilities
    std::string analyzeConvergence(const std::string& experimentName) const;
    std::string analyzePerformance(const std::string& experimentName) const;
    double calculateStatisticalSignificance(const std::string& exp1Name, const std::string& exp2Name) const;
    
    // Advanced experiment modes
    void enableCrossValidation(size_t folds);
    void enableHyperparameterOptimization(const std::map<std::string, std::vector<double>>& paramGrid);
    void enableAblationStudy(const std::vector<std::string>& componentsToRemove);
    
    // Data export
    bool exportResultsToCSV(const std::string& filepath, const std::vector<std::string>& experimentNames) const;
    bool exportResultsToJSON(const std::string& filepath, const std::vector<std::string>& experimentNames) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void updateRealTimeMetrics() const;
    void analyzeLearningProgress() const;
    void handleAdaptiveParameters() const;
    double calculatePerformanceScore(const std::shared_ptr<AdvancedExperimentResult>& result) const;
    
    // Threading and concurrency
    void runParallelExperiment(const std::string& experimentName,
                              std::shared_ptr<Brain> brain,
                              std::shared_ptr<Environment> environment,
                              std::shared_ptr<ParallelExperimentStatus> status);
};

// Convenience functions
AdvancedExperimentRunner& getAdvancedExperimentRunner();
void initializeAdvancedExperimentRunner();
void shutdownAdvancedExperimentRunner();

} // namespace nlm
