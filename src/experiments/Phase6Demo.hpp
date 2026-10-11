#pragma once

/**
 * Phase6Demo - Simple interface for Phase 6 integration tests
 * 
 * This provides a simple API to run Phase 6 integration tests similar to
 * Phase6Demo.cpp but integrated into main.cpp for comprehensive testing
 * of the complete integrated brain systems.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace nlm {

/**
 * Simple phase 6 integration demo interface
 * 
 * This class provides a simple way to run Phase 6 integration tests
 * that verify all brain subsystems are properly connected and functional.
 */
class Phase6Demo {
public:
    Phase6Demo() = default;
    ~Phase6Demo() = default;
    
    /**
     * Run the complete Phase 6 integration demo
     * 
     * This method executes a comprehensive integration test that:
     * 1. Creates a complete integrated brain with all subsystems enabled
     * 2. Tests memory integration (working memory, episodic memory, associative memory)
     * 3. Tests neuromodulation integration (dopamine, curiosity, novelty, prediction error)
     * 4. Tests prediction system integration
     * 5. Tests development system integration
     * 6. Tests checkpointing and save/load functionality
     * 7. Tests replay and memory consolidation
     * 8. Demonstrates the complete brain loop with real experience
     * 
     * @param config Configuration for the integration test
     * @return Phase6IntegrationResult with detailed results for each subsystem
     */
    Phase6IntegrationResult runIntegrationTest(const Phase6Config& config) {
        Phase6IntegratedExperiment experiment;
        
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Creating complete integrated brain with all subsystems..." << std::endl;
        std::cout << "Neuron count: " << config.neuronCount << std::endl;
        std::cout << "Max steps: " << config.maxSteps << std::endl;
        std::cout << "Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << std::endl;
        
        // Initialize logging for detailed output
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Run the complete integration experiment
        Phase6IntegrationResult result = experiment.run(config);
        
        return result;
    }
    
    /**
     * Run a quick integration verification
     * 
     * This method performs a minimal integration check to ensure
     * all subsystems are properly wired together before running
     * the full experiment.
     * 
     * @return true if integration verification passes, false otherwise
     */
    bool verifyIntegration() {
        Phase6IntegratedExperiment experiment;
        
        std::cout << "=== Quick Integration Verification ===" << std::endl;
        bool success = experiment.verifyIntegration();
        
        if (success) {
            std::cout << "✓ Integration verification passed!" << std::endl;
        } else {
            std::cout << "✗ Integration verification failed!" << std::endl;
        }
        
        return success;
    }
    
    /**
     * Test individual subsystem integration
     * 
     * This method runs focused tests on each subsystem to verify
     * they are properly connected and functional.
     * 
     * @return Vector of test results for each subsystem
     */
    std::vector<std::pair<std::string, bool>> testSubsystemIntegration() {
        Phase6IntegratedExperiment experiment;
        std::vector<std::pair<std::string, bool>> results;
        
        std::cout << "=== Individual Subsystem Integration Tests ===" << std::endl;
        
        // Test memory integration
        bool memoryTest = experiment.testMemoryIntegration();
        results.emplace_back("Memory Integration", memoryTest);
        
        // Test neuromodulation integration
        bool neuromodTest = experiment.testNeuromodulationIntegration();
        results.emplace_back("Neuromodulation Integration", neuromodTest);
        
        // Test checkpointing
        bool checkpointTest = experiment.testCheckpointing();
        results.emplace_back("Checkpointing", checkpointTest);
        
        // Test replay
        bool replayTest = experiment.testReplay();
        results.emplace_back("Replay", replayTest);
        
        std::cout << std::endl;
        for (const auto& result : results) {
            std::cout << result.first << ": " << (result.second ? "PASS" : "FAIL") << std::endl;
        }
        
        return results;
    }
    
    /**
     * Display integration results summary
     * 
     * This method formats and displays the results from a Phase 6
     * integration test in a human-readable format.
     * 
     * @param result The integration test results to display
     */
    void displayResults(const Phase6IntegrationResult& result) {
        std::cout << std::endl;
        std::cout << "=== FINAL INTEGRATION RESULTS ===" << std::endl;
        std::cout << std::endl;
        
        // System integration status
        std::cout << "System Integration Status:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  Prediction System: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  Development System: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
        std::cout << "  Replay: " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        // Key metrics
        std::cout << "Key Performance Metrics:" << std::endl;
        std::cout << "  Total Reward: " << result.totalReward << std::endl;
        std::cout << "  Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "  Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
        std::cout << "  Novelty Level: " << result.noveltyLevel << std::endl;
        std::cout << "  Curiosity Level: " << result.curiosityLevel << std::endl;
        std::cout << "  Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        // Timing information
        std::cout << "Experiment Timing:" << std::endl;
        std::cout << "  Wall Clock Time: " << result.totalWallClockTime << " seconds" << std::endl;
        std::cout << std::endl;
        
        // Integration assessment
        int connectedSystems = 0;
        if (result.memoryWorkingMemoryIntegrated) connectedSystems++;
        if (result.memoryEpisodicMemoryIntegrated) connectedSystems++;
        if (result.neuromodulationIntegrated) connectedSystems++;
        if (result.predictionIntegrated) connectedSystems++;
        if (result.developmentIntegrated) connectedSystems++;
        
        std::cout << "Integration Assessment: " << connectedSystems << " out of 5 major systems connected" << std::endl;
        if (connectedSystems >= 4) {
            std::cout << "✓ SUCCESS: Brain shows high integration!" << std::endl;
        } else if (connectedSystems >= 3) {
            std::cout << "⚠ PARTIAL: Some systems disconnected" << std::endl;
        } else {
            std::cout << "✗ FAILURE: Poor integration" << std::endl;
        }
    }
};

} // namespace nlm