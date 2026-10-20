// YAML Configuration Tests
// Tests for YAML configuration file support in the NLM configuration system

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace test_config_yaml {

void testYamlFileDetection() {
    nlm::Config config;
    
    // Test that YAML file detection works
    assert(config.isYamlFile("configs/nlm_config.yaml"));
    assert(!config.isYamlFile("configs/nlm_config.json"));
    assert(!config.isYamlFile("configs/default.cfg"));
    
    std::cout << "    testYamlFileDetection passed" << std::endl;
}

void testLoadFromYaml() {
    nlm::Config config;
    
    // Load YAML configuration
    assert(config.loadFromFile("configs/nlm_config.yaml"));
    
    // Verify YAML-specific values
    assert(config.has("brain.neuron_count"));
    assert(config.has("plasticity.stdp.learning_rate"));
    assert(config.has("simulation.max_steps"));
    
    // Verify YAML data types
    auto neuronCount = config.getOr<int>("brain.neuron_count", 0);
    assert(neuronCount == 1000);
    
    auto learningRate = config.getOr<double>("plasticity.stdp.learning_rate", 0.0);
    assert(learningRate > 0.0 && learningRate < 1.0);
    
    auto maxSteps = config.getOr<uint64_t>("simulation.max_steps", 0);
    assert(maxSteps == 10000);
    
    std::cout << "    testLoadFromYaml passed" << std::endl;
}

void testYamlNestedAccess() {
    nlm::Config config;
    
    assert(config.loadFromFile("configs/nlm_config.yaml"));
    
    // Test nested key access
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.region_count"));
    assert(config.has("plasticity.stdp.learning_rate"));
    assert(config.has("plasticity.stdp.tau_plus"));
    
    // Test set nested values
    config.setNested("brain.neuron_count", 2000);
    assert(config.getOr<int>("brain.neuron_count", 0) == 2000);
    
    // Test that other values remain unchanged
    assert(config.getOr<int>("brain.region_count", 0) == 1);
    
    std::cout << "    testYamlNestedAccess passed" << std::endl;
}

void testYamlSaveToYaml() {
    nlm::Config config;
    
    // Load existing config
    assert(config.loadFromFile("configs/nlm_config.yaml"));
    
    // Save to YAML format
    std::string yamlString = config.saveToYamlString();
    
    // Parse the YAML string and verify it's valid
    auto parsedYaml = YAML::Load(yamlString);
    
    // Verify some key values are preserved
    assert(parsedYaml["brain"].IsDefined());
    assert(parsedYaml["brain"]["neuron_count"].IsDefined());
    assert(parsedYaml["brain"]["neuron_count"].as<int>() == 1000);
    
    std::cout << "    testYamlSaveToYaml passed" << std::endl;
}

void testYamlValidation() {
    nlm::Config config;
    
    // Test schema validation with YAML config
    config.loadFromFile("configs/nlm_config.yaml");
    
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
    assert(config.validate("brain.neuron_count", 1000));
    
    std::cout << "    testYamlValidation passed" << std::endl;
}

void testYamlErrorHandling() {
    nlm::Config config;
    
    // Try to load non-existent YAML file
    assert(!config.loadFromFile("configs/nonexistent.yaml"));
    
    // Try to load malformed YAML file
    std::ofstream badFile("configs/bad.yaml");
    badFile << "brain:\n  neuron_count: invalid\n  region_count:"; // Invalid YAML
    badFile.close();
    
    assert(!config.loadFromFile("configs/bad.yaml"));
    
    // Clean up
    std::remove("configs/bad.yaml");
    
    std::cout << "    testYamlErrorHandling passed" << std::endl;
}

void runAll() {
    testYamlFileDetection();
    testLoadFromYaml();
    testYamlNestedAccess();
    testYamlSaveToYaml();
    testYamlValidation();
    testYamlErrorHandling();
}

} // namespace test_config_yaml

// Simple test for YAML format
int main() {
    std::cout << "=== NLM YAML Configuration Tests ===" << std::endl;
    test_config_yaml::runAll();
    std::cout << "=== All YAML configuration tests passed! ===" << std::endl;
    return 0;
}