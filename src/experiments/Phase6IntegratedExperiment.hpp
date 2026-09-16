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
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin") {}
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

    /**
     * Test neuromodulation flow to plasticity
     */
    bool testNeuromodulationFlow(std::shared_ptr<Brain> brain);
    
    /**
     * Test memory flow to neural processing
     */
    bool testMemoryFlow(std::shared_ptr<Brain> brain);
    
    /**
     * Get integration quality score (0.0 - 1.0)
     */
    float getIntegrationQualityScore(const Phase6IntegrationResult& result) const;
    
    /**
     * Get integration weaknesses report
     */
    std::string getIntegrationWeaknesses(const Phase6IntegrationResult& result) const;
    
    /**
     * Create an integration validator for the Python API
     */
    static std::unique_ptr<Phase6IntegrationValidator> createValidator();
    
    /**
     * Create a comprehensive integration test for Python
     */
    static std::unique_ptr<Phase6PythonIntegrationTest> createPythonIntegrationTest();
};

/**
 * Phase 6 integration validator for runtime monitoring
 */
class Phase6IntegrationValidator {
public:
    Phase6IntegrationValidator();
    ~Phase6IntegrationValidator();
    
    /**
     * Validate integration of a brain with agent and world
     */
    bool validate(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                  std::shared_ptr<SimpleWorld> world);
    
    /**
     * Run a comprehensive integration test
     */
    Phase6IntegrationResult testIntegration(const Phase6Config& config);
    
    /**
     * Get integration report with scores and recommendations
     */
    std::string getReport(const Phase6IntegrationResult& result) const;
};

/**
 * Python-friendly integration test wrapper
 */
class Phase6PythonIntegrationTest {
public:
    Phase6PythonIntegrationTest() = default;
    ~Phase6PythonIntegrationTest() = default;
    
    /**
     * Run integration test with Python-style interface
     */
    bool runTest() {
        Phase6Config config;
        config.neuronCount = 200;
        config.maxSteps = 1000;
        config.enableDevelopment = true;
        config.enableCuriosity = true;
        
        auto result = test.run(config);
        
        // Convert to Python-friendly result
        integrationReport = {
            "total_reward": result.totalReward,
            "avg_firing_rate": result.avgFiringRate,
            "integration_score": test.getIntegrationQualityScore(result),
            "weaknesses": test.getIntegrationWeaknesses(result),
            "systems": {
                "memory_working_memory": result.memoryWorkingMemoryIntegrated,
                "memory_episodic_memory": result.memoryEpisodicMemoryIntegrated,
                "neuromodulation": result.neuromodulationIntegrated,
                "prediction": result.predictionIntegrated,
                "development": result.developmentIntegrated
            }
        };
        
        return result.memoryWorkingMemoryIntegrated &&
               result.memoryEpisodicMemoryIntegrated &&
               result.neuromodulationIntegrated;
    }
    
    /**
     * Get integration report
     */
    std::string getReport() const { return integrationReport; }
    
    /**
     * Get integration status for easy checking
     */
    bool getStatus() const {
        return !integrationReport.empty();
    }
    
private:
    Phase6IntegratedExperiment test;
    std::string integrationReport;
};

} // namespace nlm
