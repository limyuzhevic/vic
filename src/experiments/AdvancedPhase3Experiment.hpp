#pragma once

#include "Phase3Experiment.hpp"
#include "../experiments/ExperimentPresets.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>

namespace nlm {

/**
 * Advanced Phase 3 experiment configuration
 * Extends basic Phase 3 with configurable parameters and advanced features
 */
struct AdvancedPhase3Config {
    // World interaction parameters
    size_t numAgents;
    size_t worldWidth;
    size_t worldHeight;
    float timeStep;
    
    // Behavioral parameters
    float explorationRate;
    float noveltyThreshold;
    float energyDecayRate;
    float curiosityCoefficient;
    
    // Learning parameters
    size_t maxEpisodes;
    size_t stepsPerEpisode;
    float rewardScaling;
    float noveltyScaling;
    
    // Environmental parameters
    float temperature;
    float humidity;
    float resourceDensity;
    
    // Logging and output
    std::string outputFile;
    bool enableVerboseLogging;
    size_t logInterval;
    
    // Advanced features
    bool enableParameterSweeps;
    std::unordered_map<std::string, std::vector<float>> parameterRanges;
    bool enableMultiObjectiveOptimization;
    std::vector<std::string> optimizationObjectives;
    bool enableAdaptiveExperimentation;
    float adaptationRate;
    
    // Development tracking
    bool enableDevelopmentalTracking;
    float developmentalMilestoneInterval;
    std::unordered_map<std::string, float> targetMilestones;
    
    // Comparison and benchmarking
    bool enableComparativeAnalysis;
    std::vector<std::string> comparativePresetNames;
    bool runInParallel;
    
    // Constructors
    AdvancedPhase3Config();
    AdvancedPhase3Config(const std::string& presetName);
    
    // Preset management
    static AdvancedPhase3Config getPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    // Validation
    bool validate() const;
    std::string getValidationErrors() const;
};

/**
 * Advanced Phase 3 experiment with configurable parameters
 * Extends the basic Phase 3 experiment with:
 * - Preset configuration management
 * - Parameter sweep capabilities
 * - Multi-objective optimization support
 * - Adaptive experimentation
 * - Comparative analysis framework
 * - Developmental milestone tracking
 */
class AdvancedPhase3Experiment {
public:
    AdvancedPhase3Experiment(const AdvancedPhase3Config& config);
    AdvancedPhase3Experiment(const std::string& presetName);
    ~AdvancedPhase3Experiment();
    
    // Core experiment execution
    void run();
    bool isRunning() const { return running_; }
    
    // Configuration access
    const AdvancedPhase3Config& getConfig() const { return config_; }
    void updateConfig(const AdvancedPhase3Config& newConfig);
    
    // Preset management
    static std::vector<std::string> getAvailablePresets();
    static AdvancedPhase3Config getResearchPreset(const std::string& researchType);
    static AdvancedPhase3Config getPerformancePreset(const std::string& performanceLevel);
    static AdvancedPhase3Config getResearchQuestionPreset(const std::string& question);
    
    // Advanced features
    void runParameterSweep(const std::string& parameterName, const std::vector<float>& values);
    void runMultiObjectiveOptimization(const std::vector<std::string>& objectives);
    void enableAdaptiveExperimentation(float rate = 0.01f);
    void runComparativeAnalysis(const std::vector<std::string>& presetNames);
    
    // Developmental tracking
    void trackDevelopmentalStage(float age, const std::string& stageName);
    float getCurrentAge() const { return currentAge_; }
    std::string getCurrentStage() const { return currentStage_; }
    
    // Results collection
    const std::vector<ExperimentMetrics>& getResults() const { return results_; }
    float getFinalPerformanceScore() const { return performanceScore_; }
    std::string getOptimizationReport() const;
    
    // Hyperparameter optimization
    void enableHyperparameterOptimization();
    void setOptimizationAlgorithm(const std::string& algorithm);
    void setOptimizationParameters(const std::unordered_map<std::string, float>& params);
    
    // Progress tracking
    float getProgress() const;
    std::string getStatusReport() const;
    
    // Export and import
    bool exportResults(const std::string& filepath) const;
    static std::shared_ptr<AdvancedPhase3Experiment> importResults(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    AdvancedPhase3Config config_;
    bool running_;
    float currentAge_;
    std::string currentStage_;
    std::vector<ExperimentMetrics> results_;
    float performanceScore_;
    
    // Helper methods
    void initializeExperiment();
    void executeExperimentStep(size_t step);
    void finalizeExperiment();
    void updateAdaptiveParameters();
    float calculatePerformanceScore() const;
    void generateComparativeReport() const;
};

} // namespace nlm