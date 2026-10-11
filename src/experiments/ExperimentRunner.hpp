#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {

// ExperimentResult: Complete results from a single experiment run
struct ExperimentResult {
    std::shared_ptr<Experiment> experiment;
    std::map<std::string, double> metrics;
    std::vector<Experiment::StepData> stepData;
    uint64_t executionTime; // in milliseconds
    bool success;
    std::string errorMessage;
    std::chrono::system_clock::time_point timestamp;
    
    ExperimentResult() : executionTime(0), success(false), timestamp(std::chrono::system_clock::now()) {}
};

// ExperimentStatistics: Statistical analysis of multiple experiment runs
struct ExperimentStatistics {
    std::string name;
    size_t runCount;
    double meanReward;
    double stdDevReward;
    double minReward;
    double maxReward;
    double meanPerformance;
    double stdDevPerformance;
    double coefficientOfVariation;
    size_t successCount;
    size_t failureCount;
    
    ExperimentStatistics() : runCount(0), meanReward(0), stdDevReward(0), 
                             minReward(0), maxReward(0), meanPerformance(0),
                             stdDevPerformance(0), coefficientOfVariation(0),
                             successCount(0), failureCount(0) {}
};

// AdvancedExperimentConfig: Enhanced configuration for experiment runs
struct AdvancedExperimentConfig {
    std::string experimentMode;
    uint64_t seed;
    SimulationStep maxSteps;
    bool enableParallelExecution;
    size_t threadCount;
    std::map<std::string, std::string> customParameters;
    std::map<std::string, double> metricThresholds;
    std::function<bool(Brain* brain, uint64_t step)> earlyStopCondition;
    
    AdvancedExperimentConfig() : experimentMode("standard"), seed(42), 
                                 maxSteps(10000), enableParallelExecution(false),
                                 threadCount(0) {}
};

// ExperimentRunner: Advanced experiment management and execution
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // === Basic Experiment Management ===
    
    // Create new experiment
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    
    // Run single experiment with advanced configuration
    ExperimentResult runExperiment(std::shared_ptr<Experiment> experiment,
                                  std::shared_ptr<Brain> brain,
                                  std::shared_ptr<Environment> environment,
                                  const AdvancedExperimentConfig& config);
    
    // Run single experiment with convenience method
    ExperimentResult runExperiment(std::shared_ptr<Experiment> experiment,
                                  std::shared_ptr<Brain> brain,
                                  std::shared_ptr<Environment> environment);
    
    // Run batch of experiments with advanced configuration
    std::vector<ExperimentResult> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        const AdvancedExperimentConfig& config);
    
    // Run multiple trials with different seeds for statistical significance
    std::vector<ExperimentResult> runWithMultipleSeeds(
        std::shared_ptr<Experiment> experiment,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        size_t numSeeds,
        const AdvancedExperimentConfig& config);
    
    // === Advanced Experiment Features ===
    
    // Run continual learning experiment with task switching
    ExperimentResult runContinualLearning(
        std::shared_ptr<Brain> brain,
        const std::vector<std::function<float(Brain*)>>& tasks,
        size_t cycles = 3,
        const AdvancedExperimentConfig& config = AdvancedExperimentConfig());
    
    // Run damage recovery experiment
    ExperimentResult runDamageRecovery(
        std::shared_ptr<Brain> brain,
        uint64_t damageStep,
        float damageFraction,
        const std::string& damageRegion,
        const AdvancedExperimentConfig& config = AdvancedExperimentConfig());
    
    // Run scaling experiment
    std::vector<ExperimentResult> runScalingExperiment(
        const std::vector<std::pair<size_t, std::function<std::shared_ptr<Brain>(const AdvancedExperimentConfig&)>>>& scales,
        uint64_t stepsPerScale,
        const AdvancedExperimentConfig& config = AdvancedExperimentConfig());
    
    // === Statistical Analysis ===
    
    // Compute statistics from experiment results
    static ExperimentStatistics computeStatistics(const std::vector<ExperimentResult>& results);
    
    // Generate statistical report
    static std::string generateStatisticalReport(const ExperimentStatistics& stats);
    
    // Generate comparative report between multiple experiment sets
    static std::string generateComparativeReport(
        const std::vector<ExperimentStatistics>& stats,
        const std::vector<std::string>& labels);
    
    // === Results Management ===
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Get experiment results by name
    std::vector<ExperimentResult> getExperimentResults(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Export results to JSON
    static bool exportResultsToJSON(const std::vector<ExperimentResult>& results, 
                                    const std::string& filepath);
    
    // Export results to CSV
    static bool exportResultsToCSV(const std::vector<ExperimentResult>& results,
                                   const std::string& filepath);
    
    // === Configuration Management ===
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
    // Set global configuration
    void setGlobalConfig(const AdvancedExperimentConfig& config);
    
    // Get global configuration
    AdvancedExperimentConfig getGlobalConfig() const;
    
    // === Advanced Analysis ===
    
    // Analyze learning curves
    static std::vector<double> analyzeLearningCurve(const std::vector<Experiment::StepData>& stepData);
    
    // Detect learning plateaus
    static bool detectLearningPlateau(const std::vector<double>& learningCurve, 
                                     double threshold = 0.01, 
                                     size_t windowSize = 100);
    
    // Compute convergence metrics
    static double computeConvergenceMetric(const std::vector<double>& learningCurve);
    
    // === Experiment Templates ===
    
    // Create standard experiment template
    static std::shared_ptr<Experiment> createStandardExperiment(const std::string& name);
    
    // Create plasticity experiment template
    static std::shared_ptr<Experiment> createPlasticityExperiment(const std::string& name);
    
    // Create learning experiment template
    static std::shared_ptr<Experiment> createLearningExperiment(const std::string& name);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
