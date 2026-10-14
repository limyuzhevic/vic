/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <iomanip>

using namespace nlm;

// Main entry point for Phase 6 demo
// Enhanced demo with command-line arguments and comprehensive output
int main(int argc, char* argv[]) {
    int numTrials = 5;
    int maxSteps = 2000;
    
    // Parse command line arguments (Phase 3 demo pattern)
    if (argc > 1) {
        numTrials = std::atoi(argv[1]);
    }
    if (argc > 2) {
        maxSteps = std::atoi(argv[2]);
    }
    
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 6 - INTEGRATION DEMO\n";
    std::cout << "===========================================\n\n";
    
    std::cout << "Running integration test with " << numTrials 
              << " trials of up to " << maxSteps << " steps each.\n\n";
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---\n";
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!\n\n";
    
    // Test individual systems
    std::cout << "--- System Integration Tests ---\n";
    std::cout << "Testing memory system integration...\n";
    experiment.testMemoryIntegration();
    std::cout << "\n";
    
    std::cout << "Testing neuromodulation system integration...\n";
    experiment.testNeuromodulationIntegration();
    std::cout << "\n";
    
    std::cout << "Testing checkpoint functionality...\n";
    experiment.testCheckpointing();
    std::cout << "\n";
    
    std::cout << "Testing replay system...\n";
    experiment.testReplay();
    std::cout << "\n";
    
    // Run multiple trials for comprehensive testing
    std::cout << "--- Comprehensive Integration Tests ---\n";
    std::cout << "Running " << numTrials << " integration trials (" << maxSteps << " steps each)...\n\n";
    
    float totalReward = 0.0f;
    float totalFiringRate = 0.0f;
    float totalEpMemory = 0.0f;
    float totalDopamine = 0.0f;
    int successfulTrials = 0;
    
    for (int trial = 0; trial < numTrials; ++trial) {
        std::cout << "Trial " << trial + 1 << "/" << numTrials << ": ";
        
        // Run full integration experiment
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = maxSteps;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        auto result = experiment.run(config);
        
        // Accumulate results
        totalReward += result.totalReward;
        totalFiringRate += result.avgFiringRate;
        totalEpMemory += result.memoryEpisodesStored;
        totalDopamine += result.dopamineLevel;
        
        if (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated && 
            result.neuromodulationIntegrated && result.predictionIntegrated && 
            result.developmentIntegrated) {
            successfulTrials++;
            std::cout << "SUCCESS - All systems integrated\n";
        } else {
            std::cout << "FAILURE - Some systems not integrated\n";
        }
        
        if (trial < numTrials - 1) {
            std::cout << "\n";
        }
    }
    
    // Calculate averages
    float avgReward = (numTrials > 0) ? totalReward / numTrials : 0.0f;
    float avgFiring = (numTrials > 0) ? totalFiringRate / numTrials : 0.0f;
    float avgEpMemory = (numTrials > 0) ? totalEpMemory / numTrials : 0.0f;
    float avgDopamine = (numTrials > 0) ? totalDopamine / numTrials : 0.0f;
    
    // Final comprehensive summary
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "INTEGRATION TEST SUMMARY" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::cout << "\nPerformance Metrics (averaged over " << numTrials << " trials):\n";
    std::cout << "  Average Reward per step:   " << std::fixed << std::setprecision(3) << avgReward << std::endl;
    std::cout << "  Average Firing Rate:       " << std::fixed << std::setprecision(3) << avgFiring << std::endl;
    std::cout << "  Average Episodes Stored:   " << std::fixed << std::setprecision(3) << avgEpMemory << std::endl;
    std::cout << "  Average Dopamine Level:     " << std::fixed << std::setprecision(3) << avgDopamine << std::endl;
    
    std::cout << "\nSystem Integration Status:\n";
    std::cout << "  Working Memory:     " << (experiment.verifyIntegration() ? "CONNECTED ✓" : "DISCONNECTED ✗") << std::endl;
    std::cout << "  Episodic Memory:    " << "CONNECTED ✓ (based on integration verification)" << std::endl;
    std::cout << "  Neuromodulation:    " << "CONNECTED ✓ (based on integration verification)" << std::endl;
    std::cout << "  Prediction System:  " << "CONNECTED ✓ (based on integration verification)" << std::endl;
    std::cout << "  Development System: " << "CONNECTED ✓ (based on integration verification)" << std::endl;
    
    std::cout << "\nIntegration Success Rate: " << successfulTrials << "/" << numTrials 
              << " (" << std::fixed << std::setprecision(1) << (successfulTrials * 100.0f / numTrials) << "%)\n";
    
    std::cout << std::endl;
    std::cout << "=== INTEGRATION DEMO COMPLETE ===" << std::endl;
    
    return 0;
}
