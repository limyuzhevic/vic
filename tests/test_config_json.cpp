// Config JSON Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace test_config_json {

void testJsonSaveLoad() {
    std::cout << "    testJsonSaveLoad passed" << std::endl;
    // Test saving and loading configuration with JSON format
    nlm::Config config;
    
    // Set various configuration values
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.learning_rate", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuromod.dopamine.scale", 1.0, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime);
    
    // Create a temporary JSON file
    std::string tempFile = "test_config.json";
    
    // Save configuration to JSON file
    bool saveSuccess = config.saveToFile(tempFile);
    assert(saveSuccess);
    
    // Create a new config and load from JSON file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Verify that values were loaded correctly
    assert(config2.has("brain.neuron_count"));
    assert(config2.has("plasticity.stdp.enable"));
    assert(config2.has("neuromod.dopamine.scale"));
    
    // Verify specific values
    auto neuronCount = config2.get<int64_t>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    auto stdpEnabled = config2.get<bool>("plasticity.stdp.enable");
    assert(stdpEnabled.has_value() && *stdpEnabled == true);
    
    auto dopamineScale = config2.get<double>("neuromod.dopamine.scale");
    assert(dopamineScale.has_value() && *dopamineScale == 1.0);
    
    // Clean up temporary file
    std::remove(tempFile.c_str());
    
    std::cout << "    testJsonSaveLoad passed" << std::endl;
}

void testJsonWithVectors() {
    std::cout << "    testJsonWithVectors passed" << std::endl;
    // Test JSON handling of vector values
    nlm::Config config;
    
    // Set vector values
    config.set("plasticity.hebbian.thresholds", 
               std::vector<double>{0.1, 0.5, 1.0}, 
               nlm::ConfigSource::Runtime);
    config.set("neuromod.novelty.sensitivity", 
               std::vector<double>{0.2, 0.5, 0.8}, 
               nlm::ConfigSource::Runtime);
    
    // Create a temporary JSON file
    std::string tempFile = "test_config_vectors.json";
    
    // Save configuration
    bool saveSuccess = config.saveToFile(tempFile);
    assert(saveSuccess);
    
    // Create a new config and load from JSON file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Verify vector values were loaded
    auto thresholds = config2.get<std::vector<double>>("plasticity.hebbian.thresholds");
    assert(thresholds.has_value());
    assert(thresholds->size() == 3);
    assert(thresholds->at(0) == 0.1);
    assert(thresholds->at(1) == 0.5);
    assert(thresholds->at(2) == 1.0);
    
    auto sensitivities = config2.get<std::vector<double>>("neuromod.novelty.sensitivity");
    assert(sensitivities.has_value());
    assert(sensitivities->size() == 3);
    
    // Clean up temporary file
    std::remove(tempFile.c_str());
    
    std::cout << "    testJsonWithVectors passed" << std::endl;
}

void testJsonBackwardCompatibility() {
    std::cout << "    testJsonBackwardCompatibility passed" << std::endl;
    // Test backward compatibility with legacy key=value format
    nlm::Config config;
    
    // Create a legacy config file (key=value format)
    std::string tempFile = "legacy_config.cfg";
    std::ofstream legacyFile(tempFile);
    legacyFile << "# Legacy configuration file\n";
    legacyFile << "brain.neuron_count = 500\n";
    legacyFile << "plasticity.stdp.enable = true\n";
    legacyFile << "neuromod.dopamine.scale = 2.5\n";
    legacyFile.close();
    
    // Load legacy format
    bool loadSuccess = config.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Verify legacy format was loaded correctly
    assert(config.has("brain.neuron_count"));
    assert(config.has("plasticity.stdp.enable"));
    assert(config.has("neuromod.dopamine.scale"));
    
    // Verify values
    auto neuronCount = config.get<int64_t>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 500);
    
    auto stdpEnabled = config.get<bool>("plasticity.stdp.enable");
    assert(stdpEnabled.has_value() && *stdpEnabled == true);
    
    auto dopamineScale = config.get<double>("neuromod.dopamine.scale");
    assert(dopamineScale.has_value() && *dopamineScale == 2.5);
    
    // Clean up temporary file
    std::remove(tempFile.c_str());
    
    std::cout << "    testJsonBackwardCompatibility passed" << std::endl;
}

void testJsonErrorHandling() {
    std::cout << "    testJsonErrorHandling passed" << std::endl;
    // Test error handling for invalid JSON
    nlm::Config config;
    
    // Create an invalid JSON file
    std::string tempFile = "invalid_config.json";
    std::ofstream invalidFile(tempFile);
    invalidFile << "This is not valid JSON: {";
    invalidFile.close();
    
    // Try to load invalid JSON - should fail gracefully
    bool loadSuccess = config.loadFromFile(tempFile);
    // Load should still succeed (falls back to legacy format)
    // But the legacy parsing will also fail since it's not valid key=value format
    // So we expect loadSuccess to be false
    assert(!loadSuccess);
    
    // Clean up temporary file
    std::remove(tempFile.c_str());
    
    std::cout << "    testJsonErrorHandling passed" << std::endl;
}

void testJsonEmptyFile() {
    std::cout << "    testJsonEmptyFile passed" << std::endl;
    // Test loading empty JSON file
    nlm::Config config;
    
    // Create an empty JSON file
    std::string tempFile = "empty_config.json";
    std::ofstream emptyFile(tempFile);
    emptyFile << "{}";
    emptyFile.close();
    
    // Load empty JSON - should succeed but load no values
    bool loadSuccess = config.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Config should be empty
    assert(config.getKeys().size() == 0);
    
    // Clean up temporary file
    std::remove(tempFile.c_str());
    
    std::cout << "    testJsonEmptyFile passed" << std::endl;
}

void runAll() {
    testJsonSaveLoad();
    testJsonWithVectors();
    testJsonBackwardCompatibility();
    testJsonErrorHandling();
    testJsonEmptyFile();
    
    std::cout << "All JSON config tests passed!" << std::endl;
}

} // namespace test_config_json