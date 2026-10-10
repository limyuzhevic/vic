#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

// Test function for JSON configuration
void testJSONConfig() {
    nlm::Config config;
    
    std::string jsonContent = R"({
        "test_value": 42,
        "name": "JSON Test",
        "enabled": true
    })";
    
    std::string jsonFile = "/tmp/test_json_config.json";
    std::ofstream file(jsonFile);
    file << jsonContent;
    file.close();
    
    bool loaded = config.loadFromFile(jsonFile);
    assert(loaded);
    
    auto value = config.get<int64_t>("test_value");
    assert(value.has_value() && *value == 42);
    
    auto name = config.get<std::string>("name");
    assert(name.has_value() && *name == "JSON Test");
    
    auto enabled = config.get<bool>("enabled");
    assert(enabled.has_value() && *enabled == true);
    
    std::cout << "✓ JSON Configuration Test PASSED" << std::endl;
    
    // Save test
    config.set("new_value", std::string("Saved"), nlm::ConfigSource::Runtime);
    std::string savedJsonFile = "/tmp/saved_json_config.json";
    bool saved = config.saveToFile(savedJsonFile);
    assert(saved);
    
    std::cout << "✓ JSON Save Test PASSED" << std::endl;
    
    std::remove(jsonFile.c_str());
    std::remove(savedJsonFile.c_str());
}

// Test function for YAML configuration
void testYAMLConfig() {
    nlm::Config config;
    
    std::string yamlContent = R"(
test_value: 123
name: "YAML Test"
enabled: false
float_value: 3.14
array_value:
  - 1.0
  - 2.0
  - 3.0
)";
    
    std::string yamlFile = "/tmp/test_yaml_config.yaml";
    std::ofstream file(yamlFile);
    file << yamlContent;
    file.close();
    
    bool loaded = config.loadFromFile(yamlFile);
    assert(loaded);
    
    auto value = config.get<int64_t>("test_value");
    assert(value.has_value() && *value == 123);
    
    auto name = config.get<std::string>("name");
    assert(name.has_value() && *name == "YAML Test");
    
    auto enabled = config.get<bool>("enabled");
    assert(enabled.has_value() && *enabled == false);
    
    auto floatVal = config.get<double>("float_value");
    assert(floatVal.has_value() && *floatVal > 3.14 && *floatVal < 3.15);
    
    auto arrayVal = config.get<std::vector<float>>("array_value");
    assert(arrayVal.has_value() && arrayVal->size() == 3);
    
    std::cout << "✓ YAML Configuration Test PASSED" << std::endl;
    
    // Save test
    config.set("yaml_saved", std::string("YAML Saved"), nlm::ConfigSource::Runtime);
    std::string savedYamlFile = "/tmp/saved_yaml_config.yaml";
    bool saved = config.saveToFile(savedYamlFile);
    assert(saved);
    
    std::cout << "✓ YAML Save Test PASSED" << std::endl;
    
    std::remove(yamlFile.c_str());
    std::remove(savedYamlFile.c_str());
}

// Test function for legacy configuration
void testLegacyConfig() {
    nlm::Config config;
    
    std::string legacyContent = R"(
# This is a comment
legacy_value = 999
description = "Legacy Configuration"
enabled = true
)";
    
    std::string legacyFile = "/tmp/test_legacy_config.legacy";
    std::ofstream file(legacyFile);
    file << legacyContent;
    file.close();
    
    bool loaded = config.loadFromFile(legacyFile);
    assert(loaded);
    
    auto value = config.get<int64_t>("legacy_value");
    assert(value.has_value() && *value == 999);
    
    auto desc = config.get<std::string>("description");
    assert(desc.has_value() && *desc == "Legacy Configuration");
    
    auto enabled = config.get<bool>("enabled");
    assert(enabled.has_value() && *enabled == true);
    
    std::cout << "✓ Legacy Configuration Test PASSED" << std::endl;
    
    std::remove(legacyFile.c_str());
}

// Test function for format detection
void testFormatDetection() {
    nlm::Config config;
    
    // Test JSON format
    std::string jsonFile = "/tmp/detection_test.json";
    std::ofstream file(jsonFile);
    file << "{\"detected\": \"json\"}";
    file.close();
    
    bool loaded = config.loadFromFile(jsonFile);
    assert(loaded);
    auto val = config.get<std::string>("detected");
    assert(val.has_value() && *val == "json");
    std::remove(jsonFile.c_str());
    
    // Test YAML format
    std::string yamlFile = "/tmp/detection_test.yaml";
    std::ofstream file2(yamlFile);
    file2 << "detected: yaml";
    file2.close();
    
    config.clear();
    loaded = config.loadFromFile(yamlFile);
    assert(loaded);
    auto val2 = config.get<std::string>("detected");
    assert(val2.has_value() && *val2 == "yaml");
    std::remove(yamlFile.c_str());
    
    // Test legacy format
    std::string legacyFile = "/tmp/detection_test.legacy";
    std::ofstream file3(legacyFile);
    file3 << "detected = legacy";
    file3.close();
    
    config.clear();
    loaded = config.loadFromFile(legacyFile);
    assert(loaded);
    auto val3 = config.get<std::string>("detected");
    assert(val3.has_value() && *val3 == "legacy");
    std::remove(legacyFile.c_str());
    
    std::cout << "✓ Format Detection Test PASSED" << std::endl;
}

int main() {
    std::cout << "=== NLM JSON/YAML Configuration Tests ===" << std::endl;
    
    try {
        testJSONConfig();
        testYAMLConfig();
        testLegacyConfig();
        testFormatDetection();
        
        std::cout << std::endl;
        std::cout << "=== All JSON/YAML Configuration Tests PASSED ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test FAILED: " << e.what() << " ===" << std::endl;
        return 1;
    }
}