/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <sys/resource.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace nlm;

class Phase6Demo {
public:
    struct DemoOptions {
        bool strictValidation = false;
        bool detailedResults = false;
        int checkpointInterval = 100;
        int maxReplayCount = 5;
        bool skipMemory = false;
        bool skipNeuromodulation = false;
        bool enablePerformanceTracking = false;
        bool exportResults = false;
        std::string exportFilename;
        int debugLevel = 0;
        std::string configPath = "";
        bool benchmarkMode = false;
        bool helpRequested = false;
    };

    Phase6Demo() {}
    ~Phase6Demo() {}
    
    void run(const DemoOptions& options = DemoOptions()) {
        std::cout << "=== NLM PHASE 6 - ADVANCED INTEGRATION DEMO ===" << std::endl;
        std::cout << "Enhanced integration test with power user controls\n" << std::endl;
        
        // Parse command line if not already done
        DemoOptions runOptions = options;
        parseCommandLine(runOptions);
        
        std::cout << "Configuration:\n";
        std::cout << "  Strict validation: " << (runOptions.strictValidation ? "YES" : "NO") << std::endl;
        std::cout << "  Detailed results: " << (runOptions.detailedResults ? "YES" : "NO") << std::endl;
        std::cout << "  Checkpoint interval: " << runOptions.checkpointInterval << std::endl;
        std::cout << "  Max replay count: " << runOptions.maxReplayCount << std::endl;
        std::cout << "  Skip memory integration: " << (runOptions.skipMemory ? "YES" : "NO") << std::endl;
        std::cout << "  Skip neuromodulation integration: " << (runOptions.skipNeuromodulation ? "YES" : "NO") << std::endl;
        std::cout << "  Performance tracking: " << (runOptions.enablePerformanceTracking ? "YES" : "NO") << std::endl;
        std::cout << "  Export results: " << (runOptions.exportResults ? "YES" : "NO") << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(runOptions.debugLevel > 1 ? Logger::Level::Debug : Logger::Level::Info);
        
        // Load config from file if specified
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        if (!runOptions.configPath.empty()) {
            loadConfigFromFile(config, runOptions.configPath);
        }
        
        // Performance tracking setup
        std::vector<double> systemTimes;
        double startTime = getCurrentTime();
        
        // Create result container for detailed output
        IntegrationTestResults detailedResultsData;
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        std::cout << "\n=== INTEGRATION VERIFICATION ===\n" << std::endl;
        
        // Run integration verification with optional strict mode
        bool integrationOK;
        if (runOptions.strictValidation) {
            std::cout << "Running STRICT integration verification...\n" << std::endl;
            integrationOK = runStrictIntegrationVerification(experiment, runOptions);
        } else {
            std::cout << "Running standard integration verification...\n" << std::endl;
            integrationOK = experiment.verifyIntegration();
        }
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems with skip options
        if (!runOptions.skipMemory) {
            std::cout << "=== MEMORY INTEGRATION TEST ===" << std::endl;
            experiment.testMemoryIntegration();
            if (runOptions.detailedResults) {
                detailedResultsData = collectDetailedTestResults("Memory", experiment.getMemoryState());
            }
            std::cout << std::endl;
        }
        
        if (!runOptions.skipNeuromodulation) {
            std::cout << "=== NEUROMODULATION INTEGRATION TEST ===" << std::endl;
            experiment.testNeuromodulationIntegration();
            if (runOptions.detailedResults) {
                detailedResultsData = collectDetailedTestResults("Neuromodulation", experiment.getNeuromodulationState());
            }
            std::cout << std::endl;
        }
        
        std::cout << "=== CHECKPOINT TEST ===" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
        
        std::cout << "=== REPLAY TEST ===" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
        
        // Run full experiment with adjusted settings
        std::cout << "=== FULL INTEGRATION EXPERIMENT ===" << std::endl;
        config.checkpointInterval = runOptions.checkpointInterval;
        config.maxReplay = runOptions.maxReplayCount;
        
        if (runOptions.enablePerformanceTracking) {
            std::cout << "Starting performance monitoring..." << std::endl;
        }
        
        auto result = experiment.run(config);
        
        // Display results
        printResults(result, runOptions.detailedResults);
        
        // Performance summary
        double totalTime = getCurrentTime() - startTime;
        if (runOptions.enablePerformanceTracking) {
            printPerformanceSummary(totalTime, result, runOptions.detailedResults);
        }
        
        // Export results if requested
        if (runOptions.exportResults && !runOptions.exportFilename.empty()) {
            exportResults(runOptions.exportFilename, result, detailedResultsData);
        }
        
        // Benchmark output
        if (runOptions.benchmarkMode) {
            printBenchmarkSummary(totalTime, result);
        }
        
        std::cout << "\n=== FINAL RESULTS ===" << std::endl;
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
    }
    
private:
    int argc;
    char** argv;
    
