// Phase6Demo - Comprehensive Phase 6 Integration Demonstration
// This demo showcases all Phase 6 integration achievements

#pragma once

#include <string>
#include <memory>

namespace nlm {

/**
 * Demo configuration for Phase 6 integration demo
 */
struct DemoConfig {
    size_t neuronCount;
    size_t maxSteps;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    std::string outputFile;
    
    // Integration test flags
    bool runIntegrationVerification;
    bool runMemoryIntegration;
    bool runNeuromodulationTest;
    bool runCheckpointTest;
    bool runReplayTest;
    
    DemoConfig()
        : neuronCount(1000)
        , maxSteps(10000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint.bin")
        , outputFile("")
        , runIntegrationVerification(true)
        , runMemoryIntegration(true)
        , runNeuromodulationTest(true)
        , runCheckpointTest(true)
        , runReplayTest(true) {}
};

/**
 * Phase6Demo - Comprehensive integration demonstration
 * This demo showcases all Phase 6 integration achievements
 */
class Phase6Demo {
public:
    Phase6Demo();
    ~Phase6Demo();
    
    // Run comprehensive Phase 6 demo
    void runComprehensiveDemo(const DemoConfig& config);
    
    // Print results to console
    void printResults(const Phase6IntegrationResult& result, double wallTime);
    
    // Save results to file
    void saveDemoResults(const Phase6IntegrationResult& result, double wallTime, const std::string& filename);
    
    // Help and version utilities
    void printHelp();
    void printVersion();
    
    // Command-line interface
    int run(int argc, char** argv);
};

} // namespace nlm
