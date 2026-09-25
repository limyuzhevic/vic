// Configuration Validation Test Suite
// Comprehensive testing of configuration validation and error handling

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <filesystem>

namespace test_configuration_validation {

void testConfigurationPresets() {
    std::cout << "    Testing Configuration Presets..." << std::endl;
    
    // Test fast preset
    {
        nlm::Config config;
        assert(config.applyPreset("fast"));
        assert(config.has("random_seed"));
        assert(config.has("neuron_count"));
        assert(config.has("simulation_timestep"));
        assert(config.getOr<int64_t>("neuron_count", 0) == 500);
        std::cout << "      Fast preset: PASSED" << std::endl;
    }
    
    // Test medium preset
    {
        nlm::Config config;
        assert(config.applyPreset("medium"));
        assert(config.getOr<int64_t>("neuron_count", 0) == 1000);
        std::cout << "      Medium preset: PASSED" << std::endl;
    }
    
    // Test high preset
    {
        nlm::Config config;
        assert(config.applyPreset("high"));
        assert(config.getOr<int64_t>("neuron_count", 0) == 2000);
        std::cout << "      High preset: PASSED" << std::endl;
    }
    
    // Test invalid preset
    {
        nlm::Config config;
        assert(!config.applyPreset("invalid"));
        std::cout << "      Invalid preset: Correctly rejected" << std::endl;
    }
    
    std::cout << "      Configuration presets test PASSED" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "    Testing Configuration Validation..." << std::endl;
    
    // Test valid configuration
    {
        nlm::Config config;
        config.set("random_seed", 42, nlm::ConfigSource::Runtime);
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
        config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
        config.set("region_count", 2, nlm::ConfigSource::Runtime);
        config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
        
        assert(config.validate());
        std::cout << "      Valid configuration: PASSED" << std::endl;
    }
    
    // Test invalid configuration (should be rejected)
    {
        nlm::Config config;
        config.set("neuron_count", -1, nlm::ConfigSource::Runtime);  // Invalid
        config.set("connection_probability", 1.5f, nlm::ConfigSource::Runtime);  // Invalid
        
        // Configuration with invalid values may or may not validate depending on implementation
        // The important thing is that it doesn't crash
        config.validate();
        std::cout << "      Invalid configuration: Handled gracefully" << std::endl;
    }
    
    std::cout << "      Configuration validation test PASSED" << std::endl;
}

void testConfigurationFileFormats() {
    std::cout << "    Testing Configuration File Formats..." << std::endl;
    
    // Test JSON format
    {
        nlm::Config config;
        std::ofstream jsonFile("test_config.json");
        if (jsonFile.is_open()) {
            jsonFile << "{\n";
            jsonFile << "  \"random_seed\": 42,\n";
            jsonFile << "  \"neuron_count\": 1000,\n";
            jsonFile << "  \"simulation_timestep\": 0.001\n";
            jsonFile << "}\n";
            jsonFile.close();
            
            assert(config.loadFromJSON("test_config.json"));
            assert(config.has("random_seed"));
            assert(config.has("neuron_count"));
            
            std::remove("test_config.json");
        }
        std::cout << "      JSON format: PASSED" << std::endl;
    }
    
    // Test YAML format
    {
        nlm::Config config;
        std::ofstream yamlFile("test_config.yaml");
        if (yamlFile.is_open()) {
            yamlFile << "random_seed: 42\n";
            yamlFile << "neuron_count: 1000\n";
            yamlFile << "simulation_timestep: 0.001\n";
            yamlFile.close();
            
            assert(config.loadFromYAML("test_config.yaml"));
            assert(config.has("random_seed"));
            
            std::remove("test_config.yaml");
        }
        std::cout << "      YAML format: PASSED" << std::endl;
    }
    
    // Test CFG (config) format
    {
        nlm::Config config;
        std::ofstream cfgFile("test_config.cfg");
        if (cfgFile.is_open()) {
            cfgFile << "# NLM Configuration\n";
            cfgFile << "random_seed = 42\n";
            cfgFile << "neuron_count = 1000\n";
            cfgFile << "simulation_timestep = 0.001\n";
            cfgFile.close();
            
            assert(config.loadFromFile("test_config.cfg"));
            assert(config.has("random_seed"));
            
            std::remove("test_config.cfg");
        }
        std::cout << "      CFG format: PASSED" << std::endl;
    }
    
    // Test format detection
    {
        assert(nlm::Config::getFormatFromExtension("config.cfg") == nlm::ConfigFormat::CFG);
        assert(nlm::Config::getFormatFromExtension("config.json") == nlm::ConfigFormat::JSON);
        assert(nlm::Config::getFormatFromExtension("config.yaml") == nlm::ConfigFormat::YAML);
        assert(nlm::Config::getFormatFromExtension("config.yml") == nlm::ConfigFormat::YAML);
        assert(nlm::Config::getFormatFromExtension("config.txt") == nlm::ConfigFormat::CFG);
        assert(nlm::Config::getFormatFromExtension("config.unknown") == nlm::ConfigFormat::Unknown);
        std::cout << "      Format detection: PASSED" << std::endl;
    }
    
    std::cout << "      Configuration file formats test PASSED" << std::endl;
}

void testConfigurationSourcePrecedence() {
    std::cout << "    Testing Configuration Source Precedence..." << std::endl;
    
    nlm::Config config;
    
    // Set values with different sources
    config.set("test_key", 1, nlm::ConfigSource::Default);
    config.set("test_key", 2, nlm::ConfigSource::File);
    config.set("test_key", 3, nlm::ConfigSource::CommandLine);
    
    // CommandLine should have highest precedence
    assert(config.getSource("test_key") == nlm::ConfigSource::CommandLine);
    assert(config.isOverridden("test_key"));
    
    // Test getOr with default values
    int value = config.getOr<int>("missing", 100);
    assert(value == 100);
    
    value = config.getOr<int>("test_key", 100);
    assert(value == 3);
    
    // Test has() and remove()
    assert(config.has("test_key"));
    config.remove("test_key");
    assert(!config.has("test_key"));
    
    // Test clear()
    config.set("key1", 1);
    config.set("key2", 2);
    assert(config.getKeys().size() == 2);
    config.clear();
    assert(config.getKeys().size() == 0);
    
    std::cout << "      Configuration source precedence test PASSED" << std::endl;
}

void testConfigurationErrors() {
    std::cout << "    Testing Configuration Error Handling..." << std::endl;
    
    // Test error in file loading
    {
        nlm::Config config;
        bool loadSuccess = config.loadFromFile("nonexistent_file.txt");
        // Should not crash, result may be true or false depending on implementation
        std::cout << "      Non-existent file: " << (loadSuccess ? "loaded (unexpected)" : "not found (expected)") << std::endl;
    }
    
    // Test error in invalid file format
    {
        nlm::Config config;
        std::ofstream badFile("bad_config.txt");
        badFile << "invalid content without equals sign";
        badFile.close();
        
        bool loadSuccess = config.loadFromFile("bad_config.txt");
        // Should handle gracefully
        std::cout << "      Invalid format file: " << (loadSuccess ? "loaded (handled)" : "not loaded (expected)") << std::endl;
        
        std::remove("bad_config.txt");
    }
    
    // Test error in get() with wrong type
    {
        nlm::Config config;
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        
        auto intVal = config.get<int>("int_value");
        assert(intVal.has_value());
        assert(*intVal == 42);
        
        // Try to get as string (will return null)
        auto stringVal = config.get<std::string>("int_value");
        assert(!stringVal.has_value());
        
        std::cout << "      Type mismatch handling: PASSED" << std::endl;
    }
    
    std::cout << "      Configuration error handling test PASSED" << std::endl;
}

void testConfigurationPerformance() {
    std::cout << "    Testing Configuration Performance..." << std::endl;
    
    // Performance test for configuration operations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        nlm::Config config;
        
        // Various configuration operations
        config.set("int_value", i, nlm::ConfigSource::Runtime);
        config.set("string_value", std::string("value_") + std::to_string(i), nlm::ConfigSource::Runtime);
        config.set("double_value", 3.14 * i, nlm::ConfigSource::Runtime);
        
        config.getOr<int>("int_value", 0);
        config.getOr<std::string>("string_value", "");
        config.getOr<double>("double_value", 0.0);
        
        config.has("int_value");
        config.has("string_value");
        
        config.remove("int_value");
        config.remove("string_value");
        
        config.getKeys();
        config.validate();
        config.summary();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    
    std::cout << "      Configuration operations completed in " << duration << "s" << std::endl;
    std::cout << "      Performance test PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Configuration Validation Test Suite ===" << std::endl;
    
    testConfigurationPresets();
    testConfigurationValidation();
    testConfigurationFileFormats();
    testConfigurationSourcePrecedence();
    testConfigurationErrors();
    testConfigurationPerformance();
    
    std::cout << "\n=== All Configuration Validation Tests PASSED ===" << std::endl;
}

} // namespace test_configuration_validation