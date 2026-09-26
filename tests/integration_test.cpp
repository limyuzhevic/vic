#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Integration Verification Test ===" << std::endl;
    std::cout << "Testing that all brain systems are properly integrated..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Test integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cout << "\n=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (experiment.getIntegrationResult().memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (experiment.getIntegrationResult().memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (experiment.getIntegrationResult().neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (experiment.getIntegrationResult().predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (experiment.getIntegrationResult().developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (experiment.getIntegrationResult().checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    bool memoryOK = experiment.testMemoryIntegration();
    std::cout << "Memory integration: " << (memoryOK ? "PASSED" : "FAILED") << std::endl << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodOK = experiment.testNeuromodulationIntegration();
    std::cout << "Neuromodulation integration: " << (neuromodOK ? "PASSED" : "FAILED") << std::endl << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    bool checkpointOK = experiment.testCheckpointing();
    std::cout << "Checkpoint system: " << (checkpointOK ? "WORKING" : "FAILED") << std::endl << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    bool replayOK = experiment.testReplay();
    std::cout << "Replay system: " << (replayOK ? "WORKING" : "FAILED") << std::endl << std::endl;
    
    // Summary
    std::cout << "=== INTEGRATION TEST SUMMARY ===" << std::endl;
    std::cout << "Integration Verification: " << (integrationOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Memory Integration: " << (memoryOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Neuromodulation Integration: " << (neuromodOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Checkpoint System: " << (checkpointOK ? "PASS" : "FAIL") << std::endl;
    std::cout << "Replay System: " << (replayOK ? "PASS" : "FAIL") << std::endl;
    std::cout << std::endl;
    
    bool allTestsPassed = integrationOK && memoryOK && neuromodOK && checkpointOK && replayOK;
    
    if (allTestsPassed) {
        std::cout << "=== ALL INTEGRATION TESTS PASSED ===" << std::endl;
        std::cout << "The NLM brain systems are properly integrated and working together!" << std::endl;
    } else {
        std::cout << "=== SOME INTEGRATION TESTS FAILED ===" << std::endl;
        std::cout << "There are still integration issues that need to be addressed." << std::endl;
    }
    
    return allTestsPassed ? 0 : 1;
}
