// Phase 6 Enhanced Demo - Integration Test with New Features
//
// This demo showcases the improved NLM Phase 6 integration with:
// - Enhanced configuration validation
// - Better error handling
// - Improved API usability
// - Advanced configuration options
// - Comprehensive system testing

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>

using namespace nlm;

// Helper function to print configuration validation results
void printConfigurationResults(const std::vector<std::string>& errors) {
    if (errors.empty()) {
        std::cout << "✓ Configuration is valid!" << std::endl;
    } else {
        std::cout << "✗ Configuration errors found:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
}

// Demonstrate advanced configuration options
Phase6Config createAdvancedConfig() {
    Phase6Config config;
    
    // Set custom values for advanced testing
    config.neuronCount = 1000;
    config.maxSteps = 5000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.enableNovelty = true;
    config.enableCuriosity = true;
    config.enablePrediction = true;
    
    // Advanced neuromodulation settings
    config.dopamineGain = 0.8f;
    config.curiosityThreshold = 0.5f;
    config.noveltyDecay = 0.95f;
    
    // Advanced memory settings
    config.workingMemoryCapacity = 200;
    config.episodeCapacity = 500;
    config.replayInterval = 100;
    config.consolidationInterval = 1000;
    
    // Advanced development settings
    config.developmentStages = 5;
    config.maturationRate = 0.3f;
    
    // Advanced plasticity settings
    config.stdpLTPWeight = 0.02f;
    config.stdpLTDWeight = 0.018f;
    config.stdpTimeConstant = 25.0f;
    
    return config;
}

// Demonstrate the new convenience functions
void demonstrateConvenienceFunctions(AgentBrain& agent) {
    std::cout << "\n--- Convenience Functions Demo ---" << std::endl;
    
    // Test configuration validation
    bool configValid = agent.validateConfiguration();
    std::cout << "Configuration validation: " << (configValid ? "PASS" : "FAIL") << std::endl;
    
    // Test default configuration factory
    auto defaultConfig = AgentBrain::createDefaultConfig();
    std::cout << "Default configuration created with " << defaultConfig->getKeys().size() << " parameters" << std::endl;
    
    // Demonstrate configuration validation from the config system
    auto configErrors = defaultConfig->validateConfigurationDetailed();
    printConfigurationResults(configErrors);
}

// Demonstrate the enhanced Phase 6 integration
int main(int argc, char* argv[]) {
    std::cout << "=== ENHANCED NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Showcasing improved NLM integration with advanced features..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    std::cout << "--- Enhanced Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Enhanced integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Enhanced integration verification passed!" << std::endl << std::endl;
    
    // Demonstrate configuration validation with errors
    std::cout << "--- Configuration Validation Demo ---" << std::endl;
    auto config = std::make_shared<Config>();
    
    // Create an invalid configuration to demonstrate error handling
    config->set("neuron_count", 0);  // Invalid: must be positive
    config->set("region_count", -5);  // Invalid: must be positive
    config->set("connection_probability", 1.5f);  // Invalid: must be 0.0-1.0
    
    auto errors = config->validateConfigurationDetailed();
    printConfigurationResults(errors);
    
    // Create a valid configuration
    config->clear();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    std::cout << "\nValid configuration test:" << std::endl;
    auto validErrors = config->validateConfigurationDetailed();
    printConfigurationResults(validErrors);
    
    // Test the new enhanced Phase 6 demo with advanced configuration
    std::cout << "\n--- Enhanced Phase 6 Demo ---" << std::endl;
    Phase6Config advancedConfig = createAdvancedConfig();
    
    std::cout << "Running Phase 6 experiment with advanced configuration..." << std::endl;
    std::cout << "Settings:" << std::endl;
    std::cout << "  - Neurons: " << advancedConfig.neuronCount << std::endl;
    std::cout << "  - Max steps: " << advancedConfig.maxSteps << std::endl;
    std::cout << "  - Checkpointing: " << (advancedConfig.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Replay: " << (advancedConfig.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Development: " << (advancedConfig.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    
    auto result = experiment.run(advancedConfig);
    
    std::cout << std::endl;
    std::cout << "=== ENHANCED DEMO RESULTS ===" << std::endl;
    std::cout << "Total reward: " << std::fixed << std::setprecision(2) << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    std::cout << "Total spikes: " << result.totalSpikeCount << std::endl;
    std::cout << "Total learning events: " << result.totalLearningEvents << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== ENHANCED INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Associative Memory: " << (result.memoryAssociativeMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Procedural Memory: " << (result.memoryProceduralMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Cognition: " << (result.cognitionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << std::fixed << std::setprecision(2) << result.totalWallClockTime << "s" << std::endl;
    
    std::cout << "\n=== IMPROVEMENTS DEMONSTRATION ===" << std::endl;
    std::cout << "The enhanced Phase 6 integration demonstrates:" << std::endl;
    std::cout << "1. Better configuration validation with detailed error messages" << std::endl;
    std::cout << "2. Enhanced Phase 6 integration with more comprehensive testing" << std::endl;
    std::cout << "3. Advanced configuration options for research experiments" << std::endl;
    std::cout << "4. Improved error handling and reporting" << std::endl;
    std::cout << "5. Better separation of concerns in initialization" << std::endl;
    std::cout << "6. Enhanced developer experience with convenience functions" << std::endl;
    
    return 0;
}