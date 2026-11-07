// Simple test for the enhanced Config class
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Simple Config Test ===" << std::endl;
    
    nlm::Config config;
    
    // Test 1: Basic operations
    std::cout << "Test 1: Basic operations..." << std::endl;
    
    // Set values
    config.set("test_int", 42);
    config.set("test_double", 3.14159);
    config.set("test_string", std::string("NLM Test"));
    config.set("test_bool", true);
    config.setDescription("test_int", "Test integer value");
    
    // Check existence
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    // Get values
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14159) < 0.0001);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "NLM Test");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test getOr
    int defaultVal = config.getOr<int>("nonexistent", 999);
    assert(defaultVal == 999);
    
    int existingVal = config.getOr<int>("test_int", 999);
    assert(existingVal == 42);
    
    std::cout << "  PASSED" << std::endl;
    
    // Test 2: Validation
    std::cout << "Test 2: Validation..." << std::endl;
    
    // Create validation function
    auto rangeValidator = [](nlm::ConfigValue val) -> bool {
        bool result = false;
        std::visit([&](auto&& v) {
            result = (v >= 0 && v <= 100);
        }, val);
        return result;
    };
    
    // Set with validation - should succeed
    bool validSet = config.setWithValidation("valid_value", 50, rangeValidator);
    assert(validSet);
    
    // Set with validation - should fail
    bool invalidSet = config.setWithValidation("invalid_value", 150, rangeValidator);
    assert(!invalidSet);
    
    // Verify valid value was set
    assert(config.has("valid_value"));
    
    // Verify invalid value was NOT set
    assert(!config.has("invalid_value"));
    
    std::cout << "  PASSED" << std::endl;
    
    // Test 3: File operations
    std::cout << "Test 3: File operations..." << std::endl;
    
    // Create new config and load
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile("/tmp/nlm_config_test.cfg");
    assert(loadSuccess);
    
    // Verify loaded values
    assert(config2.has("test_int"));
    assert(config2.getOr<int>("test_int", 0) == 42);
    
    assert(config2.has("test_string"));
    auto loadedString = config2.get<std::string>("test_string");
    assert(loadedString.has_value() && *loadedString == "NLM Test");
    
    std::cout << "  PASSED" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== All Config Tests PASSED ===" << std::endl;
    
    return 0;
}
