// Enhanced Config Tests
// Tests the improved JSON/YAML configuration system
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigJSON() {
    nlm::Config config;
    
    // Test setting JSON-like structure
    config.set("name", std::string("Test Brain"), nlm::ConfigSource::Runtime);
    config.set("neurons", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    // Test getting values
    auto name = config.get<std::string>("name");
    assert(name && *name == "Test Brain");
    
    auto neurons = config.get<int64_t>("neurons");
    assert(neurons && *neurons == 1000);
    
    auto learningRate = config.get<double>("learning_rate");
    assert(learningRate && std::abs(*learningRate - 0.01) < 0.001);
    
    auto enabled = config.get<bool>("enabled");
    assert(enabled && *enabled == true);
    
    // Test JSON serialization
    std::string jsonFile = "test_config.json";
    assert(config.saveToFile(jsonFile));
    
    // Load and verify
    nlm::Config config2;
    assert(config2.loadFromFile(jsonFile));
    
    assert(config2.get<std::string>("name") == name);
    assert(config2.get<int64_t>("neurons") == neurons);
    assert(config2.get<double>("learning_rate") == learningRate);
    assert(config2.get<bool>("enabled") == enabled);
    
    // Clean up
    std::filesystem::remove(jsonFile);
    
    std::cout << "    testConfigJSON passed" << std::endl;
}

void testConfigYAML() {
    nlm::Config config;
    
    // Test setting YAML-like structure
    config.set("brain_type", std::string("Enhanced"), nlm::ConfigSource::Runtime);
    config.set("neuron_count", static_cast<size_t>(500), nlm::ConfigSource::Runtime);
    config.set("plasticity", 0.05f, nlm::ConfigSource::Runtime);
    
    // Test getting values
    auto brainType = config.get<std::string>("brain_type");
    assert(brainType && *brainType == "Enhanced");
    
    auto neuronCount = config.get<size_t>("neuron_count");
    assert(neuronCount && *neuronCount == 500);
    
    auto plasticity = config.get<double>("plasticity");
    assert(plasticity && std::abs(*plasticity - 0.05) < 0.001);
    
    // Test YAML serialization
    std::string yamlFile = "test_config.yaml";
    assert(config.saveToFile(yamlFile));
    
    // Load and verify
    nlm::Config config2;
    assert(config2.loadFromFile(yamlFile));
    
    assert(config2.get<std::string>("brain_type") == brainType);
    assert(config2.get<size_t>("neuron_count") == neuronCount);
    assert(config2.get<double>("plasticity") == plasticity);
    
    // Clean up
    std::filesystem::remove(yamlFile);
    
    std::cout << "    testConfigYAML passed" << std::endl;
}

void testConfigFileFormats() {
    nlm::Config config;
    
    // Set up test data
    config.set("format_test", std::string("multi_format_support"), nlm::ConfigSource::Runtime);
    config.set("test_number", 123, nlm::ConfigSource::Runtime);
    config.set("test_float", 3.14159f, nlm::ConfigSource::Runtime);
    config.set("test_array", std::vector<double>({1.0, 2.0, 3.0}), nlm::ConfigSource::Runtime);
    
    // Test all supported file formats
    std::string jsonFile = "test_format.json";
    std::string yamlFile = "test_format.yaml";
    std::string simpleFile = "test_format.txt";
    
    // Test JSON
    assert(config.saveToFile(jsonFile));
    nlm::Config configJSON;
    assert(configJSON.loadFromFile(jsonFile));
    assert(configJSON.get<std::string>("format_test") == config.get<std::string>("format_test"));
    std::filesystem::remove(jsonFile);
    
    // Test YAML  
    assert(config.saveToFile(yamlFile));
    nlm::Config configYAML;
    assert(configYAML.loadFromFile(yamlFile));
    assert(configYAML.get<std::string>("format_test") == config.get<std::string>("format_test"));
    std::filesystem::remove(yamlFile);
    
    // Test simple format
    assert(config.saveToFile(simpleFile));
    nlm::Config configSimple;
    assert(configSimple.loadFromFile(simpleFile));
    assert(configSimple.get<std::string>("format_test") == config.get<std::string>("format_test"));
    std::filesystem::remove(simpleFile);
    
    std::cout << "    testConfigFileFormats passed" << std::endl;
}

void runAll() {
    std::cout << "Running Config Tests..." << std::endl;
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSON();
    testConfigYAML();
    testConfigFileFormats();
    std::cout << "All Config Tests passed!" << std::endl;
}

} // namespace test_config
