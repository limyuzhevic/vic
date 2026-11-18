// Comprehensive Configuration System Tests
// Tests for the enhanced JSON/YAML configuration system

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

namespace test_enhanced_config {

void testConfigJsonSerialization() {
#ifdef HAVE_NLOHMANN_JSON
    nlm::Config config;
    
    // Add various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("int64_value", static_cast<int64_t>(1000000), nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("float_value", 2.71828f, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Test JSON serialization
    auto json = config.toJson();
    
    // Verify values
    assert(json["int_value"] == 42);
    assert(json["int64_value"] == 1000000);
    assert(std::abs(json["double_value"] - 3.14159) < 0.00001);
    assert(std::abs(json["float_value"] - 2.71828) < 0.0001f);
    assert(json["bool_value"] == true);
    assert(json["string_value"] == "hello");
    
    assert(json["int_array"].size() == 5);
    assert(json["double_array"].size() == 3);
    assert(json["string_array"].size() == 3);
    
    // Test pretty printing
    std::string pretty = config.toPrettyString();
    assert(pretty.find("int_value") != std::string::npos);
    assert(pretty.find("int_array") != std::string::npos);
    
    std::cout << "    testConfigJsonSerialization passed" << std::endl;
#else
    std::cout << "    testConfigJsonSerialization skipped (no nlohmann_json)" << std::endl;
#endif
}

void testConfigJsonDeserialization() {
#ifdef HAVE_NLOHMANN_JSON
    // Create a JSON string manually
    std::string jsonStr = R"({
        "int_value": 100,
        "double_value": 3.14,
        "string_value": "test",
        "bool_value": false,
        "int_array": [1, 2, 3],
        "nested": {"key": "value"}
    })";
    
    // Parse JSON
    auto json = nlohmann::json::parse(jsonStr);
    
    // Create config from JSON
    nlm::Config config;
    config.loadFromJson(json);
    
    // Verify values
    assert(config.has("int_value"));
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 100);
    
    assert(config.has("double_value"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.001);
    
    assert(config.has("string_value"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test");
    
    assert(config.has("bool_value"));
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == false);
    
    std::cout << "    testConfigJsonDeserialization passed" << std::endl;
#else
    std::cout << "    testConfigJsonDeserialization skipped (no nlohmann_json)" << std::endl;
#endif
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Add test values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string testFile = "/tmp/nlm_test_config.txt";
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    // Load from file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(testFile);
    assert(loadSuccess);
    
    // Verify values
    assert(config2.has("test_int"));
    assert(config2.getOr<int>("test_int", 0) == 42);
    
    assert(config2.has("test_string"));
    assert(config2.getOr<std::string>("test_string", "") == "hello world");
    
    assert(config2.has("test_bool"));
    assert(config2.getOr<bool>("test_bool", false) == true);
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void testConfigTypeValidation() {
    nlm::Config config;
    
    // Add values
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_string", "test", nlm::ConfigSource::Runtime);
    config.set("valid_bool", true, nlm::ConfigSource::Runtime);
    
    // Test type validation
    assert(config.validateType("valid_int", nlm::ConfigValue(42)));
    assert(config.validateType("valid_string", nlm::ConfigValue(std::string("test"))));
    assert(config.validateType("valid_bool", nlm::ConfigValue(true)));
    
    // Test type mismatch
    assert(!config.validateType("valid_int", nlm::ConfigValue("string")));
    assert(!config.validateType("valid_string", nlm::ConfigValue(42)));
    
    // Test range validation
    assert(config.validateRange("valid_int", 0, 100)); // 42 is in range
    assert(!config.validateRange("valid_int", 50, 100)); // 42 is NOT in range
    
    std::cout << "    testConfigTypeValidation passed" << std::endl;
}

void testConfigUtilityFunctions() {
    // Test utility functions
    assert(nlm::Config::isNumericType(nlm::ConfigValue(42)));
    assert(nlm::Config::isNumericType(nlm::ConfigValue(3.14)));
    assert(nlm::Config::isBooleanType(nlm::ConfigValue(true)));
    assert(nlm::Config::isStringType(nlm::ConfigValue(std::string("test"))));
    assert(nlm::Config::isArrayType(nlm::ConfigValue(std::vector<int>{1, 2, 3})));
    
    // Test type name functions
    assert(nlm::Config::configValueTypeName(nlm::ConfigValue(42)) == "int");
    assert(nlm::Config::configValueTypeName(nlm::ConfigValue(3.14)) == "double");
    assert(nlm::Config::configValueTypeName(nlm::ConfigValue(true)) == "bool");
    assert(nlm::Config::configValueTypeName(nlm::ConfigValue(std::string("test"))) == "string");
    
    // Test numeric value extraction
    assert(std::abs(nlm::Config::getNumericValue(nlm::ConfigValue(42)) - 42.0) < 0.001);
    assert(std::abs(nlm::Config::getNumericValue(nlm::ConfigValue(3.14)) - 3.14) < 0.001);
    
    std::cout << "    testConfigUtilityFunctions passed" << std::endl;
}

void testConfigSchemaValidation() {
    // Test schema validation (placeholder - would need schema files)
    // For now, just verify the function exists
    bool validationResult = nlm::Config::validateSchema("/tmp/nlm_test_schema.json");
    // This should return true (empty validation)
    assert(validationResult);
    
    std::cout << "    testConfigSchemaValidation passed (placeholder)" << std::endl;
}

void runAll() {
    testConfigJsonSerialization();
    testConfigJsonDeserialization();
    testConfigFileIO();
    testConfigTypeValidation();
    testConfigUtilityFunctions();
    testConfigSchemaValidation();
}

} // namespace test_enhanced_config

// Integration test runner
void runEnhancedConfigTests() {
    std::cout << "Running enhanced configuration system tests..." << std::endl;
    test_enhanced_config::runAll();
    std::cout << "All enhanced configuration system tests completed." << std::endl;
}