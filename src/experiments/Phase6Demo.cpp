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
    
    // Parse command line arguments
    Phase6Config config = parsePhase6Arguments(argc, argv);
    
    // Initialize logging with appropriate level based on verbosity
    Logger::getGlobal().setLevel(LogLevel::Info);
    
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
    if (!experiment.testMemoryIntegration()) {
        std::cerr << "ERROR: Memory integration test failed!" << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    if (!experiment.testNeuromodulationIntegration()) {
        std::cerr << "ERROR: Neuromodulation integration test failed!" << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    if (!experiment.testCheckpointing()) {
        std::cerr << "ERROR: Checkpoint test failed!" << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    if (!experiment.testReplay()) {
        std::cerr << "ERROR: Replay test failed!" << std::endl;
        return 1;
    }
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " 
              << config.maxSteps << " steps" << std::endl;
    std::cout << "(Development: " << (config.enableDevelopment ? "enabled" : "disabled") << ")" << std::endl;
    std::cout << "(Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << ")" << std::endl;
    std::cout << "(Replay: " << (config.enableReplay ? "enabled" : "disabled") << ")" << std::endl;
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
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Print experiment summary
    printPhase6Summary(result);
    
    return 0;
}

// Helper function to parse Phase 6 command line arguments
Phase6Config parsePhase6Arguments(int argc, char* argv[]) {
    Phase6Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Parse configuration parameters
                if (key == "neuron_count" || key == "neuronCount") {
                    config.neuronCount = static_cast<size_t>(std::stoi(value));
                } else if (key == "max_steps" || key == "maxSteps" || key == "max_simulation_steps") {
                    config.maxSteps = std::stoi(value);
                } else if (key == "region_count" || key == "regionCount") {
                    config.regionCount = static_cast<size_t>(std::stoi(value));
                } else if (key == "connection_probability" || key == "connectionProbability") {
                    config.connectionProbability = std::stof(value);
                } else if (key == "checkpoint_path" || key == "checkpointPath") {
                    config.checkpointPath = value;
                } else if (key == "enable_checkpointing" || key == "enableCheckpointing") {
                    config.enableCheckpointing = (value == "true" || value == "1" || value == "yes");
                } else if (key == "enable_replay" || key == "enableReplay") {
                    config.enableReplay = (value == "true" || value == "1" || value == "yes");
                } else if (key == "enable_development" || key == "enableDevelopment") {
                    config.enableDevelopment = (value == "true" || value == "1" || value == "yes");
                }
            }
        }
    }
    
    return config;
}

// Print experiment summary
void printPhase6Summary(const Phase6IntegrationResult& result) {
    std::cout << std::endl;
    std::cout << "=== EXPERIMENT SUMMARY ===" << std::endl;
    std::cout << "Simulation completed successfully in " << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "Total reward per step: " << result.totalReward << std::endl;
    std::cout << "Episodic memory storage: " << result.memoryEpisodesStored << " episodes" << std::endl;
    std::cout << "Neuromodulation levels: Dopamine=" << result.dopamineLevel << ", Novelty=" << result.noveltyLevel << ", Curiosity=" << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    // Integration status summary
    std::cout << "INTEGRATION STATUS:" << std::endl;
    std::cout << "  Memory Systems: " << (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated ? "✓ OPERATIONAL" : "✗ ISSUES") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ OPERATIONAL" : "✗ ISSUES") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "✓ OPERATIONAL" : "✗ ISSUES") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "✓ OPERATIONAL" : "✗ ISSUES") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓ OPERATIONAL" : "✗ ISSUES") << std::endl;
    std::cout << "  Replay System: " << (result.replayWorks ? "✓ OPERATIONAL" : "✗ ISSUES") << std::endl;
}
