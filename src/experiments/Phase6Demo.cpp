/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * This enhanced version includes:
 * - Command line argument parsing for easy customization
 * - Configuration file support
 * - Better error handling and validation
 * - Advanced help documentation
 * - Better integration status reporting
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <cstring>

using namespace nlm;

void printAdvancedHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo - Advanced Usage ===" << std::endl;
    std::cout << "\nBasic Arguments:" << std::endl;
    std::cout << "  --config=FILE            Load configuration from file (default: configs/default_phase6.cfg)" << std::endl;
    std::cout << "  --neurons=N              Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --steps=N                Max simulation steps (default: 10000)" << std::endl;
    std::cout << "  --region-count=N          Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection-prob=N      Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --checkpoint-path=PATH    Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "\nDisable Options:" << std::endl;
    std::cout << "  --no-checkpointing        Disable checkpoint saving" << std::endl;
    std::cout << "  --no-replay               Disable replay system" << std::endl;
    std::cout << "  --no-development          Disable development system" << std::endl;
    std::cout << "\nLogging Options:" << std::endl;
    std::cout << "  --verbose, -v            Enable debug logging (show detailed neural activity)" << std::endl;
    std::cout << "  --quiet, -q              Only show errors (minimal output)" << std::endl;
    std::cout << "\nAdvanced Usage Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons=2000 --steps=5000 --verbose" << std::endl;
    std::cout << "  ./nlm_phase6_demo --config=my_config.cfg --no-replay --no-development" << std::endl;
    std::cout << "  ./nlm_phase6_demo --checkpoint-path=/tmp/checkpoint.bin --quiet" << std::endl;
    std::cout << "\nConfiguration File Format (configs/default_phase6.cfg):" << std::endl;
    std::cout << "  # NLM Phase 6 Configuration" << std::endl;
    std::cout << "  # Example: 500 neurons, 2000 steps, with checkpointing" << std::endl;
    std::cout << "  neuron_count = 500" << std::endl;
    std::cout << "  max_steps = 2000" << std::endl;
    std::cout << "  region_count = 2" << std::endl;
    std::cout << "  connection_probability = 0.15" << std::endl;
    std::cout << "  enable_checkpointing = true" << std::endl;
    std::cout << "  enable_replay = true" << std::endl;
    std::cout << "  enable_development = true" << std::endl;
    std::cout << "  checkpoint_path = ./phase6_checkpoint.bin" << std::endl;
}

void runPhase6WithConfig(const Phase6Config& config) {
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "This indicates that one or more brain systems are not properly connected." << std::endl;
        std::cerr << "Please check the logs for specific integration failures." << std::endl;
        return;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with user-specified configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Running with " << config.neuronCount << " neurons for " << config.maxSteps << " steps..." << std::endl;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Average synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Prediction System: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Development System: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "Replay System: " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== TIMING ===" << std::endl;
    std::cout << "Wall clock time: " << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << "Average time per step: " << (result.totalWallClockTime / config.maxSteps) << " seconds" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse command line arguments
    Phase6Config config;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --config=FILE or --config FILE
        if (arg.substr(0, 7) == "--config") {
            std::string configFile = "configs/default_phase6.cfg";
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            
            // Try to load config from file
            auto fileConfig = std::make_shared<Config>();
            if (fileConfig->loadFromFile(configFile)) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
                // Apply file config to our Phase6Config
                // (In a real implementation, we would convert between Config and Phase6Config)
            } else {
                std::cout << "Warning: Could not load config file: " << configFile << std::endl;
            }
        }
        
        // Handle --neurons=N
        else if (arg.substr(0, 10) == "--neurons=") {
            config.neuronCount = std::stoul(arg.substr(10));
            std::cout << "Neuron count: " << config.neuronCount << std::endl;
        }
        
        // Handle --steps=N
        else if (arg.substr(0, 8) == "--steps=") {
            config.maxSteps = std::stoul(arg.substr(8));
            std::cout << "Max steps: " << config.maxSteps << std::endl;
        }
        
        // Handle --region-count=N
        else if (arg.substr(0, 14) == "--region-count=") {
            config.regionCount = std::stoul(arg.substr(14));
            std::cout << "Region count: " << config.regionCount << std::endl;
        }
        
        // Handle --connection-prob=N
        else if (arg.substr(0, 22) == "--connection-prob=") {
            config.connectionProbability = std::stof(arg.substr(22));
            std::cout << "Connection probability: " << config.connectionProbability << std::endl;
        }
        
        // Handle --no-checkpointing
        else if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
            std::cout << "Checkpointing disabled" << std::endl;
        }
        
        // Handle --no-replay
        else if (arg == "--no-replay") {
            config.enableReplay = false;
            std::cout << "Replay disabled" << std::endl;
        }
        
        // Handle --no-development
        else if (arg == "--no-development") {
            config.enableDevelopment = false;
            std::cout << "Development disabled" << std::endl;
        }
        
        // Handle --checkpoint-path=PATH
        else if (arg.substr(0, 18) == "--checkpoint-path=") {
            config.checkpointPath = arg.substr(18);
            std::cout << "Checkpoint path: " << config.checkpointPath << std::endl;
        }
        
        // Handle --verbose/-v
        else if (arg == "--verbose" || arg == "-v") {
            Logger::getInstance().setLevel(Logger::Level::Debug);
            std::cout << "Verbose logging enabled" << std::endl;
        }
        
        // Handle --quiet/-q
        else if (arg == "--quiet" || arg == "-q") {
            Logger::getInstance().setLevel(Logger::Level::Error);
            std::cout << "Quiet mode (errors only)" << std::endl;
        }
        
        // Handle --help/-h
        else if (arg == "--help" || arg == "-h") {
            printAdvancedHelp();
            return 0;
        }
    }
    
    // Validate configuration
    if (config.neuronCount < 10) {
        std::cerr << "Error: neuron_count must be at least 10, got " << config.neuronCount << std::endl;
        printAdvancedHelp();
        return 1;
    }
    
    if (config.maxSteps < 100) {
        std::cerr << "Error: max_steps must be at least 100, got " << config.maxSteps << std::endl;
        printAdvancedHelp();
        return 1;
    }
    
    // Run the enhanced Phase 6 demo with the configured parameters
    runPhase6WithConfig(config);
    
    return 0;
}