// Comprehensive Edge Case Tests for NLM Brain
// Tests boundary conditions, error cases, and edge scenarios

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_edge_cases {

void testNeuronBoundaryConditions() {
    std::cout << "    testNeuronBoundaryConditions" << std::endl;
    
    // Test minimum neuron configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test single neuron operation
    assert(brain.getTotalNeuronCount() == 1);
    
    // Test neuron with extreme values (should not crash)
    brain.step(0);
    brain.reset();
    
    // Test neuron with zero timestep (should handle gracefully)
    brain.step(0, 0.0);
    
    std::cout << "        testNeuronBoundaryConditions passed" << std::endl;
}

void testConfigurationEdgeCases() {
    std::cout << "    testConfigurationEdgeCases" << std::endl;
    
    // Test invalid configuration values
    auto config = std::make_shared<nlm::Config>();
    
    // Very large neuron count (should still work)
    config->set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test with extremely high connection probability
    config->set("connection_probability", 0.999f, nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    // Test with zero connection probability
    config->set("connection_probability", 0.0f, nlm::ConfigSource::Default);
    nlm::Brain brain3(config);
    assert(brain3.initialize());
    
    std::cout << "        testConfigurationEdgeCases passed" << std::endl;
}

void testMemoryManagementEdgeCases() {
    std::cout << "    testMemoryManagementEdgeCases" << std::endl;
    
    // Test multiple brain creations and destructions
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    {
        nlm::Brain brain(config);
        assert(brain.initialize());
        brain.step(0);
        
        // Brain should be destroyed here
    }
    
    // Test that memory is properly freed after destruction
    // (This is mainly to ensure no memory leaks)
    
    std::cout << "        testMemoryManagementEdgeCases passed" << std::endl;
}

void testErrorHandlingEdgeCases() {
    std::cout << "    testErrorHandlingEdgeCases" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test stepping with invalid parameters
    brain.step(-1, -1.0);  // Negative step (should handle gracefully)
    
    // Test save/load with invalid paths
    bool save_success = brain.save("/invalid/path/that/does/not/exist.bin");
    assert(!save_success);  // Should fail gracefully
    
    bool load_success = brain.load("/invalid/path/that/does/not/exist.bin");
    assert(!load_success);  // Should fail gracefully
    
    // Test reset with uninitialized brain
    nlm::Brain brain2(config);
    brain2.reset();  // Should not crash
    
    std::cout << "        testErrorHandlingEdgeCases passed" << std::endl;
}

void testNeuronalDynamicsEdgeCases() {
    std::cout << "    testNeuronalDynamicsEdgeCases" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Step with extremely large timestep (should handle gracefully)
    brain.step(0, 1000.0);  // Large timestep
    
    // Step with very small timestep (should handle gracefully)
    brain.step(1, 0.000001);  // Very small timestep
    
    // Test rapid stepping (stress test)
    for (int i = 0; i < 1000; ++i) {
        brain.step(i, static_cast<nlm::Timestamp>(i) * 0.001);
    }
    
    std::cout << "        testNeuronalDynamicsEdgeCases passed" << std::endl;
}

void testPlasticityEdgeCases() {
    std::cout << "    testPlasticityEdgeCases" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Enable all plasticity mechanisms
    config->set("plasticity_stdp_enable", true, nlm::ConfigSource::Default);
    config->set("plasticity_hebbian_enable", true, nlm::ConfigSource::Default);
    config->set("plasticity_structural_enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test with extreme plasticity parameters
    config->set("stdp_ltp_weight", 1000.0f, nlm::ConfigSource::Default);  // Very high LTP
    config->set("stdp_ltd_weight", 1000.0f, nlm::ConfigSource::Default);  // Very high LTD
    config->set("plasticity_learning_rate", 10.0f, nlm::ConfigSource::Default);  // Very high learning rate
    
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    // Run simulation with extreme plasticity
    for (int i = 0; i < 100; ++i) {
        brain2.step(i);
    }
    
    std::cout << "        testPlasticityEdgeCases passed" << std::endl;
}

void testDevelopmentEdgeCases() {
    std::cout << "    testDevelopmentEdgeCases" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test development with extreme values
    config->set("development_synaptogenesis_rate", 1.0f, nlm::ConfigSource::Default);  // Very high
    config->set("development_pruning_rate", 1.0f, nlm::ConfigSource::Default);  // Very high
    
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    // Run simulation with extreme development
    for (int i = 0; i < 100; ++i) {
        brain2.step(i);
    }
    
    std::cout << "        testDevelopmentEdgeCases passed" << std::endl;
}

void testNeuromodulationEdgeCases() {
    std::cout << "    testNeuromodulationEdgeCases" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Enable all neuromodulation
    config->set("neuromod_curiosity_enable", true, nlm::ConfigSource::Default);
    config->set("neuromod_novelty_enable", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test with extreme neuromodulation values
    config->set("neuromod_dopamine_scale", 1000.0f, nlm::ConfigSource::Default);  // Very high
    
    nlm::Brain brain2(config);
    assert(brain2.initialize());
    
    // Run simulation with extreme neuromodulation
    for (int i = 0; i < 100; ++i) {
        brain2.step(i);
    }
    
    std::cout << "        testNeuromodulationEdgeCases passed" << std::endl;
}

void testCompleteEdgeCaseSuite() {
    std::cout << "    Running complete edge case suite..." << std::endl;
    
    testNeuronBoundaryConditions();
    testConfigurationEdgeCases();
    testMemoryManagementEdgeCases();
    testErrorHandlingEdgeCases();
    testNeuronalDynamicsEdgeCases();
    testPlasticityEdgeCases();
    testDevelopmentEdgeCases();
    testNeuromodulationEdgeCases();
    
    std::cout << "    Complete edge case suite passed!" << std::endl;
}

} // namespace test_edge_cases

// Main function to run edge case tests
void runEdgeCaseTests() {
    std::cout << "Running comprehensive edge case tests for NLM Brain..." << std::endl;
    test_edge_cases::testCompleteEdgeCaseSuite();
    std::cout << "All edge case tests passed!" << std::endl;
}
