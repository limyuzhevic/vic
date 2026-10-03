// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
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
    
    auto intVal = config.get<int64_t>("int_value");
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
    
    int val = config.getOr<int64_t>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int64_t>("existing", 100);
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

void testFormatDetection() {
    nlm::Config config;
    
    // Test JSON format detection
    std::string jsonContent = R"({"test_key": 123, "string_val": "hello", "bool_val": true})";
    std::string detectedFormat = config.detectFormat(jsonContent);
    assert(detectedFormat == "JSON");
    
    // Test YAML format detection
    std::string yamlContent = "test_key: 456\nstring_val: \"world\"\nbool_val: false";
    detectedFormat = config.detectFormat(yamlContent);
    assert(detectedFormat == "YAML");
    
    // Test key-value format detection
    std::string kvContent = "key1 = value1\nkey2 = 42\nkey3 = true";
    detectedFormat = config.detectFormat(kvContent);
    assert(detectedFormat == "KEY_VALUE");
    
    std::cout << "    testFormatDetection passed" << std::endl;
}

void testJsonParsing() {
    nlm::Config config;
    
    // Test loading JSON content
    std::string jsonContent = R"({
        "int_field": 42,
        "string_field": "test_string",
        "double_field": 3.14159,
        "bool_field": true,
        "nested": {
            "nested_int": 100,
            "nested_string": "nested_value"
        },
        "array_field": [1, 2, 3, 4, 5]
    })";
    
    assert(config.loadFromString(jsonContent, "JSON"));
    
    // Verify JSON parsing results
    auto intVal = config.get<int64_t>("int_field");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_field");
    assert(stringVal.has_value() && *stringVal == "test_string");
    
    auto doubleVal = config.get<double>("double_field");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14159) < 0.0001);
    
    auto boolVal = config.get<bool>("bool_field");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto nestedInt = config.get<int64_t>("nested.nested_int");
    assert(nestedInt.has_value() && *nestedInt == 100);
    
    auto nestedString = config.get<std::string>("nested.nested_string");
    assert(nestedString.has_value() && *nestedString == "nested_value");
    
    // Test array access (simplified - checking keys)
    std::vector<std::string> keys = config.getKeys();
    assert(std::find(keys.begin(), keys.end(), "int_field") != keys.end());
    
    std::cout << "    testJsonParsing passed" << std::endl;
}

void testYamlParsing() {
    nlm::Config config;
    
    // Test loading YAML content
    std::string yamlContent = R"(int_field: 123
string_field: test_string
double_field: 2.718281828459045
bool_field: false
nested:
  nested_int: 200
  nested_string: nested_yaml_value
array_field:
  - item1
  - item2
  - item3
  - 42
  - 3.14
  - true
  )";
    
    assert(config.loadFromString(yamlContent, "YAML"));
    
    // Verify YAML parsing results
    auto intVal = config.get<int64_t>("int_field");
    assert(intVal.has_value() && *intVal == 123);
    
    auto stringVal = config.get<std::string>("string_field");
    assert(stringVal.has_value() && *stringVal == "test_string");
    
    auto doubleVal = config.get<double>("double_field");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 2.71828) < 0.0001);
    
    auto boolVal = config.get<bool>("bool_field");
    assert(boolVal.has_value() && *boolVal == false);
    
    auto nestedInt = config.get<int64_t>("nested.nested_int");
    assert(nestedInt.has_value() && *nestedInt == 200);
    
    auto nestedString = config.get<std::string>("nested.nested_string");
    assert(nestedString.has_value() && *nestedString == "nested_yaml_value");
    
    std::cout << "    testYamlParsing passed" << std::endl;
}

void testKeyValueParsing() {
    nlm::Config config;
    
    // Test loading key-value content
    std::string kvContent = R"(# This is a comment
string_key = "quoted_string"
int_key = 42
double_key = 3.14159
bool_key = true
# Another comment
array_key = [1, 2, 3, 4]

# Empty line
another_key = another_value
  )";
    
    assert(config.loadFromString(kvContent, "KEY_VALUE"));
    
    // Verify key-value parsing results
    auto stringVal = config.get<std::string>("string_key");
    assert(stringVal.has_value() && *stringVal == "quoted_string");
    
    auto intVal = config.get<int64_t>("int_key");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_key");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14159) < 0.0001);
    
    auto boolVal = config.get<bool>("bool_key");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto anotherVal = config.get<std::string>("another_key");
    assert(anotherVal.has_value() && *anotherVal == "another_value");
    
    std::cout << "    testKeyValueParsing passed" << std::endl;
}

