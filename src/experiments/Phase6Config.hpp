#pragma once

/**
 * Phase6Config - Configuration for Phase 6 integration experiment
 * 
 * Enhanced configuration structure for Phase 6 experiments with
 * advanced options for testing different neural development scenarios.
 */

#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

/**
 * Configuration structure for Phase 6 integration experiment
 * 
 * This configuration supports advanced neural development scenarios
 * including different plasticity regimes, memory configurations,
 * and experimental parameters for testing integration completeness.
 */
struct Phase6Config {
    // Basic simulation parameters
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    double simulationTimestep;
    
    // Memory system configuration
    bool enableWorkingMemory;
    bool enableEpisodicMemory;
    bool enableSemanticMemory;
    bool enableProceduralMemory;
    bool enableAssociativeMemory;
    size_t workingMemoryCapacity;
    size_t maxEpisodicEpisodes;
    
    // Neuromodulation configuration
    bool enableDopamine;
    bool enableCuriosity;
    bool enablePredictionError;
    bool enableNovelty;
    float baselineDopamineLevel;
    float curiosityThreshold;
    
    // Plasticity configuration
    bool enableHebbianPlasticity;
    bool enableSTDP;
    bool enableStructuralPlasticity;
    float hebbianLearningRate;
    float stdpLTPWeight;
    float stdpLTDWeight;
    float stdpTimeConstant;
    float synaptogenesisRate;
    float pruningRate;
    
    // Development configuration
    bool enableDevelopment;
    bool enableSynaptogenesis;
    bool enablePruning;
    bool enableMaturation;
    size_t replayInterval;
    size_t consolidationInterval;
    size_t developmentUpdateInterval;
    
    // Prediction and cognition
    bool enablePredictionSystem;
    bool enableNeuralPlanner;
    bool enableConceptFormation;
    bool enableAttentionSystem;
    int planningDepth;
    float attentionInhibitionStrength;
    
    // Environment and interaction
    bool enableWorldInteraction;
    bool enableSensoryProcessing;
    bool enableMotorOutput;
    std::string worldType;
    std::string sensoryMode;
    std::string motorMode;
    
    // Checkpoint and persistence
    bool enableCheckpointing;
    bool enableReplay;
    std::string checkpointPath;
    int checkpointFrequency;
    int maxCheckpoints;
    
    // Experimental parameters
    bool enableAgentBrain;
    bool enableSocialLearning;
    bool enableContinualLearning;
    std::vector<std::string> enabledExperiments;
    
    // Performance and debugging
    bool enableLogging;
    bool enableProfiling;
    bool enableValidation;
    int logLevel;
    int experimentSeed;
    
    // Advanced options for testing edge cases
    bool testEdgeCases;
    bool stressTest;
    bool performanceBenchmark;
    int concurrencyLevel;
    
    // Configuration methods
    Phase6Config() : maxSteps(10000), neuronCount(1000), regionCount(1), 
                   connectionProbability(0.1f), simulationTimestep(0.001),
                   enableWorkingMemory(true), enableEpisodicMemory(true),
                   enableSemanticMemory(true), enableProceduralMemory(true),
                   enableAssociativeMemory(true), workingMemoryCapacity(100),
                   maxEpisodicEpisodes(1000), enableDopamine(true),
                   enableCuriosity(true), enablePredictionError(true),
                   enableNovelty(true), baselineDopamineLevel(0.5f),
                   curiosityThreshold(0.1f), enableHebbianPlasticity(true),
                   enableSTDP(true), enableStructuralPlasticity(true),
                   hebbianLearningRate(0.01f), stdpLTPWeight(0.01f),
                   stdpLTDWeight(0.012f), stdpTimeConstant(20.0f),
                   synaptogenesisRate(0.0001f), pruningRate(0.00001f),
                   enableDevelopment(true), enableSynaptogenesis(true),
                   enablePruning(true), enableMaturation(true),
                   replayInterval(100), consolidationInterval(1000),
                   developmentUpdateInterval(1000), enablePredictionSystem(true),
                   enableNeuralPlanner(true), enableConceptFormation(true),
                   enableAttentionSystem(true), planningDepth(5),
                   attentionInhibitionStrength(0.5f), enableWorldInteraction(true),
                   enableSensoryProcessing(true), enableMotorOutput(true),
                   worldType("SimpleWorld"), sensoryMode("Vision"),
                   motorMode("ActionSelection"), enableCheckpointing(true),
                   enableReplay(true), checkpointPath("./checkpoint_test.bin"),
                   checkpointFrequency(1000), maxCheckpoints(10),
                   enableAgentBrain(true), enableSocialLearning(false),
                   enableContinualLearning(true),
                   enableLogging(true), enableProfiling(false),
                   enableValidation(false), logLevel(2), experimentSeed(42),
                   testEdgeCases(false), stressTest(false),
                   performanceBenchmark(false), concurrencyLevel(1) {}
    
    /**
     * Load configuration from a string (CSV format)
     * 
     * @param configString String containing key=value pairs
     */
    void loadFromString(const std::string& configString);
    
    /**
     * Load configuration from file
     * 
     * @param filename Configuration file path
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * Validate configuration
     * 
     * @return True if configuration is valid
     */
    bool validate() const;
    
    /**
     * Get configuration value by key
     * 
     * @param key Configuration key
     * @return Value as string, or empty string if not found
     */
    std::string getValue(const std::string& key) const;
    
    /**
     * Set configuration value by key
     * 
     * @param key Configuration key
     * @param value New value
     */
    void setValue(const std::string& key, const std::string& value);
    
    /**
     * Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * Reset configuration to default values
     */
    void resetToDefaults();
    
    /**
     * Print configuration summary
     */
    void printSummary() const;
    
    /**
     * Create configuration for specific test scenario
     * 
     * @param scenario Test scenario name
     * @return Configuration tailored for the scenario
     */
    static Phase6Config createScenario(const std::string& scenario);
    
private:
    // Internal storage for configuration values
    std::unordered_map<std::string, std::string> configValues;
};

} // namespace nlm
