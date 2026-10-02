#pragma once

#include "Phase4Experiment.hpp"
#include "../experiments/ExperimentPresets.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <random>
#include <algorithm>
#include <numeric>
#include <thread>

namespace nlm {

/**
 * Advanced Phase 4 experiment configuration
 * Extends basic Phase 4 with multi-objective optimization and parameter sweeps
 */
struct AdvancedPhase4Config {
    // Experiment scale
    size_t numTrials;
    size_t stepsPerTrial;
    uint64_t seed;
    
    // Performance optimization
    bool enableMultiObjectiveOptimization;
    std::vector<std::string> optimizationObjectives;
    float convergenceThreshold;
    size_t maxIterations;
    
    // Parameter sweeps
    bool enableParameterSweeps;
    std::unordered_map<std::string, std::vector<float>> parameterRanges;
    std::vector<std::string> sweepParameters;
    bool parallelSweeps;
    
    // Adaptive optimization
    bool enableAdaptiveOptimization;
    float adaptationRate;
    float explorationRate;
    
    // Comparative analysis
    bool enableComparativeAnalysis;
    std::vector<std::string> comparativePresets;
    bool runAllCombinations;
    
    // Hyperparameter optimization
    bool enableHyperparameterOptimization;
    std::string optimizationAlgorithm;
    std::unordered_map<std::string, float> optimizationParameters;
    
    // Resource management
    bool enableMultithreading;
    size_t threadCount;
    bool enableSIMD;
    bool enableMemoryOptimization;
    
    // Output and logging
    std::string outputDirectory;
    bool generateVisualizations;
    bool saveIntermediateResults;
    size_t checkpointInterval;
    
    // Experimental controls
    bool enableAblations;
    std::vector<std::string> ablationTargets;
    bool enableCrossValidation;
    size_t cvFolds;
    
    // Constructors
    AdvancedPhase4Config();
    AdvancedPhase4Config(const std::string& presetName);
    
    // Preset management
    static AdvancedPhase4Config getPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    // Validation and optimization
    bool validate() const;
    std::string getValidationErrors() const;
    std::vector<std::string> getOptimizationRecommendations() const;
};

/**
 * Advanced Phase 4 experiment with multi-objective optimization
 * Extends the basic Phase 4 experiment with:
 * - Multi-objective optimization capabilities
 * - Parameter sweep automation
 * - Adaptive optimization algorithms
 * - Comparative experiment framework
 * - Hyperparameter optimization integration
 * - Parallel execution support
 */
class AdvancedPhase4Experiment {
public:
    AdvancedPhase4Experiment(const AdvancedPhase4Config& config);
    AdvancedPhase4Experiment(const std::string& presetName);
    ~AdvancedPhase4Experiment();
    
    // Core experiment execution
    void run();
    bool isRunning() const { return running_; }
    
    // Configuration management
    const AdvancedPhase4Config& getConfig() const { return config_; }
    void updateConfig(const AdvancedPhase4Config& newConfig);
    
    // Preset management
    static std::vector<std::string> getAvailablePresets();
    static AdvancedPhase4Config getResearchPreset(const std::string& researchArea);
    static AdvancedPhase4Config getPerformancePreset(const std::string& scale);
    static AdvancedPhase4Config getResearchQuestionPreset(const std::string& question);
    
    // Advanced features
    void runParameterSweep();
    void runMultiObjectiveOptimization();
    void enableAdaptiveOptimization(float rate = 0.01f, float exploration = 0.1f);
    void runComparativeAnalysis(const std::vector<std::string>& presetNames);
    void enableHyperparameterOptimization(const std::string& algorithm = "CMA-ES");
    
    // Experiment control
    void setOptimizationAlgorithm(const std::string& algorithm);
    void setOptimizationParameters(const std::unordered_map<std::string, float>& params);
    void enableMultithreading(size_t threads = std::thread::hardware_concurrency());
    
    // Results collection and analysis
    const std::vector<Phase4Results>& getResults() const { return results_; }
    const std::unordered_map<std::string, std::vector<float>>& getSweepResults() const { return sweepResults_; }
    float getOptimizationScore(const std::string& objective) const;
    std::string getParetoFrontDescription() const;
    std::string getOptimizationReport() const;
    
    // Progress and status
    float getProgress() const;
    std::string getStatusReport() const;
    
    // Export and import
    bool exportResults(const std::string& filepath) const;
    static std::shared_ptr<AdvancedPhase4Experiment> importResults(const std::string& filepath);
    
    // Statistical analysis
    void computeStatistics();
    float computeConfidenceInterval(float confidence = 0.95f) const;
    bool checkConvergence() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    AdvancedPhase4Config config_;
    bool running_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::vector<Phase4Results> results_;
    std::unordered_map<std::string, std::vector<float>> sweepResults_;
    std::vector<std::vector<float>> paretoFront_;
    std::unordered_map<std::string, float> currentObjectives_;
    
    // Helper methods
    void initializeExperiment();
    void executeSingleExperiment(size_t trialIndex);
    void executeParameterSweep();
    void executeMultiObjectiveOptimization();
    void updateAdaptiveParameters();
    float evaluateObjective(const std::string& objective, const Phase4Results& results) const;
    void updateParetoFront(const std::vector<float>& objectiveValues);
    void generateComparativeReport() const;
    void generateOptimizationReport() const;
    float calculateWeightedScore(const std::vector<float>& objectives) const;
};

} // namespace nlm