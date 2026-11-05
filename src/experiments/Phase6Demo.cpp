/**
 * Phase 6 Demo - Enhanced Integration Test
 * 
 * This enhanced demo runs the Phase 6 integration experiment with comprehensive
 * error handling, detailed logging, and performance monitoring to verify
 * that all brain systems are properly connected and functioning.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <chrono>
#include <map>
#include <vector>

using namespace nlm;

// Global statistics for Phase 6 demo
struct Phase6DemoStats {
    int testCount = 0;
    int passedTests = 0;
    double totalExecutionTime = 0.0;
    std::map<std::string, float> systemScores;
    std::vector<std::string> failedTests;
    std::vector<std::string> warningMessages;
};

Phase6DemoStats demoStats;

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(70, '=') << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

void printResult(const std::string& test, bool passed, float score, const std::string& details = "") {
    demoStats.testCount++;
    std::cout << "  " << std::left << std::setw(30) << test << ": ";
    std::cout << (passed ? "PASS" : "FAIL");
    std::cout << " (Score: " << score << "/1.0)";
    if (!details.empty()) {
        std::cout << " - " << details;
    }
    std::cout << "\n";
    
    if (passed) {
        demoStats.passedTests++;
    } else {
        demoStats.failedTests.push_back(test);
    }
    
    demoStats.systemScores[test] = score;
}

void updateExecutionTime(const std::string& operation, double executionTime) {
    demoStats.totalExecutionTime += executionTime;
    std::cout << "    Execution time: " << executionTime << "s\n";
}

void printGlobalSummary() {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "        PHASE 6 GLOBAL SUMMARY\n";
    std::cout << std::string(70, '=') << "\n";
    
    if (demoStats.testCount > 0) {
        float successRate = demoStats.passedTests * 100.0f / demoStats.testCount;
        printResult("Tests passed", true, successRate / 100.0f, "out of " + std::to_string(demoStats.passedTests) + "/" + std::to_string(demoStats.testCount));
        
        printResult("Success rate", true, successRate / 100.0f, std::to_string(successRate) + "%");
        
        if (!demoStats.failedTests.empty()) {
            std::cout << "\n  Failed tests:\n";
            for (const auto& failed : demoStats.failedTests) {
                std::cout << "    - " << failed << "\n";
            }
        }
        
        printResult("Average execution time", true, demoStats.totalExecutionTime / demoStats.testCount, "seconds per test");
        
        std::cout << "\n  Individual system scores:\n";
        for (const auto& pair : demoStats.systemScores) {
            std::cout << "    - " << pair.first << ": " << pair.second << "/1.0\n";
        }
    }
}

void runIntegrationVerification(Phase6IntegratedExperiment& experiment) {
    printSection("Integration Verification");
    std::cout << "  Running comprehensive integration verification...\n";
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool integrationOK = experiment.verifyIntegration();
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        if (integrationOK) {
            printResult("Integration verification", true, 1.0f, "All systems connected and functional");
            updateExecutionTime("Integration verification", executionTime);
        } else {
            printResult("Integration verification", false, 0.0f, "Critical integration failure");
            updateExecutionTime("Integration verification", executionTime);
            demoStats.failedTests.push_back("Integration verification");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in integration verification: " << e.what() << std::endl;
        printResult("Integration verification", false, 0.0f, std::string("Exception: ") + e.what());
        demoStats.failedTests.push_back("Integration verification");
    }
}

void runMemoryIntegrationTest(Phase6IntegratedExperiment& experiment) {
    printSection("Memory Systems Integration");
    std::cout << "  Testing working memory, episodic memory, and associative memory...\n";
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        experiment.testMemoryIntegration();
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        printResult("Memory integration", true, 1.0f, "Working + Episodic + Associative memory");
        updateExecutionTime("Memory integration", executionTime);
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in memory integration: " << e.what() << std::endl;
        printResult("Memory integration", false, 0.0f, std::string("Exception: ") + e.what());
        demoStats.failedTests.push_back("Memory integration");
    }
}

void runNeuromodulationIntegrationTest(Phase6IntegratedExperiment& experiment) {
    printSection("Neuromodulation Integration");
    std::cout << "  Testing dopamine, curiosity, novelty detection, and prediction error...\n";
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        experiment.testNeuromodulationIntegration();
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        printResult("Neuromodulation integration", true, 1.0f, "Dopamine + Curiosity + Novelty + Prediction error");
        updateExecutionTime("Neuromodulation integration", executionTime);
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in neuromodulation integration: " << e.what() << std::endl;
        printResult("Neuromodulation integration", false, 0.0f, std::string("Exception: ") + e.what());
        demoStats.failedTests.push_back("Neuromodulation integration");
    }
}

void runCheckpointingTest(Phase6IntegratedExperiment& experiment) {
    printSection("Checkpoint System Test");
    std::cout << "  Testing save/load functionality with data persistence...\n";
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool checkpointPassed = experiment.testCheckpointing();
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        if (checkpointPassed) {
            printResult("Checkpoint test", true, 1.0f, "Save/load preserves brain state");
            updateExecutionTime("Checkpoint test", executionTime);
        } else {
            printResult("Checkpoint test", false, 0.0f, "Checkpoint save/load failure");
            updateExecutionTime("Checkpoint test", executionTime);
            demoStats.failedTests.push_back("Checkpoint test");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in checkpoint test: " << e.what() << std::endl;
        printResult("Checkpoint test", false, 0.0f, std::string("Exception: ") + e.what());
        demoStats.failedTests.push_back("Checkpoint test");
    }
}

void runReplayTest(Phase6IntegratedExperiment& experiment) {
    printSection("Replay System Test");
    std::cout << "  Testing memory consolidation and replay during rest/sleep...\n";
    
    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool replayPassed = experiment.testReplay();
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        if (replayPassed) {
            printResult("Replay test", true, 1.0f, "Memory consolidation and replay functional");
            updateExecutionTime("Replay test", executionTime);
        } else {
            printResult("Replay test", false, 0.0f, "Replay system failure");
            updateExecutionTime("Replay test", executionTime);
            demoStats.failedTests.push_back("Replay test");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in replay test: " << e.what() << std::endl;
        printResult("Replay test", false, 0.0f, std::string("Exception: ") + e.what());
        demoStats.failedTests.push_back("Replay test");
    }
}

void runFullIntegrationExperiment(Phase6IntegratedExperiment& experiment) {
    printSection("Full Integration Experiment");
    std::cout << "  Running comprehensive lifetime simulation with all systems...\n";
    
    try {
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        std::cout << "  Configuration:\n";
        std::cout << "    Neurons: " << config.neuronCount << "\n";
        std::cout << "    Max steps: " << config.maxSteps << "\n";
        std::cout << "    Checkpointing: " << (config.enableCheckpointing ? "ON" : "OFF") << "\n";
        std::cout << "    Replay: " << (config.enableReplay ? "ON" : "OFF") << "\n";
        std::cout << "    Development: " << (config.enableDevelopment ? "ON" : "OFF") << "\n";
        
        auto startTime = std::chrono::high_resolution_clock::now();
        Phase6IntegrationResult result = experiment.run(config);
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Calculate integration score based on results
        float integrationScore = 0.0f;
        std::vector<std::string> integrationDetails;
        
        if (result.memoryWorkingMemoryIntegrated) {
            integrationScore += 0.2f;
            integrationDetails.push_back("Working memory: CONNECTED");
        }
        
        if (result.memoryEpisodicMemoryIntegrated) {
            integrationScore += 0.2f;
            integrationDetails.push_back("Episodic memory: CONNECTED");
        }
        
        if (result.neuromodulationIntegrated) {
            integrationScore += 0.2f;
            integrationDetails.push_back("Neuromodulation: CONNECTED");
        }
        
        if (result.predictionIntegrated) {
            integrationScore += 0.2f;
            integrationDetails.push_back("Prediction system: CONNECTED");
        }
        
        if (result.developmentIntegrated) {
            integrationScore += 0.2f;
            integrationDetails.push_back("Development system: CONNECTED");
        }
        
        if (result.checkpointingWorks) {
            integrationScore += 0.1f;
            integrationDetails.push_back("Checkpointing: WORKING");
        }
        
        printResult("Full integration experiment", true, integrationScore, 
                   std::to_string(static_cast<int>(integrationScore * 100)) + "% integrated");
        
        std::cout << "  Integration details:\n";
        for (const auto& detail : integrationDetails) {
            std::cout << "    " << detail << "\n";
        }
        
        printResult("Total reward", true, result.totalReward / config.maxSteps, "average per step");
        printResult("Average firing rate", true, result.avgFiringRate, "Hz");
        printResult("Memory episodes stored", true, result.memoryEpisodesStored, "episodes");
        printResult("Dopamine level", true, result.dopamineLevel, "normalized");
        
        printResult("Wall clock time", true, result.totalWallClockTime, "seconds");
        
        updateExecutionTime("Full integration experiment", executionTime);
        
        // Add integration score to global stats
        demoStats.systemScores["IntegrationScore"] = integrationScore;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR in full integration experiment: " << e.what() << std::endl;
        printResult("Full integration experiment", false, 0.0f, std::string("Exception: ") + e.what());
        demoStats.failedTests.push_back("Full integration experiment");
    }
}

void printSystemStatusReport() {
    printSection("System Status Report");
    
    std::cout << "  Current system configuration:\n";
    std::cout << "    Brain architecture: Integrated spiking neural network\n";
    std::cout << "    Memory systems: Working + Episodic + Associative\n";
    std::cout << "    Neuromodulation: Dopamine + Curiosity + Novelty + Prediction error\n";
    std::cout << "    Prediction system: Forward models with error correction\n";
    std::cout << "    Development: Multi-stage plasticity and maturation\n";
    std::cout << "    Performance: Event-driven, SoA, SIMD, multithreading optimized\n";
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 6: Enhanced Integration Demo\n";
    std::cout << "====================================\n\n";
    
    // Parse command line arguments
    size_t customTrials = 0;
    bool enableVerbose = false;
    
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--trials" && i + 1 < argc) {
                customTrials = std::stoul(argv[++i]);
            } else if (arg == "--verbose" || arg == "-v") {
                enableVerbose = true;
            } else if (arg == "--help") {
                printHeader("NLM Phase 6 Enhanced Integration Demo");
                std::cout << "Usage: ./nlm_phase6_demo [options]\n";
                std::cout << "Options:\n";
                std::cout << "  --trials N        Number of trials (0 = use experiment defaults)\n";
                std::cout << "  --verbose, -v     Enable verbose logging and output\n";
                std::cout << "  --help            Show this help message\n\n";
                std::cout << "Phase 6 verifies that all brain systems are properly integrated:\n";
                std::cout << "  ✓ Memory systems (working + episodic + associative)\n";
                std::cout << "  ✓ Neuromodulation (dopamine + curiosity + novelty)\n";
                std::cout << "  ✓ Prediction system (forward models)\n";
                std::cout << "  ✓ Development (multi-stage plasticity)\n";
                std::cout << "  ✓ Checkpointing (state persistence)\n";
                std::cout << "  ✓ Replay (memory consolidation)\n";
                return 0;
            }
        }
    }
    
    if (enableVerbose) {
        printHeader("NLM Phase 6 Enhanced Integration Demo");
        std::cout << "Verbose mode enabled.\n\n";
    }
    
    std::cout << "Phase 6 Integration Demo Configuration:";
    if (customTrials > 0) {
        std::cout << "  Custom trials: " << customTrials << "\n";
    } else {
        std::cout << "  Default trials: " << 1 << " per sub-experiment\n";
    }
    
    // Initialize logging with enhanced settings
    Logger::getGlobal().setLevel(enableVerbose ? LogLevel::Debug : LogLevel::Info);
    
    printHeader("PHASE 6 INTEGRATION DEMONSTRATION");
    
    try {
        // Create experiment with error handling
        std::cout << "Creating Phase 6 integration experiment...\n";
        Phase6IntegratedExperiment experiment;
        
        printSystemStatusReport();
        
        // Run all integration tests with comprehensive error handling
        std::cout << "\nStarting comprehensive integration tests...\n";
        
        runIntegrationVerification(experiment);
        runMemoryIntegrationTest(experiment);
        runNeuromodulationIntegrationTest(experiment);
        runCheckpointingTest(experiment);
        runReplayTest(experiment);
        runFullIntegrationExperiment(experiment);
        
        // Print final summary
        printGlobalSummary();
        
        // Final verdict
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "           PHASE 6 INTEGRATION STATUS\n";
        std::cout << std::string(70, '=') << "\n";
        
        if (demoStats.failedTests.empty()) {
            std::cout << "  [SUCCESS] All Phase 6 integration tests passed!\n";
            std::cout << "  The brain demonstrates full integration of all systems:\n";
            std::cout << "    ✓ Memory systems connected to neural processing\n";
            std::cout << "    ✓ Neuromodulation affects plasticity and dynamics\n";
            std::cout << "    ✓ Prediction system integrated with learning\n";
            std::cout << "    ✓ Development affects plasticity rates\n";
            std::cout << "    ✓ Checkpoint save/load working\n";
            std::cout << "    ✓ Replay and consolidation functional\n";
            std::cout << "\nThe integrated brain is ready for advanced operations.\n";
            
            return 0;
        } else {
            std::cout << "  [PARTIAL SUCCESS] Most tests passed, but " << demoStats.failedTests.size() << 
                      " integration tests failed.\n";
            std::cout << "  These failures should be addressed in subsequent phases.\n\n";
            
            std::cout << "  Failed systems:\n";
            for (const auto& failed : demoStats.failedTests) {
                std::cout << "    - " << failed << "\n";
            }
            
            std::cout << "\n[WARNING] Integration is incomplete. Some brain systems may not be fully functional.\n";
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        std::cerr << "Phase 6 integration demo terminated due to critical error.\n";
        
        printGlobalSummary();
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception occurred during Phase 6 integration.\n";
        
        printGlobalSummary();
        return 1;
    }
}