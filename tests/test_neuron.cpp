// Enhanced Neuron Tests
// Tests LIF neuron dynamics and functionality

#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuron {

void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    neuron.setType(nlm::NeuronType::Sensory);
    assert(neuron.getType() == nlm::NeuronType::Sensory);
    
    neuron.setType(nlm::NeuronType::Motor);
    assert(neuron.getType() == nlm::NeuronType::Motor);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testNeuronLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setType(nlm::NeuronType::Internal);
    
    // Initialize neuron
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Set up for firing test
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-75.0f);
    
    // Initially below threshold
    assert(!neuron.checkThreshold());
    
    // Inject enough current to cause firing
    neuron.injectCurrent(30.0f);  // Should cause spike
    
    // Step through time
    bool fired = neuron.stepLIF(0.0, 0.001);
    
    // Should have fired
    assert(fired == true);
    
    // After firing, should be in refractory state
    assert(neuron.isRefractory());
    
    // Should have spike in history
    const auto& spikeHistory = neuron.getSpikeHistory();
    assert(!spikeHistory.empty());
    
    std::cout << "    testNeuronLIFDynamics passed" << std::endl;
}

void testNeuronInputOutput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test excitatory input
    neuron.receiveExcitatoryInput(10.0f);
    assert(neuron.getState().synapticConductance > 0.0f);
    
    // Test inhibitory input
    neuron.receiveInhibitoryInput(10.0f);
    // Should subtract from conductance
    
    // Test modulatory input
    neuron.receiveModulatoryInput(5.0f);
    
    // Test current injection
    neuron.injectCurrent(15.0f);
    
    std::cout << "    testNeuronInputOutput passed" << std::endl;
}

void testNeuronState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test initial state
    assert(neuron.getState().membranePotential == -70.0f);
    assert(neuron.getState().threshold == -55.0f);
    assert(neuron.getState().refractoryPeriod == 5);
    
    // Test state modifications
    neuron.setMembranePotential(-60.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    neuron.setThreshold(-50.0f);
    assert(neuron.getThreshold() == -50.0f);
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.getRefractoryPeriod() == 10);
    
    neuron.setRestingPotential(-68.0f);
    assert(neuron.getRestingPotential() == -68.0f);
    
    neuron.setResetPotential(-72.0f);
    assert(neuron.getResetPotential() == -72.0f);
    
    std::cout << "    testNeuronState passed" << std::endl;
}

void testNeuronSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initially empty
    assert(neuron.getSpikeHistory().empty());
    
    // Record spikes
    neuron.recordSpike(0.1f);
    neuron.recordSpike(0.2f);
    neuron.recordSpike(0.3f);
    
    assert(neuron.getSpikeHistory().size() == 3);
    assert(neuron.getSpikeHistory()[0] == 0.1f);
    assert(neuron.getSpikeHistory()[1] == 0.2f);
    assert(neuron.getSpikeHistory()[2] == 0.3f);
    
    // Clear history
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testNeuronSpikeHistory passed" << std::endl;
}

void testNeuronPlasticity() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test initial state
    assert(!neuron.getPlasticityFlags().hebbian);
    assert(!neuron.getPlasticityFlags().stdp);
    assert(!neuron.getPlasticityFlags().reward_modulated);
    
    // Test enabling plasticity
    neuron.enablePlasticity(true, true, true);
    assert(neuron.getPlasticityFlags().hebbian);
    assert(neuron.getPlasticityFlags().stdp);
    assert(neuron.getPlasticityFlags().reward_modulated);
    
    // Test disabling specific rules
    neuron.enablePlasticity(false, true, false);
    assert(!neuron.getPlasticityFlags().hebbian);
    assert(neuron.getPlasticityFlags().stdp);
    assert(!neuron.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testNeuronPlasticity passed" << std::endl;
}

void testNeuronReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Modify state
    neuron.setMembranePotential(-60.0f);
    neuron.setThreshold(-50.0f);
    neuron.setRefractoryPeriod(10);
    neuron.recordSpike(0.1f);
    neuron.recordSpike(0.2f);
    
    // Reset neuron
    neuron.reset();
    
    // Should be back to initial state
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getThreshold() == -55.0f);
    assert(neuron.getRefractoryPeriod() == 5);
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testNeuronReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuron tests..." << std::endl;
    testNeuronCreation();
    testNeuronType();
    testNeuronLIFDynamics();
    testNeuronInputOutput();
    testNeuronState();
    testNeuronSpikeHistory();
    testNeuronPlasticity();
    testNeuronReset();
}

} // namespace test_neuron
