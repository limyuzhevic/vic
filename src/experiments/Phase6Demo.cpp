/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Improved version with:
 * - Command-line argument support for configuration
 * - Enhanced error handling and validation
 * - Detailed progress reporting and timing
 * - Configurable demo settings
 * - Better memory and performance monitoring
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <limits>
#include <algorithm>

using namespace nlm;

// Configuration structure for the demo
struct DemoConfig {
    uint64_t neuronCount = 500;
    uint64_t maxSteps = 2000;
    uint64_t replayInterval = 100;
    uint64_t consolidationInterval = 1000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "/tmp/nlm_phase6_demo";
    uint64_t minNeuronCount = 100;
    uint64_t maxNeuronCount = 5000;
    double minTimeStep = 0.0001;
    double maxTimeStep = 0.01;
    std::string configFile = "";
};

// Parse command-line arguments into demo configuration
demo_config parseCommandLineArgs(int argc, char* argv[]) {
    demo_config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg.substr(0, 7) == "--config") {
            // Handle --config=file.json or --config file.json
            if (arg.find('=') != std::string::npos) {
                config.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
            // Handle --key=value
            std::string key = arg.substr(2, arg.find('=') - 2);
            std::string value = arg.substr(arg.find('=') + 1);
            
            try {
                if (key == "neuron_count" || key == "neurons") {
                    config.neuronCount = std::stoull(value);
                } else if (key == "max_steps" || key == "steps") {
                    config.maxSteps = std::stoull(value);
                } else if (key == "replay_interval") {
                    config.replayInterval = std::stoull(value);
                } else if (key == "consolidation_interval") {
                    config.consolidationInterval = std::stoull(value);
                } else if (key == "checkpoint_path") {
                    config.checkpointPath = value;
                } else if (key == "min_neurons") {
                    config.minNeuronCount = std::stoull(value);
                } else if (key == "max_neurons") {
                    config.maxNeuronCount = std::stoull(value);
                } else if (key == "min_timestep") {
                    config.minTimeStep = std::stod(value);
                } else if (key == "max_timestep") {
                    config.maxTimeStep = std::stod(value);
                } else if (key == "enable_checkpointing") {
                    config.enableCheckpointing = (value == "true" || value == "1" || value == "yes");
                } else if (key == "enable_replay") {
                    config.enableReplay = (value == "true" || value == "1" || value == "yes");
                } else if (key == "enable_development") {
                    config.enableDevelopment = (value == "true" || value == "1" || value == "yes");
                }
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Invalid value for " << key << ": " << value << std::endl;
                std::cerr << "  " << e.what() << std::endl;
            }
        } else if (arg[0] == '-') {
            // Handle -key value
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                try {
                    if (key == "neurons") {
                        config.neuronCount = std::stoull(value);
                    } else if (key == "steps") {
                        config.maxSteps = std::stoull(value);
                    }
                } catch (const std::exception& e) {
                    std::cerr << "ERROR: Invalid value for " << key << ": " << value << std::endl;
                }
            }
        }
    }
    
    return config;
}

// Validate configuration
demo_config validateConfig(const demo_config& config) {
    demo_config validated = config;
    
    // Validate neuron count
    if (validated.neuronCount < validated.minNeuronCount) {
        std::cout << "WARNING: Neuron count too low (" << validated.neuronCount 
                  << "), using minimum (" << validated.minNeuronCount << ")" << std::endl;
        validated.neuronCount = validated.minNeuronCount;
    } else if (validated.neuronCount > validated.maxNeuronCount) {
        std::cout << "WARNING: Neuron count too high (" << validated.neuronCount 
                  << "), using maximum (" << validated.maxNeuronCount << ")" << std::endl;
        validated.neuronCount = validated.maxNeuronCount;
    }
    
    // Validate steps
    if (validated.maxSteps < 100) {
        std::cout << "WARNING: Max steps too low (" << validated.maxSteps 
                  << "), using minimum (100)" << std::endl;
        validated.maxSteps = 100;
    }
    
    // Validate timestep
    if (validated.minTimeStep <= 0) {
        std::cerr << "ERROR: Min timestep must be positive, using default (0.001)" << std::endl;
        validated.minTimeStep = 0.001;
    }
    
    if (validated.maxTimeStep < validated.minTimeStep) {
        std::cerr << "ERROR: Max timestep must be >= min timestep, using equal values" << std::endl;
        validated.maxTimeStep = validated.minTimeStep;
    }
    
    // Ensure consistency between intervals and steps
    if (validated.replayInterval < 10) {
        std::cout << "WARNING: Replay interval too low (" << validated.replayInterval 
                  << "), using minimum (10)" << std::endl;
        validated.replayInterval = 10;
    }
    
    if (validated.consolidationInterval < 100) {
        std::cout << "WARNING: Consolidation interval too low (" << validated.consolidationInterval 
                  << "), using minimum (100)" << std::endl;
        validated.consolidationInterval = 100;
    }
    
    return validated;
}

