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
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Parse command line arguments for enhanced demo options
    bool showHelp = false;
    std::string configFile = "configs/default.cfg";
    bool showConfig = false;
    bool showStats = false;
    bool validateConfig = false;
    
    // Process enhanced command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg.find("--config=") == 0) {
            configFile = arg.substr(9);
        } else if (arg == "--show-config") {
            showConfig = true;
        } else if (arg == "--show-stats") {
            showStats = true;
        } else if (arg == "--validate-config") {
            validateConfig = true;
        }
    }
    
    if (showHelp) {
        std::cout << R"(
NLM Phase 6 Demo - Integration Test

USAGE:
  ./nlm_phase6_demo [OPTIONS]

OPTIONS:
  --help, -h                Show this help message
  --config=file             Load configuration from file
  --show-config             Display configuration summary
  --show-stats              Display configuration statistics
  --validate-config         Validate configuration before loading

This demo tests the integration of all Phase 6 brain systems:
  - Memory systems (working, episodic, associative)
  - Neuromodulation (dopamine, curiosity, novelty)
  - Prediction and planning systems
  - Cognitive functions (attention, concept formation)
  - Development system with plasticity rules
  - Checkpoint save/load functionality
  - Replay and consolidation systems
)" << std::endl;
        return 0;
    }
    
    // Create enhanced configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration from file
    if (!configFile.empty() && configFile != "configs/default.cfg") {
        if (config->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration (file not found: " << configFile << ")" << std::endl;
        }
    }
    
    // Apply default configuration
    config->setDefaults();
    
    // Set Phase 6 demo configuration
    config->setInt("neuron_count", 500);
    config->setInt("region_count", 1);
    config->setDouble("connection_probability", 0.15f);
    config->setDouble("stdp_ltp_weight", 0.01f);
    config->setDouble("stdp_ltd_weight", 0.012f);
    config->setDouble("synaptogenesis_rate", 0.0001f);
    config->setDouble("pruning_rate", 0.00001f);
    
    // Validate configuration if requested
    if (validateConfig) {
        if (!config->validate()) {
            std::cerr << "ERROR: Configuration validation failed!" << std::endl;
            return 1;
        }
        std::cout << "Configuration validation passed!" << std::endl << std::endl;
    }
    
    // Show configuration summary if requested
    if (showConfig) {
        std::cout << "=== Configuration Summary ===" << std::endl;
        std::cout << config->summary() << std::endl;
        std::cout << "=== Detailed Configuration ===" << std::endl;
        std::cout << config->detailedSummary() << std::endl;
    }
    
    // Show configuration statistics if requested
    if (showStats) {
        std::cout << "=== Configuration Statistics ===" << std::endl;
        std::cout << config->getStatistics() << std::endl;
    }
    
    // Create experiment with enhanced configuration
    Phase6Config phase6Config;
    phase6Config.neuronCount = config->getInt("neuron_count").value_or(500);
    phase6Config.maxSteps = 2000;
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    phase6Config.regionCount = config->getInt("region_count").value_or(1);
    phase6Config.connectionProbability = config->getDouble("connection_probability").value_or(0.15f);
    phase6Config.checkpointPath = "/tmp/nlm_checkpoint_demo.bin";
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
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
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
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
    
    return 0;
}
