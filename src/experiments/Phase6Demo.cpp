/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>

using namespace nlm;

/**
 * Print formatted result with units
 */
void printMetric(const std::string& name, float value, const std::string& unit = "") {
    std::cout << std::left << std::setw(35) << name << ": " << std::right;
    if (std::abs(value) >= 100.0f) {
        std::cout << std::fixed << std::setprecision(0) << value;
    } else if (std::abs(value) >= 1.0f) {
        std::cout << std::fixed << std::setprecision(3) << value;
    } else {
        std::cout << std::scientific << std::setprecision(4) << value;
    }
    if (!unit.empty()) std::cout << " " << unit;
    std::cout << std::endl;
}

/**
 * Print integration status
 */
void printIntegrationStatus(const Phase6IntegrationResult& result) {
    std::cout << "\n=== INTEGRATION STATUS ===" << std::endl;
    
    struct SystemStatus {
        const char* name;
        bool integrated;
    };
    
    SystemStatus systems[] = {
        {"Working Memory", result.memoryWorkingMemoryIntegrated},
        {"Episodic Memory", result.memoryEpisodicMemoryIntegrated},
        {"Neuromodulation", result.neuromodulationIntegrated},
        {"Prediction", result.predictionIntegrated},
        {"Development", result.developmentIntegrated},
        {"Checkpointing", result.checkpointingWorks},
        {"Replay", result.replayWorks}
    };
    
    for (const auto& sys : systems) {
        std::cout << std::left << std::setw(25) << sys.name << ": ";
        if (sys.integrated) {
            std::cout << "✓ CONNECTED" << std::endl;
        } else {
            std::cout << "✗ DISCONNECTED" << std::endl;
        }
    }
}

/**
 * Print detailed metrics
 */
void printDetailedMetrics(const Phase6IntegrationResult& result) {
    std::cout << "\n=== DETAILED METRICS ===" << std::endl;
    
    printMetric("Total Wall Clock Time", result.totalWallClockTime, "s");
    printMetric("Avg Firing Rate", result.avgFiringRate, "Hz");
    printMetric("Memory Episodes Stored", result.memoryEpisodesStored);
    printMetric("Novelty Level", result.noveltyLevel);
    printMetric("Curiosity Level", result.curiosityLevel);
    printMetric("Dopamine Level", result.dopamineLevel);
    printMetric("Total Reward", result.totalReward);
}

/**
 * Print experiment summary
 */
void printExperimentSummary(const Phase6IntegrationResult& result) {
    std::cout << "\n=== EXPERIMENT SUMMARY ===" << std::endl;
    
    int totalSystems = 7;
    int integratedSystems = 0;
    
    if (result.memoryWorkingMemoryIntegrated) integratedSystems++;
    if (result.memoryEpisodicMemoryIntegrated) integratedSystems++;
    if (result.neuromodulationIntegrated) integratedSystems++;
    if (result.predictionIntegrated) integratedSystems++;
    if (result.developmentIntegrated) integratedSystems++;
    if (result.checkpointingWorks) integratedSystems++;
    if (result.replayWorks) integratedSystems++;
    
    float integrationScore = (float)integratedSystems / totalSystems * 100.0f;
    
    std::cout << "Systems Integrated: " << integratedSystems << "/" << totalSystems << " (" << std::fixed << std::setprecision(1) << integrationScore << "%)" << std::endl;
    
    if (integrationScore >= 90.0f) {
        std::cout << "✓ EXCELLENT: All major systems are properly integrated!" << std::endl;
    } else if (integrationScore >= 70.0f) {
        std::cout << "✓ GOOD: Most systems are working correctly." << std::endl;
    } else if (integrationScore >= 50.0f) {
        std::cout << "⚠ FAIR: Some systems need attention." << std::endl;
    } else {
        std::cout << "✗ POOR: Multiple systems are not properly integrated." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    
    auto startWall = std::chrono::high_resolution_clock::now();
    
    // Initialize logging with detailed output
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "\n--- Integration Verification ---" << std::endl;
    std::cout << "Running comprehensive integration checks..." << std::endl;
    
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "The brain systems are not properly integrated." << std::endl;
        std::cerr << "Please check the logs for details on which systems failed." << std::endl;
        return 1;
    }
    
    std::cout << "✓ Integration verification passed!" << std::endl;
    std::cout << "All major brain systems are properly connected." << std::endl;
    std::cout << std::endl;
    
    // Test individual systems with better output
    std::cout << "--- Memory Integration Test ---" << std::endl;
    if (experiment.testMemoryIntegration()) {
        std::cout << "✓ Memory integration test completed successfully." << std::endl;
    } else {
        std::cerr << "⚠ Memory integration test had issues, but continuing." << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    if (experiment.testNeuromodulationIntegration()) {
        std::cout << "✓ Neuromodulation integration test completed successfully." << std::endl;
    } else {
        std::cerr << "⚠ Neuromodulation integration test had issues, but continuing." << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    if (experiment.testCheckpointing()) {
        std::cout << "✓ Checkpoint test completed successfully." << std::endl;
    } else {
        std::cerr << "⚠ Checkpoint test failed - this may be expected in some configurations." << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    if (experiment.testReplay()) {
        std::cout << "✓ Replay test completed successfully." << std::endl;
    } else {
        std::cerr << "⚠ Replay test had issues, but continuing." << std::endl;
    }
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Running complete integration simulation with enhanced metrics..." << std::endl;
    
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    // Add additional configuration options
    config.regionCount = 2;
    config.connectionProbability = 0.05f;
    
    auto result = experiment.run(config);
    
    auto endWall = std::chrono::high_resolution_clock::now();
    auto totalWallTime = std::chrono::duration<double>(endWall - startWall).count();
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    
    // Print detailed results with better formatting
    printDetailedMetrics(result);
    
    printIntegrationStatus(result);
    
    printExperimentSummary(result);
    
    std::cout << std::endl;
    std::cout << "=== EXPERIMENT METADATA ===" << std::endl;
    std::cout << "Start Time: " << (result.startTime > 0 ? std::to_string(result.startTime) : "N/A") << std::endl;
    std::cout << "End Time: " << (result.endTime > 0 ? std::to_string(result.endTime) : "N/A") << std::endl;
    std::cout << "Real Time: " << std::fixed << std::setprecision(2) << totalWallTime << "s" << std::endl;
    
    // Performance assessment
    if (totalWallTime > 0) {
        double stepsPerSecond = result.maxSteps / totalWallTime;
        std::cout << "Simulation Speed: " << std::fixed << std::setprecision(1) << stepsPerSecond << " steps/second" << std::endl;
    }
    
    return 0;
}
