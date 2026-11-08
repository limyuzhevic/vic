// Demonstration of the Enhanced Configuration System
// This file shows how to use the new JSON/YAML configuration features

#include <iostream>
#include <memory>
#include <fstream>
#include <filesystem>
#include "core/Config/Config_enhanced.hpp"

void demo_file_format_detection() {
    std::cout << "=== File Format Detection Demo ===" << std::endl;
    
    // Test JSON format detection
    std::string jsonPath = "configs/default_enhanced.json";
    std::string jsonFormat = nlm::Config::detectFileFormat(jsonPath);
    std::cout << "JSON file format: " << jsonFormat << std::endl;
    
    // Test YAML format detection
    std::string yamlPath = "configs/default_enhanced.yaml";
    std::string yamlFormat = nlm::Config::detectFileFormat(yamlPath);
    std::cout << "YAML file format: " << yamlFormat << std::endl;
    
    // Test key-value format detection
    std::string cfgPath = "configs/default.cfg";
    std::string cfgFormat = nlm::Config::detectFileFormat(cfgPath);
    std::cout << "Key-value file format: " << cfgFormat << std::endl;
    
    std::cout << "" << std::endl;
}

void demo_loading_from_different_formats() {
    std::cout << "=== Loading from Different Formats Demo ===" << std::endl;
    
    nlm::Config config;
    
    // Load from JSON
    std::string jsonPath = "configs/default_enhanced.json";
    bool jsonLoaded = config.loadFromFile(jsonPath);
    std::cout << "JSON loaded: " << (jsonLoaded ? "Yes" : "No") << std::endl;
    std::cout << "JSON summary: " << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Create a new config and load from YAML
    nlm::Config config2;
    std::string yamlPath = "configs/default_enhanced.yaml";
    bool yamlLoaded = config2.loadFromFile(yamlPath);
    std::cout << "YAML loaded: " << (yamlLoaded ? "Yes" : "No") << std::endl;
    std::cout << "YAML summary: " << std::endl;
    std::cout << config2.summary() << std::endl;
    
    // Create a new config and load from key-value
    nlm::Config config3;
    std::string cfgPath = "configs/default.cfg";
    bool cfgLoaded = config3.loadFromFile(cfgPath);
    std::cout << "Key-value loaded: " << (cfgLoaded ? "Yes" : "No") << std::endl;
    std::cout << "Key-value summary: " << std::endl;
    std::cout << config3.summary() << std::endl;
    
    std::cout << "" << std::endl;
}

