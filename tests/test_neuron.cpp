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

void testNeuronValidation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test initial state validation
    assert(neuron.validateState());
    
    // Test membrane potential validation
    neuron.setMembranePotential(-80.0f); // Valid: -100 <= -80 <= 50
    assert(neuron.validateMembranePotential(-80.0f));
    
    neuron.setMembranePotential(60.0f);  // Invalid: > 50
    assert(!neuron.validateMembranePotential(60.0f));
    
    neuron.setMembranePotential(-110.0f); // Invalid: < -100
    assert(!neuron.validateMembranePotential(-110.0f));
    
    // Test threshold validation (should be > resting potential)
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-60.0f);  // Valid: -60 > -70
    assert(neuron.validateThreshold(-60.0f));
    
    neuron.setThreshold(-75.0f);   // Invalid: -75 <= -70
    assert(!neuron.validateThreshold(-75.0f));
    
    // Test refractory period validation
    neuron.setRefractoryPeriod(10);   // Valid: 0 <= 10 <= 1000
    assert(neuron.validateRefractoryPeriod(10));
    
    neuron.setRefractoryPeriod(-5);   // Invalid: < 0
    assert(!neuron.validateRefractoryPeriod(-5));
    
    neuron.setRefractoryPeriod(2000); // Invalid: > 1000
    assert(!neuron.validateRefractoryPeriod(2000));
    
    // Test adaptation variable validation
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-60.0f);
    
    // Test leak conductance validation
    neuron.setLeakConductance(10.0f);   // Valid: 0 <= 10 <= 100
    assert(neuron.validateLeakConductance(10.0f));
    
    neuron.setLeakConductance(-5.0f);   // Invalid: < 0
    assert(!neuron.validateLeakConductance(-5.0f));
    
    neuron.setLeakConductance(150.0f);  // Invalid: > 100
    assert(!neuron.validateLeakConductance(150.0f));
    
    // Test neuron ID validation
    assert(neuron.validateNeuronId());
    
    std::cout << "    testNeuronValidation passed" << std::endl;
}

void testNeuronValidationClamp() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test that setters clamp invalid values
    neuron.setMembranePotential(60.0f);  // Should clamp to 50.0f
    assert(neuron.getMembranePotential() == 50.0f);
    
    neuron.setMembranePotential(-110.0f); // Should clamp to -100.0f
    assert(neuron.getMembranePotential() == -100.0f);
    
    neuron.setThreshold(-80.0f);   // Should be > resting potential (-70)
    // Since threshold was -60 before, it should be valid
    assert(neuron.getThreshold() == -60.0f); // Unchanged due to validation
    
    neuron.setRefractoryPeriod(-5);   // Should clamp to 0
    assert(neuron.getRefractoryPeriod() == 0);
    
    neuron.setRefractoryPeriod(2000); // Should clamp to 1000
    assert(neuron.getRefractoryPeriod() == 1000);
    
    neuron.setLeakConductance(-5.0f);   // Should clamp to 0
    assert(neuron.getLeakConductance() == 0.0f);
    
    neuron.setLeakConductance(150.0f);  // Should clamp to 100
    assert(neuron.getLeakConductance() == 100.0f);
    
    std::cout << "    testNeuronValidationClamp passed" << std::endl;
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
    testNeuronValidation();
    testNeuronValidationClamp();
}

} // namespace test_neuron
