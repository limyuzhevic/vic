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

void testConfigSummary() {
    nlm::Config config;
    
    config.set("test_int", 42, nlm::ConfigSource::Default);
    config.set("test_string", "hello", nlm::ConfigSource::File);
    config.set("test_double", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("test_int") != std::string::npos);
    assert(summary.find("test_string") != std::string::npos);
    assert(summary.find("test_double") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigJSONLoad() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::ofstream file("test_config.json");
    file << "{\"test_int\": 42, \"test_string\": \"hello\", \"test_double\": 3.14, \"test_bool\": true}";
    file.close();
    
    bool success = config.loadFromFile("test_config.json");
    assert(success);
    
    assert(config.has("test_int"));
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config.has("test_string"));
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    assert(config.has("test_double"));
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.01);
    
    assert(config.has("test_bool"));
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigJSONLoad passed" << std::endl;
}

void testConfigSaveJSON() {
    nlm::Config config;
    
    config.set("test_int", 42, nlm::ConfigSource::Default, "Test integer value");
    config.set("test_string", "hello world", nlm::ConfigSource::File, "Test string value");
    config.set("test_double", 3.14159, nlm::ConfigSource::CommandLine, "Test double value");
    config.set("test_bool", true, nlm::ConfigSource::Runtime, "Test boolean value");
    
    bool success = config.saveToFile("test_config_output.json");
    assert(success);
    
    // Load the saved file back
    nlm::Config config2;
    success = config2.loadFromFile("test_config_output.json");
    assert(success);
    
    // Verify values match
    assert(config2.has("test_int"));
    auto val1 = config.get<int>("test_int");
    auto val2 = config2.get<int>("test_int");
    assert(val1.has_value() && val2.has_value() && val1.value() == val2.value());
    
    assert(config2.has("test_string"));
    auto str1 = config.get<std::string>("test_string");
    auto str2 = config2.get<std::string>("test_string");
    assert(str1.has_value() && str2.has_value() && str1.value() == str2.value());
    
    // Clean up
    std::remove("test_config_output.json");
    
    std::cout << "    testConfigSaveJSON passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSummary();
    testConfigJSONLoad();
    testConfigSaveJSON();
    
    std::cout << std::endl;
    std::cout << "=== Config Tests PASSED ===" << std::endl;
}

} // namespace test_config