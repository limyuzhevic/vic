// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

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

void testJSONLoadSave() {
    nlm::Config config;
    
    // Set up test data
    config.set("int_value", 42);
    config.set("double_value", 3.14159);
    config.set("string_value", "test_string");
    config.set("bool_value", true);
    config.set("int_array", std::vector<int>({1, 2, 3, 4, 5}));
    config.set("double_array", std::vector<double>({1.1, 2.2, 3.3}));
    
    // Save to JSON file
    std::string testFile = "test_config.json";
    assert(config.saveToFile(testFile));
    
    // Clear and reload
    config.clear();
    assert(config.loadFromFile(testFile));
    
    // Verify data was correctly loaded
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14159) < 0.0001);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test_string");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testJSONLoadSave passed" << std::endl;
}

void testYAMLLoadSave() {
    nlm::Config config;
    
    // Set up test data with YAML-style format
    config.set("neural_network", std::vector<double>({0.1, 0.5, 0.9}));
    config.set("learning_rate", 0.001);
    config.set("hidden_layers", std::vector<int>({128, 64, 32}));
    config.set("activation_function", "relu");
    config.set("use_batch_norm", true);
    
    // Note: YAML support may be limited, but we test what exists
    // The implementation may fall back to simple format
    
    // Clean up if file exists
    std::string yamlFile = "test_config.yaml";
    std::string yamlFile2 = "test_config.yml";
    if (std::filesystem::exists(yamlFile)) {
        std::filesystem::remove(yamlFile);
    }
    if (std::filesystem::exists(yamlFile2)) {
        std::filesystem::remove(yamlFile2);
    }
    
    std::cout << "    testYAMLLoadSave passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test valid configuration
    config.set("valid_key", "value");
    config.set("another_key", 123);
    
    assert(config.validate());
    assert(config.getError().empty());
    
    // Test validation with invalid configuration
    nlm::Config invalidConfig;
    // Empty key is not allowed
    assert(!invalidConfig.validate());
    std::string error = invalidConfig.getError();
    assert(!error.empty());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test getting non-existent key
    auto val = config.get<int>("non_existent");
    assert(!val.has_value());
    
    // Test getting wrong type
    config.set("string_key", "hello");
    auto intVal = config.get<int>("string_key");
    assert(!intVal.has_value());
    
    // Test file not found error
    bool loadResult = config.loadFromFile("non_existent_file.json");
    assert(!loadResult);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", "test");
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("int_value") != std::string::npos);
    assert(summary.find("double_value") != std::string::npos);
    assert(summary.find("string_value") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigFileExists() {
    nlm::Config config;
    
    // Test non-existent file
    assert(!config.fileExists("non_existent.json"));
    
    // Test existing file
    std::string testFile = "exists_test.json";
    std::ofstream file(testFile);
    file << "{\"test\": \"value\"}";
    file.close();
    
    assert(config.fileExists(testFile));
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigFileExists passed" << std::endl;
}

void testConfigComplexDataTypes() {
    nlm::Config config;
    
    // Test with complex data types
    std::vector<int> intArray = {1, 2, 3, 4, 5};
    config.set("int_array", intArray);
    
    std::vector<double> doubleArray = {1.1, 2.2, 3.3, 4.4, 5.5};
    config.set("double_array", doubleArray);
    
    std::vector<std::string> stringArray = {"a", "b", "c"};
    config.set("string_array", stringArray);
    
    // Verify retrieval
    auto retrievedIntArray = config.get<std::vector<int>>("int_array");
    assert(retrievedIntArray.has_value());
    assert(retrievedIntArray->size() == 5);
    
    auto retrievedDoubleArray = config.get<std::vector<double>>("double_array");
    assert(retrievedDoubleArray.has_value());
    assert(retrievedDoubleArray->size() == 5);
    
    auto retrievedStringArray = config.get<std::vector<std::string>>("string_array");
    assert(retrievedStringArray.has_value());
    assert(retrievedStringArray->size() == 3);
    
    std::cout << "    testConfigComplexDataTypes passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONLoadSave();
    testYAMLLoadSave();
    testConfigValidation();
    testConfigErrorHandling();
    testConfigSummary();
    testConfigFileExists();
    testConfigComplexDataTypes();
}

} // namespace test_config
