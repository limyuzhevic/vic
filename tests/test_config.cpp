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

void testConfigJsonSaveLoad() {
    nlm::Config config;
    
    config.set("name", std::string("TestConfig"), nlm::ConfigSource::File);
    config.set("count", 42, nlm::ConfigSource::File);
    config.set("enabled", true, nlm::ConfigSource::File);
    config.set("rate", 3.14159, nlm::ConfigSource::File);
    
    // Save to JSON
    std::string jsonContent = config.toJsonString();
    assert(!jsonContent.empty());
    
    // Create new config and load from JSON
    nlm::Config config2;
    config2.loadFromString(jsonContent, nlm::ConfigFormat::JSON);
    
    assert(config2.has("name"));
    assert(config2.get<std::string>("name").value() == "TestConfig");
    assert(config2.has("count"));
    assert(config2.get<int64_t>("count").value() == 42);
    assert(config2.has("enabled"));
    assert(config2.get<bool>("enabled").value() == true);
    assert(config2.has("rate"));
    assert(config2.get<double>("rate").value() > 3.14 && config2.get<double>("rate").value() < 3.15);
    
    std::cout << "    testConfigJsonSaveLoad passed" << std::endl;
}

void testConfigYamlSaveLoad() {
    nlm::Config config;
    
    config.set("database", std::string("localhost"), nlm::ConfigSource::File);
    config.set("port", 5301LL, nlm::ConfigSource::File);
    config.set("ssl", false, nlm::ConfigSource::File);
    
    // Save to YAML
    std::string yamlContent = config.toYamlString();
    assert(!yamlContent.empty());
    
    // Create new config and load from YAML
    nlm::Config config2;
    config2.loadFromString(yamlContent, nlm::ConfigFormat::YAML);
    
    assert(config2.has("database"));
    assert(config2.get<std::string>("database").value() == "localhost");
    assert(config2.has("port"));
    assert(config2.get<int64_t>("port").value() == 5301);
    assert(config2.has("ssl"));
    assert(config2.get<bool>("ssl").value() == false);
    
    std::cout << "    testConfigYamlSaveLoad passed" << std::endl;
}

void testConfigFileLoad() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string jsonPath = "/tmp/test_config.json";
    config.set("test_key", "json_test", nlm::ConfigSource::File);
    config.set("test_value", 123, nlm::ConfigSource::File);
    
    bool saved = config.saveToFile(jsonPath, nlm::ConfigFormat::JSON);
    assert(saved);
    
    // Load from file
    nlm::Config config2;
    bool loaded = config2.loadFromFile(jsonPath);
    assert(loaded);
    assert(config2.has("test_key"));
    assert(config2.get<std::string>("test_key").value() == "json_test");
    assert(config2.has("test_value"));
    assert(config2.get<int64_t>("test_value").value() == 123);
    
    // Create a temporary YAML file
    std::string yamlPath = "/tmp/test_config.yaml";
    config.set("yaml_key", "yaml_test", nlm::ConfigSource::File);
    config.set("yaml_value", 456, nlm::ConfigSource::File);
    
    saved = config.saveToFile(yamlPath, nlm::ConfigFormat::YAML);
    assert(saved);
    
    // Load from file
    nlm::Config config3;
    loaded = config3.loadFromFile(yamlPath);
    assert(loaded);
    assert(config3.has("yaml_key"));
    assert(config3.get<std::string>("yaml_key").value() == "yaml_test");
    assert(config3.has("yaml_value"));
    assert(config3.get<int64_t>("yaml_value").value() == 456);
    
    // Test backward compatibility with key=value format
    std::string cfgPath = "/tmp/test_config.cfg";
    std::ofstream cfgFile(cfgPath);
    cfgFile << "# Test config file\n";
    cfgFile << "simple_key = simple_value\n";
    cfgFile << "numeric = 42\n";
    cfgFile << "boolean = true\n";
    cfgFile.close();
    
    nlm::Config config4;
    loaded = config4.loadFromFile(cfgPath);
    assert(loaded);
    assert(config4.has("simple_key"));
    assert(config4.get<std::string>("simple_key").value() == "simple_value");
    assert(config4.has("numeric"));
    assert(config4.get<int64_t>("numeric").value() == 42);
    assert(config4.has("boolean"));
    assert(config4.get<bool>("boolean").value() == true);
    
    std::cout << "    testConfigFileLoad passed" << std::endl;
}

void testConfigFormatDetection() {
    nlm::Config config;
    
    // Test JSON file detection
    config.set("key", "value", nlm::ConfigSource::File);
    std::string jsonPath = "/tmp/test_json.json";
    config.saveToFile(jsonPath, nlm::ConfigFormat::JSON);
    assert(config.detectFormat(jsonPath) == nlm::ConfigFormat::JSON);
    
    // Test YAML file detection
    std::string yamlPath = "/tmp/test_yaml.yaml";
    config.saveToFile(yamlPath, nlm::ConfigFormat::YAML);
    assert(config.detectFormat(yamlPath) == nlm::ConfigFormat::YAML);
    
    // Test YAML alternative extension
    std::string ymlPath = "/tmp/test_yml.yml";
    config.saveToFile(ymlPath, nlm::ConfigFormat::YAML);
    assert(config.detectFormat(ymlPath) == nlm::ConfigFormat::YAML);
    
    // Test key=value file detection (no extension)
    std::string cfgPath = "/tmp/test_cfg.cfg";
    config.saveToFile(cfgPath, nlm::ConfigFormat::JSON); // Actually saves JSON
    assert(config.detectFormat(cfgPath) == nlm::ConfigFormat::JSON); // .cfg defaults to JSON
    
    // Test backward compatibility - simple file
    std::string simplePath = "/tmp/simple_config";
    std::ofstream simpleFile(simplePath);
    simpleFile << "legacy = true\n";
    simpleFile.close();
    assert(config.detectFormat(simplePath) == nlm::ConfigFormat::SimpleKeyValue);
    
    std::cout << "    testConfigFormatDetection passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test invalid values
    config.set("invalid_timestep", 2.0, nlm::ConfigSource::File); // > 1.0
    config.set("invalid_neurons", 0, nlm::ConfigSource::File);    // == 0
    config.set("invalid_regions", 200, nlm::ConfigSource::File);  // > 100
    
    // These should fail validation
    assert(!config.validate());
    
    // Set valid values
    config.set("valid_timestep", 0.001, nlm::ConfigSource::File);
    config.set("valid_neurons", 1000, nlm::ConfigSource::File);
    config.set("valid_regions", 5, nlm::ConfigSource::File);
    
    // This should pass validation
    assert(config.validate());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigCriticalDefaults() {
    nlm::Config config;
    
    // Clear config
    config.clear();
    
    // Should not have critical values yet
    assert(!config.has("simulation_timestep"));
    assert(!config.has("neuron_count"));
    assert(!config.has("region_count"));
    
    // Set critical defaults
    config.setCriticalDefaults();
    
    // Should now have critical values
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    
    assert(config.get<double>("simulation_timestep").value() == 0.001);
    assert(config.get<int64_t>("neuron_count").value() == 1000);
    assert(config.get<int64_t>("region_count").value() == 2);
    
    std::cout << "    testConfigCriticalDefaults passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJsonSaveLoad();
    testConfigYamlSaveLoad();
    testConfigFileLoad();
    testConfigFormatDetection();
    testConfigValidation();
    testConfigCriticalDefaults();
    
    std::cout << "\nAll configuration tests passed!" << std::endl;
}

} // namespace test_config
