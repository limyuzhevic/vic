// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_neuron {

void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    if (neuron.getId() != nlm::NeuronId(1)) {
        std::cerr << "testNeuronCreation failed: neuron ID mismatch" << std::endl;
        std::exit(1);
    }
    if (neuron.getType() != nlm::NeuronType::Internal) {
        std::cerr << "testNeuronCreation failed: neuron type mismatch" << std::endl;
        std::exit(1);
    }
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    if (neuron.getType() != nlm::NeuronType::Excitatory) {
        std::cerr << "testNeuronType failed: excitatory type mismatch" << std::endl;
        std::exit(1);
    }
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    if (neuron.getType() != nlm::NeuronType::Inhibitory) {
        std::cerr << "testNeuronType failed: inhibitory type mismatch" << std::endl;
        std::exit(1);
    }
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    if (neuron.getMembranePotential() != -70.0f) {
        std::cerr << "testMembranePotential failed: default potential mismatch" << std::endl;
        std::exit(1);
    }
    
    neuron.setMembranePotential(-65.0f);
    if (neuron.getMembranePotential() != -65.0f) {
        std::cerr << "testMembranePotential failed: set potential mismatch" << std::endl;
        std::exit(1);
    }
    
    neuron.addToMembranePotential(5.0f);
    if (neuron.getMembranePotential() != -60.0f) {
        std::cerr << "testMembranePotential failed: add potential mismatch" << std::endl;
        std::exit(1);
    }
    
    std::cout << "    testMembranePotential passed" << std::endl;
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    if (!neuron.isFiring()) {
        std::cerr << "testFiringState failed: neuron should not be firing initially" << std::endl;
        std::exit(1);
    }
    if (!neuron.isRefractory()) {
        std::cerr << "testFiringState failed: neuron should not be refractory initially" << std::endl;
        std::exit(1);
    }
    
    neuron.setFiringState(nlm::FiringState::Active);
    if (!neuron.isFiring()) {
        std::cerr << "testFiringState failed: neuron should be firing after state set to Active" << std::endl;
        std::exit(1);
    }
    
    neuron.setRefractoryPeriod(10);
    if (!neuron.isRefractory()) {
        std::cerr << "testFiringState failed: neuron should be refractory after setting period" << std::endl;
        std::exit(1);
    }
    if (neuron.isFiring()) {
        std::cerr << "testFiringState failed: neuron should not be firing when refractory" << std::endl;
        std::exit(1);
    }
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    if (!neuron.isRefractory()) {
        std::cerr << "testRefractoryDecrement failed: neuron should be refractory" << std::endl;
        std::exit(1);
    }
    
    neuron.decrementRefractory();
    if (!neuron.isRefractory()) {
        std::cerr << "testRefractoryDecrement failed: neuron should still be refractory after one decrement" << std::endl;
        std::exit(1);
    }
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    if (neuron.isRefractory()) {
        std::cerr << "testRefractoryDecrement failed: neuron should not be refractory after all decrements" << std::endl;
        std::exit(1);
    }
    
    std::cout << "    testRefractoryDecrement passed" << std::endl;
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    if (neuron.getTotalCurrent() != 0.0f) {
        std::cerr << "testCurrentInjection failed: neuron should have 0 current initially" << std::endl;
        std::exit(1);
    }
    
    neuron.injectCurrent(5.0f);
    if (neuron.getTotalCurrent() != 5.0f) {
        std::cerr << "testCurrentInjection failed: injectCurrent didn't work" << std::endl;
        std::exit(1);
    }
    
    neuron.clearTotalCurrent();
    if (neuron.getTotalCurrent() != 0.0f) {
        std::cerr << "testCurrentInjection failed: clearTotalCurrent didn't work" << std::endl;
        std::exit(1);
    }
    
    std::cout << "    testCurrentInjection passed" << std::endl;
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    if (neuron.getTotalCurrent() != 5.0f) {
        std::cerr << "testSynapticInput failed: excitatory input didn't work" << std::endl;
        std::exit(1);
    }
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    if (neuron.getTotalCurrent() != -3.0f) {
        std::cerr << "testSynapticInput failed: inhibitory input didn't work" << std::endl;
        std::exit(1);
    }
    
    std::cout << "    testSynapticInput passed" << std::endl;
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    assert(neuron.getSpikeHistory().size() == 3);
    
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
    
    std::cout << "    testRandomInitialization passed" << std::endl;
}

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testReset passed" << std::endl;
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
}

} // namespace test_neuron
