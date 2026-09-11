/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/CommandLineParser.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    // Parse command line arguments
    auto parser = std::make_unique<CommandLineParser>(argc, argv);
    
    // Handle help/version flags before anything else
    if (parser->showHelp()) {
        parser->printHelp();
        return 0;
    }
    
    if (parser->showVersion()) {
        parser->printVersion();
        return 0;
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
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
    
    // Create config from command line parser
    Phase6Config config;
    
    // Apply default values from parser config if available
    auto nlmConfig = parser->getConfig();
    if (nlmConfig) {
        config.neuronCount = nlmConfig->getOr<int64_t>("neuron_count", config.neuronCount);
        config.maxSteps = nlmConfig->getOr<int64_t>("max_steps", config.maxSteps);
        config.regionCount = nlmConfig->getOr<int64_t>("region_count", config.regionCount);
        config.connectionProbability = nlmConfig->getOr<float>("connection_probability", config.connectionProbability);
        config.enableCheckpointing = nlmConfig->getOr<bool>("enable_checkpointing", config.enableCheckpointing);
        config.enableReplay = nlmConfig->getOr<bool>("enable_replay", config.enableReplay);
        config.enableDevelopment = nlmConfig->getOr<bool>("enable_development", config.enableDevelopment);
        config.checkpointPath = nlmConfig->getOr<std::string>("checkpoint_path", config.checkpointPath);
    }
    
    // Apply command line parameter overrides
    auto overrides = parser->getParameterOverrides();
    for (const auto& [key, value] : overrides) {
        if (key == "neuron_count") config.neuronCount = std::stoi(value);
        else if (key == "max_steps") config.maxSteps = std::stoi(value);
        else if (key == "region_count") config.regionCount = std::stoi(value);
        else if (key == "connection_probability") config.connectionProbability = std::stof(value);
        else if (key == "enable_checkpointing") config.enableCheckpointing = (value == "true" || value == "1");
        else if (key == "enable_replay") config.enableReplay = (value == "true" || value == "1");
        else if (key == "enable_development") config.enableDevelopment = (value == "true" || value == "1");
        else if (key == "checkpoint_path") config.checkpointPath = value;
    }
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
    
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
