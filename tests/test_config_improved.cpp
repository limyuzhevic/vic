// Improved Config Tests
#include "core/Config/ConfigImproved.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_config_improved {

void testConfigCreation() {
    nlm::ConfigImproved config;
    assert(!config.has("test"));
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::ConfigImproved config;
    
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
    nlm::ConfigImproved config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::ConfigImproved config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::ConfigImproved config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testJSONFileLoad() {
    nlm::ConfigImproved config;
    
    // Create a temporary JSON file
    std::string testFile = "test_config.json";
    std::ofstream file(testFile);
    file << "{\"test_int\": 42, \"test_string\": \"hello\", \"test_bool\": true, \"test_float\": 3.14}";
    file.close();
    
    // Load the JSON file
    bool result = config.loadFromFile(testFile);
    assert(result);
    
    // Verify values
    assert(config.get<int>("test_int").has_value());
    assert(config.get<std::string>("test_string").has_value());
    assert(config.get<bool>("test_bool").has_value());
    assert(config.get<double>("test_float").has_value());
    
    assert(config.get<int>("test_int").value() == 42);
    assert(config.get<std::string>("test_string").value() == "hello");
    assert(config.get<bool>("test_bool").value() == true);
    assert(config.get<double>("test_float").value() > 3.13 && config.get<double>("test_float").value() < 3.15);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testJSONFileLoad passed" << std::endl;
}

void testINIFileLoad() {
    nlm::ConfigImproved config;
    
    // Create a temporary INI file
    std::string testFile = "test_config.cfg";
    std::ofstream file(testFile);
    file << "# Test comment\ntest_int = 42\ntest_string = hello\ntest_bool = true\ntest_float = 3.14";
    file.close();
    
    // Load the INI file
    bool result = config.loadFromFile(testFile);
    assert(result);
    
    // Verify values
    assert(config.get<int>("test_int").has_value());
    assert(config.get<std::string>("test_string").has_value());
    assert(config.get<bool>("test_bool").has_value());
    assert(config.get<double>("test_float").has_value());
    
    assert(config.get<int>("test_int").value() == 42);
    assert(config.get<std::string>("test_string").value() == "hello");
    assert(config.get<bool>("test_bool").value() == true);
    assert(config.get<double>("test_float").value() > 3.13 && config.get<double>("test_float").value() < 3.15);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testINIFileLoad passed" << std::endl;
}

void testSchemaValidation() {
    nlm::ConfigImproved config;
    
    // Define a simple schema
    std::vector<nlm::ConfigSchemaEntry> schema = {
        nlm::ConfigSchemaEntry("required_int", "int", 0, "Required integer parameter", nullptr, true),
        nlm::ConfigSchemaEntry("optional_string", "string", std::string(""), "Optional string parameter", nullptr, false),
        nlm::ConfigSchemaEntry("required_float", "float", 0.0f, "Required float parameter", nullptr, true)
    };
    
    config.setSchema(schema);
    
    // Set required values
    config.set("required_int", 42);
    config.set("optional_string", std::string("test"));
    config.set("required_float", 3.14);
    
    // Validate - should pass
    auto validation = config.validateAll();
    assert(validation.valid);
    
    // Test missing required value
    nlm::ConfigImproved config2;
    config2.setSchema(schema);
    config2.set("optional_string", std::string("test"));
    
    validation = config2.validateAll();
    assert(!validation.valid);
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONFileLoad();
    testINIFileLoad();
    testSchemaValidation();
}

} // namespace test_config_improved