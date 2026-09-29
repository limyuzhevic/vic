// Comprehensive Configuration Tests
// Tests for enhanced JSON/YAML configuration system

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

void testJSONParsing() {
    nlm::Config config;
    
    // Test JSON loading from string
    std::string jsonContent = R"({
        "int_value": 42,
        "double_value": 3.14159,
        "string_value": "hello",
        "bool_value": true,
        "array_int": [1, 2, 3],
        "array_string": ["a", "b", "c"],
        "object_value": {
            "nested_key": "nested_value"
        }
    })";
    
    assert(config.loadFromString(jsonContent));
    
    // Verify loaded values
    assert(config.get<int>("int_value").has_value());
    assert(*config.get<int>("int_value") == 42);
    
    assert(config.get<double>("double_value").has_value());
    assert(config.get<double>("double_value").value() > 3.14 && 
           config.get<double>("double_value").value() < 3.15);
    
    assert(config.get<std::string>("string_value").has_value());
    assert(*config.get<std::string>("string_value") == "hello");
    
    assert(config.get<bool>("bool_value").has_value());
    assert(*config.get<bool>("bool_value") == true);
    
    // Test array access
    auto arrayInt = config.get<std::vector<int>>("array_int");
    assert(arrayInt.has_value());
    assert(arrayInt->size() == 3);
    assert((*arrayInt)[0] == 1 && (*arrayInt)[1] == 2 && (*arrayInt)[2] == 3);
    
    auto arrayString = config.get<std::vector<std::string>>("array_string");
    assert(arrayString.has_value());
    assert(arrayString->size() == 3);
    assert((*arrayString)[0] == "a" && (*arrayString)[1] == "b" && (*arrayString)[2] == "c");
    
    // Test object access
    auto objectValue = config.get<decltype(std::unordered_map<std::string, nlm::Config::ConfigValue>)({
        {"nested_key", "nested_value"}
    })>("object_value");
    
    std::cout << "    testJSONParsing passed" << std::endl;
}

void testYAMLParsing() {
    nlm::Config config;
    
    // Test YAML loading from string
    std::string yamlContent = R"(
        int_value: 42
        double_value: 3.14
        string_value: "hello"
        bool_value: true
        array_int: [1, 2, 3]
        array_string: ["a", "b", "c"]
        object_value:
          nested_key: nested_value
    )";
    
    assert(config.loadFromString(yamlContent));
    
    // Verify loaded values
    assert(config.get<int>("int_value").has_value());
    assert(*config.get<int>("int_value") == 42);
    
    assert(config.get<double>("double_value").has_value());
    assert(config.get<double>("double_value").value() > 3.13 && 
           config.get<double>("double_value").value() < 3.15);
    
    assert(config.get<std::string>("string_value").has_value());
    assert(*config.get<std::string>("string_value") == "hello");
    
    assert(config.get<bool>("bool_value").has_value());
    assert(*config.get<bool>("bool_value") == true);
    
    // Test array access
    auto arrayInt = config.get<std::vector<int>>("array_int");
    assert(arrayInt.has_value());
    assert(arrayInt->size() == 3);
    
    // Test object access
    auto objectValue = config.get<decltype(std::unordered_map<std::string, nlm::Config::ConfigValue>)({
        {"nested_key", "nested_value"}
    })>("object_value");
    
    std::cout << "    testYAMLParsing passed" << std::endl;
}

void testSchemaValidation() {
    nlm::Config config;
    
    // Set up test values
    config.set("int_value", 42, nlm::ConfigSource::Runtime, "Integer value");
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime, "Double value");
    config.set("string_value", "hello", nlm::ConfigSource::Runtime, "String value");
    config.set("bool_value", true, nlm::ConfigSource::Runtime, "Boolean value");
    config.set("array_int", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime, "Array of integers");
    
    // Create schema
    std::vector<nlm::ConfigSchema> schema;
    schema.emplace_back("int_value", "int", true, "Integer value", std::nullopt, std::make_optional<std::pair<double, double>>(0, 100));
    schema.emplace_back("double_value", "double", true, "Double value", std::nullopt, std::make_optional<std::pair<double, double>>(0.0, 10.0));
    schema.emplace_back("string_value", "string", false, "String value");
    schema.emplace_back("bool_value", "bool", true, "Boolean value");
    schema.emplace_back("array_int", "array", "int", false, "Array of integers", std::nullopt, std::nullopt, std::make_optional<size_t>(2), std::make_optional<size_t>(10));
    
    // Test validation
    auto result = config.validate(schema);
    assert(result.isValid);
    assert(result.errorMessage.empty());
    
    // Test invalid range
    config.set("invalid_int", 200, nlm::ConfigSource::Runtime, "Invalid integer (out of range)");
    auto invalidResult = config.validate(schema);
    assert(!invalidResult.isValid);
    assert(invalidResult.errorMessage.find("int_value") != std::string::npos || 
           invalidResult.errorMessage.find("invalid_int") != std::string::npos);
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testNestedAccess() {
    nlm::Config config;
    
    // Set nested values
    nlm::NestedConfig nestedObj;
    nestedObj["inner_key"] = "inner_value";
    nestedObj["number"] = 123;
    
    config.setObject("nested", nestedObj, nlm::ConfigSource::Runtime, "Nested object");
    
    // Test nested access
    auto retrievedObj = config.getObject("nested");
    assert(retrievedObj.has_value());
    assert(retrievedObj->find("inner_key") != retrievedObj->end());
    assert(retrievedObj->at("inner_key") == "inner_value");
    assert(retrievedObj->at("number") == 123);
    
    // Test key path access
    config.set("parent.child.grandchild", "deep_value", nlm::ConfigSource::Runtime, "Deep nested value");
    auto deepValue = config.getNestedValue("parent.child.grandchild");
    assert(deepValue.has_value());
    assert(std::get<std::string>(*deepValue) == "deep_value");
    
    std::cout << "    testNestedAccess passed" << std::endl;
}

