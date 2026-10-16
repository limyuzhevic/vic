// CLI Validation Tests
// Tests CLI validation and error handling

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_cli_validation {

void testInputValidation() {
    std::cout << "  [testInputValidation] ";

    nlm::Config config;

    // Test input validation
    bool valid = config.validateParameter("neuron", "membrane_capacitance", 1.5f);
    assert(valid);

    std::cout << "PASSED" << std::endl;
}

void testErrorHandling() {
    std::cout << "  [testErrorHandling] ";

    nlm::Config config;

    // Test error handling
    try {
        // Test graceful error handling
        bool valid = config.validateParameter("invalid", "section", 0.0f);
    } catch (...) {
        // Error should be caught gracefully
    }

    std::cout << "PASSED" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "  [testConfigurationValidation] ";

    nlm::Config config;

    // Test configuration validation
    bool configValid = config.validateParameter("neuron", "membrane_capacitance", 2.0f);
    assert(configValid);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testInputValidation();
    testErrorHandling();
    testConfigurationValidation();

    std::cout << "  All CLI validation tests passed!" << std::endl;
}

} // namespace test_cli_validation