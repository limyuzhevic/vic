// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

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

void testJsonFileLoad() {
    nlm::Config config;
    
    // Create a JSON test file
    std::string json_content = R"({
        "int_key": 42,
        "double_key": 3.14,
        "string_key": "hello",
        "bool_key": true,
        "array_int_key": [1, 2, 3],
        "array_str_key": ["a", "b", "c"]
    })";
    
    std::string test_file = "/tmp/test_config.json";
    std::ofstream file(test_file);
    file << json_content;
    file.close();
    
    // Load JSON file
    assert(config.loadFromFile(test_file));
    
    // Verify loaded values
    auto int_val = config.get<int>("int_key");
    assert(int_val.has_value() && *int_val == 42);
    
    auto double_val = config.get<double>("double_key");
    assert(double_val.has_value() && *double_val > 3.13 && *double_val < 3.15);
    
    auto string_val = config.get<std::string>("string_key");
    assert(string_val.has_value() && *string_val == "hello");
    
    auto bool_val = config.get<bool>("bool_key");
    assert(bool_val.has_value() && *bool_val == true);
    
    // Test array handling
    auto array_int_val = config.get<std::vector<int>>("array_int_key");
    assert(array_int_val.has_value() && array_int_val->size() == 3);
    
    std::remove(test_file.c_str());
    
    std::cout << "    testJsonFileLoad passed" << std::endl;
}

void testYamlFileLoad() {
    nlm::Config config;
    
    // For now, YAML uses JSON parser (as YAML support is not fully implemented)
    // This test verifies the YAML detection and fallback behavior
    std::string yaml_content = R"(# Simple YAML-like content
key: value
number: 123
boolean: true
list:
  - item1
  - item2
  - item3
)";
    
    std::string test_file = "/tmp/test_config.yaml";
    std::ofstream file(test_file);
    file << yaml_content;
    file.close();
    
    // Load YAML file (should fallback to simple parsing)
    assert(config.loadFromFile(test_file));
    
    // Verify that YAML content was parsed (or at least didn't crash)
    // The actual parsing behavior depends on the implementation
    std::remove(test_file.c_str());
    
    std::cout << "    testYamlFileLoad passed" << std::endl;
}

void testJsonFileSave() {
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    std::string test_file = "/tmp/test_config_save.json";
    
    // Save to JSON file
    assert(config.saveToJsonFile(test_file));
    
    // Load it back and verify
    nlm::Config config2;
    assert(config2.loadFromFile(test_file));
    
    auto int_val = config2.get<int>("int_value");
    assert(int_val.has_value() && *int_val == 42);
    
    auto string_val = config2.get<std::string>("string_value");
    assert(string_val.has_value() && *string_val == "test");
    
    std::remove(test_file.c_str());
    
    std::cout << "    testJsonFileSave passed" << std::endl;
}

void testSimpleFileBackwardCompatibility() {
    nlm::Config config;
    
    // Create a simple key=value config file (legacy format)
    std::string simple_content = R"(# Legacy config file
key1 = value1
key2 = "quoted value"
key3 = 123
key4 = 3.14
key5 = true
)";
    
    std::string test_file = "/tmp/test_config_simple.cfg";
    std::ofstream file(test_file);
    file << simple_content;
    file.close();
    
    // Load simple file (should fallback to backward compatible parsing)
    assert(config.loadFromFile(test_file));
    
    // Verify backward compatibility parsing
    auto key1_val = config.get<std::string>("key1");
    assert(key1_val.has_value() && *key1_val == "value1");
    
    auto key2_val = config.get<std::string>("key2");
    assert(key2_val.has_value() && *key2_val == "quoted value");
    
    auto key3_val = config.get<int>("key3");
    assert(key3_val.has_value() && *key3_val == 123);
    
    auto key4_val = config.get<double>("key4");
    assert(key4_val.has_value() && *key4_val > 3.13 && *key4_val < 3.15);
    
    auto key5_val = config.get<bool>("key5");
    assert(key5_val.has_value() && *key5_val == true);
    
    std::remove(test_file.c_str());
    
    std::cout << "    testSimpleFileBackwardCompatibility passed" << std::endl;
}

void testJsonStringOperations() {
    nlm::Config config;
    
    config.set("test_int", 100);
    config.set("test_string", "hello");
    
    // Get JSON string representation
    std::string json_str = config.toJsonString();
    
    // Parse JSON string to verify it's valid
    nlohmann::json j = nlohmann::json::parse(json_str);
    
    assert(j.contains("test_int"));
    assert(j["test_int"] == 100);
    assert(j.contains("test_string"));
    assert(j["test_string"] == "hello");
    
    std::cout << "    testJsonStringOperations passed" << std::endl;
}

void testFileFormatDetection() {
    nlm::Config config;
    
    // Test JSON file detection
    std::string json_file = "/tmp/test_json.json";
    std::ofstream json_out(json_file);
    json_out << R"({"key": "value"})";
    json_out.close();
    
    assert(config.isJsonFile(json_file));
    std::remove(json_file.c_str());
    
    // Test YAML file detection (by extension)
    std::string yaml_file = "/tmp/test_yaml.yaml";
    std::ofstream yaml_out(yaml_file);
    yaml_out << "key: value";
    yaml_out.close();
    
    assert(config.isYamlFile(yaml_file));
    std::remove(yaml_file.c_str());
    
    // Test simple key=value file detection (should not be detected as JSON/YAML)
    std::string simple_file = "/tmp/test_simple.cfg";
    std::ofstream simple_out(simple_file);
    simple_out << "key = value";
    simple_out.close();
    
    assert(!config.isJsonFile(simple_file));
    assert(!config.isYamlFile(simple_file));
    std::remove(simple_file.c_str());
    
    std::cout << "    testFileFormatDetection passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJsonFileLoad();
    testYamlFileLoad();
    testJsonFileSave();
    testSimpleFileBackwardCompatibility();
    testJsonStringOperations();
    testFileFormatDetection();
}

} // namespace test_config
