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
#include <string>
#include <cstdlib>

using namespace nlm;

// Print help information for Phase 6 demo
void printHelp(const char* programName) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
    std::cout << "  --verbose               Enable verbose logging" << std::endl;
    std::cout << "  --loglevel <level>      Set log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --neuron_count <value>  Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max_steps <value>     Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --region_count <value>  Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection_prob <value> Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --quick                 Run quick verification only (skip full experiment)" << std::endl;
    std::cout << "  --no-checkpointing      Disable checkpoint testing" << std::endl;
    std::cout << "  --no-replay             Disable replay testing" << std::endl;
    std::cout << "  --no-development        Disable development system testing" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --verbose --neuron_count 1000" << std::endl;
    std::cout << "  " << programName << " --quick" << std::endl;
    std::cout << "  " << programName << " --config myconfig.cfg" << std::endl;
}

// Parse command line arguments for Phase 6 demo
bool parseArguments(int argc, char* argv[], bool& verbose, std::string& configFile,
                   size_t& neuronCount, size_t& maxSteps, size_t& regionCount,
                   float& connectionProb, bool& quickMode,
                   bool& enableCheckpointing, bool& enableReplay,
                   bool& enableDevelopment) {
    bool helpShown = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            helpShown = true;
            break;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quick") {
            quickMode = true;
        } else if (arg == "--no-checkpointing") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg == "--loglevel") {
            if (i + 1 < argc) {
                std::string level = argv[++i];
                // Note: This would need to be converted to LogLevel enum
                // For now, just acknowledge the option
                std::cout << "Note: --loglevel " << level << " (logging level setting not fully implemented yet)" << std::endl;
            } else {
                std::cerr << "Error: --loglevel requires a value" << std::endl;
                return false;
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename" << std::endl;
                return false;
            }
        } else if (arg == "--neuron_count") {
            if (i + 1 < argc) {
                neuronCount = std::stoull(argv[++i]);
            } else {
                std::cerr << "Error: --neuron_count requires a value" << std::endl;
                return false;
            }
        } else if (arg == "--max_steps") {
            if (i + 1 < argc) {
                maxSteps = std::stoull(argv[++i]);
            } else {
                std::cerr << "Error: --max_steps requires a value" << std::endl;
                return false;
            }
        } else if (arg == "--region_count") {
            if (i + 1 < argc) {
                regionCount = std::stoull(argv[++i]);
            } else {
                std::cerr << "Error: --region_count requires a value" << std::endl;
                return false;
            }
        } else if (arg == "--connection_prob") {
            if (i + 1 < argc) {
                connectionProb = std::stof(argv[++i]);
            } else {
                std::cerr << "Error: --connection_prob requires a value" << std::endl;
                return false;
            }
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            helpShown = true;
        }
    }
    
    if (helpShown) {
        printHelp(argv[0]);
        return false;
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool verbose = false;
    std::string configFile = "";
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    size_t regionCount = 1;
    float connectionProb = 0.1f;
    bool quickMode = false;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    if (argc > 1) {
        bool parseSuccess = parseArguments(argc, argv, verbose, configFile,
                                         neuronCount, maxSteps, regionCount,
                                         connectionProb, quickMode,
                                         enableCheckpointing, enableReplay,
                                         enableDevelopment);
        if (!parseSuccess) {
            return 1;
        }
    }
    
    // Print header
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    
    if (verbose) {
        std::cout << "Verbose mode enabled" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << neuronCount << std::endl;
        std::cout << "  Max Steps: " << maxSteps << std::endl;
        std::cout << "  Regions: " << regionCount << std::endl;
        std::cout << "  Connection Prob: " << connectionProb << std::endl;
        std::cout << "  Checkpointing: " << (enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Replay: " << (enableReplay ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Development: " << (enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << std::endl;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Quick mode - just run verification
    if (quickMode) {
        std::cout << "--- Quick Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Quick verification completed successfully!" << std::endl;
        return 0;
    }
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    if (enableCheckpointing) {
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
    }
    
    if (enableReplay) {
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
    }
    
    // Run full experiment with provided configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.regionCount = regionCount;
    config.connectionProbability = connectionProb;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    
    // Use config file if provided (for additional custom settings)
    if (!configFile.empty()) {
        // Note: Phase6Config doesn't have file loading capability
        // Users would need to modify the config struct or create a new config system
        std::cout << "Config file loading requested: " << configFile << std::endl;
        std::cout << "(Note: File-based configuration not yet implemented for Phase6Config)" << std::endl;
    }
    
    auto result = experiment.run(config);
    
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
    
    // Provide summary
    std::cout << std::endl;
    std::cout << "=== DEMONSTRATION SUMMARY ===" << std::endl;
    if (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated && 
        result.neuromodulationIntegrated && result.checkpointingWorks && result.replayWorks) {
        std::cout << "✓ All systems integrated and functional!" << std::endl;
        std::cout << "✓ Phase 6 integration test PASSED" << std::endl;
    } else {
        std::cout << "⚠ Some systems may have issues (see details above)" << std::endl;
        std::cout << "  Phase 6 integration test COMPLETED with warnings" << std::endl;
    }
    
    return 0;
}
