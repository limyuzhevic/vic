#include "tests/TestEnhancedConfig.h"
#include "src/core/Config/EnhancedConfig.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

void testEnhancedConfig_jsonLoading() {
    std::cout << "Testing EnhancedConfig JSON loading..." << std::endl;
    
    EnhancedConfig config;
    
    // Test simple JSON
    std::string jsonStr = R"({
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "enable_plasticity": true,
        "simulation_timestep": 0.001
    })";
    
    assert(config.loadFromJson(jsonStr));
    
    // Verify loaded values
    assert(config.getOr<int64_t>("neuron_count") == 100);
    assert(config.getOr<int64_t>("region_count") == 1);
    assert(config.getOr<float>("connection_probability") == 0.15f);
    assert(config.getOr<float>("stdp_ltp_weight") == 0.02f);
    assert(config.getOr<float>("stdp_ltd_weight") == 0.015f);
    assert(config.getOr<bool>("enable_plasticity") == true);
    assert(config.getOr<double>("simulation_timestep") == 0.001);
    
    // Test non-existent key
    assert(config.getOr<int64_t>("nonexistent", 999) == 999);
    
    std::cout << "  ✓ JSON loading tests passed" << std::endl;
}

void testEnhancedConfig_jsonSerialization() {
    std::cout << "Testing EnhancedConfig JSON serialization..." << std::endl;
    
    EnhancedConfig config;
    config.set("test_string", std::string("hello"), ConfigSource::Default);
    config.set("test_int", 42, ConfigSource::Default);
    config.set("test_float", 3.14, ConfigSource::Default);
    config.set("test_bool", true, ConfigSource::Default);
    
    std::string json = EnhancedConfig::toJson(config);
    assert(!json.empty());
    assert(json.find("test_string") != std::string::npos);
    assert(json.find("test_int") != std::string::npos);
    assert(json.find("test_float") != std::string::npos);
    assert(json.find("test_bool") != std::string::npos);
    
    // Test round-trip
    EnhancedConfig config2;
    assert(config2.loadFromJson(json));
    assert(config2.getOr<std::string>("test_string") == "hello");
    assert(config2.getOr<int>("test_int") == 42);
    assert(config2.getOr<double>("test_float") == 3.14);
    assert(config2.getOr<bool>("test_bool") == true);
    
    std::cout << "  ✓ JSON serialization tests passed" << std::endl;
}

void testEnhancedConfig_yamlLoading() {
    std::cout << "Testing EnhancedConfig YAML loading..." << std::endl;
    
    EnhancedConfig config;
    
    // Simple YAML (similar to JSON for this test)
    std::string yamlStr = R"(
        neuron_count: 200
        region_count: 2
        connection_probability: 0.2
        enable_plasticity: true
    )";
    
    assert(config.loadFromYaml(yamlStr));
    
    // Verify loaded values
    assert(config.getOr<int64_t>("neuron_count") == 200);
    assert(config.getOr<int64_t>("region_count") == 2);
    assert(config.getOr<float>("connection_probability") == 0.2f);
    assert(config.getOr<bool>("enable_plasticity") == true);
    
    std::cout << "  ✓ YAML loading tests passed" << std::endl;
}

void testEnhancedConfig_validation() {
    std::cout << "Testing EnhancedConfig validation..." << std::endl;
    
    EnhancedConfig config;
    config.set("required_string", std::string("value"), ConfigSource::Default);
    config.set("required_int", 123, ConfigSource::Default);
    
    // Test valid schema
    std::string validSchema = R"({
        "required_string": "string",
        "required_int": "integer"
    })";
    
    assert(config.validate(validSchema));
    assert(config.getValidationErrors().empty());
    
    // Test invalid schema (missing key)
    std::string invalidSchema = R"({
        "required_string": "string",
        "missing_key": "integer"
    })";
    
    assert(!config.validate(invalidSchema));
    assert(!config.getValidationErrors().empty());
    
    std::cout << "  ✓ Validation tests passed" << std::endl;
}

void testEnhancedConfig_merging() {
    std::cout << "Testing EnhancedConfig merging..." << std::endl;
    
    EnhancedConfig config1;
    config1.set("key1", std::string("value1"), ConfigSource::Default);
    config1.set("key2", 100, ConfigSource::CommandLine);
    
    EnhancedConfig config2;
    config2.set("key2", std::string("value2"), ConfigSource::File); // Should override
    config2.set("key3", 200, ConfigSource::Default);
    
    config1.merge(config2);
    
    // key1 should remain from config1
    assert(config1.getOr<std::string>("key1") == "value1");
    
    // key2 should come from config2 (higher priority - File > CommandLine)
    // Note: ConfigSource priority may vary based on implementation
    assert(config1.getOr<std::string>("key2") == "value2");
    
    // key3 should be added
    assert(config1.getOr<int>("key3") == 200);
    
    std::cout << "  ✓ Merging tests passed" << std::endl;
}

void testEnhancedConfig_patternMatching() {
    std::cout << "Testing EnhancedConfig pattern matching..." << std::endl;
    
    EnhancedConfig config;
    config.set("neural.count", 100, ConfigSource::Default);
    config.set("neural.region", 1, ConfigSource::Default);
    config.set("plasticity.enabled", true, ConfigSource::Default);
    config.set("plasticity.stdp_weight", 0.02, ConfigSource::Default);
    config.set("simulation.timestep", 0.001, ConfigSource::Default);
    
    // Test wildcard patterns
    assert(config.matchesPattern("*")); // Matches any key
    assert(config.matchesPattern("neural*")); // Matches keys starting with neural
    assert(config.matchesPattern("*count")); // Matches keys ending with count
    assert(config.matchesPattern("?eural.count")); // Matches single char pattern
    assert(config.matchesPattern("neural.*")); // Matches keys with dot in middle
    
    // Test prefix matching
    std::vector<std::string> neuralKeys = config.getKeysStartingWith("neural.");
    assert(neuralKeys.size() >= 2);
    assert(std::find(neuralKeys.begin(), neuralKeys.end(), "neural.count") != neuralKeys.end());
    
    std::cout << "  ✓ Pattern matching tests passed" << std::endl;
}

int main() {
    std::cout << "=== EnhancedConfig Unit Tests ===" << std::endl << std::endl;
    
    try {
        testEnhancedConfig_jsonLoading();
        std::cout << std::endl;
        
        testEnhancedConfig_jsonSerialization();
        std::cout << std::endl;
        
        testEnhancedConfig_yamlLoading();
        std::cout << std::endl;
        
        testEnhancedConfig_validation();
        std::cout << std::endl;
        
        testEnhancedConfig_merging();
        std::cout << std::endl;
        
        testEnhancedConfig_patternMatching();
        std::cout << std::endl;
        
        std::cout << "=== All EnhancedConfig tests passed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}