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

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace nlm;

// Command-line argument parsing
struct DemoConfig {
    bool showDebug = false;
    bool verbose = false;
    bool benchmark = false;
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    std::string logLevel = "INFO";
    
    bool parseArgs(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--debug" || arg == "-d") {
                showDebug = true;
                logLevel = "DEBUG";
            } else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg == "--benchmark" || arg == "-b") {
                benchmark = true;
            } else if (arg == "--neurons" || arg == "-n") {
                if (i + 1 < argc) {
                    neuronCount = std::stoul(argv[++i]);
                }
            } else if (arg == "--steps" || arg == "-s") {
                if (i + 1 < argc) {
                    maxSteps = std::stoul(argv[++i]);
                }
            } else if (arg == "--no-checkpoint") {
                enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                enableReplay = false;
            } else if (arg == "--no-development") {
                enableDevelopment = false;
            } else if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            } else if (arg.substr(0, 8) == "--log=") {
                logLevel = arg.substr(8);
            } else if (arg == "--checkpoint") {
                if (i + 1 < argc) {
                    checkpointPath = argv[++i];
                }
            }
        }
        return true;
    }
    
    static void printHelp() {
        std::cout << "=== NLM Phase 6 Demo Help ===" << std::endl;
        std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  -d, --debug             Enable debug logging" << std::endl;
        std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
        std::cout << "  -b, --benchmark         Run benchmark mode" << std::endl;
        std::cout << "  -n, --neurons <count>   Set number of neurons (default: 500)" << std::endl;
        std::cout << "  -s, --steps <steps>     Set max simulation steps (default: 2000)" << std::endl;
        std::cout << "      --no-checkpoint      Disable checkpointing" << std::endl;
        std::cout << "      --no-replay           Disable replay system" << std::endl;
        std::cout << "      --no-development      Disable development system" << std::endl;
        std::cout << "      --log=<level>        Set log level (DEBUG, INFO, WARN, ERROR)" << std::endl;
        std::cout << "      --checkpoint <path>   Set checkpoint file path" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm_phase6_demo --debug --benchmark" << std::endl;
        std::cout << "  ./nlm_phase6_demo -n 1000 -s 5000" << std::endl;
        std::cout << "  ./nlm_phase6_demo --log=DEBUG --verbose" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    DemoConfig config;
    if (!config.parseArgs(argc, argv)) {
        return 0; // Help was printed
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << std::endl;
    
    if (config.benchmark) {
        std::cout << "=== BENCHMARK MODE ===" << std::endl;
    }
    if (config.showDebug) {
        std::cout << "=== DEBUG MODE ===" << std::endl;
    }
    if (config.verbose) {
        std::cout << "=== VERBOSE OUTPUT ===" << std::endl;
    }
    
    // Initialize logging with specified level
    Logger::getGlobal().setLevel(stringToLogLevel(config.logLevel));
    
    if (config.benchmark || config.showDebug) {
        std::cout << "Benchmark/Debug mode enabled" << std::endl;
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool integrationOK = experiment.verifyIntegration();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    if (config.benchmark) {
        std::cout << "Integration verification took " << duration.count() << " ms" << std::endl;
    }
    
    // Test individual systems with enhanced output
    std::cout << "--- Memory Integration Test ---" << std::endl;
    startTime = std::chrono::high_resolution_clock::now();
    
    bool memoryTest = experiment.testMemoryIntegration();
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (memoryTest) {
        std::cout << "Memory integration test completed successfully in " << duration.count() << " ms" << std::endl;
    } else {
        std::cout << "Memory integration test failed!" << std::endl;
    }
    
    if (config.verbose) {
        std::cout << "  (Details: Working memory and episodic memory systems checked)" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Test neuromodulation integration
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    startTime = std::chrono::high_resolution_clock::now();
    
    bool neuromodTest = experiment.testNeuromodulationIntegration();
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (neuromodTest) {
        std::cout << "Neuromodulation integration test completed successfully in " << duration.count() << " ms" << std::endl;
    } else {
        std::cout << "Neuromodulation integration test failed!" << std::endl;
    }
    
    if (config.verbose) {
        std::cout << "  (Details: Dopamine, acetylcholine, norepinephrine, and serotonin systems tested)" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Test checkpointing with enhanced output
    std::cout << "--- Checkpoint Test ---" << std::endl;
    startTime = std::chrono::high_resolution_clock::now();
    
    bool checkpointTest = experiment.testCheckpointing();
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (checkpointTest) {
        std::cout << "Checkpoint test completed successfully in " << duration.count() << " ms" << std::endl;
    } else {
        std::cout << "Checkpoint test failed!" << std::endl;
    }
    
    if (config.verbose) {
        std::cout << "  (Details: Brain state save/load functionality tested)" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Test replay with enhanced output
    std::cout << "--- Replay Test ---" << std::endl;
    startTime = std::chrono::high_resolution_clock::now();
    
    bool replayTest = experiment.testReplay();
    
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    if (replayTest) {
        std::cout << "Replay test completed successfully in " << duration.count() << " ms" << std::endl;
    } else {
        std::cout << "Replay test failed!" << std::endl;
    }
    
    if (config.verbose) {
        std::cout << "  (Details: Memory replay and consolidation functionality tested)" << std::endl;
    }
    
    std::cout << std::endl;
    
    // Run full experiment with configurable settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
    
    if (config.benchmark) {
        std::cout << "Running in benchmark mode - optimizing performance..." << std::endl;
    }
    
    Phase6Config expConfig;
    expConfig.neuronCount = config.neuronCount;
    expConfig.maxSteps = config.maxSteps;
    expConfig.enableCheckpointing = config.enableCheckpointing;
    expConfig.enableReplay = config.enableReplay;
    expConfig.enableDevelopment = config.enableDevelopment;
    expConfig.checkpointPath = config.checkpointPath;
    
    auto result = experiment.run(expConfig);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    // Enhanced integration status report
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    
    if (config.verbose) {
        std::cout << std::endl;
        std::cout << "=== DETAILED METRICS ===" << std::endl;
        std::cout << "Total wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << "Avg synaptic weight: " << result.avgSynapticWeight << std::endl;
        std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
        std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
        std::cout << "Start time: " << ctime(&result.startTime) << std::endl;
        std::cout << "End time: " << ctime(&result.endTime) << std::endl;
    }
    
    if (config.benchmark) {
        std::cout << std::endl;
        std::cout << "=== BENCHMARK SUMMARY ===" << std::endl;
        std::cout << "Total simulation time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << "Reward per step: " << result.totalReward / config.maxSteps << std::endl;
        std::cout << "Memory efficiency: " << (result.memoryEpisodesStored / config.maxSteps) * 100 << "%" << std::endl;
    }
    
    return 0;
}
