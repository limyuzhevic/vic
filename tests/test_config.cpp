// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
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

void testConfigJsonLoadSave() {
    nlm::Config config;
    
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    config.set("test_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.setDescription("test_int", "Test integer value");
    
    // Save to temporary file
    std::ofstream outfile("test_config.json");
    config.saveToFile("test_config.json");
    outfile.close();
    
    // Clear and load back
    config.clear();
    assert(config.loadFromFile("test_config.json"));
    
    // Verify values
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value());
    assert(*stringVal == "hello world");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    auto vectorVal = config.get<std::vector<int>>("test_vector");
    assert(vectorVal.has_value());
    assert(vectorVal->size() == 3);
    assert((*vectorVal)[0] == 1 && (*vectorVal)[1] == 2 && (*vectorVal)[2] == 3);
    
    // Check metadata was preserved
    assert(config.hasDescription("test_int"));
    assert(config.getDescription("test_int") == "Test integer value");
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigJsonLoadSave passed" << std::endl;
}

void testConfigJsonErrorHandling() {
    nlm::Config config;
    
    // Test invalid JSON file
    std::ofstream invalidFile("invalid_config.json");
    invalidFile << "invalid json content";
    invalidFile.close();
    
    assert(!config.loadFromFile("invalid_config.json"));
    
    // Test empty JSON file
    std::ofstream emptyFile("empty_config.json");
    emptyFile.close();
    
    assert(!config.loadFromFile("empty_config.json"));
    
    // Test non-existent file
    assert(!config.loadFromFile("non_existent_config.json"));
    
    // Clean up
    std::remove("invalid_config.json");
    std::remove("empty_config.json");
    
    std::cout << "    testConfigJsonErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJsonLoadSave();
    testConfigJsonErrorHandling();
}

} // namespace test_config
