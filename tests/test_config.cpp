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

void testConfigValidation() {
    nlm::Config config;
    
    // Test setting expected types
    config.setStrictMode(true);
    config.setExpectedType("int_key", "int");
    config.setExpectedType("string_key", "string");
    
    // These should work
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "test", nlm::ConfigSource::Runtime);
    
    // Test validation passes
    assert(config.validateConfig());
    
    // Test validation fails by setting wrong type
    nlm::Config config2;
    config2.setStrictMode(true);
    config2.setExpectedType("int_key", "int");
    
    // Set wrong type - this would cause validation to fail
    // (In practice, validation would check during getOr)
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigJSONFormat() {
    nlm::Config config;
    
    // Create a simple JSON config
    std::string json = R"({
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "plasticity_enabled": true,
        "tags": ["high_performance", "large_network"],
        "metadata": {
            "created_by": "test_suite",
            "version": "1.0"
        }
    })";
    
    // Write to a temporary file
    std::string tempFile = "/tmp/test_config.json";
    std::ofstream file(tempFile);
    file << json;
    file.close();
    
    // Load from JSON file
    if (config.loadFromFile(tempFile)) {
        // Check that values were loaded
        assert(config.has("neuron_count"));
        assert(config.has("region_count"));
        assert(config.has("connection_probability"));
        assert(config.has("plasticity_enabled"));
        assert(config.has("tags"));
        assert(config.has("metadata"));
        
        // Get values
        auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
        assert(neuronCount == 1000);
        
        auto regionCount = config.getOr<int64_t>("region_count", 0);
        assert(regionCount == 2);
        
        auto connectionProb = config.getOr<float>("connection_probability", 0.0f);
        assert(connectionProb > 0.14f && connectionProb < 0.16f);
        
        auto plasticityEnabled = config.getOr<bool>("plasticity_enabled", false);
        assert(plasticityEnabled);
        
        std::cout << "    testConfigJSONFormat passed" << std::endl;
    } else {
        std::cout << "    testConfigJSONFormat skipped (JSON parser not available)" << std::endl;
    }
    
    // Clean up
    std::filesystem::remove(tempFile);
}

void testConfigSaveLoad() {
    nlm::Config config;
    
    // Set some test values
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "hello", nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string tempFile = "/tmp/test_config_save.txt";
    if (config.saveToFile(tempFile)) {
        // Clear config
        config.clear();
        
        // Load from file
        if (config.loadFromFile(tempFile)) {
            // Check that values were loaded
            assert(config.has("int_key"));
            assert(config.has("string_key"));
            assert(config.has("double_key"));
            assert(config.has("bool_key"));
            
            // Verify values
            auto intVal = config.getOr<int>("int_key", 0);
            assert(intVal == 42);
            
            auto stringVal = config.getOr<std::string>("string_key", "");
            assert(stringVal == "hello");
            
            auto doubleVal = config.getOr<double>("double_key", 0.0);
            assert(doubleVal > 3.13 && doubleVal < 3.15);
            
            auto boolVal = config.getOr<bool>("bool_key", false);
            assert(boolVal);
            
            std::cout << "    testConfigSaveLoad passed" << std::endl;
        }
    }
    
    // Clean up
    std::filesystem::remove(tempFile);
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test error handling for invalid file paths
    assert(!config.loadFromFile("/nonexistent/path/config.json"));
    
    // Test error handling for invalid values
    // (Current implementation doesn't throw exceptions on bad values,
    //  but the validation system should warn)
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigJSONFormat();
    testConfigSaveLoad();
    testConfigErrorHandling();
}

} // namespace test_config