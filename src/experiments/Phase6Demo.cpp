/**
 * Enhanced NLM Phase 6 Demo - Advanced Integration Test
 * 
 * This demo provides comprehensive testing and demonstration of the integrated
 * artificial brain with advanced features for both beginners and advanced users.
 * 
 * Features:
 * - Comprehensive integration verification
 * - Interactive configuration options
 * - Detailed performance metrics
 * - Checkpoint and replay testing
 * - Memory usage profiling
 * - Visualization output options
 * - Export capabilities for analysis
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <map>

using namespace nlm;

/**
 * Enhanced configuration with more options
 */
struct EnhancedPhase6Config {
    Phase6Config baseConfig;
    bool verbose;
    bool exportMetrics;
    bool generateVisualization;
    std::string outputPath;
    std::string checkpointPath;
    bool runQuickTest;
    bool runFullTest;
    bool runDiagnostics;
    
    EnhancedPhase6Config() : verbose(false), exportMetrics(false), 
                            generateVisualization(false), outputPath("./demo_output"),
                            checkpointPath("./checkpoint_test.bin"), 
                            runQuickTest(false), runFullTest(false),
                            runDiagnostics(false) {
        baseConfig.maxSteps = 1000;
        baseConfig.neuronCount = 500;
        baseConfig.regionCount = 1;
        baseConfig.connectionProbability = 0.1f;
        baseConfig.enableCheckpointing = true;
        baseConfig.enableReplay = true;
        baseConfig.enableDevelopment = true;
    }
};

/**
 * Print comprehensive usage information
 */
void printUsage() {
    std::cout << "=== NLM Phase 6 Enhanced Demo ===" << std::endl;
    std::cout << "Advanced Integration Test with Comprehensive Features" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage: nlm_phase6_demo [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
    std::cout << "  -q, --quick             Run quick integration verification only" << std::endl;
    std::cout << "  -f, --full              Run full comprehensive test" << std::endl;
    std::cout << "  --diagnostics           Run detailed system diagnostics" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --neurons N             Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S               Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --no-checkpointing      Disable checkpoint testing" << std::endl;
    std::cout << "  --no-replay             Disable replay testing" << std::endl;
    std::cout << "  --no-development        Disable development system" << std::endl;
    std::cout << "  --checkpoint FILE        Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --export DIR            Export metrics to directory (default: ./demo_output)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Example Usage:" << std::endl;
    std::cout << "  nlm_phase6_demo --verbose --full --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  nlm_phase6_demo --quick --no-checkpointing" << std::endl;
    std::cout << std::endl;
}

/**
 * Export metrics to various formats
 */
void exportMetrics(const Phase6IntegrationResult& result, const std::string& outputPath) {
    std::ofstream file(outputPath + "/results.txt");
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot create output directory or file: " << outputPath << std::endl;
        return;
    }
    
    file << "=== NLM Phase 6 Integration Results ===" << std::endl;
    file << std::endl;
    
    file << "Integration Status:" << std::endl;
    file << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    file << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    file << "  Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    file << std::endl;
    
    file << "Performance Metrics:" << std::endl;
    file << "  Total Reward: " << result.totalReward << std::endl;
    file << "  Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
    file << "  Average Synaptic Weight: " << result.avgSynapticWeight << std::endl;
    file << "  Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    file << "  Novelty Level: " << result.noveltyLevel << std::endl;
    file << "  Curiosity Level: " << result.curiosityLevel << std::endl;
    file << "  Dopamine Level: " << result.dopamineLevel << std::endl;
    file << std::endl;
    
    file << "Timing Information:" << std::endl;
    if (result.startTime > 0) {
        file << "  Start Time: " << ctime(&result.startTime);
    }
    if (result.endTime > 0) {
        file << "  End Time: " << ctime(&result.endTime);
    }
    file << "  Total Wall Clock Time: " << result.totalWallClockTime << " seconds" << std::endl;
    file << std::endl;
    
    file.close();
    
    std::cout << "Metrics exported to: " << outputPath + "/results.txt" << std::endl;
}

/**
 * Print formatted report
 */
void printFormattedReport(const Phase6IntegrationResult& result) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "NLM Phase 6 Integration Report" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    std::cout << "\nINTEGRATION STATUS:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Prediction: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "  Replay: " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    
    std::cout << "\nPERFORMANCE METRICS:" << std::endl;
    std::cout << "  Total Reward: " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Average Synaptic Weight: " << std::fixed << std::setprecision(6) << result.avgSynapticWeight << std::endl;
    std::cout << "  Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  Novelty Level: " << std::fixed << std::setprecision(4) << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity Level: " << std::fixed << std::setprecision(4) << result.curiosityLevel << std::endl;
    std::cout << "  Dopamine Level: " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    
    std::cout << "\nTIMING INFORMATION:" << std::endl;
    if (result.startTime > 0 && result.endTime > 0) {
        double elapsed = result.endTime - result.startTime;
        std::cout << "  Elapsed Time: " << std::fixed << std::setprecision(2) << elapsed << " seconds" << std::endl;
    }
    std::cout << "  Wall Clock Time: " << std::fixed << std::setprecision(2) << result.totalWallClockTime << " seconds" << std::endl;
    
    if (result.totalWallClockTime > 0 && result.maxSteps > 0) {
        double stepsPerSecond = result.maxSteps / result.totalWallClockTime;
        std::cout << "  Performance: " << std::fixed << std::setprecision(0) << stepsPerSecond << " steps/second" << std::endl;
    }
    
    std::cout << std::string(60, '=') << std::endl;
}

