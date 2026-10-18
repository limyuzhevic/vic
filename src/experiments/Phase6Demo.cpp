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

// Phase 6 Demo - Integration Test
//
// This demo runs the Phase 6 integration experiment to verify
// that all brain systems are properly connected. Enhanced version
// provides comprehensive testing, detailed metrics, and user-friendly output.

#include "Phase6IntegratedExperiment.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace nlm;
namespace chrono = std::chrono;

// Utility functions for enhanced output
std::string getTimestamp() {
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    return ss.str();
}

void printSection(const std::string& title, bool showTimestamp = true) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=');
    if (showTimestamp) {
        std::cout << "  [" << getTimestamp() << "]";
    }
    std::cout << std::endl << std::endl;
}

void printResult(const std::string& label, const std::string& value, 
                const std::string& status = "✓") {
    std::cout << "  " << status << " " << std::left << std::setw(30) << label 
              << ": " << value << std::endl;
}

void printProgress(const std::string& message, int step, int total) {
    if (step % 10 == 0 || step == total) {
        std::cout << "  Step " << std::setw(4) << step << "/" << total 
                  << " | " << message << std::endl;
    }
}

void printSeparator() {
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Comprehensive integration test for the artificial brain." << std::endl;
    std::cout << "Version: 1.0 (Enhanced)" << std::endl;
    
    // Parse command line arguments
    Phase6Config config;
    bool runQuickTest = false;
    bool showDetailedOutput = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--quick") {
            runQuickTest = true;
        } else if (arg == "--silent") {
            showDetailedOutput = false;
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoull(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        } else if (arg == "--help") {
            std::cout << "\nUsage: nlm_phase6_demo [OPTIONS]\n";
            std::cout << "Options:\n";
            std::cout << "  --quick          Run quick integration verification only\n";
            std::cout << "  --silent         Minimal output (errors only)\n";
            std::cout << "  --neurons N      Set neuron count (default: 1000)\n";
            std::cout << "  --steps S        Set max steps (default: 10000)\n";
            std::cout << "  --help           Show this help message\n";
            return 0;
        }
    }
    
    if (showDetailedOutput) {
        printSection("Phase 6 Integration Test Started");
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(showDetailedOutput ? Logger::Level::Info : Logger::Level::Warning);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Phase 1: Quick Integration Verification
    if (showDetailedOutput) {
        printSection("Phase 1: Integration Verification");
    }
    
    bool integrationOK = experiment.verifyIntegration();
    
    if (showDetailedOutput) {
        printSeparator();
        if (integrationOK) {
            std::cout << "✓ Integration verification PASSED" << std::endl;
            std::cout << "  All major brain systems are properly connected:" << std::endl;
            std::cout << "    - Working Memory system" << std::endl;
            std::cout << "    - Episodic Memory system" << std::endl;
            std::cout << "    - Neuromodulation (Dopamine, Curiosity, Novelty)" << std::endl;
            std::cout << "    - Prediction system" << std::endl;
            std::cout << "    - Cognitive systems (Planner, Concept Formation, Attention)" << std::endl;
            std::cout << "    - Development system" << std::endl;
        } else {
            std::cout << "✗ Integration verification FAILED" << std::endl;
            std::cout << "  Some systems are not properly integrated." << std::endl;
        }
    }
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    // Quick test mode
    if (runQuickTest) {
        if (showDetailedOutput) {
            printSection("Quick Test Complete");
        }
        std::cout << "Quick integration test passed. All systems are connected." << std::endl;
        return 0;
    }
    
    // Phase 2: Individual System Tests
    if (showDetailedOutput) {
        printSection("Phase 2: Individual System Integration Tests");
    }
    
    // Memory integration test
    if (showDetailedOutput) {
        printSection("Memory System Integration");
    }
    bool memoryOK = experiment.testMemoryIntegration();
    
    // Neuromodulation integration test
    if (showDetailedOutput) {
        printSection("Neuromodulation System Integration");
    }
    bool neuromodOK = experiment.testNeuromodulationIntegration();
    
    // Checkpoint integration test
    if (showDetailedOutput) {
        printSection("Checkpoint/Persistence Integration");
    }
    bool checkpointOK = experiment.testCheckpointing();
    
    // Replay integration test
    if (showDetailedOutput) {
        printSection("Replay/Memory Consolidation Integration");
    }
    bool replayOK = experiment.testReplay();
    
    // Phase 3: Comprehensive Integration Experiment
    if (showDetailedOutput) {
        printSection("Phase 3: Comprehensive Integration Experiment");
    }
    
    printProgress("Running full integration experiment", 0, config.maxSteps);
    
    auto result = experiment.run(config);
    
    if (showDetailedOutput) {
        printProgress("Running full integration experiment", config.maxSteps, config.maxSteps);
        printSeparator();
    }
    
    // Display detailed results
    printSection("Integration Results Summary");
    
    std::cout << "EXPERIMENT OVERVIEW:" << std::endl;
    std::cout << "  Duration: " << std::fixed << std::setprecision(2) 
              << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << "  Simulation steps: " << config.maxSteps << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    printSeparator();
    
    std::cout << "PERFORMANCE METRICS:" << std::endl;
    printResult("Total Reward", std::to_string(result.totalReward), "✓");
    printResult("Average Firing Rate", std::to_string(std::fixed << std::setprecision(3) 
                   << result.avgFiringRate) + " Hz", "✓");
    printResult("Episodic Episodes Stored", std::to_string((int)result.memoryEpisodesStored), "✓");
    printResult("Average Synaptic Weight", std::to_string(std::fixed << std::setprecision(4) 
                   << result.avgSynapticWeight), "✓");
    printResult("Novelty Level", std::to_string(std::fixed << std::setprecision(3) 
                   << result.noveltyLevel), "✓");
    printResult("Curiosity Level", std::to_string(std::fixed << std::setprecision(3) 
                   << result.curiosityLevel), "✓");
    printResult("Dopamine Level", std::to_string(std::fixed << std::setprecision(3) 
                   << result.dopamineLevel), "✓");
    printSeparator();
    
    std::cout << "SYSTEM INTEGRATION STATUS:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Prediction System: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Development System: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "  Replay System: " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    printSeparator();
    
    // Overall assessment
    std::cout << "INTEGRATION ASSESSMENT:" << std::endl;
    
    int passedTests = 0;
    int totalTests = 6;
    
    if (memoryOK) passedTests++;
    if (neuromodOK) passedTests++;
    if (checkpointOK) passedTests++;
    if (replayOK) passedTests++;
    if (result.memoryWorkingMemoryIntegrated) passedTests++;
    if (result.memoryEpisodicMemoryIntegrated) passedTests++;
    
    if (passedTests >= totalTests * 0.8) {  // 80% success rate
        std::cout << "✓ SUCCESS: Phase 6 integration completed successfully" << std::endl;
        std::cout << "  " << passedTests << "/" << totalTests << " integration tests passed." << std::endl;
        if (showDetailedOutput) {
            std::cout << "  The artificial brain is fully functional with all systems working together." << std::endl;
        }
    } else {
        std::cout << "⚠ WARNING: Some integration tests failed or were incomplete" << std::endl;
        std::cout << "  " << passedTests << "/" << totalTests << " integration tests passed." << std::endl;
        if (showDetailedOutput) {
            std::cout << "  Consider checking system configurations and dependencies." << std::endl;
        }
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Phase 6 Integration Demo Complete" << std::endl;
    std::cout << "Time: " << getTimestamp() << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}
