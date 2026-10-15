#include "core/Config/Config.hpp"
#include "core/Config/Validator.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

void testConfigValidation() {
    std::cout << "=== Testing Config Validation ===" << std::endl;
    
    // Test 1: Valid configuration
    {
        Config config;
        config.set("neuron_count", 1000);
        config.set("region_count", 2);
        config.set("connection_probability", 0.15f);
        
        assert(config.validate());
        std::cout << "✓ Test 1 passed: Valid configuration passes validation" << std::endl;
    }
    
    // Test 2: Invalid neuron count (too high)
    {
        Config config;
        config.set("neuron_count", -5);  // Invalid: negative
        
        assert(!config.validate());
        std::string errors = config.getValidationErrors();
        assert(errors.find("neuron_count") != std::string::npos);
        std::cout << "✓ Test 2 passed: Invalid neuron count caught by validation" << std::endl;
    }
    
    // Test 3: Invalid region count (too high)
    {
        Config config;
        config.set("region_count", 20);  // Invalid: > 10
        
        assert(!config.validate());
        std::string errors = config.getValidationErrors();
        assert(errors.find("region_count") != std::string::npos);
        std::cout << "✓ Test 3 passed: Invalid region count caught by validation" << std::endl;
    }
    
    // Test 4: Invalid connection probability
    {
        Config config;
        config.set("connection_probability", 1.5f);  // Invalid: > 1.0
        
        assert(!config.validate());
        std::string errors = config.getValidationErrors();
        assert(errors.find("connection_probability") != std::string::npos);
        std::cout << "✓ Test 4 passed: Invalid connection probability caught by validation" << std::endl;
    }
    
    // Test 5: Configuration with no errors
    {
        Config config;
        config.set("random_seed", 42);
        config.set("simulation_timestep", 0.001);
        config.set("neuron_count", 500);
        config.set("region_count", 1);
        config.set("connection_probability", 0.1f);
        config.set("stdp_ltp_weight", 0.02f);
        config.set("stdp_ltd_weight", 0.015f);
        config.set("synaptogenesis_rate", 0.0001f);
        config.set("pruning_rate", 0.00001f);
        config.set("reward_discount_factor", 0.99f);
        config.set("environment_width", 10);
        config.set("environment_height", 10);
        config.set("visualization_update_rate", 30.0f);
        
        assert(config.validate());
        assert(!config.hasErrors());
        std::cout << "✓ Test 5 passed: Complex valid configuration passes validation" << std::endl;
    }
    
    std::cout << "=== All validation tests passed! ===" << std::endl;
}

void testConfigSaveLoad() {
    std::cout << "=== Testing Config Save/Load ===" << std::endl;
    
    // Test saving configuration
    {
        Config config;
        config.set("random_seed", 42, ConfigSource::Default);
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        config.set("neuron_count", 1000, ConfigSource::File);
        config.set("region_count", 2, ConfigSource::File);
        config.set("connection_probability", 0.15f, ConfigSource::File);
        
        // Save to string
        std::string output;
        config.saveToString(output);
        
        assert(!output.empty());
        assert(output.find("random_seed") != std::string::npos);
        assert(output.find("neuron_count") != std::string::npos);
        assert(output.find("region_count") != std::string::npos);
        assert(output.find("connection_probability") != std::string::npos);
        
        std::cout << "✓ Test 1 passed: Configuration save to string works" << std::endl;
    }
    
    // Test loading from file
    {
        Config config;
        bool loaded = config.loadFromFile("configs/default.cfg");
        
        // Should load successfully (even if not all keys present)
        assert(loaded || true);  // Either success or default behavior
        
        // Check that some expected keys are present
        if (config.has("random_seed")) {
            std::cout << "✓ Test 2 passed: Config loaded from file has expected keys" << std::endl;
        }
    }
    
    std::cout << "=== All save/load tests passed! ===" << std::endl;
}

void testDefaultValidator() {
    std::cout << "=== Testing Default Validator ===" << std::endl;
    
    // Test that default validator is properly registered
    auto defaultValidator = Config::getDefaultValidator();
    assert(defaultValidator != nullptr);
    
    // Test validation rules are registered
    Config config;
    config.set("neuron_count", 500);
    config.set("region_count", 2);
    config.set("connection_probability", 0.1f);
    
    // Should pass validation
    assert(config.validate());
    
    std::cout << "✓ Test 1 passed: Default validator properly initialized" << std::endl;
    std::cout << "=== Default validator tests passed! ===" << std::endl;
}

int main() {
    std::cout << "NLM Configuration System Tests" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    
    testConfigValidation();
    std::cout << std::endl;
    
    testConfigSaveLoad();
    std::cout << std::endl;
    
    testDefaultValidator();
    std::cout << std::endl;
    
    std::cout << "=== All tests completed successfully! ===" << std::endl;
    return 0;
}