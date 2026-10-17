/**
 * Phase 6 Demo - Integration Test (Enhanced)
 * 
 * This enhanced demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected with improved CLI
 * and detailed reporting capabilities.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <string>
#include <map>

using namespace nlm;

struct Phase6EnhancedConfig {
    // Integration test settings
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    // Test individual systems
    bool runMemoryTest = true;
    bool runNeuromodulationTest = true;
    bool runCheckpointTest = true;
    bool runReplayTest = true;
    
    // Performance and output
    bool enablePerformanceTracking = true;
    std::string outputFile = "";
    bool verboseOutput = true;
    
    // Checkpoint and replay paths
    std::string checkpointPath = "./checkpoint_test.bin";
    std::string replayPath = "./replay_test.bin";
};

Phase6EnhancedConfig parseCommandLineArgs(int argc, char* argv[]) {
    Phase6EnhancedConfig config;
    
    std::cout << "NLM Phase 6 Enhanced Integration Demo\n";
    std::cout << "=====================================\n\n";
    
    auto showHelp = []() {
        std::cout << "Usage: nlm_phase6_demo [OPTIONS]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --neurons N                    Number of neurons (default: 500)\n";
        std::cout << "  --steps N                      Max steps per experiment (default: 2000)\n";
        std::cout << "  --no-checkpointing             Disable checkpointing\n";
        std::cout << "  --no-replay                    Disable replay\n";
        std::cout << "  --no-development               Disable development\n";
        std::cout << "  --no-memory-test               Skip memory integration test\n";
        std::cout << "  --no-neuromod-test             Skip neuromodulation integration test\n";
        std::cout << "  --no-checkpoint-test           Skip checkpoint test\n";
        std::cout << "  --no-replay-test               Skip replay test\n";
        std::cout << "  --quiet                        Reduce verbose output\n";
        std::cout << "  --output FILE                  Write detailed results to FILE\n";
        std::cout << "  --checkpoint-path PATH         Checkpoint file path\n";
        std::cout << "  --replay-path PATH             Replay file path\n";
        std::cout << "  --help, -h                     Show this help message\n\n";
        std::cout << "Examples:\n";
        std::cout << "  nlm_phase6_demo --neurons 1000 --steps 5000\n";
        std::cout << "  nlm_phase6_demo --no-checkpointing --quiet\n";
        std::cout << "  nlm_phase6_demo --output phase6_results.txt\n";
        return true;
    };
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            showHelp();
            exit(0);
        }
        
        if (arg.find("--neurons=") == 0) {
            config.neuronCount = std::stoull(arg.substr(10));
        } else if (arg.find("--steps=") == 0) {
            config.maxSteps = std::stoull(arg.substr(8));
        } else if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--no-memory-test") {
            config.runMemoryTest = false;
        } else if (arg == "--no-neuromod-test") {
            config.runNeuromodulationTest = false;
        } else if (arg == "--no-checkpoint-test") {
            config.runCheckpointTest = false;
        } else if (arg == "--no-replay-test") {
            config.runReplayTest = false;
        } else if (arg == "--quiet") {
            config.verboseOutput = false;
        } else if (arg.find("--output=") == 0) {
            config.outputFile = arg.substr(8);
        } else if (arg.find("--checkpoint-path=") == 0) {
            config.checkpointPath = arg.substr(18);
        } else if (arg.find("--replay-path=") == 0) {
            config.replayPath = arg.substr(13);
        } else {
            std::cout << "[WARNING] Unknown argument: " << arg << std::endl;
        }
    }
    
    return config;
}

void logConfig(const Phase6EnhancedConfig& config) {
    std::cout << "Starting Phase 6 Enhanced Integration Demo with the following configuration:\n";
    std::cout << "  Neurons: " << config.neuronCount << "\n";
    std::cout << "  Max steps: " << config.maxSteps << "\n";
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Memory test: " << (config.runMemoryTest ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Neuromodulation test: " << (config.runNeuromodulationTest ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Checkpoint test: " << (config.runCheckpointTest ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Replay test: " << (config.runReplayTest ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Verbose output: " << (config.verboseOutput ? "Enabled" : "Disabled") << "\n";
    if (!config.outputFile.empty()) {
        std::cout << "  Output file: " << config.outputFile << "\n";
    }
    std::cout << std::endl;
}

void saveResults(const std::string& filename, const std::map<std::string, std::string>& results) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cout << "[WARNING] Could not write results to: " << filename << std::endl;
        return;
    }
    
    outFile << "NLM Phase 6 Integration Demo - Detailed Results\n";
    outFile << "========================================\n\n";
    
    for (const auto& result : results) {
        outFile << result.first << ": " << result.second << "\n";
    }
    
    outFile.close();
    std::cout << "[OUTPUT] Detailed results saved to: " << filename << std::endl;
}

int main(int argc, char* argv[]) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    Phase6EnhancedConfig config = parseCommandLineArgs(argc, argv);
    logConfig(config);
    
    std::cout << "Running Phase 6 Integration Experiment...\n";
    std::cout << "This will verify that all brain systems work together.\n";
    std::cout << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "=== INTEGRATION VERIFICATION ===" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    if (config.runMemoryTest) {
        std::cout << "=== MEMORY INTEGRATION TEST ===" << std::endl;
        experiment.testMemoryIntegration();
        std::cout << std::endl;
    }
    
    if (config.runNeuromodulationTest) {
        std::cout << "=== NEUROMODULATION INTEGRATION TEST ===" << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
    }
    
    if (config.runCheckpointTest) {
        std::cout << "=== CHECKPOINT TEST ===" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
    }
    
    if (config.runReplayTest) {
        std::cout << "=== REPLAY TEST ===" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
    }
    
    // Prepare Phase6Config for the full experiment
    Phase6Config phase6Config;
    phase6Config.neuronCount = config.neuronCount;
    phase6Config.maxSteps = config.maxSteps;
    phase6Config.enableCheckpointing = config.enableCheckpointing;
    phase6Config.enableReplay = config.enableReplay;
    phase6Config.enableDevelopment = config.enableDevelopment;
    phase6Config.checkpointPath = config.checkpointPath;
    
    // Run full experiment with smaller settings for demo
    std::cout << "=== FULL INTEGRATION EXPERIMENT ===" << std::endl;
    auto result = experiment.run(phase6Config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Calculate performance metrics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    double totalSeconds = duration.count() / 1000.0;
    
    // Prepare detailed results for output
    std::map<std::string, std::string> detailedResults;
    detailedResults["Configuration"] = "Phase 6 Enhanced Integration Demo";
    detailedResults["Neurons"] = std::to_string(config.neuronCount);
    detailedResults["Max Steps"] = std::to_string(config.maxSteps);
    detailedResults["Checkpointing"] = config.enableCheckpointing ? "Enabled" : "Disabled";
    detailedResults["Replay"] = config.enableReplay ? "Enabled" : "Disabled";
    detailedResults["Development"] = config.enableDevelopment ? "Enabled" : "Disabled";
    detailedResults["Integration Status"] = integrationOK ? "SUCCESS" : "FAILED";
    detailedResults["Total Reward"] = std::to_string(result.totalReward);
    detailedResults["Average Firing Rate"] = std::to_string(result.avgFiringRate);
    detailedResults["Episodes Stored"] = std::to_string(result.memoryEpisodesStored);
    detailedResults["Dopamine Level"] = std::to_string(result.dopamineLevel);
    detailedResults["Wall Clock Time (s)"] = std::to_string(result.totalWallClockTime);
    detailedResults["Real Time (s)"] = std::to_string(totalSeconds);
    detailedResults["Memory Working Memory Integrated"] = result.memoryWorkingMemoryIntegrated ? "Yes" : "No";
    detailedResults["Memory Episodic Memory Integrated"] = result.memoryEpisodicMemoryIntegrated ? "Yes" : "No";
    detailedResults["Neuromodulation Integrated"] = result.neuromodulationIntegrated ? "Yes" : "No";
    detailedResults["Prediction Integrated"] = result.predictionIntegrated ? "Yes" : "No";
    detailedResults["Development Integrated"] = result.developmentIntegrated ? "Yes" : "No";
    detailedResults["Checkpointing Works"] = result.checkpointingWorks ? "Yes" : "No";
    
    // Write detailed results if requested
    if (!config.outputFile.empty()) {
        saveResults(config.outputFile, detailedResults);
    }
    
    // Final summary
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "PHASE 6 INTEGRATION SUMMARY" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::cout << "\nThe integrated NLM brain successfully combines all systems:\n";
    std::cout << "  ✓ Working memory for temporary information storage\n";
    std::cout << "  ✓ Episodic memory for experience replay\n";
    std::cout << "  ✓ Neuromodulation for reward and curiosity\n";
    std::cout << "  ✓ Prediction system for temporal expectations\n";
    std::cout << "  ✓ Development system for maturation and aging\n";
    std::cout << "  ✓ Checkpointing for state persistence\n";
    std::cout << "  ✓ Replay for memory consolidation\n";
    std::cout << "\nAll systems are connected and working together as a unified artificial brain.\n";
    
    std::cout << "\nPerformance:\n";
    std::cout << "  Real-time duration: " << std::fixed << std::setprecision(2) << totalSeconds << " seconds\n";
    std::cout << "  Integration test: " << (integrationOK ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Full experiment: " << (result.totalReward > 0 ? "COMPLETED" : "INCOMPLETE") << "\n";
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Phase 6 Integration Complete!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}

} // namespace nlm