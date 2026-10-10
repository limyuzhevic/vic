// JSON/YAML Configuration Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_json_yaml_config {

void testJSONConfiguration() {
    nlm::Config config;
    
    // Create a test JSON file
    std::string jsonContent = R"({
        "integer_value": 42,
        "float_value": 3.14159,
        "string_value": "Hello JSON",
        "boolean_value": true,
        "array_value": [1.0, 2.0, 3.0, 4.0],
        "nested_object": {
            "nested_key": "nested_value",
            "nested_number": 123
        }
    })";
    
    // Write to a temporary file
    std::string jsonFile = "/tmp/test_config.json";
    std::ofstream jsonFileStream(jsonFile);
    jsonFileStream << jsonContent;
    jsonFileStream.close();
    
    // Load JSON configuration
    bool jsonLoaded = config.loadFromFile(jsonFile);
    assert(jsonLoaded);
    
    // Test loaded values
    auto intVal = config.get<int64_t>("integer_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto floatVal = config.get<double>("float_value");
    assert(floatVal.has_value() && *floatVal > 3.14 && *floatVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "Hello JSON");
    
    auto boolVal = config.get<bool>("boolean_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto arrayVal = config.get<std::vector<float>>("array_value");
    assert(arrayVal.has_value() && arrayVal->size() == 4);
    
    std::cout << "    testJSONConfiguration passed" << std::endl;
    
    // Clean up
    std::remove(jsonFile.c_str());
}

void testYAMLConfiguration() {
    nlm::Config config;
    
    // Create a test YAML file
    std::string yamlContent = R"(
integer_value: 42
float_value: 2.71828
string_value: Hello YAML
boolean_value: false
array_value:
  - 10.0
  - 20.0
  - 30.0
nested_object:
  nested_key: nested_value
  nested_number: 999
)";
    
    // Write to a temporary file
    std::string yamlFile = "/tmp/test_config.yaml";
    std::ofstream yamlFileStream(yamlFile);
    yamlFileStream << yamlContent;
    yamlFileStream.close();
    
    // Load YAML configuration
    bool yamlLoaded = config.loadFromFile(yamlFile);
    assert(yamlLoaded);
    
    // Test loaded values
    auto intVal = config.get<int64_t>("integer_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto floatVal = config.get<double>("float_value");
    assert(floatVal.has_value() && *floatVal > 2.71 && *floatVal < 2.72);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "Hello YAML");
    
    auto boolVal = config.get<bool>("boolean_value");
    assert(boolVal.has_value() && *boolVal == false);
    
    auto arrayVal = config.get<std::vector<float>>("array_value");
    assert(arrayVal.has_value() && arrayVal->size() == 3);
    
    std::cout << "    testYAMLConfiguration passed" << std::endl;
    
    // Clean up
    std::remove(yamlFile.c_str());
}

void testLegacyConfiguration() {
    nlm::Config config;
    
    // Create a test legacy config file
    std::string legacyContent = R"(
# This is a comment
integer_value = 100
double_value = 3.14159
string_value = "Hello Legacy"
boolean_value = true
array_value = [1.0, 2.0, 3.0]
)";
    
    // Write to a temporary file
    std::string legacyFile = "/tmp/test_config.legacy";
    std::ofstream legacyFileStream(legacyFile);
    legacyFileStream << legacyContent;
    legacyFileStream.close();
    
    // Load legacy configuration
    bool legacyLoaded = config.loadFromFile(legacyFile);
    assert(legacyLoaded);
    
    // Test loaded values
    auto intVal = config.get<int64_t>("integer_value");
    assert(intVal.has_value() && *intVal == 100);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.14 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "Hello Legacy");
    
    auto boolVal = config.get<bool>("boolean_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testLegacyConfiguration passed" << std::endl;
    
    // Clean up
    std::remove(legacyFile.c_str());
}

