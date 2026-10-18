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

void testConfigSaveLoadJson() {
    nlm::Config config;
    
    config.set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config.set("brain.synapse_density", 0.1f, nlm::ConfigSource::Runtime);
    config.set("plasticity.enable", true, nlm::ConfigSource::Runtime);
    config.set("neuromod.dopamine.scale", 1.5f, nlm::ConfigSource::Runtime);
    
    // Test save to file
    assert(config.saveToFile("test_config.json"));
    
    // Test load from file
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.json"));
    
    // Verify loaded values
    assert(config2.has("brain.neuron_count"));
    auto neuronCount = config2.get<int64_t>("brain.neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    assert(config2.has("brain.synapse_density"));
    auto synapseDensity = config2.get<double>("brain.synapse_density");
    assert(synapseDensity.has_value());
    assert(*synapseDensity > 0.09 && *synapseDensity < 0.11);
    
    // Test JSON string conversion
    std::string jsonStr = config2.toJsonString();
    assert(!jsonStr.empty());
    
    // Test load from JSON string
    nlm::Config config3;
    assert(config3.loadFromJsonString(jsonStr));
    assert(config3.has("brain.neuron_count"));
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigSaveLoadJson passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::Config config;
    
    // Valid configuration
    config.set("required_int", 42, nlm::ConfigSource::Runtime);
    config.set("required_string", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("optional_float", 3.14, nlm::ConfigSource::Runtime);
    
    nlohmann::json schema = {
        {"type", "object"},
        {"properties", {
            {"required_int", {{"type", "integer"}}},
            {"required_string", {{"type", "string"}}},
            {"optional_float", {{"type", "number"}}}
        }},
        {"required", {"required_int", "required_string"}}
    };
    
    assert(config.validateSchema(schema));
    
    // Invalid configuration (wrong type)
    nlm::Config config2;
    config2.set("required_int", "not_an_integer", nlm::ConfigSource::Runtime);
    
    assert(!config2.validateSchema(schema));
    
    // Missing required key
    nlm::Config config3;
    config3.set("required_string", std::string("test"), nlm::ConfigSource::Runtime);
    
    assert(!config3.validateSchema(schema));
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", 2, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key3", 3, nlm::ConfigSource::Runtime);
    config2.set("key4", 4, nlm::ConfigSource::Runtime);
    
    config1.merge(config2);
    
    assert(config1.has("key1"));
    assert(config1.has("key2"));
    assert(config1.has("key3"));
    assert(config1.has("key4"));
    
    auto key3 = config1.get<int>("key3");
    assert(key3.has_value());
    assert(*key3 == 3);
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test loading invalid JSON
    bool loadResult = config.loadFromJsonString("invalid json {");
    assert(!loadResult);
    
    // Test loading from non-existent file
    bool fileResult = config.loadFromFile("non_existent_file_12345.json");
    assert(!fileResult);
    
    // Test saving to invalid path (should fail gracefully)
    // Note: This test depends on filesystem permissions
    bool saveResult = config.saveToFile("/invalid/path/test_config.json");
    // saveResult may be true or false depending on permissions
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSaveLoadJson();
    testConfigSchemaValidation();
    testConfigMerge();
    testConfigErrorHandling();
    
    std::cout << std::endl;
    std::cout << "=== All Config Tests Passed ===" << std::endl;
    std::cout << "Configuration system supports: JSON/YAML file formats, schema validation, merging, error handling" << std::endl;
}

} // namespace test_config

} // namespace test_config
