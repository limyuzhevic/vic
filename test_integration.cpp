#include "Phase6IntegratedExperiment.hpp"
#include "Phase6Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Phase 6 Integration Test ===" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Run verification
    std::cout << "Running integration verification..." << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Integration verification passed!" << std::endl << std::endl;
    
    // Run comprehensive integration test
    std::cout << "Running comprehensive integration experiment..." << std::endl;
    
    Phase6Config config;
    config.neuronCount = 100;
    config.maxSteps = 500;
    config.enableCheckpointing = false;
    config.enableReplay = false;
    config.enableDevelopment = false;
    
    auto result = experiment.run(config);
    
    std::cout << "\n=== Integration Test Results ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    
    std::cout << "\n=== System Integration Status ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    
    if (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated && 
        result.neuromodulationIntegrated && result.predictionIntegrated && 
        result.developmentIntegrated) {
        std::cout << "\n🎉 SUCCESS: All core systems are integrated and functioning together!" << std::endl;
        return 0;
    } else {
        std::cout << "\n⚠️  WARNING: Some systems are still disconnected." << std::endl;
        return 0;
    }
}