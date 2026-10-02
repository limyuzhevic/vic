#pragma once

#include "../agent/AgentBody.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Simple text logger for experiments
class ExperimentLogger {
public:
    ExperimentLogger(const std::string& filename);
    ~ExperimentLogger();
    
    void log(const std::string& message);
    void logEpisodeStart(int episode);
    void logEpisodeEnd(int episode, float totalReward, int steps, float finalNovelty);
    void logStep(int step, float reward, float neuromod, float curiosity, 
                 float energy, const std::string& action);
    void logDevelopment(float age, const std::string& stage);
    void logSynapticStats(size_t totalSynapses, float avgWeight, float maxWeight);
    void logPhase3Header();
    
    void flush();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Metrics collector for Phase 3 experiments
struct ExperimentMetrics {
    // Episode metrics
    int episode;
    int steps;
    float totalReward;
    float finalEnergy;
    float finalNovelty;
    float finalCuriosity;
    float predictionError;
    
    // Development
    float developmentalAge;
    std::string developmentalStage;
    
    // Network state
    size_t totalSynapses;
    float averageWeight;
    float maxWeight;
    size_t totalSpikes;
    float averageFiringRate;
    
    // Motor behavior
    int forwardCount;
    int backwardCount;
    int turnLeftCount;
    int turnRightCount;
    int interactCount;
    int waitCount;
    
    ExperimentMetrics();
    void reset();
    std::string toCSV() const;
};

/**
 * Advanced Phase 3 experiment configuration
 * Extended configuration for enhanced Phase 3 experiments
 */
struct AdvancedPhase3Config {
    // Basic configuration
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
    explicit AdvancedPhase3Config(const std::string& presetName);
    
    // Preset management
    static AdvancedPhase3Config getPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    // Research preset factories
    static AdvancedPhase3Config getResearchScientificPreset();
    static AdvancedPhase3Config getResearchSpeedPreset();
    static AdvancedPhase3Config getResearchReproducibilityPreset();
    
    // Performance preset factories
    static AdvancedPhase3Config getPerformanceSmallPreset();
    static AdvancedPhase3Config getPerformanceMediumPreset();
    static AdvancedPhase3Config getPerformanceLargePreset();
    
    // Research question preset factories
    static AdvancedPhase3Config getLearningCurvesPreset();
    static AdvancedPhase3Config getMemoryCapacityPreset();
    static AdvancedPhase3Config getSocialDynamicsPreset();
    static AdvancedPhase3Config getDevelopmentStagesPreset();
    
    // Validation
    bool validate() const;
    std::string getValidationErrors() const;
};

/**
 * Advanced Phase 3 experiment class
 * Enhanced Phase 3 experiment with configurable parameters
 */
class AdvancedPhase3Experiment {
public:
    AdvancedPhase3Experiment(const AdvancedPhase3Config& config);
    explicit AdvancedPhase3Experiment(const std::string& presetName);
    ~AdvancedPhase3Experiment();
    
    // Core functionality
    void run();
    bool isRunning() const { return running_; }
    
    // Configuration access
    const AdvancedPhase3Config& getConfig() const { return config_; }
    void updateConfig(const AdvancedPhase3Config& newConfig);
    
    // Results access
    const std::vector<ExperimentMetrics>& getResults() const { return results_; }
    float getFinalPerformanceScore() const { return performanceScore_; }
    
    // Advanced features
    void runParameterSweep(const std::string& parameterName, const std::vector<float>& values);
    void runMultiObjectiveOptimization(const std::vector<std::string>& objectives);
    void enableAdaptiveExperimentation(float rate = 0.01f);
    void runComparativeAnalysis(const std::vector<std::string>& presetNames);
    void enableHyperparameterOptimization();
    
    // Progress and status
    float getProgress() const;
    std::string getStatusReport() const;
    
    // Export/import
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
