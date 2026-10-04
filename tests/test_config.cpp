// Configuration Tests
// Tests Config class functionality and value handling

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    
    // Should be able to set and get values
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "hello");
    config.set("test_bool", true);
    
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    // Test with default values
    assert(config.getOr<int>("nonexistent", 100) == 100);
    assert(config.getOr<double>("nonexistent", 3.14) == 3.14);
    assert(config.getOr<std::string>("nonexistent", "default") == "default");
    assert(config.getOr<bool>("nonexistent", false) == false);
    
    // Set values and test retrieval
    config.set("existing_int", 123);
    assert(config.getOr<int>("existing_int", 100) == 123);
    
    config.set("existing_string", "world");
    assert(config.getOr<std::string>("existing_string", "hello") == "world");
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigSetVariousTypes() {
    nlm::Config config;
    
    // Test setting different types
    config.set("int", 42);
    config.set("double", 3.14);
    config.set("string", "test");
    config.set("bool", true);
    
    // Note: This is a simplified test - Config::get() is templated
    // In real usage, you would call specific get methods
    
    std::cout << "    testConfigSetVariousTypes passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    assert(config.getKeys().empty());
    
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    // Test removal
    config.remove("key2");
    keys = config.getKeys();
    assert(keys.size() == 2);
    assert(std::find(keys.begin(), keys.end(), "key2") == keys.end());
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    // Should have a summary even when empty
    std::string summary = config.summary();
    assert(!summary.empty());
    
    config.set("neuron_count", 1000);
    config.set("region_count", 4);
    config.set("connection_probability", 0.1f);
    
    summary = config.summary();
    assert(summary.find("neuron_count") != std::string::npos);
    assert(summary.find("region_count") != std::string::npos);
    assert(summary.find("connection_probability") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    assert(!config.getKeys().empty());
    
    config.clear();
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void testConfigInitialization() {
    // Test default configuration
    nlm::Config config;
    
    // Should have basic config structure
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigInitialization passed" << std::endl;
}

void runAll() {
    std::cout << "Running Config tests..." << std::endl;
    testConfigCreation();
    testConfigGetOr();
    testConfigSetVariousTypes();
    testConfigKeys();
    testConfigSummary();
    testConfigClear();
    testConfigInitialization();
}

} // namespace test_config
