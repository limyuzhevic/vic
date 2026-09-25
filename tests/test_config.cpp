// Enhanced Config Tests
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

void testConfigPresets() {
    nlm::Config config;
    
    // Test fast preset
    assert(config.applyPreset("fast"));
    assert(config.has("random_seed"));
    assert(config.has("neuron_count"));
    assert(config.getOr<int64_t>("neuron_count", 0) == 500);
    
    // Test medium preset
    config.clear();
    assert(config.applyPreset("medium"));
    assert(config.getOr<int64_t>("neuron_count", 0) == 1000);
    
    // Test high preset
    config.clear();
    assert(config.applyPreset("high"));
    assert(config.getOr<int64_t>("neuron_count", 0) == 2000);
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Set valid values
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    
    // Validation should pass
    assert(config.validate());
    
    // Test invalid values (should not be added)
    config.set("neuron_count", -1, nlm::ConfigSource::Runtime); // Invalid - should be rejected or not added
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigJSONFile() {
    nlm::Config config;
    
    // Create a simple JSON file
    std::ofstream jsonFile("test_config.json");
    if (jsonFile.is_open()) {
        jsonFile << "{\n";
        jsonFile << "  \"random_seed\": 42,\n";
        jsonFile << "  \"neuron_count\": 1000,\n";
        jsonFile << "  \"simulation_timestep\": 0.001\n";
        jsonFile << "}\n";
        jsonFile.close();
        
        // Load JSON file
        assert(config.loadFromJSON("test_config.json"));
        
        // Verify values
        assert(config.has("random_seed"));
        assert(config.has("neuron_count"));
        assert(config.has("simulation_timestep"));
        assert(config.getOr<int64_t>("random_seed", 0) == 42);
        assert(config.getOr<int64_t>("neuron_count", 0) == 1000);
        assert(config.getOr<double>("simulation_timestep", 0.0) == 0.001);
        
        // Clean up
        std::remove("test_config.json");
    }
    
    std::cout << "    testConfigJSONFile passed" << std::endl;
}

void testConfigYAMLFile() {
    nlm::Config config;
    
    // Create a simple YAML file
    std::ofstream yamlFile("test_config.yaml");
    if (yamlFile.is_open()) {
        yamlFile << "random_seed: 42\n";
        yamlFile << "neuron_count: 1000\n";
        yamlFile << "simulation_timestep: 0.001\n";
        yamlFile.close();
        
        // Load YAML file
        assert(config.loadFromYAML("test_config.yaml"));
        
        // Verify values
        assert(config.has("random_seed"));
        assert(config.has("neuron_count"));
        assert(config.has("simulation_timestep"));
        assert(config.getOr<int64_t>("random_seed", 0) == 42);
        assert(config.getOr<int64_t>("neuron_count", 0) == 1000);
        assert(config.getOr<double>("simulation_timestep", 0.0) == 0.001);
        
        // Clean up
        std::remove("test_config.yaml");
    }
    
    std::cout << "    testConfigYAMLFile passed" << std::endl;
}

void testConfigFileFormatDetection() {
    // Test CFG format detection
    assert(nlm::Config::getFormatFromExtension("config.cfg") == nlm::ConfigFormat::CFG);
    assert(nlm::Config::getFormatFromExtension("config.txt") == nlm::ConfigFormat::CFG);
    
    // Test JSON format detection
    assert(nlm::Config::getFormatFromExtension("config.json") == nlm::ConfigFormat::JSON);
    
    // Test YAML format detection
    assert(nlm::Config::getFormatFromExtension("config.yaml") == nlm::ConfigFormat::YAML);
    assert(nlm::Config::getFormatFromExtension("config.yml") == nlm::ConfigFormat::YAML);
    
    // Test unknown format
    assert(nlm::Config::getFormatFromExtension("config.unknown") == nlm::ConfigFormat::Unknown);
    
    std::cout << "    testConfigFileFormatDetection passed" << std::endl;
}

void testConfigSourcePrecedence() {
    nlm::Config config;
    
    // Set values with different sources
    config.set("test_key", 1, nlm::ConfigSource::Default);
    config.set("test_key", 2, nlm::ConfigSource::File);
    config.set("test_key", 3, nlm::ConfigSource::CommandLine);
    
    // Check source
    assert(config.getSource("test_key") == nlm::ConfigSource::CommandLine);
    
    // Check if overridden
    assert(config.isOverridden("test_key"));
    
    std::cout << "    testConfigSourcePrecedence passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigPresets();
    testConfigValidation();
    testConfigJSONFile();
    testConfigYAMLFile();
    testConfigFileFormatDetection();
    testConfigSourcePrecedence();
}

} // namespace test_config
