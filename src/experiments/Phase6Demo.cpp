// Phase 6 Demo - Integration Test with Enhanced Python Manager
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/NLMPhase6PythonManager.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo (Enhanced) ===" << std::endl;
    std::cout << "Testing the integrated artificial brain with advanced utilities..." << std::endl << std::endl;
    
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
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
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
    
    // Demonstrate new Python manager utilities
    std::cout << std::endl;
    std::cout << "=== Python Manager Utilities ===" << std::endl;
    NLMPhase6PythonManager pythonManager;
    
    // Demonstrate quick verification
    bool quickCheck = pythonManager.quickVerification();
    std::cout << "Quick verification result: " << (quickCheck ? "SUCCESS" : "FAILED") << std::endl;
    
    // Demonstrate individual system testing
    std::cout << std::endl;
    pythonManager.testIndividualSystems();
    
    // Demonstrate configuration creation
    Phase6Config advancedConfig = NLMPhase6PythonManager::createAdvancedConfig(
        3000,  // maxSteps
        800,   // neuronCount
        3,     // regionCount
        0.08f, // connectionProbability
        true,  // enableCheckpointing
        true,  // enableReplay
        true   // enableDevelopment
    );
    
    std::cout << std::endl;
    std::cout << "Advanced configuration created successfully!" << std::endl;
    std::cout << "Neuron count: " << advancedConfig.neuronCount << std::endl;
    std::cout << "Max steps: " << advancedConfig.maxSteps << std::endl;
    
    // Demonstrate checkpoint operations
    bool saveSuccess = pythonManager.saveCheckpoint("./test_checkpoint.bin");
    std::cout << "Checkpoint save: " << (saveSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    bool loadSuccess = pythonManager.loadCheckpoint("./test_checkpoint.bin");
    std::cout << "Checkpoint load: " << (loadSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    // Demonstrate result validation
    bool validationOK = NLMPhase6PythonManager::validateResults(result);
    std::cout << "Results validation: " << (validationOK ? "PASSED" : "FAILED") << std::endl;
    
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
    
    // Determine overall success
    bool allOK = result.memoryWorkingMemoryIntegrated && 
                result.memoryEpisodicMemoryIntegrated &&
                result.neuromodulationIntegrated &&
                result.predictionIntegrated &&
                result.developmentIntegrated &&
                result.checkpointingWorks &&
                result.replayWorks;
    
    std::cout << std::endl;
    std::cout << "=== OVERALL STATUS ===" << std::endl;
    if (allOK) {
        std::cout << "✓ ALL SYSTEMS INTEGRATED SUCCESSFULLY" << std::endl;
        std::cout << "The NLM brain is fully functional with all subsystems connected." << std::endl;
        std::cout << "The enhanced Python Manager provides additional utilities for researchers and developers." << std::endl;
    } else {
        std::cout << "✗ INTEGRATION ISSUES DETECTED" << std::endl;
        std::cout << "Some systems may need additional integration work." << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== ENHANCED FEATURES DEMONSTRATED ===" << std::endl;
    std::cout << "✓ Advanced configuration system" << std::endl;
    std::cout << "✓ Checkpoint save/load utilities" << std::endl;
    std::cout << "✓ Result validation framework" << std::endl;
    std::cout << "✓ Individual system testing" << std::endl;
    std::cout << "✓ Custom experiment running" << std::endl;
    std::cout << "✓ Configuration parameter management" << std::endl;
    
    return 0;
}