#pragma once

/**
 * Phase6Experiment - Phase 6 Final Integration Experiment
 * 
 * This experiment runs the comprehensive Phase 6 integration test that verifies
 * all brain systems are properly connected and functional together.
 * 
 * The experiment simulates a complete artificial brain lifecycle with:
 * - Memory formation and retrieval
 * - Neuromodulation-driven learning
 * - Prediction and planning capabilities
 * - Developmental changes over time
 * - Checkpoint persistence
 * - Memory replay and consolidation
 * 
 * This experiment is the culmination of all Phase 6 development and demonstrates
 * that the integrated brain system functions as a coherent artificial intelligence.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * Phase 6 Experiment Configuration
 * 
 * Extended configuration for Phase 6 integration experiments
 */
struct Phase6ExperimentConfig {
    // Basic experiment parameters
    uint64_t maxSteps = 10000;
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    
    // Feature toggles
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
    bool enableNovelty = true;
    
    // World configuration
    float worldWidth = 16.0f;
    float worldHeight = 16.0f;
    size_t visionWidth = 8;
    size_t visionHeight = 8;
    
    // File paths
    std::string checkpointPath = "./checkpoint_test.bin";
    std::string outputDir = ".";
    
    // Agent configuration
    bool enableRewardModulation = true;
    bool enableStructuralPlasticity = true;
    
    // Performance settings
    uint32_t threadCount = 0;  // 0 = auto
    bool enableMultithreading = false;
    bool enableSIMD = true;
    
    Phase6ExperimentConfig() = default;
};

/**
 * Phase 6 Experiment Result
 * 
 * Extended result structure with additional metrics
 */
struct Phase6ExperimentResult {
    // Phase 6 integration result
    Phase6IntegrationResult integration;
    
    // Additional experiment metrics
    uint64_t totalStepsCompleted;
    uint64_t totalEpisodes;
    
    // Performance metrics
    float averageRewardPerStep;
    float rewardEfficiency;
    float learningProgress;
    
    // System health metrics
    float systemStability;
    float memoryRetention;
    float behavioralFlexibility;
    
    // Timing metrics
    double simulationWallClockTime;
    double realTimePerStep;
    
    Phase6ExperimentResult() = default;
};

/**
 * Phase 6 Final Integration Experiment
 * 
 * The complete Phase 6 integration test that validates all brain systems
 * working together as a coherent artificial brain.
 */
class Phase6Experiment {
public:
    Phase6Experiment();
    ~Phase6Experiment();
    
    /**
     * Run the complete Phase 6 integration experiment
     * 
     * This runs a comprehensive test of all Phase 6 capabilities with:
     * - Large-scale neural simulation
     * - Agent-world interaction
     * - Reward-based learning
     * - Memory formation and consolidation
     * - Developmental progression
     * - System validation and verification
     */
    Phase6ExperimentResult run(const Phase6ExperimentConfig& config);
    
    /**
     * Get the experiment result
     */
    const Phase6IntegrationResult& getIntegrationResult() const { return integrationResult_; }
    
    /**
     * Print experiment summary
     */
    void printSummary() const;
    
    /**
     * Check if experiment passed all validation tests
     */
    bool success() const;
    
    /**
     * Get detailed integration status
     */
    std::string getIntegrationStatus() const;
    
private:
    Phase6IntegrationResult integrationResult_;
    
    /**
     * Run Phase 6 experiment with agent-world simulation
     */
    Phase6IntegrationResult runWithAgentWorld(const Phase6ExperimentConfig& config);
    
    /**
     * Run Phase 6 experiment with pure neural simulation
     */
    Phase6IntegrationResult runPureNeural(const Phase6ExperimentConfig& config);
    
    /**
     * Run Phase 6 experiment with multiple agents
     */
    Phase6IntegrationResult runMultiAgent(const Phase6ExperimentConfig& config);
    
    /**
     * Validate experiment results
     */
    bool validateResults(const Phase6IntegrationResult& result) const;
    
    /**
     * Print experiment configuration
     */
    void printConfiguration(const Phase6ExperimentConfig& config) const;
    
    /**
     * Create experiment configuration from defaults
     */
    static Phase6ExperimentConfig createDefaultConfig();
};

} // namespace nlm
