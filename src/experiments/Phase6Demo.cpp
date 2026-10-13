/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
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
    
    // Parse command line arguments for custom configuration
    Phase6Config config;
    
    // Default settings for demo
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--quick") {
            // Quick demo with minimal settings
            config.neuronCount = 100;
            config.maxSteps = 500;
            config.enableCheckpointing = false;
            config.enableReplay = false;
            config.enableDevelopment = false;
        } else if (arg == "--demo") {
            // Compact demo output
            std::cout << "[DEMO MODE] - Compact output enabled" << std::endl;
        } else if (arg == "--verbose") {
            // Verbose output
            Logger::getInstance().setLevel(Logger::Level::Debug);
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "=== NLM Phase 6 Demo Usage ===" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --quick        Run quick demo with minimal settings" << std::endl;
            std::cout << "  --demo         Compact output mode" << std::endl;
            std::cout << "  --verbose      Enable debug logging" << std::endl;
            std::cout << "  --help, -h     Show this help message" << std::endl;
            std::cout << "Default: Full demo with checkpointing and development" << std::endl;
            return 0;
        }
    }
    
    // Run full experiment with configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
    if (config.enableDevelopment) std::cout << "Development: ENABLED" << std::endl;
    if (config.enableCheckpointing) std::cout << "Checkpointing: ENABLED" << std::endl;
    if (config.enableReplay) std::cout << "Replay: ENABLED" << std::endl;
    std::cout << std::endl;
    
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
    std::cout << std::endl;
    
    // Display integration summary
    std::cout << "=== INTEGRATION SUMMARY ===" << std::endl;
    int connectedSystems = 0;
    if (result.memoryWorkingMemoryIntegrated) connectedSystems++;
    if (result.memoryEpisodicMemoryIntegrated) connectedSystems++;
    if (result.neuromodulationIntegrated) connectedSystems++;
    if (result.predictionIntegrated) connectedSystems++;
    if (result.developmentIntegrated) connectedSystems++;
    
    std::cout << "Connected Systems: " << connectedSystems << "/5" << std::endl;
    std::cout << "Integration Success Rate: " << (connectedSystems * 20.0f) << "%" << std::endl;
    
    if (connectedSystems == 5) {
        std::cout << "✓ PHASE 6 INTEGRATION COMPLETE - All systems are connected!" << std::endl;
    } else {
        std::cout << "⚠ Integration incomplete - Some systems need attention" << std::endl;
    }
    
    return 0;
}
