// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

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
    
    // Test array types
    config.set("int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    auto intArrayVal = config.get<std::vector<int>>("int_array");
    assert(intArrayVal.has_value());
    assert(intArrayVal.value().size() == 3);
    assert(intArrayVal.value()[0] == 1);
    assert(intArrayVal.value()[2] == 3);
    
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

void testConfigJSONLoading() {
    nlm::Config config;
    
    // Create a test JSON file
    std::string jsonContent = "{\n  \"string_value\": \"test\",\n  \"number_value\": 42,\n  \"float_value\": 3.14,\n  \"bool_value\": true,\n  \"int_array\": [1, 2, 3],\n  \"nested\": {\n    \"nested_key\": \"nested_value\"\n  }\n}";
    
    std::ofstream jsonFile("test_config.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    // Load JSON file
    bool result = config.loadFromFile("test_config.json");
    assert(result);
    
    // Check values were loaded
    assert(config.has("string_value"));
    assert(config.has("number_value"));
    assert(config.has("float_value"));
    assert(config.has("bool_value"));
    assert(config.has("int_array"));
    assert(config.has("nested.nested_key"));
    
    // Verify values
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test");
    
    auto intVal = config.get<int>("number_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto floatVal = config.get<double>("float_value");
    assert(floatVal.has_value() && *floatVal > 3.13 && *floatVal < 3.15);
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto intArrayVal = config.get<std::vector<int>>("int_array");
    assert(intArrayVal.has_value() && intArrayVal.value().size() == 3);
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigJSONLoading passed" << std::endl;
}

void testConfigYAMLLoading() {
    nlm::Config config;
    
    // Create a test YAML file
    std::string yamlContent = "string_value: test\nnumber_value: 42\nfloat_value: 3.14\nbool_value: true\nint_array:\n  - 1\n  - 2\n  - 3\nnested:\n  nested_key: nested_value\n";
    
    std::ofstream yamlFile("test_config.yaml");
    yamlFile << yamlContent;
    yamlFile.close();
    
    // Load YAML file
    bool result = config.loadFromFile("test_config.yaml");
    assert(result);
    
    // Check values were loaded
    assert(config.has("string_value"));
    assert(config.has("number_value"));
    assert(config.has("float_value"));
    assert(config.has("bool_value"));
    assert(config.has("int_array"));
    assert(config.has("nested.nested_key"));
    
    // Verify values
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test");
    
    auto intVal = config.get<int>("number_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto floatVal = config.get<double>("float_value");
    assert(floatVal.has_value() && *floatVal > 3.13 && *floatVal < 3.15);
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto intArrayVal = config.get<std::vector<int>>("int_array");
    assert(intArrayVal.has_value() && intArrayVal.value().size() == 3);
    
    // Clean up
    std::remove("test_config.yaml");
    
    std::cout << "    testConfigYAMLLoading passed" << std::endl;
}

void testConfigSimpleFileLoading() {
    nlm::Config config;
    
    // Create a test simple config file
    std::string configContent = "# Test Config File\nstring_value = test\nnumber_value = 42\nbool_value = true\n";
    
    std::ofstream configFile("test_config.cfg");
    configFile << configContent;
    configFile.close();
    
    // Load simple config file
    bool result = config.loadFromFile("test_config.cfg");
    assert(result);
    
    // Check values were loaded
    assert(config.has("string_value"));
    assert(config.has("number_value"));
    assert(config.has("bool_value"));
    
    // Verify values
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test");
    
    auto intVal = config.get<int>("number_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove("test_config.cfg");
    
    std::cout << "    testConfigSimpleFileLoading passed" << std::endl;
}

void testConfigJSONSaving() {
    nlm::Config config;
    
    // Set some test values
    config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("number_value", 42, nlm::ConfigSource::Runtime);
    config.set("float_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Save as JSON
    bool result = config.saveToFile("test_save.json", "json");
    assert(result);
    
    // Load it back
    nlm::Config config2;
    result = config2.loadFromFile("test_save.json");
    assert(result);
    
    // Verify values match
    assert(config2.has("string_value"));
    assert(config2.has("number_value"));
    assert(config2.has("float_value"));
    assert(config2.has("bool_value"));
    assert(config2.has("int_array"));
    
    // Clean up
    std::remove("test_save.json");
    
    std::cout << "    testConfigJSONSaving passed" << std::endl;
}

void testConfigYAMLSaving() {
    nlm::Config config;
    
    // Set some test values
    config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("number_value", 42, nlm::ConfigSource::Runtime);
    config.set("float_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Save as YAML
    bool result = config.saveToFile("test_save.yaml", "yaml");
    assert(result);
    
    // Load it back
    nlm::Config config2;
    result = config2.loadFromFile("test_save.yaml");
    assert(result);
    
    // Verify values match
    assert(config2.has("string_value"));
    assert(config2.has("number_value"));
    assert(config2.has("float_value"));
    assert(config2.has("bool_value"));
    assert(config2.has("int_array"));
    
    // Clean up
    std::remove("test_save.yaml");
    
    std::cout << "    testConfigYAMLSaving passed" << std::endl;
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
    
    // Create schema for testing
    std::vector<nlm::ConfigSchema> schema;
    
    nlm::ConfigSchema intSchema;
    intSchema.key = "test_int";
    intSchema.type = "int";
    intSchema.required = true;
    intSchema.description = "Test integer";
    
    nlm::ConfigSchema stringSchema;
    stringSchema.key = "test_string";
    stringSchema.type = "string";
    stringSchema.required = true;
    stringSchema.description = "Test string";
    
    schema.push_back(intSchema);
    schema.push_back(stringSchema);
    
    // Test validation with missing required keys
    assert(!config.validate(schema));
    
    // Set valid values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
    
    // Test validation with all values present
    assert(config.validate(schema));
    
    // Test validation with invalid value type
    nlm::ConfigSchema invalidSchema;
    invalidSchema.key = "test_int";
    invalidSchema.type = "string";
    invalidSchema.required = true;
    invalidSchema.description = "Test string validation";
    
    std::vector<nlm::ConfigSchema> invalidSchemaList;
    invalidSchemaList.push_back(invalidSchema);
    
    assert(!config.validate(invalidSchemaList));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigMerging() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set values in both configs
    config1.set("key1", 1, nlm::ConfigSource::File);
    config1.set("key2", 2, nlm::ConfigSource::File);
    config1.set("key3", 3, nlm::ConfigSource::File);
    
    config2.set("key2", 20, nlm::ConfigSource::CommandLine);
    config2.set("key3", 30, nlm::ConfigSource::CommandLine);
    config2.set("key4", 40, nlm::ConfigSource::CommandLine);
    
    // Merge config2 into config1
    config1.merge(config2);
    
    // Check that key1 remains unchanged (from file)
    auto key1Val = config1.get<int>("key1");
    assert(key1Val.has_value() && *key1Val == 1);
    
    // Check that key2 was updated (command line overrides)
    auto key2Val = config1.get<int>("key2");
    assert(key2Val.has_value() && *key2Val == 20);
    
    // Check that key3 was updated (command line overrides)
    auto key3Val = config1.get<int>("key3");
    assert(key3Val.has_value() && *key3Val == 30);
    
    // Check that key4 was added (from command line)
    auto key4Val = config1.get<int>("key4");
    assert(key4Val.has_value() && *key4Val == 40);
    
    std::cout << "    testConfigMerging passed" << std::endl;
}

void testConfigOverrideResolution() {
    nlm::Config config;
    
    // Set values in different sources
    config.set("key1", 1, nlm::ConfigSource::Default);
    config.set("key2", 2, nlm::ConfigSource::File);
    config.set("key3", 3, nlm::ConfigSource::CommandLine);
    
    // Create source config
    nlm::Config sourceConfig;
    sourceConfig.set("key1", 10, nlm::ConfigSource::File);
    sourceConfig.set("key2", 20, nlm::ConfigSource::File);
    sourceConfig.set("key4", 40, nlm::ConfigSource::File);
    
    // Resolve with CommandLine priority
    bool changed = config.resolveOverrides(sourceConfig, nlm::ConfigSource::CommandLine);
    assert(changed);
    
    // key1 should remain CommandLine (higher priority)
    auto key1Val = config.get<int>("key1");
    assert(key1Val.has_value() && *key1Val == 3);
    
    // key2 should be updated (CommandLine overrides File)
    auto key2Val = config.get<int>("key2");
    assert(key2Val.has_value() && *key2Val == 3);
    
    // key3 should remain CommandLine
    auto key3Val = config.get<int>("key3");
    assert(key3Val.has_value() && *key3Val == 3);
    
    // key4 should be added (from source)
    auto key4Val = config.get<int>("key4");
    assert(key4Val.has_value() && *key4Val == 40);
    
    std::cout << "    testConfigOverrideResolution passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigJSONLoading();
    testConfigYAMLLoading();
    testConfigSimpleFileLoading();
    testConfigJSONSaving();
    testConfigYAMLSaving();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigMerging();
    testConfigOverrideResolution();
}

} // namespace test_config
