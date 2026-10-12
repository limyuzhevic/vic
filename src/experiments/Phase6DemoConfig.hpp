// NLM (熙然) - Neural Learning Machine
// Phase 6 Demo Configuration
//
// This header declares the configuration structure for the Phase 6 demo,
// providing options for controlling which tests to run and with what parameters.

#pragma once

#include <string>
#include <cstdint>

namespace nlm {

/**
 * Configuration structure for Phase 6 demo
 * 
 * This structure contains all the configuration options for the Phase 6
 * integration demo, including test selection and experiment parameters.
 */
struct Phase6DemoConfig {
    std::string name = "NLM Phase 6 Integration Demo";
    std::string description = "Testing the integrated artificial brain systems";
    
    // Test selection flags
    bool runIntegrationVerification = true;
    bool runMemoryTest = true;
    bool runNeuromodulationTest = true;
    bool runCheckpointTest = true;
    bool runReplayTest = true;
    bool runFullExperiment = true;
    
    // Full experiment configuration
    uint64_t maxSteps = 2000;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_demo.bin";
    
    Phase6DemoConfig() = default;
    
    /**
     * Parse command line arguments to configure demo
     * 
     * @param argc Argument count
     * @param argv Argument vector
     * @return true if parsing succeeded, false if help was shown
     */
    bool parseArgs(int argc, char* argv[]);
    
    /**
     * Print the current configuration
     */
    void printConfig() const;
    
    /**
     * Generate a report string from integration results
     * 
     * @param result The integration results to report on
     * @return Formatted report string
     */
    std::string generateReport(const Phase6IntegrationResult& result) const;
};

} // namespace nlm