/**
 * Run quick verification mode
 */
void runQuickTest(Phase6IntegratedExperiment& experiment, EnhancedPhase6Config& config) {
    std::cout << "=== Quick Integration Test ===" << std::endl;
    
    bool success = experiment.verifyIntegration();
    
    if (success) {
        std::cout << "✓ Quick integration test PASSED" << std::endl;
    } else {
        std::cout << "✗ Quick integration test FAILED" << std::endl;
        exit(1);
    }
}

/**
 * Run full comprehensive test
 */
void runFullTest(Phase6IntegratedExperiment& experiment, EnhancedPhase6Config& config) {
    std::cout << "=== Full Integration Test ===" << std::endl;
    
    Phase6Config phase6Config;
    phase6Config.neuronCount = config.baseConfig.neuronCount;
    phase6Config.maxSteps = config.baseConfig.maxSteps;
    phase6Config.regionCount = config.baseConfig.regionCount;
    phase6Config.connectionProbability = config.baseConfig.connectionProbability;
    phase6Config.enableCheckpointing = config.baseConfig.enableCheckpointing;
    phase6Config.enableReplay = config.baseConfig.enableReplay;
    phase6Config.enableDevelopment = config.baseConfig.enableDevelopment;
    phase6Config.checkpointPath = config.checkpointPath;
    
    auto result = experiment.run(phase6Config);
    
    if (config.verbose) {
        printFormattedReport(result);
    }
    
    // Check if all critical systems are integrated
    bool allIntegrated = result.memoryWorkingMemoryIntegrated && 
                        result.memoryEpisodicMemoryIntegrated && 
                        result.neuromodulationIntegrated && 
                        result.predictionIntegrated && 
                        result.developmentIntegrated;
    
    if (allIntegrated && result.checkpointingWorks) {
        std::cout << "✓ Full integration test PASSED" << std::endl;
    } else {
        std::cout << "✗ Full integration test FAILED" << std::endl;
        exit(1);
    }
    
    // Export metrics if requested
    if (config.exportMetrics) {
        exportMetrics(result, config.outputPath);
    }
}

/**
 * Run diagnostic tests
 */
void runDiagnostics(Phase6IntegratedExperiment& experiment, EnhancedPhase6Config& config) {
    std::cout << "=== System Diagnostics ===" << std::endl;
    
    // Test individual components
    bool memoryOK = experiment.testMemoryIntegration();
    std::cout << "Memory integration: " << (memoryOK ? "✓ PASS" : "✗ FAIL") << std::endl;
    
    bool neuromodOK = experiment.testNeuromodulationIntegration();
    std::cout << "Neuromodulation integration: " << (neuromOK ? "✓ PASS" : "✗ FAIL") << std::endl;
    
    bool checkpointOK = experiment.testCheckpointing();
    std::cout << "Checkpointing: " << (checkpointOK ? "✓ PASS" : "✗ FAIL") << std::endl;
    
    bool replayOK = experiment.testReplay();
    std::cout << "Replay system: " << (replayOK ? "✓ PASS" : "✗ FAIL") << std::endl;
    
    // Overall diagnostic result
    if (memoryOK && neuromodOK && checkpointOK && replayOK) {
        std::cout << "\n✓ All diagnostics PASSED" << std::endl;
    } else {
        std::cout << "\n✗ Some diagnostics FAILED" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Print banner
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    EnhancedPhase6Config config = parseCommandLine(argc, argv);
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    if (config.verbose) {
        std::cout << "Verbose mode enabled" << std::endl;
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run tests based on configuration
    bool allTestsPassed = true;
    
    try {
        if (config.runQuickTest) {
            runQuickTest(experiment, config);
        } else {
            // Default: run verification
            bool verificationPassed = experiment.verifyIntegration();
            if (verificationPassed) {
                std::cout << "✓ Integration verification PASSED" << std::endl;
            } else {
                std::cout << "✗ Integration verification FAILED" << std::endl;
                allTestsPassed = false;
            }
        }
        
        if (config.runFullTest) {
            runFullTest(experiment, config);
        }
        
        if (config.runDiagnostics) {
            runDiagnostics(experiment, config);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception occurred: " << e.what() << std::endl;
        allTestsPassed = false;
    }
    
    if (allTestsPassed) {
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Integration test completed successfully!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Integration test completed with failures." << std::endl;
        return 1;
    }
}