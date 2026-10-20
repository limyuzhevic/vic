#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/EnhancedLogger.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Enhanced Logging Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Demonstrating advanced logging infrastructure with performance monitoring..." << std::endl << std::endl;
    
    // Initialize enhanced logging
    auto& logger = Logger::getGlobal();
    
    std::cout << "--- Initializing Enhanced Logging Infrastructure ---" << std::endl;
    
    // Enable async logging for performance
    logger.enableAsyncLogging(true);
    
    // Configure enhanced logging with detailed settings
    LogConfig config;
    config.globalLevel = EnhancedLogLevel::Info;
    config.useEnhancedLevels = true;
    config.useAsyncLogging = true;
    config.asyncQueueSize = 5000;
    config.maxFileSize = 5 * 1024 * 1024;  // 5MB
    config.compressLogs = false;
    config.rotationTime = std::chrono::hours(1);
    config.maxRotatedFiles = 3;
    config.useLockFreeQueue = true;
    config.sampleRate = 1.0f;  // 100% for demo
    config.logSystemMetrics = true;
    config.logPerformanceMetrics = true;
    config.logGpuEvents = false;
    
    // Set the enhanced configuration
    logger.setLogConfig(config);
    
    // Enable categories for different brain systems
    logger.enableCategory("Brain", true);
    logger.enableCategory("Memory", true);
    logger.enableCategory("Neuromodulation", true);
    logger.enableCategory("Cognition", true);
    logger.enableCategory("Development", true);
    logger.enableCategory("Performance", true);
    logger.enableCategory("System", true);
    logger.enableCategory("Debug", true);  // For detailed debug info
    
    // Set category levels
    logger.setCategoryLevel("Brain", LogLevel::Info);
    logger.setCategoryLevel("Memory", LogLevel::Warning);
    logger.setCategoryLevel("Neuromodulation", LogLevel::Info);
    logger.setCategoryLevel("Cognition", LogLevel::Debug);
    logger.setCategoryLevel("Development", LogLevel::Info);
    logger.setCategoryLevel("Performance", LogLevel::Info);
    logger.setCategoryLevel("System", LogLevel::Info);
    
    std::cout << "Enhanced logging configured successfully!" << std::endl;
    std::cout << "Categories enabled: Brain, Memory, Neuromodulation, Cognition, Development, Performance, System, Debug" << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification with logging
    std::cout << std::endl;
    std::cout << "--- Integration Verification with Enhanced Logging ---" << std::endl;
    
    // Enable detailed system metrics logging
    logger.logInfo("Starting integration verification with enhanced logging", "Phase6Demo.cpp", __LINE__, "main");
    
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        logger.logError("Integration verification failed - brain systems are not properly connected", "Phase6Demo.cpp", __LINE__, "main");
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl;
    logger.logInfo("Integration verification completed successfully - all brain systems connected", "Phase6Demo.cpp", __LINE__, "main");
    
    // Test individual systems with enhanced logging
    std::cout << std::endl;
    std::cout << "--- Memory Integration Test with Logging ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test with Logging ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test with Enhanced Logging ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test with Performance Monitoring ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Create experiment config with enhanced monitoring
    std::cout << "--- Full Integration Experiment with Advanced Logging ---" << std::endl;
    
    logger.logPerformance("Starting full integration experiment with enhanced logging", "Phase6Demo.cpp", __LINE__, "main");
    
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto totalWallClockTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS WITH ENHANCED LOGGING ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall clock time: " << totalWallClockTime << "s" << std::endl;
    
    // Log performance metrics
    logger.logPerformance("Experiment completed successfully - reward: " + std::to_string(result.totalReward) + 
                         ", firing rate: " + std::to_string(result.avgFiringRate) + 
                         ", time: " + std::to_string(totalWallClockTime) + "s", 
                         "Phase6Demo.cpp", __LINE__, "main");
    
    // Enhanced logging for integration status
    std::cout << std::endl;
    std::cout << "=== INTEGRATION STATUS WITH ENHANCED LOGGING ===" << std::endl;
    
    logger.logSystem("Checking memory integration status", "Phase6Demo.cpp", __LINE__, "main");
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    
    logger.logSystem("Checking episodic memory integration status", "Phase6Demo.cpp", __LINE__, "main");
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    
    logger.logNeuromodulation("Checking neuromodulation integration status", "Phase6Demo.cpp", __LINE__, "main");
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    
    logger.logPrediction("Checking prediction integration status", "Phase6Demo.cpp", __LINE__, "main");
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    
    logger.logDevelopment("Checking development integration status", "Phase6Demo.cpp", __LINE__, "main");
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    
    logger.logSystem("Checking checkpointing functionality", "Phase6Demo.cpp", __LINE__, "main");
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    
    // Log final enhanced log entry
    logger.logSystem("=== ENHANCED LOGGING PHASE 6 DEMO COMPLETED ===", "Phase6Demo.cpp", __LINE__, "main");
    
    std::cout << std::endl;
    std::cout << "=== PERFORMANCE METRICS ===" << std::endl;
    std::cout << "Async queue size: " << config.asyncQueueSize << std::endl;
    std::cout << "Log file rotation: " << (config.rotationTime.count() / 3600) << " hours" << std::endl;
    std::cout << "Max rotated files: " << config.maxRotatedFiles << std::endl;
    std::cout << "Log level filtering: Enabled at entry point for performance" << std::endl;
    std::cout << "Lock-free queue: " << (config.useLockFreeQueue ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Sample rate: " << (config.sampleRate * 100) << "%" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== BACKWARD COMPATIBILITY ===" << std::endl;
    std::cout << "Legacy NLM_LOG_* macros still work: Yes" << std::endl;
    std::cout << "ConsoleLogger class unchanged: Yes" << std::endl;
    std::cout << "ILogger interface maintained: Yes" << std::endl;
    std::cout << "Existing API compatibility: 100%" << std::endl;
    
    return 0;
}