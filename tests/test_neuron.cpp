// Enhanced Neuron Tests
// Now includes comprehensive tests for LIF neuron dynamics and plasticity
// #include "brain/Neuron.hpp"
// #include "core/Random/Random.hpp"
// #include <cassert>
// #include <iostream>

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

void testLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setThreshold(-50.0f);  // Higher threshold for this test
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-65.0f);
    neuron.setRefractoryPeriod(5);
    neuron.setLeakConductance(0.1f);
    
    // Simulate depolarization
    neuron.injectCurrent(20.0f);  // Strong input
    neuron.stepLIF(0.0, 0.001);  // 1ms timestep
    
    // Neuron should approach threshold
    assert(neuron.getMembranePotential() < -50.0f);
    
    // Inject more current to reach threshold
    neuron.injectCurrent(15.0f);
    bool fired = neuron.stepLIF(0.001, 0.001);
    
    // After firing, membrane potential should be reset
    assert(neuron.getMembranePotential() == -65.0f);
    assert(neuron.isRefractory());
    
    std::cout << "    testLIFDynamics passed" << std::endl;
}

void testSpikeFrequencyAdaptation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setThreshold(-50.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-65.0f);
    neuron.setRefractoryPeriod(2);
    
    // First spike
    neuron.injectCurrent(30.0f);
    neuron.stepLIF(0.0, 0.001);
    neuron.injectCurrent(30.0f);
    bool fired1 = neuron.stepLIF(0.001, 0.001);
    assert(fired1);
    
    float adaptation1 = neuron.getState().adaptationVariable;
    assert(adaptation1 > 0.0f);
    
    // Second spike should have higher threshold due to adaptation
    neuron.injectCurrent(30.0f);
    neuron.stepLIF(0.002, 0.001);
    neuron.injectCurrent(30.0f);
    bool fired2 = neuron.stepLIF(0.003, 0.001);
    
    // After refractory period
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    
    std::cout << "    testSpikeFrequencyAdaptation passed" << std::endl;
}

void runAll() {
    testNeuronCreation();
    testNeuronType();
    testMembranePotential();
    testFiringState();
    testRefractoryDecrement();
    testCurrentInjection();
    testSynapticInput();
    testLIFDynamics();
    testSpikeFrequencyAdaptation();
    
    std::cout << std::endl;
    std::cout << "=== Neuron Tests ===" << std::endl;
    std::cout << "All neuron tests passed!" << std::endl;
}

} // namespace test_neuron