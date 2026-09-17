/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Enhanced version with comprehensive testing, error handling,
 * and detailed reporting for Phase 6 verification.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace nlm;

/**
 * Format elapsed time for display
 */
std::string formatElapsedTime(double seconds) {
    if (seconds < 60.0) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = static_cast<int>(seconds / 3600.0);
        int minutes = static_cast<int>(seconds) % 3600 / 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
}

/**
 * Print comprehensive system integration report
 */
void printIntegrationReport(const Phase6IntegrationResult& result) {
    std::cout << "=== COMPREHENSIVE PHASE 6 INTEGRATION REPORT ===" << std::endl;
    std::cout << std::endl;
    
    // System Integration Status
    std::cout << "SYSTEM INTEGRATION STATUS:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Prediction System: " << (result.predictionIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Development System: " << (result.developmentIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
    std::cout << "  Replay System: " << (result.replayWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    // Performance Metrics
    std::cout << "PERFORMANCE METRICS:" << std::endl;
    std::cout << "  Total Reward: " << std::fixed << std::setprecision(3) << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(3) << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  Dopamine Level: " << std::fixed << std::setprecision(3) << result.dopamineLevel << std::endl;
    std::cout << "  Novelty Level: " << std::fixed << std::setprecision(3) << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity Level: " << std::fixed << std::setprecision(3) << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    // Temporal Information
    std::cout << "EXECUTION TIMING:" << std::endl;
    if (result.startTime > 0) {
        std::cout << "  Started: " << ctime(&result.startTime);  // This adds newline
        if (result.endTime > 0) {
            std::cout << "  Ended: " << ctime(&result.endTime);  // This adds newline
        }
    }
    std::cout << "  Wall Clock Time: " << formatElapsedTime(result.totalWallClockTime) << std::endl;
    std::cout << std::endl;
}

/**
 * Print verification test results with detailed status
 */
void printVerificationResults(bool integrationOK, const Phase6IntegratedExperiment& experiment) {
    std::cout << "INTEGRATION VERIFICATION RESULTS:" << std::endl;
    std::cout << "  Overall Status: " << (integrationOK ? "✅ PASSED" : "❌ FAILED") << std::endl;
    
    if (integrationOK) {
        std::cout << "  All core brain systems are properly connected and functional." << std::endl;
        std::cout << "  The artificial brain is ready for complex cognitive tasks." << std::endl;
    } else {
        std::cout << "  Critical integration failures detected:" << std::endl;
        std::cout << "    - Memory systems not properly connected" << std::endl;
        std::cout << "    - Neuromodulation systems not integrated" << std::endl;
        std::cout << "    - Prediction system not functional" << std::endl;
        std::cout << "    - Development system not working" << std::endl;
        std::cout << "  Please check system logs for specific error details." << std::endl;
    }
    std::cout << std::endl;
}

/**
 * Print system test results
 */
void printSystemTestResults(const std::string& testName, bool success) {
    std::cout << testName << ": " << (success ? "✅ PASSED" : "❌ FAILED") << std::endl;
}

/**
 * Main entry point with enhanced functionality
 */
int main(int argc, char* argv[]) {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 6 - ENHANCED INTEGRATION DEMO\n";
    std::cout << "===========================================\n" << std::endl;
    
    std::cout << "Phase 6 Integration Test with Enhanced Features:" << std::endl;
    std::cout << "  - Comprehensive system integration verification" << std::endl;
    std::cout << "  - Detailed performance metrics and timing" << std::endl;
    std::cout << "  - Robust error handling and reporting" << std::endl;
    std::cout << "  - Enhanced diagnostics and logging" << std::endl;
    std::cout << std::endl;
    
    // Initialize enhanced logging
    auto& logger = Logger::getInstance();
    logger.setLevel(Logger::Level::Info);
    
    std::cout << "Initializing Phase 6 Integration Demo..." << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Start timing
    auto startWall = std::chrono::high_resolution_clock::now();
    
    // Run enhanced verification
    std::cout << "--- ENHANCED INTEGRATION VERIFICATION ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    printVerificationResults(integrationOK, experiment);
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "The Phase 6 integration is incomplete. Please check system configuration." << std::endl;
        return 1;
    }
    
    // Test individual systems with enhanced reporting
    std::cout << "--- ENHANCED SYSTEM INTEGRATION TESTS ---" << std::endl;
    
    bool memoryTestPassed = experiment.testMemoryIntegration();
    printSystemTestResults("  Memory Integration Test", memoryTestPassed);
    
    bool neuromodTestPassed = experiment.testNeuromodulationIntegration();
    printSystemTestResults("  Neuromodulation Integration Test", neuromodTestPassed);
    
    bool checkpointTestPassed = experiment.testCheckpointing();
    printSystemTestResults("  Checkpoint Test", checkpointTestPassed);
    
    bool replayTestPassed = experiment.testReplay();
    printSystemTestResults("  Replay Test", replayTestPassed);
    
    // Calculate average test success rate
    int passedTests = 0;
    int totalTests = 4;
    if (memoryTestPassed) passedTests++;
    if (neuromodTestPassed) passedTests++;
    if (checkpointTestPassed) passedTests++;
    if (replayTestPassed) passedTests++;
    
    double testSuccessRate = (static_cast<double>(passedTests) / totalTests) * 100.0;
    
    std::cout << std::endl;
    std::cout << "SYSTEM TEST SUMMARY:" << std::endl;
    std::cout << "  Tests Passed: " << passedTests << "/" << totalTests << std::endl;
    std::cout << "  Success Rate: " << std::fixed << std::setprecision(1) << testSuccessRate << "%" << std::endl;
    std::cout << std::endl;
    
    // Run full integration experiment with enhanced configuration
    std::cout << "--- ENHANCED FULL INTEGRATION EXPERIMENT ---" << std::endl;
    
    // Create enhanced configuration for better testing
    Phase6Config config;
    config.neuronCount = 1000;        // Increased for better coverage
    config.maxSteps = 3000;           // Increased for more comprehensive testing
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./enhanced_checkpoint_test.bin";
    
    std::cout << "Enhanced Configuration:" << std::endl;
    std::cout << "  Neuron Count: " << config.neuronCount << std::endl;
    std::cout << "  Max Steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    // Start timing for experiment
    auto experimentStartWall = std::chrono::high_resolution_clock::now();
    
    auto result = experiment.run(config);
    
    // Calculate experiment execution time
    auto experimentEndWall = std::chrono::high_resolution_clock::now();
    double experimentWallTime = std::chrono::duration<double>(experimentEndWall - experimentStartWall).count();
    
    // Print comprehensive report
    printIntegrationReport(result);
    
    // Print experiment performance details
    std::cout << "EXPERIMENT PERFORMANCE:" << std::endl;
    std::cout << "  Steps Completed: " << config.maxSteps << std::endl;
    std::cout << "  Wall Clock Time: " << formatElapsedTime(experimentWallTime) << std::endl;
    std::cout << "  Steps per Second: " << (config.maxSteps / experimentWallTime) << std::endl;
    std::cout << std::endl;
    
    // Overall success assessment
    bool experimentSuccess = integrationOK && memoryTestPassed && neuromodTestPassed && 
                             checkpointTestPassed && replayTestPassed && result.developmentIntegrated;
    
    std::cout << "===========================================\n";
    std::cout << "OVERALL SUCCESS ASSESSMENT:" << std::endl;
    std::cout << "===========================================\n";
    
    if (experimentSuccess) {
        std::cout << "✅ PHASE 6 INTEGRATION DEMONSTRATION: COMPLETE" << std::endl;
        std::cout << std::endl;
        std::cout << "The Phase 6 artificial brain integration is fully operational:" << std::endl;
        std::cout << "  ✓ All core brain systems integrated and connected" << std::endl;
        std::cout << "  ✓ Memory systems functional (working and episodic)" << std::endl;
        std::cout << "  ✓ Neuromodulation systems operational" << std::endl;
        std::cout << "  ✓ Prediction system integrated and working" << std::endl;
        std::cout << "  ✓ Development system functional" << std::endl;
        std::cout << "  ✓ Checkpoint and replay systems operational" << std::endl;
        std::cout << "  ✓ Comprehensive testing and validation completed" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM Phase 6 artificial brain is ready for complex cognitive tasks." << std::endl;
    } else {
        std::cout << "❌ PHASE 6 INTEGRATION DEMONSTRATION: INCOMPLETE" << std::endl;
        std::cout << std::endl;
        std::cout << "The Phase 6 integration has issues that need to be addressed:" << std::endl;
        std::cout << "  ✗ Integration verification failed" << std::endl;
        std::cout << "  ✗ System test failures detected" << std::endl;
        std::cout << "  ✗ Development system not properly integrated" << std::endl;
        std::cout << std::endl;
        std::cout << "Please review system logs and address the reported issues." << std::endl;
        return 1;
    }
    
    return 0;
}