void testConfigurationInheritance() {
    nlm::Config parentConfig;
    parentConfig.set("parent_key", "parent_value", nlm::ConfigSource::Runtime, "Parent value");
    parentConfig.set("common_key", "parent", nlm::ConfigSource::Runtime, "Common value (parent)");
    
    nlm::Config childConfig;
    childConfig.applyInheritance(parentConfig, nlm::ConfigSource::Runtime);
    
    // Child should have parent's keys
    assert(childConfig.has("parent_key"));
    assert(*childConfig.get<std::string>("parent_key") == "parent_value");
    assert(*childConfig.get<std::string>("common_key") == "parent");
    
    // Child can override
    childConfig.set("common_key", "child", nlm::ConfigSource::Runtime, "Common value (child)");
    assert(*childConfig.get<std::string>("common_key") == "child");
    
    // Child can add new keys
    childConfig.set("child_key", "child_specific", nlm::ConfigSource::Runtime, "Child specific value");
    assert(childConfig.has("child_key"));
    
    std::cout << "    testConfigurationInheritance passed" << std::endl;
}

void testConfigurationComparison() {
    nlm::Config config1;
    config1.set("key1", "value1", nlm::ConfigSource::Runtime, "Value 1");
    config1.set("key2", 42, nlm::ConfigSource::Runtime, "Value 2");
    
    nlm::Config config2;
    config2.set("key1", "value1", nlm::ConfigSource::Runtime, "Value 1");
    config2.set("key2", 42, nlm::ConfigSource::Runtime, "Value 2");
    
    assert(config1.equals(config2));
    
    nlm::Config config3;
    config3.set("key1", "different", nlm::ConfigSource::Runtime, "Different value");
    
    assert(!config1.equals(config3));
    
    // Test diff
    std::string diff = config1.diff(config2);
    assert(diff.find("Configurations are identical") != std::string::npos);
    
    diff = config1.diff(config3);
    assert(diff.find("key1") != std::string::npos);
    
    std::cout << "    testConfigurationComparison passed" << std::endl;
}

void testFileSaveLoad() {
    nlm::Config config;
    
    // Set various values
    config.set("test_int", 123, nlm::ConfigSource::Runtime, "Test integer");
    config.set("test_double", 3.14159, nlm::ConfigSource::Runtime, "Test double");
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime, "Test string");
    config.set("test_bool", true, nlm::ConfigSource::Runtime, "Test boolean");
    config.setArray("test_array", std::vector<int>({1, 2, 3, 4, 5}), nlm::ConfigSource::Runtime, "Test array");
    
    // Save to JSON file
    assert(config.saveToFile("/tmp/test_config.json", "json"));
    
    // Load back
    nlm::Config config2;
    assert(config2.loadFromFile("/tmp/test_config.json"));
    
    // Verify values are preserved
    assert(config.equals(config2));
    
    // Save to YAML file
    assert(config.saveToFile("/tmp/test_config.yaml", "yaml"));
    
    // Load back from YAML
    nlm::Config config3;
    assert(config3.loadFromFile("/tmp/test_config.yaml"));
    
    // Verify YAML values are correct
    assert(config.get<int>("test_int") == config3.get<int>("test_int"));
    
    std::cout << "    testFileSaveLoad passed" << std::endl;
}

void testCLIArguments() {
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--key1=value1",
        (char*)"-k2",
        (char*)"value2",
        (char*)"--key3=true"
    };
    
    assert(config.loadFromArgs(argc, argv));
    
    // Verify values were loaded
    assert(config.has("key1"));
    assert(*config.get<std::string>("key1") == "value1");
    
    assert(config.has("k2"));
    assert(*config.get<std::string>("k2") == "value2");
    
    assert(config.has("key3"));
    assert(*config.get<bool>("key3") == true);
    
    std::cout << "    testCLIArguments passed" << std::endl;
}

void testBackwardCompatibility() {
    nlm::Config config;
    
    // Test old-style key=value format (INI style)
    std::string iniContent = R"(
        # Comment line
        key1 = value1
        key2 = 42
        key3 = true
        key4 = 3.14
    )";
    
    assert(config.loadFromString(iniContent));
    
    // Verify values
    assert(config.has("key1"));
    assert(*config.get<std::string>("key1") == "value1");
    
    assert(config.has("key2"));
    assert(*config.get<int>("key2") == 42);
    
    assert(config.has("key3"));
    assert(*config.get<bool>("key3") == true);
    
    assert(config.has("key4"));
    assert(config.get<double>("key4").value() > 3.13 && 
           config.get<double>("key4").value() < 3.15);
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void runAll() {
    testJSONParsing();
    testYAMLParsing();
    testSchemaValidation();
    testNestedAccess();
    testConfigurationInheritance();
    testConfigurationComparison();
    testFileSaveLoad();
    testCLIArguments();
    testBackwardCompatibility();
    
    std::cout << "\nAll enhanced configuration tests passed!" << std::endl;
}

} // namespace test_config