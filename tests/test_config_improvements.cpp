// Configuration tests
#include <cassert>
#include <filesystem>
#include <fstream>
#include "../src/core/Config/Config.hpp"

void testConfigKeyValueFormat() {
    nlm::Config config;
    
    // Write a simple key=value file
    std::ofstream testFile("test_config.txt");
    testFile << "neuron_count = 1000\n";
    testFile << "region_count = 2\n";
    testFile << "connection_probability = 0.15\n";
    testFile << "# Comment line\n";
    testFile << "stdp_ltp_weight = 0.01\n";
    testFile << "enabled = true\n";
    testFile << "rate = 0.5\n";
    testFile.close();
    
    // Load and verify
    assert(config.loadFromFile("test_config.txt"));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("stdp_ltp_weight"));
    assert(config.has("enabled"));
    assert(config.has("rate"));
    
    // Check value types
    assert(config.get<int64_t>("neuron_count") == 1000);
    assert(config.get<int64_t>("region_count") == 2);
    assert(config.get<float>("connection_probability") == 0.15f);
    assert(config.get<float>("stdp_ltp_weight") == 0.01f);
    assert(config.get<bool>("enabled") == true);
    assert(config.get<double>("rate") == 0.5);
    
    std::remove("test_config.txt");
    std::cout << "✓ Config key=value format test passed" << std::endl;
}

void testConfigJSONFormat() {
    nlm::Config config;
    
    // Write a JSON file
    std::ofstream jsonFile("test_config.json");
    jsonFile << "{\n";
    jsonFile << "  \"brain\": {\n";
    jsonFile << "    \"neuron_count\": 2000,\n";
    jsonFile << "    \"region_count\": 4,\n";
    jsonFile << "    \"connection_probability\": 0.2\n";
    jsonFile << "  },\n";
    jsonFile << "  \"plasticity\": {\n";
    jsonFile << "    \"stdp_ltp_weight\": 0.02,\n";
    jsonFile << "    \"stdp_ltd_weight\": 0.018\n";
    jsonFile << "  },\n";
    jsonFile << "  \"neuromodulation\": {\n";
    jsonFile << "    \"dopamine_scale\": 1.5,\n";
    jsonFile << "    \"curiosity_enabled\": true\n";
    jsonFile << "  }\n";
    jsonFile << "}\n";
    jsonFile.close();
    
    // Load and verify
    assert(config.loadFromFile("test_config.json"));
    
    // Check nested values (they should be accessible with dot notation)
    // Note: JSON parsing uses dot notation for nested access
    assert(config.has("brain.neuron_count"));
    assert(config.get<int64_t>("brain.neuron_count") == 2000);
    assert(config.has("brain.region_count"));
    assert(config.get<int64_t>("brain.region_count") == 4);
    assert(config.has("brain.connection_probability"));
    assert(config.get<float>("brain.connection_probability") == 0.2f);
    
    assert(config.has("plasticity.stdp_ltp_weight"));
    assert(config.get<float>("plasticity.stdp_ltp_weight") == 0.02f);
    assert(config.has("plasticity.stdp_ltd_weight"));
    assert(config.get<float>("plasticity.stdp_ltd_weight") == 0.018f);
    
    assert(config.has("neuromodulation.dopamine_scale"));
    assert(config.get<float>("neuromodulation.dopamine_scale") == 1.5f);
    assert(config.has("neuromodulation.curiosity_enabled"));
    assert(config.get<bool>("neuromodulation.curiosity_enabled") == true);
    
    std::remove("test_config.json");
    std::cout << "✓ Config JSON format test passed" << std::endl;
}

void testConfigYAMLFormat() {
    nlm::Config config;
    
    // Write a YAML file
    std::ofstream yamlFile("test_config.yaml");
    yamlFile << "brain:\n";
    yamlFile << "  neuron_count: 3000\n";
    yamlFile << "  region_count: 6\n";
    yamlFile << "  connection_probability: 0.25\n";
    yamlFile << "plasticity:\n";
    yamlFile << "  stdp_ltp_weight: 0.03\n";
    yamlFile << "  stdp_ltd_weight: 0.025\n";
    yamlFile << "neuromodulation:\n";
    yamlFile << "  dopamine_scale: 1.8\n";
    yamlFile << "  curiosity_enabled: true\n";
    yamlFile.close();
    
    // Load and verify
    assert(config.loadFromFile("test_config.yaml"));
    
    // Check nested values (YAML uses dot notation for nested access)
    assert(config.has("brain.neuron_count"));
    assert(config.get<int64_t>("brain.neuron_count") == 3000);
    assert(config.has("brain.region_count"));
    assert(config.get<int64_t>("brain.region_count") == 6);
    assert(config.has("brain.connection_probability"));
    assert(config.get<float>("brain.connection_probability") == 0.25f);
    
    assert(config.has("plasticity.stdp_ltp_weight"));
    assert(config.get<float>("plasticity.stdp_ltp_weight") == 0.03f);
    assert(config.has("plasticity.stdp_ltd_weight"));
    assert(config.get<float>("plasticity.stdp_ltd_weight") == 0.025f);
    
    assert(config.has("neuromodulation.dopamine_scale"));
    assert(config.get<float>("neuromodulation.dopamine_scale") == 1.8f);
    assert(config.has("neuromodulation.curiosity_enabled"));
    assert(config.get<bool>("neuromodulation.curiosity_enabled") == true);
    
    std::remove("test_config.yaml");
    std::cout << "✓ Config YAML format test passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    assert(!config.loadFromFile("non_existent_file.xyz"));
    
    // Test loading malformed JSON
    std::ofstream badJson("bad_config.json");
    badJson << "{\n";
    badJson << "  \"invalid json: missing quotes\"\n";
    badJson << "}\n";
    badJson.close();
    
    assert(!config.loadFromFile("bad_config.json"));
    std::remove("bad_config.json");
    
    // Test loading malformed YAML
    std::ofstream badYaml("bad_config.yaml");
    badYaml << "brain:\n";
    badYaml << "  neuron_count: 1000\n";
    badYaml << "  region_count: : 2\n";  // Invalid YAML
    badYaml.close();
    
    assert(!config.loadFromFile("bad_config.yaml"));
    std::remove("bad_config.yaml");
    
    std::cout << "✓ Config error handling test passed" << std::endl;
}

void testConfigDefaultValues() {
    nlm::Config config;
    
    // Set a value
    config.set("test_value", 42, nlm::ConfigSource::Runtime);
    
    // Check that getOr returns the value
    assert(config.getOr<int64_t>("test_value", 0) == 42);
    
    // Check that non-existent key returns default
    assert(config.getOr<int64_t>("non_existent", 100) == 100);
    
    // Test that has works correctly
    assert(config.has("test_value"));
    assert(!config.has("another_value"));
    
    std::cout << "✓ Config default values test passed" << std::endl;
}

int main() {
    try {
        testConfigKeyValueFormat();
        testConfigJSONFormat();
        testConfigYAMLFormat();
        testConfigErrorHandling();
        testConfigDefaultValues();
        
        std::cout << "\n=== All Configuration Tests Passed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}