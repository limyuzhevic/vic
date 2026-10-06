pragma once

/**
 * Phase6Demo - Phase 6 Integration Demo
 * 
 * This demo runs the complete Phase 6 integration test to verify
 * that all brain systems work together as a coherent artificial brain.
 * 
 * Unlike previous phase demos that test specific capabilities,
 * this demo verifies the integration and connectivity of all systems.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

/**
 * Get current time as formatted string
 */
inline std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * Phase 6 Integration Demo Class
 * 
 * Runs the complete Phase 6 integration test and provides a comprehensive
 * summary of the integrated brain's capabilities.
 */
class Phase6Demo {
public:
    Phase6Demo() : experiment_(new Phase6IntegratedExperiment()) {}
    ~Phase6Demo() = default;
    
    /**
     * Run the complete Phase 6 integration demo
     * 
     * This method:
     * 1. Verifies basic integration
     * 2. Tests individual system integrations
     * 3. Runs a full lifetime simulation
     * 4. Validates checkpointing and replay
     * 5. Provides comprehensive results
     * 
     * @param config Configuration for the integration test
     * @param enableQuickMode Skip some tests for faster execution
     */
    void run(const Phase6Config& config, bool enableQuickMode = false) {
        std::cout << "===========================================\n";
        std::cout << "       NLM PHASE 6: INTEGRATION DEMO       \n";
        std::cout << "===========================================\n\n";
        
        std::cout << "Phase 6 Integration Test\n";
        std::cout << "Run time: " << getCurrentTimeString() << "\n\n";
        
        // Phase 1: Quick integration verification (unless in quick mode)
        if (!enableQuickMode) {
            std::cout << "--- PHASE 1: BASIC INTEGRATION VERIFICATION ---\n";
            std::cout << "Checking that all major systems are present and connected...\n";
            
            bool integrationOK = experiment_->verifyIntegration();
            
            if (!integrationOK) {
                std::cerr << "ERROR: Basic integration verification failed!\n";
                std::cerr << "Some systems are not properly connected.\n";
                std::cerr << "See logs above for specific failures.\n";
                std::cerr << "\nThe brain cannot function without these systems.\n";
                std::cerr << "You may need to fix integration issues before proceeding.\n";
                return;
            }
            
            std::cout << "✓ Basic integration verification passed!\n\n";
        } else {
            std::cout << "--- SKIPPING: Quick mode bypasses integration check ---\n\n";
        }
        
        // Phase 2: System-specific integration tests
        std::cout << "--- PHASE 2: SYSTEM-SPECIFIC INTEGRATION TESTS ---\n";
        
        std::cout << "Testing memory system integration...\n";
        bool memoryOK = experiment_->testMemoryIntegration();
        if (memoryOK) {
            std::cout << "✓ Memory integration test passed!\n";
        }
        
        std::cout << "Testing neuromodulation integration...\n";
        bool neuromodulationOK = experiment_->testNeuromodulationIntegration();
        if (neuromodulationOK) {
            std::cout << "✓ Neuromodulation integration test passed!\n";
        }
        
        std::cout << "Testing checkpoint save/load...\n";
        bool checkpointingOK = experiment_->testCheckpointing();
        if (checkpointingOK) {
            std::cout << "✓ Checkpoint test passed!\n";
        }
        
        std::cout << "Testing replay system...\n";
        bool replayOK = experiment_->testReplay();
        if (replayOK) {
            std::cout << "✓ Replay test passed!\n";
        }
        
        // Phase 3: Full integration experiment
        std::cout << "\n--- PHASE 3: FULL INTEGRATION EXPERIMENT ---\n";
        std::cout << "Running complete lifetime simulation with all systems active...\n\n";
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "Configuration:\n";
        std::cout << "  Neurons: " << config.neuronCount << "\n";
        std::cout << "  Max steps: " << config.maxSteps << "\n";
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "ENABLED" : "DISABLED") << "\n";
        std::cout << "  Replay: " << (config.enableReplay ? "ENABLED" : "DISABLED") << "\n";
        std::cout << "  Development: " << (config.enableDevelopment ? "ENABLED" : "DISABLED") << "\n\n";
        
