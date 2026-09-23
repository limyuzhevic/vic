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

void testConfigSchema() {
    nlm::Config config;
    nlm::ConfigSchema schema("test_schema");
    
    // Add parameters with different types
    nlm::ParameterMetadata intParam;\n    intParam.description = "Integer parameter";
    intParam.type = nlm::ParameterType::INT;
    intParam.required = true;
    intParam.range = std::make_pair(0, 100);
    schema.addParameter("int_param", intParam);
    
    nlm::ParameterMetadata stringParam;
    stringParam.description = "String parameter";
    stringParam.type = nlm::ParameterType::STRING;
    stringParam.required = true;
    stringParam.enumValues = {"option1", "option2", "option3"};
    schema.addParameter("string_param", stringParam);
    
    nlm::ParameterMetadata doubleParam;
    doubleParam.description = "Double parameter";
    doubleParam.type = nlm::ParameterType::DOUBLE;
    doubleParam.range = std::make_pair(0.0, 1.0);
    schema.addParameter("double_param", doubleParam);
    
    // Update config with schema
    config.updateSchema(schema);
    
    // Test validation - missing required parameters
    auto validationErrors = config.validate();
    assert(validationErrors.size() >= 3); // Missing int_param, string_param, double_param
    
    // Add some parameters
    config.set("int_param", 50, nlm::ConfigSource::Runtime);
    config.set("string_param", std::string("option1"), nlm::ConfigSource::Runtime);
    config.set("double_param", 0.5, nlm::ConfigSource::Runtime);
    
    // Test validation - should pass now
    validationErrors = config.validate();
    assert(validationErrors.empty());
    
    // Test validation errors
    config.set("int_param", -10, nlm::ConfigSource::Runtime); // Outside range
    validationErrors = config.validate();
    assert(!validationErrors.empty()); // Should have range error
    
    config.set("string_param", std::string("invalid"), nlm::ConfigSource::Runtime); // Invalid enum
    validationErrors = config.validate();
    assert(!validationErrors.empty()); // Should have enum error
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigFileFormats() {
    nlm::Config config;
    
    // Test simple format (backward compatibility)
    config.set("simple_key", std::string("value1"), nlm::ConfigSource::Runtime);
    config.set("simple_num", 123, nlm::ConfigSource::Runtime);
    config.set("simple_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to simple file
    bool saveSuccess = config.saveToFile("/tmp/test_simple.cfg");
    assert(saveSuccess);
    
    // Load back
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile("/tmp/test_simple.cfg");
    assert(loadSuccess);
    
    // Compare
    assert(config2.has("simple_key"));
    assert(config2.getOr<std::string>("simple_key", "") == "value1");
    assert(config2.has("simple_num"));
    assert(config2.getOr<int>("simple_num", 0) == 123);
    assert(config2.has("simple_bool"));
    assert(config2.getOr<bool>("simple_bool", false) == true);
    
    // Test JSON format
    config.clear();
    config.set("json_key", std::string("json_value"), nlm::ConfigSource::Runtime);
    config.set("json_number", 3.14, nlm::ConfigSource::Runtime);
    config.set("json_flag", false, nlm::ConfigSource::Runtime);
    
    saveSuccess = config.saveToFile("/tmp/test_json.json");
    assert(saveSuccess);
    
    config2.clear();
    loadSuccess = config2.loadFromFile("/tmp/test_json.json");
    assert(loadSuccess);
    
    assert(config2.getOr<std::string>("json_key", "") == "json_value");
    
    // Test YAML format
    config.clear();
    config.set("yaml_key", std::string("yaml_value"), nlm::ConfigSource::Runtime);
    config.set("yaml_list", std::vector<std::string>{"item1", "item2", "item3"}, nlm::ConfigSource::Runtime);
    
    saveSuccess = config.saveToFile("/tmp/test_yaml.yaml");
    assert(saveSuccess);
    
    config2.clear();
    loadSuccess = config2.loadFromFile("/tmp/test_yaml.yaml");
    assert(loadSuccess);
    
    assert(config2.getOr<std::string>("yaml_key", "") == "yaml_value");
    
    auto yamlList = config2.getAny("yaml_list");
    assert(yamlList.has_value());
    auto listVal = std::get<std::vector<std::string>>(yamlList.value());
    assert(listVal.size() == 3);
    assert(listVal[0] == "item1");
    
    std::cout << "    testConfigFileFormats passed" << std::endl;
}

void testConfigUtils() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set up config1
    config1.set("key1", std::string("value1"), nlm::ConfigSource::Runtime);
    config1.set("key2", 42, nlm::ConfigSource::Runtime);
    config1.set("key3", true, nlm::ConfigSource::Runtime);
    
    // Set up config2 - same as config1
    config2.set("key1", std::string("value1"), nlm::ConfigSource::Runtime);
    config2.set("key2", 42, nlm::ConfigSource::Runtime);
    config2.set("key3", true, nlm::ConfigSource::Runtime);
    
    // Test comparison
    bool equal = nlm::ConfigUtils::compare(config1, config2);
    assert(equal);
    
    // Modify config2
    config2.set("key2", 43, nlm::ConfigSource::Runtime);
    equal = nlm::ConfigUtils::compare(config1, config2);
    assert(!equal);
    
    // Test merging
    nlm::Config merged = nlm::ConfigUtils::merge(config1, config2, true);
    assert(merged.getOr<int>("key2", 0) == 43); // Should overwrite with config2 value
    
    nlm::Config merged2 = nlm::ConfigUtils::merge(config1, config2, false);
    assert(merged2.getOr<int>("key2", 0) == 42); // Should keep config1 value
    
    // Test diffing
    std::vector<std::string> differences = nlm::ConfigUtils::diff(config1, config2);
    assert(!differences.empty());
    
    // Add a new key to config2
    config2.set("key4", std::string("value4"), nlm::ConfigSource::Runtime);
    differences = nlm::ConfigUtils::diff(config1, config2);
    assert(!differences.empty());
    
    std::cout << "    testConfigUtils passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    bool loadSuccess = config.loadFromFile("/tmp/non_existent_file_12345.cfg");
    assert(!loadSuccess);
    
    // Test loading malformed JSON
    std::ofstream malformedJson("/tmp/malformed.json");
    malformedJson << "{ " << std::endl;
    malformedJson.close();
    
    loadSuccess = config.loadFromFile("/tmp/malformed.json");
    assert(!loadSuccess);
    
    // Test loading malformed YAML
    std::ofstream malformedYaml("/tmp/malformed.yaml");
    malformedYaml << "key: [unclosed bracket" << std::endl;
    malformedYaml.close();
    
    loadSuccess = config.loadFromFile("/tmp/malformed.yaml");
    assert(!loadSuccess);
    
    // Test loading malformed simple format
    std::ofstream malformedSimple("/tmp/malformed.cfg");
    malformedSimple << "key=value" << std::endl;
    malformedSimple << "invalid_line" << std::endl;
    malformedSimple << "key2 = value2" << std::endl;
    malformedSimple.close();
    
    loadSuccess = config.loadFromFile("/tmp/malformed.cfg");
    assert(!loadSuccess);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSchema();
    testConfigFileFormats();
    testConfigUtils();
    testConfigErrorHandling();
}

} // namespace test_config
