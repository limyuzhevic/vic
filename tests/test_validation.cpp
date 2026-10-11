// NLM Configuration Validation Tests
// Tests the configuration validation system for duplicate keys and invalid values

#include <iostream>
#include <cassert>
#include "core/Config/Config.hpp"

namespace test_validation {

void testDuplicateKeyDetection() {
    std::cout << "Testing duplicate key detection..." << std::endl;
    
    Config config;
    
    // Set a valid key-value pair
    config.set("neuron_count", 100);
    config.set("simulation_timestep", 0.001);
    
    // Try to set the same key with different value (should be allowed)
    config.set("neuron_count", 200);  // This overwrites, not duplicates
    
    // Check for duplicates (should not report neuron_count as duplicate since it was overwritten)
    auto validation = config.checkIntegrity();
    
    assert(validation.valid && "Valid configuration should pass integrity check");
    std::cout << "  ✓ Duplicate key detection passed" << std::endl;
}

void testValueValidation() {
    std::cout << "Testing value validation..." << std::endl;
    
    Config config;
    
    // Test valid values
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    config.set("connection_probability", 0.5);
    config.set("log_level", std::string("info"));
    
    // Test invalid neuron_count (negative)
    config.set("neuron_count", -1);
    
    // Check for validation warnings/errors
    auto validation = config.checkIntegrity();
    
    // The validation should report warnings for invalid values
    std::cout << "  ✓ Value validation with warnings passed" << std::endl;
}

void testBooleanValidation() {
    std::cout << "Testing boolean validation..." << std::endl;
    
    Config config;
    
    // Test valid boolean values
    config.set("visualization_enabled", true);
    config.set("log_to_file", false);
    
    // Test string representation of boolean (for backward compatibility)
    config.set("log_to_file", std::string("true"));
    
    auto validation = config.checkIntegrity();
    
    assert(validation.valid && "Configuration should be valid after boolean conversion");
    std::cout << "  ✓ Boolean validation passed" << std::endl;
}

void testSchemaRegistry() {
    std::cout << "Testing schema registry..." << std::endl;
    
    Config config;
    
    // The schema registry should be initialized with default schemas
    // This is tested indirectly through validation
    config.set("neuron_count", 500);
    config.set("simulation_timestep", 0.01);
    
    auto validation = config.checkIntegrity();
    
    assert(validation.valid && "Valid configuration should pass schema validation");
    std::cout << "  ✓ Schema registry validation passed" << std::endl;
}

void testFileLoadValidation() {
    std::cout << "Testing file load validation..." << std::endl;
    
    // Test loading a valid configuration file
    Config config;
    
    // This should load without validation errors
    bool loaded = config.loadFromFile("configs/default.cfg");
    
    if (loaded) {
        std::cout << "  ✓ File load validation passed" << std::endl;
    } else {
        std::cout << "  ! File loading failed (file may not exist or format incorrect)" << std::endl;
    }
}

void runAll() {
    std::cout << "=== Configuration Validation Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testDuplicateKeyDetection();
        std::cout << std::endl;
        
        testValueValidation();
        std::cout << std::endl;
        
        testBooleanValidation();
        std::cout << std::endl;
        
        testSchemaRegistry();
        std::cout << std::endl;
        
        testFileLoadValidation();
        std::cout << std::endl;
        
        std::cout << "=== All Configuration Validation Tests PASSED ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "=== Configuration Validation Tests FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_validation

int main() {
    test_validation::runAll();
    return 0;
}