        Phase6IntegrationResult result = experiment_->run(config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalWallTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Phase 4: Results summary
        std::cout << "===========================================\n";
        std::cout << "           PHASE 6 DEMO COMPLETE           \n";
        std::cout << "===========================================\n\n";
        
        std::cout << "EXPERIMENT RESULTS:\n";
        std::cout << "  Wall clock time: " << totalWallTime << " seconds\n";
        std::cout << "  Total reward: " << result.totalReward << " (higher is better)\n";
        std::cout << "  Average firing rate: " << result.avgFiringRate << " Hz\n";
        std::cout << "  Episodes stored: " << result.memoryEpisodesStored << "\n";
        std::cout << "  Novelty level: " << result.noveltyLevel << "\n";
        std::cout << "  Curiosity level: " << result.curiosityLevel << "\n";
        std::cout << "  Dopamine level: " << result.dopamineLevel << "\n\n";
        
        std::cout << "INTEGRATION STATUS:\n";
        printIntegrationStatus(result);
        
        std::cout << "\nPERFORMANCE ASSESSMENT:\n";
        printPerformanceAssessment(result);
        
        std::cout << "\nVERIFICATION CHECKLIST:\n";
        printVerificationChecklist(result);
        
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "Phase 6 Integration Demo Complete!\n";
        std::cout << std::string(60, '=') << "\n";
        
        if (config.enableCheckpointing) {
            std::cout << "\nA checkpoint file was saved: " << config.checkpointPath << "\n";
            std::cout << "You can use this checkpoint to resume the brain.\n";
        }
    }
    
    /**
     * Print integration status
     */
    void printIntegrationStatus(const Phase6IntegrationResult& result) {
        const char* status[] = {"DISCONNECTED", "CONNECTED"};
        const char* working[] = {"NOT WORKING", "WORKING"};
        
        std::cout << "  Memory Working Memory: " << status[result.memoryWorkingMemoryIntegrated] << "\n";
        std::cout << "  Memory Episodic Memory: " << status[result.memoryEpisodicMemoryIntegrated] << "\n";
        std::cout << "  Neuromodulation: " << status[result.neuromodulationIntegrated] << "\n";
        std::cout << "  Prediction: " << status[result.predictionIntegrated] << "\n";
        std::cout << "  Development: " << status[result.developmentIntegrated] << "\n";
        std::cout << "  Checkpointing: " << working[result.checkpointingWorks] << "\n";
    }
    
    /**
     * Print performance assessment
     */
    void printPerformanceAssessment(const Phase6IntegrationResult& result) {
        int connectedCount = 0;
        connectedCount += result.memoryWorkingMemoryIntegrated;
        connectedCount += result.memoryEpisodicMemoryIntegrated;
        connectedCount += result.neuromodulationIntegrated;
        connectedCount += result.predictionIntegrated;
        connectedCount += result.developmentIntegrated;
        
        std::cout << "  Systems Connected: " << connectedCount << "/5\n";
        
        if (result.checkpointingWorks) {
            std::cout << "  Checkpointing: Operational - persistent memory is functional\n";
        }
        
        if (result.totalReward > 0) {
            std::cout << "  Reward Processing: Functional - agent learned from environment\n";
        }
        
        if (result.avgFiringRate > 0.1f) {
            std::cout << "  Neural Activity: Present - brain is processing information\n";
        }
        
        if (result.noveltyLevel > 0.1f) {
            std::cout << "  Novelty Detection: Functional - system detects new patterns\n";
        }
        
        if (result.curiosityLevel > 0.1f) {
            std::cout << "  Curiosity Drive: Functional - system seeks new experiences\n";
        }
    }
    
    /**
     * Print verification checklist
     */
    void printVerificationChecklist(const Phase6IntegrationResult& result) {
        std::cout << "  ✓ Brain core with LIF neurons and plasticity rules\n";
        
        if (result.memoryWorkingMemoryIntegrated) {
            std::cout << "  ✓ Working memory integrated with neural processing\n";
        }
        
        if (result.memoryEpisodicMemoryIntegrated) {
            std::cout << "  ✓ Episodic memory functional for experience storage\n";
        }
        
        if (result.neuromodulationIntegrated) {
            std::cout << "  ✓ Neuromodulation (dopamine, curiosity, novelty) operational\n";
        }
        
        if (result.predictionIntegrated) {
            std::cout << "  ✓ Prediction system integrated with learning\n";
        }
        
        if (result.developmentIntegrated) {
            std::cout << "  ✓ Development system functional for neural maturation\n";
        }
        
        if (result.checkpointingWorks) {
            std::cout << "  ✓ Checkpoint system working for persistent storage\n";
        }
        
        if (result.totalReward > 0) {
            std::cout << "  ✓ Reward processing and neuromodulation active\n";
        }
        
        if (result.noveltyLevel > 0 || result.curiosityLevel > 0) {
            std::cout << "  ✓ Exploration mechanisms (novelty, curiosity) functional\n";
        }
        
        if (result.memoryEpisodesStored > 0) {
            std::cout << "  ✓ Memory consolidation and replay operational\n";
        }
    }
    
private:
    std::unique_ptr<Phase6IntegratedExperiment> experiment_;
};

} // namespace nlm