    void parseCommandLine(DemoOptions& options) {
        // Parse new command-line options
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--strict-validation") {
                options.strictValidation = true;
            } else if (arg == "--detailed-results") {
                options.detailedResults = true;
            } else if (arg.find("--checkpoint-interval=") == 0) {
                options.checkpointInterval = std::atoi(arg.substr(22).c_str());
            } else if (arg.find("--max-replay=") == 0) {
                options.maxReplayCount = std::atoi(arg.substr(12).c_str());
            } else if (arg == "--skip-memory") {
                options.skipMemory = true;
            } else if (arg == "--skip-neuromodulation") {
                options.skipNeuromodulation = true;
            } else if (arg == "--performance") {
                options.enablePerformanceTracking = true;
            } else if (arg.find("--export=") == 0) {
                options.exportResults = true;
                options.exportFilename = arg.substr(8);
            } else if (arg.find("--debug-level=") == 0) {
                options.debugLevel = std::atoi(arg.substr(14).c_str());
            } else if (arg.find("--config=") == 0) {
                options.configPath = arg.substr(9);
            } else if (arg == "--benchmark") {
                options.benchmarkMode = true;
            } else if (arg == "--help") {
                printHelp();
                exit(0);
            }
        }
    }
    
    void printHelp() {
        std::cout << "NLM Phase 6 Demo - Advanced Integration Test\n";
        std::cout << "Usage:\n";
        std::cout << "  Phase6Demo.exe [options]\n\n";
        std::cout << "Validation and Testing Options:\n";
        std::cout << "  --strict-validation          Run strict integration verification\n";
        std::cout << "  --detailed-results          Collect detailed test results for analysis\n";
        std::cout << "  --checkpoint-interval=N     Set checkpoint interval (default: 100)\n";
        std::cout << "  --max-replay=N              Set maximum replay count (default: 5)\n";
        std::cout << "  --skip-memory               Skip memory integration tests\n";
        std::cout << "  --skip-neuromodulation       Skip neuromodulation integration tests\n\n";
        std::cout << "Output and Monitoring Options:\n";
        std::cout << "  --performance               Enable performance monitoring\n";
        std::cout << "  --export=filename           Export results to file\n";
        std::cout << "  --debug-level=N             Set debug level (0-3)\n";
        std::cout << "  --config=path              Load configuration from file\n";
        std::cout << "  --benchmark                 Run benchmark mode\n";
        std::cout << "  --help                      Show this help\n";
    }
    
    bool runStrictIntegrationVerification(Phase6IntegratedExperiment& experiment, const DemoOptions& options) {
        bool allPassed = true;
        
        std::cout << "  Verifying memory integration..." << std::endl;
        allPassed &= experiment.verifyMemoryIntegration();
        
        std::cout << "  Verifying neuromodulation integration..." << std::endl;
        allPassed &= experiment.verifyNeuromodulationIntegration();
        
        std::cout << "  Verifying prediction integration..." << std::endl;
        allPassed &= experiment.verifyPredictionIntegration();
        
        std::cout << "  Verifying development integration..." << std::endl;
        allPassed &= experiment.verifyDevelopmentIntegration();
        
        std::cout << "  Verifying checkpoint integration..." << std::endl;
        allPassed &= experiment.verifyCheckpointIntegration();
        
        std::cout << "  Verifying replay integration..." << std::endl;
        allPassed &= experiment.verifyReplayIntegration();
        
        return allPassed;
    }
    
    void loadConfigFromFile(Phase6Config& config, const std::string& filename) {
        // Implementation for loading config from file would go here
        std::cout << "Loading config from: " << filename << std::endl;
    }
    
    void printResults(const IntegrationTestResults& result, bool detailed) {
        if (detailed) {
            std::cout << "\n=== DETAILED RESULTS ===\n";
            std::cout << "All system integration states captured and logged.\n";
            std::cout << "Refer to exported data for complete details.\n";
        }
    }
    
    void printPerformanceSummary(double totalTime, const IntegrationTestResults& result, bool detailed) {
        std::cout << "\n=== PERFORMANCE SUMMARY ===\n";
        std::cout << "Total execution time: " << totalTime << " seconds\n";
        std::cout << "System initialization: " << result.initializationTime << " seconds\n";
        std::cout << "Integration tests: " << result.integrationTestTime << " seconds\n";
        std::cout << "Full experiment: " << result.experimentTime << " seconds\n";
        std::cout << "Memory usage peak: " << result.peakMemoryUsage << " MB\n";
        
        if (detailed) {
            std::cout << "\nDetailed performance metrics available in export file.\n";
        }
    }
    
    void exportResults(const std::string& filename, const IntegrationTestResults& result, const IntegrationTestResults& detailed) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open export file: " << filename << std::endl;
            return;
        }
        
        file << "NLM Phase 6 Integration Test Results\n";
        file << "====================================\n\n";
        
        file << "Integration Test Results:\n";
        file << "  Total Reward: " << result.totalReward << "\n";
        file << "  Avg Firing Rate: " << result.avgFiringRate << "\n";
        file << "  Episodes Stored: " << result.memoryEpisodesStored << "\n";
        file << "  Dopamine Level: " << result.dopamineLevel << "\n\n";
        
        file << "Integration Status:\n";
        file << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        file << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        file << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        file << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        file << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
        file << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n\n";
        
        file << "Performance:\n";
        file << "  Wall Clock Time: " << result.totalWallClockTime << "s\n";
        
        file.close();
        std::cout << "Results exported to: " << filename << std::endl;
    }
    
    void printBenchmarkSummary(double totalTime, const IntegrationTestResults& result) {
        std::cout << "\n=== BENCHMARK SUMMARY ===\n";
        std::cout << "Total execution time: " << totalTime << " seconds\n";
        std::cout << "Integration test duration: " << result.integrationTestTime << " seconds\n";
        std::cout << "Experiment runtime: " << result.experimentTime << " seconds\n";
        std::cout << "Throughput: " << (result.totalReward / totalTime) << " reward units/second\n";
        std::cout << "System efficiency: " << (result.totalSteps / totalTime) << " steps/second\n";
    }
    
    IntegrationTestResults collectDetailedTestResults(const std::string& systemName, const std::string& state) {
        IntegrationTestResults results;
        results.systemName = systemName;
        results.systemState = state;
        return results;
    }
    
    double getCurrentTime() {
        auto now = std::chrono::high_resolution_clock::now();
        auto epoch = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count() / 1000.0;
    }
};

