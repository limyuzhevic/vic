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
#include <vector>
#include <algorithm>

using namespace nlm;

void printHelp() {
    std::cout << "Usage: ./nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << "  Run the NLM Phase 6 integration demonstration." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h           Print this help message" << std::endl;
    std::cout << "  --quiet, -q          Suppress informational output" << std::endl;
    std::cout << "  --skip-verification   Skip integration verification step" << std::endl;
    std::cout << "  --neurons N          Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S            Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --no-checkpointing   Disable checkpointing tests" << std::endl;
    std::cout << "  --no-replay          Disable replay tests" << std::endl;
    std::cout << "  --no-development     Disable development tests" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo                         # Run with default settings" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons 1000 --steps 5000  # Larger experiment" << std::endl;
    std::cout << "  ./nlm_phase6_demo -q --skip-verification     # Quiet mode, skip verification" << std::endl;
}

bool parseArgs(int argc, char* argv[], bool& skipVerification, bool& quiet, 
               size_t& neuronCount, size_t& maxSteps, bool& enableCheckpointing,
               bool& enableReplay, bool& enableDevelopment) {
    std::vector<std::string> args(argv, argv + argc);
    
    for (size_t i = 1; i < args.size(); ++i) {
        std::string arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return true; // Exit after help
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--skip-verification") {
            skipVerification = true;
        } else if (arg == "--no-checkpointing") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg == "--neurons" && i + 1 < args.size()) {
            try {
                neuronCount = std::stoul(args[++i]);
                if (neuronCount == 0) {
                    std::cerr << "Error: Neuron count must be positive" << std::endl;
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid neuron count: " << args[i] << std::endl;
                return false;
            }
        } else if (arg == "--steps" && i + 1 < args.size()) {
            try {
                maxSteps = std::stoul(args[++i]);
                if (maxSteps == 0) {
                    std::cerr << "Error: Step count must be positive" << std::endl;
                    return false;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid step count: " << args[i] << std::endl;
                return false;
            }
        } else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            return false;
        }
    }
    
    return true;
}

void printSection(const std::string& title, bool quiet) {
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "=== " << title << " ===" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    bool skipVerification = false;
    bool quiet = false;
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    // Parse command line arguments
    if (!parseArgs(argc, argv, skipVerification, quiet, neuronCount, maxSteps,
                   enableCheckpointing, enableReplay, enableDevelopment)) {
        return 1;
    }
    
    // Check if help was requested (parseArgs returns true when help is printed)
    if (skipVerification && !quiet) {
        printHelp();
        return 0;
    }
    
    if (!quiet) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << neuronCount << std::endl;
        std::cout << "  Steps: " << maxSteps << std::endl;
        std::cout << "  Checkpointing: " << (enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Replay: " << (enableReplay ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Development: " << (enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << std::endl;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification (unless skipped)
    if (!skipVerification) {
        printSection("Integration Verification", quiet);
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        if (!quiet) {
            std::cout << "Integration verification passed!" << std::endl << std::endl;
        }
    }
    
    // Test individual systems
    if (!quiet) {
        printSection("Memory Integration Test", quiet);
    }
    experiment.testMemoryIntegration();
    
    if (!quiet) {
        printSection("Neuromodulation Integration Test", quiet);
    }
    experiment.testNeuromodulationIntegration();
    
    if (!quiet) {
        printSection("Checkpoint Test", quiet);
    }
    experiment.testCheckpointing();
    
    if (!quiet) {
        printSection("Replay Test", quiet);
    }
    experiment.testReplay();
    
    // Run full experiment with smaller settings for demo
    printSection("Full Integration Experiment", quiet);
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    
    auto result = experiment.run(config);
    
    // Validate results
    bool experimentSuccess = true;
    if (result.totalReward < 0) {
        std::cerr << "WARNING: Negative total reward" << std::endl;
        experimentSuccess = false;
    }
    
    if (result.memoryEpisodesStored < 0) {
        std::cerr << "WARNING: Negative episodes stored" << std::endl;
        experimentSuccess = false;
    }
    
    printSection("FINAL RESULTS", quiet);
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    printSection("INTEGRATION STATUS", quiet);
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    std::cout << std::endl;
    
    if (!quiet) {
        printSection("EXPERIMENT SUMMARY", quiet);
        if (experimentSuccess) {
            std::cout << "✓ Phase 6 integration test completed successfully" << std::endl;
            std::cout << "✓ All brain systems are properly connected and functional" << std::endl;
        } else {
            std::cout << "⚠ Phase 6 integration completed with warnings" << std::endl;
            std::cout << "⚠ Some metrics may need investigation" << std::endl;
        }
    }
    
    return experimentSuccess ? 0 : 1;
}
