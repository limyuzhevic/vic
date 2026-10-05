#pragma once

/**
 * Phase6.hpp - Phase 6 Final Integration Definitions
 * 
 * This file contains all the definitions needed for Phase 6 integration test,
 * including configuration structures, result structures, and function prototypes.
 */

#include "experiments/Experiment.hpp"
#include "experiments/Metrics.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <chrono>

namespace nlm {

/**
 * Configuration for Phase 6 integration experiment
 * Defines all parameters needed for comprehensive brain system integration testing
 */
struct Phase6Config {
    uint64_t maxSteps;                    // Maximum simulation steps
    size_t neuronCount;                   // Number of neurons in the brain
    size_t regionCount;                   // Number of brain regions
    float connectionProbability;          // Probability of synaptic connections
    bool enableCheckpointing;             // Enable checkpoint save/load testing
    bool enableReplay;                    // Enable replay system testing
    bool enableDevelopment;               // Enable development system testing
    std::string checkpointPath;           // Path for checkpoint files
    std::string environmentType;          // Type of environment to use
    size_t environmentSize;               // Size of the environment
    
    // Default constructor with Phase 6 integration parameters
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , environmentType("SimpleWorld")
        , environmentSize(16) {}
    
    // Helper method to create configuration from existing Config object
    static Phase6Config fromConfig(const Config& config) {
        Phase6Config phase6Config;
        
        // Map configuration keys to Phase6Config fields
        if (config.has("max_simulation_steps")) {
            phase6Config.maxSteps = config.getOr<uint64_t>("max_simulation_steps", 10000);
        }
        if (config.has("neuron_count")) {
            phase6Config.neuronCount = config.getOr<size_t>("neuron_count", 1000);
        }
        if (config.has("region_count")) {
            phase6Config.regionCount = config.getOr<size_t>("region_count", 1);
        }
        if (config.has("connection_probability")) {
            phase6Config.connectionProbability = config.getOr<float>("connection_probability", 0.1f);
        }
        if (config.has("enable_checkpointing")) {
            phase6Config.enableCheckpointing = config.getOr<bool>("enable_checkpointing", true);
        }
        if (config.has("enable_replay")) {
            phase6Config.enableReplay = config.getOr<bool>("enable_replay", true);
        }
        if (config.has("enable_development")) {
            phase6Config.enableDevelopment = config.getOr<bool>("enable_development", true);
        }
        if (config.has("checkpoint_path")) {
            phase6Config.checkpointPath = config.getOr<std::string>("checkpoint_path", "./checkpoint_test.bin");
        }
        if (config.has("environment_type")) {
            phase6Config.environmentType = config.getOr<std::string>("environment_type", "SimpleWorld");
        }
        if (config.has("environment_size")) {
            phase6Config.environmentSize = config.getOr<size_t>("environment_size", 16);
        }
        
        return phase6Config;
    }
};

/**
 * Integration test results for Phase 6
 * Contains comprehensive metrics for all integrated brain systems
 */
struct Phase6IntegrationResult {
    // System integration status - boolean flags for each system
    bool memoryWorkingMemoryIntegrated;    // Working memory properly connected
    bool memoryEpisodicMemoryIntegrated;   // Episodic memory properly connected
    bool memoryAssociativeMemoryIntegrated; // Associative memory properly connected
    bool neuromodulationIntegrated;        // Neuromodulation systems integrated
    bool predictionIntegrated;             // Prediction system integrated
    bool developmentIntegrated;            // Development system integrated
    bool checkpointingWorks;               // Checkpoint save/load functional
    bool replayWorks;                      // Replay system functional
    bool cognitionIntegrated;              // Cognitive systems integrated
    bool environmentalInteractionIntegrated; // Environmental interaction integrated
    
    // Detailed metrics for each system
    struct SystemMetrics {
        float memoryEfficiency;             // Memory storage efficiency
        float learningRate;                 // Learning speed
        float stability;                     // System stability
        float adaptability;                  // Ability to adapt
        float energyEfficiency;             // Energy consumption efficiency
    };
    
