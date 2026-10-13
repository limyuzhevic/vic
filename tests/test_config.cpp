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

void testConfigJSONSaveLoad() {
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string jsonFile = "test_config.json";
    assert(config.saveToFile(jsonFile));
    
    // Create new config and load from JSON
    nlm::Config config2;
    assert(config2.loadFromFile(jsonFile));
    
    // Verify values
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config2.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test");
    
    auto boolVal = config2.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    std::cout << "    testConfigJSONSaveLoad passed" << std::endl;
}

void testConfigSimpleFormatSaveLoad() {
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    
    // Save to simple format file
    std::string simpleFile = "test_config.txt";
    assert(config.saveToFile(simpleFile));
    
    // Verify file was created and has content
    std::ifstream file(simpleFile);
    assert(file.good());
    
    // Create new config and load from simple format
    nlm::Config config2;
    assert(config2.loadFromFile(simpleFile));
    
    // Verify values (simple format preserves original types)
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config2.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test");
    
    // Clean up
    std::remove(simpleFile.c_str());
    
    std::cout << "    testConfigSimpleFormatSaveLoad passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test validation of different value types
    config.set("int_val", 123, nlm::ConfigSource::Runtime);
    config.set("double_val", 45.67, nlm::ConfigSource::Runtime);
    config.set("string_val", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Test vector types
    config.set("int_vector", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("string_vector", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Verify all values are accessible
    assert(config.has("int_val"));
    assert(config.has("double_val"));
    assert(config.has("string_val"));
    assert(config.has("bool_val"));
    assert(config.has("int_vector"));
    assert(config.has("string_vector"));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigFileNotFound() {
    nlm::Config config;
    
    // Try to load non-existent file
    bool result = config.loadFromFile("non_existent_file_12345.json");
    assert(!result);  // Should return false for non-existent file
    
    std::cout << "    testConfigFileNotFound passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONSaveLoad();
    testConfigSimpleFormatSaveLoad();
    testConfigValidation();
    testConfigFileNotFound();
}

} // namespace test_config
