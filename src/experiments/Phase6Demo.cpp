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

}

// Parse command line arguments for standardized CLI
void parseCommandLineArgs(int argc, char* argv[], int& neurons, int& steps,
                         int& repeats, std::string& checkpointPath,
                         bool& verbose, bool& quiet) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
            std::cout << std::endl;
            std::cout << "NLM Phase 6 - Integration Demo" << std::endl;
            std::cout << "=" << std::string(50, '=') << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h                Show this help message" << std::endl;
            std::cout << "  --version                 Show version information" << std::endl;
            std::cout << "  --neurons N               Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps S                 Number of simulation steps (default: 2000)" << std::endl;
            std::cout << "  --repeats R               Number of episode repeats (default: 3)" << std::endl;
            std::cout << "  --checkpoint PATH         Enable checkpointing with path (default: ./nlm_checkpoint.bin)" << std::endl;
            std::cout << "  --verbose                 Enable verbose logging" << std::endl;
            std::cout << "  --quiet                   Suppress informational messages" << std::endl;
            std::cout << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  ./nlm_phase6_demo --neurons 1000 --steps 5000" << std::endl;
            std::cout << "  ./nlm_phase6_demo --repeats 10 --verbose" << std::endl;
            exit(0);
        } else if (arg == "--version") {
            std::cout << "NLM Phase 6 Demo (Version 0.1.0)" << std::endl;
            exit(0);
        } else if (arg.substr(0, 10) == "--neurons=") {
            neurons = std::stoi(arg.substr(10));
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                neurons = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 8) == "--steps=") {
            steps = std::stoi(arg.substr(8));
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 9) == "--repeats=") {
            repeats = std::stoi(arg.substr(9));
        } else if (arg == "--repeats") {
            if (i + 1 < argc) {
                repeats = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 13) == "--checkpoint=") {
            checkpointPath = arg.substr(13);
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                checkpointPath = argv[++i];
            }
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quiet") {
            quiet = true;
        }
    }
}

// Main entry point for Phase 6 demo
int main(int argc, char* argv[]) {
    // Default parameters
    int neurons = 500;
    int steps = 2000;
    int repeats = 3;
    std::string checkpointPath = "./nlm_checkpoint.bin";
    bool verbose = false;
    bool quiet = false;
    
    // Parse command line arguments with standardized interface
    parseCommandLineArgs(argc, argv, neurons, steps, repeats, checkpointPath, verbose, quiet);
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level based on verbosity
    if (quiet) {
        consoleLogger->setLevel(LogLevel::Error);
    } else if (verbose) {
        consoleLogger->setLevel(LogLevel::Debug);
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
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
    
    // Run full experiment with specified settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = neurons;
    config.maxSteps = steps;
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
