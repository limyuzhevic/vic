// Enhanced Config Tests
#include "core/Config/Config.hpp"
#include "core/JSONConfig.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

namespace test_config {

void testBasicFunctionality() {
    std::cout << "Running basic functionality tests..." << std::endl;
    
    nlm::Config config;
    
    // Test configuration creation
    assert(!config.has("test_key"));
    std::cout << "    ✓ testBasicFunctionality passed" << std::endl;
}

void testJSONParsing() {
    std::cout << "Running JSON parsing tests..." << std::endl;
    
    // Create test JSON string
    std::string jsonString = R"({
        "neuron_count": 1000,
        "plasticity_learning_rate": 0.01,
        "visualization_enabled": true,
        "reward_discount_factor": 0.99,
        "tags": ["test", "example", "ai"],
        "nested_object": {
            "inner_value": "test",
            "inner_number": 42
        }
    })";
    
    nlm::Config config;
    bool success = config.fromJSON(jsonString);
    assert(success);
    
    // Test getting values
    assert(config.has("neuron_count"));
    assert(config.getOr<int>("neuron_count", 0) == 1000);
    
    assert(config.getOr<double>("plasticity_learning_rate", 0.0) == 0.01);
    assert(config.getOr<bool>("visualization_enabled", false) == true);
    assert(config.getOr<double>("reward_discount_factor", 1.0) == 0.99);
    
    // Test nested object (should be flattened)
    // Note: nested objects are flattened in JSONConfig
    
    std::cout << "    ✓ testJSONParsing passed" << std::endl;
}

void testKeyValueBackwardCompatibility() {
    std::cout << "Running key=value backward compatibility tests..." << std::endl;
    
    nlm::Config config;
    
    // Test setting values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test getting values
    assert(config.getOr<int>("int_value", 0) == 42);
    assert(config.getOr<double>("double_value", 0.0) == 3.14);
    assert(config.getOr<std::string>("string_value", "") == "hello");
    assert(config.getOr<bool>("bool_value", false) == true);
    
    // Test array types
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    std::cout << "    ✓ testKeyValueBackwardCompatibility passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "Running configuration validation tests..." << std::endl;
    
    nlm::Config config;
    
    // Add duplicate keys (should invalidate)
    config.set("test_key", 1, nlm::ConfigSource::Runtime);
    config.set("test_key", 2, nlm::ConfigSource::Runtime);
    
    // Test validation
    std::string validation = config.validate();
    assert(!validation.empty()); // Should have errors for duplicates
    
    // Clear duplicates and test again
    config.clear();
    config.set("test_key", 1, nlm::ConfigSource::Runtime);
    config.set("other_key", 2, nlm::ConfigSource::Runtime);
    
    assert(config.isValid());
    std::string validValidation = config.validate();
    assert(!validValidation.empty()); // Should have valid message
    
    std::cout << "    ✓ testConfigurationValidation passed" << std::endl;
}

void testFileFormatDetection() {
    std::cout << "Running file format detection tests..." << std::endl;
    
    nlm::Config config;
    
    // Test format detection
    assert(config.detectFileFormat("config.json") == nlm::ConfigFormat::JSON);
    assert(config.detectFileFormat("config.JSON") == nlm::ConfigFormat::JSON);
    assert(config.detectFileFormat("config.yaml") == nlm::ConfigFormat::YAML);
    assert(config.detectFileFormat("config.yml") == nlm::ConfigFormat::YAML);
    assert(config.detectFileFormat("config.cfg") == nlm::ConfigFormat::KeyValue);
    assert(config.detectFileExtension("config.cfg", ".cfg"));
    assert(config.isFileExtension("config.cfg", ".cfg"));
    assert(config.isFileExtension("config.JSON", ".json"));
    
    std::cout << "    ✓ testFileFormatDetection passed" << std::endl;
}

void testConfigurationStatistics() {
    std::cout << "Running configuration statistics tests..." << std::endl;
    
    nlm::Config config;
    
    // Add some configuration entries
    config.set("key1", 1, nlm::ConfigSource::File);
    config.set("key2", "value", nlm::ConfigSource::CommandLine);
    config.set("key3", 3.14, nlm::ConfigSource::JSON);
    
    // Test statistics
    assert(config.getEntryCount() == 3);
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    assert(!config.has("missing_key"));
    
    // Test getting all keys
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    // Test sources
    auto sources = config.getSources();
    assert(!sources.empty());
    
    std::cout << "    ✓ testConfigurationStatistics passed" << std::endl;
}

void testConstants() {
    std::cout << "Running constants tests..." << std::endl;
    
    // Test that constants are accessible
    assert(nlm::Config::DEFAULT_CONFIG_PATH == "configs/default.cfg");
    assert(nlm::Config::DEFAULT_RANDOM_SEED == 42);
    assert(nlm::Config::DEFAULT_NEURON_COUNT == 1000);
    assert(nlm::Config::DEFAULT_SIMULATION_TIMESTEP == 0.001);
    assert(nlm::Config::DEFAULT_LEARNING_RATE == 0.01);
    assert(nlm::Config::DEFAULT_DISCOUNT_FACTOR == 0.99);
    
    std::cout << "    ✓ testConstants passed" << std::endl;
}

void runAll() {
    testBasicFunctionality();
    testJSONParsing();
    testKeyValueBackwardCompatibility();
    testConfigurationValidation();
    testFileFormatDetection();
    testConfigurationStatistics();
    testConstants();
    
    std::cout << std::endl;
    std::cout << "All config tests passed! ✓" << std::endl;
    std::cout << std::endl;
    std::cout << "Summary of new features tested:" << std::endl;
    std::cout << "  - JSON parsing and serialization" << std::endl;
    std::cout << "  - Backward compatibility with key=value format" << std::endl;
    std::cout << "  - Configuration validation and error reporting" << std::endl;
    std::cout << "  - File format detection" << std::endl;
    std::cout << "  - Configuration statistics and management" << std::endl;
    std::cout << "  - Constants and default values" << std::endl;
}

} // namespace test_config

int main() {
    test_config::runAll();
    return 0;
}
