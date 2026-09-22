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

#include "Experiment.hpp"
#include "Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

/**
 * Custom exceptions for Phase 6 integration experiments
 */
class Phase6Error : public std::runtime_error {
public:
    explicit Phase6Error(const std::string& msg) : std::runtime_error(msg) {}
};

class IntegrationVerificationError : public Phase6Error {
public:
    explicit IntegrationVerificationError(const std::string& msg) : Phase6Error(msg) {}
};

class SystemIntegrationError : public Phase6Error {
public:
    explicit SystemIntegrationError(const std::string& msg) : Phase6Error(msg) {}
};

/**
 * Enhanced configuration for Phase 6 integration experiment
 * Organized by system categories for better readability and management
 */
struct Phase6Config {
    // Core simulation parameters
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    
    // System enable flags
    struct SystemEnableFlags {
        bool enableCheckpointing;
        bool enableReplay;
        bool enableDevelopment;
    } systems;
    
    // File paths and settings
    std::string checkpointPath;
    
    // Configuration validation
    bool isValid() const {
        return neuronCount > 0 && regionCount > 0 && 
               connectionProbability >= 0.0f && connectionProbability <= 1.0f &&
               maxSteps > 0;
    }
    
    // Phase6Config constructor with sensible defaults
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , checkpointPath("./checkpoint_test.bin") {
        systems.enableCheckpointing = true;
        systems.enableReplay = true;
        systems.enableDevelopment = true;
    }
};

/**
 * Comprehensive integration test results
 * Organized by system category for better analysis
 */
struct Phase6IntegrationResult {
    // System integration status
    struct SystemStatus {
        bool memoryWorkingMemoryIntegrated;
        bool memoryEpisodicMemoryIntegrated;
        bool neuromodulationIntegrated;
        bool predictionIntegrated;
        bool developmentIntegrated;
        bool checkpointingWorks;
        bool replayWorks;
        
        SystemStatus() 
            : memoryWorkingMemoryIntegrated(false)
            , memoryEpisodicMemoryIntegrated(false)
            , neuromodulationIntegrated(false)
            , predictionIntegrated(false)
            , developmentIntegrated(false)
            , checkpointingWorks(false)
            , replayWorks(false) {}
            
        // Helper to check if all systems are integrated
        bool allSystemsIntegrated() const {
            return memoryWorkingMemoryIntegrated && memoryEpisodicMemoryIntegrated &&
                   neuromodulationIntegrated && predictionIntegrated &&
                   developmentIntegrated && checkpointingWorks && replayWorks;
        }
    } systemStatus;
    
    // Metrics organized by category
    struct Metrics {
        // Performance metrics
        float totalReward;
        float avgFiringRate;
        float avgSynapticWeight;
        
        // Memory metrics
        float memoryEpisodesStored;
        
        // Neuromodulation metrics
        float noveltyLevel;
        float curiosityLevel;
        float dopamineLevel;
        
        // Temporal metrics
        time_t startTime;
        time_t endTime;
        double totalWallClockTime;
        
        Metrics()
            : totalReward(0.0f)
            , avgFiringRate(0.0f)
            , avgSynapticWeight(0.0f)
            , memoryEpisodesStored(0.0f)
            , noveltyLevel(0.0f)
            , curiosityLevel(0.0f)
            , dopamineLevel(0.0f)
            , startTime(0)
            , endTime(0)
            , totalWallClockTime(0.0) {}
    } metrics;
    
    // Helper methods for result summary
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Phase 6 Integration Results:" << std::endl;
        ss << "  Overall Integration: " << (systemStatus.allSystemsIntegrated() ? "SUCCESS" : "FAILURE") << std::endl;
        ss << "  Total Reward: " << metrics.totalReward << std::endl;
        ss << "  Average Firing Rate: " << metrics.avgFiringRate << " Hz" << std::endl;
        ss << "  Memory Episodes: " << metrics.memoryEpisodesStored << std::endl;
        ss << "  Wall Clock Time: " << metrics.totalWallClockTime << " s" << std::endl;
        return ss.str();
    }
};

/**
 * Phase 6 integrated experiment runner
 * Provides clean interface for running comprehensive integration tests
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test with comprehensive error handling
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification (backward compatibility)
     */
    bool verifyIntegration();
    
    /**
     * Test memory integration (backward compatibility)
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration (backward compatibility)
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test checkpoint save/load (backward compatibility)
     */
    bool testCheckpointing();
    
    /**
     * Test replay system (backward compatibility)
     */
    bool testReplay();
    
    /**
     * Enhanced verification with detailed error reporting
     */
    bool verifyIntegrationDetailed(std::vector<std::string>& errors);
    
    /**
     * Create a brain instance with configuration
     * Centralized brain creation logic
     */
    static std::shared_ptr<Brain> createBrain(const Phase6Config& config);
    
    /**
     * Initialize agent with brain and world
     * Centralized agent initialization logic
     */
    static void initializeAgent(std::shared_ptr<Brain>& brain, SimpleWorld& world);
    
private:
    // Private helper methods for organization
    void setupConfiguration(std::shared_ptr<Config> cfg, const Phase6Config& phase6Config);
    void runSimulation(std::shared_ptr<Brain>& brain, std::shared_ptr<SimpleWorld>& world,
                      const Phase6Config& config, Phase6IntegrationResult& result);
    void verifyAllSystemsIntegration(std::shared_ptr<Brain>& brain, Phase6IntegrationResult& result);
    
    // Private implementation
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
