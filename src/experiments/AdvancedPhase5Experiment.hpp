#pragma once

#include "Phase5Experiment.hpp"
#include "../experiments/ExperimentPresets.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <map>
#include <set>

namespace nlm {

/**
 * Advanced Phase 5 experiment configuration
 * Extends basic Phase 5 with parameter sweeps and advanced optimization
 */
struct AdvancedPhase5Config {
    // Lifetime experiment parameters
    size_t initialNeurons;
    size_t maxNeurons;
    float connectionProbability;
    
    // Development phase settings
    uint64_t synapseFormationSteps;
    uint64_t sensoryDevelopmentSteps;
    uint64_t motorDevelopmentSteps;
    uint64_t associativeDevelopmentSteps;
    uint64_t memoryConsolidationSteps;
    uint64_t socialDevelopmentSteps;
    
    // Experiment controls
    size_t trialsPerPhase;
    uint64_t stepsPerTrial;
    
    // Advanced optimization
    bool enableParameterSweeps;
    std::unordered_map<std::string, std::vector<float>> parameterRanges;
    std::vector<std::string> sweepParameters;
    bool parallelSweeps;
    
    bool enableMultiObjectiveOptimization;
    std::vector<std::string> optimizationObjectives;
    float convergenceThreshold;
    size_t maxIterations;
    
    bool enableAdaptiveExperimentation;
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
    bool enableSparseConnectivity;
    bool enableEventDriven;
    
    // Output and analysis
    std::string outputDirectory;
    bool generateVisualizations;
    bool saveCheckpoints;
    bool generateReports;
    size_t checkpointInterval;
    
    // Specialized research
    bool enableContinualLearningAnalysis;
    bool enableDamageRecoveryAnalysis;
    bool enableScalingAnalysis;
    bool enableAblationAnalysis;
    std::vector<std::string> specializedAnalyses;
    
    // Statistical rigor
    bool enableMultiSeedExperiments;
    size_t numSeeds;
    bool enableCrossValidation;
    size_t cvFolds;
    
    // Constructors
    AdvancedPhase5Config();
    AdvancedPhase5Config(const std::string& presetName);
    
    // Preset management
    static AdvancedPhase5Config getPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    // Validation and optimization
    bool validate() const;
    std::string getValidationErrors() const;
    std::vector<std::string> getOptimizationRecommendations() const;
};

/**
 * Advanced Phase 5 experiment with parameter sweeps
 * Extends the basic Phase 5 experiment with:
 * - Automated parameter sweep capabilities
 * - Multi-objective optimization
 * - Adaptive experimentation
 * - Comparative analysis framework
 * - Hyperparameter optimization integration
 * - Statistical rigor through multi-seed experiments
 * - Specialized analysis modes (continual learning, damage recovery, scaling)
 */
class AdvancedPhase5Experiment {
public:
    AdvancedPhase5Experiment(const AdvancedPhase5Config& config);
    AdvancedPhase5Experiment(const std::string& presetName);
    ~AdvancedPhase5Experiment();
    
    // Core experiment execution
    void run();
    bool isRunning() const { return running_; }
    
    // Configuration management
    const AdvancedPhase5Config& getConfig() const { return config_; }
    void updateConfig(const AdvancedPhase5Config& newConfig);
    
    // Preset management
    static std::vector<std::string> getAvailablePresets();
    static AdvancedPhase5Config getResearchPreset(const std::string& researchArea);
    static AdvancedPhase5Config getPerformancePreset(const std::string& scale);
    static AdvancedPhase5Config getResearchQuestionPreset(const std::string& question);
    
    // Advanced features
    void runParameterSweep();
    void runMultiObjectiveOptimization();
    void enableAdaptiveExperimentation(float rate = 0.01f, float exploration = 0.1f);
    void runComparativeAnalysis(const std::vector<std::string>& presetNames);
    void enableHyperparameterOptimization(const std::string& algorithm = "CMA-ES");
    
    // Specialized analyses
    void runContinualLearningAnalysis();
    void runDamageRecoveryAnalysis(float damageFraction, const std::string& region);
    void runScalingAnalysis();
    void runAblationAnalysis();
    
    // Experiment control
    void setOptimizationAlgorithm(const std::string& algorithm);
    void setOptimizationParameters(const std::unordered_map<std::string, float>& params);
    void enableMultithreading(size_t threads = std::thread::hardware_concurrency());
    
    // Results collection and analysis
    const std::vector<LifetimePhaseResult>& getResults() const { return phaseResults_; }
    const std::vector<std::vector<float>>& getSweepResults() const { return sweepResults_; }
    const std::vector<LifetimeExperimentResult>& getMultiSeedResults() const { return multiSeedResults_; }
    float getOptimizationScore(const std::string& objective) const;
    std::string getParetoFrontDescription() const;
    std::string getComprehensiveReport() const;
    
    // Progress and status
    float getProgress() const;
    std::string getStatusReport() const;
    
    // Export and import
    bool exportResults(const std::string& filepath) const;
    static std::shared_ptr<AdvancedPhase5Experiment> importResults(const std::string& filepath);
    
    // Statistical analysis
    void computeStatistics();
    float computeConfidenceInterval(float confidence = 0.95f) const;
    bool checkConvergence() const;
    
    // Specialized result access
    const std::vector<ScalingAnalysis>& getScalingResults() const { return scalingResults_; }
    const std::vector<std::vector<AblationResult>>& getAblationResults() const { return ablativeResults_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    AdvancedPhase5Config config_;
    bool running_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::vector<LifetimePhaseResult> phaseResults_;
    std::vector<std::vector<float>> sweepResults_;
    std::vector<LifetimeExperimentResult> multiSeedResults_;
    std::vector<ScalingAnalysis> scalingResults_;
    std::vector<std::vector<AblationResult>> ablativeResults_;
    std::vector<std::vector<float>> paretoFront_;
    std::unordered_map<std::string, float> currentObjectives_;
    
    // Helper methods
    void initializeExperiment();
    void executeLifetimeExperiment();
    void executeParameterSweep();
    void executeMultiObjectiveOptimization();
    void updateAdaptiveParameters();
    float evaluateObjective(const std::string& objective, const LifetimePhaseResult& results) const;
    void updateParetoFront(const std::vector<float>& objectiveValues);
    void generateComparativeReport() const;
    void generateOptimizationReport() const;
    void generateScalingReport() const;
    void generateAblationReport() const;
    float calculateWeightedScore(const std::vector<float>& objectives) const;
    
    // Brain creation factory methods
    std::shared_ptr<Brain> createBrainForPhase(const std::string& phaseName, const LifetimeExperimentConfig& baseConfig);
    std::shared_ptr<Brain> createBrainForScale(const ScaleLevel& scale, uint64_t seed);
    std::shared_ptr<Brain> createBrainForAblation(const AblationConfig& config);
};

} // namespace nlm