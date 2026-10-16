// Neuron Integration Tests
// Tests neuron parameter integration and validation

#include "tests/test_suite_runner.cpp"
#include "src/brain/Neuron.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_neuron_integration {

void testNeuronParameterIntegration() {
    std::cout << "  [testNeuronParameterIntegration] ";

    nlm::Config config;

    // Create neuron with configuration parameters
    nlm::Neuron neuron(nlm::NeuronId(1));

    // Verify neuron ID
    assert(neuron.getId() == nlm::NeuronId(1));

    // Test configuration integration
    config.set("neuron_count", 100, nlm::ConfigSource::Default);
    int neuronCount = config.getOr<int>("neuron_count", 50);
    assert(neuronCount == 100);

    std::cout << "PASSED" << std::endl;
}

void testNeuronValidationIntegration() {
    std::cout << "  [testNeuronValidationIntegration] ";

    nlm::Config config;

    // Test neuron parameter validation
    bool capacitanceValid = config.validateParameter("neuron", "membrane_capacitance", 1.5f);
    assert(capacitanceValid);

    bool timeConstantValid = config.validateParameter("neuron", "time_constant", 25.0f);
    assert(timeConstantValid);

    std::cout << "PASSED" << std::endl;
}

void testNeuronConfigurationCompatibility() {
    std::cout << "  [testNeuronConfigurationCompatibility] ";

    nlm::Config config;

    // Test neuron configuration compatibility
    config.setParameter("neuron", "membrane_capacitance", 3.0f, nlm::ConfigSource::File);

    float cap = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    assert(cap == 3.0f);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testNeuronParameterIntegration();
    testNeuronValidationIntegration();
    testNeuronConfigurationCompatibility();

    std::cout << "  All neuron integration tests passed!" << std::endl;
}

} // namespace test_neuron_integration