void demo_configuration_groups() {
    std::cout << "=== Configuration Groups Demo ===" << std::endl;
    
    nlm::Config config;
    
    // Load from JSON and explore groups
    config.loadFromFile("configs/default_enhanced.json");
    
    std::cout << "All groups: ";
    auto groups = config.getAllGroups();
    for (const auto& group : groups) {
        std::cout << group << " ";
    }
    std::cout << std::endl;
    
    // Show configuration in specific groups
    if (config.has("brain.neuron_count")) {
        std::cout << "Brain group keys: ";
        auto brainKeys = config.getKeysInGroup("brain");
        for (const auto& key : brainKeys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
    }
    
    if (config.has("plasticity.stdp_ltp_weight")) {
        std::cout << "Plasticity group keys: ";
        auto plasticityKeys = config.getKeysInGroup("plasticity");
        for (const auto& key : plasticityKeys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "" << std::endl;
}

void demo_configuration_validation() {
    std::cout << "=== Configuration Validation Demo ===" << std::endl;
    
    nlm::Config config;
    
    // Load from JSON
    config.loadFromFile("configs/default_enhanced.json");
    
    // Create a schema
    nlm::ConfigSchema schema("production_schema", "Production configuration schema");
    
    // Add some required entries to schema
    nlm::ConfigEntry brainNeuronCount("brain.neuron_count", nlm::ConfigValue(500), nlm::ConfigSource::Default, 
                                      "Number of neurons", "int", "500", true);
    schema.addEntry(brainNeuronCount);
    
    nlm::ConfigEntry plasticityLTP("plasticity.stdp_ltp_weight", nlm::ConfigValue(0.02), nlm::ConfigSource::Default,
                                   "STDP LTP weight", "double", "0.02", true);
    schema.addEntry(plasticityLTP);
    
    // Validate the configuration
    bool isValid = schema.validate(config);
    std::cout << "Configuration validation: " << (isValid ? "Passed" : "Failed") << std::endl;
    
    if (!isValid) {
        std::vector<std::string> errors = schema.getValidationErrors(config);
        std::cout << "Validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    std::cout << "" << std::endl;
}

void demo_configuration_comparison() {
    std::cout << "=== Configuration Comparison Demo ===" << std::endl;
    
    nlm::Config config1;
    config1.loadFromFile("configs/default_enhanced.json");
    
    nlm::Config config2;
    config2.loadFromFile("configs/default_enhanced.yaml");
    
    // Compare configurations
    bool areEqual = (config1 == config2);
    std::cout << "Configurations are equal: " << (areEqual ? "Yes" : "No") << std::endl;
    
    // Show differences
    nlm::Config diff = config1.diff(config2);
    std::cout << "Differences:" << std::endl;
    std::cout << diff.summary() << std::endl;
    
    // Merge configurations
    nlm::Config merged = config1.merge(config2);
    std::cout << "Merged configuration size: " << merged.getKeys().size() << " entries" << std::endl;
    
    std::cout << "" << std::endl;
}

void demo_configuration_output() {
    std::cout << "=== Configuration Output Demo ===" << std::endl;
    
    nlm::Config config;
    config.loadFromFile("configs/default_enhanced.json");
    
    // Save to JSON
    config.saveToFile("output/demo_config.json");
    std::cout << "Saved to JSON: output/demo_config.json" << std::endl;
    
    // Save to YAML
    config.saveToFile("output/demo_config.yaml");
    std::cout << "Saved to YAML: output/demo_config.yaml" << std::endl;
    
    // Save to key-value format
    config.saveToFile("output/demo_config.cfg");
    std::cout << "Saved to key-value: output/demo_config.cfg" << std::endl;
    
    // Show documentation
    std::cout << "Documentation:" << std::endl;
    std::cout << config.getDocumentation() << std::endl;
    
    std::cout << "" << std::endl;
}

void demo_environment_configurations() {
    std::cout << "=== Environment Configurations Demo ===" << std::endl;
    
    nlm::Config config;
    
    // Set environment to production
    config.setEnvironment("production");
    std::cout << "Environment set to: " << config.getEnvironment() << std::endl;
    
    // Load production configuration
    bool productionLoaded = config.loadEnvironmentConfig("production");
    std::cout << "Production configuration loaded: " << (productionLoaded ? "Yes" : "No") << std::endl;
    
    // Set environment to development
    config.setEnvironment("development");
    std::cout << "Environment set to: " << config.getEnvironment() << std::endl;
    
    // Load development configuration
    bool developmentLoaded = config.loadEnvironmentConfig("development");
    std::cout << "Development configuration loaded: " << (developmentLoaded ? "Yes" : "No") << std::endl;
    
    std::cout << "" << std::endl;
}

int main() {
    std::cout << "=== Enhanced Configuration System Demonstration ===" << std::endl;
    std::cout << "This demo shows the new features of the enhanced configuration system." << std::endl;
    std::cout << "" << std::endl;
    
    // Create output directory
    std::filesystem::create_directories("output");
    
    // Run all demonstrations
    demo_file_format_detection();
    demo_loading_from_different_formats();
    demo_configuration_groups();
    demo_configuration_validation();
    demo_configuration_comparison();
    demo_configuration_output();
    demo_environment_configurations();
    
    std::cout << "=== Demonstration Complete ===" << std::endl;
    std::cout << "The enhanced configuration system supports:" << std::endl;
    std::cout << "- Multiple file formats (JSON, YAML, key-value)" << std::endl;
    std::cout << "- Configuration groups and organization" << std::endl;
    std::cout << "- Schema validation" << std::endl;
    std::cout << "- Configuration comparison and merging" << std::endl;
    std::cout << "- Environment-specific configurations" << std::endl;
    std::cout << "- Rich output formats" << std::endl;
    std::cout << "" << std::endl;
    
    return 0;
}
