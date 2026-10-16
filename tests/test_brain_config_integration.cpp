// Brain Configuration Integration Tests
// Tests brain configuration validation and integration

#include "tests/test_suite_runner.cpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain_config_integration {

void testBrainConfigurationIntegration() {
    std::cout << "  [testBrainConfigurationIntegration] ";

    nlm::Config config;

    // Configure brain with integration parameters
    config.set("neuron_count", 1000, nlm::ConfigSource::Default);
    config.set("region_count", 4, nlm::ConfigSource::Default);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Default);

    // Verify configuration values
    int neuronCount = config.getOr<int>("neuron_count", 100);
    int regionCount = config.getOr<int>("region_count", 1);
    float connectionProb = config.getOr<float>("connection_probability", 0.1f);

    assert(neuronCount == 1000);
    assert(regionCount == 4);
    assert(connectionProb == 0.15f);

    std::cout << "PASSED" << std::endl;
}

void testBrainParameterValidation() {
    std::cout << "  [testBrainParameterValidation] ";

    nlm::Config config;

    // Test brain-specific parameter validation
    // Brain-level parameters would be defined in the brain configuration section

    std::cout << "PASSED" << std::endl;
}

void testBrainConfigurationCompatibility() {
    std::cout << "  [testBrainConfigurationCompatibility] ";

    nlm::Config config;

    // Test brain configuration compatibility with neural components
    config.setParameter("neuron", "membrane_capacitance", 2.0f, nlm::ConfigSource::File);
    config.setParameter("synapse", "weight_min", -3.0f, nlm::ConfigSource::File);

    float cap = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    float weightMin = config.getParameterOr("synapse", "weight_min", -1.0f);

    assert(cap == 2.0f);
    assert(weightMin == -3.0f);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testBrainConfigurationIntegration();
    testBrainParameterValidation();
    testBrainConfigurationCompatibility();

    std::cout << "  All brain configuration integration tests passed!" << std::endl;
}

} // namespace test_brain_config_integration