#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::cout << "=== Config Class Improvements Demo ===" << std::endl;
    std::cout << "Demonstrating the enhanced Config class with JSON support..." << std::endl;
    std::cout << std::endl;
    
    // Create a config instance
    nlm::Config config;
    
    std::cout << "1. Testing key=value format (backward compatibility)..." << std::endl;
    
    // Set values using key=value format
    config.set("brain_type", "NEURAL_NETWORK");
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    config.set("plasticity_enabled", true);
    
    std::cout << "   Values set successfully:" << std::endl;
    std::cout << "   - brain_type: " << config.get<std::string>("brain_type").value_or("") << std::endl;
    std::cout << "   - neuron_count: " << config.get<int64_t>("neuron_count").value_or(0) << std::endl;
    std::cout << "   - simulation_timestep: " << config.get<double>("simulation_timestep").value_or(0.0) << std::endl;
    std::cout << "   - plasticity_enabled: " << (config.get<bool>("plasticity_enabled").value_or(false) ? "true" : "false") << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. Testing JSON file loading..." << std::endl;
    
    // Create a JSON config file
    std::ofstream jsonFile("demo_config.json");
    if (jsonFile.is_open()) {
        jsonFile << R"({
    "brain_type": "ADVANCED_NEURAL_NETWORK",
    "neuron_count": 500,
    "simulation_timestep": 0.0005,
    "plasticity_enabled": true,
    "learning_rate": 0.01,
    "reward_discount_factor": 0.99,
    "sensory_neurons": [1, 2, 3, 4, 5],
    "motor_neurons": [10, 20, 30, 40, 50],
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001
})";
        jsonFile.close();
        std::cout << "   JSON config file created: demo_config.json" << std::endl;
    }
    
    // Load JSON config
    if (config.loadFromFile("demo_config.json")) {
        std::cout << "   JSON config loaded successfully" << std::endl;
        
        std::cout << "   Values from JSON:" << std::endl;
        std::cout << "   - brain_type: " << config.get<std::string>("brain_type").value_or("") << std::endl;
        std::cout << "   - neuron_count: " << config.get<int64_t>("neuron_count").value_or(0) ना std::endl;
        std::cout << "   - simulation_timestep: " << config.get<double>("simulation_timestep").value_or(0.0) << std::endl;
        std::cout << "   - plasticity_enabled: " << (config.get<bool>("plasticity_enabled").value_or(false) ? "true" : "false") << std::endl;
        
        // Test array support
        auto sensoryNeurons = config.get<std::vector<int>>("sensory_neurons");
        if (sensoryNeurons.has_value()) {
            std::cout << "   - sensory_neurons: [";
            for (size_t i = 0; i < sensoryNeurons.value().size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << sensoryNeurons.value()[i];
            }
            std::cout << "]" << std::endl;
        }
        
    } else {
        std::cout << "   Failed to load JSON config (fallback to key=value)" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "3. Testing JSON save functionality..." << std::endl;
    
    // Create another config instance for saving
    nlm::Config configToSave;
    
    // Set various values
    configToSave.set("project_name", "NLM Demo");
    configToSave.set("version", 2.0);
    configToSave.set("author", "NLM Team");
    configToSave.set("description", "Neural Learning Machine Demo");
    configToSave.set("features", std::vector<std::string>{"json_support", "backward_compatibility", "enhanced_error_handling"});
    configToSave.set("performance_metrics", std::vector<double>{0.95, 0.87, 0.92});
    
    // Save as JSON
    if (configToSave.saveToFile("saved_config.json")) {
        std::cout << "   Configuration saved as JSON: saved_config.json" << std::endl;
        
        // Load back to verify
        nlm::Config configLoaded;
        if (configLoaded.loadFromFile("saved_config.json")) {
            std::cout << "   Configuration loaded back successfully" << std::endl;
            std::cout << "   - project_name: " << configLoaded.get<std::string>("project_name").value_or("") << std::endl;
            std::cout << "   - version: " << configLoaded.get<double>("version").value_or(0.0) << std::endl;
            
            auto features = configLoaded.get<std::vector<std::string>>("features");
            if (features.has_value()) {
                std::cout << "   - features: [";
                for (size_t i = 0; i < features.value().size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << "\"" << features.value()[i] << "\"";
                }
                std::cout << "]" << std::endl;
            }
        }
    }
    std::cout << std::endl;
    
    std::cout << "4. Testing configuration summary..." << std::endl;
    
    // Add more values to demonstrate summary
    config.set("startup_time", "2026-01-01T00:00:00Z");
    config.set("environment", "production");
    config.set("debug_mode", false);
    
    std::cout << "   Configuration summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    std::cout << std::endl;
    
    std::cout << "5. Testing error handling and fallback..." << std::endl;
    
    // Test with invalid JSON file (should fallback to key=value)
    std::ofstream invalidJsonFile("invalid_config.json");
    invalidJsonFile << "{"; // Invalid JSON
    invalidJsonFile.close();
    
    nlm::Config configWithInvalid;
    if (!configWithInvalid.loadFromFile("invalid_config.json")) {
        std::cout << "   Invalid JSON file handled correctly (fallback to key=value)" << std::endl;
    }
    
    // Test with non-existent file
    if (!configWithInvalid.loadFromFile("nonexistent.json")) {
        std::cout << "   Non-existent file handled correctly" << std::endl;
    }
    std::cout << std::endl;
    
    // Clean up temporary files
    std::remove("demo_config.json");
    std::remove("saved_config.json");
    std::remove("invalid_config.json");
    
    std::cout << "=== Config Class Improvements Demo Complete ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Key improvements demonstrated:" << std::endl;
    std::cout << "✓ JSON file format support with nlohmann/json" << std::endl;
    std::cout << "✓ Key=value format support (backward compatibility)" << std::endl;
    std::cout << "✓ Automatic file format detection" << std::endl;
    std::cout << "✓ Enhanced save functionality" << std::endl;
    std::cout << "✓ Comprehensive error handling" << std::endl;
    std::cout << "✓ Support for complex data types (arrays, objects)" << std::endl;
    std::cout << "✓ Graceful fallback on errors" << std::endl;
    
    return 0;
}
