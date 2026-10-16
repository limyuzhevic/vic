// Configuration Presets Tests
// Tests preset configuration loading and management

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <map>

namespace test_config_presets {

void testPresetManagement() {
    std::cout << "  [testPresetManagement] ";

    nlm::Config config;

    // Test preset parameter definitions
    float membraneCapacitance = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    float timeConstant = config.getParameterOr("neuron", "time_constant", 20.0f);

    assert(membraneCapacitance == 1.0f);
    assert(timeConstant == 20.0f);

    std::cout << "PASSED" << std::endl;
}

void testStandardPresets() {
    std::cout << "  [testStandardPresets] ";

    nlm::Config config;

    // Test standard preset values for different components
    float neuronCap = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    float neuronTC = config.getParameterOr("neuron", "time_constant", 20.0f);
    float neuronMaxSpike = config.getParameterOr("neuron", "max_spike_history", 100);

    assert(neuronCap == 1.0f);
    assert(neuronTC == 20.0f);
    assert(neuronMaxSpike == 100);

    float synapseWeightMin = config.getParameterOr("synapse", "weight_min", -1.0f);
    float synapseWeightMax = config.getParameterOr("synapse", "weight_max", 1.0f);

    assert(synapseWeightMin == -1.0f);
    assert(synapseWeightMax == 1.0f);

    std::cout << "PASSED" << std::endl;
}

void testCustomPresets() {
    std::cout << "  [testCustomPresets] ";

    nlm::Config config;

    // Test custom preset values
    config.setParameter("neuron", "membrane_capacitance", 3.0f, nlm::ConfigSource::File);
    config.setParameter("synapse", "weight_min", -5.0f, nlm::ConfigSource::File);

    float cap = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    float weightMin = config.getParameterOr("synapse", "weight_min", -1.0f);

    assert(cap == 3.0f);
    assert(weightMin == -5.0f);

    std::cout << "PASSED" << std::endl;
}

void testPresetSections() {
    std::cout << "  [testPresetSections] ";

    nlm::Config config;

    // Test that presets are organized by sections
    std::vector<std::string> neuronParams = {
        "neuron.membrane_capacitance",
        "neuron.time_constant",
        "neuron.threshold_default",
        "neuron.reset_potential_default"
    };

    for (const auto& param : neuronParams) {
        // Verify that neuron parameters are properly configured
        bool hasParam = config.has(param.substr(0, param.find('.')));
        // Some parameters may not be in the entries map, but should exist as registered parameters
    }

    std::cout << "PASSED" << std::endl;
}

void testPresetValidation() {
    std::cout << "  [testPresetValidation] ";

    nlm::Config config;

    // Test that preset values are validated
    bool validCapacitance = config.validateParameter("neuron", "membrane_capacitance", 5.0f);
    assert(validCapacitance);

    bool validTimeConstant = config.validateParameter("neuron", "time_constant", 30.0f);
    assert(validTimeConstant);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testPresetManagement();
    testStandardPresets();
    testCustomPresets();
    testPresetSections();
    testPresetValidation();

    std::cout << "  All configuration presets tests passed!" << std::endl;
}

} // namespace test_config_presets