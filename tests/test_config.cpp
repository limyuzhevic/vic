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

// New JSON-related tests
void testJsonSaveToFile() {
    nlm::Config config;
    
    // Set up test data
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    std::string testFile = "test_config.json";
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Verify file exists
    assert(std::filesystem::exists(testFile));
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testJsonSaveToFile passed" << std::endl;
}

void testJsonLoadFromFile() {
    nlm::Config config;
    
    // Create test file with JSON content
    std::string testFile = "test_config.json";
    std::ofstream file(testFile);
    file << R"({
        "config": {
            "int_value": 100,
            "double_value": 2.718,
            "string_value": "test_string",
            "bool_value": false,
            "int_vector": [1, 2, 3],
            "double_vector": [1.1, 2.2, 3.3],
            "string_vector": ["a", "b", "c"]
        }
    })";
    file.close();
    
    // Load the file
    bool loadResult = config.loadFromFile(testFile);
    assert(loadResult);
    
    // Verify loaded values
    assert(config.getOr<int>("int_value", 0) == 100);
    assert(config.getOr<double>("double_value", 0.0) > 2.717 && config.getOr<double>("double_value", 0.0) < 2.719);
    assert(config.getOr<std::string>("string_value", "") == "test_string");
    assert(config.getOr<bool>("bool_value", true) == false);
    
    // Verify vector types
    auto intVector = config.get<std::vector<int>>("int_vector");
    assert(intVector.has_value());
    assert(intVector->size() == 3);
    
    auto doubleVector = config.get<std::vector<double>>("double_vector");
    assert(doubleVector.has_value());
    assert(doubleVector->size() == 3);
    
    auto stringVector = config.get<std::vector<std::string>>("string_vector");
    assert(stringVector.has_value());
    assert(stringVector->size() == 3);
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testJsonLoadFromFile passed" << std::endl;
}

void testJsonLoadFromString() {
    nlm::Config config;
    
    std::string jsonString = R"({
        "config": {
            "int_value": 200,
            "string_value": "from_json_string"
        }
    })";
    
    bool loadResult = config.loadFromJsonString(jsonString);
    assert(loadResult);
    
    assert(config.getOr<int>("int_value", 0) == 200);
    assert(config.getOr<std::string>("string_value", "") == "from_json_string");
    
    std::cout << "    testJsonLoadFromString passed" << std::endl;
}

void testJsonSaveToString() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "test_value", nlm::ConfigSource::Runtime);
    
    std::string jsonString = config.saveToJsonString();
    assert(!jsonString.empty());
    
    // Verify it's valid JSON
    nlohmann::json j = nlohmann::json::parse(jsonString);
    assert(j.contains("config"));
    assert(j["config"]["int_value"] == 42);
    assert(j["config"]["string_value"] == "test_value");
    
    std::cout << "    testJsonSaveToString passed" << std::endl;
}

void testConfigValueTypes() {
    nlm::Config config;
    
    // Test all ConfigValue types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.getOr<int>("int_value", 0) == 42);
    
    config.set("int64_value", static_cast<int64_t>(9223372036854775807), nlm::ConfigSource::Runtime);
    assert(config.getOr<int64_t>("int64_value", 0) == 9223372036854775807LL);
    
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    assert(config.getOr<double>("double_value", 0.0) > 3.14 && config.getOr<double>("double_value", 0.0) < 3.15);
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    assert(config.getOr<bool>("bool_value", false) == true);
    
    config.set("string_value", "test_string", nlm::ConfigSource::Runtime);
    assert(config.getOr<std::string>("string_value", "") == "test_string");
    
    // Test vectors
    config.set("int_vector", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    auto intVector = config.get<std::vector<int>>("int_vector");
    assert(intVector.has_value() && intVector->size() == 3);
    
    config.set("double_vector", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    auto doubleVector = config.get<std::vector<double>>("double_vector");
    assert(doubleVector.has_value() && doubleVector->size() == 3);
    
    config.set("string_vector", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    auto stringVector = config.get<std::vector<std::string>>("string_vector");
    assert(stringVector.has_value() && stringVector->size() == 3);
    
    std::cout << "    testConfigValueTypes passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // New JSON tests
    testJsonSaveToFile();
    testJsonLoadFromFile();
    testJsonLoadFromString();
    testJsonSaveToString();
    testConfigValueTypes();
}

} // namespace test_config
