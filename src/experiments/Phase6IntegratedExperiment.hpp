/**
 * Phase6IntegratedExperiment - Phase 6 final integration test
 * 
 * This experiment demonstrates that the integrated brain systems work together:
 * - Memory systems are connected to neural processing
 * - Neuromodulation affects plasticity and neural dynamics
 * - Development affects plasticity rates
 * - Prediction system is integrated
 * - Replay and consolidation are functional
 * - Checkpoint save/load works
 * 
 * The experiment runs a complete lifetime simulation and verifies
 * that all systems interact properly.
 */

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>

namespace nlm {

/**
 * Phase 6 integration test result
 */
struct Phase6IntegrationResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    
    // Metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    // Performance metrics
    double stepsPerSecond;
    size_t totalStepsProcessed;
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0)
        , stepsPerSecond(0.0)
        , totalStepsProcessed(0) {}
};

/**
 * Configuration for Phase 6 integration experiment
 * Enhanced with more advanced options for better experimentation
 */
struct Phase6Config {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    // Advanced configuration options
    bool enableVisualization;
    bool enablePerformanceProfiling;
    bool enableDetailedLogging;
    bool enableAdaptiveTimestep;
    float customTimestep;
    bool enablePlasticityLearning;
    bool enableNeuromodulationLearning;
    bool enableDevelopmentLearning;
    
    // Environment configuration
    size_t worldWidth;
    size_t worldHeight;
    bool enableEnvironmentLearning;
    
    // Memory configuration
    bool enableEpisodicMemory;
    bool enableWorkingMemory;
    bool enableAssociativeMemory;
    bool enableSemanticMemory;
    bool enableProceduralMemory;
    
    // Neuromodulation configuration
    bool enableDopamineSystem;
    bool enableCuriositySystem;
    bool enableNoveltyDetection;
    bool enablePredictionError;
    
    // Cognition configuration
    bool enablePlanning;
    bool enableConceptFormation;
    bool enableAttention;
    
    // Development configuration
    bool enableSynaptogenesis;
    bool enablePruning;
    bool enableMaturation;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , enableVisualization(false)
        , enablePerformanceProfiling(true)
        , enableDetailedLogging(false)
        , enableAdaptiveTimestep(false)
        , customTimestep(0.001f)
        , enablePlasticityLearning(true)
        , enableNeuromodulationLearning(true)
        , enableDevelopmentLearning(true)
        , worldWidth(16)
        , worldHeight(16)
        , enableEnvironmentLearning(true)
        , enableEpisodicMemory(true)
        , enableWorkingMemory(true)
        , enableAssociativeMemory(true)
        , enableSemanticMemory(true)
        , enableProceduralMemory(true)
        , enableDopamineSystem(true)
        , enableCuriositySystem(true)
        , enableNoveltyDetection(true)
        , enablePredictionError(true)
        , enablePlanning(true)
        , enableConceptFormation(true)
        , enableAttention(true)
        , enableSynaptogenesis(true)
        , enablePruning(true)
        , enableMaturation(true) {}
};

/**
 * Phase 6 integrated experiment runner
 * Enhanced with modular design for better extensibility
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test with enhanced configuration
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification
     */
    bool verifyIntegration();
    
    /**
     * Test memory integration
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test checkpoint save/load
     */
    bool testCheckpointing();
    
    /**
     * Test replay system
     */
    bool testReplay();
    
    /**
     * Run individual system tests with detailed reporting
     */
    bool runSystemTest(const std::string& systemName);
    
    /**
     * Get the current experiment status
     */
    std::string getStatus() const;
    
    /**
     * Set custom configuration parameters
     */
    void setConfigValue(const std::string& key, const std::string& value);
    
    /**
     * Get configuration parameter value
     */
    std::string getConfigValue(const std::string& key) const;
    
    /**
     * Enable or disable a specific feature
     */
    void enableFeature(const std::string& featureName, bool enable);
    
    /**
     * Check if a feature is enabled
     */
    bool isFeatureEnabled(const std::string& featureName) const;
    
    /**
     * Add a custom test to the experiment suite
     */
    void addCustomTest(std::function<bool()> testFunction, const std::string& testName);
    
    /**
     * Get all custom tests
     */
    const std::vector<std::pair<std::function<bool()>, std::string>>& getCustomTests() const;
    
    /**
     * Set experiment name for better organization
     */
    void setExperimentName(const std::string& name);
    
    /**
     * Get experiment name
     */
    const std::string& getExperimentName() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Custom tests
    std::vector<std::pair<std::function<bool()>, std::string>> customTests;
    
    // Feature flags
    std::unordered_map<std::string, bool> enabledFeatures;
    
    // Experiment metadata
    std::string experimentName;
    
    /**
     * Initialize features based on configuration
     */
    void initializeFeatures(const Phase6Config& config);
    
    /**
     * Run all system tests
     */
    bool runAllSystemTests();
    
    /**
     * Validate configuration
     */
    bool validateConfig(const Phase6Config& config) const;
    
    /**
     * Calculate integration score
     */
    float calculateIntegrationScore(const Phase6IntegrationResult& result) const;
};

} // namespace nlm
