// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

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

void testConfigJSONFile() {
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("int_vector", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    assert(config.saveToJSON("test_config.json"));
    
    // Load from JSON file
    nlm::Config config2;
    assert(config2.loadFromJSON("test_config.json"));
    
    // Verify values
    assert(config2.has("int_value"));
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto stringVal = config2.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    std::cout << "    testConfigJSONFile passed" << std::endl;
}

void testConfigYAMLFile() {
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 100, nlm::ConfigSource::Runtime);
    config.set("double_value", 2.71, nlm::ConfigSource::Runtime);
    config.set("bool_value", false, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("yaml_test"), nlm::ConfigSource::Runtime);
    config.set("string_vector", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Save to YAML file
    assert(config.saveToYAML("test_config.yaml"));
    
    // Load from YAML file
    nlm::Config config2;
    assert(config2.loadFromYAML("test_config.yaml"));
    
    // Verify values
    assert(config2.has("int_value"));
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 100);
    
    auto boolVal = config2.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == false);
    
    std::cout << "    testConfigYAMLFile passed" << std::endl;
}

void testConfigAutoDetectFileFormat() {
    nlm::Config config;
    
    // Set values
    config.set("test_int", 123, nlm::ConfigSource::Runtime);
    
    // Save as JSON (should detect from .json extension)
    assert(config.saveToFile("test_auto.json"));
    
    // Save as YAML (should detect from .yaml extension)
    assert(config.saveToFile("test_auto.yaml"));
    
    // Load JSON
    nlm::Config config1;
    assert(config1.loadFromFile("test_auto.json"));
    assert(config1.has("test_int"));
    
    // Load YAML
    nlm::Config config2;
    assert(config2.loadFromFile("test_auto.yaml"));
    assert(config2.has("test_int"));
    
    std::cout << "    testConfigAutoDetectFileFormat passed" << std::endl;
}

void testConfigFileExtensionDetection() {
    assert(nlm::Config::isJSONFile("test.json"));
    assert(nlm::Config::isJSONFile("test.JSON"));
    assert(nlm::Config::isJSONFile("no_ext"));
    assert(!nlm::Config::isJSONFile("test.yaml"));
    assert(!nlm::Config::isJSONFile("test.yml"));
    
    assert(nlm::Config::isYAMLFile("test.yaml"));
    assert(nlm::Config::isYAMLFile("test.yml"));
    assert(nlm::Config::isYAMLFile("no_ext"));
    assert(!nlm::Config::isYAMLFile("test.json"));
    
    assert(nlm::Config::getFileExtension("test.json") == "json");
    assert(nlm::Config::getFileExtension("test.yaml") == "yaml");
    assert(nlm::Config::getFileExtension("test.yml") == "yml");
    assert(nlm::Config::getFileExtension("no_ext") == "");
    
    std::cout << "    testConfigFileExtensionDetection passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Valid config
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_string", "test", nlm::ConfigSource::Runtime);
    assert(config.validateConfig());
    
    // Invalid config (empty string)
    nlm::Config invalidConfig;
    invalidConfig.set("empty_string", "", nlm::ConfigSource::Runtime);
    assert(!invalidConfig.validateConfig());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigGetConfigValue() {
    nlm::Config config;
    
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "value", nlm::ConfigSource::Runtime);
    
    // Test type-safe retrieval
    auto intVal = config.getConfigValue<int>("int_key");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto stringVal = config.getConfigValue<std::string>("string_key");
    assert(stringVal.has_value());
    assert(*stringVal == "value");
    
    // Test default value
    auto missingVal = config.getConfigValue<int>("missing", 99);
    assert(missingVal == 99);
    
    std::cout << "    testConfigGetConfigValue passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("key1", "value1", nlm::ConfigSource::Runtime);
    config1.set("key2", 42, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key2", "new_value", nlm::ConfigSource::File); // Different type and source
    config2.set("key3", true, nlm::ConfigSource::CommandLine);
    
    // Merge without override
    assert(config1.mergeConfig(config2, false));
    assert(config1.has("key1"));
    assert(config1.has("key2")); // Should keep original
    assert(config1.has("key3")); // Should add new
    
    // Verify types
    auto key1Val = config1.get<std::string>("key1");
    assert(key1Val.has_value());
    assert(*key1Val == "value1");
    
    auto key2Val = config1.get<int>("key2"); // Original int type should be preserved
    assert(key2Val.has_value());
    assert(*key2Val == 42);
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigAsString() {
    nlm::Config config;
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "test", nlm::ConfigSource::File);
    config.set("bool_key", true, nlm::ConfigSource::CommandLine);
    
    std::string configStr = config.getConfigAsString();
    assert(configStr.find("Configuration:") != std::string::npos);
    assert(configStr.find("int_key: 42") != std::string::npos);
    assert(configStr.find("string_key: \"test\"") != std::string::npos);
    assert(configStr.find("bool_key: true") != std::string::npos);
    assert(configStr.find("Valid: Yes") != std::string::npos);
    
    std::cout << "    testConfigAsString passed" << std::endl;
}

void testConfigBackwardCompatibility() {
    nlm::Config config;
    
    // Test loading old simple key=value format
    std::ofstream oldFormatFile("old_format.txt");
    oldFormatFile << "# This is a comment\n";
    oldFormatFile << "key1 = \"value1\"\n";
    oldFormatFile << "key2 = 123\n";
    oldFormatFile << "key3 = 3.14\n";
    oldFormatFile.close();
    
    // Load from file (should use backward compatible format)
    nlm::Config config1;
    assert(config1.loadFromFile("old_format.txt"));
    
    assert(config1.has("key1"));
    assert(config1.has("key2"));
    assert(config1.has("key3"));
    
    auto key1Val = config1.get<std::string>("key1");
    assert(key1Val.has_value());
    assert(*key1Val == "value1");
    
    auto key2Val = config1.get<int>("key2");
    assert(key2Val.has_value());
    assert(*key2Val == 123);
    
    auto key3Val = config1.get<double>("key3");
    assert(key3Val.has_value());
    assert(*key3Val > 3.13 && *key3Val < 3.15);
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONFile();
    testConfigYAMLFile();
    testConfigAutoDetectFileFormat();
    testConfigFileExtensionDetection();
    testConfigValidation();
    testConfigGetConfigValue();
    testConfigMerge();
    testConfigAsString();
    testConfigBackwardCompatibility();
}

} // namespace test_config