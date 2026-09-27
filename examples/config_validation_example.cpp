// Configuration Validation Example
// Demonstrates the new configuration validation system

#include "core/Config/Config.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== NLM Configuration Validation Example ===\n";
    
    // Create a configuration object
    nlm::Config config;
    
    // Load default values from schema
    config.loadFromDefaults();
    
    std::cout << "1. Default configuration loaded:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Modify some values
    std::cout << "2. Modifying configuration values..." << std::endl;
    config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15, nlm::ConfigSource::Runtime);
    config.set("log_level", std::string("DEBUG"), nlm::ConfigSource::Runtime);
    config.set("visualization_enabled", true, nlm::ConfigSource::Runtime);
    
    std::cout << "3. Current configuration:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Validate the configuration
    std::cout << "4. Validating configuration..." << std::endl;
    nlm::ValidationResult validationResult = config.validate();
    
    if (validationResult.valid) {
        std::cout << "   ✓ Configuration is valid!" << std::endl;
    } else {
        std::cout << "   ✗ Configuration has errors:" << std::endl;
        std::cout << "   " << validationResult.getErrors() << std::endl;
    }
    
    if (!validationResult.warnings.empty()) {
        std::cout << "   Warnings:" << std::endl;
        std::cout << "   " << validationResult.getWarnings() << std::endl;
    }
    
    // Test validation of individual values
    std::cout << "5. Testing individual value validation:" << std::endl;
    
    // Test valid values
    std::cout << "   Testing valid values:" << std::endl;
    if (config.validateValue("neuron_count", 750)) {
        std::cout << "     ✓ neuron_count=750 is valid" << std::endl;
    }
    if (config.validateValue("connection_probability", 0.05)) {
        std::cout << "     ✓ connection_probability=0.05 is valid" << std::endl;
    }
    
    // Test invalid values
    std::cout << "   Testing invalid values:" << std::endl;
    if (!config.validateValue("neuron_count", 0)) {
        std::cout << "     ✓ neuron_count=0 correctly rejected" << std::endl;
    }
    if (!config.validateValue("connection_probability", 1.5)) {
        std::cout << "     ✓ connection_probability=1.5 correctly rejected" << std::endl;
    }
    
    // Test schema information
    std::cout << "6. Schema information:" << std::endl;
    auto schema = nlm::Config::getDefaultSchema();
    std::cout << "   Total schema entries: " << schema.size() << std::endl;
    
    // Show a few examples from schema
    int count = 0;
    for (const auto& pair : schema) {
        if (count < 3) {
            std::cout << "   - " << pair.first << ": " << pair.second.description << std::endl;
            count++;
        }
    }
    
    // Save configuration to file
    std::cout << "7. Saving configuration to file..." << std::endl;
    if (config.saveToFile("example_config.cfg")) {
        std::cout << "   ✓ Configuration saved to example_config.cfg" << std::endl;
        
        // Reload and validate
        nlm::Config config2;
        config2.loadFromFile("example_config.cfg");
        
        std::cout << "8. Validating reloaded configuration..." << std::endl;
        nlm::ValidationResult reloadResult = config2.validate();
        if (reloadResult.valid) {
            std::cout << "   ✓ Reloaded configuration is valid!" << std::endl;
        } else {
            std::cout << "   ✗ Reloaded configuration has errors:" << std::endl;
            std::cout << "   " << reloadResult.getErrors() << std::endl;
        }
    }
    
    std::cout << "\n=== Configuration Validation Example Complete ===" << std::endl;
    return 0;
}
