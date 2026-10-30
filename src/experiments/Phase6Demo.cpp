/**
 * Phase 6 Demo - Integration Test with CLI Support
 * 
 * Enhanced version of Phase6Demo.cpp with proper error handling
 * and support for command line arguments.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== Enhanced NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Starting with CLI arguments..." << std::endl;
        
        // Initialize logging
        Logger::getGlobal().setLevel(Logger::Level::Info);
        
        // Parse command line arguments for custom configuration
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.regionCount = 1;
        config.connectionProbability = 0.1f;
        config.checkpointPath = "/tmp/nlm_checkpoint_test.bin";
        
        // Check for command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--neurons" && i + 1 < argc) {
                config.neuronCount = std::stoi(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                config.maxSteps = std::stoi(argv[++i]);
            } else if (arg == "--no-checkpoint") {
                config.enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                config.enableReplay = false;
            } else if (arg == "--no-development") {
                config.enableDevelopment = false;
            } else if (arg == "--help") {
                std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --neurons <n>      Number of neurons (default: 500)" << std::endl;
                std::cout << "  --steps <n>        Number of steps (default: 2000)" << std::endl;
                std::cout << "  --no-checkpoint     Disable checkpointing" << std::endl;
                std::cout << "  --no-replay         Disable replay" << std::endl;
                std::cout << "  --no-development    Disable development" << std::endl;
                std::cout << "  --help             Show this help" << std::endl;
                return 0;
            }
        }
        
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
        
        // Run full experiment with custom settings
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        std::cout << "Configuration: " << config.neuronCount << " neurons, " 
                  << config.maxSteps << " steps" << std::endl;
        
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
        std::cout << "=== DEMO COMPLETED SUCCESSFULLY ===" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "UNKNOWN ERROR occurred" << std::endl;
        return 1;
    }
}