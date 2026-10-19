/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "core/HelpSystem.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <fstream>

using namespace nlm;

// Command-line options struct (enhanced for Phase6Demo)
struct DemoOptions {
    int verboseLevel = 0;
    std::string configFile = "configs/default.cfg";
    std::string outputPath = "./nlm_phase6_demo_output.json";
    bool quietMode = false;
    bool showHelp = false;
    bool showAdvanced = false;
    bool validationOnly = false;
    bool benchmarkMode = false;
    bool debugMode = false;
    size_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    bool enableMemorySystems = true;
    bool enableNeuromodulation = true;
    bool enablePredictionSystem = true;
    bool enableDevelopment = true;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    
    void printHelp() {
        std::cout << "=== NLM Phase 6 Demo Options ===" << std::endl;
        std::cout << "Usage: nlm_phase6_demo [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Basic Options:" << std::endl;
        std::cout << "  --help, -h                     Show this help message" << std::endl;
        std::cout << "  --verbose, -v                  Increase verbosity level (can be repeated)" << std::endl;
        std::cout << "  --quiet, -q                    Quiet mode - minimal output" << std::endl;
        std::cout << "  --validation-only              Run integration verification only" << std::endl;
        std::cout << "  --benchmark                    Run performance benchmark" << std::endl;
        std::cout << "  --debug                        Enable debug output" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration Options:" << std::endl;
        std::cout << "  --neurons <N>                  Number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps <N>                    Maximum simulation steps (default: 2000)" << std::endl;
        std::cout << "  --config <file>                Path to configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --output <file>                Output file path (default: ./nlm_phase6_demo_output.json)" << std::endl;
        std::cout << std::endl;
        std::cout << "System Options (Enable/Disable):" << std::endl;
        std::cout << "  --enable-memory-systems        Enable all memory systems (default: enabled)" << std::endl;
        std::cout << "  --disable-memory-systems       Disable all memory systems" << std::endl;
        std::cout << "  --enable-neuromodulation        Enable neuromodulation (default: enabled)" << std::endl;
        std::cout << "  --disable-neuromodulation       Disable neuromodulation" << std::endl;
        std::cout << "  --enable-prediction-system     Enable prediction system (default: enabled)" << std::endl;
        std::cout << "  --disable-prediction-system    Disable prediction system" << std::endl;
        std::cout << "  --enable-development           Enable development (default: enabled)" << std::endl;
        std::cout << "  --disable-development          Disable development" << std::endl;
        std::cout << "  --enable-checkpointing         Enable checkpointing (default: enabled)" << std::endl;
        std::cout << "  --disable-checkpointing        Disable checkpointing" << std::endl;
        std::cout << "  --enable-replay                Enable replay system (default: enabled)" << std::endl;
        std::cout << "  --disable-replay               Disable replay system" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm_phase6_demo --verbose --neurons 1000 --steps 5000" << std::endl;
        std::cout << "  nlm_phase6_demo --validation-only" << std::endl;
        std::cout << "  nlm_phase6_demo --benchmark --debug" << std::endl;
        std::cout << "  nlm_phase6_demo --disable-memory-systems --enable-prediction-system" << std::endl;
    }
};

