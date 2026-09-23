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

void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo Help ===" << std::endl;
    std::cout << "Usage: nlm_phase6_demo [options]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  -v, --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  -q, --quiet                   Suppress non-error output" << std::endl;
    std::cout << "      --no-integration           Skip integration verification" << std::endl;
    std::cout << "      --no-full-experiment       Skip full integration experiment" << std::endl;
    std::cout << "      --no-checkpointing         Disable checkpointing test" << std::endl;
    std::cout << "      --no-replay                Disable replay test" << std::endl;
    std::cout << "      --no-development           Disable development test" << std::endl;
    std::cout << "      --neuron-count N           Set neuron count (default: 500)" << std::endl;
    std::cout << "      --steps N                  Set max steps (default: 2000)" << std::endl;
    std::cout << "      --config FILE              Load configuration from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_phase6_demo --verbose --neuron-count 1000 --steps 5000" << std::endl;
    std::cout << "  nlm_phase6_demo --quiet --no-integration --no-checkpointing" << std::endl;
}

int main(int argc, char* argv[]) {
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        }
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6Config config;
    bool verbose = false;
    bool quiet = false;
    bool runAllTests = true;
    bool skipIntegration = false;
    bool skipFullExperiment = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--no-integration") {
            skipIntegration = true;
        } else if (arg == "--no-full-experiment") {
            skipFullExperiment = true;
        } else if (arg == "--neuron-count") {
            if (i + 1 < argc) {
                config.neuronCount = std::stoi(argv[++i]);
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                config.maxSteps = std::stoi(argv[++i]);
            }
        } else if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                // TODO: Load configuration from file
                std::string configFile = argv[++i];
                std::cout << "Config file loading not yet implemented: " << configFile << std::endl;
            }
        }
    }
    
    // Initialize logging based on verbosity
    if (quiet) {
        Logger::getInstance().setLevel(Logger::Level::Error);
    } else if (verbose) {
        Logger::getInstance().setLevel(Logger::Level::Debug);
    } else {
        Logger::getInstance().setLevel(Logger::Level::Info);
    }
    
    // Log startup info
    if (!quiet) {
        std::cout << "Phase 6 Configuration:" << std::endl;
        std::cout << "  Neuron count: " << config.neuronCount << std::endl;
        std::cout << "  Max steps: " << config.maxSteps << std::endl;
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
        std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
        std::cout << std::endl;
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    if (!skipIntegration) {
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    // Test individual systems (conditionally)
    if (config.enableCheckpointing) {
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
    }
    
    if (config.enableReplay) {
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
    }
    
    // Run full experiment (conditionally)
    if (!skipFullExperiment) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
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
    }
    
    return 0;
}
