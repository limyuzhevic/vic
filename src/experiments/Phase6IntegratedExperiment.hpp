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

/**
 * Configuration for Phase 6 integration experiment
 * Enhanced with validation and better defaults for both beginners and advanced users
 */
struct Phase6Config {
    // Core simulation parameters
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // Enable/disable specific systems
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    bool enableVisualization;
    
    // Configuration file and paths
    std::string checkpointPath;
    std::string configPath;
    
    // Advanced parameters for power users
    float stdpLtpWeight;
    float stdpLtdWeight;
    float synaptogenesisRate;
    float pruningRate;
    size_t replayInterval;
    size_t consolidationInterval;
    float noveltyThreshold;
    float curiosityDecay;
    
    // Performance and debugging
    bool enableProfiling;
    bool enableDetailedLogging;
    bool enableStepByStepOutput;
    uint32_t seed;
    
    // Memory and storage
    float memoryDecayRate;
    size_t maxMemoryEpisodes;
    
    // Development stages
    bool enableDevelopmentalStages;
    float plasticityModulationStrength;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , enableVisualization(false)
        , checkpointPath("./checkpoint_test.bin")
        , configPath("")
        , stdpLtpWeight(0.01f)
        , stdpLtdWeight(0.012f)
        , synaptogenesisRate(0.0001f)
        , pruningRate(0.00001f)
        , replayInterval(100)
        , consolidationInterval(1000)
        , noveltyThreshold(0.5f)
        , curiosityDecay(0.99f)
        , enableProfiling(false)
        , enableDetailedLogging(false)
        , enableStepByStepOutput(false)
        , seed(42)
        , memoryDecayRate(0.01f)
        , maxMemoryEpisodes(1000)
        , enableDevelopmentalStages(true)
        , plasticityModulationStrength(1.0f) {}
    
    /**
     * Validate the configuration and provide helpful error messages
     */
    bool validate(std::string& error) const {
        error.clear();
        
        // Validate core parameters
        if (maxSteps == 0) {
            error = "maxSteps must be greater than 0";
            return false;
        }
        
        if (neuronCount == 0) {
            error = "neuronCount must be greater than 0";
            return false;
        }
        
        if (neuronCount > 100000) {
            error = "neuronCount is very high - may cause performance issues. Consider reducing to < 100000.";
        }
        
        if (regionCount == 0) {
            error = "regionCount must be greater than 0";
            return false;
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            error = "connectionProbability must be between 0.0 and 1.0";
            return false;
        }
        
        // Validate STDP parameters
        if (stdpLtpWeight < 0.0f) {
            error = "stdpLtpWeight must be non-negative";
            return false;
        }
        
        if (stdpLtdWeight < 0.0f) {
            error = "stdpLtdWeight must be non-negative";
            return false;
        }
        
        // Validate plasticity rates
        if (synaptogenesisRate < 0.0f || synaptogenesisRate > 0.001f) {
            error = "synaptogenesisRate should be between 0.0 and 0.001";
            return false;
        }
        
        if (pruningRate < 0.0f || pruningRate > 0.0001f) {
            error = "pruningRate should be between 0.0 and 0.0001";
            return false;
        }
        
        // Validate intervals
        if (replayInterval == 0) {
            error = "replayInterval must be greater than 0";
            return false;
        }
        
        if (consolidationInterval == 0) {
            error = "consolidationInterval must be greater than 0";
            return false;
        }
        
        if (consolidationInterval < replayInterval * 10) {
            error = "consolidationInterval should be at least 10x replayInterval for effective consolidation";
        }
        
        // Validate developmental parameters
        if (plasticityModulationStrength < 0.1f || plasticityModulationStrength > 2.0f) {
            error = "plasticityModulationStrength should be between 0.1 and 2.0";
            return false;
        }
        
        if (noveltyThreshold < 0.0f || noveltyThreshold > 1.0f) {
            error = "noveltyThreshold must be between 0.0 and 1.0";
            return false;
        }
        
        if (curiosityDecay < 0.5f || curiosityDecay > 0.999f) {
            error = "curiosityDecay should be between 0.5 and 0.999";
            return false;
        }
        
        // Validate memory parameters
        if (memoryDecayRate < 0.0f || memoryDecayRate > 0.1f) {
            error = "memoryDecayRate should be between 0.0 and 0.1";
            return false;
        }
        
        if (maxMemoryEpisodes == 0) {
            error = "maxMemoryEpisodes must be greater than 0";
            return false;
        }
        
        return true;
    }
    
    /**
     * Apply configuration to an existing Config object
     */
    void applyTo(std::shared_ptr<class Config> config) const {
        if (!config) return;
        
        // Core parameters
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(regionCount));
        config->set("connection_probability", connectionProbability);
        config->set("simulation_timestep", 0.001);  // Fixed timestep
        config->set("random_seed", static_cast<int64_t>(seed));
        
        // STDP parameters
        config->set("stdp_ltp_weight", stdpLtpWeight);
        config->set("stdp_ltd_weight", stdpLtdWeight);
        config->set("stdp_tau", 20.0f);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", synaptogenesisRate);
        config->set("pruning_rate", pruningRate);
        
        // Advanced integration parameters
        config->set("replay_interval", static_cast<size_t>(replayInterval));
        config->set("consolidation_interval", static_cast<size_t>(consolidationInterval));
        config->set("checkpoint_dir", "./checkpoints");
        
        // Novelty and curiosity
        config->set("novelty_threshold", noveltyThreshold);
        config->set("curiosity_decay", curiosityDecay);
        
        // Development
        config->set("developmental_stages_enabled", enableDevelopmentalStages);
        config->set("plasticity_modulation_strength", plasticityModulationStrength);
        
        // Debug and logging
        config->set("enable_detailed_logging", enableDetailedLogging);
        config->set("enable_profiling", enableProfiling);
    }
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
