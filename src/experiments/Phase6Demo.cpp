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

int main(int argc, char* argv[]) {
    // Print help information
    if (argc == 1 || (argc >= 2 && std::string(argv[1]) == "--help") || 
        (argc >= 2 && std::string(argv[1]) == "-h") || 
        (argc >= 2 && std::string(argv[1]) == "--help")) {
        printHelp();
        return 0;
    }
    
    // Print version information
    if (argc == 2 && (std::string(argv[1]) == "--version" || std::string(argv[1]) == "-v")) {
        printVersion();
        return 0;
    }
    
    // Parse command line arguments
    int neuronCount = 500;
    int maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string configFile = "";
    bool verbose = false;
    bool quiet = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config option requires a filename argument\n";
                printHelp();
                return 1;
            }
        } else if (arg == "--neuron-count" || arg == "-n") {
            if (i + 1 < argc) {
                neuronCount = std::atoi(argv[++i]);
                if (neuronCount <= 0) {
                    std::cerr << "Error: Number of neurons must be positive\n";
                    return 1;
                }
            } else {
                std::cerr << "Error: --neuron-count option requires a value\n";
                return 1;
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                maxSteps = std::atoi(argv[++i]);
                if (maxSteps <= 0) {
                    std::cerr << "Error: Max steps must be positive\n";
                    return 1;
                }
            } else {
                std::cerr << "Error: --steps option requires a value\n";
                return 1;
            }
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg == "--verbose" || arg == "-V") {
            verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg[0] == '-') {
            std::cerr << "Error: Unknown option: " << arg << "\n";
            printHelp();
            return 1;
        } else {
            // Positional arguments
            if (neuronCount == 500) {  // Only override if not set via option
                neuronCount = std::atoi(arg.c_str());
            }
        }
    }
    
    // Validate parameter ranges
    if (neuronCount > 5000) {
        std::cerr << "Warning: High neuron count (" << neuronCount 
                  << "). This may impact performance.\n";
        if (!verbose && !quiet) {
            std::cout << "(Use --verbose for detailed progress)\n";
        }
    }
    
    if (maxSteps > 10000) {
        std::cerr << "Warning: Large number of steps (" << maxSteps 
                  << "). This may take a long time.\n";
        if (!verbose && !quiet) {
            std::cout << "(Use --verbose for detailed progress)\n";
        }
    }
    
    // Load configuration if specified
    std::shared_ptr<Config> config;
    if (!configFile.empty()) {
        config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            std::cerr << "Error: Failed to load configuration from " << configFile << "\n";
            return 1;
        }
        std::cout << "Loaded configuration from: " << configFile << "\n";
    } else {
        config = std::make_shared<Config>();
        std::cout << "Using default configuration.\n";
    }
    
    // Create experiment configuration
    Phase6Config phase6Config;
    phase6Config.neuronCount = static_cast<size_t>(neuronCount);
    phase6Config.maxSteps = maxSteps;
    phase6Config.enableCheckpointing = enableCheckpointing;
    phase6Config.enableReplay = enableReplay;
    phase6Config.enableDevelopment = enableDevelopment;
    
    // Override with config file values if available
    if (config) {
        if (config->has("brain.neuron_count")) {
            phase6Config.neuronCount = static_cast<size_t>(config->getOr<size_t>("brain.neuron_count", phase6Config.neuronCount));
        }
        if (config->has("simulation.max_steps")) {
            phase6Config.maxSteps = static_cast<size_t>(config->getOr<size_t>("simulation.max_steps", phase6Config.maxSteps));
        }
        if (config->has("checkpointing.enabled")) {
            phase6Config.enableCheckpointing = config->getOr<bool>("checkpointing.enabled", phase6Config.enableCheckpointing);
        }
        if (config->has("replay.enabled")) {
            phase6Config.enableReplay = config->getOr<bool>("replay.enabled", phase6Config.enableReplay);
        }
        if (config->has("development.enabled")) {
            phase6Config.enableDevelopment = config->getOr<bool>("development.enabled", phase6Config.enableDevelopment);
        }
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    if (!quiet) {
        std::cout << "=== NLM Phase 6 Integration Demo ===\n";
        std::cout << "Testing the integrated artificial brain...\n\n";
        std::cout << "Configuration:\n";
        std::cout << "  Neurons: " << phase6Config.neuronCount << "\n";
        std::cout << "  Max Steps: " << phase6Config.maxSteps << "\n";
        std::cout << "  Checkpointing: " << (phase6Config.enableCheckpointing ? "ENABLED" : "DISABLED") << "\n";
        std::cout << "  Replay: " << (phase6Config.enableReplay ? "ENABLED" : "DISABLED") << "\n";
        std::cout << "  Development: " << (phase6Config.enableDevelopment ? "ENABLED" : "DISABLED") << "\n\n";
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    if (!quiet) {
        std::cout << "--- Integration Verification ---\n";
    }
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!\n";
        return 1;
    }
    
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "Integration verification passed!\n\n";
    }
    
    // Test individual systems
    if (!quiet) {
        std::cout << "--- Memory Integration Test ---\n";
    }
    experiment.testMemoryIntegration();
    
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "--- Neuromodulation Integration Test ---\n";
    }
    experiment.testNeuromodulationIntegration();
    
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "--- Checkpoint Test ---\n";
    }
    experiment.testCheckpointing();
    
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "--- Replay Test ---\n";
    }
    experiment.testReplay();
    
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "--- Full Integration Experiment ---\n";
    }
    
    // Run full experiment with smaller settings for demo
    auto result = experiment.run(phase6Config);
    
    // Display results
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===\n";
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===\n";
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s\n";
    }
    
    return 0;
}
