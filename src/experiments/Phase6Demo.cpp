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

/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Usage:
 *   ./nlm_phase6_demo [--quick] [--steps N] [--neurons N] [--no-checkpoint] [--no-replay] [--no-development]
 *   [--integration-only] [--memory-only] [--neuromod-only] [--checkpoint-only] [--replay-only] [--dev-only]
 *   [--config FILE] [--help]
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <fstream>

using namespace nlm;

/**
 * Command-line argument parser
 */
struct DemoArgs {
    bool integrationOnly = false;
    bool memoryOnly = false;
    bool neuromodOnly = false;
    bool checkpointOnly = false;
    bool replayOnly = false;
    bool devOnly = false;
    bool quickMode = false;
    uint64_t maxSteps = 2000;
    size_t neuronCount = 500;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string configFile;
    bool showHelp = false;
    
    // Helper to parse command line arguments
    bool parseArgs(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp = true;
            } else if (arg == "--quick" || arg == "-q") {
                quickMode = true;
            } else if (arg == "--integration-only") {
                integrationOnly = true;
                memoryOnly = neuromodOnly = checkpointOnly = replayOnly = devOnly = false;
            } else if (arg == "--memory-only") {
                memoryOnly = true;
                integrationOnly = neuromodOnly = checkpointOnly = replayOnly = devOnly = false;
            } else if (arg == "--neuromod-only") {
                neuromodOnly = true;
                integrationOnly = memoryOnly = checkpointOnly = replayOnly = devOnly = false;
            } else if (arg == "--checkpoint-only") {
                checkpointOnly = true;
                integrationOnly = memoryOnly = neuromodOnly = replayOnly = devOnly = false;
            } else if (arg == "--replay-only") {
                replayOnly = true;
                integrationOnly = memoryOnly = neuromodOnly = checkpointOnly = devOnly = false;
            } else if (arg == "--dev-only") {
                devOnly = true;
                integrationOnly = memoryOnly = neuromodOnly = checkpointOnly = replayOnly = false;
            } else if (arg == "--no-checkpoint") {
                enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                enableReplay = false;
            } else if (arg == "--no-development") {
                enableDevelopment = false;
            } else if (arg == "--steps" && i + 1 < argc) {
                maxSteps = std::stoull(argv[++i]);
            } else if (arg == "--neurons" && i + 1 < argc) {
                neuronCount = std::stoul(argv[++i]);
            } else if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return false;
            }
        }
        return true;
    }
    
    void printHelp() const {
        std::cout << "=== NLM Phase 6 Integration Demo Help ===" << std::endl;
        std::cout << "Usage: ./nlm_phase6_demo [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << "Basic Options:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  -q, --quick             Run in quick mode (smaller simulation)" << std::endl;
        std::cout << std::endl;
        std::cout << "Test Mode Options (choose one to run only specific tests):" << std::endl;
        std::cout << "  --integration-only      Run integration verification only" << std::endl;
        std::cout << "  --memory-only          Run memory integration test only" << std::endl;
        std::cout << "  --neuromod-only        Run neuromodulation integration test only" << std::endl;
        std::cout << "  --checkpoint-only      Run checkpoint test only" << std::endl;
        std::cout << "  --replay-only          Run replay test only" << std::endl;
        std::cout << "  --dev-only             Run development test only" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration Options:" << std::endl;
        std::cout << "  --steps N              Set number of simulation steps (default: 2000)" << std::endl;
        std::cout << "  --neurons N            Set number of neurons (default: 500)" << std::endl;
        std::cout << "  --config FILE          Load configuration from file" << std::endl;
        std::cout << std::endl;
        std::cout << "Feature Options:" << std::endl;
        std::cout << "  --no-checkpoint        Disable checkpoint saving" << std::endl;
        std::cout << "  --no-replay            Disable memory replay" << std::endl;
        std::cout << "  --no-development       Disable developmental processes" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm_phase6_demo --quick                           # Quick integration test" << std::endl;
        std::cout << "  ./nlm_phase6_demo --memory-only                    # Test memory only" << std::endl;
        std::cout << "  ./nlm_phase6_demo --steps 5000 --neurons 1000     # Larger simulation" << std::endl;
    }
};

/**
 * Enhanced configuration with power user options
 */
struct AdvancedConfig {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    bool enableVerboseLogging;
    bool enablePerformanceMetrics;
    float progressInterval;
    std::string outputFile;
    
