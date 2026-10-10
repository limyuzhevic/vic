# Advanced Configuration Examples
// Example usage of the advanced configuration system for power users

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "core/AdvancedConfig.hpp"

void exampleJSONConfiguration() {
    std::cout << "=== JSON Configuration Example ===" << std::endl;
    
    auto config = nlm::createAdvancedConfig();
    
    // Load from JSON file
    bool success = config->loadFromFile("config.json", nlm::ConfigFormat::JSON);
    if (success) {
        std::cout << "Configuration loaded successfully" << std::endl;
        
        // Access values
        std::string neuralNetwork = config->getValue<std::string>("neural_network_type");
        int64_t neuronCount = config->getValue<int64_t>("neuron_count");
        double learningRate = config->getValue<double>("learning_rate");
        
        std::cout << "  Neural network type: " << neuralNetwork << std::endl;
        std::cout << "  Neuron count: " << neuronCount << std::endl;
        std::cout << "  Learning rate: " << learningRate << std::endl;
        
        // Export to YAML
        std::string yamlOutput = config->exportToYAML();
        std::cout << "YAML export:\n" << yamlOutput << std::endl;
    }
}

void exampleYAMLConfiguration() {
    std::cout << "=== YAML Configuration Example ===" << std::endl;
    
    auto config = nlm::createAdvancedConfig();
    
    // Load from YAML string
    std::string yamlContent = R"""
# Advanced NLM Configuration
brain:
  type: "Adaptive"
  neuron_count: 5000
  connectivity: "spatially_clustered"
  
plasticity:
  stdp:
    learning_rate: 0.01
    time_constant: 20.0
    ltp_weight: 0.012
    ltd_weight: 0.01
  
memory:
  working_memory_size: 1000
  episodic_capacity: 10000
  
neuromodulation:
  dopamine_enabled: true
  curiosity_enabled: true
  novelty_enabled: true
""";
    
    bool success = config->loadFromYAML(yamlContent);
    if (success) {
        std::cout << "YAML configuration loaded successfully" << std::endl;
        
        // Access nested values
        try {
            auto brainSection = config->getValue<std::unordered_map<std::string, nlm::ConfigValue>>("brain");
            if (brainSection.find("neuron_count") != brainSection.end()) {
                int64_t count = std::get<int64_t>(brainSection.at("neuron_count"));
                std::cout << "Brain neuron count: " << count << std::endl;
            }
        } catch (...) {
            std::cout << "Could not access nested brain section" << std::endl;
        }
        
        // Export to JSON
        std::string jsonOutput = config->exportToJSON();
        std::cout << "JSON export:\n" << jsonOutput << std::endl;
    }
}

void exampleConfigurationCommands() {
    std::cout << "=== Configuration Commands Example ===" << std::endl;
    
    auto config = nlm::createAdvancedConfig();
    
    // Load default configuration
    config->loadFromText(R"""
random_seed = 42
neuron_count = 1000
simulation_timestep = 0.001
plasticity_learning_rate = 0.01
""");
    
    std::cout << "Default configuration loaded" << std::endl;
    
    // Show available commands
    std::cout << "Available commands:" << std::endl;
    auto commands = config->listCommands();
    for (const auto& cmd : commands) {
        std::cout << "  " << cmd << std::endl;
    }
    
    // Execute save command
    std::vector<std::string> saveArgs = {"output.json"};
    bool saveSuccess = config->executeCommand("save", saveArgs);
    std::cout << "Save command: " << (saveSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    // Execute export command
    std::vector<std::string> exportArgs = {"output.yaml", "yaml"};
    bool exportSuccess = config->executeCommand("export", exportArgs);
    std::cout << "Export command: " << (exportSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    // Execute validate command
    bool validateSuccess = config->executeCommand("validate", {});
    std::cout << "Validate command: " << (validateSuccess ? "SUCCESS" : "FAILED") << std::endl;
}

void exampleAdvancedConfigurationUsage() {
    std::cout << "=== Advanced Configuration Usage Example ===" << std::endl;
    
    auto config = nlm::createAdvancedConfig();
    
    // Set advanced configuration values
    config->setValue<std::string>("brain.model.type", "AdvancedLIF");
    config->setValue<double>("brain.model.parameters.a", 0.02);
    config->setValue<double>("brain.model.parameters.b", -65.0);
    config->setValue<int64_t>("brain.model.parameters.threshold", -50);
    
    config->setValue<std::string>("plasticity.rule", "STDP");
    config->setValue<double>("plasticity.learning_rate", 0.015);
    config->setValue<double>("plasticity.time_constant", 25.0);
    
    config->setValue<std::vector<double>>("memory.working.capacity", std::vector<double>{0.8, 0.9, 1.0});
    config->setValue<bool>("neuromodulation.dopamine.enabled", true);
    config->setValue<bool>("neuromodulation.curiosity.enabled", true);
    
    // Validate configuration
    std::vector<std::string> validationErrors = config->validate();
    if (validationErrors.empty()) {
        std::cout << "Configuration is valid" << std::endl;
    } else {
        std::cout << "Configuration validation errors:" << std::endl;
        for (const auto& error : validationErrors) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    // Get summary
    std::string summary = config->getSummary();
    std::cout << "Configuration summary:" << std::endl;
    std::cout << summary << std::endl;
}

int main() {
    std::cout << "=== Advanced Configuration Examples ===" << std::endl;
    std::cout << "This demonstrates the new advanced configuration system" << std::endl;
    std::cout << "with support for JSON, YAML, and powerful command interfaces." << std::endl;
    std::cout << std::endl;
    
    exampleJSONConfiguration();
    std::cout << std::endl;
    
    exampleYAMLConfiguration();
    std::cout << std::endl;
    
    exampleConfigurationCommands();
    std::cout << std::endl;
    
    exampleAdvancedConfigurationUsage();
    std::cout << std::endl;
    
    std::cout << "=== All Examples Completed ===" << std::endl;
    return 0;
}
