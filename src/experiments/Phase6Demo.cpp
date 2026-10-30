/**
 * Phase 6 Demo - Integration Test
 * 
 * Enhanced Phase 6 demo with comprehensive command-line interface
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Features:
 * - Integration verification
 * - Memory system testing
 * - Neuromodulation testing
 * - Checkpoint save/load testing
 * - Replay system testing
 * - Configurable via command line
 * - Verbose output options
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>

using namespace nlm;

/**
 * Print help information
 */
void printHelp(const std::string& programName) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain." << std::endl << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                 Show this help message" << std::endl;
    std::cout << "  --verbose, -v              Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                Disable most output (quiet mode)" << std::endl;
    std::cout << "  --no-integration           Skip integration verification" << std::endl;
    std::cout << "  --no-memory-test           Skip memory integration test" << std::endl;
    std::cout << "  --no-neuromod-test         Skip neuromodulation integration test" << std::endl;
    std::cout << "  --no-checkpoint-test       Skip checkpoint save/load test" << std::endl;
    std::cout << "  --no-replay-test           Skip replay system test" << std::endl;
    std::cout << "  --full-run                 Run full integration experiment (slower)" << std::endl;
    std::cout << "  --steps N                  Number of steps for full run (default: 2000)" << std::endl;
    std::cout << "  --neurons N                Number of neurons for full run (default: 500)" << std::endl;
    std::cout << "  --config FILE              Load configuration from file" << std::endl;
    std::cout << "  --seed N                   Random seed for reproducible results (default: 42)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --verbose                    Run with verbose output" << std::endl;
    std::cout << "  " << programName << " --quiet --no-integration     Skip integration, run quietly" << std::endl;
    std::cout << "  " << programName << " --steps 5000 --full-run      Run longer full experiment" << std::endl;
}

/**
 * Parse command line arguments
 */
struct DemoConfig {
    bool help = false;
    bool verbose = false;
    bool quiet = false;
    bool integrationTest = true;
    bool memoryTest = true;
    bool neuromodTest = true;
    bool checkpointTest = true;
    bool replayTest = true;
    bool fullRun = false;
    size_t steps = 2000;
    size_t neurons = 500;
    std::string configFile;
    uint64_t randomSeed = 42;
};

DemoConfig parseArgs(int argc, char* argv[]) {
    DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            config.help = true;
        } else if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            config.quiet = true;
        } else if (arg == "--no-integration") {
            config.integrationTest = false;
        } else if (arg == "--no-memory-test") {
            config.memoryTest = false;
        } else if (arg == "--no-neuromod-test") {
            config.neuromodTest = false;
        } else if (arg == "--no-checkpoint-test") {
            config.checkpointTest = false;
        } else if (arg == "--no-replay-test") {
            config.replayTest = false;
        } else if (arg == "--full-run") {
            config.fullRun = true;
        } else if (arg == "--steps" && i + 1 < argc) {
            config.steps = std::stoull(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neurons = std::stoull(argv[++i]);
        } else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        } else if (arg == "--seed" && i + 1 < argc) {
            config.randomSeed = std::stoull(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            exit(1);
        }
    }
    
    // Override verbose/quiet if both specified
    if (config.verbose && config.quiet) {
        config.verbose = false;
        config.quiet = false;
        std::cerr << "Warning: Both verbose and quiet specified, using normal output." << std::endl;
    }
    
    // If help requested, just print help and exit
    if (config.help) {
        printHelp(argv[0]);
        exit(0);
    }
    
    return config;
}

/**
 * Print status message
 */
void printStatus(const std::string& message, bool verbose = false, bool quiet = false) {
    if (quiet) return;
    
    if (verbose) {
        std::cout << "[VERBOSE] " << message << std::endl;
    } else {
        std::cout << message << std::endl;
    }
}

/**
 * Print section header
 */