// Print help information
void printHelp() {
    std::cout << "NLM Phase 6 Demo - Enhanced Integration Test" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config=file                Load configuration from JSON/YAML file" << std::endl;
    std::cout << "  --neuron_count=N             Set number of neurons (default: 500)" << std::endl;
    std::cout << "  --max_steps=N                Set maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --replay_interval=N          Set replay interval (default: 100)" << std::endl;
    std::cout << "  --consolidation_interval=N    Set consolidation interval (default: 1000)" << std::endl;
    std::cout << "  --checkpoint_path=PATH       Set checkpoint file path (default: /tmp/nlm_phase6_demo)" << std::endl;
    std::cout << "  --min_timestep=T             Set minimum timestep (default: 0.001)" << std::endl;
    std::cout << "  --max_timestep=T             Set maximum timestep (default: 0.01)" << std::endl;
    std::cout << "  --enable_checkpointing[=true/false]   Enable checkpointing (default: true)" << std::endl;
    std::cout << "  --enable_replay[=true/false]         Enable replay (default: true)" << std::endl;
    std::cout << "  --enable_development[=true/false]    Enable development (default: true)" << std::endl;
    std::cout << "  -n N                         Shortcut for --neuron_count" << std::endl;
    std::cout << "  -s N                         Shortcut for --max_steps" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neuron_count 1000 --max_steps 5000" << std::endl;
    std::cout << "  ./nlm_phase6_demo --config demo_config.json" << std::endl;
    std::cout << "  ./nlm_phase6_demo -n 2000 -s 10000" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration files can be JSON (.json) or YAML (.yaml/.yml) format." << std::endl;
    std::cout << "File content overrides command-line arguments." << std::endl;
}

// Load configuration from file if specified
demo_config loadConfigFromFile(const std::string& filepath) {
    demo_config config;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "WARNING: Cannot open config file: " << filepath << std::endl;
        std::cout << "Using default configuration" << std::endl;
        return config;
    }
    
    std::string line;
    uint64_t lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // Remove whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        
        size_t end = line.find_last_not_of(" \t\r\n");
        line = line.substr(start, end - start + 1);
        
        // Skip comments
        if (line.empty() || line[0] == '#' || line[0] == '/') continue;
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cout << "WARNING: Invalid line " << lineNum << " in config file: '" << line << "'" << std::endl;
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Remove quotes
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Set configuration based on key
        try {
            if (key == "neuron_count" || key == "neurons") {
                config.neuronCount = std::stoull(value);
            } else if (key == "max_steps" || key == "steps") {
                config.maxSteps = std::stoull(value);
            } else if (key == "replay_interval") {
                config.replayInterval = std::stoull(value);
            } else if (key == "consolidation_interval") {
                config.consolidationInterval = std::stoull(value);
            } else if (key == "checkpoint_path") {
                config.checkpointPath = value;
            } else if (key == "min_timestep") {
                config.minTimeStep = std::stod(value);
            } else if (key == "max_timestep") {
                config.maxTimeStep = std::stod(value);
            } else if (key == "enable_checkpointing") {
                config.enableCheckpointing = (value == "true" || value == "1" || value == "yes");
            } else if (key == "enable_replay") {
                config.enableReplay = (value == "true" || value == "1" || value == "yes");
            } else if (key == "enable_development") {
                config.enableDevelopment = (value == "true" || value == "1" || value == "yes");
            }
        } catch (const std::exception& e) {
            std::cout << "WARNING: Invalid value for " << key << ": " << value << " (" << e.what() << ")" << std::endl;
        }
    }
    
    std::cout << "Configuration loaded from file: " << filepath << std::endl;
    return config;
}

