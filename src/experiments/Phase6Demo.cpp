// Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

using namespace nlm;

// Improved Phase 6 Demo with better argument parsing and validation
int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    std::string configFile = "";
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            std::cout << "Usage: nlm_phase6_demo [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --neuron-count N     Number of neurons (default: 500)" << std::endl;
            std::cout << "  --max-steps S        Maximum simulation steps (default: 2000)" << std::endl;
            std::cout << "  --no-checkpoint       Disable checkpointing" << std::endl;
            std::cout << "  --no-replay           Disable replay" << std::endl;
            std::cout << "  --no-development      Disable development" << std::endl;
            std::cout << "  --config file         Load configuration from file" << std::endl;
            std::cout << "  --help                Show this help message" << std::endl;
            return 0;
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg.find("--neuron-count=") == 0) {
            neuronCount = std::stoul(arg.substr(14));
        } else if (arg.find("--max-steps=") == 0) {
            maxSteps = std::stoul(arg.substr(11));
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        }
    }
    
    // Initialize logging with better level control
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    std::cout << "=== Configuration ===" << std::endl;
    std::cout << "  Neuron count: " << neuronCount << std::endl;
    std::cout << "  Max steps: " << maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    if (!configFile.empty()) {
        std::cout << "  Config file: " << configFile << std::endl;
    }
    std::cout << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with configurable settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Return appropriate exit code based on results
    if (!result.memoryWorkingMemoryIntegrated || !result.memoryEpisodicMemoryIntegrated || 
        !result.neuromodulationIntegrated || !result.checkpointingWorks) {
        std::cerr << "ERROR: Some system integrations failed!" << std::endl;
        return 1;
    }
    
    std::cout << "=== SUCCESS === All brain systems integrated and functioning!" << std::endl;
    return 0;
}
