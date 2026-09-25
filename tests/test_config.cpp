// Config Tests
#include "core/Config/Config.hpp"
#include "core/Error.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <fstream>

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

void testConfigValidation() {
    nlm::Config config;
    
    // Add valid config entries
    config.set("app_name", std::string("TestApp"), nlm::ConfigSource::Runtime);
    config.set("version", std::string("1.0"), nlm::ConfigSource::Runtime);
    config.set("max_connections", 100, nlm::ConfigSource::Runtime);
    config.set("timeout", 5.5, nlm::ConfigSource::Runtime);
    
    // Should pass validation
    assert(config.validate());
    
    // Remove required key
    config.remove("app_name");
    assert(!config.validate());
    
    // Get validation errors
    auto errors = config.getValidationErrors();
    assert(errors.size() > 0);
    assert(errors[0].find("app_name") != std::string::npos);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigFileLoad() {
    nlm::Config config;
    
    // Create a temporary config file
    std::ofstream tempFile("test_config.json");
    if (tempFile.is_open()) {
        tempFile << "{\n  \"test_key\": 42,\n  \"string_value\": \"test\"\n}";
        tempFile.close();
        
        try {
            config.loadFromFile("test_config.json");
            assert(config.has("test_key"));
            assert(config.has("string_value"));
            
            auto val = config.get<int>("test_key");
            assert(val.has_value() && *val == 42);
            
            auto strVal = config.get<std::string>("string_value");
            assert(strVal.has_value() && *strVal == "test");
            
            std::cout << "    testConfigFileLoad passed" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    testConfigFileLoad failed: " << e.what() << std::endl;
            throw;
        }
    }
    
    // Clean up
    std::remove("test_config.json");
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    try {
        // Try to load a non-existent file
        config.loadFromFile("non_existent_file.json");
        assert(false && "Should have thrown an exception");
    } catch (const nlm::ConfigError& e) {
        // Expected exception
        assert(std::string(e.what()).find("test_config.cpp") != std::string::npos ||
               std::string(e.what()).find("non_existent_file.json") != std::string::npos);
        std::cout << "    testConfigErrorHandling passed" << std::endl;
    }
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigFileLoad();
    testConfigErrorHandling();
}

} // namespace test_config
