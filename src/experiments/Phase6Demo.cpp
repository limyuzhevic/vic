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

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments for Phase 6 demo
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "/tmp/nlm_phase6_checkpoint.bin";
    
    if (argc > 1) {
        try {
            neuronCount = std::stoul(argv[1]);
            if (neuronCount == 0 || neuronCount > 10000) {
                std::cerr << "Error: Neuron count must be between 1 and 10000" << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid neuron count: " << argv[1] << std::endl;
            return 1;
        }
    }
    
    if (argc > 2) {
        try {
            maxSteps = std::stoul(argv[2]);
            if (maxSteps == 0 || maxSteps > 100000) {
                std::cerr << "Error: Max steps must be between 1 and 100000" << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid max steps: " << argv[2] << std::endl;
            return 1;
        }
    }
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neuron count: " << neuronCount << std::endl;
    std::cout << "  Max steps: " << maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "enabled" : "disabled") << std::endl;
    std::cout << "  Replay: " << (enableReplay ? "enabled" : "disabled") << std::endl;
    std::cout << "  Development: " << (enableDevelopment ? "enabled" : "disabled") << std::endl;
    
    try {
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
        
        // Run full experiment with configuration
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = neuronCount;
        config.maxSteps = maxSteps;
        config.enableCheckpointing = enableCheckpointing;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        
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
        
    } catch (const std::exception& e) {
        std::cerr << "Error during Phase 6 demo execution: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error during Phase 6 demo execution" << std::endl;
        return 1;
    }
    
    return 0;
}
