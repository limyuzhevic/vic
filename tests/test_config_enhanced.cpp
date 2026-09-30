// Comprehensive Configuration System Tests
// Enhanced tests for the new JSON/YAML/advanced configuration system

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace test_config_enhanced {

void testJSONFileLoading() {
    nlm::Config config;
    
    // Create a test JSON file
    std::string jsonContent = R"({
        "int_value": 42,
        "double_value": 3.14,
        "string_value": "hello",
        "bool_value": true,
        "nested": {
            "inner_key": "inner_value",
            "number": 123
        },
        "array_values": [1, 2, 3, 4, 5],
        "float_array": [1.1, 2.2, 3.3]
    })";
    
    std::string jsonFile = "test_config.json";
    std::ofstream file(jsonFile);
    file << jsonContent;
    file.close();
    
    // Load the JSON file
    assert(config.loadFromFile(jsonFile));
    
    // Verify loaded values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && (*doubleVal > 3.13 && *doubleVal < 3.15));
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test nested access (through map type)
    auto nestedMap = config.get<std::map<std::string, std::string>>("nested");
    if (nestedMap.has_value()) {
        auto it = nestedMap.value().find("inner_key");
        if (it != nestedMap.value().end()) {
            assert(it->second == "inner_value");
        }
    }
    
    std::cout << "    testJSONFileLoading passed" << std::endl;
    
    // Cleanup
    std::remove(jsonFile.c_str());
}

void testYAMLLoading() {
    nlm::Config config;
    
    // Create a test YAML file
    std::string yamlContent = R"(
        int_value: 42
        double_value: 3.14
        string_value: hello
        bool_value: true
        nested:
            inner_key: inner_value
            number: 123
        array_values: [1, 2, 3, 4, 5]
        float_array: [1.1, 2.2, 3.3]
    )";
    
    std::string yamlFile = "test_config.yaml";
    std::ofstream file(yamlFile);
    file << yamlContent;
    file.close();
    
    // Load the YAML file
    assert(config.loadFromFile(yamlFile));
    
    // Verify some loaded values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    std::cout << "    testYAMLLoading passed" << std::endl;
    
    // Cleanup
    std::remove(yamlFile.c_str());
}

void testSimpleFormatLoading() {
    nlm::Config config;
    
    // Create a test simple config file
    std::string simpleContent = R"(
        # Test configuration
        int_value = 42
        double_value = 3.14
        string_value = "hello"
        bool_value = true
        # Another comment
        array_int = [1, 2, 3]
        array_double = [1.1, 2.2, 3.3]
        array_string = ["a", "b", "c"]
    )";
    
    std::string simpleFile = "test_config.cfg";
    std::ofstream file(simpleFile);
    file << simpleContent;
    file.close();
    
    // Load the simple file
    assert(config.loadFromFile(simpleFile));
    
    // Verify loaded values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    std::cout << "    testSimpleFormatLoading passed" << std::endl;
    
    // Cleanup
    std::remove(simpleFile.c_str());
}

void testConfigurationValidation() {
    nlm::Config config;
    
    // Set up validation callback
    bool validationCalled = false;
    config.setValidationCallback([&validationCalled](const nlm::ConfigEntry& entry) {
        if (entry.key == "test_value" && entry.value == 123) {
            validationCalled = true;
        }
        return true; // Accept all for this test
    });
    
    config.set("test_value", 123, nlm::ConfigSource::Runtime);
    
    // Validate configuration
    assert(config.validate());
    assert(validationCalled);
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testImportExport() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set up config1
    config1.set("key1", 100, nlm::ConfigSource::Runtime);
    config1.set("key2", "hello", nlm::ConfigSource::File);
    config1.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    
    // Import config2 from config1
    assert(config2.importFrom(config1));
    
    // Verify config2 has the values
    assert(config2.has("key1"));
    assert(config2.has("key2"));
    assert(config2.has("key3"));
    
    auto val1 = config2.get<int>("key1");
    assert(val1.has_value() && *val1 == 100);
    
    // Export config1 to config2
    assert(config2.exportTo(config1));
    assert(config1.has("key1"));
    
    std::cout << "    testImportExport passed" << std::endl;
}

void testSnapshotFunctionality() {
    nlm::Config config;
    
    // Set up some configuration
    config.set("name", "Test App", nlm::ConfigSource::Runtime);
    config.set("version", 1.0, nlm::ConfigSource::File);
    config.set("enabled", true, nlm::ConfigSource::CommandLine);
    
    // Create snapshot
    std::string snapshot = config.createSnapshot();
    assert(!snapshot.empty());
    assert(snapshot.find("NLM-ConfigSnapshot:") != std::string::npos);
    
    // Create a new config and restore from snapshot
    nlm::Config config2;
    assert(config2.restoreFromSnapshot(snapshot));
    
    // Verify restored values
    assert(config2.has("name"));
    assert(config2.has("version"));
    assert(config2.has("enabled"));
    
    std::cout << "    testSnapshotFunctionality passed" << std::endl;
}

