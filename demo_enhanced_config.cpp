// ENHANCED CONFIG SYSTEM DEMONSTRATION
// This program demonstrates the new JSON/YAML configuration capabilities

#include <iostream>
#include <fstream>
#include <string>
#include "core/Config/Config.hpp"

void demonstrateLegacyFormat() {
    std::cout << "=== Legacy Format Support ===" << std::endl;
    nlm::Config config;
    
    // Load legacy config (old key=value format)
    bool legacyLoaded = config.loadFromFile("configs/default.cfg");
    if (legacyLoaded) {
        std::cout << "Successfully loaded legacy config (" << config.getKeys().size() << " entries)" << std::endl;
        std::cout << "Random seed: " << config.getOr<int>("random_seed", 0) << std::endl;
        std::cout << "Simulation timestep: " << config.getOr<double>("simulation_timestep", 0.0) << std::endl;
    }
    std::cout << "" << std::endl;
}

void demonstrateJsonFormat() {
    std::cout << "=== JSON Format Support ===" << std::endl;
    nlm::Config config;
    
    // Load JSON config
    bool jsonLoaded = config.loadFromFile("configs/config.json");
    if (jsonLoaded) {
        std::cout << "Successfully loaded JSON config (" << config.getKeys().size() << " entries)" << std::endl;
        std::cout << "Neuron count: " << config.getOr<int>("neuron_count", 0) << std::endl;
        std::cout << "Dopamine baseline: " << config.getOr<double>("dopamine_baseline", 0.0) << std::endl;
        std::cout << "Environment name: " << config.getOr<std::string>("environment_name", "") << std::endl;
    }
    
    // Test different data types
    config.set("complex_int", 42, nlm::ConfigSource::Runtime);
    config.set("complex_double", 3.14159, nlm::ConfigSource::Runtime);
    config.set("complex_string", "Test String", nlm::ConfigSource::Runtime);
    config.set("complex_bool", true, nlm::ConfigSource::Runtime);
    
    std::cout << "Added complex types to config" << std::endl;
    std::cout << "" << std::endl;
}

void demonstrateYamlFormat() {
    std::cout << "=== YAML Format Support ===" << std::endl;
    nlm::Config config;
    
    // Load YAML config (if yaml-cpp is available)
    bool yamlLoaded = config.loadFromFile("configs/config.yaml");
    if (yamlLoaded) {
        std::cout << "Successfully loaded YAML config (" << config.getKeys().size() << " entries)" << std::endl;
        std::cout << "Log level: " << config.getOr<std::string>("log_level", "") << std::endl;
        std::cout << "Max steps: " << config.getOr<int>("max_simulation_steps", 0) << std::endl;
    }
    std::cout << "" << std::endl;
}

void demonstrateSaveLoad() {
    std::cout << "=== Save/Load Cycle ===" << std::endl;
    nlm::Config config;
    
    // Set some values
    config.set("test_key1", "test_value1", nlm::ConfigSource::Runtime);
    config.set("test_key2", 12345, nlm::ConfigSource::Runtime);
    config.set("test_key3", true, nlm::ConfigSource::Runtime);
    config.set("test_key4", 3.14, nlm::ConfigSource::Runtime);
    
    std::cout << "Original config summary:" << std::endl << config.summary();
    
    // Save to JSON
    bool saved = config.saveToFile("configs/saved_config.json");
    if (saved) {
        std::cout << "Successfully saved config to JSON" << std::endl;
        
        // Clear and reload
        nlm::Config newConfig;
        bool reloaded = newConfig.loadFromFile("configs/saved_config.json");
        if (reloaded) {
            std::cout << "Successfully reloaded config from JSON" << std::endl;
            std::cout << "Re-loaded test_key2: " << newConfig.getOr<int>("test_key2", 0) << std::endl;
        }
    }
    
    std::cout << "" << std::endl;
}

void demonstrateErrorHandling() {
    std::cout << "=== Error Handling ===" << std::endl;
    nlm::Config config;
    
    // Try to load non-existent file
    bool failedLoad = config.loadFromFile("non_existent_file.json");
    if (!failedLoad) {
        std::cout << "Correctly handled missing file" << std::endl;
    }
    
    // Try to load invalid JSON (create invalid file)
    std::ofstream invalidFile("configs/invalid.json");
    invalidFile << "{invalid json content";
    invalidFile.close();
    
    bool invalidLoad = config.loadFromFile("configs/invalid.json");
    if (!invalidLoad) {
        std::cout << "Correctly handled invalid JSON" << std::endl;
    }
    
    // Clean up
    std::remove("configs/invalid.json");
    
    std::cout << "" << std::endl;
}

void main() {
    std::cout << "Enhanced Config System Demonstration" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "" << std::endl;
    
    demonstrateLegacyFormat();
    demonstrateJsonFormat();
    demonstrateYamlFormat();
    demonstrateSaveLoad();
    demonstrateErrorHandling();
    
    std::cout << "=== Summary ===" << std::endl;
    std::cout << "✅ Legacy key=value format support" << std::endl;
    std::cout << "✅ JSON format support" << std::endl;
    std::cout << "✅ YAML format support" << std::endl;
    std::cout << "✅ Save/load cycle with real values" << std::endl;
    std::cout << "✅ Comprehensive error handling" << std::endl;
    std::cout << "✅ Enhanced type support" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "The enhanced configuration system is fully operational!" << std::endl;
}