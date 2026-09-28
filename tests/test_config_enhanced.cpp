// Enhanced Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>

namespace test_config_enhanced {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    // Test all basic types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("int64_value", static_cast<int64_t>(999), nlm::ConfigSource::Runtime);
    auto int64Val = config.get<int64_t>("int64_value");
    assert(int64Val.has_value());
    assert(*int64Val == 999);
    
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
    
    // Test getOr with defaults
    assert(config.getOr<int>("missing", 100) == 100);
    assert(config.getOr<std::string>("missing", "default") == "default");
    
    // Test empty key handling
    try {
        config.set("", 123, nlm::ConfigSource::Runtime);
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    // Test with missing keys
    assert(config.getOr<int>("missing", 100) == 100);
    assert(config.getOr<double>("missing", 3.14) == 3.14);
    assert(config.getOr<std::string>("missing", "default") == "default");
    assert(config.getOr<bool>("missing", false) == false);
    
    // Test with existing keys
    config.set("existing_int", 50, nlm::ConfigSource::Runtime);
    assert(config.getOr<int>("existing_int", 100) == 50);
    
    config.set("existing_string", std::string("test"), nlm::ConfigSource::Runtime);
    assert(config.getOr<std::string>("existing_string", "default") == "test");
    
    // Test with wrong type (should return default)
    config.set("wrong_type", std::string("text"), nlm::ConfigSource::Runtime);
    assert(config.getOr<int>("wrong_type", 777) == 777);  // Wrong type, returns default
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    // Add multiple entries
    config.set("test1", 123, nlm::ConfigSource::Runtime);
    config.set("test2", 456, nlm::ConfigSource::Runtime);
    config.set("test3", std::string("hello"), nlm::ConfigSource::Runtime);
    
    assert(config.has("test1"));
    assert(config.has("test2"));
    assert(config.has("test3"));
    assert(config.getKeys().size() == 3);
    
    // Remove one entry
    config.remove("test2");
    assert(!config.has("test2"));
    assert(config.has("test1"));
    assert(config.has("test3"));
    assert(config.getKeys().size() == 2);
    
    // Remove remaining entries
    config.remove("test1");
    config.remove("test3");
    assert(!config.has("test1"));
    assert(!config.has("test3"));
    assert(config.getKeys().empty());
    
    // Try removing non-existent key (should not crash)
    config.remove("nonexistent");
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    // Add multiple entries in specific order
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    config.set("key1", 100, nlm::ConfigSource::Runtime);  // Update existing
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);  // Should be 3 unique keys
    
    // Verify keys are present (order not guaranteed)
    bool hasKey1 = std::find(keys.begin(), keys.end(), "key1") != keys.end();
    bool hasKey2 = std::find(keys.begin(), keys.end(), "key2") != keys.end();
    bool hasKey3 = std::find(keys.begin(), keys.end(), "key3") != keys.end();
    
    assert(hasKey1);
    assert(hasKey2);
    assert(hasKey3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    // Add various types
    config.set("int_key", 42, nlm::ConfigSource::Default);
    config.set("string_key", "test_value", nlm::ConfigSource::File);
    config.set("double_key", 3.14159, nlm::ConfigSource::CommandLine);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    assert(summary.find("int_key") != std::string::npos);
    assert(summary.find("string_key") != std::string::npos);
    assert(summary.find("double_key") != std::string::npos);
    assert(summary.find("bool_key") != std::string::npos);
    assert(summary.find("4") != std::string::npos);  // Number of entries
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    // Add some entries
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    assert(config.getKeys().size() == 3);
    assert(config.has("key1"));
    
    // Clear all entries
    config.clear();
    assert(config.getKeys().empty());
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(!config.has("key3"));
    
    // Should be able to add entries after clear
    config.set("new_key", 999, nlm::ConfigSource::Runtime);
    assert(config.has("new_key"));
    assert(config.getOr<int>("new_key", 0) == 999);
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test type safety
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    
    // Wrong type access should return nullopt
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // This should return nullopt (wrong type)
    auto stringVal = config.get<std::string>("int_val");
    assert(!stringVal.has_value());  // Wrong type, should fail
    
    // getOr should still return default for wrong type
    assert(config.getOr<std::string>("int_val", "default") == "default");
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSummary();
    testConfigClear();
    testConfigErrorHandling();
}

} // namespace test_config_enhanced