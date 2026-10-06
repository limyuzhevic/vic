// /**
//  * Phase 6 Demo - Integration Test
//  *
//  * This demo runs the Phase 6 integration experiment to verify
//  * that all brain systems are properly connected.
//  */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

// Constants for Phase 6 demo
namespace {
    constexpr const char* PHASE6_DEMO_NAME = "=== NLM Phase 6 Integration Demo ===";
    constexpr const char* INTEGRATION_TEST_NAME = "Testing the integrated artificial brain...";
    constexpr const char* INTEGRATION_VERIFICATION_NAME = "--- Integration Verification ---";
    constexpr const char* MEMORY_INTEGRATION_TEST_NAME = "--- Memory Integration Test ---";
    constexpr const char* NEUROMODULATION_INTEGRATION_TEST_NAME = "--- Neuromodulation Integration Test ---";
    constexpr const char* CHECKPOINT_TEST_NAME = "--- Checkpoint Test ---";
    constexpr const char* REPLAY_TEST_NAME = "--- Replay Test ---";
    constexpr const char* FULL_INTEGRATION_EXPERIMENT_NAME = "--- Full Integration Experiment ---";
    constexpr const char* FINAL_RESULTS_NAME = "=== FINAL RESULTS ===";
    constexpr const char* INTEGRATION_STATUS_NAME = "=== INTEGRATION STATUS ===";
} // anonymous namespace

int main(int argc, char* argv[]) {
    std::cout << PHASE6_DEMO_NAME << std::endl;
    std::cout << INTEGRATION_TEST_NAME << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << INTEGRATION_VERIFICATION_NAME << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << MEMORY_INTEGRATION_TEST_NAME << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << NEUROMODULATION_INTEGRATION_TEST_NAME << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << CHECKPOINT_TEST_NAME << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << REPLAY_TEST_NAME << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << FULL_INTEGRATION_EXPERIMENT_NAME << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << FINAL_RESULTS_NAME << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << INTEGRATION_STATUS_NAME << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
