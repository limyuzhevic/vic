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

// Simple command-line argument parser for demo
struct DemoArgs {
    int neuronCount = 500;
    int maxSteps = 2000;
    bool enableCheckpointing = false;
    bool enableReplay = false;
    bool enableDevelopment = true;
    bool verbose = false;
};

DemoArgs parseDemoArgs(int argc, char* argv[]) {
    DemoArgs args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--neurons" || arg == "--neuron-count") {
            if (i + 1 < argc) {
                args.neuronCount = std::stoi(argv[++i]);
            }
        } else if (arg == "--steps" || arg == "--max-steps") {
            if (i + 1 < argc) {
                args.maxSteps = std::stoi(argv[++i]);
            }
        } else if (arg == "--enable-checkpointing") {
            args.enableCheckpointing = true;
        } else if (arg == "--enable-replay") {
            args.enableReplay = true;
        } else if (arg == "--disable-development") {
            args.enableDevelopment = false;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        }
    }

    return args;
}

void printHelp(const std::string& programName) {
    std::cout << "NLM Phase 6 Demo - Integration Test" << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons=N, --neuron-count=N    Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=N, --max-steps=N          Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --enable-checkpointing            Enable checkpoint save/load testing" << std::endl;
    std::cout << "  --enable-replay                   Enable replay system testing" << std::endl;
    std::cout << "  --disable-development             Disable development system" << std::endl;
    std::cout << "  --verbose, -v                    Enable verbose logging" << std::endl;
    std::cout << "  --help, -h                        Show this help message" << std::endl;
    std::cout << "  --version                         Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --neurons=1000 --steps=5000" << std::endl;
    std::cout << "  " << programName << " --enable-checkpointing --verbose" << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    DemoArgs args = parseDemoArgs(argc, argv);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << "Neurons: " << args.neuronCount << ", Steps: " << args.maxSteps << std::endl;
    if (args.verbose) {
        std::cout << "Verbose logging enabled." << std::endl;
    }
    std::cout << std::endl;
    
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
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration: " << args.neuronCount << " neurons, " << args.maxSteps << " steps" << std::endl;
    std::cout << "Development: " << (args.enableDevelopment ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Checkpointing: " << (args.enableCheckpointing ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "Replay: " << (args.enableReplay ? "ENABLED" : "DISABLED") << std::endl;
    
    Phase6Config config;
    config.neuronCount = args.neuronCount;
    config.maxSteps = args.maxSteps;
    config.enableCheckpointing = args.enableCheckpointing;
    config.enableReplay = args.enableReplay;
    config.enableDevelopment = args.enableDevelopment;
    
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
