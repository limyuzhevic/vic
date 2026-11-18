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
#include <unordered_map>
#include <vector>

using namespace nlm;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << "  --quick             Run only integration verification (no full experiment)" << std::endl;
    std::cout << "  --neuron-count N    Set number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps S       Set maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --no-checkpoint     Disable checkpointing" << std::endl;
    std::cout << "  --no-replay         Disable replay mechanism" << std::endl;
    std::cout << "  --no-development    Disable development" << std::endl;
    std::cout << "  --verbose           Enable verbose logging" << std::endl;
    std::cout << "  --memory-only       Only test memory integration" << std::endl;
    std::cout << "  --cognitive-only    Only test cognitive system integration" << std::endl;
    std::cout << "  --neuromod-only     Only test neuromodulation integration" << std::endl;
    std::cout << "  --state FILE        Save final brain state to FILE" << std::endl;
    std::cout << "  --load-state FILE   Load brain state from FILE" << std::endl;
    std::cout << "  --config FILE       Load configuration from FILE (default: configs/default.cfg)" << std::endl;
}

Phase6Config parseArguments(int argc, char* argv[]) {
    Phase6Config config;
    std::string configFile = "configs/default.cfg";
    std::string saveStateFile;
    std::string loadStateFile;
    bool quickMode = false;
    bool verbose = false;
    bool memoryOnly = false;
    bool cognitiveOnly = false;
    bool neuromodOnly = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--quick") {
            quickMode = true;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--memory-only") {
            memoryOnly = true;
        } else if (arg == "--cognitive-only") {
            cognitiveOnly = true;
        } else if (arg == "--neuromod-only") {
            neuromodOnly = true;
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--state") {
            if (i + 1 < argc) {
                saveStateFile = argv[++i];
            }
        } else if (arg == "--load-state") {
            if (i + 1 < argc) {
                loadStateFile = argv[++i];
            }
        } else if (arg == "--neuron-count") {
            if (i + 1 < argc) {
                config.neuronCount = std::stoi(argv[++i]);
            }
        } else if (arg == "--max-steps") {
            if (i + 1 < argc) {
                config.maxSteps = std::stoi(argv[++i]);
            }
        } else if (arg.find("--") == 0) {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            exit(1);
        }
    }
    
    return config;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6Config config = parseArguments(argc, argv);
    
    // Initialize logging
    Logger::getGlobal().setLevel(verbose ? LogLevel::Debug : LogLevel::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // If loading from state, do that first
    if (!loadStateFile.empty()) {
        std::cout << "Loading brain state from: " << loadStateFile << std::endl;
        if (!experiment.load(loadStateFile)) {
            std::cerr << "ERROR: Failed to load brain state from " << loadStateFile << std::endl;
            return 1;
        }
    }
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems based on command line options
    if (memoryOnly || !cognitiveOnly && !neuromodOnly) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();
        std::cout << std::endl;
    }
    
    if (cognitiveOnly || !memoryOnly && !neuromodOnly) {
        std::cout << "--- Cognitive System Integration Test ---" << std::endl;
        experiment.testCognitiveIntegration();
        std::cout << std::endl;
    }
    
    if (neuromodOnly || !memoryOnly && !cognitiveOnly) {
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
    }
    
    if (!quickMode) {
        // Test other features
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
        
        // Run full experiment with configured settings
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        config.neuronCount = std::max(500, config.neuronCount); // Ensure minimum
        config.maxSteps = std::max(2000, config.maxSteps);     // Ensure minimum
        
        auto result = experiment.run(config);
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Concepts formed: " << result.conceptsFormed << std::endl;
        std::cout << "Actions learned: " << result.actionsLearned << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Semantic Memory: " << (result.memorySemanticMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Procedural Memory: " << (result.memoryProceduralMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        // Save final brain state if requested
        if (!saveStateFile.empty()) {
            std::cout << "Saving brain state to: " << saveStateFile << std::endl;
            if (!experiment.save(saveStateFile)) {
                std::cerr << "WARNING: Failed to save brain state to " << saveStateFile << std::endl;
            }
        }
    }
    
    std::cout << "=== DEMO COMPLETE ===" << std::endl;
    
    return 0;
}
