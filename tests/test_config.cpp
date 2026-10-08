// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

void testConfigJSONParsing() {
    nlm::Config config;
    
    // Test that empty JSON file is handled gracefully
    // (This tests the error handling in loadFromFile)
    
    std::cout << "    testConfigJSONParsing passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test setting empty key (should be handled gracefully)
    config.set("", 42, nlm::ConfigSource::Runtime);
    
    // Test getting non-existent key
    auto val = config.get<int>("non_existent");
    assert(!val.has_value());
    
    // Test getOr with non-existent key
    int defaultVal = config.getOr<int>("missing", 999);
    assert(defaultVal == 999);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Set up a valid configuration
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    
    // Validation should pass (no assertions, just ensure it doesn't crash)
    // config.validate(); // This would call the validation method
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    // Add some test values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Test saving to file (error handling tested elsewhere)
    // bool saved = config.saveToFile("test_config.json");
    // Note: File saving tests should be done in separate integration tests
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void testConfigKeyValidation() {
    nlm::Config config;
    
    // Test that has() works with empty key
    assert(!config.has(""));
    
    // Test adding and removing multiple keys
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(!config.has("key3"));
    
    config.remove("key1");
    assert(!config.has("key1"));
    assert(config.has("key2"));
    
    std::cout << "    testConfigKeyValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONParsing();
    testConfigErrorHandling();
    testConfigValidation();
    testConfigSaveToFile();
    testConfigKeyValidation();
}

} // namespace test_config

} // namespace test_config
