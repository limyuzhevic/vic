#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentLifecycleManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Automation modes
enum class AutomationMode {
    Sequential,
    Parallel,
    Pipeline,
    Batch,
    Stream,
    Scheduled,
    EventDriven,
    Hybrid
};

// Parameter sweep configuration
struct ParameterSweepConfig {
    std::string experimentName;
    std::map<std::string, std::vector<ConfigValue>> parameters;
    std::string sweepType; // "grid", "random", "latin_hypercube", "morris_salas"
    size_t sampleSize;
    bool includeBaseline;
    std::chrono::seconds maxDuration;
    bool useSmartSampling;
    double confidenceLevel;
    
    ParameterSweepConfig() : sampleSize(100), includeBaseline(true), 
                           maxDuration(std::chrono::seconds(3600)), useSmartSampling(false),
                           confidenceLevel(0.95) {}
};

// Hyperparameter optimization configuration
struct HyperparameterOptimizerConfig {
    std::string name;
    std::string algorithm; // "random_search", "bayesian_optimization", "genetic_algorithm", "gradient_descent"
    size_t populationSize;
    size_t generations;
    double mutationRate;
    double crossoverRate;
    std::string objectiveFunction;
    std::map<std::string, double> bounds;
    bool useConstraints;
    bool parallelEvaluation;
    std::chrono::seconds maxDuration;
    
    HyperparameterOptimizerConfig() : populationSize(50), generations(100), mutationRate(0.1), 
                                     crossoverRate(0.8), useConstraints(true), 
                                     parallelEvaluation(false), maxDuration(std::chrono::seconds(3600)) {}
};

// Bayesian optimization configuration
struct BayesianOptimizerConfig {
    std::string name;
    std::string acquisitionFunction; // "expected_improvement", "upper_confidence_bound"
    size_t initialSamples;
    size_t maxIterations;
    double explorationExploitationBalance;
    std::string kernelFunction; // "rbf", "matern52", "matern32", "periodic"
    double kernelParameters;
    std::chrono::seconds maxDuration;
    
    BayesianOptimizerConfig() : initialSamples(10), maxIterations(100), 
                               explorationExploitationBalance(0.5), kernelParameters(1.0),
                               maxDuration(std::chrono::seconds(3600)) {}
};

// Genetic algorithm configuration
struct GeneticAlgorithmConfig {
    std::string name;
    size_t populationSize;
    size_t generations;
    double mutationRate;
    double crossoverRate;
    double elitismRate;
    std::string selectionMethod; // "roulette_wheel", "tournament", "rank_based"
    std::string encoding; // "binary", "real", "integer", "permutation"
    std::map<std::string, double> geneRanges;
    bool useConstraints;
    bool parallelEvaluation;
    std::chrono::seconds maxDuration;
    
    GeneticAlgorithmConfig() : populationSize(50), generations(100), mutationRate(0.1), 
                              crossoverRate(0.8), elitismRate(0.1), useConstraints(true),
                              parallelEvaluation(false), maxDuration(std::chrono::seconds(3600)) {}
};

// Automation task
struct AutomationTask {
    std::string taskId;
    std::string experimentName;
    AutomationMode mode;
    std::chrono::system_clock::time_point scheduledTime;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string status; // "pending", "running", "completed", "failed", "cancelled"
    std::map<std::string, ConfigValue> parameters;
    std::map<std::string, double> results;
    std::string errorMessage;
    double progress;
    
    AutomationTask() : progress(0.0), scheduledTime(std::chrono::system_clock::now()) {}
    AutomationTask(const std::string& id, const std::string& experimentName, AutomationMode mode)
        : taskId(id), experimentName(experimentName), mode(mode), progress(0.0),
          scheduledTime(std::chrono::system_clock::now()) {}
};

// Automation callback
using AutomationCallback = std::function<void(const AutomationTask&)>
;

class ExperimentAutomation {
public:
    ExperimentAutomation();
    ~ExperimentAutomation();
    
    // Disable copying, enable moving
    ExperimentAutomation(const ExperimentAutomation&) = delete;
    ExperimentAutomation& operator=(const ExperimentAutomation&) = delete;
    ExperimentAutomation(ExperimentAutomation&&) noexcept;
    ExperimentAutomation& operator=(ExperimentAutomation&&) noexcept;
    
