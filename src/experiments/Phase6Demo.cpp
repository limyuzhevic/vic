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
#include <map>

using namespace nlm;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons N      Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S        Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --checkpoint      Enable checkpointing (default: enabled)" << std::endl;
    std::cout << "  --replay         Enable replay system (default: enabled)" << std::endl;
    std::cout << "  --development    Enable development system (default: enabled)" << std::endl;
    std::cout << "  --help           Show this help message" << std::endl;
}

Phase6Config parseArgs(int argc, char* argv[]) {
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        }
    }
    
    return config;
}

void printResults(const Phase6IntegrationResult& result) {
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << std::fixed << std::setprecision(2) << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    std::cout << "Novelty level: " << std::fixed << std::setprecision(4) << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << std::fixed << std::setprecision(4) << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory:      " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Episodic Memory:     " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Neuromodulation:     " << (result.neuromodulationIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Prediction:           " << (result.predictionIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Development:         " << (result.developmentIntegrated ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "Checkpointing:        " << (result.checkpointingWorks ? "WORKING ✓" : "NOT WORKING ✗") << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== PERFORMANCE METRICS ===" << std::endl;
    std::cout << "Wall clock time:     " << std::fixed << std::setprecision(2) << result.totalWallClockTime << "s" << std::endl;
    
    // Calculate success rate based on integration
    int successCount = 0;
    int totalCount = 0;
    if (result.memoryWorkingMemoryIntegrated) successCount++;
    totalCount++;
    if (result.memoryEpisodicMemoryIntegrated) successCount++;
    totalCount++;
    if (result.neuromodulationIntegrated) successCount++;
    totalCount++;
    if (result.predictionIntegrated) successCount++;
    totalCount++;
    if (result.developmentIntegrated) successCount++;
    totalCount++;
    if (result.checkpointingWorks) successCount++;
    totalCount++;
    
    float integrationScore = static_cast<float>(successCount) / static_cast<float>(totalCount);
    std::cout << "Integration Score:    " << std::fixed << std::setprecision(2) << (integrationScore * 100.0f) << "% (" 
              << successCount << "/" << totalCount << " systems connected)" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        // Parse command line arguments
        Phase6Config config = parseArgs(argc, argv);
        
        std::cout << "=== Configuration ===" << std::endl;
        std::cout << "Neurons:            " << config.neuronCount << std::endl;
        std::cout << "Simulation steps:  " << config.maxSteps << std::endl;
        std::cout << "Checkpointing:      " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
        std::cout << "Replay system:      " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << "Development:        " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
        std::cout << std::endl;
        
        // Initialize logging
        Logger::getGlobal().setLevel(LogLevel::Info);
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        // First, run the quick integration verification
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            std::cerr << "Some brain systems are not properly connected." << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems
        std::cout << "--- Memory Integration Test ---" << std::endl;
        bool memoryTest = experiment.testMemoryIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        bool neuromodTest = experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        bool checkpointTest = experiment.testCheckpointing();
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        bool replayTest = experiment.testReplay();
        std::cout << std::endl;
        
        // Run full experiment with configured settings
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        std::cout << "Running with " << config.neuronCount << " neurons for " << config.maxSteps << " steps..." << std::endl;
        
        auto result = experiment.run(config);
        
        printResults(result);
        
        // Final verdict
        std::cout << std::endl;
        std::cout << "=== FINAL VERDICT ===" << std::endl;
        if (result.totalReward > 0.0f) {
            std::cout << "✓ Learning detected: Brain is acquiring knowledge through experience" << std::endl;
        } else {
            std::cout << "⚠ No measurable learning: Consider adjusting network parameters or increasing simulation time" << std::endl;
        }
        
        if (result.memoryEpisodesStored > 0) {
            std::cout << "✓ Memory systems are functional: " << result.memoryEpisodesStored << " episodes stored" << std::endl;
        } else {
            std::cout << "⚠ Memory systems may have issues: No episodes stored" << std::endl;
        }
        
        if (result.checkpointingWorks) {
            std::cout << "✓ Checkpointing is working: Brain state can be saved and loaded" << std::endl;
        } else {
            std::cout << "⚠ Checkpointing has issues: Save/load functionality may not work properly" << std::endl;
        }
        
        std::cout << "✓ Phase 6 integration complete: All major brain systems are connected" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        std::cerr << "Phase 6 integration failed due to an exception." << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception occurred" << std::endl;
        std::cerr << "Phase 6 integration failed." << std::endl;
        return 1;
    }
}
