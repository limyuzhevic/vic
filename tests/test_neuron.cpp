// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

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

void testLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up neuron for LIF dynamics
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setLeakConductance(10.0f);
    neuron.setRefractoryPeriod(5);
    
    // Test 1: Neuron starts at rest and integrates input
    neuron.injectCurrent(50.0f);  // Strong current input
    bool fired = neuron.stepLIF(0.0, 0.001);  // 1ms timestep
    assert(!fired);  // Shouldn't fire immediately
    assert(neuron.getMembranePotential() > neuron.getRestingPotential());  // Potential should increase
    
    // Test 2: Apply more current to reach threshold and fire
    neuron.injectCurrent(100.0f);
    neuron.stepLIF(0.001, 0.001);
    assert(neuron.isFiring());  // Should be firing
    assert(neuron.isRefractory());  // Should enter refractory period
    
    // Reset and test refractory decrement
    neuron.reset();
    neuron.setRefractoryPeriod(3);
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(!neuron.isRefractory());  // Should exit refractory
    
    std::cout << "    testLIFDynamics passed" << std::endl;
}

void testSynapticIntegration() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    
    // Test excitatory input
    neuron.receiveExcitatoryInput(25.0f);
    neuron.stepLIF(0.0, 0.001);
    assert(neuron.getMembranePotential() > -70.0f);  // Potential should increase
    
    // Reset and test inhibitory input
    neuron.reset();
    neuron.receiveInhibitoryInput(15.0f);
    neuron.stepLIF(0.0, 0.001);
    assert(neuron.getMembranePotential() < -70.0f);  // Potential should decrease
    
    // Reset and test adaptation
    neuron.reset();
    neuron.recordSpike(0.0);  // Record a spike
    neuron.stepLIF(0.001, 0.001);  // Step after spike
    assert(neuron.getMembranePotential() < -70.0f);  // Adaptation should hyperpolarize
    
    std::cout << "    testSynapticIntegration passed" << std::endl;
}
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
    testLIFDynamics();
    testSynapticIntegration();
}

} // namespace test_neuron