    // Parameter sweep
    bool runParameterSweep(const ExperimentConfig& baseConfig, 
                         const std::map<std::string, std::vector<ConfigValue>>& parameters);
    bool runParameterSweep(const ParameterSweepConfig& config);
    bool runGridSearch(const std::vector<std::map<std::string, ConfigValue>>& parameterCombinations);
    bool runRandomSearch(const ExperimentConfig& baseConfig, 
                       const std::map<std::string, std::vector<ConfigValue>>& parameters,
                       size_t sampleSize);
    
    // Hyperparameter optimization
    bool runHyperparameterOptimization(const ExperimentConfig& baseConfig, 
                                      const HyperparameterOptimizerConfig& config);
    bool runBayesianOptimization(const ExperimentConfig& baseConfig, 
                                const BayesianOptimizerConfig& config);
    bool runGeneticAlgorithm(const ExperimentConfig& baseConfig, 
                           const GeneticAlgorithmConfig& config);
    bool runGradientDescent(const ExperimentConfig& baseConfig, 
                          const std::map<std::string, double>& learningRate);
    
    // Automated experimentation
    bool runAutomatedExperiment(const ExperimentConfig& config, 
                               const std::string& schedule = "") const;
    bool runBatchExperiments(const std::vector<ExperimentConfig>& configs);
    bool runSequentialExperiments(const std::vector<ExperimentConfig>& configs);
    bool runParallelExperiments(const std::vector<ExperimentConfig>& configs);
    
    // Automation management
    bool createAutomationTask(const AutomationTask& task);
    bool removeAutomationTask(const std::string& taskId);
    bool updateAutomationTask(const std::string& taskId, const AutomationTask& task);
    AutomationTask getAutomationTask(const std::string& taskId) const;
    std::vector<AutomationTask> getAllAutomationTasks() const;
    std::vector<AutomationTask> getTasksByExperiment(const std::string& experimentName) const;
    
    // Automation control
    bool startAutomationTask(const std::string& taskId);
    bool pauseAutomationTask(const std::string& taskId);
    bool resumeAutomationTask(const std::string& taskId);
    bool stopAutomationTask(const std::string& taskId);
    bool cancelAutomationTask(const std::string& taskId);
    
    // Callback management
    bool addAutomationCallback(AutomationCallback callback);
    bool removeAutomationCallback(AutomationCallback callback);
    bool clearAutomationCallbacks();
    
    // Task monitoring
    double getTaskProgress(const std::string& taskId) const;
    std::string getTaskStatus(const std::string& taskId) const;
    std::chrono::system_clock::time_point getTaskStartTime(const std::string& taskId) const;
    std::chrono::system_clock::time_point getTaskEndTime(const std::string& taskId) const;
    std::chrono::seconds getTaskDuration(const std::string& taskId) const;
    std::string getTaskErrorMessage(const std::string& taskId) const;
    
    // Result management
    bool storeTaskResults(const std::string& taskId, const std::map<std::string, double>& results);
    std::map<std::string, double> getTaskResults(const std::string& taskId) const;
    bool exportTaskResults(const std::string& taskId, const std::string& filepath) const;
    
    // Automation analysis
    double calculateTaskEfficiency(const std::string& taskId) const;
    std::string analyzeAutomationPerformance(const std::string& taskId) const;
    std::vector<AutomationTask> getTaskHistory(size_t limit = 100) const;
    
    // Automation optimization
    bool optimizeAutomationSchedule();
    bool autoAdjustParameters(const std::string& taskId);
    bool enableParallelExecution(const std::string& taskId, bool enable);
    bool enableResourceOptimization(const std::string& taskId, bool enable);
    
    // Automation cleanup
    bool cleanupCompletedTasks(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupFailedTasks(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetTask(const std::string& taskId);
    bool clearAllTasks();
    std::vector<std::string> getAllTaskIds() const;
    size_t getTaskCount() const;
    
    // Automation utilities
    static std::string generateTaskId(const std::string& experimentName, const std::string& mode) const;
    static bool isValidAutomationMode(AutomationMode mode) const;
    static std::string automationModeToString(AutomationMode mode) const;
    static AutomationMode stringToAutomationMode(const std::string& mode) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm