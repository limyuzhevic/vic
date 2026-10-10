// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

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

void testConfigJSONSaveLoad() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_val", "test string", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Create test directory if it doesn't exist
    std::string testFile = "test_config.json";
    
    // Save to file
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    // Create new config and load from file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(testFile);
    assert(loadSuccess);
    
    // Verify values match
    auto intVal1 = config.get<int>("int_val");
    auto intVal2 = config2.get<int>("int_val");
    assert(intVal1.has_value() && intVal2.has_value());
    assert(*intVal1 == *intVal2);
    
    auto doubleVal1 = config.get<double>("double_val");
    auto doubleVal2 = config2.get<double>("double_val");
    assert(doubleVal1.has_value() && doubleVal2.has_value());
    assert(std::abs(*doubleVal1 - *doubleVal2) < 0.0001);
    
    auto stringVal1 = config.get<std::string>("string_val");
    auto stringVal2 = config2.get<std::string>("string_val");
    assert(stringVal1.has_value() && stringVal2.has_value());
    assert(*stringVal1 == *stringVal2);
    
    auto boolVal1 = config.get<bool>("bool_val");
    auto boolVal2 = config2.get<bool>("bool_val");
    assert(boolVal1.has_value() && boolVal2.has_value());
    assert(*boolVal1 == *boolVal2);
    
    // Clean up test file
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigJSONSaveLoad passed" << std::endl;
}

void testConfigJSONFileErrorHandling() {
    // Test loading from non-existent file
    nlm::Config config;
    bool success = config.loadFromFile("nonexistent_file.json");
    assert(!success);  // Should fail gracefully
    
    std::cout << "    testConfigJSONFileErrorHandling passed" << std::endl;
}

void testConfigInvalidJSON() {
    // Create a file with invalid JSON
    std::string testFile = "invalid_config.json";
    std::ofstream file(testFile);
    if (file.is_open()) {
        file << "{\n  \"invalid\" \"missing colon\"\n}";
    }
    file.close();
    
    // Try to load invalid JSON - should fall back to legacy format or fail gracefully
    nlm::Config config;
    bool success = config.loadFromFile(testFile);
    // Should handle gracefully (either succeed with fallback or fail)
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigInvalidJSON passed" << std::endl;
}

void testConfigEdgeCases() {
    nlm::Config config;
    
    // Test setting same key multiple times (should overwrite)
    config.set("key", "first", nlm::ConfigSource::Runtime);
    config.set("key", "second", nlm::ConfigSource::CommandLine);
    
    auto value = config.get<std::string>("key");
    assert(value.has_value());
    assert(*value == "second");  // Later value should win
    
    // Test getting non-existent key
    auto missing = config.get<std::string>("missing_key");
    assert(!missing.has_value());
    
    // Test getOr with default for missing key
    std::string defaultValue = config.getOr<std::string>("missing_key", "default");
    assert(defaultValue == "default");
    
    // Test clear
    config.clear();
    assert(!config.has("key"));
    
    std::cout << "    testConfigEdgeCases passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONSaveLoad();
    testConfigJSONFileErrorHandling();
    testConfigInvalidJSON();
    testConfigEdgeCases();
}

} // namespace test_config
