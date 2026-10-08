// Neuron Tests
// Tests for Neuron class functionality

#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuron {

void testNeuronCreation() {
    nlm::NeuronId id(42);
    nlm::Neuron neuron(id);
    
    assert(neuron.getId().value == 42);
    assert(neuron.getType() == nlm::NeuronType::Internal);
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getThreshold() == -55.0f);
    assert(neuron.isFiring() == false);
    assert(neuron.isRefractory() == false);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronStateModification() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test state modification
    neuron.setMembranePotential(-60.0f);
    assert(std::abs(neuron.getMembranePotential() - (-60.0f)) < 0.01f);
    
    neuron.setThreshold(-50.0f);
    assert(std::abs(neuron.getThreshold() - (-50.0f)) < 0.01f);
    
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring() == true);
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.getRefractoryPeriod() == 10);
    
    neuron.addToMembranePotential(5.0f);
    assert(std::abs(neuron.getMembranePotential() - (-55.0f)) < 0.01f);
    
    std::cout << "    testNeuronStateModification passed" << std::endl;
}

void testNeuronLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(12345));
    
    // Set neuron near threshold to test spike generation
    neuron.setThreshold(-65.0f);
    neuron.setMembranePotential(-64.0f);  // Above threshold
    neuron.setRefractoryPeriod(0);  // Clear any existing refractory
    
    float initialPot = neuron.getMembranePotential();
    bool fired = neuron.stepLIF(0.0, 0.001);
    
    // Neuron should have fired
    assert(fired == true);
    assert(neuron.isFiring() == false);  // Should be in refractory after firing
    assert(neuron.isRefractory() == true);
    
    // Membrane potential should have been reset
    assert(neuron.getMembranePotential() == neuron.getResetPotential());
    assert(neuron.getLastSpikeTime() >= 0.0f);
    
    std::cout << "    testNeuronLIFDynamics passed" << std::endl;
}

void testNeuronInputProcessing() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Clear any existing input
    neuron.clearTotalCurrent();
    
    // Test different types of inputs
    neuron.receiveExcitatoryInput(10.0f);
    neuron.receiveInhibitoryInput(5.0f);
    neuron.receiveModulatoryInput(2.0f);
    neuron.injectCurrent(8.0f);
    
    // The input should be accumulated (exact implementation depends on Neuron)
    // For now just test that methods don't crash
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(1.0);
    assert(neuron.getSpikeHistory().size() == 1);
    assert(neuron.getSpikeHistory()[0] == 1.0);
    
    std::cout << "    testNeuronInputProcessing passed" << std::endl;
}

void testNeuronPlasticity() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test plasticity flags
    assert(neuron.getPlasticityFlags().hebbian == false);
    assert(neuron.getPlasticityFlags().stdp == false);
    assert(neuron.getPlasticityFlags().reward_modulated == false);
    
    neuron.enablePlasticity(true, false, true);
    assert(neuron.getPlasticityFlags().hebbian == true);
    assert(neuron.getPlasticityFlags().stdp == false);
    assert(neuron.getPlasticityFlags().reward_modulated == true);
    
    // Test region/population membership
    neuron.setRegionId(nlm::RegionId(5));
    assert(neuron.getRegionId().value == 5);
    
    neuron.setPopulationId(nlm::PopulationId(10));
    assert(neuron.getPopulationId().value == 10);
    
    std::cout << "    testNeuronPlasticity passed" << std::endl;
}

void testNeuronResetAndInitialization() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setMembranePotential(-50.0f);
    neuron.recordSpike(1.0);
    neuron.setFiringState(nlm::FiringState::Active);
    
    neuron.reset();
    
    // Should be reset to default state
    assert(std::abs(neuron.getMembranePotential() - (-70.0f)) < 0.01f);
    assert(neuron.isFiring() == false);
    assert(neuron.isRefractory() == false);
    assert(neuron.getSpikeHistory().empty());
    
    // Test random initialization
    nlm::RandomGenerator rng(12345);
    neuron.initializeRandom(rng);
    assert(neuron.getLastSpikeTime() == -1.0f);
    
    std::cout << "    testNeuronResetAndInitialization passed" << std::endl;
}

void runAll() {
    std::cout << "=== Neuron Tests ===" << std::endl;
    testNeuronCreation();
    testNeuronStateModification();
    testNeuronLIFDynamics();
    testNeuronInputProcessing();
    testNeuronPlasticity();
    testNeuronResetAndInitialization();
    std::cout << "All neuron tests passed!" << std::endl << std::endl;
}

} // namespace test_neuron