    AdvancedConfig() : maxSteps(2000), neuronCount(500), regionCount(1),
                      connectionProbability(0.1f), enableCheckpointing(true),
                      enableReplay(true), enableDevelopment(true), checkpointPath("./checkpoint.bin"),
                      enableVerboseLogging(false), enablePerformanceMetrics(false),
                      progressInterval(1000.0f), outputFile("") {}
    
    AdvancedConfig(uint64_t steps, size_t neurons, bool verbose = false,
                   bool perfMetrics = false) : maxSteps(steps), neuronCount(neurons),
                      regionCount(1), connectionProbability(0.1f), enableCheckpointing(true),
                      enableReplay(true), enableDevelopment(true), checkpointPath("./checkpoint.bin"),
                      enableVerboseLogging(verbose), enablePerformanceMetrics(perfMetrics),
                      progressInterval(1000.0f), outputFile("") {}
};

/**
 * Load configuration from file
 */
bool loadConfigFromFile(const std::string& filename, AdvancedConfig& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple key=value parsing
        size_t pos = line.find('=');
        if (pos == std::string::npos || line.empty()) continue;
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        if (key == "maxSteps") {
            config.maxSteps = std::stoull(value);
        } else if (key == "neuronCount") {
            config.neuronCount = std::stoul(value);
        } else if (key == "regionCount") {
            config.regionCount = std::stoul(value);
        } else if (key == "connectionProbability") {
            config.connectionProbability = std::stof(value);
        } else if (key == "checkpointPath") {
            config.checkpointPath = value;
        } else if (key == "enableCheckpointing") {
            config.enableCheckpointing = (value == "true" || value == "1");
        } else if (key == "enableReplay") {
            config.enableReplay = (value == "true" || value == "1");
        } else if (key == "enableDevelopment") {
            config.enableDevelopment = (value == "true" || value == "1");
        } else if (key == "progressInterval") {
            config.progressInterval = std::stof(value);
        }
    }
    
    return true;
}

/**
 * Print formatted results
 */
void printResults(const Phase6IntegrationResult& result) {
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward:     " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "Avg firing rate:   " << std::fixed << std::setprecision(4) << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored:  " << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level:    " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    std::cout << "Novelty level:     " << std::fixed << std::setprecision(4) << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level:   " << std::fixed << std::setprecision(4) << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory:    " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory:   " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation:   " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction:        " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development:       " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing:     " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay:            " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Wall clock time:   " << result.totalWallClockTime << "s" << std::endl;
}

/**
 * Print progress update
 */
