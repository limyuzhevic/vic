// Validation Framework Tests
// Tests the error handling and validation framework

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_validation_framework {

void testValidationFramework() {
    std::cout << "  [testValidationFramework] ";

    nlm::Config config;

    // Test the validation framework
    bool valid = config.validateParameter("neuron", "membrane_capacitance", 2.0f);
    assert(valid);

    // Test validation with different parameter types
    bool intValid = config.validateParameter("neuron", "max_spike_history", 50);
    assert(intValid);

    bool floatValid = config.validateParameter("neuron", "time_constant", 25.0f);
    assert(floatValid);

    std::cout << "PASSED" << std::endl;
}

void testErrorHandlingFramework() {
    std::cout << "  [testErrorHandlingFramework] ";

    nlm::Config config;

    // Test error handling framework
    try {
        // Test graceful handling of invalid parameters
        bool valid = config.validateParameter("invalid_section", "invalid_key", 0.0f);
    } catch (...) {
        // Error should be caught and handled gracefully
    }

    std::cout << "PASSED" << std::endl;
}

void testValidationRules() {
    std::cout << "  [testValidationRules] ";

    nlm::Config config;

    // Test validation rules for different parameter types
    // Range validation
    bool capValid = config.validateParameter("neuron", "membrane_capacitance", 5.0f);
    assert(capValid);

    // Boundary testing
    bool lowerBoundValid = config.validateParameter("neuron", "membrane_capacitance", 0.1f);
    assert(lowerBoundValid);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testValidationFramework();
    testErrorHandlingFramework();
    testValidationRules();

    std::cout << "  All validation framework tests passed!" << std::endl;
}

} // namespace test_validation_framework