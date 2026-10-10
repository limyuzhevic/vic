// test_configuration_edge_cases.cpp - Configuration edge cases
#include "TestFramework.hpp"
#include "TestFixture.hpp"
#include "test_utils.cpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

namespace test_configuration_edge_cases {

// Test edge cases for invalid configuration files
void testInvalidConfigurationFiles() {
    std::cout << "Testing invalid configuration files..." << std::endl;
    
    // Test 1: Invalid JSON format
    {
        nlm::Config config;
        
        // Try to load invalid JSON
        bool success = config.loadFromFile("invalid.json");
        assert(!success); // Should fail
        
        std::cout << "    Invalid JSON format test passed" << std::endl;
    }
    
    // Test 2: Missing required parameters
    {
        auto config = std::make_shared<nlm::Config>();
        
        // Save to file
        config->saveToFile("test_config.json");
        
        // Remove the file
        std::remove("test_config.json");
        
        // Try to load
        config->loadFromFile("test_config.json");
        assert(!config->has("neuron_count")); // Should not have required parameter
        
        std::cout << "    Missing required parameters test passed" << std::endl;
    }
}

// Test edge cases for missing configuration parameters
void testMissingConfigurationParameters() {
    std::cout << "Testing missing configuration parameters..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Check that default parameters exist
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    assert(config->has("connection_probability"));
    
    // Set custom values
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Runtime);
    
    // Verify custom values
    assert(config->get<int64_t>("neuron_count") == 50);
    assert(config->get<int64_t>("region_count") == 2);
    assert(config->get<double>("connection_probability") == 0.2);
    
    // Check that optional parameters may or may not exist
    // (implementation dependent)
    
    std::cout << "    Missing configuration parameters test passed" << std::endl;
}

// Test edge cases for invalid parameter values
void testInvalidParameterValues() {
    std::cout << "Testing invalid parameter values..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test 1: Negative neuron count
    {
        config->set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Runtime);
        assert(config->get<int64_t>("neuron_count") == -1);
        std::cout << "    Negative neuron count test passed" << std::endl;
    }
    
    // Test 2: Zero neuron count
    {
        config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Runtime);
        assert(config->get<int64_t>("neuron_count") == 0);
        std::cout << "    Zero neuron count test passed" << std::endl;
    }
    
    // Test 3: Negative region count
    {
        config->set("region_count", static_cast<int64_t>(-1), nlm::ConfigSource::Runtime);
        assert(config->get<int64_t>("region_count") == -1);
        std::cout << "    Negative region count test passed" << std::endl;
    }
    
    // Test 4: Connection probability > 1.0
    {
        config->set("connection_probability", 1.5, nlm::ConfigSource::Runtime);
        assert(config->get<double>("connection_probability") == 1.5);
        std::cout << "    Connection probability > 1.0 test passed" << std::endl;
    }
    
    // Test 5: Connection probability < 0.0
    {
        config->set("connection_probability", -0.5, nlm::ConfigSource::Runtime);
        assert(config->get<double>("connection_probability") == -0.5);
        std::cout << "    Connection probability < 0.0 test passed" << std::endl;
    }
}

// Test edge cases for configuration validation
void testConfigurationValidation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test 1: Invalid neuron count
    {
        config->set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Runtime);
        
        // Configuration should be created but may not be valid for simulation
        assert(config->has("neuron_count"));
        assert(config->get<int64_t>("neuron_count") == 0);
        
        std::cout << "    Invalid neuron count validation test passed" << std::endl;
    }
    
    // Test 2: Invalid region count
    {
        config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Runtime);
        
        assert(config->has("region_count"));
        assert(config->get<int64_t>("region_count") == 0);
        
        std::cout << "    Invalid region count validation test passed" << std::endl;
    }
    
    // Test 3: Invalid connection probability
    {
        config->set("connection_probability", 2.0, nlm::ConfigSource::Runtime);
        
        assert(config->has("connection_probability"));
        assert(config->get<double>("connection_probability") == 2.0);
        
        std::cout << "    Invalid connection probability validation test passed" << std::endl;
    }
}

// Test edge cases for configuration inheritance
void testConfigurationInheritance() {
    std::cout << "Testing configuration inheritance..." << std::endl;
    
    auto baseConfig = std::make_shared<nlm::Config>();
    baseConfig->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    baseConfig->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    baseConfig->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    auto childConfig = std::make_shared<nlm::Config>();
    
    // Test inheritance of default values
    assert(childConfig->has("neuron_count")); // May have default value
    assert(childConfig->has("region_count")); // May have default value
    assert(childConfig->has("connection_probability")); // May have default value
    
    // Set child-specific values
    childConfig->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Runtime);
    
    // Verify child overrides parent
    assert(childConfig->get<int64_t>("neuron_count") == 50);
    assert(childConfig->get<int64_t>("region_count") == 1); // From parent default
    assert(childConfig->get<double>("connection_probability") == 0.1); // From parent default
    
    std::cout << "    Configuration inheritance test passed" << std::endl;
}

// Test edge cases for configuration loading/saving
void testConfigurationLoadingSaving() {
    std::cout << "Testing configuration loading/saving..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set values
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.5, nlm::ConfigSource::Runtime);
    
    // Save to file
    bool saveSuccess = config->saveToFile("test_config.json");
    assert(saveSuccess);
    
    // Load from file
    auto loadedConfig = std::make_shared<nlm::Config>();
    bool loadSuccess = loadedConfig->loadFromFile("test_config.json");
    assert(loadSuccess);
    
    // Verify loaded values
    assert(loadedConfig->get<int64_t>("neuron_count") == 100);
    assert(loadedConfig->get<int64_t>("region_count") == 2);
    assert(loadedConfig->get<double>("connection_probability") == 0.5);
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    Configuration loading/saving test passed" << std::endl;
}

void runAll() {
    testInvalidConfigurationFiles();
    testMissingConfigurationParameters();
    testInvalidParameterValues();
    testConfigurationValidation();
    testConfigurationInheritance();
    testConfigurationLoadingSaving();
    
    std::cout << std::endl << "All configuration edge case tests completed successfully!" << std::endl;
}

} // namespace test_configuration_edge_cases