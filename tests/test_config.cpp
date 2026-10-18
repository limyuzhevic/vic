// Enhanced Config Tests for NLM Configuration System
// Tests for JSON/YAML support, type safety, validation, and enhanced features

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

namespace test_config {

// Test basic configuration creation
void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

// Test enhanced type-safe configuration methods
void testTypeSafeConfiguration() {
    nlm::Config config;
    
    // Test enhanced getters
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.getInt("int_value", 0) == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    assert(config.getDouble("double_value", 0.0) > 3.13 && 
           config.getDouble("double_value", 0.0) < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    assert(config.getString("string_value", "") == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    assert(config.getBool("bool_value", false) == true);
    
    // Test vector types
    std::vector<int> intList = {1, 2, 3, 4, 5};
    config.set("int_vector", intList, nlm::ConfigSource::Runtime);
    
    std::vector<double> doubleList = {1.1, 2.2, 3.3};
    config.set("double_vector", doubleList, nlm::ConfigSource::Runtime);
    
    std::vector<std::string> stringList = {"a", "b", "c"};
    config.set("string_vector", stringList, nlm::ConfigSource::Runtime);
    
    std::cout << "    testTypeSafeConfiguration passed" << std::endl;
}

// Test JSON configuration loading
void testJSONConfiguration() {
    nlm::Config config;
    
    // Create JSON content
    std::string jsonContent = R"({
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.15,
        "plasticity.stdp.enable": true,
        "neuromodulation.reward.scale": 1.0,
        "visualization.enabled": false
    })";
    
    // Test loading from JSON string
    bool success = config.loadFromJSON(jsonContent);
    assert(success);
    
    // Verify values
    assert(config.getOr<int>("brain.neuron_count", 0) == 1000);
    assert(config.getOr<float>("brain.synapse_density", 0.0f) == 0.15f);
    assert(config.getOr<bool>("plasticity.stdp.enable", false) == true);
    
    std::cout << "    testJSONConfiguration passed" << std::endl;
}

// Test YAML configuration loading
void testYAMLConfiguration() {
    nlm::Config config;
    
    // Create YAML content
    std::string yamlContent = R"(brain:
  neuron_count: 500
  synapse_density: 0.1
plasticity:
  stdp:
    enable: true
neuromodulation:
  reward:
    scale: 1.0
visualization:
  enabled: false
)";
    
    // Test loading from YAML string
    bool success = config.loadFromYAML(yamlContent);
    assert(success);
    
    // Verify values
    assert(config.getOr<int>("brain.neuron_count", 0) == 500);
    assert(config.getOr<float>("brain.synapse_density", 0.0f) == 0.1f);
    assert(config.getOr<bool>("plasticity.stdp.enable", false) == true);
    
    std::cout << "    testYAMLConfiguration passed" << std::endl;
}

// Test simple configuration loading
void testSimpleConfiguration() {
    nlm::Config config;
    
    // Create simple content
    std::string simpleContent = R"(
# NLM Configuration
brain.neuron_count = 1000
brain.synapse_density = 0.15
plasticity.stdp.enable = true
neuromodulation.reward.scale = 1.0
visualization.enabled = false
)";
    
    // Test loading from simple format
    bool success = config.loadFromSimpleFormat(simpleContent);
    assert(success);
    
    // Verify values
    assert(config.getOr<int>("brain.neuron_count", 0) == 1000);
    assert(config.getOr<float>("brain.synapse_density", 0.0f) == 0.15f);
    assert(config.getOr<bool>("plasticity.stdp.enable", false) == true);
    
    std::cout << "    testSimpleConfiguration passed" << std::endl;
}

// Test configuration export
void testConfigurationExport() {
    nlm::Config config;
    
    // Add test values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test JSON export
    std::string jsonExport = config.exportToJSON();
    assert(!jsonExport.empty());
    assert(jsonExport.find("42") != std::string::npos);
    assert(jsonExport.find("test") != std::string::npos);
    
    // Test YAML export
    std::string yamlExport = config.exportToYAML();
    assert(!yamlExport.empty());
    assert(yamlExport.find("int_value") != std::string::npos);
    assert(yamlExport.find("string_value") != std::string::npos);
    
    std::cout << "    testConfigurationExport passed" << std::endl;
}

