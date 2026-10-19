// Config Tests - Updated for new JSON/YAML support
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
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
    
    // Test vector types
    config.set("int_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    auto intVector = config.get<std::vector<int>>("int_vector");
    assert(intVector.has_value());
    assert(intVector->size() == 3);
    assert((*intVector)[0] == 1);
    
    config.set("string_vector", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    auto stringVector = config.get<std::vector<std::string>>("string_vector");
    assert(stringVector.has_value());
    assert(stringVector->size() == 3);
    
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
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    
    config.clear();
    
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(config.getKeys().size() == 0);
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("str_key", "test", nlm::ConfigSource::File);
    config.set("bool_key", true, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration (3 entries):") != std::string::npos);
    assert(summary.find("int_key = [42]") != std::string::npos);
    assert(summary.find("str_key = [\"test\"]") != std::string::npos);
    assert(summary.find("bool_key = [true]") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testJSONLoad() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string testFile = "test_config.json";
    std::ofstream jsonFile(testFile);
    jsonFile << "{\"int_value\": 42, \"string_value\": \"hello\", \"bool_value\": true}";
    jsonFile.close();
    
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    assert(config.has("int_value"));
    assert(config.getOr<int>("int_value", 0) == 42);
    assert(config.has("string_value"));
    assert(config.getOr<std::string>("string_value", "") == "hello");
    assert(config.has("bool_value"));
    assert(config.getOr<bool>("bool_value", false) == true);
    
    std::remove(testFile.c_str());
    
    std::cout << "    testJSONLoad passed" << std::endl;
}

void testYAMLLoad() {
    nlm::Config config;
    
    // Create a temporary YAML file
    std::string testFile = "test_config.yaml";
    std::ofstream yamlFile(testFile);
    yamlFile << "int_value: 42\nstring_value: hello\nbool_value: true";
    yamlFile.close();
    
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    assert(config.has("int_value"));
    assert(config.getOr<int>("int_value", 0) == 42);
    
    std::remove(testFile.c_str());
    
    std::cout << "    testYAMLLoad passed" << std::endl;
}

void testJSONSave() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    std::string testFile = "test_save.json";
    bool saved = config.saveToFile(testFile, "json");
    assert(saved);
    
    // Load it back and verify
    nlm::Config config2;
    bool loaded = config2.loadFromFile(testFile);
    assert(loaded);
    assert(config2.has("int_value"));
    assert(config2.getOr<int>("int_value", 0) == 42);
    
    std::remove(testFile.c_str());
    
    std::cout << "    testJSONSave passed" << std::endl;
}

void testYAMLSave() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    
    std::string testFile = "test_save.yaml";
    bool saved = config.saveToFile(testFile, "yaml");
    assert(saved);
    
    // Load it back and verify
    nlm::Config config2;
    bool loaded = config2.loadFromFile(testFile);
    assert(loaded);
    
    std::remove(testFile.c_str());
    
    std::cout << "    testYAMLSave passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_string", "test", nlm::ConfigSource::Runtime);
    config.set("valid_bool", true, nlm::ConfigSource::Runtime);
    
    assert(config.validate("valid_int"));
    assert(config.validate("valid_string"));
    assert(config.validate("valid_bool"));
    assert(!config.validate("non_existent_key"));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigMetadata() {
    nlm::Config config;
    
    config.set("test_key", 42, nlm::ConfigSource::Runtime);
    config.setDescription("test_key", "A test configuration key");
    config.setType("test_key", "integer");
    config.setValidation("test_key", "range(0,100)");
    config.setDefaultValue("test_key", "0");
    config.setRequired("test_key", true);
    
    assert(config.getDescription("test_key") == "A test configuration key");
    assert(config.getType("test_key") == "integer");
    assert(config.getValidation("test_key") == "range(0,100)");
    assert(config.getDefaultValue("test_key") == "0");
    assert(config.isRequired("test_key") == true);
    
    std::cout << "    testConfigMetadata passed" << std::endl;
}

void testConfigInheritance() {
    nlm::Config baseConfig;
    baseConfig.set("base_key", 1, nlm::ConfigSource::Default);
    
    nlm::Config childConfig;
    childConfig.loadFromConfig(baseConfig, nlm::ConfigSource::File);
    
    assert(childConfig.has("base_key"));
    assert(childConfig.getOr<int>("base_key", 0) == 1);
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", 2, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key2", 3, nlm::ConfigSource::Runtime);
    config2.set("key3", 4, nlm::ConfigSource::Runtime);
    
    config1.mergeWith(config2, nlm::ConfigSource::CommandLine);
    
    assert(config1.has("key1"));
    assert(config1.getOr<int>("key1", 0) == 1);
    assert(config1.has("key2"));
    assert(config1.getOr<int>("key2", 0) == 3); // Should be overridden
    assert(config1.has("key3"));
    assert(config1.getOr<int>("key3", 0) == 4);
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigVersion() {
    nlm::Config config;
    
    std::string version = config.getVersion();
    assert(version == "2.0.0");
    
    std::cout << "    testConfigVersion passed" << std::endl;
}

void testConfigPerformance() {
    nlm::Config config;
    
    // Set many values to test performance
    for (int i = 0; i < 100; ++i) {
        config.set("key_" + std::to_string(i), i, nlm::ConfigSource::Runtime);
    }
    
    // Check that all values are accessible
    for (int i = 0; i < 100; ++i) {
        assert(config.has("key_" + std::to_string(i)));
        assert(config.getOr<int>("key_" + std::to_string(i), -1) == i);
    }
    
    std::cout << "    testConfigPerformance passed" << std::endl;
}

void testConfigDiff() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", 2, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key2", 3, nlm::ConfigSource::Runtime);
    config2.set("key3", 4, nlm::ConfigSource::Runtime);
    
    std::string diff = config1.diff(config2);
    assert(!diff.empty());
    assert(diff.find("Added") != std::string::npos || diff.find("Changed") != std::string::npos);
    
    std::cout << "    testConfigDiff passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigClear();
    testConfigSummary();
    testJSONLoad();
    testYAMLLoad();
    testJSONSave();
    testYAMLSave();
    testConfigValidation();
    testConfigMetadata();
    testConfigInheritance();
    testConfigMerge();
    testConfigVersion();
    testConfigPerformance();
    testConfigDiff();
    
    std::cout << "\nAll Config tests passed!" << std::endl;
}

} // namespace test_config