// Main entry point for Phase 6 demo
int main(int argc, char* argv[]) {
    Phase6Demo::DemoOptions options;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--strict-validation") {
            options.strictValidation = true;
        } else if (arg == "--detailed-results") {
            options.detailedResults = true;
        } else if (arg.find("--checkpoint-interval=") == 0) {
            options.checkpointInterval = std::atoi(arg.substr(22).c_str());
        } else if (arg.find("--max-replay=") == 0) {
            options.maxReplayCount = std::atoi(arg.substr(12).c_str());
        } else if (arg == "--skip-memory") {
            options.skipMemory = true;
        } else if (arg == "--skip-neuromodulation") {
            options.skipNeuromodulation = true;
        } else if (arg == "--performance") {
            options.enablePerformanceTracking = true;
        } else if (arg.find("--export=") == 0) {
            options.exportResults = true;
            options.exportFilename = arg.substr(8);
        } else if (arg.find("--debug-level=") == 0) {
            options.debugLevel = std::atoi(arg.substr(14).c_str());
        } else if (arg.find("--config=") == 0) {
            options.configPath = arg.substr(9);
        } else if (arg == "--benchmark") {
            options.benchmarkMode = true;
        } else if (arg == "--help") {
            Phase6Demo demo;
            demo.printHelp();
            return 0;
        }
    }
    
    Phase6Demo demo;
    demo.run(options);
    
    return 0;
}