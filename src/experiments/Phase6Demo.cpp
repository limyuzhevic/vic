// Phase6Demo - Comprehensive Phase 6 Integration Demonstration
// This demo showcases all Phase 6 integration achievements

#include "Phase6Demo.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include "ExperimentFramework.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <fstream>
#include <iomanip>

namespace nlm {

Phase6Demo::Phase6Demo() {}

Phase6Demo::~Phase6Demo() = default;

void Phase6Demo::runComprehensiveDemo(const DemoConfig& config) {
    NLM_LOG_INFO("=== NLM Phase 6 Integration Demo ===");
    NLM_LOG_INFO("This demo demonstrates the complete integrated brain system");
    NLM_LOG_INFO("Neural Learning Machine - Phase 6: Final Integration");
    NLM_LOG_INFO("");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Create Phase 6 experiment
        Phase6IntegratedExperiment phase6Experiment;
        
        // Configure experiment
        Phase6Config experimentConfig;
        experimentConfig.maxSteps = config.maxSteps;
        experimentConfig.neuronCount = config.neuronCount;
        experimentConfig.regionCount = config.regionCount;
        experimentConfig.connectionProbability = config.connectionProbability;
        experimentConfig.enableCheckpointing = config.enableCheckpointing;
        experimentConfig.enableReplay = config.enableReplay;
        experimentConfig.enableDevelopment = config.enableDevelopment;
        experimentConfig.checkpointPath = config.checkpointPath;
        
        NLM_LOG_INFO("Starting Phase 6 integration experiment...");
        NLM_LOG_INFO("Configuration: " << experimentConfig.neuronCount << " neurons, " 
                    << experimentConfig.maxSteps << " steps");
        
        // Run the comprehensive experiment
        Phase6IntegrationResult result = phase6Experiment.run(experimentConfig);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double wallTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Display results
        printResults(result, wallTime);
        
        // Save detailed results if requested
        if (!config.outputFile.empty()) {
            saveDemoResults(result, wallTime, config.outputFile);
            NLM_LOG_INFO("Demo results saved to: " << config.outputFile);
        }
        
        // Run integration verification
        if (config.runIntegrationVerification) {
            NLM_LOG_INFO("Running integration verification...");
            bool verificationPassed = phase6Experiment.verifyIntegration();
            NLM_LOG_INFO("Integration verification: " 
                        << (verificationPassed ? "PASSED" : "FAILED"));
        }
        
        // Run specific integration tests
        if (config.runMemoryIntegration) {
            NLM_LOG_INFO("Running memory integration test...");
            bool memoryTestPassed = phase6Experiment.testMemoryIntegration();
            NLM_LOG_INFO("Memory integration test: " 
                        << (memoryTestPassed ? "PASSED" : "FAILED"));
        }
        
        if (config.runNeuromodulationTest) {
            NLM_LOG_INFO("Running neuromodulation integration test...");
            bool neuromodTestPassed = phase6Experiment.testNeuromodulationIntegration();
            NLM_LOG_INFO("Neuromodulation test: " 
                        << (neuromTestPassed ? "PASSED" : "FAILED"));
        }
        
        if (config.runCheckpointTest) {
            NLM_LOG_INFO("Running checkpoint test...");
            bool checkpointTestPassed = phase6Experiment.testCheckpointing();
            NLM_LOG_INFO("Checkpoint test: " 
                        << (checkpointTestPassed ? "PASSED" : "FAILED"));
        }
        
        if (config.runReplayTest) {
            NLM_LOG_INFO("Running replay test...");
            bool replayTestPassed = phase6Experiment.testReplay();
            NLM_LOG_INFO("Replay test: " 
                        << (replayTestPassed ? "PASSED" : "FAILED"));
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Demo Complete ===");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Demo failed with error: " << e.what());
        throw;
    }
}

