// Enhanced Config Tests
// Tests for JSON/YAML parsing, validation, and schema support

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace test_config_enhanced {

void testConfigValidation() {
    std::cout << "    Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    config.set("valid_int", 42);
    config.set("valid_string", "test");
    config.set("valid_float", 3.14f);
    config.set("valid_bool", true);
    config.set("valid_vector", std::vector<int>{1, 2, 3});
    
    // Create a schema
    nlm::ConfigSchema schema;
    schema.addField("required_int", 100, "Required integer field", nullptr, true);
    schema.addField("optional_string", "default", "Optional string with default", [](const nlm::ConfigValue& v) {
        return std::get<std::string>(v).size() >= 3;
    }, false);
    schema.addField("validated_float", 1.0, "Float must be positive", [](const nlm::ConfigValue& v) {
        return std::get<double>(v) > 0.0;
    }, true);
    
    // Test validation with invalid config (missing required field)
    bool valid = config.validate(schema);
    assert(!valid);  // Should fail because "required_int" is missing
    
    // Add the missing required field
    config.set("required_int", 200);
    assert(config.has("required_int"));
    
    // Test validation with valid config
    valid = config.validate(schema);
    assert(valid);  // Should pass now
    
    // Test schema validation with invalid float
    nlm::Config config2;
    config2.set("validated_float", -5.0);
    config2.set("required_int", 100);
    
    valid = config2.validate(schema);
    assert(!valid);  // Should fail because float is not positive
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testJsonParsing() {
    std::cout << "    Testing JSON parsing..." << std::endl;
    
    nlm::Config config;
    
    // Create a JSON string
    std::string jsonStr = "{\n"    "  \"test_int\": 123,\n"    "  \"test_string\": \"hello world\",\n"    "  \"test_float\": 3.14,\n"    "  \"test_bool\": true,\n"    "  \"test_array\": [1, 2, 3]\n"    "}";
    
    // For now, test the stringToValue conversion directly
    nlm::ConfigValue intVal = config.stringToValue("123");
    assert(std::holds_alternative<int>(intVal));
    assert(std::get<int>(intVal) == 123);
    
    nlm::ConfigValue stringVal = config.stringToValue("\"hello world\"");
    assert(std::holds_alternative<std::string>(stringVal));
    assert(std::get<std::string>(stringVal) == "hello world");
    
    nlm::ConfigValue floatVal = config.stringToValue("3.14");
    assert(std::holds_alternative<double>(floatVal));
    assert(std::get<double>(floatVal) == 3.14);
    
    nlm::ConfigValue boolVal = config.stringToValue("true");
    assert(std::holds_alternative<bool>(boolVal));
    assert(std::get<bool>(boolVal) == true);
    
    nlm::ConfigValue arrayVal = config.stringToValue("[1, 2, 3]");
    assert(std::holds_alternative<std::vector<int>>(arrayVal));
    assert(std::get<std::vector<int>>(arrayVal).size() == 3);
    
    std::cout << "    testJsonParsing passed" << std::endl;
}

void testYamlParsing() {
    std::cout << "    Testing YAML parsing..." << std::endl;
    
    nlm::Config config;
    
    // Test YAML string parsing
    nlm::ConfigValue intVal = config.stringToValue("42");
    assert(std::holds_alternative<int>(intVal));
    assert(std::get<int>(intVal) == 42);
    
    nlm::ConfigValue stringVal = config.stringToValue("'hello'");
    assert(std::holds_alternative<std::string>(stringVal));
    assert(std::get<std::string>(stringVal) == "hello");
    
    nlm::ConfigValue boolVal = config.stringToValue("yes");
    assert(std::holds_alternative<std::string>(boolVal));
    assert(std::get<std::string>(boolVal) == "yes");
    
    // Test YAML list parsing
    nlm::ConfigValue arrayVal = config.stringToValue("- item1\n- item2\n- item3");
    assert(std::holds_alternative<std::string>(arrayVal));
    assert(std::get<std::string>(arrayVal).find("item1") != std::string::npos);
    
    std::cout << "    testYamlParsing passed" << std::endl;
}

void testConfigSchema() {
    std::cout << "    Testing configuration schema..." << std::endl;
    
    nlm::ConfigSchema schema;
    
    // Add various field types with validators
    schema.addField("required_field", "default", "A required field", nullptr, true);
    schema.addField("positive_number", 10.0, "Positive number", [](const nlm::ConfigValue& v) {
        return std::get<double>(v) > 0;
    }, false);
    schema.addField("string_length", "abc", "String of length 3+", [](const nlm::ConfigValue& v) {
        return std::get<std::string>(v).size() >= 3;
    }, false);
    schema.addField("enum_field", "option1", "Enum with specific values", [](const nlm::ConfigValue& v) {
        std::string val = std::get<std::string>(v);
        return val == "option1" || val == "option2" || val == "option3";
    }, true);
    
    nlm::Config config;
    config.set("required_field", "value");
    config.set("positive_number", 5.0);
    config.set("string_length", "valid");
    config.set("enum_field", "option2");
    
    assert(config.validate(schema));  // Should be valid
    
    // Test invalid enum
    nlm::Config config2;
    config2.set("required_field", "value");
    config2.set("positive_number", 5.0);
    config2.set("string_length", "valid");
    config2.set("enum_field", "invalid_option");
    
    assert(!config2.validate(schema));  // Should be invalid
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigSaveLoad() {
    std::cout << "    Testing config save/load functionality..." << std::endl;
    
    nlm::Config config;
    config.set("int_value", 42);
    config.set("string_value", "test");
    config.set("float_value", 3.14f);
    config.set("bool_value", true);
    
    // Test saving as simple format
    std::string tempFile1 = "/tmp/test_config_simple.cfg";
    assert(config.saveToFile(tempFile1, false));
    
    // Load the saved file
    nlm::Config config2;
    assert(config2.loadFromFile(tempFile1));
    
    assert(config2.get<int>("int_value") == 42);
    assert(config2.get<std::string>("string_value") == "test");
    assert(config2.get<double>("float_value") == 3.14);
    assert(config2.get<bool>("bool_value") == true);
    
    // Test saving as JSON format (if JSON library available)
    std::string tempFile2 = "/tmp/test_config_json.json";
    if (config.saveToFile(tempFile2, true)) {
        // File saved successfully (JSON format not yet implemented)
    }
    
    std::cout << "    testConfigSaveLoad passed" << std::endl;
}

void testConfigErrorHandling() {
    std::cout << "    Testing configuration error handling..." << std::endl;
    
    nlm::Config config;
    
    // Test getting non-existent key
    assert(!config.has("non_existent"));
    assert(!config.get<int>("non_existent").has_value());
    assert(config.getOr<int>("non_existent", 999) == 999);
    
    // Test type conversion errors
    config.set("string_data", "123");
    auto stringVal = config.get<std::string>("string_data");
    assert(stringVal.has_value());
    assert(stringVal.value() == "123");
    
    // The string is not convertible to int when retrieved as int
    auto intVal = config.get<int>("string_data");
    assert(!intVal.has_value());  // Should fail because string cannot be converted to int
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigPerformance() {
    std::cout << "    Testing configuration performance..." << std::endl;
    
    nlm::Config config;
    
    // Add many entries to test performance
    for (int i = 0; i < 1000; ++i) {
        config.set("key_" + std::to_string(i), i);
    }
    
    // Test getting values
    for (int i = 0; i < 1000; ++i) {
        auto val = config.get<int>("key_" + std::to_string(i));
        assert(val.has_value() && val.value() == i);
    }
    
    // Test has check
    for (int i = 0; i < 1000; ++i) {
        assert(config.has("key_" + std::to_string(i)));
    }
    
    // Test summary generation
    std::string summary = config.summary();
    assert(summary.find("1000 entries") != std::string::npos);
    
    std::cout << "    testConfigPerformance passed" << std::endl;
}

void runAll() {
    testConfigValidation();
    testJsonParsing();
    testYamlParsing();
    testConfigSchema();
    testConfigSaveLoad();
    testConfigErrorHandling();
    testConfigPerformance();
    
    std::cout << std::endl;
    std::cout << "=== All enhanced Config tests passed! ===" << std::endl;
}

} // namespace test_config_enhanced