void testSaveJSONConfiguration() {
    nlm::Config config;
    
    // Set some configuration values
    config.set("name", std::string("TestApp"), nlm::ConfigSource::Runtime);
    config.set("version", 1.0, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    config.set("count", int64_t(42), nlm::ConfigSource::Runtime);
    config.set("values", std::vector<float>{1.5, 2.5, 3.5}, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string jsonFile = "/tmp/saved_config.json";
    bool saved = config.saveToFile(jsonFile);
    assert(saved);
    
    // Reload and verify
    nlm::Config config2;
    bool loaded = config2.loadFromFile(jsonFile);
    assert(loaded);
    
    auto nameVal = config2.get<std::string>("name");
    assert(nameVal.has_value() && *nameVal == "TestApp");
    
    auto versionVal = config2.get<double>("version");
    assert(versionVal.has_value() && *versionVal == 1.0);
    
    auto enabledVal = config2.get<bool>("enabled");
    assert(enabledVal.has_value() && *enabledVal == true);
    
    auto countVal = config2.get<int64_t>("count");
    assert(countVal.has_value() && *countVal == 42);
    
    auto valuesVal = config2.get<std::vector<float>>("values");
    assert(valuesVal.has_value() && valuesVal->size() == 3);
    
    std::cout << "    testSaveJSONConfiguration passed" << std::endl;
    
    // Clean up
    std::remove(jsonFile.c_str());
}

void testSaveYAMLConfiguration() {
    nlm::Config config;
    
    // Set some configuration values
    config.set("database_url", std::string("postgresql://localhost:5432/test"), nlm::ConfigSource::Runtime);
    config.set("max_connections", int64_t(100), nlm::ConfigSource::Runtime);
    config.set("timeout", 30.5, nlm::ConfigSource::Runtime);
    config.set("debug_mode", false, nlm::ConfigSource::Runtime);
    
    // Save to YAML file
    std::string yamlFile = "/tmp/saved_config.yaml";
    bool saved = config.saveToFile(yamlFile);
    assert(saved);
    
    // Reload and verify
    nlm::Config config2;
    bool loaded = config2.loadFromFile(yamlFile);
    assert(loaded);
    
    auto urlVal = config2.get<std::string>("database_url");
    assert(urlVal.has_value() && *urlVal == "postgresql://localhost:5432/test");
    
    auto maxConnVal = config2.get<int64_t>("max_connections");
    assert(maxConnVal.has_value() && *maxConnVal == 100);
    
    auto timeoutVal = config2.get<double>("timeout");
    assert(timeoutVal.has_value() && *timeoutVal == 30.5);
    
    auto debugVal = config2.get<bool>("debug_mode");
    assert(debugVal.has_value() && *debugVal == false);
    
    std::cout << "    testSaveYAMLConfiguration passed" << std::endl;
    
    // Clean up
    std::remove(yamlFile.c_str());
}

void testFormatDetection() {
    nlm::Config config;
    
    // Test JSON format detection
    std::string jsonFile = "/tmp/format_test.json";
    std::ofstream jsonFileStream(jsonFile);
    jsonFileStream << "{\"key\": \"value\"}";
    jsonFileStream.close();
    
    bool loaded1 = config.loadFromFile(jsonFile);
    assert(loaded1);
    auto val1 = config.get<std::string>("key");
    assert(val1.has_value() && *val1 == "value");
    
    // Test YAML format detection
    std::string yamlFile = "/tmp/format_test.yaml";
    std::ofstream yamlFileStream(yamlFile);
    yamlFileStream << "key: value\nanother: 123";
    yamlFileStream.close();
    
    config.clear();
    bool loaded2 = config.loadFromFile(yamlFile);
    assert(loaded2);
    auto val2 = config.get<std::string>("key");
    assert(val2.has_value() && *val2 == "value");
    auto val3 = config.get<int64_t>("another");
    assert(val3.has_value() && *val3 == 123);
    
    // Test legacy format detection
    std::string legacyFile = "/tmp/format_test.legacy";
    std::ofstream legacyFileStream(legacyFile);
    legacyFileStream << "legacy_key = legacy_value";
    legacyFileStream.close();
    
    config.clear();
    bool loaded3 = config.loadFromFile(legacyFile);
    assert(loaded3);
    auto val4 = config.get<std::string>("legacy_key");
    assert(val4.has_value() && *val4 == "legacy_value");
    
    std::cout << "    testFormatDetection passed" << std::endl;
    
    // Clean up
    std::remove(jsonFile.c_str());
    std::remove(yamlFile.c_str());
    std::remove(legacyFile.c_str());
}

void testErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    bool loaded = config.loadFromFile("/tmp/non_existent_file.xyz");
    assert(!loaded);
    
    // Test loading invalid JSON
    std::string invalidJsonFile = "/tmp/invalid_json.json";
    std::ofstream invalidJsonStream(invalidJsonFile);
    invalidJsonStream << "{invalid json syntax";
    invalidJsonStream.close();
    
    bool loaded2 = config.loadFromFile(invalidJsonFile);
    assert(!loaded2);
    
    // Test loading invalid YAML
    std::string invalidYamlFile = "/tmp/invalid_yaml.yaml";
    std::ofstream invalidYamlStream(invalidYamlFile);
    invalidYamlStream << "invalid: yaml: syntax:";
    invalidYamlStream.close();
    
    config.clear();
    bool loaded3 = config.loadFromFile(invalidYamlFile);
    assert(!loaded3);
    
    std::cout << "    testErrorHandling passed" << std::endl;
    
    // Clean up
    std::remove(invalidJsonFile.c_str());
    std::remove(invalidYamlFile.c_str());
}

void runAll() {
    std::cout << "Running JSON/YAML Configuration Tests..." << std::endl;
    testJSONConfiguration();
    testYAMLConfiguration();
    testLegacyConfiguration();
    testSaveJSONConfiguration();
    testSaveYAMLConfiguration();
    testFormatDetection();
    testErrorHandling();
    std::cout << "All JSON/YAML Configuration Tests passed!" << std::endl;
}

} // namespace test_json_yaml_config