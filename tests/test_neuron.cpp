// Neuron Tests
#include "nlm.h"
#include <cassert>
#include <iostream>

namespace test_neuron {

void testNeuronCreation() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    assert(neuron.getId() == id);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    neuron.setType(nlm::NeuronType::Sensory);
    assert(neuron.getType() == nlm::NeuronType::Sensory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testNeuronMembranePotential() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    neuron.setMembranePotential(-65.0f);
    assert(std::abs(neuron.getMembranePotential() + 65.0f) < 0.001f);
    
    neuron.addToMembranePotential(10.0f);
    assert(std::abs(neuron.getMembranePotential() + 55.0f) < 0.001f);
    
    std::cout << "    testNeuronMembranePotential passed" << std::endl;
}

void testNeuronThreshold() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    neuron.setThreshold(-50.0f);
    assert(std::abs(neuron.getThreshold() + 50.0f) < 0.001f);
    
    std::cout << "    testNeuronThreshold passed" << std::endl;
}

void testNeuronFiring() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    // Set parameters to trigger firing
    neuron.setThreshold(-60.0f);
    neuron.setMembranePotential(-55.0f);  // Above threshold
    
    // Check threshold (neuron will fire when membrane potential >= threshold)
    assert(neuron.checkThreshold());
    
    std::cout << "    testNeuronFiring passed" << std::endl;
}

void testNeuronCurrentInjection() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    neuron.injectCurrent(20.0f);
    neuron.clearTotalCurrent();  // This is called internally during step
    
    std::cout << "    testNeuronCurrentInjection passed" << std::endl;
}

void testNeuronPlasticity() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    neuron.enablePlasticity(true, true, false);  // Enable Hebbian and STDP
    
    // Check that plasticity flags are set
    const auto& flags = neuron.getPlasticityFlags();
    assert(flags.hebbian);
    assert(flags.stdp);
    assert(!flags.reward_modulated);
    
    std::cout << "    testNeuronPlasticity passed" << std::endl;
}

void testNeuronState() {
    nlm::NeuronId id(1);
    nlm::Neuron neuron(id);
    
    const auto& state = neuron.getState();
    assert(state.membranePotential == -70.0f);
    assert(state.threshold == -55.0f);
    assert(state.firingState == nlm::FiringState::Resting);
    
    std::cout << "    testNeuronState passed" << std::endl;
}

void runAll() {
    testNeuronCreation();
    testNeuronType();
    testNeuronMembranePotential();
    testNeuronThreshold();
    testNeuronFiring();
    testNeuronCurrentInjection();
    testNeuronPlasticity();
    testNeuronState();
}

} // namespace test_neuron