    SystemMetrics systemMetrics;           // Overall system performance metrics
    
    // Performance metrics
    float totalReward;                      // Total reward accumulated
    float avgFiringRate;                    // Average neural firing rate
    float avgSynapticWeight;                // Average synaptic weight
    float memoryEpisodesStored;             // Number of episodes stored in episodic memory
    float memoryAssociations;               // Number of associations in associative memory
    float noveltyLevel;                     // Novelty detection level
    float curiosityLevel;                   // Curiosity motivation level
    float dopamineLevel;                    // Dopamine neuromodulation level
    float predictionAccuracy;               // Prediction system accuracy
    float planningSuccessRate;              // Planning system success rate
    float attentionFocus;                   // Attention system focus
    float developmentalProgress;            // Developmental stage progress
    
    // Timing metrics
    time_t startTime;                       // Experiment start timestamp
    time_t endTime;                         // Experiment end timestamp
    double totalWallClockTime;              // Real time elapsed
    double averageStepTime;                 // Average time per simulation step
    
    // System state metrics
    size_t activeNeuronCount;               // Number of active neurons
    size_t totalNeuronCount;                // Total number of neurons
    size_t totalSynapseCount;               // Total number of synapses
    size_t firingNeuronCount;               // Number of firing neurons
    size_t workingMemoryActiveTraces;       // Active traces in working memory
    size_t episodicMemoryEpisodeCount;      // Number of stored episodes
    size_t associativeMemoryAssociationCount; // Number of stored associations
    
    // Quality metrics
    float memoryCoherence;                  // Memory coherence score
    float behavioralConsistency;            // Behavioral consistency
    float learningProgress;                 // Learning progress over time
    float memoryRetention;                  // Memory retention quality
    