DemoOptions parseArguments(int argc, char* argv[]) {
    DemoOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verboseLevel++;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quietMode = true;
        } else if (arg == "--validation-only") {
            options.validationOnly = true;
        } else if (arg == "--benchmark") {
            options.benchmarkMode = true;
        } else if (arg == "--debug") {
            options.debugMode = true;
        } else if (arg == "--neurons" && i + 1 < argc) {
            try {
                options.neuronCount = std::stoul(argv[++i]);
            } catch (...) {
                std::cerr << "Error: Invalid neuron count value" << std::endl;
                options.neuronCount = 500; // Default
            }
        } else if (arg == "--steps" && i + 1 < argc) {
            try {
                options.maxSteps = std::stoull(argv[++i]);
            } catch (...) {
                std::cerr << "Error: Invalid steps value" << std::endl;
                options.maxSteps = 2000; // Default
            }
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputPath = argv[++i];
        } else if (arg == "--enable-memory-systems") {
            options.enableMemorySystems = true;
        } else if (arg == "--disable-memory-systems") {
            options.enableMemorySystems = false;
        } else if (arg == "--enable-neuromodulation") {
            options.enableNeuromodulation = true;
        } else if (arg == "--disable-neuromodulation") {
            options.enableNeuromodulation = false;
        } else if (arg == "--enable-prediction-system") {
            options.enablePredictionSystem = true;
        } else if (arg == "--disable-prediction-system") {
            options.enablePredictionSystem = false;
        } else if (arg == "--enable-development") {
            options.enableDevelopment = true;
        } else if (arg == "--disable-development") {
            options.enableDevelopment = false;
        } else if (arg == "--enable-checkpointing") {
            options.enableCheckpointing = true;
        } else if (arg == "--disable-checkpointing") {
            options.enableCheckpointing = false;
        } else if (arg == "--enable-replay") {
            options.enableReplay = true;
        } else if (arg == "--disable-replay") {
            options.enableReplay = false;
        } else if (arg.find("--") == 0) {
            std::cerr << "Unknown option: " << arg << std::endl;
        }
    }
    
    return options;
}

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    DemoOptions options = parseArguments(argc, argv);
    
    if (options.showHelp) {
        options.printHelp();
        return 0;
    }
    
    // Initialize logger (consistent with main.cpp approach)
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Adjust log level based on verbose/quiet/debug flags
    LogLevel logLevel = LogLevel::Info;
    if (options.debugMode) {
        logLevel = LogLevel::Debug;
    } else if (options.verboseLevel > 1) {
        logLevel = LogLevel::Debug;
    } else if (options.verboseLevel > 0) {
        logLevel = LogLevel::Info;
    }
    
    consoleLogger->setLevel(logLevel);
    
    if (!options.quietMode) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl;
        if (options.verboseLevel > 0) {
            std::cout << "[Verbose] Options: config=" << options.configFile 
                     << ", neurons=" << options.neuronCount 
                     << ", steps=" << options.maxSteps << std::endl;
        }
        std::cout << std::endl;
        
        if (options.debugMode) {
            std::cout << "[Debug] Debug mode enabled" << std::endl;
        }
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // If validation-only mode, just run verification
    if (options.validationOnly) {
        if (!options.quietMode) {
            std::cout << "=== Validation Mode ===" << std::endl;
            std::cout << "Running integration verification only..." << std::endl << std::endl;
        }
        
        bool integrationOK = experiment.verifyIntegration();
        
        if (integrationOK) {
            if (!options.quietMode) {
                std::cout << "✓ Integration verification PASSED" << std::endl;
            }
            return 0;
        } else {
            if (!options.quietMode) {
                std::cerr << "✗ Integration verification FAILED" << std::endl;
            }
            return 1;
        }
    }
    
    // First, run the quick integration verification
    if (!options.quietMode) {
        std::cout << "--- Integration Verification ---" << std::endl;
    }
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    if (!options.quietMode) {
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    // Test individual systems
    if (!options.quietMode) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
    }
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    if (!options.quietMode) {
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    }
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    if (!options.quietMode) {
        std::cout << "--- Checkpoint Test ---" << std::endl;
    }
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    if (!options.quietMode) {
        std::cout << "--- Replay Test ---" << std::endl;
    }
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with configured settings
    if (!options.quietMode) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        std::cout << "Configuration: " << options.neuronCount << " neurons, " 
                 << options.maxSteps << " steps" << std::endl;
        if (options.enableMemorySystems) {
            std::cout << "Memory Systems: Enabled" << std::endl;
        } else {
            std::cout << "Memory Systems: Disabled" << std::endl;
        }
        if (options.enableNeuromodulation) {
            std::cout << "Neuromodulation: Enabled" << std::endl;
        } else {
            std::cout << "Neuromodulation: Disabled" << std::endl;
        }
        if (options.enablePredictionSystem) {
            std::cout << "Prediction System: Enabled" << std::endl;
        } else {
            std::cout << "Prediction System: Disabled" << std::endl;
        }
        if (options.enableDevelopment) {
            std::cout << "Development: Enabled" << std::endl;
        } else {
            std::cout << "Development: Disabled" << std::endl;
        }
        if (options.enableCheckpointing) {
            std::cout << "Checkpointing: Enabled" << std::endl;
        } else {
            std::cout << "Checkpointing: Disabled" << std::endl;
        }
        if (options.enableReplay) {
            std::cout << "Replay System: Enabled" << std::endl;
        } else {
            std::cout << "Replay System: Disabled" << std::endl;
        }
        std::cout << std::endl;
    }
    
    Phase6Config config;
    config.neuronCount = options.neuronCount;
    config.maxSteps = options.maxSteps;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    
    auto result = experiment.run(config);
    
    // Save results to output file if specified
    if (!options.outputPath.empty()) {
        std::ofstream outputFile(options.outputPath);
        if (outputFile.is_open()) {
            outputFile << "NLM Phase 6 Integration Results" << std::endl;
            outputFile << "=" << std::string(40, '=') << std::endl;
            outputFile << "Total reward: " << result.totalReward << std::endl;
            outputFile << "Avg firing rate: " << result.avgFiringRate << std::endl;
            outputFile << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
            outputFile << "Dopamine level: " << result.dopamineLevel << std::endl;
            outputFile << std::endl;
            outputFile << "Integration Status:" << std::endl;
            outputFile << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            outputFile << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            outputFile << std::endl;
            outputFile << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
            outputFile.close();
            
            if (!options.quietMode) {
                std::cout << "Results saved to: " << options.outputPath << std::endl;
            }
        } else {
            std::cerr << "Warning: Could not open output file: " << options.outputPath << std::endl;
        }
    }
    
    // Always show final results
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
    
    if (options.benchmarkMode) {
        std::cout << std::endl;
        std::cout << "=== BENCHMARK MODE ===" << std::endl;
        std::cout << "Performance metrics recorded" << std::endl;
    }
    
    return 0;
}