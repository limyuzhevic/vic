/**
 * Phase 6 Demo - Integration Test
 *
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 *
 * Usage:
 *   nlm_phase6_demo [options]
 *
 * Options:
 *   --help, -h              Show this help message
 *   --quick                 Run only integration verification (no full experiment)
 *   --skip-verification     Skip integration verification
 *   --neuron-count N        Number of neurons (default: 500)
 *   --max-steps S           Maximum simulation steps (default: 2000)
 *   --region-count R        Number of neural regions (default: 1)
 *   --connection-prob P     Connection probability (default: 0.1)
 *   --checkpoint-path P      Checkpoint file path (default: ./checkpoint_test.bin)
 *   --enable-checkpointing  Enable checkpointing (default: true)
 *   --enable-replay         Enable replay system (default: true)
 *   --enable-development    Enable development system (default: true)
 *   --verbose, -v           Verbose output
 *   --quiet, -q             Quiet output (only errors and summary)
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

using namespace nlm;

/**
 * Print help message
 */
void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo ===\n";
    std::cout << "\n";
    std::cout << "Usage: nlm_phase6_demo [options]\n";
    std::cout << "\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h              Show this help message\n";
    std::cout << "  --quick                 Run only integration verification (no full experiment)\n";
    std::cout << "  --skip-verification     Skip integration verification\n";
    std::cout << "  --neuron-count N        Number of neurons (default: 500)\n";
    std::cout << "  --max-steps S           Maximum simulation steps (default: 2000)\n";
    std::cout << "  --region-count R        Number of neural regions (default: 1)\n";
    std::cout << "  --connection-prob P     Connection probability (default: 0.1)\n";
    std::cout << "  --checkpoint-path P      Checkpoint file path (default: ./checkpoint_test.bin)\n";
    std::cout << "  --enable-checkpointing  Enable checkpointing (default: true)\n";
    std::cout << "  --enable-replay         Enable replay system (default: true)\n";
    std::cout << "  --enable-development    Enable development system (default: true)\n";
    std::cout << "  --verbose, -v           Verbose output\n";
    std::cout << "  --quiet, -q             Quiet output (only errors and summary)\n";
    std::cout << "\n";
    std::cout << "Example:\n";
    std::cout << "  nlm_phase6_demo --neuron-count 1000 --max-steps 5000 --verbose\n";
}

/**
 * Parse command line arguments
 */
void parseArgs(int argc, char* argv[], Phase6Config& config, bool& runQuick, bool& skipVerification, bool& verbose, bool& quiet) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--quick") {
            runQuick = true;
        } else if (arg == "--skip-verification") {
            skipVerification = true;
        } else if (arg == "--neuron-count") {
            if (i + 1 < argc) {
                config.neuronCount = static_cast<size_t>(std::stoul(argv[++i]));
            } else {
                std::cerr << "ERROR: --neuron-count requires a value\n";
                exit(1);
            }
        } else if (arg == "--max-steps") {
            if (i + 1 < argc) {
                config.maxSteps = std::stoull(argv[++i]);
            } else {
                std::cerr << "ERROR: --max-steps requires a value\n";
                exit(1);
            }
        } else if (arg == "--region-count") {
            if (i + 1 < argc) {
                config.regionCount = static_cast<size_t>(std::stoul(argv[++i]));
            } else {
                std::cerr << "ERROR: --region-count requires a value\n";
                exit(1);
            }
        } else if (arg == "--connection-prob") {
            if (i + 1 < argc) {
                config.connectionProbability = std::stof(argv[++i]);
            } else {
                std::cerr << "ERROR: --connection-prob requires a value\n";
                exit(1);
            }
        } else if (arg == "--checkpoint-path") {
            if (i + 1 < argc) {
                config.checkpointPath = argv[++i];
            } else {
                std::cerr << "ERROR: --checkpoint-path requires a value\n";
                exit(1);
            }
        } else if (arg == "--enable-checkpointing") {
            config.enableCheckpointing = true;
        } else if (arg == "--disable-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--enable-replay") {
            config.enableReplay = true;
        } else if (arg == "--disable-replay") {
            config.enableReplay = false;
        } else if (arg == "--enable-development") {
            config.enableDevelopment = true;
        } else if (arg == "--disable-development") {
            config.enableDevelopment = false;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else {
            std::cerr << "ERROR: Unknown option: " << arg << "\n";
            printHelp();
            exit(1);
        }
    }
}

/**
 * Validate configuration
 */
