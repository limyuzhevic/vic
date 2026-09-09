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

// Define error codes for structured reporting
enum class DemoErrorCode {
    SUCCESS = 0,
    INTEGRATION_FAILED = 1,
    MEMORY_FAILED = 2,
    NEUROMODULATION_FAILED = 3,
    CHECKPOINT_FAILED = 4,
    REPLAY_FAILED = 5,
    EXPERIMENT_FAILED = 6,
    UNKNOWN_ERROR = 7
};

struct DemoTestResult {
    std::string testName;
    bool passed;
    DemoErrorCode errorCode;
    std::string errorMessage;
    std::string details;
    double executionTime;
};

DemoTestResult runTest(const std::string& testName, bool condition, const std::string& errorMsg = "") {
    DemoTestResult result;
    result.testName = testName;
    result.passed = condition;
    result.errorCode = condition ? DemoErrorCode::SUCCESS : DemoErrorCode::UNKNOWN_ERROR;
    result.errorMessage = condition ? "" : errorMsg.empty() ? (testName + " failed") : errorMsg;
    result.executionTime = 0.0;
    return result;
}

DemoTestResult runTestWithCallback(const std::string& testName, 
                                 std::function<DemoTestResult()> testFunc) {
    auto start = std::chrono::high_resolution_clock::now();
    DemoTestResult result = testFunc();
    auto end = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(end - start).count();
    return result;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Store test results
    std::vector<DemoTestResult> testResults;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    auto integrationResult = runTestWithCallback("Integration Verification", [&]() {
        DetailedIntegrationResult detailedResult = experiment.verifyIntegration();
        if (!detailedResult.memoryWorkingMemoryIntegrated) {
            return runTest("Integration Verification", false, 
                          "Memory working memory not integrated: " + detailedResult.memoryWorkingMemoryError);
        }
        if (!detailedResult.memoryEpisodicMemoryIntegrated) {
            return runTest("Integration Verification", false,
                          "Memory episodic memory not integrated: " + detailedResult.memoryEpisodicMemoryError);
        }
        if (!detailedResult.neuromodulationIntegrated) {
            return runTest("Integration Verification", false,
                          "Neuromodulation not integrated: " + detailedResult.neuromodulationError);
        }
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        return runTest("Integration Verification", true, "All systems integrated successfully");
    });
    testResults.push_back(integrationResult);
    
    // Test memory integration
    std::cout << "--- Memory Integration Test ---" << std::endl;
    auto memoryResult = runTestWithCallback("Memory Integration", [&]() {
        MemoryMetrics memoryMetrics = experiment.testMemoryIntegration();
        if (!memoryMetrics.workingMemorySuccess || !memoryMetrics.episodicMemorySuccess) {
            std::string errorMsg = "Memory integration failed";
            if (!memoryMetrics.workingMemorySuccess) errorMsg += ": " + memoryMetrics.workingMemoryError;
            if (!memoryMetrics.episodicMemorySuccess) errorMsg += ": " + memoryMetrics.episodicMemoryError;
            return runTest("Memory Integration", false, errorMsg);
        }
        return runTest("Memory Integration", true, 
                      "Working memory: " + std::to_string(memoryMetrics.workingMemoryActiveTraces) + " traces, " +
                      "Episodic memory: " + std::to_string(memoryMetrics.episodicMemoryEpisodeCount) + " episodes");
    });
    testResults.push_back(memoryResult);
    
    // Test neuromodulation integration
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    auto neuromodulationResult = runTestWithCallback("Neuromodulation Integration", [&]() {
        NeuromodulationMetrics neuromodulationMetrics = experiment.testNeuromodulationIntegration();
        if (!neuromodulationMetrics.dopamineSuccess || !neuromodulationMetrics.curiositySuccess || 
            !neuromodulationMetrics.noveltySuccess || !neuromodulationMetrics.neuromodulationSignals) {
            std::string errorMsg = "Neuromodulation integration failed";
            if (!neuromodulationMetrics.dopamineSuccess) errorMsg += ": " + neuromodulationMetrics.dopamineError;
            if (!neuromodulationMetrics.curiositySuccess) errorMsg += ": " + neuromodulationMetrics.curiosityError;
            if (!neuromodulationMetrics.noveltySuccess) errorMsg += ": " + neuromodulationMetrics.noveltyError;
            if (!neuromodulationMetrics.neuromodulationSignals) errorMsg += ": " + neuromodulationMetrics.neuromodulationError;
            return runTest("Neuromodulation Integration", false, errorMsg);
        }
        return runTest("Neuromodulation Integration", true,
                      "Dopamine: " + std::to_string(neuromodulationMetrics.dopamineBaseline) + ", " +
                      "Curiosity: " + std::to_string(neuromodulationMetrics.curiosityBaseline) + ", " +
                      "Novelty: " + std::to_string(neuromodulationMetrics.noveltyBaseline));
    });
    testResults.push_back(neuromodulationResult);
    
    // Test checkpointing
    std::cout << "--- Checkpoint Test ---" << std::endl;
    auto checkpointResult = runTestWithCallback("Checkpoint Test", [&]() {
        CheckpointValidation checkpointValidation = experiment.testCheckpointing();
        if (!checkpointValidation.saveSuccess || !checkpointValidation.loadSuccess || 
            !checkpointValidation.integritySuccess) {
            std::string errorMsg = "Checkpoint test failed";
            if (!checkpointValidation.saveSuccess) errorMsg += ": " + checkpointValidation.saveError;
            if (!checkpointValidation.loadSuccess) errorMsg += ": " + checkpointValidation.loadError;
            if (!checkpointValidation.integritySuccess) errorMsg += ": " + checkpointValidation.integrityError;
            return runTest("Checkpoint Test", false, errorMsg);
        }
        return runTest("Checkpoint Test", true,
                      "Saved: " + std::to_string(checkpointValidation.savedNeurons) + " neurons, " +
                      "Loaded: " + std::to_string(checkpointValidation.loadedNeurons) + " neurons");
    });
    testResults.push_back(checkpointResult);
    
    // Test replay
    std::cout << "--- Replay Test ---" << std::endl;
    auto replayResult = runTestWithCallback("Replay Test", [&]() {
        ReplayMetrics replayMetrics = experiment.testReplay();
        if (!replayMetrics.episodicMemoryAvailable) {
            return runTest("Replay Test", false, "Episodic memory not available: " + replayMetrics.episodicMemoryError);
        }
        if (!replayMetrics.replayRetrievalSuccess) {
            return runTest("Replay Test", false, "Replay retrieval failed: " + replayMetrics.replayRetrievalError);
        }
        if (!replayMetrics.replayConsistency) {
            return runTest("Replay Test", false, "Replay inconsistency: " + replayMetrics.replayConsistencyError);
        }
        return runTest("Replay Test", true,
                      "Available episodes: " + std::to_string(replayMetrics.availableEpisodes) + ", " +
                      "Replayed: " + std::to_string(replayMetrics.replayedEpisodeCount) + ", " +
                      "Quality: " + std::to_string(replayMetrics.replayQualityScore) + "/100.0");
    });
    testResults.push_back(replayResult);
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_test.bin";
    
    auto experimentResult = runTestWithCallback("Full Integration Experiment", [&]() {
        Phase6IntegrationResult result = experiment.run(config);
        
        // Check for major failures
        if (!result.memoryWorkingMemoryIntegrated || !result.memoryEpisodicMemoryIntegrated || 
            !result.neuromodulationIntegrated || !result.developmentIntegrated) {
            std::string errorMsg = "Full experiment failed - system integration errors";
            if (!result.memoryWorkingMemoryIntegrated) errorMsg += ", Working memory failed";
            if (!result.memoryEpisodicMemoryIntegrated) errorMsg += ", Episodic memory failed";
            if (!result.neuromodulationIntegrated) errorMsg += ", Neuromodulation failed";
            if (!result.developmentIntegrated) errorMsg += ", Development failed";
            return runTest("Full Integration Experiment", false, errorMsg);
        }
        
        // Print detailed results
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") 
                  << " (" << result.memoryWorkingMemoryError << ")" << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") 
                  << " (" << result.memoryEpisodicMemoryError << ")" << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") 
                  << " (" << result.neuromodulationError << ")" << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") 
                  << " (" << result.predictionError << ")" << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") 
                  << " (" << result.developmentError << ")" << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") 
                  << " (" << result.checkpointingError << ")" << std::endl;
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") 
                  << " (" << result.replayError << ")" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        if (!result.recoveryMessage.empty()) {
            std::cout << "\n=== RECOVERY ACTIONS APPLIED ===" << std::endl;
            std::cout << "Message: " << result.recoveryMessage << std::endl;
            for (const auto& action : result.errorRecoveryActions) {
                std::cout << "  - " << action << std::endl;
            }
        }
        
        return runTest("Full Integration Experiment", true, 
                      "Experiment completed successfully with " +
                      std::to_string(result.memoryEpisodesStored) + " episodes stored");
    });
    testResults.push_back(experimentResult);
    
    // Print comprehensive test summary
    printTestSummary(testResults);
    
    return 0;
}
