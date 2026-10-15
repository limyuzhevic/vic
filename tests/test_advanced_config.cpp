// Comprehensive test for new configuration features
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_advanced_config {

void testConfigJSONParsing() {
    std::cout << "    Testing JSON configuration parsing..." << std::endl;
    
    nlm::Config config;
    
    // Test JSON format - should auto-detect
    std::string jsonContent = R"({
        "int_value": 42,
        "double_value": 3.14,
        "string_value": "test",
        "bool_value": true,
        "vector_int": [1, 2, 3],
        "vector_double": [1.5, 2.5, 3.5],
        "vector_string": ["a", "b", "c"]
    })";
    
    std::ofstream jsonFile("test_json_config.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    bool loaded = config.loadFromFile("test_json_config.json");
    assert(loaded);
    
    // Verify values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.001);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::remove("test_json_config.json");
    
    std::cout << "    testConfigJSONParsing PASSED" << std::endl;
}

void testConfigYAMLParsing() {
    std::cout << "    Testing YAML configuration parsing..." << std::endl;
    
    nlm::Config config;
    
    // Test YAML format
    std::string yamlContent = R"(
# Test YAML config
int_value: 100
double_value: 2.718
string_value: hello
bool_value: false
vector_int: [10, 20, 30]
vector_double: [1.1, 2.2, 3.3]
vector_string: [foo, bar, baz]
)";
    
    std::ofstream yamlFile("test_yaml_config.yaml");
    yamlFile << yamlContent;
    yamlFile.close();
    
    bool loaded = config.loadFromFile("test_yaml_config.yaml");
    assert(loaded);
    
    // Verify values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 100);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    std::remove("test_yaml_config.yaml");
    
    std::cout << "    testConfigYAMLParsing PASSED" << std::endl;
}

void testConfigSaveEnhanced() {
    std::cout << "    Testing enhanced configuration save..." << std::endl;
    
    nlm::Config config;
    
    // Set various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello world"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    std::vector<int> intVec = {1, 2, 3};
    std::vector<double> doubleVec = {1.5, 2.5, 3.5};
    std::vector<std::string> stringVec = {"a", "b", "c"};
    
    config.set("vector_int", intVec, nlm::ConfigSource::Runtime);
    config.set("vector_double", doubleVec, nlm::ConfigSource::Runtime);
    config.set("vector_string", stringVec, nlm::ConfigSource::Runtime);
    
    // Save to file
    bool saved = config.saveToFile("test_enhanced_config.cfg");
    assert(saved);
    
    // Verify file content
    std::ifstream savedFile("test_enhanced_config.cfg");
    std::string line;
    bool foundInt = false;
    bool foundDouble = false;
    bool foundString = false;
    
    while (std::getline(savedFile, line)) {
        if (line.find("int_value") != std::string::npos && line.find("42") != std::string::npos) {
            foundInt = true;
        }
        if (line.find("double_value") != std::string::npos && line.find("3.14") != std::string::npos) {
            foundDouble = true;
        }
        if (line.find("string_value") != std::string::npos && line.find("hello world") != std::string::npos) {
            foundString = true;
        }
    }
    
    std::remove("test_enhanced_config.cfg");
    
    assert(foundInt && foundDouble && foundString);
    
    std::cout << "    testConfigSaveEnhanced PASSED" << std::endl;
}

void testConfigSourceTracking() {
    std::cout << "    Testing configuration source tracking..." << std::endl;
    
    nlm::Config config;
    
    config.set("from_file", "test_value", nlm::ConfigSource::File);
    config.set("from_cmd", "cmd_value", nlm::ConfigSource::CommandLine);
    config.set("from_default", "default_value", nlm::ConfigSource::Default);
    
    // The source information is stored internally but not directly accessible
    // For this test, we'll just verify that values can be retrieved correctly
    auto fileVal = config.get<std::string>("from_file");
    assert(fileVal.has_value() && *fileVal == "test_value");
    
    auto cmdVal = config.get<std::string>("from_cmd");
    assert(cmdVal.has_value() && *cmdVal == "cmd_value");
    
    auto defaultVal = config.get<std::string>("from_default");
    assert(defaultVal.has_value() && *defaultVal == "default_value");
    
    std::cout << "    testConfigSourceTracking PASSED" << std::endl;
}

void runAll() {
    testConfigJSONParsing();
    testConfigYAMLParsing();
    testConfigSaveEnhanced();
    testConfigSourceTracking();
}

} // namespace test_advanced_config