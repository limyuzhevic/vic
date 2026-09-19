// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
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
    
    // Test nested configuration
    config.set("nested.key1", "value1", nlm::ConfigSource::Runtime);
    auto nestedVal = config.get<std::string>("nested.key1", "nested");
    assert(nestedVal.has_value());
    assert(*nestedVal == "value1");
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    // Test nested getOr
    int nestedVal = config.getOr<int>("missing", "nested", -1);
    assert(nestedVal == -1);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    // Test nested remove
    config.set("section.subkey", "value", nlm::ConfigSource::Runtime);
    assert(config.has("section.subkey", "section"));
    
    config.remove("subkey", "section");
    assert(!config.has("subkey", "section"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    // Test nested keys
    config.set("section.key1", "value1", nlm::ConfigSource::Runtime);
    config.set("section.key2", "value2", nlm::ConfigSource::Runtime);
    
    auto sectionKeys = config.getKeys("section");
    assert(sectionKeys.size() == 2);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigJSONYAML() {
    nlm::Config config;
    
    // Test JSON serialization
    config.set("string", "test", nlm::ConfigSource::Runtime);
    config.set("number", 42, nlm::ConfigSource::Runtime);
    config.set("boolean", true, nlm::ConfigSource::Runtime);
    
    std::string json = config.toJson();
    nlohmann::json parsed = nlohmann::json::parse(json);
    assert(parsed["string"] == "test");
    assert(parsed["number"] == 42);
    assert(parsed["boolean"] == true);
    
    // Test YAML serialization
    std::string yaml = config.toYaml();
    // YAML check would require YAML library, but at least ensure no crash
    assert(!yaml.empty());
    
    // Test loading from JSON string
    std::string jsonStr = R"({"loaded": true, "value": 123})";
    bool loaded = config.loadFromJson(jsonStr);
    assert(loaded);
    assert(config.getOr<bool>("loaded", false));
    assert(config.getOr<int>("value", 0) == 123);
    
    std::cout << "    testConfigJSONYAML passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Set default validators
    config.setDefaults();
    
    // Test validation for registered parameters
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    auto result = config.validate("random_seed", config.get("random_seed", -1));
    assert(result.isValid);
    
    config.set("neuron_count", -1, nlm::ConfigSource::Runtime);
    result = config.validate("neuron_count", config.get("neuron_count", -1));
    assert(!result.isValid);
    assert(result.errorMessage.find("neuron_count") != std::string::npos);
    
    config.set("log_level", "INVALID", nlm::ConfigSource::Runtime);
    result = config.validate("log_level", config.get("log_level", ""));
    assert(!result.isValid);
    assert(result.errorMessage.find("log_level") != std::string::npos);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSaveLoad() {
    nlm::Config config;
    
    // Set various configuration values
    config.set("string", "test", nlm::ConfigSource::Runtime);
    config.set("number", 42, nlm::ConfigSource::Runtime);
    config.set("boolean", true, nlm::ConfigSource::Runtime);
    config.set("list", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Test saving to JSON file (using pretty print)
    bool saved = config.saveToFile("/tmp/test_config.json", true);
    assert(saved);
    
    // Test loading back
    nlm::Config config2;
    bool loaded = config2.loadFromFile("/tmp/test_config.json");
    assert(loaded);
    
    assert(config2.getOr<std::string>("string", "") == "test");
    assert(config2.getOr<int>("number", 0) == 42);
    assert(config2.getOr<bool>("boolean", false));
    
    std::cout << "    testConfigSaveLoad passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONYAML();
    testConfigValidation();
    testConfigSaveLoad();
}

} // namespace test_config
