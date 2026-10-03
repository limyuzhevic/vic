// Simple JSON/YAML config test
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_json_yaml_config {

void testJsonConfig() {
    nlm::Config config;
    
    // Set some test values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Test saving to JSON
    bool saveSuccess = config.saveAsJson("test_config.json");
    assert(saveSuccess);
    
    // Load from JSON
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile("test_config.json");
    assert(loadSuccess);
    
    // Verify values
    auto intVal = config2.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config2.get<double>("double_val");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config2.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config2.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testJsonConfig passed" << std::endl;
    
    // Clean up
    std::remove("test_config.json");
}

void testYamlConfig() {
    nlm::Config config;
    
    // Set some test values
    config.set("name", std::string("Test Brain"), nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000LL, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    config.set("threshold", 0.5, nlm::ConfigSource::Runtime);
    
    // Test saving to YAML
    bool saveSuccess = config.saveAsYaml("test_config.yaml");
    assert(saveSuccess);
    
    // Load from YAML
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile("test_config.yaml");
    assert(loadSuccess);
    
    // Verify values
    auto nameVal = config2.get<std::string>("name");
    assert(nameVal.has_value() && *nameVal == "Test Brain");
    
    auto neuronCountVal = config2.get<int64_t>("neuron_count");
    assert(neuronCountVal.has_value() && *neuronCountVal == 1000);
    
    auto enabledVal = config2.get<bool>("enabled");
    assert(enabledVal.has_value() && *enabledVal == true);
    
    auto thresholdVal = config2.get<double>("threshold");
    assert(thresholdVal.has_value() && *thresholdVal > 0.49 && *thresholdVal < 0.51);
    
    std::cout << "    testYamlConfig passed" << std::endl;
    
    // Clean up
    std::remove("test_config.yaml");
}

void testConfigCommandLineArgs() {
    nlm::Config config;
    
    // Test command line argument parsing
    const char* args[] = {"test", "--int_val=42", "--double_val=3.14", "--string_val=hello", "--bool_val=true"};
    int argc = 5;
    
    bool success = config.loadFromArgs(argc, const_cast<char**>(args));
    assert(success);
    
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testConfigCommandLineArgs passed" << std::endl;
}

void testConfigFileFormats() {
    // Test that different file extensions work correctly
    nlm::Config config;
    
    // Set a test value
    config.set("test_value", 123, nlm::ConfigSource::Runtime);
    
    // Test saving with different extensions
    bool jsonSave = config.saveAsJson("test_extensions.json");
    bool yamlSave = config.saveAsYaml("test_extensions.yaml");
    bool textSave = config.saveAsText("test_extensions.txt");
    
    assert(jsonSave && yamlSave && textSave);
    
    // Load each format
    nlm::Config config1;
    nlm::Config config2;
    nlm::Config config3;
    
    bool jsonLoad = config1.loadFromFile("test_extensions.json");
    bool yamlLoad = config2.loadFromFile("test_extensions.yaml");
    bool textLoad = config3.loadFromFile("test_extensions.txt");
    
    assert(jsonLoad && yamlLoad && textLoad);
    
    // Verify values
    auto val1 = config1.get<int>("test_value");
    auto val2 = config2.get<int>("test_value");
    auto val3 = config3.get<int>("test_value");
    
    assert(val1.has_value() && *val1 == 123);
    assert(val2.has_value() && *val2 == 123);
    assert(val3.has_value() && *val3 == 123);
    
    std::cout << "    testConfigFileFormats passed" << std::endl;
    
    // Clean up
    std::remove("test_extensions.json");
    std::remove("test_extensions.yaml");
    std::remove("test_extensions.txt");
}

void runAll() {
    testJsonConfig();
    testYamlConfig();
    testConfigCommandLineArgs();
    testConfigFileFormats();
    
    std::cout << "All JSON/YAML config tests passed!" << std::endl;
}

} // namespace test_json_yaml_config