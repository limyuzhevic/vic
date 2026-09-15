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

void testBoundaryConditions() {
    nlm::Neuron neuron(nlm::NeuronId(2));
    
    // Test membrane potential bounds
    neuron.setMembranePotential(-200.0f);  // Below biological limit
    assert(neuron.getMembranePotential() <= -100.0f);
    
    neuron.setMembranePotential(100.0f);   // Above biological limit
    assert(neuron.getMembranePotential() <= 50.0f);
    
    // Test threshold bounds
    neuron.setThreshold(-120.0f);  // Very low threshold
    assert(neuron.getThreshold() >= -90.0f);
    
    neuron.setThreshold(20.0f);    // Very high threshold
    assert(neuron.getThreshold() <= 0.0f);
    
    // Test refractory period bounds
    neuron.setRefractoryPeriod(0);   // Zero refractory period
    assert(neuron.getRefractoryPeriod() >= 0);
    
    neuron.setRefractoryPeriod(5000); // Very long refractory period
    assert(neuron.getRefractoryPeriod() <= 1000);
    
    std::cout << "    testBoundaryConditions passed" << std::endl;
}

void testErrorHandling() {
    nlm::Neuron neuron(nlm::NeuronId(3));
    
    // Test negative current injection (should log warning but not crash)
    neuron.injectCurrent(-100.0f);
    assert(neuron.getTotalCurrent() == -100.0f);
    
    // Test very large current injection
    neuron.injectCurrent(1000.0f);
    assert(neuron.getTotalCurrent() >= -100.0f + 1000.0f);
    
    // Test negative input to synaptic functions
    neuron.receiveExcitatoryInput(-10.0f);
    neuron.receiveInhibitoryInput(-5.0f);  // Negative inhibitory input
    
    // Test spike history with invalid timestamp
    neuron.recordSpike(-1.0f);  // Negative timestamp
    assert(neuron.getSpikeHistory().size() == 1);
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void testRefractoryEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(4));
    
    // Test immediate refractory decrement
    neuron.setRefractoryPeriod(0);
    neuron.decrementRefractory();
    assert(!neuron.isRefractory());
    
    // Test full refractory cycle
    neuron.setRefractoryPeriod(3);
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 2; ++i) {
        neuron.decrementRefractory();
        assert(neuron.isRefractory());
    }
    
    neuron.decrementRefractory();
    assert(!neuron.isRefractory());
    
    // Test decrement when not refractory
    neuron.decrementRefractory();  // Should not crash
    assert(!neuron.isRefractory());
    
    std::cout << "    testRefractoryEdgeCases passed" << std::endl;
}

void testSTDPInteractions() {
    nlm::Neuron neuron(nlm::NeuronId(5));
    
    // Enable plasticity
    neuron.enablePlasticity(true, true, false);
    assert(neuron.getPlasticityFlags().hebbian);
    assert(neuron.getPlasticityFlags().stdp);
    assert(!neuron.getPlasticityFlags().reward_modulated);
    
    // Test plasticity state persistence
    neuron.setMembranePotential(-60.0f);
    neuron.recordSpike(0.1);
    
    assert(neuron.getLastSpikeTime() == 0.1);
    assert(neuron.getSpikeHistory().size() == 1);
    
    // Reset neuron and check state
    neuron.reset();
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testSTDPInteractions passed" << std::endl;
}

void testRandomInitializationRange() {
    nlm::RandomGenerator rng(42);
    
    // Test multiple neurons for statistical distribution
    std::vector<float> potentials;
    std::vector<float> thresholds;
    std::vector<float> restingPotentials;
    
    for (int i = 0; i < 100; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i));
        neuron.initializeRandom(rng);
        
        potentials.push_back(neuron.getMembranePotential());
        thresholds.push_back(neuron.getThreshold());
        restingPotentials.push_back(neuron.getRestingPotential());
    }
    
    // Check that all values are within reasonable biological ranges
    for (float pot : potentials) {
        assert(pot < -60.0f && pot > -80.0f);
    }
    
    for (float thresh : thresholds) {
        assert(thresh < -50.0f && thresh > -60.0f);
    }
    
    for (float rest : restingPotentials) {
        assert(rest < -50.0f && rest > -80.0f);
    }
    
    // Check refractory periods
    for (int i = 0; i < 100; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i + 1000));
        neuron.initializeRandom(rng);
        assert(neuron.getRefractoryPeriod() >= 2 && neuron.getRefractoryPeriod() <= 10);
    }
    
    std::cout << "    testRandomInitializationRange passed" << std::endl;
}

void testLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(6));
    
    // Set up neuron with specific parameters
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setRefractoryPeriod(5);
    neuron.setLeakConductance(10.0f);
    
    // Test step without spike
    neuron.step(0.0);
    assert(!neuron.isFiring());
    
    // Inject current to cause spike
    neuron.injectCurrent(5.0f);
    neuron.step(0.001);
    assert(neuron.isFiring());
    assert(neuron.isRefractory());
    
    // Simulate refractory period
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
        assert(neuron.isRefractory());
    }
    
    neuron.decrementRefractory();
    assert(!neuron.isRefractory());
    
    // Reset and check
    neuron.reset();
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    testLIFDynamics passed" << std::endl;
}

void testMemoryManagement() {
    // Test neuron move constructor
    nlm::Neuron neuron1(nlm::NeuronId(7));
    neuron1.setMembranePotential(-60.0f);
    
    nlm::Neuron neuron2(std::move(neuron1));
    assert(neuron2.getId() == nlm::NeuronId(7));
    assert(neuron2.getMembranePotential() == -60.0f);
    
    // neuron1 should be in valid but unspecified state after move
    // (test that it doesn't crash)
    
    // Test assignment operator
    nlm::Neuron neuron3(nlm::NeuronId(8));
    neuron3 = std::move(neuron2);
    assert(neuron3.getId() == nlm::NeuronId(7));
    
    std::cout << "    testMemoryManagement passed" << std::endl;
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
    testBoundaryConditions();
    testErrorHandling();
    testRefractoryEdgeCases();
    testSTDPInteractions();
    testRandomInitializationRange();
    testLIFDynamics();
    testMemoryManagement();
}

} // namespace test_neuron
