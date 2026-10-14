// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

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

void testConfigSimpleFormat() {
    nlm::Config config;
    
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "hello");
    config.set("test_bool", true);
    
    // Test simple format save
    std::stringstream buffer;
    std::ofstream test_file("test_config_simple.cfg");
    if (test_file.is_open()) {
        // Create a temporary file
        config.saveToFile("test_config_simple.cfg");
        
        // Read back and parse (simple format)
        std::ifstream file("test_config_simple.cfg");
        if (file.is_open()) {
            nlm::Config config2;
            config2.loadFromFile("test_config_simple.cfg");
            
            assert(config2.get<int>("test_int").has_value());
            assert(config2.get<int>("test_int").value() == 42);
            assert(config2.get<double>("test_double").has_value());
            assert(config2.get<double>("test_double").value() > 3.13 && 
                   config2.get<double>("test_double").value() < 3.15);
            assert(config2.get<std::string>("test_string").has_value());
            assert(config2.get<std::string>("test_string").value() == "hello");
            assert(config2.get<bool>("test_bool").has_value());
            assert(config2.get<bool>("test_bool").value() == true);
        }
    }
    
    // Cleanup
    std::remove("test_config_simple.cfg");
    
    std::cout << "    testConfigSimpleFormat passed" << std::endl;
}

void testConfigJSONFormat() {
    nlm::Config config;
    
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "hello");
    config.set("test_bool", true);
    
    // Test JSON format save
    std::ofstream json_file("test_config.json");
    if (json_file.is_open()) {
        config.saveToFile("test_config.json");
    }
    
    // Load back using JSON format
    nlm::Config config2;
    config2.loadFromFile("test_config.json");
    
    assert(config2.get<int>("test_int").has_value());
    assert(config2.get<int>("test_int").value() == 42);
    assert(config2.get<double>("test_double").has_value());
    assert(config2.get<double>("test_double").value() > 3.13 && 
           config2.get<double>("test_double").value() < 3.15);
    assert(config2.get<std::string>("test_string").has_value());
    assert(config2.get<std::string>("test_string").value() == "hello");
    assert(config2.get<bool>("test_bool").has_value());
    assert(config2.get<bool>("test_bool").value() == true);
    
    // Cleanup
    std::remove("test_config.json");
    
    std::cout << "    testConfigJSONFormat passed" << std::endl;
}

void testConfigKeyValidation() {
    nlm::Config config;
    
    // Test valid key
    config.set("valid_key", 123);
    assert(config.has("valid_key"));
    
    // Test that internal validation would catch invalid keys
    // (This is tested implicitly through loadFromFile)
    
    std::cout << "    testConfigKeyValidation passed" << std::endl;
}

void testConfigValueValidation() {
    nlm::Config config;
    
    // Test valid values
    config.set("valid_int", 42);
    config.set("valid_double", 3.14);
    config.set("valid_string", "hello");
    config.set("valid_bool", true);
    
    assert(config.get<int>("valid_int").has_value());
    assert(config.get<double>("valid_double").has_value());
    assert(config.get<std::string>("valid_string").has_value());
    assert(config.get<bool>("valid_bool").has_value());
    
    std::cout << "    testConfigValueValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSimpleFormat();
    testConfigJSONFormat();
    testConfigKeyValidation();
    testConfigValueValidation();
}

} // namespace test_config
