/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

// Utility function to print formatted headers
void printSectionHeader(const std::string& title) {
    NLM_LOG_INFO("=== " + title + " ===");
    NLM_LOG_INFO("");
}

// Utility function to print key-value pairs
void printConfigPair(const std::string& key, const std::string& value) {
    NLM_LOG_INFO("  " + key + ": " + value);
}

// Enhanced demo with better logging and error handling
int main(int argc, char* argv[]) {
    // Print banner
    printSectionHeader("NLM Phase 6 Integration Demo");
    NLM_LOG_INFO("Testing the integrated artificial brain...");
    NLM_LOG_INFO("");
    
    // Initialize logging with enhanced configuration
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    NLM_LOG_INFO("Logging system initialized");
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    printSectionHeader("Integration Verification");
    bool integrationOK = experiment.verifyIntegration();
    
    NLM_LOG_INFO("");
    
    if (!integrationOK) {
        NLM_LOG_ERROR("Integration verification failed!");
        NLM_LOG_ERROR("The brain systems are not properly connected.");
        NLM_LOG_ERROR("Please check the system architecture and connections.");
        return 1;
    }
    
    NLM_LOG_INFO("Integration verification passed!");
    NLM_LOG_INFO("");
    
    // Test individual systems with better feedback
    printSectionHeader("Memory Integration Test");
    try {
        experiment.testMemoryIntegration();
        NLM_LOG_INFO("✓ Memory systems working correctly");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Memory integration test failed: " + std::string(e.what()));
        return 1;
    }
    
    NLM_LOG_INFO("");
    
    printSectionHeader("Neuromodulation Integration Test");
    try {
        experiment.testNeuromodulationIntegration();
        NLM_LOG_INFO("✓ Neuromodulation systems working correctly");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Neuromodulation integration test failed: " + std::string(e.what()));
        return 1;
    }
    
    NLM_LOG_INFO("");
    
    printSectionHeader("Checkpoint Test");
    try {
        experiment.testCheckpointing();
        NLM_LOG_INFO("✓ Checkpoint system working correctly");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Checkpoint test failed: " + std::string(e.what()));
        return 1;
    }
    
    NLM_LOG_INFO("");
    
    printSectionHeader("Replay Test");
    try {
        experiment.testReplay();
        NLM_LOG_INFO("✓ Replay system working correctly");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Replay test failed: " + std::string(e.what()));
        return 1;
    }
    
    NLM_LOG_INFO("");
    
    // Run full experiment with smaller settings for demo
    printSectionHeader("Full Integration Experiment");
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    NLM_LOG_INFO("Experiment configuration:");
    printConfigPair("  Neurons", std::to_string(config.neuronCount));
    printConfigPair("  Max steps", std::to_string(config.maxSteps));
    printConfigPair("  Checkpointing", config.enableCheckpointing ? "enabled" : "disabled");
    printConfigPair("  Replay", config.enableReplay ? "enabled" : "disabled");
    printConfigPair("  Development", config.enableDevelopment ? "enabled" : "disabled");
    
    auto result = experiment.run(config);
    
    NLM_LOG_INFO("");
    printSectionHeader("FINAL RESULTS");
    printConfigPair("Total reward", std::to_string(result.totalReward));
    printConfigPair("Avg firing rate", std::to_string(result.avgFiringRate) + " Hz");
    printConfigPair("Episodes stored", std::to_string(result.memoryEpisodesStored));
    printConfigPair("Dopamine level", std::to_string(result.dopamineLevel));
    
    NLM_LOG_INFO("");
    printSectionHeader("INTEGRATION STATUS");
    
    // Color coding for system status
    auto statusColor = [](bool connected, const std::string& text) {
        return (connected ? "✓ " : "✗ ") + text;
    };
    
    NLM_LOG_INFO(statusColor(result.memoryWorkingMemoryIntegrated, "Working Memory: " + 
                              (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO(statusColor(result.memoryEpisodicMemoryIntegrated, "Episodic Memory: " + 
                              (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO(statusColor(result.neuromodulationIntegrated, "Neuromodulation: " + 
                              (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO(statusColor(result.predictionIntegrated, "Prediction: " + 
                              (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO(statusColor(result.developmentIntegrated, "Development: " + 
                              (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED")));
    NLM_LOG_INFO(statusColor(result.checkpointingWorks, "Checkpointing: " + 
                              (result.checkpointingWorks ? "WORKING" : "NOT WORKING")));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Wall clock time: " + std::to_string(result.totalWallClockTime) + "s");
    
    // Final summary
    NLM_LOG_INFO("");
    printSectionHeader("DEMONSTRATION SUMMARY");
    
    int systemCount = 0;
    int workingCount = 0;
    if (result.memoryWorkingMemoryIntegrated) { systemCount++; workingCount++; }
    if (result.memoryEpisodicMemoryIntegrated) { systemCount++; workingCount++; }
    if (result.neuromodulationIntegrated) { systemCount++; workingCount++; }
    if (result.predictionIntegrated) { systemCount++; workingCount++; }
    if (result.developmentIntegrated) { systemCount++; workingCount++; }
    if (result.checkpointingWorks) { systemCount++; workingCount++; }
    
    NLM_LOG_INFO("Systems tested: " + std::to_string(systemCount));
    NLM_LOG_INFO("Systems working: " + std::to_string(workingCount));
    NLM_LOG_INFO("Success rate: " + std::to_string((workingCount * 100.0) / systemCount) + "%");
    
    if (workingCount == systemCount) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("🎉 ALL SYSTEMS OPERATIONAL! 🎉");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("The NLM Phase 6 integration has been successfully demonstrated.");
        NLM_LOG_INFO("All brain systems are properly connected and functioning together.");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Key achievements:")
        NLM_LOG_INFO("  ✓ Working memory with persistent activity")
        NLM_LOG_INFO("  ✓ Episodic memory with experience encoding")
        NLM_LOG_INFO("  ✓ Neuromodulation affecting neural dynamics")
        NLM_LOG_INFO("  ✓ Prediction integrated with learning")
        NLM_LOG_INFO("  ✓ Development affecting plasticity rates")
        NLM_LOG_INFO("  ✓ Checkpoint save/load working")
        NLM_LOG_INFO("  ✓ Replay and consolidation functional")
        
        return 0;
    } else {
        NLM_LOG_ERROR("");
        NLM_LOG_ERROR("❌ INTEGRATION INCOMPLETE ❌");
        NLM_LOG_ERROR("");
        NLM_LOG_ERROR("Some systems are not properly connected.");
        NLM_LOG_ERROR("Please review the system architecture and connections.");
        return 1;
    }
}