// Test configuration validation
void testConfigurationValidation() {
    nlm::Config config;
    
    // Add test values
    config.set("required_key", "required_value", nlm::ConfigSource::Runtime);
    config.addDescription("required_key", "A required configuration key");
    
    // Create simple schema
    nlm::ConfigSchema schema;
    schema.addEntry("required_key", "string", "A required configuration key", "", true);
    
    // Validate configuration
    assert(config.validate(schema));
    
    // Test validation errors
    std::string errorMsg;
    std::vector<std::string> errors = config.validate(schema, errorMsg);
    assert(errors.empty());
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

// Test configuration statistics
void testConfigurationStatistics() {
    nlm::Config config;
    
    // Add entries of different types
    config.set("default1", 1, nlm::ConfigSource::Default);
    config.set("file1", "test", nlm::ConfigSource::File);
    config.set("command1", 3.14, nlm::ConfigSource::CommandLine);
    config.set("runtime1", false, nlm::ConfigSource::Runtime);
    
    // Test statistics
    assert(config.getEntryCount() == 4);
    assert(config.getEntryCountBySource(nlm::ConfigSource::Default) == 1);
    assert(config.getEntryCountBySource(nlm::ConfigSource::File) == 1);
    assert(config.getEntryCountBySource(nlm::ConfigSource::CommandLine) == 1);
    assert(config.getEntryCountBySource(nlm::ConfigSource::Runtime) == 1);
    
    std::cout << "    testConfigurationStatistics passed" << std::endl;
}

// Test type conversion utilities
void testTypeConversion() {
    // Test string to value conversion
    nlm::ConfigValue value;
    assert(nlm::ConfigConverter::stringToValue("42", value));
    assert(std::holds_alternative<int>(value));
    assert(std::get<int>(value) == 42);
    
    assert(nlm::ConfigConverter::stringToValue("3.14", value));
    assert(std::holds_alternative<double>(value));
    assert(std::abs(std::get<double>(value) - 3.14) < 0.01);
    
    assert(nlm::ConfigConverter::stringToValue("true", value));
    assert(std::holds_alternative<bool>(value));
    assert(std::get<bool>(value) == true);
    
    assert(nlm::ConfigConverter::stringToValue("hello", value));
    assert(std::holds_alternative<std::string>(value));
    assert(std::get<std::string>(value) == "hello");
    
    // Test value to string conversion
    std::string str1 = nlm::ConfigConverter::valueToString(42);
    assert(str1 == "42");
    
    std::string str2 = nlm::ConfigConverter::valueToString(3.14);
    assert(str2.find("3.14") != std::string::npos);
    
    std::string str3 = nlm::ConfigConverter::valueToString(true);
    assert(str3 == "true");
    
    std::string str4 = nlm::ConfigConverter::valueToString(std::string("hello"));
    assert(str4 == "\"hello\"");
    
    std::cout << "    testTypeConversion passed" << std::endl;
}

// Test file-based configuration loading
void testFileBasedConfiguration() {
    nlm::Config config;
    
    // Create temporary config files
    std::string jsonContent = R"({
        "brain.neuron_count": 750,
        "plasticity.enable": true
    })";
    
    std::string yamlContent = R"(
brain:
  neuron_count: 750
plasticity:
  enable: true
)";
    
    std::string simpleContent = R"(
brain.neuron_count = 750
plasticity.enable = true
)";
    
    // Write files
    std::ofstream jsonFile("test_config.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    std::ofstream yamlFile("test_config.yaml");
    yamlFile << yamlContent;
    yamlFile.close();
    
    std::ofstream simpleFile("test_config.cfg");
    simpleFile << simpleContent;
    simpleFile.close();
    
    // Test loading from different file formats
    assert(config.loadFromFile("test_config.json"));
    assert(config.getOr<int>("brain.neuron_count", 0) == 750);
    
    config.clear();
    assert(config.loadFromFile("test_config.yaml"));
    assert(config.getOr<int>("brain.neuron_count", 0) == 750);
    
    config.clear();
    assert(config.loadFromFile("test_config.cfg"));
    assert(config.getOr<int>("brain.neuron_count", 0) == 750);
    
    // Clean up
    std::remove("test_config.json");
    std::remove("test_config.yaml");
    std::remove("test_config.cfg");
    
    std::cout << "    testFileBasedConfiguration passed" << std::endl;
}

// Test configuration save to file
void testSaveToFile() {
    nlm::Config config;
    
    // Add test values with descriptions
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Default);
    config.addDescription("brain.neuron_count", "Number of neurons in the brain");
    
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
    config.addDescription("plasticity.stdp.enable", "Enable STDP plasticity");
    
    config.set("visualization.enabled", false, nlm::ConfigSource::File);
    config.addDescription("visualization.enabled", "Enable visualization output");
    
    // Save to file
    assert(config.saveToFile("test_output.cfg"));
    
    // Verify file content
    std::ifstream savedFile("test_output.cfg");
    std::string line;
    std::vector<std::string> fileLines;
    
    while (std::getline(savedFile, line)) {
        fileLines.push_back(line);
    }
    
    savedFile.close();
    
    // Check that file contains expected content
    bool hasNeuronCount = false;
    bool hasDescription = false;
    
    for (const std::string& fileLine : fileLines) {
        if (fileLine.find("brain.neuron_count") != std::string::npos) {
            hasNeuronCount = true;
        }
        if (fileLine.find("Number of neurons") != std::string::npos) {
            hasDescription = true;
        }
    }
    
    assert(hasNeuronCount);
    assert(hasDescription);
    
    // Clean up
    std::remove("test_output.cfg");
    
    std::cout << "    testSaveToFile passed" << std::endl;
}

// Run all enhanced configuration tests
void runEnhancedConfigTests() {
    std::cout << "Running enhanced configuration tests..." << std::endl;
    
    testConfigCreation();
    testTypeSafeConfiguration();
    testJSONConfiguration();
    testYAMLConfiguration();
    testSimpleConfiguration();
    testConfigurationExport();
    testConfigurationValidation();
    testConfigurationStatistics();
    testTypeConversion();
    testFileBasedConfiguration();
    testSaveToFile();
    
    std::cout << std::endl;
    std::cout << "=== All enhanced configuration tests passed! ===" << std::endl;
}

} // namespace test_config
