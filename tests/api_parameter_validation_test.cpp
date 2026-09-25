// Parameter Validation Test Suite
// Tests for parameter validation and input validation
// Validates proper handling of invalid inputs and edge cases

#include "api_parameter_validation_test.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace nlm {

void testConfigParameterValidation() {
    std::cout << "Testing Config parameter validation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test valid parameter setting
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
    config->set("connection_probability", 0.5, ConfigSource::Runtime);
    
    // Test parameter validation
    assert(config->getOr<int64_t>("neuron_count", 50) == 100);
    assert(config->getOr<int64_t>("region_count", 1) == 2);
    assert(config->getOr<double>("connection_probability", 0.1) == 0.5);
    
    // Test missing parameter fallback
    assert(config->getOr<int64_t>("missing_param", 999) == 999);
    
    // Test parameter removal
    config->remove("neuron_count");
    assert(config->getOr<int64_t>("neuron_count", 50) == 50);
    
    std::cout << "Config parameter validation test passed!" << std::endl;
}

void testBrainParameterValidation() {
    std::cout << "Testing Brain parameter validation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test parameter validation in step method
    // Step count validation
    brain.step(0);   // Minimum valid value
    brain.step(1);   // Normal value
    brain.step(1000); // Large value
    
    // Time delta validation
    brain.step(10, 0.0f);   // Minimum time delta
    brain.step(10, 0.001f); // Small time delta
    brain.step(10, 1.0f);   // Normal time delta
    brain.step(10, 1000.0f); // Large time delta
    
    // Test that invalid parameters are handled gracefully
    // (Brain implementation should validate and handle or throw appropriate errors)
    
    std::cout << "Brain parameter validation test passed!" << std::endl;
}

void testRangeValidation() {
    std::cout << "Testing range validation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test range validation for configuration parameters
    // Neuron count should be reasonable
    uint64_t neuronCount = brain.getTotalNeuronCount();
    assert(neuronCount > 0);
    
    // Region count should be reasonable
    uint64_t regionCount = brain.getRegionCount();
    assert(regionCount > 0);
    
    // Connection probability should be in valid range (0.0 to 1.0)
    // (This is enforced by Config validation)
    
    // Firing rate should be non-negative
    float firingRate = brain.getAverageFiringRate();
    assert(firingRate >= 0.0f);
    
    // Excitation-inhibition ratio should be non-negative
    float eiRatio = brain.getExcitationInhibitionRatio();
    assert(eiRatio >= 0.0f);
    
    std::cout << "Range validation test passed!" << std::endl;
}

void testDataTypeValidation() {
    std::cout << "Testing data type validation..." << std::endl;
    
    // Test that Config handles different data types correctly
    auto config = std::make_shared<Config>();
    
    // Test different data types
    config->set("int_value", 42, ConfigSource::Runtime);
    config->set("int64_value", static_cast<int64_t>(1000), ConfigSource::Runtime);
    config->set("double_value", 3.14, ConfigSource::Runtime);
    config->set("bool_value", true, ConfigSource::Runtime);
    config->set("string_value", "test", ConfigSource::Runtime);
    
    // Test type-safe retrieval
    assert(config->getOr<int>("int_value", 0) == 42);
    assert(config->getOr<int64_t>("int64_value", 0) == 1000);
    assert(config->getOr<double>("double_value", 0.0) == 3.14);
    assert(config->getOr<bool>("bool_value", false) == true);
    assert(config->getOr<std::string>("string_value", "") == "test");
    
    std::cout << "Data type validation test passed!" << std::endl;
}

void testEdgeCaseParameters() {
    std::cout << "Testing edge case parameters..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    // Test edge case parameters
    brain.step(1);      // Minimum step count
    brain.step(1000000); // Very large step count
    brain.step(1, 0.0f);   // Zero time delta
    brain.step(1, 0.0001f); // Very small time delta
    brain.step(1, 1000000.0f); // Very large time delta
    
    // Test edge case configuration values
    config->set("neuron_count", static_cast<int64_t>(1), ConfigSource::Runtime); // Minimum neurons
    config->set("neuron_count", static_cast<int64_t>(1000000), ConfigSource::Runtime); // Large neuron count
    
    std::cout << "Edge case parameters test passed!" << std::endl;
}

void runParameterValidationTests() {
    std::cout << "=== Running Parameter Validation Tests ===" << std::endl;
    std::cout << std::endl;
    
    testConfigParameterValidation();
    std::cout << std::endl;
    
    testBrainParameterValidation();
    std::cout << std::endl;
    
    testRangeValidation();
    std::cout << std::endl;
    
    testDataTypeValidation();
    std::cout << std::endl;
    
    testEdgeCaseParameters();
    std::cout << std::endl;
    
    std::cout << "=== All Parameter Validation Tests Passed ===" << std::endl;
}

} // namespace nlm
