#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Test ===" << std::endl;
    std::cout << "Testing all neural systems integration..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Test basic integration
    std::cout << "--- Basic Integration Test ---" << std::endl;
    bool basicOK = experiment.verifyIntegration();
    
    if (!basicOK) {
        std::cerr << "ERROR: Basic integration test failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Basic integration test passed!" << std::endl << std::endl;
    
    // Test memory integration
    std::cout << "--- Memory Integration Test ---" << std::endl;
    bool memoryOK = experiment.testMemoryIntegration();
    
    if (!memoryOK) {
        std::cerr << "ERROR: Memory integration test failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Memory integration test passed!" << std::endl << std::endl;
    
    // Test neuromodulation integration (including ACh, NE, 5-HT)
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodOK = experiment.testNeuromodulationIntegration();
    
    if (!neuromodOK) {
        std::cerr << "ERROR: Neuromodulation integration test failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Neuromodulation integration test passed!" << std::endl << std::endl;
    
    // Test checkpointing
    std::cout << "--- Checkpoint Integration Test ---" << std::endl;
    bool checkpointOK = experiment.testCheckpointing();
    
    if (!checkpointOK) {
        std::cerr << "ERROR: Checkpoint test failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Checkpoint test passed!" << std::endl << std::endl;
    
    // Test replay
    std::cout << "--- Replay Integration Test ---" << std::endl;
    bool replayOK = experiment.testReplay();
    
    if (!replayOK) {
        std::cerr << "ERROR: Replay test failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Replay test passed!" << std::endl << std::endl;
    
    std::cout << "=== ALL INTEGRATION TESTS PASSED ===" << std::endl;
    std::cout << "All neural systems are properly integrated and functional!" << std::endl;
    
    return 0;
}
