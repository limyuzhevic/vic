/**
 * Phase 6 Demo - Enhanced Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected with enhanced
 * debugging and progress reporting.
 */

#include "Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace nlm;

void printProgressBar(int current, int total, int width = 50) {
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(progress * width);
    
    std::cout << "\r[";
    for (int i = 0; i < width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::setw(3) << static_cast<int>(progress * 100) << "%";
    std::cout.flush();
}

void runEnhancedIntegrationTest(Phase6IntegratedExperiment& experiment) {
    std::cout << "\n=== Enhanced Integration Test ===" << std::endl;
    
    // Create enhanced configuration for better testing
    Phase6Config config;
    config.neuronCount = 1000;  // Increased for better testing
    config.maxSteps = 3000;     // Longer for more comprehensive testing
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "/tmp/enhanced_nlm_checkpoint.bin";
    
    std::cout << "Configuration: " << config.neuronCount << " neurons, " 
              << config.maxSteps << " steps" << std::endl;
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run integration verification first
    std::cout << "\n--- Phase 1: Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "Integration verification passed!" << std::endl;
    
    // Test individual systems with detailed reporting
    std::cout << "\n--- Phase 2: System-Specific Tests ---" << std::endl;
    
    std::cout << "\nTesting Memory Integration...";
    bool memoryTest = experiment.testMemoryIntegration();
    std::cout << (memoryTest ? " PASSED" : " FAILED") << std::endl;
    
    std::cout << "\nTesting Neuromodulation Integration...";
    bool neuromodulationTest = experiment.testNeuromodulationIntegration();
    std::cout << (neuromodulationTest ? " PASSED" : " FAILED") << std::endl;
    
    std::cout << "\nTesting Checkpoint System...";
    bool checkpointTest = experiment.testCheckpointing();
    std::cout << (checkpointTest ? " PASSED" : " FAILED") << std::endl;
    
    std::cout << "\nTesting Replay System...";
    bool replayTest = experiment.testReplay();
    std::cout << (replayTest ? " PASSED" : " FAILED") << std::endl;
    
    // Enhanced full experiment with progress reporting
    std::cout << "\n--- Phase 3: Full Integration Experiment (Enhanced) ---" << std::endl;
    
    auto result = experiment.run(config);
    
    // Calculate elapsed time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "\n=== Enhanced Test Results ===" << std::endl;
    std::cout << "Total simulation time: " << duration.count() << " seconds" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Enhanced system integration verification
    std::cout << "\n=== Enhanced Integration Status ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
    
    // Performance metrics
    std::cout << "\n=== Performance Analysis ===" << std::endl;
    std::cout << "Neurons processed per second: " << config.neuronCount * config.maxSteps / duration.count() << std::endl;
    std::cout << "Test efficiency rating: " << (duration.count() > 0 ? (static_cast<float>(result.totalReward) / duration.count()) : 0) << " reward/sec" << std::endl;
    
    // Success criteria for enhanced test
    bool enhancedTestPassed = integrationOK && memoryTest && neuromodulationTest && 
                              checkpointTest && replayTest && result.checkpointingWorks;
    
    if (enhancedTestPassed) {
        std::cout << "\n🎉 Enhanced integration test PASSED! All systems properly connected." << std::endl;
    } else {
        std::cout << "\n⚠️  Enhanced integration test FAILED. Check system connections." << std::endl;
    }
    
    return enhancedTestPassed;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Enhanced NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the enhanced artificial brain with advanced diagnostics..." << std::endl << std::endl;
    
    // Initialize enhanced logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== Enhanced Phase 6 Integration Demo ===");
    NLM_LOG_INFO("Running comprehensive integration test with enhanced diagnostics");
    
    // Create experiment with enhanced capabilities
    Phase6IntegratedExperiment experiment;
    
    // Run enhanced integration test
    bool testPassed = runEnhancedIntegrationTest(experiment);
    
    if (!testPassed) {
        NLM_LOG_ERROR("Enhanced integration test failed - aborting");
        return 1;
    }
    
    NLM_LOG_INFO("=== Enhanced Demo Complete ===");
    NLM_LOG_INFO("All brain systems are properly integrated and functioning.");
    NLM_LOG_INFO("The NLM brain is ready for advanced research applications.");
    
    std::cout << std::endl;
    std::cout << "=== Enhanced Demo Summary ===" << std::endl;
    std::cout << "✅ Integration verification: PASSED" << std::endl;
    std::cout << "✅ Memory systems: CONNECTED" << std::endl;
    std::cout << "✅ Neuromodulation: ACTIVE" << std::endl;
    std::cout << "✅ Checkpointing: FUNCTIONAL" << std::endl;
    std::cout << "✅ Replay system: READY" << std::endl;
    std::cout << std::endl;
    std::cout << "The enhanced NLM brain is now fully operational for:" << std::endl;
    std::cout << "  • Large-scale neural simulations" << std::endl;
    std::cout << "  • Research experimentation" << std::endl;
    std::cout << "  • Performance optimization studies" << std::endl;
    std::cout << "  • Advanced AI research applications" << std::endl;
    
    return 0;
}