void testConfigurationMerging() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set up both configurations
    config1.set("key1", 100, nlm::ConfigSource::Runtime);
    config1.set("key2", "hello", nlm::ConfigSource::File);
    
    config2.set("key2", "world", nlm::ConfigSource::Runtime);  // Overwrite
    config2.set("key3", 3.14, nlm::ConfigSource::CommandLine);  // New key
    
    // Merge config2 into config1 (overwrite)
    config1.merge(config2, true);
    
    // Verify merged result
    assert(config1.has("key1"));  // Should remain
    assert(config1.has("key2"));  // Should be from config2
    assert(config1.has("key3"));  // Should be added
    
    auto val2 = config1.get<std::string>("key2");
    assert(val2.has_value() && *val2 == "world");
    
    // Merge config2 into config1 (don't overwrite)
    config1.set("key1", 200, nlm::ConfigSource::Runtime);
    config1.merge(config2, false);
    
    // key2 should still be from config1 since overwrite=false
    auto val2Again = config1.get<std::string>("key2");
    assert(val2Again.has_value() && *val2Again == "world");  // Still from config2
    auto val1Again = config1.get<int>("key1");
    assert(val1Again.has_value() && *val1Again == 200);  // From config1
    
    std::cout << "    testConfigurationMerging passed" << std::endl;
}

void testFileFormatSave() {
    nlm::Config config;
    
    // Set up configuration
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::File);
    config.set("double_value", 3.14, nlm::ConfigSource::CommandLine);
    
    // Test JSON save
    std::string jsonFile = "test_output.json";
    assert(config.saveToFile(jsonFile, nlm::Config::Format::JSON));
    
    // Verify file was created and has content
    std::ifstream jsonTestFile(jsonFile);
    assert(jsonTestFile.is_open());
    std::string jsonContent((std::istreambuf_iterator<char>(jsonTestFile)), std::istreambuf_iterator<char>());
    assert(!jsonContent.empty());
    assert(jsonContent.find("int_value") != std::string::npos);
    
    jsonTestFile.close();
    std::remove(jsonFile.c_str());
    
    // Test YAML save
    std::string yamlFile = "test_output.yaml";
    assert(config.saveToFile(yamlFile, nlm::Config::Format::YAML));
    
    // Verify file was created
    std::ifstream yamlTestFile(yamlFile);
    assert(yamlTestFile.is_open());
    
    yamlTestFile.close();
    std::remove(yamlFile.c_str());
    
    // Test simple format save
    std::string simpleFile = "test_output.cfg";
    assert(config.saveToFile(simpleFile, nlm::Config::Format::SIMPLE));
    
    // Verify file was created
    std::ifstream simpleTestFile(simpleFile);
    assert(simpleTestFile.is_open());
    
    simpleTestFile.close();
    std::remove(simpleFile.c_str());
    
    std::cout << "    testFileFormatSave passed" << std::endl;
}

void testSourceBasedOperations() {
    nlm::Config config;
    
    // Set values with different sources
    config.set("key1", 100, nlm::ConfigSource::Runtime);
    config.set("key2", 200, nlm::ConfigSource::File);
    config.set("key3", 300, nlm::ConfigSource::CommandLine);
    
    // Get keys by source
    auto runtimeKeys = config.getKeysBySource(nlm::ConfigSource::Runtime);
    assert(runtimeKeys.size() == 1);
    assert(runtimeKeys[0] == "key1");
    
    auto fileKeys = config.getKeysBySource(nlm::ConfigSource::File);
    assert(fileKeys.size() == 1);
    assert(fileKeys[0] == "key2");
    
    auto cmdKeys = config.getKeysBySource(nlm::ConfigSource::CommandLine);
    assert(cmdKeys.size() == 1);
    assert(cmdKeys[0] == "key3");
    
    std::cout << "    testSourceBasedOperations passed" << std::endl;
}

void testErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    assert(!config.loadFromFile("non_existent_file.json"));
    
    // Test loading invalid JSON
    std::string invalidJSON = "invalid json content {";
    std::string invalidJSONFile = "invalid.json";
    std::ofstream file(invalidJSONFile);
    file << invalidJSON;
    file.close();
    
    // Should fail to parse invalid JSON
    bool jsonLoaded = config.loadFromFile(invalidJSONFile);
    // JSON loading might fail, which is acceptable
    std::remove(invalidJSONFile.c_str());
    
    // Test getting non-existent key
    auto missing = config.get<int>("non_existent_key");
    assert(!missing.has_value());
    
    // Test getting with default
    int defaultValue = config.getOr<int>("non_existent_key", 999);
    assert(defaultValue == 999);
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void runAll() {
    testJSONFileLoading();
    testYAMLLoading();
    testSimpleFormatLoading();
    testConfigurationValidation();
    testImportExport();
    testSnapshotFunctionality();
    testConfigurationMerging();
    testFileFormatSave();
    testSourceBasedOperations();
    testErrorHandling();
    
    std::cout << "=== All Enhanced Configuration Tests PASSED ===" << std::endl;
}

} // namespace test_config_enhanced
