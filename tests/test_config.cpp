// Enhanced Config Tests
// Comprehensive tests for the improved configuration system with validation
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
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

// Enhanced validation tests
void testConfigTypeValidation() {
    nlm::Config config;
    
    // Add custom type validator for "positive_int"
    config.addTypeValidator("positive_int", [](const nlm::ConfigValue& value) {
        if (const auto* val = std::get_if<int>(&value)) {
            return *val > 0;
        }
        return false;
    });
    
    // Test valid positive int
    config.set("positive", 42, nlm::ConfigSource::Runtime, "positive_int");
    assert(config.validateType("positive", "positive_int"));
    
    // Test invalid negative int
    config.set("negative", -10, nlm::ConfigSource::Runtime, "positive_int");
    assert(!config.validateType("negative", "positive_int"));
    
    std::cout << "    testConfigTypeValidation passed" << std::endl;
}

void testConfigRangeValidation() {
    nlm::Config config;
    
    // Test range validation for temperature
    assert(config.validateRange("temp", 0.0, 100.0)); // No temp set, should be valid
    
    config.set("temp", 50.0, nlm::ConfigSource::Runtime);
    assert(config.validateRange("temp", 0.0, 100.0));
    assert(!config.validateRange("temp", 60.0, 100.0));
    
    config.set("pressure", 100, nlm::ConfigSource::Runtime);
    assert(config.validateRange("pressure", 0, 200)); // int version
    
    std::cout << "    testConfigRangeValidation passed" << std::endl;
}

void testConfigEnumValidation() {
    nlm::Config config;
    
    std::vector<std::string> colors = {"red", "green", "blue", "yellow"};
    
    config.set("color", std::string("red"), nlm::ConfigSource::Runtime);
    assert(config.validateEnum("color", colors));
    
    config.set("color2", std::string("purple"), nlm::ConfigSource::Runtime);
    assert(!config.validateEnum("color2", colors));
    
    std::cout << "    testConfigEnumValidation passed" << std::endl;
}

void testConfigValidationResults() {
    nlm::Config config;
    
    // Add a type validator
    config.addTypeValidator("positive_int", [](const nlm::ConfigValue& value) {
        if (const auto* val = std::get_if<int>(&value)) {
            return *val > 0;
        }
        return false;
    });
    
    // Valid configuration
    config.set("good", 25, nlm::ConfigSource::Runtime, "positive_int");
    auto result1 = config.validate("good");
    assert(result1.isValid);
    assert(!result1.hasErrors());
    
    // Invalid configuration
    config.set("bad", -5, nlm::ConfigSource::Runtime, "positive_int");
    auto result2 = config.validate("bad");
    assert(!result2.isValid);
    assert(result2.hasErrors());
    
    // Test error and warning methods
    assert(config.getValidationErrors().find("bad") != std::string::npos);
    assert(config.getValidationWarnings().empty());
    
    std::cout << "    testConfigValidationResults passed" << std::endl;
}

void testConfigSerialization() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Save to file
    assert(config.saveToFile("test_config.txt"));
    
    // Clear and reload
    config.clear();
    assert(config.loadFromFile("test_config.txt"));
    
    // Verify values were loaded correctly
    assert(config.has("int_val"));
    assert(config.getOr<int>("int_val", 0) == 42);
    assert(config.getOr<double>("double_val", 0.0) > 3.14 && config.getOr<double>("double_val", 0.0) < 3.142);
    assert(config.getOr<std::string>("string_val", "") == "test");
    assert(config.getOr<bool>("bool_val", false) == true);
    
    // Clean up
    std::remove("test_config.txt");
    
    std::cout << "    testConfigSerialization passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("key1", 100, nlm::ConfigSource::Default);
    config1.set("key2", 200, nlm::ConfigSource::Default);
    
    nlm::Config config2;
    config2.set("key2", 300, nlm::ConfigSource::File);  // Should override
    config2.set("key3", 400, nlm::ConfigSource::File);
    
    config1.merge(config2, nlm::ConfigSource::File);
    
    // key1 should remain from config1 (default)
    assert(config1.getOr<int>("key1", 0) == 100);
    
    // key2 should be from config2 (file overrides)
    assert(config1.getOr<int>("key2", 0) == 300);
    
    // key3 should be from config2 (file)
    assert(config1.getOr<int>("key3", 0) == 400);
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigStatistics() {
    nlm::Config config;
    
    // Add entries from different sources
    config.set("key1", 1, nlm::ConfigSource::Default);
    config.set("key2", 2, nlm::ConfigSource::File);
    config.set("key3", 3, nlm::ConfigSource::CommandLine);
    config.set("key4", 4, nlm::ConfigSource::Runtime);
    
    assert(config.getEntryCount() == 4);
    assert(config.getEntriesBySource(nlm::ConfigSource::Default) == 1);
    assert(config.getEntriesBySource(nlm::ConfigSource::File) == 1);
    assert(config.getEntriesBySource(nlm::ConfigSource::CommandLine) == 1);
    assert(config.getEntriesBySource(nlm::ConfigSource::Runtime) == 1);
    
    std::cout << "    testConfigStatistics passed" << std::endl;
}

void runAll() {
    std::cout << "Running enhanced configuration tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigTypeValidation();
    testConfigRangeValidation();
    testConfigEnumValidation();
    testConfigValidationResults();
    testConfigSerialization();
    testConfigMerge();
    testConfigStatistics();
    
    std::cout << "\nAll enhanced configuration tests passed!" << std::endl;
}

} // namespace test_config