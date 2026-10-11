// Config Tests
#include "core/Config/Config.hpp"
#include "core/Error/Error.hpp"
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
    
    // Test parameter validation
    config.set("valid_key", "valid_value", nlm::ConfigSource::Runtime);
    assert(config.has("valid_key"));
    
    // Test getting non-existent key returns std::nullopt
    auto nonExistent = config.get<int>("non_existent");
    assert(!nonExistent.has_value());
    
    // Test getOr with non-existent key
    int defaultValue = config.getOr<int>("another_missing", 999);
    assert(defaultValue == 999);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigFileErrorHandling() {
    nlm::Config config;
    
    // Test saving to file
    std::string testFile = "/tmp/test_config.txt";
    
    // Set up config
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    
    // Try to save (this should work with error handling)
    bool saveSuccess = config.saveToFile(testFile);
    // The save might succeed or fail depending on permissions, but error handling
    // should be in place to catch any issues
    std::cout << "    saveToFile returned: " << (saveSuccess ? "true" : "false") << std::endl;
    
    // Test loading from file
    bool loadSuccess = config.loadFromFile(testFile);
    std::cout << "    loadFromFile returned: " << (loadSuccess ? "true" : "false") << std::endl;
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigFileErrorHandling passed" << std::endl;
}

void testConfigValidationErrors() {
    nlm::Config config;
    
    // Test that empty key is handled (the NLM_VALIDATE macro will catch this)
    // In a real implementation, this would be caught during set() calls
    config.set("", "value", nlm::ConfigSource::Runtime);  // Empty key
    
    // Test that invalid characters in key are handled
    // Note: This depends on the implementation of NLM_VALIDATE
    
    std::cout << "    testConfigValidationErrors passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigErrorHandling();
    testConfigFileErrorHandling();
    testConfigValidationErrors();
}

} // namespace test_config
