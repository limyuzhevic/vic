// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    nlm::Brain brain(config);
    
    // Brain should be created without errors
    assert(brain.getTotalNeuronCount() == 0);  // Not initialized yet
    assert(brain.getTotalSynapseCount() == 0);  // Not initialized yet
    
    std::cout << "    testBrainCreation passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    
    // Set configuration
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42u);
    
    nlm::Brain brain(config);
    
    assert(brain.initialize());
    
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);  // Should have some synapses
    assert(brain.getFiringNeuronCount() == 0);  // No neurons should be firing initially
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    size_t initialNeurons = brain.getTotalNeuronCount();
    assert(initialNeurons == 50);
    
    // Reset brain
    brain.reset();
    brain.initialize();
    
    // Should have same number of neurons after reset
    assert(brain.getTotalNeuronCount() == initialNeurons);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 20);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Step the brain
    brain.step(0, 0.0);
    
    // Should not crash
    // Brain should be in a valid state after stepping
    assert(brain.getTotalNeuronCount() > 0);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainStatistics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.2f);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Check initial statistics
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getFiringNeuronCount() == 0);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Step the brain multiple times to generate some activity
    for (int step = 0; step < 10; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Check that statistics are still valid
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainStatistics passed" << std::endl;
}

void testBrainPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Enable plasticity on some synapses
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Step the brain multiple times
    for (int step = 0; step < 100; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Plasticity should have affected synapse weights
    // (This is a basic test - in practice, we'd check specific weight changes)
    
    std::cout << "    testBrainPlasticity passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainReset();
    testBrainStep();
    testBrainStatistics();
    testBrainPlasticity();
}

} // namespace test_brain
