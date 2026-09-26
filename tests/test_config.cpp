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

void testConfigJSONIntegration() {
    nlm::Config config;
    
    // Test JSON file loading
    std::string jsonContent = "{\"config_value\": 123, \"enabled\": true}";
    assert(config.loadFromJSON(jsonContent));
    assert(config.has("config_value"));
    assert(config.get<int>("config_value").value() == 123);
    
    std::cout << "    testConfigJSONIntegration passed" << std::endl;
}

void testConfigYAMLIntegration() {
    nlm::Config config;
    
    // Test YAML file loading (fallback to JSON)
    std::string yamlContent = "config_value: 456\nenabled: false";
    assert(config.loadFromYAML(yamlContent));
    assert(config.has("config_value"));
    assert(config.get<int>("config_value").value() == 456);
    
    std::cout << "    testConfigYAMLIntegration passed" << std::endl;
}

void testConfigFileOperations() {
    nlm::Config config;
    
    // Create a temporary config file
    std::string tempFile = "/tmp/test_config_file.json";
    config.set("file_key", "file_value", nlm::ConfigSource::Runtime);
    assert(config.saveToFile(tempFile));
    
    // Load from file
    nlm::Config config2;
    assert(config2.loadFromFile(tempFile));
    assert(config2.has("file_key"));
    assert(config2.get<std::string>("file_key").value() == "file_value");
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test basic validation
    config.set("required_field", "value", nlm::ConfigSource::Runtime);
    config.setRequired("required_field", true);
    
    assert(config.validate());
    
    // Test validation failure
    config.remove("required_field");
    assert(!config.validate());
    
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigDependencies() {
    nlm::Config config;
    
    // Test dependency management
    config.set("dependency", "value", nlm::ConfigSource::Runtime);
    assert(config.addDependency("dependent", "dependency"));
    
    config.set("dependent", "dependent_value", nlm::ConfigSource::Runtime, true);
    assert(config.validate());
    
    std::cout << "    testConfigDependencies passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONIntegration();
    testConfigYAMLIntegration();
    testConfigFileOperations();
    testConfigValidation();
    testConfigDependencies();
}

} // namespace test_config
