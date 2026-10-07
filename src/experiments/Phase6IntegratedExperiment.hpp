#pragma once

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
        , totalWallClockTime(0.0) {}
};

// Forward declarations
class Config;

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
        , totalWallClockTime(0.0) {}
};

/**
 * Configuration for Phase 6 integration experiment
 * Uses the core Config system to avoid duplication
 */
class Phase6Config {
public:
    Phase6Config();
    ~Phase6Config();
    
    // Disable copying, enable moving
    Phase6Config(const Phase6Config&) = delete;
    Phase6Config& operator=(const Phase6Config&) = delete;
    Phase6Config(Phase6Config&&) noexcept;
    Phase6Config& operator=(Phase6Config&&) noexcept;
    
    // Create from core Config
    static Phase6Config fromCoreConfig(const Config& config);
    
    // Create core Config
    std::shared_ptr<Config> toCoreConfig() const;
    
    // Access individual values with defaults
    uint64_t getMaxSteps() const { return maxSteps; }
    size_t getNeuronCount() const { return neuronCount; }
    size_t getRegionCount() const { return regionCount; }
    float getConnectionProbability() const { return connectionProbability; }
    bool getEnableCheckpointing() const { return enableCheckpointing; }
    bool getEnableReplay() const { return enableReplay; }
    bool getEnableDevelopment() const { return enableDevelopment; }
    std::string getCheckpointPath() const { return checkpointPath; }
    
    // Set values
    void setMaxSteps(uint64_t steps) { maxSteps = steps; }
    void setNeuronCount(size_t count) { neuronCount = count; }
    void setRegionCount(size_t count) { regionCount = count; }
    void setConnectionProbability(float prob) { connectionProbability = prob; }
    void setEnableCheckpointing(bool enable) { enableCheckpointing = enable; }
    void setEnableReplay(bool enable) { enableReplay = enable; }
    void setEnableDevelopment(bool enable) { enableDevelopment = enable; }
    void setCheckpointPath(const std::string& path) { checkpointPath = path; }
    
    // Load from file using core Config system
    bool loadFromFile(const std::string& filepath);
    
    // Save to file using core Config system
    bool saveToFile(const std::string& filepath) const;
    
    // Apply command line arguments
    bool loadFromArgs(int argc, char** argv);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Phase 6 specific config values
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    
    // Generate config key prefix
    static std::string getPhase6Prefix() { return "phase6_"; }
    
    // Core config keys
    static const std::string MAX_STEPS_KEY;
    static const std::string NEURON_COUNT_KEY;
    static const std::string REGION_COUNT_KEY;
    static const std::string CONNECTION_PROBABILITY_KEY;
    static const std::string ENABLE_CHECKPOINTING_KEY;
    static const std::string ENABLE_REPLAY_KEY;
    static const std::string ENABLE_DEVELOPMENT_KEY;
    static const std::string CHECKPOINT_PATH_KEY;
};

/**
 * Phase 6 integrated experiment runner
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test
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
};

} // namespace nlm
