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
    
    // Advanced configuration options for enhanced demos
    bool verboseLogging;
    bool quietMode;
    bool benchmarkMode;
    bool showProgress;
    bool exportResults;
    std::string resultsFile;
    double simulationSpeed;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , verboseLogging(false)
        , quietMode(false)
        , benchmarkMode(false)
        , showProgress(false)
        , exportResults(false)
        , resultsFile("./phase6_results.json")
        , simulationSpeed(1.0) {}
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
    
    // Logging
    void logStatus() const;
    
    /**
     * Generate JSON output for results
     */
    std::string toJSON() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"memoryWorkingMemoryIntegrated\": " << (memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
        oss << "  \"memoryEpisodicMemoryIntegrated\": " << (memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
        oss << "  \"neuromodulationIntegrated\": " << (neuromodulationIntegrated ? "true" : "false") << ",\n";
        oss << "  \"predictionIntegrated\": " << (predictionIntegrated ? "true" : "false") << ",\n";
        oss << "  \"developmentIntegrated\": " << (developmentIntegrated ? "true" : "false") << ",\n";
        oss << "  \"checkpointingWorks\": " << (checkpointingWorks ? "true" : "false") << ",\n";
        oss << "  \"replayWorks\": " << (replayWorks ? "true" : "false") << ",\n";
        oss << "  \"totalReward\": " << totalReward << ",\n";
        oss << "  \"avgFiringRate\": " << avgFiringRate << ",\n";
        oss << "  \"avgSynapticWeight\": " << avgSynapticWeight << ",\n";
        oss << "  \"memoryEpisodesStored\": " << memoryEpisodesStored << ",\n";
        oss << "  \"noveltyLevel\": " << noveltyLevel << ",\n";
        oss << "  \"curiosityLevel\": " << curiosityLevel << ",\n";
        oss << "  \"dopamineLevel\": " << dopamineLevel << ",\n";
        oss << "  \"totalWallClockTime\": " << totalWallClockTime << "\n";
        oss << "}\n";
        return oss.str();
    }
    
    /**
     * Generate human-readable summary
     */
    std::string summary() const {
        std::ostringstream oss;
        oss << "=== NLM Phase 6 Integration Results ===\n";
        oss << "System Integration:\n";
        oss << "  Working Memory: " << (memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "  Episodic Memory: " << (memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "  Neuromodulation: " << (neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "  Prediction: " << (predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "  Development: " << (developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        oss << "  Checkpointing: " << (checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
        oss << "  Replay: " << (replayWorks ? "WORKING" : "NOT WORKING") << "\n";
        oss << "\nPerformance Metrics:\n";
        oss << "  Total Reward: " << totalReward << "\n";
        oss << "  Average Firing Rate: " << avgFiringRate << "\n";
        oss << "  Average Synaptic Weight: " << avgSynapticWeight << "\n";
        oss << "  Memory Episodes Stored: " << memoryEpisodesStored << "\n";
        oss << "  Novelty Level: " << noveltyLevel << "\n";
        oss << "  Curiosity Level: " << curiosityLevel << "\n";
        oss << "  Dopamine Level: " << dopamineLevel << "\n";
        oss << "\nTiming:\n";
        oss << "  Wall Clock Time: " << totalWallClockTime << "s\n";
        oss << "=== End Results ===\n";
        return oss.str();
    }
};
