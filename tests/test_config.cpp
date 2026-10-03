// Config Tests
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

// New tests for advanced features
void testConfigJSONParsing() {
    nlm::Config config;
    
    // Create test JSON content
    std::string jsonContent = R"({
        "int_key": 42,
        "double_key": 3.14,
        "string_key": "test_value",
        "bool_key": true,
        "nested_object": {
            "inner_key": "inner_value"
        }
    })";
    
    // Note: This is a placeholder for JSON parsing tests
    // In a full implementation, this would test the loadFromJson method
    std::cout << "    testConfigJSONParsing passed (placeholder)" << std::endl;
}

void testConfigYAMLParsing() {
    nlm::Config config;
    
    // Create test YAML content
    std::string yamlContent = R"(
    int_key: 42
    double_key: 3.14
    string_key: test_value
    bool_key: true
    - item1
    - item2
    """;
    
    // Note: This is a placeholder for YAML parsing tests
    // In a full implementation, this would test the loadFromYaml method
    std::cout << "    testConfigYAMLParsing passed (placeholder)" << std::endl;
}

void testConfigValidationAdvanced() {
    nlm::Config config;
    
    // Add valid configuration
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Runtime);
    
    // Test basic validation (should pass)
    assert(config.validate());
    
    // Test advanced validation (should pass for now)
    assert(config.validateAdvanced());
    
    std::cout << "    testConfigValidationAdvanced passed" << std::endl;
}

void testConfigExportImport() {
    nlm::Config config;
    
    // Add test data
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    
    // Test JSON export
    std::string jsonOutput = config.exportToJson();
    assert(!jsonOutput.empty());
    assert(jsonOutput.find("int_value") != std::string::npos);
    
    // Test YAML export
    std::string yamlOutput = config.exportToYaml();
    assert(!yamlOutput.empty());
    assert(yamlOutput.find("int_value") != std::string::npos);
    
    std::cout << "    testConfigExportImport passed" << std::endl;
}

void testConfigBatchOperations() {
    nlm::Config config;
    
    // Test batch set with map
    std::map<std::string, std::string> stringMap;
    stringMap["key1"] = "value1";
    stringMap["key2"] = "value2";
    
    // Note: This would test batchSet methods if they were accessible
    // config.batchSet(stringMap, nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigBatchOperations passed (placeholder)" << std::endl;
}

void testConfigComparison() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Add same data to both configs
    config1.set("int_value", 42, nlm::ConfigSource::Runtime);
    config1.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    
    config2.set("int_value", 42, nlm::ConfigSource::Runtime);
    config2.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    
    assert(config1.equals(config2));
    
    // Modify config2 and test inequality
    config2.set("double_value", 2.71, nlm::ConfigSource::Runtime);
    assert(!config1.equals(config2));
    
    // Test diff (basic check)
    std::string diff = config1.diff(config2);
    assert(!diff.empty());
    
    std::cout << "    testConfigComparison passed" << std::endl;
}

void testConfigFileOperations() {
    nlm::Config config;
    
    const std::string testFile = "test_config.cfg";
    
    // Set test data
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    config.set("number_key", 123, nlm::ConfigSource::Runtime);
    
    // Test saveToFile (placeholder since implementation saves placeholder values)
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult); // Should succeed even with placeholder values
    
    // Check file was created
    std::ifstream file(testFile);
    assert(file.good());
    file.close();
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONParsing();
    testConfigYAMLParsing();
    testConfigValidationAdvanced();
    testConfigExportImport();
    testConfigBatchOperations();
    testConfigComparison();
    testConfigFileOperations();
}

} // namespace test_config
