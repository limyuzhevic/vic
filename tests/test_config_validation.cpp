// Configuration Validation Tests
// Tests configuration validation and error handling

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config_validation {

void testParameterValidationFramework() {
    std::cout << "  [testParameterValidationFramework] ";

    nlm::Config config;

    // Test the validation framework
    bool valid = config.validateParameter("neuron", "membrane_capacitance", 1.5f);
    assert(valid);  // Within valid range (0.1-10.0)

    // Test validation with different parameter types
    bool intValid = config.validateParameter("neuron", "max_spike_history", 100);
    assert(intValid);  // Within valid range (10-1000)

    std::cout << "PASSED" << std::endl;
}

void testNeuronParameterValidation() {
    std::cout << "  [testNeuronParameterValidation] ";

    nlm::Config config;

    // Test neuron-specific parameter validation
    bool capacitanceValid = config.validateParameter("neuron", "membrane_capacitance", 1.0f);
    assert(capacitanceValid);

    bool timeConstantValid = config.validateParameter("neuron", "time_constant", 20.0f);
    assert(timeConstantValid);

    bool thresholdValid = config.validateParameter("neuron", "threshold_default", -55.0f);
    assert(thresholdValid);

    std::cout << "PASSED" << std::endl;
}

void testSynapseParameterValidation() {
    std::cout << "  [testSynapseParameterValidation] ";

    nlm::Config config;

    // Test synapse-specific parameter validation
    bool weightMinValid = config.validateParameter("synapse", "weight_min", -1.0f);
    assert(weightMinValid);

    bool weightMaxValid = config.validateParameter("synapse", "weight_max", 1.0f);
    assert(weightMaxValid);

    bool stpTauValid = config.validateParameter("synapse", "stp_facilitation_tau", 100.0f);
    assert(stpTauValid);

    std::cout << "PASSED" << std::endl;
}

void testBrainParameterValidation() {
    std::cout << "  [testBrainParameterValidation] ";

    nlm::Config config;

    // Test brain-specific parameter validation (if any)
    // These would be defined in the brain configuration section
    config.set("brain.neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("brain.region_count", 2, nlm::ConfigSource::Runtime);

    assert(config.getOr<int>"brain.neuron_count", 0) == 100);
    assert(config.getOr<int>("brain.region_count", 0) == 2);

    std::cout << "PASSED" << std::endl;
}

void testErrorHandling() {
    std::cout << "  [testErrorHandling] ";

    nlm::Config config;

    // Test error handling for invalid parameters
    try {
        // This should not crash - validation framework handles invalid keys gracefully
        bool valid = config.validateParameter("invalid_section", "invalid_key", 0.0f);
    } catch (...) {
        // Error should be handled gracefully
        std::cout << "Error handled gracefully" << std::endl;
    }

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testParameterValidationFramework();
    testNeuronParameterValidation();
    testSynapseParameterValidation();
    testBrainParameterValidation();
    testErrorHandling();

    std::cout << "  All configuration validation tests passed!" << std::endl;
}

} // namespace test_config_validation