void printSection(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

/**
 * Print result
 */
void printResult(const std::string& test, bool passed, const std::string& message = "") {
    std::string status = passed ? "✓ PASS" : "✗ FAIL";
    std::cout << "  " << status << " | " << test;
    if (!message.empty()) {
        std::cout << " - " << message;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    DemoConfig config = parseArgs(argc, argv);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain." << std::endl << std::endl;
    
    printStatus("Configuration:", config.verbose);
    std::cout << "  - Integration test: " << (config.integrationTest ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Memory test: " << (config.memoryTest ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Neuromodulation test: " << (config.neuromodTest ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Checkpoint test: " << (config.checkpointTest ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Replay test: " << (config.replayTest ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Full run: " << (config.fullRun ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Steps: " << config.steps << std::endl;
    std::cout << "  - Neurons: " << config.neurons << std::endl;
    std::cout << "  - Random seed: " << config.randomSeed << std::endl;
    if (!config.configFile.empty()) {
        std::cout << "  - Config file: " << config.configFile << std::endl;
    }
    std::cout << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    int passedTests = 0;
    int totalTests = 0;
    
    // First, run the quick integration verification (if enabled)
    if (config.integrationTest) {
        printSection("Integration Verification");
        totalTests++;
        bool integrationOK = experiment.verifyIntegration();
        if (integrationOK) {
            printResult("Integration verification", true, "All brain systems properly integrated");
            passedTests++;
        } else {
            printResult("Integration verification", false, "Some brain systems not integrated");
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
    }
    
    // Test individual systems (if enabled)
    if (config.memoryTest) {
        printSection("Memory Integration Test");
        totalTests++;
        bool memoryOK = experiment.testMemoryIntegration();
        if (memoryOK) {
            printResult("Memory integration test", true, "Memory systems functional");
            passedTests++;
        } else {
            printResult("Memory integration test", false, "Memory systems not functional");
        }
    }
    
    if (config.neuromodTest) {
        printSection("Neuromodulation Integration Test");
        totalTests++;
        bool neuromodOK = experiment.testNeuromodulationIntegration();
        if (neuromodOK) {
            printResult("Neuromodulation integration test", true, "Neuromodulation systems functional");
            passedTests++;
        } else {
            printResult("Neuromodulation integration test", false, "Neuromodulation systems not functional");
        }
    }
    
    if (config.checkpointTest) {
        printSection("Checkpoint Save/Load Test");
        totalTests++;
        bool checkpointOK = experiment.testCheckpointing();
        if (checkpointOK) {
            printResult("Checkpoint save/load test", true, "Checkpoint system working");
            passedTests++;
        } else {
            printResult("Checkpoint save/load test", false, "Checkpoint system not working");
        }
    }
    
    if (config.replayTest) {
        printSection("Replay System Test");
        totalTests++;
        bool replayOK = experiment.testReplay();
        if (replayOK) {
            printResult("Replay system test", true, "Replay system functional");
            passedTests++;
        } else {
            printResult("Replay system test", false, "Replay system not functional");
        }
    }
    
    // Run full integration experiment (if enabled)
    if (config.fullRun) {
        printSection("Full Integration Experiment");
        totalTests++;
        
        Phase6Config expConfig;
        expConfig.neuronCount = config.neurons;
        expConfig.maxSteps = config.steps;
        expConfig.enableCheckpointing = true;
        expConfig.enableReplay = true;
        expConfig.enableDevelopment = true;
        
        auto result = experiment.run(expConfig);
        
        std::cout << "\nFull Experiment Results:" << std::endl;
        std::cout << "  - Total reward: " << result.totalReward << std::endl;
        std::cout << "  - Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "  - Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  - Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "  - Wall clock time: " << result.totalWallClockTime << " s" << std::endl;
        
        // Check integration status
        bool experimentSuccess = true;
        if (!result.memoryWorkingMemoryIntegrated) {
            std::cout << "  ✗ Working Memory: DISCONNECTED" << std::endl;
            experimentSuccess = false;
        } else {
            std::cout << "  ✓ Working Memory: CONNECTED" << std::endl;
            passedTests++;
        }
        
        if (!result.memoryEpisodicMemoryIntegrated) {
            std::cout << "  ✗ Episodic Memory: DISCONNECTED" << std::endl;
            experimentSuccess = false;
        } else {
            std::cout << "  ✓ Episodic Memory: CONNECTED" << std::endl;
            passedTests++;
        }
        
        if (!result.neuromodulationIntegrated) {
            std::cout << "  ✗ Neuromodulation: DISCONNECTED" << std::endl;
            experimentSuccess = false;
        } else {
            std::cout << "  ✓ Neuromodulation: CONNECTED" << std::endl;
            passedTests++;
        }
        
        if (!result.predictionIntegrated) {
            std::cout << "  ✗ Prediction: DISCONNECTED" << std::endl;
            experimentSuccess = false;
        } else {
            std::cout << "  ✓ Prediction: CONNECTED" << std::endl;
            passedTests++;
        }
        
        if (!result.developmentIntegrated) {
            std::cout << "  ✗ Development: DISCONNECTED" << std::endl;
            experimentSuccess = false;
        } else {
            std::cout << "  ✓ Development: CONNECTED" << std::endl;
            passedTests++;
        }
        
        if (result.checkpointingWorks) {
            std::cout << "  ✓ Checkpointing: WORKING" << std::endl;
            passedTests++;
        } else {
            std::cout << "  ✗ Checkpointing: NOT WORKING" << std::endl;
        }
        
        if (experimentSuccess) {
            std::cout << "\n✓ Full integration experiment completed successfully!" << std::endl;
        } else {
            std::cout << "\n✗ Full integration experiment failed - some systems not connected." << std::endl;
        }
    }
    
    // Print summary
    printSection("SUMMARY");
    std::cout << "Tests completed: " << passedTests << "/" << totalTests << std::endl;
    
    if (totalTests > 0) {
        float successRate = (static_cast<float>(passedTests) / static_cast<float>(totalTests)) * 100.0f;
        std::cout << "Success rate: " << successRate << "%" << std::endl;
        
        if (passedTests == totalTests) {
            std::cout << "\n✓ ALL TESTS PASSED! The integrated brain is functioning correctly." << std::endl;
            return 0;
        } else {
            std::cout << "\n✗ Some tests failed. The integrated brain may have issues." << std::endl;
            return 1;
        }
    }
    
    return 0;
}
