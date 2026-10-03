// NLM Phase 6 Python Integration Manager
// Provides advanced utilities for running Phase 6 integration experiments from Python

#pragma once

#include "Phase6IntegratedExperiment.hpp"
#include <string>
#include <iostream>

namespace nlm {

/**
 * NLMPhase6PythonManager provides advanced utilities for running Phase 6
 * integration experiments from the Python bindings.
 */
class NLMPhase6PythonManager {
public:
    NLMPhase6PythonManager();
    
    /**
     * Create advanced Phase 6 configuration with custom parameters
     */
    static Phase6Config createAdvancedConfig(
        uint64_t maxSteps = 5000,
        size_t neuronCount = 800,
        size_t regionCount = 2,
        float connectionProbability = 0.08f,
        bool enableCheckpointing = true,
        bool enableReplay = true,
        bool enableDevelopment = true,
        const std::string& checkpointPath = "./checkpoint.bin"
    );
    
    /**
     * Run the complete integration test
     */
    Phase6IntegrationResult runFullIntegrationTest();
    
    /**
     * Quick integration verification
     */
    bool quickVerification();
    
    /**
     * Test individual systems
     */
    void testIndividualSystems();
    
    /**
     * Save checkpoint
     */
    bool saveCheckpoint(const std::string& filepath);
    
    /**
     * Load checkpoint
     */
    bool loadCheckpoint(const std::string& filepath);
    
    /**
     * Print and format integration results
     */
    void printResults(const Phase6IntegrationResult& result);
    
    /**
     * Create configuration from Python parameters
     */
    static std::string createConfigFromParams(
        const std::string& name,
        uint64_t maxSteps,
        size_t neuronCount,
        size_t regionCount,
        float connectionProbability,
        bool enableCheckpointing,
        bool enableReplay,
        bool enableDevelopment
    );
    
    /**
     * Validate experiment results
     */
    static bool validateResults(const Phase6IntegrationResult& result);
    
    /**
     * Print experiment summary
     */
    static void printExperimentSummary(const Phase6IntegrationResult& result);
    
    /**
     * Run experiment with custom settings
     */
    Phase6IntegrationResult runExperimentWithSettings(
        const std::string& experimentName,
        uint64_t steps,
        size_t neurons,
        bool checkpoint,
        bool replay,
        bool development
    );
};

} // namespace nlm