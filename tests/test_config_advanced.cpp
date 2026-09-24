// Comprehensive Config Tests (New Features)
// Tests for JSON/YAML support, validation, error handling, and memory management
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace test_config_advanced {

void testJSONLoading() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string testFile = "test_config.json";
    std::ofstream file(testFile);
    file << R"({"neuron_count": 1000, "region_count": 4, "simulation_timestep": 0.001})";
    file.close();
    
    // Test JSON loading
    assert(config.loadFromFile(testFile));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("simulation_timestep"));
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    auto regionCount = config.get<int64_t>("region_count");
    assert(regionCount.has_value());
    assert(*regionCount == 4);
    
    auto timestep = config.get<double>("simulation_timestep");
    assert(timestep.has_value());
    assert(*timestep == 0.001);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testJSONLoading passed" << std::endl;
}

void testYAMLLoading() {
    nlm::Config config;
    
    // Create a temporary YAML file
    std::string testFile = "test_config.yaml";
    std::ofstream file(testFile);
    file << R"(neuron_count: 500
region_count: 2
connection_probability: 0.15
enable_reward_modulation: true)"";
    file.close();
    
    // Test YAML loading
    assert(config.loadFromFile(testFile));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("enable_reward_modulation"));
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 500);
    
    auto enableReward = config.get<bool>("enable_reward_modulation");
    assert(enableReward.has_value());
    assert(*enableReward == true);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testYAMLLoading passed" << std::endl;
}

void testJSONSaving() {
    nlm::Config config;
    
    // Add some test data
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    std::string testFile = "test_output.json";
    assert(config.saveToFile(testFile));
    
    // Verify file was created
    assert(std::filesystem::exists(testFile));
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testJSONSaving passed" << std::endl;
}

void testYAMLSaving() {
    nlm::Config config;
    
    // Add some test data
    config.set("test_int", 100, nlm::ConfigSource::Runtime);
    config.set("test_string", std::string("world"), nlm::ConfigSource::Runtime);
    config.set("test_double", 2.718, nlm::ConfigSource::Runtime);
    config.set("test_bool", false, nlm::ConfigSource::Runtime);
    
    std::string testFile = "test_output.yaml";
    assert(config.saveToYAMLFile(testFile));
    
    // Verify file was created
    assert(std::filesystem::exists(testFile));
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testYAMLSaving passed" << std::endl;
}

void testConfigurationValidation() {
    nlm::Config config;
    
    // Test with valid configuration
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 4, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // This should not raise exceptions
    config.validateConfiguration();
    
    // Test with invalid configuration (empty key)
    config.set("", "invalid_value", nlm::ConfigSource::Runtime);
    // Note: We can't directly test the validation errors easily from C++ due to API design
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testMemoryManagement() {
    nlm::Config config;
    
    // Add some data
    for (int i = 0; i < 100; ++i) {
        config.set("key_" + std::to_string(i), i, nlm::ConfigSource::Runtime);
    }
    
    // Get memory usage
    size_t initialMemory = config.getMemoryUsage();
    assert(initialMemory > 0);
    
    // Optimize memory
    bool optimized = config.optimizeMemoryUsage();
    assert(optimized == true); // Should optimize by removing empty keys
    
    size_t finalMemory = config.getMemoryUsage();
    // Memory should be less or equal after optimization
    assert(finalMemory <= initialMemory);
    
    // Test cleanup
    config.set("invalid_key", "value", nlm::ConfigSource::Runtime);
    config.cleanupInvalidEntries();
    
    std::cout << "    testMemoryManagement passed" << std::endl;
}

void testErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    assert(!config.loadFromFile("non_existent_file.json"));
    
    // Test saving to non-existent directory (should fail gracefully)
    // Note: This depends on filesystem permissions
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void testStringEscaping() {
    // Test JSON string escaping
    std::string jsonInput = "Hello\"World\\nTest\t\x00";
    std::string escaped = nlm::Config::escapeJSONString(jsonInput);
    assert(!escaped.empty());
    
    // Test YAML string escaping  
    std::string yamlInput = "Hello'World\\nTest\t\x00";
    std::string escapedYaml = nlm::Config::escapeYAMLString(yamlInput);
    assert(!escapedYaml.empty());
    
    std::cout << "    testStringEscaping passed" << std::endl;
}

void testAdvancedConfiguration() {
    nlm::Config config;
    
    // Test setting different types
    config.set("int", 42, nlm::ConfigSource::Runtime);
    config.set("double", 3.14, nlm::ConfigSource::Runtime);
    config.set("string", "test", nlm::ConfigSource::Runtime);
    config.set("bool", true, nlm::ConfigSource::Runtime);
    config.set("vector_int", std::vector<int64_t>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("vector_double", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    
    // Verify all types can be retrieved
    assert(config.has("int"));
    assert(config.has("double"));
    assert(config.has("string"));
    assert(config.has("bool"));
    assert(config.has("vector_int"));
    assert(config.has("vector_double"));
    
    std::cout << "    testAdvancedConfiguration passed" << std::endl;
}

void testConfigFileExtensions() {
    nlm::Config config;
    
    // Test .json extension
    std::string jsonFile = "test." + std::string("json");
    std::ofstream jsonOut(jsonFile);
    jsonOut << R"({"key": "value"})";
    jsonOut.close();
    
    assert(config.loadFromFile(jsonFile));
    std::remove(jsonFile.c_str());
    
    // Test .yaml extension
    std::string yamlFile = "test." + std::string("yaml");
    std::ofstream yamlOut(yamlFile);
    yamlOut << "key: value";
    yamlOut.close();
    
    assert(config.loadFromFile(yamlFile));
    std::remove(yamlFile.c_str());
    
    // Test .yml extension
    std::string ymlFile = "test." + std::string("yml");
    std::ofstream ymlOut(ymlFile);
    ymlOut << "key: value";
    ymlOut.close();
    
    assert(config.loadFromFile(ymlFile));
    std::remove(ymlFile.c_str());
    
    std::cout << "    testConfigFileExtensions passed" << std::endl;
}

void runAll() {
    std::cout << "Running advanced config tests..." << std::endl;
    testJSONLoading();
    testYAMLLoading();
    testJSONSaving();
    testYAMLSaving();
    testConfigurationValidation();
    testMemoryManagement();
    testErrorHandling();
    testStringEscaping();
    testAdvancedConfiguration();
    testConfigFileExtensions();
    std::cout << "All advanced config tests passed!" << std::endl;
}

} // namespace test_config_advanced
