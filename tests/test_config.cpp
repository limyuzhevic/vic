// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <string>

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
    
    // Test setting with empty key (should be rejected)
    config.set("", 123, nlm::ConfigSource::Runtime);  // Should log error but continue
    
    // Test setting with newline in key (should be rejected)
    config.set("bad\nkey", 456, nlm::ConfigSource::Runtime);  // Should log error but continue
    
    // Test valid setting
    config.set("valid_key", 789, nlm::ConfigSource::Runtime);
    assert(config.has("valid_key"));
    
    auto val = config.get<int>("valid_key");
    assert(val.has_value());
    assert(*val == 789);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test that config operations don't crash on invalid input
    config.set("test", "valid", nlm::ConfigSource::Runtime);
    
    // Test setting with various types
    config.set("int", 42, nlm::ConfigSource::Runtime);
    config.set("double", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool", false, nlm::ConfigSource::Runtime);
    config.set("string", std::string("test"), nlm::ConfigSource::Runtime);
    
    // Verify all settings
    assert(config.getOr<int>("int", 0) == 42);
    assert(config.getOr<double>("double", 0.0) > 3.13 && config.getOr<double>("double", 0.0) < 3.15);
    assert(config.getOr<bool>("bool", true) == false);
    assert(config.getOr<std::string>("string", "default") == "test");
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    // Add various configuration entries
    config.set("neuron_count", 1000, nlm::ConfigSource::Default);
    config.set("region_count", 3, nlm::ConfigSource::Default);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    config.set("plasticity_rate", 0.001, nlm::ConfigSource::Default);
    
    // Generate summary
    std::string summary = config.summary();
    
    // Verify summary contains expected entries
    assert(summary.find("neuron_count") != std::string::npos);
    assert(summary.find("region_count") != std::string::npos);
    assert(summary.find("connection_probability") != std::string::npos);
    assert(summary.find("plasticity_rate") != std::string::npos);
    
    // Verify summary contains entry count
    assert(summary.find("4 entries") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigErrorHandling();
    testConfigSummary();
}

} // namespace test_config
