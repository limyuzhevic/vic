#include "AdvancedConfig.hpp"
#include <iostream>
#include <fstream>

namespace nlm {

// Example configuration functions
void createResearchExampleConfig() {
    AdvancedConfig config;
    
    std::cout << "Creating Research Preset Example..." << std::endl;
    config.applyPreset("research");
    config.saveAdvancedConfig("research_example_config.json", 
        "Example configuration optimized for scientific research and experimentation");
    std::cout << "Saved to research_example_config.json" << std::endl;
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config.getSummary() << std::endl;
}

void createLearningExampleConfig() {
    AdvancedConfig config;
    
    std::cout << "Creating Learning Preset Example..." << std::endl;
    config.applyPreset("learning");
    config.saveAdvancedConfig("learning_example_config.json", 
        "Example configuration optimized for agent learning and adaptation");
    std::cout << "Saved to learning_example_config.json" << std::endl;
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config.getSummary() << std::endl;
}

void createSimulationExampleConfig() {
    AdvancedConfig config;
    
    std::cout << "Creating Simulation Preset Example..." << std::endl;
    config.applyPreset("simulation");
    config.saveAdvancedConfig("simulation_example_config.json", 
        "Example configuration optimized for simulation environments");
    std::cout << "Saved to simulation_example_config.json" << std::endl;
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config.getSummary() << std::endl;
}

void createProductionExampleConfig() {
    AdvancedConfig config;
    
    std::cout << "Creating Production Preset Example..." << std::endl;
    config.applyPreset("production");
    config.saveAdvancedConfig("production_example_config.json", 
        "Example configuration optimized for production deployment");
    std::cout << "Saved to production_example_config.json" << std::endl;
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config.getSummary() << std::endl;
}

void createCustomExampleConfig() {
    AdvancedConfig config;
    
    std::cout << "Creating Custom Example Configuration..." << std::endl;
    
    // Set neural parameters
    NeuralParameters neuralParams;
    neuralParams.neuronThreshold = 1.2;
    neuralParams.learningRate = 0.08;
    neuralParams.neuronCount = 2000;
    config.setNeuralParameters(neuralParams);
    
    // Set memory parameters
    MemoryParameters memoryParams;
    memoryParams.workingMemoryCapacity = 200;
    memoryParams.episodicMemoryConsolidationRate = 0.08;
    config.setMemoryParameters(memoryParams);
    
    // Set prediction parameters
    PredictionParameters predictionParams;
    predictionParams.predictionAccuracyThreshold = 0.85;
    predictionParams.predictionHorizonSteps = 15;
    config.setPredictionParameters(predictionParams);
    
    // Set neuromodulation parameters
    NeuromodulationParameters neuromodulationParams;
    neuromodulationParams.rewardStrength = 0.9;
    neuromodulationParams.errorStrength = 0.8;
    config.setNeuromodulationParameters(nuromodulationParams);
    
    // Set cognitive parameters
    CognitiveParameters cognitiveParams;
    cognitiveParams.planningDepth = 4;
    cognitiveParams.attentionFocusStrength = 0.8;
    config.setCognitiveParameters(cognitiveParams);
    
    // Save custom configuration
    config.saveAdvancedConfig("custom_example_config.json", 
        "Custom example configuration with fine-tuned parameters");
    std::cout << "Saved to custom_example_config.json" << std::endl;
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config.getSummary() << std::endl;
}

void demonstrateConfigurationValidation() {
    AdvancedConfig config;
    
    std::cout << "=== Configuration Validation Example ===" << std::endl;
    
    // Valid configuration
    config.applyPreset("research");
    std::cout << "Research preset is valid: " << (config.isValid() ? "YES" : "NO") << std::endl;
    if (!config.isValid()) {
        std::cout << "Validation errors:" << std::endl << config.getValidationErrors();
    }
    
    // Modify to create an invalid configuration
    config.getNeuralParameters().learningRate = 1.5; // Invalid value > 1.0
    std::cout << "\nModified configuration (invalid) is valid: " << (config.isValid() ? "YES" : "NO") << std::endl;
    if (!config.isValid()) {
        std::cout << "Validation errors:" << std::endl << config.getValidationErrors();
    }
}

void demonstrateConfigurationComparison() {
    AdvancedConfig config1;
    AdvancedConfig config2;
    
    std::cout << "=== Configuration Comparison Example ===" << std::endl;
    
    config1.applyPreset("research");
    config2.applyPreset("learning");
    
    std::cout << "Comparing Research vs Learning presets:" << std::endl;
    config1.compareConfigurations(config2);
}

void demonstratePresetExamples() {
    std::cout << "=== Available Preset Examples ===" << std::endl;
    
    auto presets = AdvancedConfig::getAvailablePresets();
    auto descriptions = AdvancedConfig::getPresetDescriptions();
    
    for (const auto& preset : presets) {
        std::cout << preset << ": " << descriptions[preset] << std::endl;
    }
    
    std::cout << "\nNote: Run the example functions to see actual configurations." << std::endl;
}

void runAllExamples() {
    std::cout << "=== Advanced Configuration Examples ===" << std::endl << std::endl;
    
    createResearchExampleConfig();
    std::cout << std::endl;
    
    createLearningExampleConfig();
    std::cout << std::endl;
    
    createSimulationExampleConfig();
    std::cout << std::endl;
    
    createProductionExampleConfig();
    std::cout << std::endl;
    
    createCustomExampleConfig();
    std::cout << std::endl;
    
    demonstrateConfigurationValidation();
    std::cout << std::endl;
    
    demonstrateConfigurationComparison();
    std::cout << std::endl;
    
    demonstratePresetExamples();
}

} // namespace nlm
