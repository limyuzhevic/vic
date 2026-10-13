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

void testStepLIF() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set neuron to fire
    neuron.setMembranePotential(-50.0f);  // Above threshold (-55.0f)
    neuron.setThreshold(-55.0f);
    
    // Neuron should fire
    bool fired = neuron.stepLIF(0.0, 0.001);
    assert(fired);
    assert(neuron.isFiring());
    assert(neuron.isRefractory());
    
    // Reset for next test
    neuron.reset();
    neuron.setMembranePotential(-70.0f);
    
    // Neuron should not fire
    fired = neuron.stepLIF(0.0, 0.001);
    assert(!fired);
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    // Test with larger timestep
    neuron.reset();
    neuron.setMembranePotential(-70.0f);
    neuron.setLeakConductance(20.0f);  // Slower leak
    
    fired = neuron.stepLIF(0.0, 0.01);  // Larger timestep
    assert(!fired);  // Should not fire with larger timestep
    
    std::cout << "    testStepLIF passed" << std::endl;
}

void testDeprecatedStepMethod() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set neuron to fire
    neuron.setMembranePotential(-50.0f);
    neuron.setThreshold(-55.0f);
    
    // Deprecated step method should still work for backward compatibility
    neuron.step(0.0);
    
    assert(neuron.isFiring() || neuron.isRefractory());  // Either could be true after step
    
    std::cout << "    testDeprecatedStepMethod passed" << std::endl;
}

void testStepMethodTimestep() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test that deprecated step method uses default timestep
    neuron.reset();
    neuron.setMembranePotential(-50.0f);
    neuron.setThreshold(-55.0f);
    
    // This should use the default 1ms timestep internally
    neuron.step(0.0);
    
    // Verify neuron state changed (should have fired)
    assert(neuron.isFiring() || neuron.isRefractory());
    
    std::cout << "    testStepMethodTimestep passed" << std::endl;
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
    testStepLIF();
    testDeprecatedStepMethod();
    testStepMethodTimestep();
}

} // namespace test_neuron