    // Default constructor initializes all metrics
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , memoryAssociativeMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , cognitionIntegrated(false)
        , environmentalInteractionIntegrated(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , memoryAssociations(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , predictionAccuracy(0.0f)
        , planningSuccessRate(0.0f)
        , attentionFocus(0.0f)
        , developmentalProgress(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0)
        , averageStepTime(0.0)
        , activeNeuronCount(0)
        , totalNeuronCount(0)
        , totalSynapseCount(0)
        , firingNeuronCount(0)
        , workingMemoryActiveTraces(0)
        , episodicMemoryEpisodeCount(0)
        , associativeMemoryAssociationCount(0)
        , memoryCoherence(0.0f)
        , behavioralConsistency(0.0f)
        , learningProgress(0.0f)
        , memoryRetention(0.0f)
    {
        systemMetrics = SystemMetrics();
    }
    
    // Calculate derived metrics
    void calculateDerivedMetrics() {
        if (totalWallClockTime > 0.0) {
            averageStepTime = totalWallClockTime / maxSteps;
        }
        
        // Calculate memory efficiency score
        if (totalNeuronCount > 0) {
            systemMetrics.memoryEfficiency = 
                static_cast<float>(episodicMemoryEpisodeCount + associativeMemoryAssociationCount) / 
                static_cast<float>(totalNeuronCount);
        }
        
        // Calculate learning rate (simplified)
        systemMetrics.learningRate = totalReward / totalWallClockTime;
        
        // Calculate stability (simplified - based on consistency of metrics)
        systemMetrics.stability = 1.0f - std::abs(learningProgress - 0.5f); // Heuristic
        
        // Calculate adaptability (simplified - based on change in metrics)
        systemMetrics.adaptability = learningProgress;
        
        // Calculate energy efficiency (simplified - based on firing rate)
        if (avgFiringRate > 0.0f) {
            systemMetrics.energyEfficiency = 1.0f / avgFiringRate;
        }
    }
    
    // Get integration quality score (0.0 to 1.0)
    float getIntegrationScore() const {
        float score = 0.0f;
        int systemCount = 0;
        
        // Count integrated systems
        if (memoryWorkingMemoryIntegrated) score += 0.1f; systemCount++;
        if (memoryEpisodicMemoryIntegrated) score += 0.1f; systemCount++;
        if (memoryAssociativeMemoryIntegrated) score += 0.1f; systemCount++;
        if (neuromodulationIntegrated) score += 0.1f; systemCount++;
        if (predictionIntegrated) score += 0.1f; systemCount++;
        if (developmentIntegrated) score += 0.1f; systemCount++;
        if (cognitionIntegrated) score += 0.1f; systemCount++;
        if (environmentalInteractionIntegrated) score += 0.1f; systemCount++;
        
        // Adjust for functional systems
        if (checkpointingWorks) score += 0.05f;
        if (replayWorks) score += 0.05f;
        
        // Cap at 1.0
        return std::min(1.0f, score);
    }
    
    // Get integration status as human-readable string
    std::string getIntegrationStatus() const {
        float score = getIntegrationScore();
        if (score >= 0.9f) return "EXCELLENT";
        if (score >= 0.8f) return "GOOD";
        if (score >= 0.7f) return "ACCEPTABLE";
        if (score >= 0.6f) return "NEEDS_IMPROVEMENT";
        return "POOR";
    }
};

/**
 * Phase 6 integration test experiment
 * Comprehensive test that verifies all brain systems are properly connected
 * and working together as an integrated artificial brain
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete Phase 6 integration test
     * @param config Phase 6 configuration parameters
     * @return Integration test results
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification
     * @return true if basic integration is successful
     */
    bool verifyIntegration();
    
    /**
     * Test memory system integration
     * @return true if memory systems are properly connected
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration
     * @return true if neuromodulation systems are properly connected
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test prediction system integration
     * @return true if prediction system is properly connected
     */
    bool testPredictionIntegration();
    
    /**
     * Test development system integration
     * @return true if development system is properly connected
     */
    bool testDevelopmentIntegration();
    
    /**
     * Test cognition system integration
     * @return true if cognition systems are properly connected
     */
    bool testCognitionIntegration();
    
    /**
     * Test environmental interaction integration
     * @return true if environmental interaction is properly connected
     */
    bool testEnvironmentalInteractionIntegration();
    
    /**
     * Test checkpoint save/load functionality
     * @param config Experiment configuration
     * @return true if checkpointing works
     */
    bool testCheckpointing(const Phase6Config& config);
    
    /**
     * Test replay system functionality
     * @param config Experiment configuration
     * @return true if replay system works
     */
    bool testReplay(const Phase6Config& config);
    
    /**
     * Run all integration tests
     * @param config Phase 6 configuration
     * @return comprehensive integration results
     */
    Phase6IntegrationResult runAllTests(const Phase6Config& config);
    
    /**
     * Print integration results in human-readable format
     * @param result Integration test results
     */
    static void printResults(const Phase6IntegrationResult& result);
    
    /**
     * Save integration results to file
     * @param result Integration test results
     * @param filepath Output file path
     * @return true if successful
     */
    static bool saveResults(const Phase6IntegrationResult& result, const std::string& filepath);
    
    /**
     * Load integration results from file
     * @param filepath Input file path
     * @return Integration test results
     */
    static Phase6IntegrationResult loadResults(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods for integration testing
    bool verifySystemConnection(const std::string& systemName, bool isConnected) const;
    bool runSystemTest(const std::string& testName, bool (*testFunc)(), std::chrono::duration<double>& testTime);
    void logSystemStatus(const std::string& systemName, bool isIntegrated, float metric) const;
};

/**
 * Factory function to create a Phase6IntegratedExperiment
 * @return Shared pointer to experiment instance
 */
std::shared_ptr<Phase6IntegratedExperiment> createPhase6Experiment();

/**
 * Convenience function to run a quick integration test
 * @param config Phase 6 configuration
 * @return Integration test results
 */
Phase6IntegrationResult runPhase6IntegrationTest(const Phase6Config& config);

} // namespace nlm
