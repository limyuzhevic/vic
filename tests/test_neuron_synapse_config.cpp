// Neuron and Synapse Config Integration Tests
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_neuron_synapse_config {

void testNeuronConfigIntegration() {
    nlm::Config config;
    
    // Set neuron parameters
    config.setParameter("neuron", "membrane_capacitance", 2.0f, nlm::ConfigSource::Runtime);
    config.setParameter("neuron", "time_constant", 25.0f, nlm::ConfigSource::Runtime);
    config.setParameter("neuron", "max_spike_history", 200, nlm::ConfigSource::Runtime);
    
    // Verify neuron parameters are accessible
    float membraneCapacitance = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    assert(membraneCapacitance == 2.0f);
    
    float timeConstant = config.getParameterOr("neuron", "time_constant", 20.0f);
    assert(timeConstant == 25.0f);
    
    int maxSpikeHistory = config.getParameterOr("neuron", "max_spike_history", 100);
    assert(maxSpikeHistory == 200);
    
    // Test validation - try to set invalid value (should throw or return false)
    try {
        config.setParameter("neuron", "membrane_capacitance", 50.0f, nlm::ConfigSource::Runtime);
        assert(false); // Should not reach here if validation works
    } catch (const std::runtime_error&) {
        // Expected - invalid value rejected
    }
    
    std::cout << "    testNeuronConfigIntegration passed" << std::endl;
}

void testSynapseConfigIntegration() {
    nlm::Config config;
    
    // Set synapse parameters
    config.setParameter("synapse", "weight_min", -2.0f, nlm::ConfigSource::Runtime);
    config.setParameter("synapse", "weight_max", 2.0f, nlm::ConfigSource::Runtime);
    config.setParameter("synapse", "stp_facilitation_tau", 150.0f, nlm::ConfigSource::Runtime);
    config.setParameter("synapse", "stp_depression_tau", 300.0f, nlm::ConfigSource::Runtime);
    
    // Verify synapse parameters are accessible
    float weightMin = config.getParameterOr("synapse", "weight_min", -1.0f);
    assert(weightMin == -2.0f);
    
    float weightMax = config.getParameterOr("synapse", "weight_max", 1.0f);
    assert(weightMax == 2.0f);
    
    float facilitationTau = config.getParameterOr("synapse", "stp_facilitation_tau", 100.0f);
    assert(facilitationTau == 150.0f);
    
    std::cout << "    testSynapseConfigIntegration passed" << std::endl;
}

void testConfigBackwardCompatibility() {
    nlm::Config config;
    
    // Test that when config values are not set, defaults are used
    // (This is handled internally by getParameterOr)
    
    float defaultCapacitance = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    assert(defaultCapacitance == 1.0f); // Default value
    
    float defaultWeightMin = config.getParameterOr("synapse", "weight_min", -1.0f);
    assert(defaultWeightMin == -1.0f); // Default value
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
}

void testConfigFileLoading() {
    nlm::Config config;
    
    // Note: This would test actual file loading if implemented
    // For now, we'll just test that the config system works
    
    std::cout << "    testConfigFileLoading skipped (placeholder)" << std::endl;
}

void testParameterValidation() {
    nlm::Config config;
    
    // Test float range validation
    bool valid = config.validateParameter("neuron", "membrane_capacitance", 1.5f);
    assert(valid); // Should be valid (within 0.1-10.0)
    
    // Test out of range validation
    valid = config.validateParameter("neuron", "membrane_capacitance", 50.0f);
    assert(!valid); // Should be invalid (out of range)
    
    // Test int range validation
    valid = config.validateParameter("neuron", "max_spike_history", 150);
    assert(valid); // Should be valid (within 10-1000)
    
    valid = config.validateParameter("neuron", "max_spike_history", 5);
    assert(!valid); // Should be invalid (below minimum)
    
    std::cout << "    testParameterValidation passed" << std::endl;
}

void testRequiredParameters() {
    nlm::Config config;
    
    // Note: None of our parameters are currently marked as required
    // This test would verify that required parameters are enforced
    
    std::cout << "    testRequiredParameters skipped (no required parameters)" << std::endl;
}

void runAll() {
    testNeuronConfigIntegration();
    testSynapseConfigIntegration();
    testConfigBackwardCompatibility();
    testConfigFileLoading();
    testParameterValidation();
    testRequiredParameters();
}

} // namespace test_neuron_synapse_config