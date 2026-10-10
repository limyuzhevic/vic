// Enhanced Config Tests for the new JSON/YAML-based configuration system
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

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

void testJSONLoading() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string testContent = R"({
        "test_int": 42,
        "test_double": 3.14,
        "test_string": "hello",
        "test_bool": true,
        "test_array": [1, 2, 3, 4, 5]
    })";
    
    std::ofstream jsonFile("test_config.json");
    jsonFile << testContent;
    jsonFile.close();
    
    // Load JSON file
    assert(config.loadFromFile("test_config.json"));
    
    // Verify values
    assert(config.getOr<int>("test_int", 0) == 42);
    assert(config.getOr<double>("test_double", 0.0) == 3.14);
    assert(config.getOr<std::string>("test_string", "") == "hello");
    assert(config.getOr<bool>("test_bool", false) == true);
    
    // Test array
    assert(config.has("test_array"));
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testJSONLoading passed" << std::endl;
}

void testYAMLLoading() {
    nlm::Config config;
    
    // Create a temporary YAML file
    std::string testContent = """test_int: 42
test_double: 3.14
test_string: hello
test_bool: true
test_array:
  - 1
  - 2
  - 3
  - 4
  - 5
""";
    
    std::ofstream yamlFile("test_config.yaml");
    yamlFile << testContent;
    yamlFile.close();
    
    // Load YAML file
    assert(config.loadFromFile("test_config.yaml"));
    
    // Verify values
    assert(config.getOr<int>("test_int", 0) == 42);
    assert(config.getOr<double>("test_double", 0.0) == 3.14);
    assert(config.getOr<std::string>("test_string", "") == "hello");
    assert(config.getOr<bool>("test_bool", false) == true);
    
    // Test array
    assert(config.has("test_array"));
    
    // Clean up
    std::remove("test_config.yaml");
    
    std::cout << "    testYAMLLoading passed" << std::endl;
}

void testConfigSchema() {
    nlm::ConfigSchema schema;
    
    // Add some schema entries
    schema.addEntry(nlm::ConfigSchemaEntry(
        "test_key", "Test configuration key", nlm::ConfigCategory::System,
        std::optional<nlm::ConfigValue>(42), false, "int"
    ));
    
    schema.addEntry(nlm::ConfigSchemaEntry(
        "test_string", "Test string key", nlm::ConfigCategory::Neural,
        std::optional<nlm::ConfigValue>(std::string("default")), false, "string"
    ));
    
    // Create config and set schema
    nlm::Config config;
    config.setSchema(schema);
    
    // Set valid value
    config.set("test_key", 100, nlm::ConfigSource::Runtime);
    assert(config.getOr<int>("test_key", 0) == 100);
    
    // Set string value
    config.set("test_string", std::string("custom"), nlm::ConfigSource::Runtime);
    assert(config.getOr<std::string>("test_string", "") == "custom");
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigSaving() {
    nlm::Config config;
    
    // Set some values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Save to JSON
    assert(config.saveToFile("test_config.json", true));
    
    // Create a new config and load from JSON
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.json"));
    
    // Verify values
    assert(config2.getOr<int>("int_val", 0) == 42);
    assert(config2.getOr<std::string>("string_val", "") == "test");
    assert(config2.getOr<double>("double_val", 0.0) == 3.14);
    assert(config2.getOr<bool>("bool_val", false) == true);
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigSaving passed" << std::endl;
}

void testConfigInheritance() {
    nlm::Config baseConfig;
    baseConfig.set("inherited_key", "base_value", nlm::ConfigSource::Default);
    baseConfig.set("base_key", "base_value", nlm::ConfigSource::Default);
    
    nlm::Config derivedConfig;
    derivedConfig.set("derived_key", "derived_value", nlm::ConfigSource::Runtime);
    
    // Merge base into derived
    derivedConfig.merge(baseConfig, nlm::ConfigSource::Inherited);
    
    // Verify both keys exist
    assert(derivedConfig.getOr<std::string>("inherited_key", "") == "base_value");
    assert(derivedConfig.getOr<std::string>("derived_key", "") == "derived_value");
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Enhanced Config Tests ===\n" << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONLoading();
    testYAMLLoading();
    testConfigSchema();
    testConfigSaving();
    testConfigInheritance();
    
    std::cout << "\n=== All Tests Passed ===\n" << std::endl;
}

} // namespace test_config
