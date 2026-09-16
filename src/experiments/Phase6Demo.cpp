/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all systems into a coherent brain.   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void parseArguments(int argc, char* argv[], Phase6Config& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--quick") {
            config.neuronCount = 100;
            config.maxSteps = 500;
            config.enableCheckpointing = true;
            config.enableReplay = true;
            config.enableDevelopment = false;
        } else if (arg == "--demo") {
            config.neuronCount = 300;
            config.maxSteps = 1500;
            config.enableCheckpointing = true;
            config.enableReplay = true;
            config.enableDevelopment = true;
        } else if (arg == "--full") {
            config.neuronCount = 1000;
            config.maxSteps = 5000;
            config.enableCheckpointing = true;
            config.enableReplay = true;
            config.enableDevelopment = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                config.checkpointPath = arg.substr(arg.find('=') + 1);
            }
        }
    }
}

void printUsage() {
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << "  --quick      Run quick integration test (100 neurons, 500 steps)" << std::endl;
    std::cout << "  --demo       Run demo test (300 neurons, 1500 steps)" << std::endl;
    std::cout << "  --full       Run full integration test (1000 neurons, 5000 steps)" << std::endl;
    std::cout << "  --config=file Use checkpoint file path" << std::endl;
    std::cout << "  --help       Show this help message" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    // Check for help
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
            printUsage();
            return 0;
        }
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Parse command line arguments
    Phase6Config config;
    parseArguments(argc, argv, config);
    
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
    
    // Run full experiment with configuration from arguments
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
    std::cout << "Checkpointing: " << (config.enableCheckpointing ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Replay: " << (config.enableReplay ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Development: " << (config.enableDevelopment ? "ENABLED" : "DISABLED") << std::endl;
    
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
    
    return 0;
}
