// Enhanced Configuration Tests
// Tests for the new configuration features including JSON/YAML support, validation, and file formats

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace test_config_advanced {

void testConfigValidation() {
    std::cout << "  testConfigValidation passed" << std::endl;
    
    nlm::Config config;
    
    // Test with minimal valid config
    config.set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    auto errors = config.validate();
    assert(errors.empty()); // Should have no errors
}

void testConfigValidationErrors() {
    std::cout << "  testConfigValidationErrors passed" << std::endl;
    
    nlm::Config config;
    
    // Test with invalid neuron_count (too low)
    config.set("neuron_count", static_cast<int64_t>(5), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    auto errors = config.validate();
    assert(!errors.empty());
    assert(errors[0].find("neuron_count") != std::string::npos);
}

void testJSONFileFormat() {
    std::cout << "  testJSONFileFormat passed" << std::endl;
    
    nlm::Config config;
    
    // Create test JSON file
    std::string jsonFile = "test_config.json";
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Save to JSON
    assert(config.saveToJSON(jsonFile));
    
    // Load from JSON
    nlm::Config config2;
    assert(config2.loadFromFile(jsonFile));
    
    // Verify values
    assert(config2.getOr<size_t>("neuron_count", 0) == 100);
    assert(config2.getOr<size_t>("region_count", 0) == 2);
    assert(config2.getOr<float>("connection_probability", 0.0f) == 0.15f);
    
    // Clean up
    std::remove(jsonFile.c_str());
}

void testYAMLFileFormat() {
    std::cout << "  testYAMLFileFormat passed" << std::endl;
    
    nlm::Config config;
    
    // Create test YAML file
    std::string yamlFile = "test_config.yaml";
    config.set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.002, nlm::ConfigSource::Default);
    
    // Save to YAML
    assert(config.saveToYAML(yamlFile));
    
    // Load from YAML
    nlm::Config config2;
    assert(config2.loadFromFile(yamlFile));
    
    // Verify values
    assert(config2.getOr<size_t>("neuron_count", 0) == 200);
    assert(config2.getOr<size_t>("region_count", 0) == 3);
    assert(config2.getOr<double>("simulation_timestep", 0.0) == 0.002);
    
    // Clean up
    std::remove(yamlFile.c_str());
}

void testFileFormatDetection() {
    std::cout << "  testFileFormatDetection passed" << std::endl;
    
    // Test JSON detection
    assert(nlm::Config::detectFileFormat("config.json") == "json");
    assert(nlm::Config::detectFileFormat("path/to/config.json") == "json");
    
    // Test YAML detection
    assert(nlm::Config::detectFileFormat("config.yaml") == "yaml");
    assert(nlm::Config::detectFileFormat("path/to/config.yml") == "yaml");
    
    // Test simple format detection
    assert(nlm::Config::detectFileFormat("config.cfg") == "simple");
    assert(nlm::Config::detectFileFormat("config.txt") == "simple");
    assert(nlm::Config::detectFileFormat("no_extension") == "simple");
}

void testConfigToJSONSave() {
    std::cout << "  testConfigToJSONSave passed" << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Default);
    config.set("test_bool", true, nlm::ConfigSource::Default);
    config.set("test_number", 42.5, nlm::ConfigSource::Default);
    
    std::string jsonFile = "test_config_json.json";
    assert(config.saveToJSON(jsonFile));
    
    // Verify file was created and has valid JSON content
    std::ifstream file(jsonFile);
    assert(file.is_open());
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    assert(content.find("neuron_count") != std::string::npos);
    assert(content.find("region_count") != std::string::npos);
    assert(content.find("50") != std::string::npos);
    
    file.close();
    std::remove(jsonFile.c_str());
}

void testConfigToYAMLSave() {
    std::cout << "  testConfigToYAMLSave passed" << std::endl;
    
    nlm::Config config;
    config.set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    std::string yamlFile = "test_config_yaml.yaml";
    assert(config.saveToYAML(yamlFile));
    
    // Verify file was created
    std::ifstream file(yamlFile);
    assert(file.is_open());
    
    // Check for YAML headers
    std::string line;
    std::getline(file, line);
    assert(line.find("# NLM Configuration") != std::string::npos);
    
    file.close();
    std::remove(yamlFile.c_str());
}

void runAll() {
    testConfigValidation();
    testConfigValidationErrors();
    testJSONFileFormat();
    testYAMLFileFormat();
    testFileFormatDetection();
    testConfigToJSONSave();
    testConfigToYAMLSave();
    
    std::cout << "=== All Enhanced Configuration Tests Passed ===" << std::endl;
}

} // namespace test_config_advanced
