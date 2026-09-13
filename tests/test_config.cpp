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
    
    auto intVal = config.get<int64_t>("int_value");
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
    
    int64_t val = config.getOr<int64_t>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int64_t>("existing", 100);
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

void testConfigLoadFromJsonFile() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string tempFile = "/tmp/test_config.json";
    std::ofstream jsonFile(tempFile);
    jsonFile << R"({
        "int_value": 42,
        "double_value": 3.14,
        "string_value": "hello",
        "bool_value": true,
        "int_array": [1, 2, 3, 4, 5],
        "float_array": [1.5, 2.5, 3.5],
        "string_array": ["a", "b", "c"]
    })";
    jsonFile.close();
    
    // Load JSON file
    assert(config.loadFromFile(tempFile));
    
    // Verify values
    auto intVal = config.get<int64_t>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.001);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test arrays
    auto intArrayVal = config.get<std::vector<int64_t>>("int_array");
    assert(intArrayVal.has_value() && intArrayVal.value().size() == 5);
    assert(intArrayVal.value()[0] == 1 && intArrayVal.value()[4] == 5);
    
    auto floatArrayVal = config.get<std::vector<double>>("float_array");
    assert(floatArrayVal.has_value() && floatArrayVal.value().size() == 3);
    
    auto stringArrayVal = config.get<std::vector<std::string>>("string_array");
    assert(stringArrayVal.has_value() && stringArrayVal.value().size() == 3);
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigLoadFromJsonFile passed" << std::endl;
}

void testConfigSaveToJsonFile() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int64_t>({1, 2, 3, 4, 5}), nlm::ConfigSource::Runtime);
    config.set("float_array", std::vector<double>({1.5, 2.5, 3.5}), nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Create a temporary file
    std::string tempFile = "/tmp/test_config_save.json";
    
    // Save to JSON file
    assert(config.saveToFile(tempFile));
    
    // Read back and verify
    nlohmann::json j;
    std::ifstream inFile(tempFile);
    inFile >> j;
    
    // Verify values
    assert(j["int_value"] == 42);
    assert(std::abs(j["double_value"].get<double>() - 3.14) < 0.001);
    assert(j["string_value"] == "hello");
    assert(j["bool_value"] == true);
    assert(j["int_array"].get<std::vector<int64_t>>() == std::vector<int64_t>({1, 2, 3, 4, 5}));
    assert(std::abs(j["float_array"].get<std::vector<double>>()[0] - 1.5) < 0.001);
    assert(j["string_array"].get<std::vector<std::string>>() == std::vector<std::string>({"a", "b", "c"}));
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigSaveToJsonFile passed" << std::endl;
}

void testConfigJsonBackwardCompatibility() {
    nlm::Config config;
    
    // Create a temporary file with old key=value format
    std::string tempFile = "/tmp/test_config_old.txt";
    std::ofstream textFile(tempFile);
    textFile << "# This is a comment\n";
    textFile << "int_value = 42\n";
    textFile << "string_value = \"hello\"\n";
    textFile << "# Another comment\n";
    textFile << "double_value = 3.14\n";
    textFile.close();
    
    // Load with backward compatibility
    assert(config.loadFromFile(tempFile));
    
    // Verify values
    auto intVal = config.get<int64_t>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.001);
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigJsonBackwardCompatibility passed" << std::endl;
}

void testConfigJsonSaveWithDescription() {
    nlm::Config config;
    
    // Set values with description
    config.set("int_value", 42, nlm::ConfigSource::Runtime, "Integer value");
    config.set("string_value", "test", nlm::ConfigSource::Runtime, "String value");
    
    // Save to JSON file
    std::string tempFile = "/tmp/test_config_with_desc.json";
    assert(config.saveToFile(tempFile));
    
    // Read JSON file
    nlohmann::json j;
    std::ifstream inFile(tempFile);
    inFile >> j;
    
    // Verify keys exist
    assert(j.contains("int_value"));
    assert(j.contains("string_value"));
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigJsonSaveWithDescription passed" << std::endl;
}

void testConfigJsonEmptyFile() {
    nlm::Config config;
    
    // Create an empty file
    std::string tempFile = "/tmp/test_config_empty.json";
    std::ofstream emptyFile(tempFile);
    emptyFile.close();
    
    // Try to load empty file
    assert(config.loadFromFile(tempFile));
    assert(config.getKeys().empty());
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigJsonEmptyFile passed" << std::endl;
}

void testConfigJsonInvalidFile() {
    nlm::Config config;
    
    // Create an invalid JSON file
    std::string tempFile = "/tmp/test_config_invalid.json";
    std::ofstream invalidFile(tempFile);
    invalidFile << "{\"int_value\": invalid, \"missing_brace\"";
    invalidFile.close();
    
    // Try to load invalid file (should fallback to key=value or be empty)
    assert(config.loadFromFile(tempFile));
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigJsonInvalidFile passed" << std::endl;
}

void testConfigJsonMixedFile() {
    nlm::Config config;
    
    // Create a temporary JSON file with mixed content
    std::string tempFile = "/tmp/test_config_mixed.json";
    std::ofstream mixedFile(tempFile);
    mixedFile << R"({
        "json_key": "json_value",
        "another_key": 123,
        "bool_key": true
    })";
    mixedFile.close();
    
    // Load the file
    assert(config.loadFromFile(tempFile));
    
    // Verify JSON values were loaded
    auto jsonVal = config.get<std::string>("json_key");
    assert(jsonVal.has_value() && *jsonVal == "json_value");
    
    auto numVal = config.get<int64_t>("another_key");
    assert(numVal.has_value() && *numVal == 123);
    
    auto boolVal = config.get<bool>("bool_key");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Verify that existing config values are preserved
    config.set("existing_key", "preserved", nlm::ConfigSource::Runtime);
    auto existingVal = config.get<std::string>("existing_key");
    assert(existingVal.has_value() && *existingVal == "preserved");
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigJsonMixedFile passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLoadFromJsonFile();
    testConfigSaveToJsonFile();
    testConfigJsonBackwardCompatibility();
    testConfigJsonSaveWithDescription();
    testConfigJsonEmptyFile();
    testConfigJsonInvalidFile();
    testConfigJsonMixedFile();
    
    std::cout << std::endl;
    std::cout << "=== All Config Tests Passed ===" << std::endl;
}

} // namespace test_config
