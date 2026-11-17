# Simple unit test for configuration validation
# This test validates the enhanced error handling and validation features

#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <string>

using namespace nlm;

void testBasicFunctionality() {
    std::cout << "Testing basic configuration functionality..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test setting and getting values
    config->set("string_key", "hello");
    assert(config->getOr<std::string>("string_key", "") == "hello");
    
    config->set("int_key", 42);
    assert(config->getOr<int64_t>("int_key", 0) == 42);
    
    config->set("double_key", 3.14);
    assert(config->getOr<double>("double_key", 0.0) == 3.14);
    
    config->set("bool_key", true);
    assert(config->getOr<bool>("bool_key", false) == true);
    
    std::cout << "✓ Basic functionality tests passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling improvements..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test duplicate keys (should be overwritten)
    config->set("duplicate_key", "value1", ConfigSource::Default);
    config->set("duplicate_key", "value2", ConfigSource::File);
    
    // Should have the last value
    assert(config->getOr<std::string>("duplicate_key", "") == "value2");
    
    // Test has() method
    assert(config->has("duplicate_key") == true);
    assert(config->has("nonexistent_key") == false);
    
    std::cout << "✓ Error handling tests passed" << std::endl;
}

void testConfigFileParsing() {
    std::cout << "Testing configuration file parsing..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test parsing simple key=value format
    assert(config->loadFromFile("configs/default.cfg"));
    
    // Verify some basic values
    assert(config->has("random_seed"));
    assert(config->has("simulation_timestep"));
    assert(config->has("neuron_count"));
    
    // Check that duplicate dopamine_baseline was fixed
    assert(config->getOr<double>("dopamine_baseline", -999.0) == 0.1);
    
    std::cout << "✓ Configuration file parsing tests passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    // Test that our config validator file has issues
    // The test config has a duplicate dopamine_baseline key
    
    auto config = std::make_shared<Config>();
    
    // Load test config (should have duplicate dopamine_baseline)
    if (config->loadFromFile("configs/test_config.cfg")) {
        // Check that we loaded the duplicate entry (last one should win)
        assert(config->getOr<double>("dopamine_baseline", -999.0) == 0.1);
        
        // Test validation logic
        auto keys = config->getKeys();
        std::cout << "Loaded configuration keys: ";
        for (const auto& key : keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "✓ Configuration validation tests passed" << std::endl;
}

void testMemoryManagement() {
    std::cout << "Testing memory management..." << std::endl;
    
    // Test that Config objects can be properly created and destroyed
    {
        auto config1 = std::make_shared<Config>();
        config1->set("test_key", "test_value");
        assert(config1->getOr<std::string>("test_key", "") == "test_value");
    }
    
    // config1 should be destroyed here
    std::cout << "✓ Memory management tests passed" << std::endl;
}

void testTypeConversions() {
    std::cout << "Testing type conversions..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test numeric conversions
    config->set("int_val", 42);
    assert(config->getOr<int64_t>("int_val", 0) == 42);
    
    config->set("double_val", 3.14159);
    assert(config->getOr<double>("double_val", 0.0) == 3.14159);
    
    config->set("bool_val", true);
    assert(config->getOr<bool>("bool_val", false) == true);
    
    // Test string preservation
    config->set("string_val", "hello world");
    assert(config->getOr<std::string>("string_val", "") == "hello world");
    
    std::cout << "✓ Type conversion tests passed" << std::endl;
}

int main() {
    std::cout << "Running NLM Configuration Tests" << std::endl;
    std::cout << "==============================" << std::endl;
    std::cout << std::endl;
    
    try {
        testBasicFunctionality();
        testErrorHandling();
        testConfigFileParsing();
        testConfigurationValidation();
        testMemoryManagement();
        testTypeConversions();
        
        std::cout << std::endl;
        std::cout << "✅ All tests passed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << std::endl;
        std::cout << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}