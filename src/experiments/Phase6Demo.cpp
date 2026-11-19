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

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>

using namespace nlm;

// Help function to display command-line options
void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo - Help ===" << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [OPTIONS]" << std::endl << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons N          Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S            Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --no-checkpoint      Disable checkpointing" << std::endl;
    std::cout << "  --no-replay          Disable replay system" << std::endl;
    std::cout << "  --no-development     Disable developmental system" << std::endl;
    std::cout << "  --config-file F      Load configuration from file" << std::endl;
    std::cout << "  --checkpoint-path P  Set checkpoint file path" << std::endl;
    std::cout << "  --help, -h           Show this help message" << std::endl << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  ./nlm_phase6_demo --config-file my_config.json" << std::endl << std::endl;
}

// Parse command-line arguments
Phase6Config parseCommandLine(int argc, char* argv[]) {
    Phase6Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg.substr(0, 9) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                config.neuronCount = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                config.neuronCount = std::stoull(argv[++i]);
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                config.maxSteps = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                config.maxSteps = std::stoull(argv[++i]);
            }
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg.substr(0, 13) == "--config-file") {
            if (arg.find('=') != std::string::npos) {
                // Would load from file (future enhancement)
            } else if (i + 1 < argc) {
                // Would load from file (future enhancement)
                ++i;
            }
        } else if (arg.substr(0, 16) == "--checkpoint-path") {
            if (arg.find('=') != std::string::npos) {
                config.checkpointPath = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                config.checkpointPath = argv[++i];
            }
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command-line arguments
    Phase6Config config = parseCommandLine(argc, argv);
    
    // Override default values with command-line arguments if provided
    if (argc > 1) {
        std::cout << "Command-line arguments parsed:" << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Steps: " << config.maxSteps << std::endl;
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        if (!config.checkpointPath.empty()) {
            std::cout << "  Checkpoint path: " << config.checkpointPath << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
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
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
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
