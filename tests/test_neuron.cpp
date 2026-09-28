// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

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
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getMembranePotential() == -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    std::cout << "    testMembranePotential passed" << std::endl;
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    assert(!neuron.isRefractory());
    
    std::cout << "    testRefractoryDecrement passed" << std::endl;
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getTotalCurrent() == 0.0f);
    
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    testCurrentInjection passed" << std::endl;
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    assert(neuron.getTotalCurrent() == -3.0f);
    
    std::cout << "    testSynapticInput passed" << std::endl;
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    assert(neuron.getSpikeHistory().size() == 3);
    assert(neuron.getSpikeHistory()[0] == 0.1);
    assert(neuron.getSpikeHistory()[1] == 0.2);
    assert(neuron.getSpikeHistory()[2] == 0.3);
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testSpikeHistory passed" << std::endl;
}

void testRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
    assert(neuron.getRestingPotential() < -60.0f && neuron.getRestingPotential() > -80.0f);
    assert(neuron.getResetPotential() >= neuron.getRestingPotential());
    assert(neuron.getRefractoryPeriod() >= 2 && neuron.getRefractoryPeriod() <= 10);
    
    std::cout << "    testRandomInitialization passed" << std::endl;
}

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    neuron.setFiringState(nlm::FiringState::Active);
    neuron.setRefractoryPeriod(5);
    
    neuron.reset();
    
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    assert(neuron.getRefractoryPeriod() == 0);
    
    std::cout << "    testReset passed" << std::endl;
}

void testLIFIntegration() {
    nlm::RandomGenerator rng(123);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initialize with reasonable values
    neuron.initializeRandom(rng);
    neuron.setFiringState(nlm::FiringState::Resting);
    neuron.setRefractoryPeriod(0);
    
    // Test LIF integration without spiking
    float initialV = neuron.getMembranePotential();
    neuron.injectCurrent(10.0f);  // Strong current
    neuron.step(0.0);  // timestep = 0.001ms default
    
    // Should move towards threshold
    assert(neuron.getMembranePotential() > initialV);
    
    std::cout << "    testLIFIntegration passed" << std::endl;
}

void testPlasticityEnableDisable() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Enable all plasticity
    neuron.enablePlasticity(true, true, false);
    auto flags1 = neuron.getPlasticityFlags();
    assert(flags1.hebbian);
    assert(flags1.stdp);
    assert(!flags1.reward_modulated);
    
    // Disable all plasticity
    neuron.enablePlasticity(false, false, false);
    auto flags2 = neuron.getPlasticityFlags();
    assert(!flags2.hebbian);
    assert(!flags2.stdp);
    assert(!flags2.reward_modulated);
    
    // Enable only STDP
    neuron.enablePlasticity(false, true, false);
    auto flags3 = neuron.getPlasticityFlags();
    assert(!flags3.hebbian);
    assert(flags3.stdp);
    assert(!flags3.reward_modulated);
    
    std::cout << "    testPlasticityEnableDisable passed" << std::endl;
}

void testPerformanceScaling() {
    const size_t numNeurons = 1000;
    std::vector<nlm::Neuron> neurons;
    neurons.reserve(numNeurons);
    
    nlm::RandomGenerator rng(999);
    
    // Create many neurons
    for (size_t i = 0; i < numNeurons; ++i) {
        neurons.emplace_back(nlm::NeuronId(i));
        neurons.back().initializeRandom(rng);
    }
    
    // Inject current into all neurons
    for (auto& neuron : neurons) {
        neuron.injectCurrent(1.0f);
    }
    
    // Simulate steps for all neurons (this tests memory efficiency)
    for (size_t step = 0; step < 10; ++step) {
        for (auto& neuron : neurons) {
            neuron.step(step * 0.001);
        }
    }
    
    // Verify all neurons still valid
    for (size_t i = 0; i < numNeurons; ++i) {
        assert(neurons[i].getId() == nlm::NeuronId(i));
        assert(neurons[i].getMembranePotential() < 50.0f);  // Should not overflow
    }
    
    std::cout << "    testPerformanceScaling passed" << std::endl;
}

void runAll() {
    testNeuronCreation();
    testNeuronType();
    testMembranePotential();
    testFiringState();
    testRefractoryDecrement();
    testCurrentInjection();
    testSynapticInput();
    testSpikeHistory();
    testRandomInitialization();
    testReset();
    testLIFIntegration();
    testPlasticityEnableDisable();
    testPerformanceScaling();
}

} // namespace test_neuron
