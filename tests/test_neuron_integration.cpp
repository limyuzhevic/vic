// Neuron Integration Tests
// Tests for the enhanced Neuron::step() methods with configurable timesteps

#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuron_integration {

void testNeuronStepWithDefaultTimestep() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    // Store initial state
    float initialPotential = neuron.getMembranePotential();
    
    // Take a step with default timestep (should use 1ms)
    neuron.step(1000, 0.001f);
    
    // Neuron should have processed the step
    // (membrane potential may have changed due to integration)
    assert(std::abs(neuron.getMembranePotential() - initialPotential) < 1.0f);
    
    std::cout << "    testNeuronStepWithDefaultTimestep passed" << std::endl;
}

void testNeuronStepWithCustomTimestep() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    float initialPotential = neuron.getMembranePotential();
    
    // Take a step with larger timestep (10ms)
    neuron.step(2000, 0.01f);
    
    // With larger timestep, changes should be more pronounced
    assert(std::abs(neuron.getMembranePotential() - initialPotential) < 5.0f);
    
    std::cout << "    testNeuronStepWithCustomTimestep passed" << std::endl;
}

void testNeuronStepWithTinyTimestep() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    float initialPotential = neuron.getMembranePotential();
    
    // Take a step with very small timestep (0.1ms)
    neuron.step(3000, 0.0001f);
    
    // With smaller timestep, changes should be more subtle
    assert(std::abs(neuron.getMembranePotential() - initialPotential) < 1.0f);
    
    std::cout << "    testNeuronStepWithTinyTimestep passed" << std::endl;
}

void testNeuronStepInvalidTimestep() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    // Test with negative timestep (should use safe default)
    neuron.step(4000, -0.001f);
    
    // Should not crash and should have some state
    assert(!neuron.isFiring());  // Default state
    
    std::cout << "    testNeuronStepInvalidTimestep passed" << std::endl;
}

void testSynapseStepWithConfiguration() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Record initial state
    float initialWeight = synapse.getWeight();
    float initialEfficacy = synapse.getEfficacy();
    
    // Step with default parameters
    synapse.step(1000);
    
    // State should have been processed
    assert(synapse.getWeight() == initialWeight);  // Weight unchanged in basic test
    assert(synapse.getEfficacy() == initialEfficacy);  // Efficacy unchanged in basic test
    
    std::cout << "    testSynapseStepWithConfiguration passed" << std::endl;
}

void testSynapseStepWithCustomParameters() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    float initialWeight = synapse.getWeight();
    
    // Step with custom parameters
    synapse.step(2000, 100.0f, 200.0f, 0.001f);
    
    // Should process with custom parameters
    assert(std::abs(synapse.getWeight() - initialWeight) < 0.001f);
    
    std::cout << "    testSynapseStepWithCustomParameters passed" << std::endl;
}

void testSynapseStepWithConfig() {
    auto config = std::make_shared<nlm::Config>();
    config->set("synaptic_facilitation_tau", 150.0f);
    config->set("synaptic_depression_tau", 250.0f);
    config->set("eligibility_decay_rate", 0.0005f);
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Step with configuration
    synapse.step(3000, config.get());
    
    // Should process with configuration-based parameters
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSynapseStepWithConfig passed" << std::endl;
}

void testSynapseConfigureMethod() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Configure synaptic parameters
    synapse.configure(150.0f, 250.0f, 0.95f, 0.5f, 1.5f);
    
    // Should accept configuration without crashing
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSynapseConfigureMethod passed" << std::endl;
}

void testNeuronSynapseIntegration() {
    // Create a simple integration test
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(0), nlm::NeuronId(1));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Neuron should be able to process the step
    neuron.step(1000, 0.001f);
    
    // Synapse should be able to process the step
    synapse.step(1000);
    
    // Both should be in a valid state
    assert(std::abs(neuron.getMembranePotential()) < 100.0f);
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    std::cout << "    testNeuronSynapseIntegration passed" << std::endl;
}

void testBrainWithEnhancedNeurons() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Run several steps to test integration
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Verify brain is still in a valid state
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getRegionCount() == 1);
    
    std::cout << "    testBrainWithEnhancedNeurons passed" << std::endl;
}

} // namespace test_neuron_integration

// Test runner
void runNeuronIntegrationTests() {
    std::cout << "Running Neuron Integration Tests..." << std::endl;
    
    test_neuron_integration::testNeuronStepWithDefaultTimestep();
    test_neuron_integration::testNeuronStepWithCustomTimestep();
    test_neuron_integration::testNeuronStepWithTinyTimestep();
    test_neuron_integration::testNeuronStepInvalidTimestep();
    test_neuron_integration::testSynapseStepWithConfiguration();
    test_neuron_integration::testSynapseStepWithCustomParameters();
    test_neuron_integration::testSynapseStepWithConfig();
    test_neuron_integration::testSynapseConfigureMethod();
    test_neuron_integration::testNeuronSynapseIntegration();
    test_neuron_integration::testBrainWithEnhancedNeurons();
    
    std::cout << "All Neuron Integration Tests passed!" << std::endl;
}