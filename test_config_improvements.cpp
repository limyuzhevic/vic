// Test program to verify the improved NLM configuration system and error handling

#include "core/Config/Config.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/Constants/Constants.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

void testConfigBasic() {
    std::cout << "Testing basic configuration..." << std::endl;
    
    Config config;
    
    // Test setting and getting string values
    config.set("test_string", "hello");
    assert(config.getString("test_string").value() == "hello");
    
    // Test setting and getting integer values
    config.set("test_int", static_cast<int64_t>(42));
    assert(config.getInt("test_int").value() == 42);
    
    // Test setting and getting double values
    config.set("test_double", 3.14);
    assert(config.getDouble("test_double").value() == 3.14);
    
    // Test setting and getting boolean values
    config.set("test_bool", true);
    assert(config.getBool("test_bool").value() == true);
    
    // Test default values
    assert(config.getStringOr("nonexistent", "default") == "default");
    assert(config.getIntOr("nonexistent", 99) == 99);
    assert(config.getDoubleOr("nonexistent", 2.71) == 2.71);
    assert(config.getBoolOr("nonexistent", false) == false);
    
    std::cout << "✓ Basic configuration tests passed" << std::endl;
}

void testConfigVectorValues() {
    std::cout << "Testing vector configuration values..." << std::endl;
    
    Config config;
    
    // Test vector of integers
    std::vector<int64_t> intVec = {1, 2, 3, 4, 5};
    config.set("int_vector", intVec);
    assert(config.getVectorInt("int_vector").value() == intVec);
    
    // Test vector of doubles
    std::vector<double> doubleVec = {1.1, 2.2, 3.3};
    config.set("double_vector", doubleVec);
    assert(config.getVectorDouble("double_vector").value() == doubleVec);
    
    // Test vector of strings
    std::vector<std::string> stringVec = {"a", "b", "c"};
    config.set("string_vector", stringVec);
    assert(config.getVectorString("string_vector").value() == stringVec);
    
    std::cout << "✓ Vector configuration tests passed" << std::endl;
}

void testConfigValidation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    Config config;
    
    // Valid configuration
    config.set("valid_key", "valid_value");
    assert(config.validate());
    
    // Invalid key (empty)
    config.set("", "value");
    assert(!config.validate());
    
    // Clear and test
    config.clear();
    config.set("valid_key", "valid_value");
    assert(config.validate());
    
    std::cout << "✓ Configuration validation tests passed" << std::endl;
}

void testConfigStats() {
    std::cout << "Testing configuration statistics..." << std::endl;
    
    Config config;
    
    // Check empty stats
    auto stats = config.getStats();
    assert(stats.totalEntries == 0);
    
    // Add some entries
    config.set("string_key", "value");
    config.set("int_key", static_cast<int64_t>(42));
    config.set("double_key", 3.14);
    config.set("bool_key", true);
    config.set("vector_key", std::vector<int64_t>({1, 2, 3}));
    
    stats = config.getStats();
    assert(stats.totalEntries == 5);
    assert(stats.stringEntries == 1);
    assert(stats.intEntries == 1);
    assert(stats.doubleEntries == 1);
    assert(stats.boolEntries == 1);
    assert(stats.vectorEntries == 1);
    
    std::cout << "✓ Configuration statistics tests passed" << std::endl;
}

void testConstants() {
    std::cout << "Testing configuration constants..." << std::endl;
    
    // Test that constants are properly defined
    assert(ConfigConstants::DEFAULT_SIMULATION_TIMESTEP > 0.0);
    assert(ConfigConstants::DEFAULT_NEURON_COUNT > 0);
    assert(ConfigConstants::DEFAULT_REGION_COUNT > 0);
    assert(ConfigConstants::DEFAULT_CONNECTION_PROBABILITY > 0.0 && ConfigConstants::DEFAULT_CONNECTION_PROBABILITY <= 1.0);
    assert(ConfigConstants::DEFAULT_STDP_LTP_WEIGHT > 0.0);
    assert(ConfigConstants::DEFAULT_STDP_LTD_WEIGHT > 0.0);
    assert(ConfigConstants::DEFAULT_STDP_TAU > 0.0);
    assert(ConfigConstants::DEFAULT_SYNAPTOGENESIS_RATE > 0.0);
    assert(ConfigConstants::DEFAULT_PRUNING_RATE > 0.0);
    
    std::cout << "✓ Constants tests passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    try {
        // Test ConfigError
        throw ConfigError("Test config error");
        assert(false && "Should have thrown ConfigError");
    } catch (const ConfigError& e) {
        assert(std::string(e.what()).find("Config Error") != std::string::npos);
    }
    
    try {
        // Test ConfigValidationError
        throw ConfigValidationError("Test validation error");
        assert(false && "Should have thrown ConfigValidationError");
    } catch (const ConfigValidationError& e) {
        assert(std::string(e.what()).find("Validation Error") != std::string::npos);
    }
    
    try {
        // Test BrainError
        throw BrainError("Test brain error");
        assert(false && "Should have thrown BrainError");
    } catch (const BrainError& e) {
        assert(std::string(e.what()).find("Brain Error") != std::string::npos);
    }
    
    std::cout << "✓ Error handling tests passed" << std::endl;
}

void testValidationHelper() {
    std::cout << "Testing validation helper functions..." << std::endl;
    
    // Test key validation
    assert(ValidationHelper::isValidKey("valid_key_123"));
    assert(!ValidationHelper::isValidKey("invalid key"));
    assert(!ValidationHelper::isValidKey(""));
    
    // Test range validation
    assert(ValidationHelper::isInRange(5, 1, 10));
    assert(!ValidationHelper::isInRange(15, 1, 10));
    
    assert(ValidationHelper::isInRange(3.14, 0.0, 10.0));
    assert(!ValidationHelper::isInRange(3.14, 5.0, 10.0));
    
    // Test neuron count validation
    assert(ValidationHelper::validateNeuronCount(500));
    assert(!ValidationHelper::validateNeuronCount(5000)); // Too high
    
    // Test region count validation
    assert(ValidationHelper::validateRegionCount(1));
    assert(!ValidationHelper::validateRegionCount(50)); // Too high
    
    std::cout << "✓ Validation helper tests passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration and Error Handling Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testConfigBasic();
        testConfigVectorValues();
        testConfigValidation();
        testConfigStats();
        testConstants();
        testErrorHandling();
        testValidationHelper();
        
        std::cout << std::endl;
        std::cout << "=== All Tests Passed ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}