void Phase6Demo::printResults(const Phase6IntegrationResult& result, double wallTime) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Results ===");
    NLM_LOG_INFO("");
    
    // System integration status
    NLM_LOG_INFO("System Integration Status:");
    NLM_LOG_INFO("  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("  Neuromodulation: " << (result.neuromodulationIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("  Prediction System: " << (result.predictionIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("  Development System: " << (result.developmentIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("  Checkpointing: " << (result.checkpointingWorks ? "YES" : "NO"));
    NLM_LOG_INFO("  Replay System: " << (result.replayWorks ? "YES" : "NO"));
    NLM_LOG_INFO("");
    
    // Performance metrics
    NLM_LOG_INFO("Performance Metrics:");
    NLM_LOG_INFO("  Wall Clock Time: " << std::fixed << std::setprecision(2) << wallTime << "s");
    NLM_LOG_INFO("  Total Reward: " << std::fixed << std::setprecision(4) << result.totalReward);
    NLM_LOG_INFO("  Average Firing Rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate);
    NLM_LOG_INFO("  Synaptic Weight Efficiency: " << std::fixed << std::setprecision(4) << result.avgSynapticWeight);
    NLM_LOG_INFO("  Memory Episodes Stored: " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored);
    NLM_LOG_INFO("");
    
    // Neuromodulation levels
    NLM_LOG_INFO("Neuromodulation Levels:");
    NLM_LOG_INFO("  Novelty: " << std::fixed << std::setprecision(4) << result.noveltyLevel);
    NLM_LOG_INFO("  Curiosity: " << std::fixed << std::setprecision(4) << result.curiosityLevel);
    NLM_LOG_INFO("  Dopamine: " << std::fixed << std::setprecision(4) << result.dopamineLevel);
    NLM_LOG_INFO("");
    
    // Success criteria
    bool systemsIntegrated = result.memoryWorkingMemoryIntegrated && 
                             result.memoryEpisodicMemoryIntegrated &&
                             result.neuromodulationIntegrated &&
                             result.predictionIntegrated &&
                             result.developmentIntegrated;
    
    bool functionalityWorking = result.checkpointingWorks && result.replayWorks;
    
    NLM_LOG_INFO("Integration Success Criteria:");
    NLM_LOG_INFO("  Core Systems Integrated: " << (systemsIntegrated ? "YES" : "NO"));
    NLM_LOG_INFO("  Functionality Working: " << (functionalityWorking ? "YES" : "NO"));
    NLM_LOG_INFO("  Overall Status: " << (systemsIntegrated && functionalityWorking ? "SUCCESS" : "NEEDS ATTENTION"));
    NLM_LOG_INFO("");
}

void Phase6Demo::saveDemoResults(const Phase6IntegrationResult& result, double wallTime, 
                                const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }
    
    file << "NLM Phase 6 Integration Demo Results\n";
    file << "===================================\n\n";
    
    file << "Timestamp: " << std::time(nullptr) << "\n";
    file << "Wall Clock Time: " << std::fixed << std::setprecision(2) << wallTime << "s\n\n";
    
    file << "System Integration:\n";
    file << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "YES" : "NO") << "\n";
    file << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "YES" : "NO") << "\n";
    file << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "YES" : "NO") << "\n";
    file << "  Prediction System: " << (result.predictionIntegrated ? "YES" : "NO") << "\n";
    file << "  Development System: " << (result.developmentIntegrated ? "YES" : "NO") << "\n";
    file << "  Checkpointing: " << (result.checkpointingWorks ? "YES" : "NO") << "\n";
    file << "  Replay System: " << (result.replayWorks ? "YES" : "NO") << "\n\n";
    
    file << "Performance Metrics:\n";
    file << "  Total Reward: " << std::fixed << std::setprecision(4) << result.totalReward << "\n";
    file << "  Average Firing Rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate << "\n";
    file << "  Synaptic Weight Efficiency: " << std::fixed << std::setprecision(4) << result.avgSynapticWeight << "\n";
    file << "  Memory Episodes Stored: " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << "\n\n";
    
    file << "Neuromodulation Levels:\n";
    file << "  Novelty: " << std::fixed << std::setprecision(4) << result.noveltyLevel << "\n";
    file << "  Curiosity: " << std::fixed << std::setprecision(4) << result.curiosityLevel << "\n";
    file << "  Dopamine: " << std::fixed << std::setprecision(4) << result.dopamineLevel << "\n\n";
    
    file.close();
}

void Phase6Demo::printHelp() {
    std::cout << "NLM Phase 6 Demo - Neural Learning Machine\n";
    std::cout << "===========================================\n\n";
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]\n\n";
    std::cout << "Demo Options:\n";
    std::cout << "  --help                    Show this help message\n";
    std::cout << "  --version                 Show version information\n";
    std::cout << "  --neurons <count>         Number of neurons (default: 1000)\n";
    std::cout << "  --steps <count>           Number of simulation steps (default: 10000)\n";
    std::cout << "  --output <file>           Save demo results to file\n";
    std::cout << "  --checkpoint <path>       Checkpoint file path\n\n";
    std::cout << "Integration Tests:\n";
    std::cout << "  --integration-verification   Run integration verification\n";
    std::cout << "  --memory-test              Run memory integration test\n";
    std::cout << "  --neuromodulation-test      Run neuromodulation integration test\n";
    std::cout << "  --checkpoint-test          Run checkpoint test\n";
    std::cout << "  --replay-test             Run replay test\n\n";
    std::cout << "All tests are enabled by default.\n";
}

void Phase6Demo::printVersion() {
    std::cout << "NLM Phase 6 Integration Demo\n";
    std::cout << "Version: 0.1.0 (Phase 6: Final Integration)\n";
    std::cout << "Architecture: Neuromorphic Computational Brain\n";
    std::cout << "Phase: Phase 6 - Integrated Artificial Brain\n";
}

int Phase6Demo::run(int argc, char** argv) {
    DemoConfig config;
    
    // Default settings
    config.neuronCount = 1000;
    config.maxSteps = 10000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint.bin";
    
    config.runIntegrationVerification = true;
    config.runMemoryIntegration = true;
    config.runNeuromodulationTest = true;
    config.runCheckpointTest = true;
    config.runReplayTest = true;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return 0;
        }
        else if (arg == "--version") {
            printVersion();
            return 0;
        }
        else if (arg == "--neurons") {
            if (i + 1 < argc) config.neuronCount = std::stoul(argv[++i]);
        }
        else if (arg == "--steps") {
            if (i + 1 < argc) config.maxSteps = std::stoul(argv[++i]);
        }
        else if (arg == "--output") {
            if (i + 1 < argc) config.outputFile = argv[++i];
        }
        else if (arg == "--checkpoint") {
            if (i + 1 < argc) config.checkpointPath = argv[++i];
        }
        else if (arg == "--no-integration-verification") {
            config.runIntegrationVerification = false;
        }
        else if (arg == "--no-memory-test") {
            config.runMemoryIntegration = false;
        }
        else if (arg == "--no-neuromodulation-test") {
            config.runNeuromodulationTest = false;
        }
        else if (arg == "--no-checkpoint-test") {
            config.runCheckpointTest = false;
        }
        else if (arg == "--no-replay-test") {
            config.runReplayTest = false;
        }
    }
    
    try {
        Phase6Demo demo;
        demo.runComprehensiveDemo(config);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

} // namespace nlm
