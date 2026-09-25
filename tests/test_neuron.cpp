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

void testSpikeHistoryBufferManagement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Fill spike history to near capacity
    // Note: MAX_SPIKE_HISTORY = 100
    for (int i = 0; i < 95; ++i) {
        neuron.recordSpike(static_cast<float>(i));
    }
    
    assert(neuron.getSpikeHistory().size() == 95);
    
    // Add more spikes to test buffer management
    for (int i = 100; i < 150; ++i) {
        neuron.recordSpike(static_cast<float>(i));
    }
    
    // History should not exceed MAX_SPIKE_HISTORY
    assert(neuron.getSpikeHistory().size() == 100);
    
    // Check that oldest spikes were removed
    assert(neuron.getSpikeHistory().front() == 50.0f);
    
    std::cout << "    testSpikeHistoryBufferManagement passed" << std::endl;
}

void testThresholdEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test with very low threshold
    neuron.setThreshold(-100.0f);
    neuron.setMembranePotential(-95.0f);
    assert(neuron.checkThreshold());
    
    // Test with very high threshold
    neuron.setThreshold(20.0f);
    neuron.setMembranePotential(15.0f);
    assert(!neuron.checkThreshold());
    
    // Test exact threshold match
    neuron.setThreshold(0.0f);
    neuron.setMembranePotential(0.0f);
    assert(neuron.checkThreshold());
    
    std::cout << "    testThresholdEdgeCases passed" << std::endl;
}

void testRefractoryPeriodEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test zero refractory period
    neuron.setRefractoryPeriod(0);
    assert(!neuron.isRefractory());
    
    // Test very long refractory period
    neuron.setRefractoryPeriod(10000);
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isRefractory());
    
    // Test refractory decrement at boundary
    neuron.setRefractoryPeriod(1);
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isRefractory());
    neuron.decrementRefractory();
    assert(!neuron.isRefractory());
    
    std::cout << "    testRefractoryPeriodEdgeCases passed" << std::endl;
}

void testSynapticInputEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test large excitatory input
    neuron.receiveExcitatoryInput(10000.0f);
    // Note: total current is stored directly, no clamping in API
    
    // Test large inhibitory input
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(10000.0f);
    
    // Test modulatory input
    neuron.receiveModulatoryInput(1.0f);
    // Modulatory input affects adaptation variable
    
    std::cout << "    testSynapticInputEdgeCases passed" << std::endl;
}

void testMembranePotentialBoundaryConditions() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test extreme values
    neuron.setMembranePotential(100.0f);  // Above max
    assert(neuron.getMembranePotential() == 100.0f);
    
    neuron.setMembranePotential(-200.0f);  // Below min
    assert(neuron.getMembranePotential() == -200.0f);
    
    // Test resting potential boundary
    neuron.setRestingPotential(-50.0f);
    neuron.setMembranePotential(-45.0f);  // Above resting
    assert(neuron.getMembranePotential() == -45.0f);
    
    std::cout << "    testMembranePotentialBoundaryConditions passed" << std::endl;
}

void testStateTransitionEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test rapid state changes
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    neuron.setFiringState(nlm::FiringState::Refractory);
    assert(!neuron.isFiring());
    assert(neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Resting);
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    std::cout << "    testStateTransitionEdgeCases passed" << std::endl;
}

void testInitializationRandomization() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(43);
    nlm::RandomGenerator rng3(42);  // Same seed
    
    nlm::Neuron neuron1(nlm::NeuronId(1));
    nlm::Neuron neuron2(nlm::NeuronId(2));
    nlm::Neuron neuron3(nlm::NeuronId(3));
    
    neuron1.initializeRandom(rng1);
    neuron2.initializeRandom(rng2);
    neuron3.initializeRandom(rng3);
    
    // Neurons with different seeds should have different parameters
    assert(neuron1.getThreshold() != neuron2.getThreshold());
    
    // Neurons with same seed should have same parameters
    assert(neuron1.getThreshold() == neuron3.getThreshold());
    assert(neuron1.getRestingPotential() == neuron3.getRestingPotential());
    assert(neuron1.getRefractoryPeriod() == neuron3.getRefractoryPeriod());
    
    std::cout << "    testInitializationRandomization passed" << std::endl;
}

void testIntegrationStepEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test with zero time step
    neuron.setMembranePotential(-60.0f);
    neuron.stepLIF(0.0f, 0.0f);
    // Should not crash
    
    // Test with very large time step
    neuron.stepLIF(1.0f, 1.0f);
    // Should handle large time steps without crashing
    
    std::cout << "    testIntegrationStepEdgeCases passed" << std::endl;
}

void testClearOperations() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Fill spike history
    for (int i = 0; i < 10; ++i) {
        neuron.recordSpike(static_cast<float>(i));
    }
    
    // Add synapses
    neuron.addIncomingSynapse(1);
    neuron.addOutgoingSynapse(2);
    
    // Reset neuron
    neuron.reset();
    
    // Check that spike history is cleared
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testClearOperations passed" << std::endl;
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
    testSpikeHistoryBufferManagement();
    testThresholdEdgeCases();
    testRefractoryPeriodEdgeCases();
    testSynapticInputEdgeCases();
    testMembranePotentialBoundaryConditions();
    testStateTransitionEdgeCases();
    testInitializationRandomization();
    testIntegrationStepEdgeCases();
    testClearOperations();
}

} // namespace test_neuron