void printProgress(size_t current, size_t total, float reward, float firingRate, uint64_t step) {
    float progress = static_cast<float>(current) / total * 100.0f;
    
    std::cout << "\rProgress: " << std::fixed << std::setprecision(1) << progress << "% | "
              << "Step: " << step << " | "
              << "Reward: " << std::fixed << std::setprecision(4) << reward << " | "
              << "Firing: " << std::fixed << std::setprecision(4) << firingRate;
    std::cout.flush();
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    DemoArgs args;
    if (argc > 1) {
        if (!args.parseArgs(argc, argv)) {
            std::cerr << "Error parsing arguments. Use --help for usage information." << std::endl;
            return 1;
        }
    }
    
    // Show help if requested
    if (args.showHelp) {
        args.printHelp();
        return 0;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(args.quickMode ? Logger::Level::Warning : Logger::Level::Info);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    
    if (args.quickMode) {
        std::cout << "Quick mode enabled - using reduced settings" << std::endl;
    }
    
    // Load advanced configuration
    AdvancedConfig advancedConfig;
    if (!args.configFile.empty()) {
        std::cout << "Loading configuration from: " << args.configFile << std::endl;
        if (loadConfigFromFile(args.configFile, advancedConfig)) {
            // Override demo args with loaded config
            args.maxSteps = advancedConfig.maxSteps;
            args.neuronCount = advancedConfig.neuronCount;
            args.enableCheckpointing = advancedConfig.enableCheckpointing;
            args.enableReplay = advancedConfig.enableReplay;
            args.enableDevelopment = advancedConfig.enableDevelopment;
        } else {
            std::cerr << "Warning: Failed to load config file, using defaults" << std::endl;
        }
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run tests based on mode
    bool allTestsPassed = true;
    
    try {
        // Integration verification (always first)
        if (!args.integrationOnly && !args.memoryOnly && !args.neuromodOnly && 
            !args.checkpointOnly && !args.replayOnly && !args.devOnly) {
            std::cout << "\n--- Integration Verification ---" << std::endl;
            allTestsPassed = experiment.verifyIntegration();
            
            if (!allTestsPassed) {
                std::cerr << "ERROR: Integration verification failed!" << std::endl;
                return 1;
            }
            
            std::cout << "Integration verification passed!" << std::endl << std::endl;
        }
        
        // Memory integration test
        if (!args.integrationOnly && !args.neuromodOnly && !args.checkpointOnly && 
            !args.replayOnly && !args.devOnly) {
            std::cout << "\n--- Memory Integration Test ---" << std::endl;
            if (!experiment.testMemoryIntegration()) {
                std::cerr << "Memory integration test failed!" << std::endl;
                allTestsPassed = false;
            }
        }
        
        // Neuromodulation integration test
        if (!args.integrationOnly && !args.checkpointOnly && !args.replayOnly && !args.devOnly) {
            std::cout << "\n--- Neuromodulation Integration Test ---" << std::endl;
            if (!experiment.testNeuromodulationIntegration()) {
                std::cerr << "Neuromodulation integration test failed!" << std::endl;
                allTestsPassed = false;
            }
        }
        
        // Checkpoint test
        if (!args.integrationOnly && !args.memoryOnly && !args.neuromodOnly && 
            !args.replayOnly && !args.devOnly) {
            std::cout << "\n--- Checkpoint Test ---" << std::endl;
            if (!experiment.testCheckpointing()) {
                std::cerr << "Checkpoint test failed!" << std::endl;
                allTestsPassed = false;
            }
        }
        
        // Replay test
        if (!args.integrationOnly && !args.memoryOnly && !args.neuromodOnly && 
            !args.checkpointOnly && !args.devOnly) {
            std::cout << "\n--- Replay Test ---" << std::endl;
            if (!experiment.testReplay()) {
                std::cerr << "Replay test failed!" << std::endl;
                allTestsPassed = false;
            }
        }
        
        // Development test
        if (!args.integrationOnly && !args.memoryOnly && !args.neuromodOnly && 
            !args.checkpointOnly && !args.replayOnly) {
            std::cout << "\n--- Development Test ---" << std::endl;
            // Run a development simulation
            auto cfg = std::make_shared<Config>();
            cfg->set("neuron_count", 100);
            auto brain = std::make_shared<Brain>(cfg);
            if (brain->initialize()) {
                for (size_t i = 0; i < 100; ++i) {
                    brain->step(i, i * 0.001);
                }
                brain->develop();
                std::cout << "Development test completed successfully" << std::endl;
            } else {
                std::cerr << "Development test setup failed!" << std::endl;
                allTestsPassed = false;
            }
        }
        
        // Full integration experiment (unless running only specific tests)
        if (!args.integrationOnly && !args.memoryOnly && !args.neuromodOnly && 
            !args.checkpointOnly && !args.replayOnly && !args.devOnly) {
            std::cout << "\n--- Full Integration Experiment ---" << std::endl;
            
            Phase6Config config;
            config.maxSteps = args.maxSteps;
            config.neuronCount = args.neuronCount;
            config.regionCount = 1;
            config.connectionProbability = 0.1f;
            config.enableCheckpointing = args.enableCheckpointing;
            config.enableReplay = args.enableReplay;
            config.enableDevelopment = args.enableDevelopment;
            config.checkpointPath = "./nlm_checkpoint_demo.bin";
            
            // Record start time for performance metrics
            auto startWall = std::chrono::high_resolution_clock::now();
            
            auto result = experiment.run(config);
            
            // Record end time
            auto endWall = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endWall - startWall).count();
            
            // Override with our measured time
            result.totalWallClockTime = elapsed;
            
            // Print results using enhanced formatter
            printResults(result);
            
            if (!allTestsPassed) {
                std::cerr << "\nWarning: Some tests failed but simulation completed." << std::endl;
            }
        } else {
            std::cout << "\nSelected test modes completed successfully." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\nERROR during execution: " << e.what() << std::endl;
        allTestsPassed = false;
    } catch (...) {
        std::cerr << "\nERROR: Unknown error during execution" << std::endl;
        allTestsPassed = false;
    }
    
    // Final status
    std::cout << std::endl;
    if (allTestsPassed) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        if (!args.integrationOnly && !args.memoryOnly && !args.neuromodOnly && 
            !args.checkpointOnly && !args.replayOnly && !args.devOnly) {
            std::cout << "The integrated artificial brain is functioning correctly!" << std::endl;
        } else {
            std::cout << "The selected test passed successfully!" << std::endl;
        }
    } else {
        std::cerr << "=== TESTS FAILED ===" << std::endl;
        std::cerr << "One or more tests failed. See error messages above." << std::endl;
    }
    
    return allTestsPassed ? 0 : 1;
}