void validateConfig(const Phase6Config& config) {
    if (config.neuronCount == 0) {
        std::cerr << "ERROR: neuronCount must be greater than 0\n";
        exit(1);
    }
    
    if (config.maxSteps == 0) {
        std::cerr << "ERROR: maxSteps must be greater than 0\n";
        exit(1);
    }
    
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        std::cerr << "ERROR: connectionProbability must be between 0.0 and 1.0\n";
        exit(1);
    }
    
    if (config.regionCount == 0) {
        std::cerr << "ERROR: regionCount must be greater than 0\n";
        exit(1);
    }
}

/**
 * Print configuration summary
 */
void printConfigSummary(const Phase6Config& config, bool verbose) {
    std::cout << "=== Configuration Summary ===\n";
    std::cout << "Neurons: " << config.neuronCount << "\n";
    std::cout << "Max Steps: " << config.maxSteps << "\n";
    std::cout << "Regions: " << config.regionCount << "\n";
    std::cout << "Connection Probability: " << config.connectionProbability << "\n";
    std::cout << "Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << "\n";
    if (config.enableCheckpointing) {
        std::cout << "Checkpoint Path: " << config.checkpointPath << "\n";
    }
    std::cout << "Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << "\n";
    std::cout << "Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << "\n";
    std::cout << "\n";
}

/**
 * Print results summary
 */
void printResultsSummary(const Phase6IntegrationResult& result) {
    std::cout << "=== FINAL RESULTS ===\n";
    std::cout << "Total reward: " << result.totalReward << "\n";
    std::cout << "Avg firing rate: " << result.avgFiringRate << "\n";
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << "\n";
    std::cout << "Dopamine level: " << result.dopamineLevel << "\n";
    std::cout << "\n";
    std::cout << "=== INTEGRATION STATUS ===\n";
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << "\n";
    std::cout << "\n";
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s\n";
}

/**
 * Print progress
 */
void printProgress(uint64_t current, uint64_t total, const std::string& label) {
    int progress = (current * 100) / total;
    std::cout << "\r[" << std::string(progress / 10, '=') << std::string(10 - progress / 10, ' ') << "] " << progress << "% " << label << " " << current << "/" << total;
    std::cout.flush();
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===\n";
    std::cout << "Testing the integrated artificial brain...\n\n";
    
    // Default configuration
    Phase6Config config;
    bool runQuick = false;
    bool skipVerification = false;
    bool verbose = false;
    bool quiet = false;
    
    // Parse command line arguments
    parseArgs(argc, argv, config, runQuick, skipVerification, verbose, quiet);
    
    // Validate configuration
    validateConfig(config);
    
    // Set logging level based on verbosity
    if (quiet) {
        Logger::getInstance().setLevel(Logger::Level::Error);
    } else if (verbose) {
        Logger::getInstance().setLevel(Logger::Level::Debug);
    } else {
        Logger::getInstance().setLevel(Logger::Level::Info);
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Print configuration summary
    printConfigSummary(config, verbose);
    
    // Run integration verification
    if (!skipVerification) {
        std::cout << "--- Integration Verification ---\n";
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << "\n";
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!\n";
            return 1;
        }
        
        std::cout << "Integration verification passed!\n\n";
    }
    
    // Test individual systems (unless running quick mode)
    if (!runQuick) {
        if (!quiet) {
            std::cout << "--- Memory Integration Test ---\n";
            experiment.testMemoryIntegration();
            std::cout << "\n";
            
            std::cout << "--- Neuromodulation Integration Test ---\n";
            experiment.testNeuromodulationIntegration();
            std::cout << "\n";
            
            std::cout << "--- Checkpoint Test ---\n";
            experiment.testCheckpointing();
            std::cout << "\n";
            
            std::cout << "--- Replay Test ---\n";
            experiment.testReplay();
            std::cout << "\n";
        }
    }
    
    // Run full experiment
    if (!runQuick) {
        std::cout << "--- Full Integration Experiment ---\n";
        
        if (verbose) {
            std::cout << "Running with " << config.neuronCount << " neurons for " << config.maxSteps << " steps...\n";
            std::cout << "Progress: ";
        }
        
        auto result = experiment.run(config);
        
        if (verbose) {
            std::cout << "\n";
        }
        
        // Print results summary
        printResultsSummary(result);
    }
    
    std::cout << "\n=== Phase 6 Integration Demo Complete ===\n";
    
    return 0;
}