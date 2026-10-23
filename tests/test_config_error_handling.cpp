// Test Config validation and error handling
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_config_error_handling {

void testConfigKeyNotFoundException() {
    nlm::Config config;
    
    try {
        // This should throw ConfigKeyNotFoundException
        auto val = config.get<int>("nonexistent_key");
        assert(false && "Should have thrown ConfigKeyNotFoundException");
    } catch (const nlm::ConfigKeyNotFoundException& e) {
        std::cout << "    testConfigKeyNotFoundException passed" << std::endl;
    }
}

void testConfigInvalidValueException() {
    nlm::Config config;
    
    try {
        // Set a string value where an int is expected
        config.set("test_key", std::string("not_an_int"));
        // Try to get it as an int - this should throw
        auto val = config.get<int>("test_key");
        assert(false && "Should have thrown ConfigInvalidValueException");
    } catch (const nlm::ConfigInvalidValueException& e) {
        std::cout << "    testConfigInvalidValueException passed" << std::endl;
    }
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test valid values
    config.set("neuron_count", 100);
    config.set("region_count", 2);
    config.set("connection_probability", 0.15);
    config.set("simulation_timestep", 0.001);
    
    assert(config.validateAll(), "All valid values should pass validation");
    
    // Test invalid values
    config.set("neuron_count", 0);  // Too small
    assert(!config.validateAll(), "Invalid neuron_count should fail validation");
    
    config.set("neuron_count", 100);  // Fix it
    config.set("connection_probability", 1.5);  // Too large
    assert(!config.validateAll(), "Invalid connection_probability should fail validation");
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSaveLoadWithValues() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_value", 42);
    config.set("double_value", 3.14159);
    config.set("string_value", "hello world");
    config.set("bool_value", true);
    config.set("vector_int", std::vector<int>{1, 2, 3});
    
    // Save to temporary file
    std::string tempFile = "/tmp/test_config_save_load.cfg";
    
    try {
        // Save config
        assert(config.saveToFile(tempFile), "Config save should succeed");
        
        // Create new config and load
        nlm::Config config2;
        assert(config2.loadFromFile(tempFile), "Config load should succeed");
        
        // Verify loaded values
        assert(config2.get<int>("int_value").value() == 42, "Loaded int value mismatch");
        assert(config2.get<double>("double_value").value() == 3.14159, "Loaded double value mismatch");
        assert(config2.get<std::string>("string_value").value() == "hello world", "Loaded string value mismatch");
        assert(config2.get<bool>("bool_value").value() == true, "Loaded bool value mismatch");
        
        std::cout << "    testConfigSaveLoadWithValues passed" << std::endl;
    } catch (const nlm::ConfigException& e) {
        std::cout << "    Config exception in testConfigSaveLoadWithValues: " << e.what() << std::endl;
        throw;
    }
}

void testConfigFileErrorException() {
    nlm::Config config;
    
    try {
        // Try to save to an invalid path
        assert(!config.saveToFile("/invalid/path/that/does/not/exist/config.cfg"), 
               "Save to invalid path should fail");
        assert(false && "Should have thrown ConfigFileErrorException");
    } catch (const nlm::ConfigFileErrorException& e) {
        std::cout << "    testConfigFileErrorException passed" << std::endl;
    }
}

void testConfigValidationException() {
    nlm::Config config;
    
    // Set an invalid region count
    config.set("region_count", 0);
    
    try {
        config.validate("region_count");
        assert(false && "Should have thrown ConfigValidationException");
    } catch (const nlm::ConfigValidationException& e) {
        std::cout << "    testConfigValidationException passed" << std::endl;
    }
}

void runAll() {
    testConfigKeyNotFoundException();
    testConfigInvalidValueException();
    testConfigValidation();
    testConfigSaveLoadWithValues();
    testConfigFileErrorException();
    testConfigValidationException();
}

} // namespace test_config_error_handling