// Synapse Integration Tests
// Tests synapse parameter integration and validation

#include "tests/test_suite_runner.cpp"
#include "src/brain/Synapse.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_synapse_integration {

void testSynapseParameterIntegration() {
    std::cout << "  [testSynapseParameterIntegration] ";

    nlm::Config config;

    // Create synapse with configuration parameters
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));

    // Test synapse creation and basic functionality
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));

    // Test configuration integration
    config.setParameter("synapse", "weight_min", -2.0f, nlm::ConfigSource::File);
    config.setParameter("synapse", "weight_max", 2.0f, nlm::ConfigSource::File);

    float weightMin = config.getParameterOr("synapse", "weight_min", -1.0f);
    float weightMax = config.getParameterOr("synapse", "weight_max", 1.0f);

    assert(weightMin == -2.0f);
    assert(weightMax == 2.0f);

    std::cout << "PASSED" << std::endl;
}

void testSynapseValidationIntegration() {
    std::cout << "  [testSynapseValidationIntegration] ";

    nlm::Config config;

    // Test synapse parameter validation
    bool weightMinValid = config.validateParameter("synapse", "weight_min", -2.0f);
    assert(weightMinValid);

    bool weightMaxValid = config.validateParameter("synapse", "weight_max", 2.0f);
    assert(weightMaxValid);

    bool stpValid = config.validateParameter("synapse", "stp_facilitation_tau", 150.0f);
    assert(stpValid);

    std::cout << "PASSED" << std::endl;
}

void testSynapseConfigurationCompatibility() {
    std::cout << "  [testSynapseConfigurationCompatibility] ";

    nlm::Config config;

    // Test synapse configuration compatibility
    config.setParameter("synapse", "weight_min", -5.0f, nlm::ConfigSource::File);
    config.setParameter("synapse", "stp_u_max", 2.0f, nlm::ConfigSource::File);

    float weightMin = config.getParameterOr("synapse", "weight_min", -1.0f);
    float stpUMax = config.getParameterOr("synapse", "stp_u_max", 1.0f);

    assert(weightMin == -5.0f);
    assert(stpUMax == 2.0f);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testSynapseParameterIntegration();
    testSynapseValidationIntegration();
    testSynapseConfigurationCompatibility();

    std::cout << "  All synapse integration tests passed!" << std::endl;
}

} // namespace test_synapse_integration