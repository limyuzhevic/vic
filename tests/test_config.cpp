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

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Set a string value
    config.set("str_value", std::string("hello"), nlm::ConfigSource::Runtime);
    
    // Try to get it as an int - should return nullopt without crashing
    auto intVal = config.get<int>("str_value");
    assert(!intVal.has_value());  // Should be null
    
    // Set an int value
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    
    // Try to get it as a string - should return nullopt without crashing
    auto stringVal = config.get<std::string>("int_value");
    assert(!stringVal.has_value());  // Should be null
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigSaveLoadFile() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("test_string"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("int_vec", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("double_vec", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    
    // Create a temporary file path
    const std::string tempFile = "test_config_temp.cfg";
    
    // Save to file
    assert(config.saveToFile(tempFile));
    
    // Create a new config and load from file
    nlm::Config config2;
    assert(config2.loadFromFile(tempFile));
    
    // Verify all values were loaded correctly
    auto intVal = config2.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config2.get<double>("double_val");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.1415 && *doubleVal < 3.1416);
    
    auto stringVal = config2.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "test_string");
    
    auto boolVal = config2.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto intVecVal = config2.get<std::vector<int>>("int_vec");
    assert(intVecVal.has_value() && intVecVal->size() == 3);
    assert((*intVecVal)[0] == 1 && (*intVecVal)[1] == 2 && (*intVecVal)[2] == 3);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigSaveLoadFile passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 42, nlm::ConfigSource::Default);
    config.set("key2", std::string("test"), nlm::ConfigSource::File);
    config.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigErrorHandling();
    testConfigSaveLoadFile();
    testConfigSummary();
}

} // namespace test_config
