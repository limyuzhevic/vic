// Enhanced Config Tests
// These tests cover the improved Config functionality including validation, error handling, and edge cases
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

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
    
    // Test invalid key (empty string)
    try {
        config.set("", "value", nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown exception for empty key");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("empty key") != std::string::npos);
    }
    
    // Test invalid key (contains equals sign)
    try {
        config.set("key=value", "test", nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown exception for key with equals sign");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("equals sign") != std::string::npos);
    }
    
    // Test invalid key (contains spaces)
    try {
        config.set("key with spaces", "test", nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown exception for key with spaces");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("spaces") != std::string::npos);
    }
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("neuron_count", 1000, nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config.set("region_count", 2, nlm::ConfigSource::Default);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("neuron_count") != std::string::npos);
    assert(summary.find("simulation_timestep") != std::string::npos);
    assert(summary.find("region_count") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigTypeMismatch() {
    nlm::Config config;
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    
    // Try to get as int (should return nullopt)
    auto intVal = config.get<int>("string_value");
    assert(!intVal.has_value());
    
    // Try to get as double (should return nullopt)
    auto doubleVal = config.get<double>("string_value");
    assert(!doubleVal.has_value());
    
    std::cout << "    testConfigTypeMismatch passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    config.set("key1", 100, nlm::ConfigSource::File);
    config.set("key2", 200, nlm::ConfigSource::CommandLine);
    config.set("key3", 300, nlm::ConfigSource::Default);
    
    auto keys = config.getKeys();
    
    // Check that all keys are present
    bool hasKey1 = std::any_of(keys.begin(), keys.end(), 
                              [](const std::string& key) { return key == "key1"; });
    bool hasKey2 = std::any_of(keys.begin(), keys.end(), 
                              [](const std::string& key) { return key == "key2"; });
    bool hasKey3 = std::any_of(keys.begin(), keys.end(), 
                              [](const std::string& key) { return key == "key3"; });
    
    assert(hasKey1 && hasKey2 && hasKey3);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigSummary();
    testConfigTypeMismatch();
    testConfigSourceTracking();
    
    std::cout << "=== All Config Tests Passed ===" << std::endl;
}

} // namespace test_config
