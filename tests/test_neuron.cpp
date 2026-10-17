// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>

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

// NEW COMPREHENSIVE TESTS

void testNeuronBoundaryConditions() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test extreme membrane potential values
    neuron.setMembranePotential(100.0f);  // Above normal clamp
    assert(neuron.getMembranePotential() == 100.0f);
    
    neuron.setMembranePotential(-200.0f); // Below normal clamp
    assert(neuron.getMembranePotential() == -200.0f);
    
    // Test refractory period boundaries
    neuron.setRefractoryPeriod(0);
    assert(!neuron.isRefractory());
    
    neuron.setRefractoryPeriod(UINT32_MAX);
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory(); // Should not crash
    
    // Test synaptic input boundaries
    neuron.clearTotalCurrent();
    neuron.receiveExcitatoryInput(0.0f); // Zero input
    assert(neuron.getTotalCurrent() == 0.0f);
    
    neuron.receiveInhibitoryInput(-100.0f); // Extreme negative
    assert(neuron.getTotalCurrent() == -100.0f);
    
    std::cout << "    testNeuronBoundaryConditions passed" << std::endl;
}

void testNeuronPlasticityFlags() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test plasticity flag defaults
    const auto& flags = neuron.getPlasticityFlags();
    assert(!flags.hebbian);
    assert(!flags.stdp);
    assert(!flags.reward_modulated);
    
    // Test enabling plasticity
    neuron.enablePlasticity(true, true, false);
    const auto& flagsAfter = neuron.getPlasticityFlags();
    assert(flagsAfter.hebbian);
    assert(flagsAfter.stdp);
    assert(!flagsAfter.reward_modulated);
    
    // Test disabling plasticity
    neuron.enablePlasticity(false, false, false);
    const auto& flagsDisabled = neuron.getPlasticityFlags();
    assert(!flagsDisabled.hebbian);
    assert(!flagsDisabled.stdp);
    assert(!flagsDisabled.reward_modulated);
    
    std::cout << "    testNeuronPlasticityFlags passed" << std::endl;
}

void testNeuronStateConsistency() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Record initial state
    auto initialState = neuron.getState();
    
    // Modify state
    neuron.setMembranePotential(-50.0f);
    neuron.setThreshold(-40.0f);
    neuron.setRefractoryPeriod(20);
    neuron.setFiringState(nlm::FiringState::Active);
    
    // Check consistency
    auto modifiedState = neuron.getState();
    assert(modifiedState.membranePotential == -50.0f);
    assert(modifiedState.threshold == -40.0f);
    assert(modifiedState.refractoryPeriod == 20);
    assert(modifiedState.firingState == nlm::FiringState::Active);
    
    // Reset and check consistency
    neuron.reset();
    auto resetState = neuron.getState();
    assert(resetState.membranePotential == -70.0f);  // Default
    assert(resetState.threshold == -55.0f);  // Default
    assert(resetState.refractoryPeriod == 5);  // Default
    assert(resetState.firingState == nlm::FiringState::Resting);  // Default
    
    std::cout << "    testNeuronStateConsistency passed" << std::endl;
}

void testNeuronSpikeHandling() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test spike recording with timestamps
    neuron.recordSpike(0.0);
    neuron.recordSpike(0.001);
    neuron.recordSpike(0.002);
    
    assert(neuron.getSpikeHistory().size() == 3);
    
    // Test spike history size limit (should not exceed 100)
    for (size_t i = 3; i < 150; ++i) {
        neuron.recordSpike(static_cast<float>(i) * 0.001f);
    }
    
    assert(neuron.getSpikeHistory().size() == 100);  // Should be clamped to max
    
    std::cout << "    testNeuronSpikeHandling passed" << std::endl;
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
    testNeuronBoundaryConditions();
    testNeuronPlasticityFlags();
    testNeuronStateConsistency();
    testNeuronSpikeHandling();
}

} // namespace test_neuron
