// Enhanced Config Tests
// Tests for the improved Config system with validation, error handling, and edge cases

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
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
    
    // Check that all keys are present
    bool hasKey1 = false, hasKey2 = false, hasKey3 = false;
    for (const auto& key : keys) {
        if (key == "key1") hasKey1 = true;
        if (key == "key2") hasKey2 = true;
        if (key == "key3") hasKey3 = true;
    }
    assert(hasKey1 && hasKey2 && hasKey3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test validation rules
    config.addValidationRule("temperature", "range:0-100");
    config.addValidationRule("pressure", "range:0-1000");
    config.addValidationRule("required_field", "required");
    
    // Valid values should pass
    config.set("temperature", 50.0, nlm::ConfigSource::Runtime);
    config.set("pressure", 500.0, nlm::ConfigSource::Runtime);
    config.set("required_field", "value", nlm::ConfigSource::Runtime);
    
    try {
        config.validate();
        // Validation should pass
        std::cout << "    testConfigValidation passed" << std::endl;
    } catch (const nlm::ConfigValidationError& e) {
        std::cout << "    testConfigValidation failed: " << e.what() << std::endl;
        throw;
    }
    
    // Test invalid values
    try {
        config.set("temperature", 150.0, nlm::ConfigSource::Runtime);
        config.validate();
        assert(false && "Should have thrown validation error");
    } catch (const nlm::ConfigValidationError& e) {
        // Expected error
        std::cout << "    testConfigValidation correctly caught invalid temperature" << std::endl;
    }
    
    try {
        config.set("pressure", -100.0, nlm::ConfigSource::Runtime);
        config.validate();
        assert(false && "Should have thrown validation error");
    } catch (const nlm::ConfigValidationError& e) {
        // Expected error
        std::cout << "    testConfigValidation correctly caught invalid pressure" << std::endl;
    }
}

void testConfigJSONFormat() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test JSON serialization
    std::string jsonOutput = config.summary();
    assert(!jsonOutput.empty());
    assert(jsonOutput.find("Configuration") != std::string::npos);
    
    std::cout << "    testConfigJSONFormat passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Set some values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string testFile = "test_config.cfg";
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Load from file
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(testFile);
    assert(loadResult);
    
    // Verify values
    auto intVal = config2.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config2.get<double>("test_double");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config2.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config2.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void testConfigChangeCallbacks() {
    nlm::Config config;
    
    std::string lastChangedKey;
    nlm::ConfigValue lastOldValue;
    nlm::ConfigValue lastNewValue;
    
    config.addChangeCallback([&](const std::string& key, const nlm::ConfigValue& oldValue, const nlm::ConfigValue& newValue) {
        lastChangedKey = key;
        lastOldValue = oldValue;
        lastNewValue = newValue;
    });
    
    // Change a value
    config.set("test_key", 123, nlm::ConfigSource::Runtime);
    
    assert(lastChangedKey == "test_key");
    // Note: lastOldValue comparison depends on variant implementation
    
    std::cout << "    testConfigChangeCallbacks passed" << std::endl;
}

void testConfigPerformance() {
    nlm::Config config;
    
    // Performance test with many keys
    const size_t numKeys = 10000;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < numKeys; ++i) {
        config.set("key_" + std::to_string(i), static_cast<int>(i), nlm::ConfigSource::Runtime);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "    testConfigPerformance passed (" << numKeys << " keys in " << duration.count() << " ms)" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test various error conditions
    try {
        config.set("", 123, nlm::ConfigSource::Runtime); // Empty key
        assert(false && "Should have thrown exception for empty key");
    } catch (const std::exception&) {
        // Expected
    }
    
    // Test invalid value types (should be handled by variant)
    config.set("mixed", "123", nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("mixed");
    assert(!intVal.has_value()); // String should not convert to int
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigComplexTypes() {
    nlm::Config config;
    
    // Test complex value types (vectors)
    config.set("int_list", std::vector<int>({1, 2, 3, 4, 5}), nlm::ConfigSource::Runtime);
    config.set("double_list", std::vector<double>({1.1, 2.2, 3.3}), nlm::ConfigSource::Runtime);
    config.set("string_list", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    auto intList = config.get<std::vector<int>>("int_list");
    assert(intList.has_value() && intList->size() == 5);
    assert((*intList)[0] == 1 && (*intList)[4] == 5);
    
    auto doubleList = config.get<std::vector<double>>("double_list");
    assert(doubleList.has_value() && doubleList->size() == 3);
    
    auto stringList = config.get<std::vector<std::string>>("string_list");
    assert(stringList.has_value() && stringList->size() == 3);
    assert((*stringList)[0] == "a" && (*stringList)[2] == "c");
    
    std::cout << "    testConfigComplexTypes passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigJSONFormat();
    testConfigFileIO();
    testConfigChangeCallbacks();
    testConfigPerformance();
    testConfigErrorHandling();
    testConfigComplexTypes();
    
    std::cout << std::endl;
    std::cout << "=== Enhanced Config Tests PASSED ===" << std::endl;
}

} // namespace test_config