// Print configuration summary
void printConfigSummary(const demo_config& config) {
    std::cout << "=== DEMO CONFIGURATION ===" << std::endl;
    std::cout << "Neuron count: " << config.neuronCount << std::endl;
    std::cout << "Max steps: " << config.maxSteps << std::endl;
    std::cout << "Replay interval: " << config.replayInterval << std::endl;
    std::cout << "Consolidation interval: " << config.consolidationInterval << std::endl;
    std::cout << "Enable checkpointing: " << (config.enableCheckpointing ? "true" : "false") << std::endl;
    std::cout << "Enable replay: " << (config.enableReplay ? "true" : "false") << std::endl;
    std::cout << "Enable development: " << (config.enableDevelopment ? "true" : "false") << std::endl;
    std::cout << "Checkpoint path: " << config.checkpointPath << std::endl;
    std::cout << "=========================" << std::endl;
}

int main(int argc, char* argv[]) {
    auto startWallClock = std::chrono::high_resolution_clock::now();
    
    std::cout << "=== Enhanced NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain with improved error handling..." << std::endl;
    std::cout << std::endl;
    
    // Parse command-line arguments
    demo_config config = parseCommandLineArgs(argc, argv);
    
    // Load configuration from file if specified
    if (!config.configFile.empty()) {
        demo_config fileConfig = loadConfigFromFile(config.configFile);
        
        // Override with command-line arguments if provided
        if (argc > 1) {
            demo_config mergedConfig = config;
            if (!fileConfig.configFile.empty()) {
                mergedConfig.configFile = fileConfig.configFile;
            }
            config = mergedConfig;
        } else {
            config = fileConfig;
        }
    }
    
    // Validate configuration
    config = validateConfig(config);
    
    // Print configuration summary
    printConfigSummary(config);
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Create Phase6Config for the experiment
    Phase6Config experimentConfig;
    experimentConfig.neuronCount = config.neuronCount;
    experimentConfig.maxSteps = config.maxSteps;
    experimentConfig.enableCheckpointing = config.enableCheckpointing;
    experimentConfig.enableReplay = config.enableReplay;
    experimentConfig.enableDevelopment = config.enableDevelopment;
    experimentConfig.checkpointPath = config.checkpointPath;
    
    std::cout << std::endl;
    std::cout << "=== INTEGRATION VERIFICATION ===" << std::endl;
    
    // First, run the quick integration verification
    std::cout << "Running integration verification..." << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "The brain systems are not properly integrated. Please check the logs above." << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems with error handling
    std::cout << "=== SYSTEM INTEGRATION TESTS ===" << std::endl;
    
    try {
        std::cout << "--- Memory Integration Test ---" << std::endl;
        if (experiment.testMemoryIntegration()) {
            std::cout << "✓ Memory integration test passed" << std::endl;
        } else {
            std::cout << "✗ Memory integration test failed" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        if (experiment.testNeuromodulationIntegration()) {
            std::cout << "✓ Neuromodulation integration test passed" << std::endl;
        } else {
            std::cout << "✗ Neuromodulation integration test failed" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Checkpoint Test ---" << std::endl;
        if (experiment.testCheckpointing()) {
            std::cout << "✓ Checkpoint test passed" << std::endl;
        } else {
            std::cout << "✗ Checkpoint test failed" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Replay Test ---" << std::endl;
        if (experiment.testReplay()) {
            std::cout << "✓ Replay test passed" << std::endl;
        } else {
            std::cout << "✗ Replay test failed" << std::endl;
        }
        std::cout << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception during system tests: " << e.what() << std::endl;
        std::cerr << "The demo may have encountered a runtime error. Check logs for details." << std::endl;
        return 1;
    }
    
    // Run full experiment with timing and progress reporting
    std::cout << "=== FULL INTEGRATION EXPERIMENT ===" << std::endl;
    std::cout << "Running integration experiment with " << config.neuronCount 
              << " neurons for " << config.maxSteps << " steps..." << std::endl;
    
    auto experimentStart = std::chrono::high_resolution_clock::now();
    
    try {
        auto result = experiment.run(experimentConfig);
        
        auto experimentEnd = std::chrono::high_resolution_clock::now();
        auto experimentDuration = std::chrono::duration<double>(experimentEnd - experimentStart).count();
        
        // Print results with enhanced formatting
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << std::fixed;
        std::cout.precision(3);
        
        std::cout << "Execution Statistics:" << std::endl;
        std::cout << "  Experiment duration: " << experimentDuration << " seconds" << std::endl;
        std::cout << "  Total reward: " << result.totalReward << std::endl;
        std::cout << "  Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "  Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << std::setprecision(6);
        std::cout << "  Performance: " << (result.maxSteps / experimentDuration) << " steps/second" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory:     " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory:    " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation:    " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Prediction System:   " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Development System: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Checkpointing:       " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        // Additional analysis
        std::cout << "=== ANALYSIS ===" << std::endl;
        std::cout << "Memory Integration:  " << (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated ? "✓ PASS" : "✗ FAIL") << std::endl;
        std::cout << "Neuromodulation:    " << (result.neuromodulationIntegrated ? "✓ PASS" : "✗ FAIL") << std::endl;
        std::cout << "Cognition Integration:" << (result.predictionIntegrated && result.developmentIntegrated ? "✓ PASS" : "✗ FAIL") << std::endl;
        std::cout << "Persistence:        " << (result.checkpointingWorks ? "✓ PASS" : "✗ FAIL") << std::endl;
        
        // Success criteria
        bool integrationSuccess = result.memoryWorkingMemoryIntegrated && 
                                 result.memoryEpisodicMemoryIntegrated && 
                                 result.neuromodulationIntegrated && 
                                 result.predictionIntegrated && 
                                 result.developmentIntegrated && 
                                 result.checkpointingWorks;
        
        std::cout << std::endl;
        if (integrationSuccess) {
            std::cout << "🎉 INTEGRATION SUCCESSFUL! All brain systems are properly integrated." << std::endl;
        } else {
            std::cout << "⚠️  INTEGRATION PARTIAL. Some systems need attention or may be working correctly in different configurations." << std::endl;
        }
        
        return integrationSuccess ? 0 : 1;
        
    } catch (const std::exception& e) {
        auto experimentEnd = std::chrono::high_resolution_clock::now();
        auto experimentDuration = std::chrono::duration<double>(experimentEnd - experimentStart).count();
        
        std::cerr << std::endl;
        std::cerr << "ERROR: Experiment failed after " << experimentDuration << " seconds!" << std::endl;
        std::cerr << "Exception: " << e.what() << std::endl;
        std::cerr << std::endl;
        std::cerr << "The integration experiment encountered an error. This could be due to:" << std::endl;
        std::cerr << "  - Configuration mismatch between systems" << std::endl;
        std::cerr << "  - Missing dependencies or initialization failures" << std::endl;
        std::cerr << "  - Resource constraints (memory, CPU)" << std::endl;
        std::cerr << "  - Network or I/O issues" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Please check the logs above for detailed error information." << std::endl;
        
        return 1;
    }
    
    auto wallClockEnd = std::chrono::high_resolution_clock::now();
    auto wallClockDuration = std::chrono::duration<double>(wallClockEnd - startWallClock).count();
    
    std::cout << std::endl;
    std::cout << "=== DEMO COMPLETED ===" << std::endl;
    std::cout << "Total wall clock time: " << wallClockDuration << " seconds" << std::endl;
    
    return 0;
}