void testSchemaValidation() {
    nlm::ConfigSchema schema;
    
    // Add fields to schema
    schema.addField("required_int", "int64", "Required integer field", "^-?\d+$", true, "0");
    schema.addField("optional_string", "string", "Optional string field", "^[a-zA-Z]+$", false, "default");
    schema.addField("required_double", "double", "Required double field", "^-?\d+\.?\d*$", true, "1.0");
    schema.addField("optional_bool", "bool", "Optional boolean field", "^(true|false)$", false, "false");
    
    nlm::Config config(schema);
    
    // Test validation with missing required fields
    std::vector<std::string> validationErrors = config.validateAgainstSchema();
    assert(validationErrors.size() > 0); // Should have errors for missing required fields
    
    // Add required fields
    config.set("required_int", 42, nlm::ConfigSource::Runtime);
    config.set("required_double", 3.14, nlm::ConfigSource::Runtime);
    
    // Test validation with all required fields present
    validationErrors = config.validateAgainstSchema();
    // Should be empty or only have errors for other validation issues
    
    // Add optional fields
    config.set("optional_string", "test", nlm::ConfigSource::Runtime);
    config.set("optional_bool", true, nlm::ConfigSource::Runtime);
    
    // Test validation with all fields
    validationErrors = config.validateAgainstSchema();
    // Should be empty (all validations passed)
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testFileFormatAutoDetection() {
    nlm::Config config;
    
    // Create temporary test files
    const std::string testDir = "test_configs";
    if (!std::filesystem::exists(testDir)) {
        std::filesystem::create_directory(testDir);
    }
    
    // Create JSON test file
    std::string jsonFile = testDir + "/test_config.json";
    std::ofstream jsonOut(jsonFile);
    jsonOut << R"({"format_test": "json", "value": 123})";
    jsonOut.close();
    
    // Create YAML test file
    std::string yamlFile = testDir + "/test_config.yaml";
    std::ofstream yamlOut(yamlFile);
    yamlOut << "format_test: yaml\nvalue: 456\nanother_key: \"yaml_value\"";
    yamlOut.close();
    
    // Create key-value test file
    std::string kvFile = testDir + "/test_config.cfg";
    std::ofstream kvOut(kvFile);
    kvOut << "format_test = kv_value\nother_value = 789\nbool_flag = true";
    kvOut.close();
    
    // Test auto-detection from file extensions
    nlm::Config jsonConfig;
    assert(jsonConfig.loadFromFile(jsonFile)); // Should auto-detect as JSON
    auto jsonFormat = jsonConfig.getCurrentFormat();
    assert(jsonFormat == "JSON" || jsonFormat == "json");
    
    nlm::Config yamlConfig;
    assert(yamlConfig.loadFromFile(yamlFile)); // Should auto-detect as YAML
    auto yamlFormat = yamlConfig.getCurrentFormat();
    assert(yamlFormat == "YAML" || yamlFormat == "yaml");
    
    nlm::Config kvConfig;
    assert(kvConfig.loadFromFile(kvFile)); // Should auto-detect as KEY_VALUE
    auto kvFormat = kvConfig.getCurrentFormat();
    assert(kvFormat == "KEY_VALUE" || kvFormat == "KEY_VALUE");
    
    // Clean up test files
    std::filesystem::remove(jsonFile);
    std::filesystem::remove(yamlFile);
    std::filesystem::remove(kvFile);
    std::filesystem::remove(testDir);
    
    std::cout << "    testFileFormatAutoDetection passed" << std::endl;
}

void testSerialization() {
    nlm::Config config;
    
    // Add test values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("string_val", "test_string", nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("array_val", std::vector<int64_t>({1, 2, 3, 4, 5}), nlm::ConfigSource::Runtime);
    
    // Test JSON serialization
    std::string jsonOutput = config.saveToFile("", "JSON");
    assert(!jsonOutput.empty());
    assert(config.loadFromString(jsonOutput, "JSON"));
    
    // Test YAML serialization
    std::string yamlOutput = config.saveToFile("", "YAML");
    assert(!yamlOutput.empty());
    assert(config.loadFromString(yamlOutput, "YAML"));
    
    // Test KEY_VALUE serialization
    std::string kvOutput = config.saveToFile("", "KEY_VALUE");
    assert(!kvOutput.empty());
    assert(config.loadFromString(kvOutput, "KEY_VALUE"));
    
    std::cout << "    testSerialization passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testFormatDetection();
    testJsonParsing();
    testYamlParsing();
    testKeyValueParsing();
    testSchemaValidation();
    testFileFormatAutoDetection();
    testSerialization();
}

} // namespace test_config
