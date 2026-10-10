// Simple test to verify the new configuration system
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing new NLM configuration system..." << std::endl;
    
    nlm::Config config;
    
    // Test basic functionality
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    assert(config.getOr<int>("test_int", 0) == 42);
    
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
    assert(config.getOr<std::string>("test_string", "") == "hello");
    
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    assert(config.getOr<double>("test_double", 0.0) == 3.14);
    
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    assert(config.getOr<bool>("test_bool", false) == true);
    
    // Test schema
    nlm::ConfigSchema schema;
    schema.addEntry(nlm::ConfigSchemaEntry(
        "schema_test", "Test schema entry", nlm::ConfigCategory::System,
        std::optional<nlm::ConfigValue>(100), false, "int"
    ));
    
    config.setSchema(schema);
    config.set("schema_test", 200, nlm::ConfigSource::Runtime);
    assert(config.getOr<int>("schema_test", 0) == 200);
    
    // Test file loading
    bool jsonLoaded = config.loadFromFile("configs/config.json");
    if (jsonLoaded) {
        std::cout << "Successfully loaded JSON config!" << std::endl;
        std::cout << config.summary() << std::endl;
    }
    
    bool yamlLoaded = config.loadFromFile("configs/config.yaml");
    if (yamlLoaded) {
        std::cout << "Successfully loaded YAML config!" << std::endl;
        std::cout << config.summary() << std::endl;
    }
    
    // Test saving
    bool jsonSaved = config.saveToFile("test_output.json", true);
    bool yamlSaved = config.saveToFile("test_output.yaml", false);
    
    if (jsonSaved && yamlSaved) {
        std::cout << "Successfully saved config to both JSON and YAML formats!" << std::endl;
    }
    
    // Test inheritance
    nlm::Config baseConfig;
    baseConfig.set("inherited", "base_value", nlm::ConfigSource::Default);
    
    nlm::Config derivedConfig;
    derivedConfig.set("derived", "derived_value", nlm::ConfigSource::Runtime);
    derivedConfig.merge(baseConfig, nlm::ConfigSource::Inherited);
    
    assert(derivedConfig.getOr<std::string>("inherited", "") == "base_value");
    assert(derivedConfig.getOr<std::string>("derived", "") == "derived_value");
    
    std::cout << "Successfully tested configuration inheritance!" << std::endl;
    
    // Clean up test files
    std::remove("test_output.json");
    std::remove("test_output.yaml");
    
    std::cout << "\n=== All configuration system tests passed! ===" << std::endl;
    
    return 0;
}
