// Unit test for core functionality - Config class tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <string>

namespace test_config_unit {

void testConfigLoading() {
    nlm::Config config;
    
    // Test setting and getting basic types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Verify values
    assert(config.get<int>("int_value").has_value());
    assert(*config.get<int>("int_value") == 42);
    
    assert(config.get<double>("double_value").has_value());
    assert(*config.get<double>("double_value") > 3.13 && *config.get<double>("double_value") < 3.15);
    
    assert(config.get<std::string>("string_value").has_value());
    assert(*config.get<std::string>("string_value") == "test");
    
    assert(config.get<bool>("bool_value").has_value());
    assert(*config.get<bool>("bool_value") == true);
    
    std::cout << "    testConfigLoading passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test validation functions
    config.set("int_value", 100, nlm::ConfigSource::Runtime);
    
    // Test getOr function
    int val = config.getOr<int>("int_value", 0);
    assert(val == 100);
    
    int missing = config.getOr<int>("missing_key", 999);
    assert(missing == 999);
    
    // Test has function
    assert(config.has("int_value"));
    assert(!config.has("missing_key"));
    
    // Test remove function
    config.remove("int_value");
    assert(!config.has("int_value"));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSaving() {
    nlm::Config config;
    
    // Set various values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_float", 3.14159f, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Get keys
    auto keys = config.getKeys();
    assert(keys.size() == 4);
    
    // Verify all keys exist
    assert(config.has("test_int"));
    assert(config.has("test_float"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    std::cout << "    testConfigSaving passed" << std::endl;
}

void testConfigTypeConversion() {
    nlm::Config config;
    
    // Test different type conversions
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_value", false, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    
    // Test get with different types
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == false);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "test");
    
    std::cout << "    testConfigTypeConversion passed" << std::endl;
}

void testConfigEdgeCases() {
    nlm::Config config;
    
    // Test with empty string
    config.set("empty_string", std::string(""), nlm::ConfigSource::Runtime);
    assert(config.get<std::string>("empty_string").has_value());
    
    // Test with negative numbers
    config.set("negative_int", -100, nlm::ConfigSource::Runtime);
    assert(config.get<int>("negative_int").has_value());
    assert(*config.get<int>("negative_int") == -100);
    
    // Test with zero
    config.set("zero_value", 0, nlm::ConfigSource::Runtime);
    assert(config.get<int>("zero_value").has_value());
    assert(*config.get<int>("zero_value") == 0);
    
    // Test with false boolean
    config.set("false_value", false, nlm::ConfigSource::Runtime);
    assert(config.get<bool>("false_value").has_value());
    assert(*config.get<bool>("false_value") == false);
    
    std::cout << "    testConfigEdgeCases passed" << std::endl;
}

void runAllConfigUnitTests() {
    testConfigLoading();
    testConfigValidation();
    testConfigSaving();
    testConfigTypeConversion();
    testConfigEdgeCases();
}

} // namespace test_config_unit