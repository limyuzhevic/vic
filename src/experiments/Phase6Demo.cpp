/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * Enhanced with command-line interface for flexible testing.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <cstring>

using namespace nlm;

void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo - Help ===" << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --config FILE           Load configuration from file" << std::endl;
    std::cout << "  --neurons N             Set number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps N               Set simulation steps (default: 2000)" << std::endl;
    std::cout << "  --no-checkpoint         Disable checkpointing" << std::endl;
    std::cout << "  --no-replay             Disable replay system" << std::endl;
    std::cout << "  --no-development        Disable development system" << std::endl;
    std::cout << "  --checkpoint PATH       Set checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --verify-only           Only run integration verification, skip full experiment" << std::endl;
    std::cout << "  --fast                  Fast mode (smaller settings for quick testing)" << std::endl;
    std::cout << "  --quiet                 Quiet mode (minimal output)" << std::endl;
    std::cout << "  --log-level LEVEL       Set logging level: TRACE, DEBUG, INFO, WARN, ERROR, FATAL" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo                    # Run default demo" << std::endl;
    std::cout << "  ./nlm_phase6_demo --fast --quiet    # Quick test with minimal output" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  ./nlm_phase6_demo --no-checkpoint --verify-only" << std::endl;
}

void parseArguments(int argc, char* argv[], Phase6Config& config, bool& verifyOnly, bool& quiet) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--verify-only") {
            verifyOnly = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--fast") {
            config.neuronCount = 100;
            config.maxSteps = 500;
            config.enableCheckpointing = false;
            config.enableReplay = false;
            config.enableDevelopment = false;
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            std::string level = argv[++i];
            if (level == "TRACE") Logger::getInstance().setLevel(Logger::Level::Trace);
            else if (level == "DEBUG") Logger::getInstance().setLevel(Logger::Level::Debug);
            else if (level == "INFO") Logger::getInstance().setLevel(Logger::Level::Info);
            else if (level == "WARN") Logger::getInstance().setLevel(Logger::Level::Warn);
            else if (level == "ERROR") Logger::getInstance().setLevel(Logger::Level::Error);
            else if (level == "FATAL") Logger::getInstance().setLevel(Logger::.Level::Fatal);
            else std::cerr << "WARNING: Unknown log level: " << level << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command-line arguments
    Phase6Config config;
    bool verifyOnly = false;
    bool quiet = false;
    parseArguments(argc, argv, config, verifyOnly, quiet);
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!quiet) {
        std::cout << std::endl;
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    if (verifyOnly) {
        std::cout << "=== Verification Mode Complete ===" << std::endl;
        return integrationOK ? 0 : 1;
    }
    
    // Test individual systems
    if (!quiet) {
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
    } else {
        // Run tests quietly
        experiment.testMemoryIntegration();
        experiment.testNeuromodulationIntegration();
        experiment.testCheckpointing();
        experiment.testReplay();
        std::cout << std::endl;
    }
    
    // Run full experiment with configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
    if (config.enableCheckpointing) std::cout << "Checkpointing: Enabled" << std::endl;
    if (config.enableReplay) std::cout << "Replay: Enabled" << std::endl;
    if (config.enableDevelopment) std::cout << "Development: Enabled" << std::endl;
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
    std::cout << "Planner: " << (result.plannerIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Concept Formation: " << (result.conceptFormationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Attention: " << (result.attentionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Check overall success
    bool success = result.memoryWorkingMemoryIntegrated &&
                   result.memoryEpisodicMemoryIntegrated &&
                   result.neuromodulationIntegrated &&
                   result.predictionIntegrated &&
                   result.developmentIntegrated &&
                   result.plannerIntegrated &&
                   result.conceptFormationIntegrated &&
                   result.attentionIntegrated;
    
    if (success && result.checkpointingWorks) {
        std::cout << std::endl;
        std::cout << "=== ✓ SUCCESS ===" << std::endl;
        std::cout << "All brain systems are properly integrated and working together!" << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "=== ✗ FAILURE ===" << std::endl;
        std::cout << "Some systems are not properly integrated." << std::endl;
    }
    
    return success ? 0 : 1;
}
