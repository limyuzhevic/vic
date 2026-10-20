// JSON Configuration Tests
// Tests for JSON configuration file support in the NLM configuration system

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace test_config_json {

void testJsonFileDetection() {
    nlm::Config config;
    
    // Test that JSON file detection works
    assert(config.isJsonFile("configs/nlm_config.json"));
    assert(!config.isJsonFile("configs/nlm_config.yaml"));
    assert(!config.isJsonFile("configs/default.cfg"));
    
    std::cout << "    testJsonFileDetection passed" << std::endl;
}

void testLoadFromJson() {
    nlm::Config config;
    
    // Load JSON configuration
    assert(config.loadFromFile("configs/nlm_config.json"));
    
    // Verify JSON-specific values
    assert(config.has("brain.neuron_count"));
    assert(config.has("plasticity.stdp.learning_rate"));
    assert(config.has("simulation.max_steps"));
    
    // Verify JSON data types
    auto neuronCount = config.getOr<int>("brain.neuron_count", 0);
    assert(neuronCount == 2000);
    
    auto learningRate = config.getOr<double>("plasticity.stdp.learning_rate", 0.0);
    assert(learningRate > 0.0 && learningRate < 1.0);
    
    auto maxSteps = config.getOr<uint64_t>("simulation.max_steps", 0);
    assert(maxSteps == 10000);
    
    std::cout << "    testLoadFromJson passed" << std::endl;
}

void testJsonNestedAccess() {
    nlm::Config config;
    
    assert(config.loadFromFile("configs/nlm_config.json"));
    
    // Test nested key access
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.region_count"));
    assert(config.has("plasticity.stdp.learning_rate"));
    assert(config.has("plasticity.stdp.tau_plus"));
    
    // Test set nested values
    config.setNested("brain.neuron_count", 3000);
    assert(config.getOr<int>("brain.neuron_count", 0) == 3000);
    
    // Test that other values remain unchanged
    assert(config.getOr<int>("brain.region_count", 0) == 2);
    
    std::cout << "    testJsonNestedAccess passed" << std::endl;
}

void testJsonSaveToJson() {
    nlm::Config config;
    
    // Load existing config
    assert(config.loadFromFile("configs/nlm_config.json"));
    
    // Save to JSON format
    std::string jsonString = config.saveToJsonString();
    
    // Parse the JSON string and verify it's valid
    auto parsedJson = nlohmann::json::parse(jsonString);
    
    // Verify some key values are preserved
    assert(parsedJson.contains("brain"));
    assert(parsedJson["brain"].contains("neuron_count"));
    assert(parsedJson["brain"]["neuron_count"] == 2000);
    
    std::cout << "    testJsonSaveToJson passed" << std::endl;
}

void testJsonValidation() {
    nlm::Config config;
    
    // Test schema validation with JSON config
    config.loadFromFile("configs/nlm_config.json");
    
    // Create a simple schema
    nlm::ConfigSchema schema;
    nlm::ConfigSchema::ConfigProperty neuronCountProp {
        "brain.neuron_count",
        "int",
        1000,
        [](const std::string&, const nlm::ConfigValue& value) {
            int count = std::get<int>(value);
            return count > 0 && count <= 10000;
        },
        "Number of neurons (must be 1-10000)"
    };
    
    schema.addProperty(neuronCountProp);
    config.setSchema(schema);
    
    // Validation should pass for valid values
    assert(config.validate("brain.neuron_count", 2000));
    
    std::cout << "    testJsonValidation passed" << std::endl;
}

void testJsonErrorHandling() {
    nlm::Config config;
    
    // Try to load non-existent JSON file
    assert(!config.loadFromFile("configs/nonexistent.json"));
    
    // Try to load malformed JSON file
    std::ofstream badFile("configs/bad.json");
    badFile << "{\"brain\": {\"neuron_count\": invalid}}"; // Invalid JSON
    badFile.close();
    
    assert(!config.loadFromFile("configs/bad.json"));
    
    // Clean up
    std::remove("configs/bad.json");
    
    std::cout << "    testJsonErrorHandling passed" << std::endl;
}

void runAll() {
    testJsonFileDetection();
    testLoadFromJson();
    testJsonNestedAccess();
    testJsonSaveToJson();
    testJsonValidation();
    testJsonErrorHandling();
}

} // namespace test_config_json

// Simple test for JSON format
int main() {
    std::cout << "=== NLM JSON Configuration Tests ===" << std::endl;
    test_config_json::runAll();
    std::cout << "=== All JSON configuration tests passed! ===" << std::endl;
    return 0;
}