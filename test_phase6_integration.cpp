// Phase 6 Integration Test Suite
// Comprehensive verification that all brain systems are properly integrated

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>

using namespace nlm;

// Test utility functions
namespace {
    bool runPhase6Tests() {
        std::cout << "=" << std::string(60, '=') << std::endl;
        std::cout << "   NLM Phase 6 Integration Test Suite" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
        Phase6IntegratedExperiment experiment;
        
        // Test 1: Integration verification
        std::cout << "\n--- Test 1: Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        if (integrationOK) {
            std::cout << "✓ Integration verification PASSED" << std::endl;
        } else {
            std::cout << "✗ Integration verification FAILED" << std::endl;
            return false;
        }
        
        // Test 2: Memory integration
        std::cout << "\n--- Test 2: Memory Integration ---" << std::endl;
        bool memoryOK = experiment.testMemoryIntegration();
        if (memoryOK) {
            std::cout << "✓ Memory integration PASSED" << std::endl;
        } else {
            std::cout << "✗ Memory integration FAILED" << std::endl;
        }
        
        // Test 3: Neuromodulation integration
        std::cout << "\n--- Test 3: Neuromodulation Integration ---" << std::endl;
        bool neuromodulationOK = experiment.testNeuromodulationIntegration();
        if (neuromodulationOK) {
            std::cout << "✓ Neuromodulation integration PASSED" << std::endl;
        } else {
            std::cout << "✗ Neuromodulation integration FAILED" << std::endl;
        }
        
        // Test 4: Checkpoint functionality
        std::cout << "\n--- Test 4: Checkpoint Functionality ---" << std::endl;
        bool checkpointOK = experiment.testCheckpointing();
        if (checkpointOK) {
            std::cout << "✓ Checkpoint functionality PASSED" << std::endl;
        } else {
            std::cout << "✗ Checkpoint functionality FAILED" << std::endl;
        }
        
        // Test 5: Replay system
        std::cout << "\n--- Test 5: Replay System ---" << std::endl;
        bool replayOK = experiment.testReplay();
        if (replayOK) {
            std::cout << "✓ Replay system PASSED" << std::endl;
        } else {
            std::cout << "✗ Replay system FAILED" << std::endl;
        }
        
        // Test 6: Full integration experiment
        std::cout << "\n--- Test 6: Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        Phase6IntegrationResult result = experiment.run(config);
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "✓ Full integration experiment completed" << std::endl;
        std::cout << "  Total reward: " << result.totalReward << std::endl;
        std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "  Wall time: " << elapsed << "s" << std::endl;
        
        // Summary
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "                TEST SUMMARY" << std::endl;
        std::cout << "=" << std::string(60, '=') << std::endl;
        
        bool allTestsPassed = integrationOK && memoryOK && neuromodulationOK && 
                           checkpointOK && replayOK && (result.totalReward >= 0);
        
        if (allTestsPassed) {
            std::cout << "✓ ALL TESTS PASSED" << std::endl;
            std::cout << "The NLM brain systems are properly integrated and functional!" << std::endl;
        } else {
            std::cout << "✗ SOME TESTS FAILED" << std::endl;
            std::cout << "Integration issues remain and should be addressed." << std::endl;
        }
        
        return allTestsPassed;
    }
}

// Main test entry point
int main(int argc, char* argv[]) {
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "NLM Phase 6 Integration Test Suite" << std::endl;
    std::cout << "Testing integration of memory, neuromodulation, prediction, and cognition systems" << std::endl;
    std::cout << std::endl;
    
    // Run tests
    bool success = runPhase6Tests();
    
    if (success) {
        return 0;
    } else {
        return 1;
    }
}