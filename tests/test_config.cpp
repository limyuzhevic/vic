// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
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
    
    // Test setting empty key
    config.set("", 123, nlm::ConfigSource::Runtime);  // Should log error but not crash
    assert(!config.has(""));  // Should not be set
    
    // Test getting empty key
    auto val = config.get<int>("");
    assert(!val.has_value());  // Should not be found
    
    // Test getOr with empty key
    int defaultVal = config.getOr<int>("", 999);
    assert(defaultVal == 999);  // Should use default
    
    // Test has with empty key
    assert(!config.has(""));
    
    // Test removing empty key
    config.remove("");  // Should not crash
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigFileHandling() {
    nlm::Config config;
    
    // Test saving to non-existent directory
    config.set("test_value", 42, nlm::ConfigSource::Runtime);
    bool saveSuccess = config.saveToFile("/nonexistent/path/config.cfg");
    assert(!saveSuccess);  // Should fail
    
    // Test saving to temporary file
    std::string tempPath = "/tmp/nlm_test_config.cfg";
    bool saveSuccess2 = config.saveToFile(tempPath);
    assert(saveSuccess2);  // Should succeed
    
    // Clean up
    std::filesystem::remove(tempPath);
    
    std::cout << "    testConfigFileHandling passed" << std::endl;
}

void testConfigWhitespaceHandling() {
    nlm::Config config;
    
    // Test keys with whitespace
    config.set("  spaced_key  ", 123, nlm::ConfigSource::Runtime);
    assert(config.has("spaced_key"));  // Should trim whitespace
    
    auto val = config.get<int>("spaced_key");
    assert(val.has_value());
    assert(*val == 123);
    
    // Test setting empty string value
    config.set("empty_string", std::string(""), nlm::ConfigSource::Runtime);
    auto emptyVal = config.get<std::string>("empty_string");
    assert(emptyVal.has_value());
    assert(emptyVal.value().empty());
    
    std::cout << "    testConfigWhitespaceHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigErrorHandling();
    testConfigFileHandling();
    testConfigWhitespaceHandling();
}

} // namespace test_config
