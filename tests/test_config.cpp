// Config Tests
#include "core/Config/Config.hpp"
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

void testConfigFileParsingEnhanced() {
    nlm::Config config;
    
    // Test enhanced file parsing with comments and whitespace
    const std::string testConfig = 
R"(
# Test configuration file
# This file tests enhanced parsing capabilities

# Boolean values
enable_feature = true
feature_disabled = false
number_value = 42
float_value = 3.14159
string_value = "hello world"
hex_value = 0xFF
decimal_value = 255
octal_value = 0123
negative_value = -100
scientific_notation = 1.23e-4
key_with_spaces = "value with spaces"
empty_value = 
inline_comment = "some value"  # this is a comment
# Another comment
boolean_flags = true
another_flag = false
";
    
    // Create a temporary file
    std::ofstream file("test_config_enhanced.cfg");
    file << testConfig;
    file.close();
    
    // Load the enhanced configuration
    bool loaded = config.loadFromFile("test_config_enhanced.cfg");
    assert(loaded);
    
    // Test that values were parsed correctly
    assert(config.has("enable_feature"));
    auto enableFlag = config.get<bool>("enable_feature");
    assert(enableFlag.has_value());
    assert(*enableFlag == true);
    
    assert(config.has("feature_disabled"));
    auto disabledFlag = config.get<bool>("feature_disabled");
    assert(disabledFlag.has_value());
    assert(*disabledFlag == false);
    
    assert(config.has("number_value"));
    auto numVal = config.get<int64_t>("number_value");
    assert(numVal.has_value());
    assert(*numVal == 42);
    
    assert(config.has("float_value"));
    auto floatVal = config.get<double>("float_value");
    assert(floatVal.has_value());
    assert(std::abs(*floatVal - 3.14159) < 0.0001);
    
    assert(config.has("string_value"));
    auto strVal = config.get<std::string>("string_value");
    assert(strVal.has_value());
    assert(*strVal == "hello world");
    
    assert(config.has("hex_value"));
    auto hexVal = config.get<int64_t>("hex_value");
    assert(hexVal.has_value());
    assert(*hexVal == 255);
    
    assert(config.has("decimal_value"));
    auto decVal = config.get<int64_t>("decimal_value");
    assert(decVal.has_value());
    assert(*decVal == 255);
    
    assert(config.has("octal_value"));
    auto octVal = config.get<int64_t>("octal_value");
    assert(octVal.has_value());
    assert(*octVal == 83);  // 0123 in octal is 83 in decimal
    
    assert(config.has("negative_value"));
    auto negVal = config.get<int64_t>("negative_value");
    assert(negVal.has_value());
    assert(*negVal == -100);
    
    // Clean up
    std::remove("test_config_enhanced.cfg");
    
    std::cout << "    testConfigFileParsingEnhanced passed" << std::endl;
}

void testConfigValueTypes() {
    nlm::Config config;
    
    // Test different value types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("int64_value", static_cast<int64_t>(2147483647), nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("bool_true", true, nlm::ConfigSource::Runtime);
    config.set("bool_false", false, nlm::ConfigSource::Runtime);
    config.set("string_value", "test_string", nlm::ConfigSource::Runtime);
    
    // Verify types
    assert(config.get<int>("int_value").has_value());
    assert(config.get<int64_t>("int64_value").has_value());
    assert(config.get<double>("double_value").has_value());
    assert(config.get<bool>("bool_true").has_value());
    assert(config.get<bool>("bool_false").has_value());
    assert(config.get<std::string>("string_value").has_value());
    
    // Test that getOr works with defaults
    assert(config.getOr<int>("int_value", 0) == 42);
    assert(config.getOr<int64_t>("int64_value", 0) == 2147483647);
    assert(std::abs(config.getOr<double>("double_value", 0.0) - 3.14159) < 0.0001);
    assert(config.getOr<bool>("bool_true", false) == true);
    assert(config.getOr<bool>("bool_false", true) == false);
    assert(config.getOr<std::string>("string_value", "default") == "test_string");
    
    std::cout << "    testConfigValueTypes passed" << std::endl;
}

void testConfigAdvancedFeatures() {
    nlm::Config config;
    
    // Test setting different value types
    config.set("int_key", 100, nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_key", "test", nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    
    // Test that keys can be retrieved
    auto keys = config.getKeys();
    assert(keys.size() == 4);
    assert(std::find(keys.begin(), keys.end(), "int_key") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "double_key") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "string_key") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "bool_key") != keys.end());
    
    // Test summary function
    std::string summary = config.summary();
    assert(summary.find("int_key") != std::string::npos);
    assert(summary.find("double_key") != std::string::npos);
    assert(summary.find("string_key") != std::string::npos);
    assert(summary.find("bool_key") != std::string::npos);
    
    std::cout << "    testConfigAdvancedFeatures passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigFileParsingEnhanced();
    testConfigValueTypes();
    testConfigAdvancedFeatures();
}

} // namespace test_config
