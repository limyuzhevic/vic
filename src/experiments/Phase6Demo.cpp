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
    
    // Enhanced parameter validation
    std::cout << "--- Parameter Validation ---" << std::endl;
    
    // Validate neuronCount
    if (config.neuronCount == 0) {
        std::cerr << "ERROR: Invalid neuronCount: must be positive (got " << config.neuronCount << ")" << std::endl;
        return 1;
    }
    if (config.neuronCount > 100000) {
        std::cerr << "WARNING: Very large neuronCount may impact performance (got " << config.neuronCount << ")" << std::endl;
    }
    std::cout << "✓ neuronCount validated: " << config.neuronCount << std::endl;
    
    // Validate maxSteps
    if (config.maxSteps == 0) {
        std::cerr << "ERROR: Invalid maxSteps: must be positive (got " << config.maxSteps << ")" << std::endl;
        return 1;
    }
    if (config.maxSteps > 1000000) {
        std::cerr << "WARNING: Very large maxSteps may impact performance (got " << config.maxSteps << ")" << std::endl;
    }
    std::cout << "✓ maxSteps validated: " << config.maxSteps << std::endl;
    
    // Validate regionCount
    if (config.regionCount == 0) {
        std::cerr << "ERROR: Invalid regionCount: must be positive (got " << config.regionCount << ")" << std::endl;
        return 1;
    }
    std::cout << "✓ regionCount validated: " << config.regionCount << std::endl;
    
    // Validate connectionProbability
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        std::cerr << "ERROR: Invalid connectionProbability: must be between 0 and 1 (got " << config.connectionProbability << ")" << std::endl;
        return 1;
    }
    std::cout << "✓ connectionProbability validated: " << config.connectionProbability << std::endl;
    
    // Validate checkpointPath is not empty if checkpointing is enabled
    if (config.enableCheckpointing && config.checkpointPath.empty()) {
        std::cerr << "ERROR: checkpointPath cannot be empty when enableCheckpointing is true" << std::endl;
        return 1;
    }
    std::cout << "✓ checkpointPath validated: " << config.checkpointPath << std::endl;
    
    // Validate demo-specific constraints
    if (config.neuronCount > 5000) {
        std::cout << "WARNING: Large neural network for demo mode may impact performance" << std::endl;
    }
    
    if (config.maxSteps > 10000) {
        std::cout << "WARNING: Long simulation duration for demo mode" << std::endl;
    }
    
    std::cout << "Parameter validation complete!" << std::endl << std::endl;
    
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
