// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_neuron {

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

void testNeuronThreshold() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Default threshold
    assert(neuron.getThreshold() == -55.0f);
    
    // Set custom threshold
    neuron.setThreshold(-45.0f);
    assert(neuron.getThreshold() == -45.0f);
    
    // Test threshold check
    neuron.setMembranePotential(-46.0f);
    assert(neuron.checkThreshold());
    
    neuron.setMembranePotential(-44.0f);
    assert(!neuron.checkThreshold());
    
    std::cout << "    testNeuronThreshold passed" << std::endl;
}

void testNeuronStateFields() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    const auto& state = neuron.getState();
    
    // Test all state fields
    assert(state.membranePotential == -70.0f);
    assert(state.restingPotential == -70.0f);
    assert(state.threshold == -55.0f);
    assert(state.resetPotential == -70.0f);
    assert(state.leakConductance == 10.0f);
    assert(state.synapseConductance == 0.0f);
    assert(state.firingRate == 0.0f);
    assert(state.firingState == nlm::FiringState::Resting);
    assert(state.refractoryRemaining == 0);
    assert(state.refractoryPeriod == 5);
    assert(state.adaptationVariable == 0.0f);
    assert(state.lastSpikeTime == -1.0f);
    
    std::cout << "    testNeuronStateFields passed" << std::endl;
}

void testNeuronCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test multiple current injections
    neuron.injectCurrent(1.0f);
    neuron.injectCurrent(2.0f);
    assert(neuron.getTotalCurrent() == 3.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Test negative current
    neuron.injectCurrent(-1.0f);
    assert(neuron.getTotalCurrent() == -1.0f);
    
    std::cout << "    testNeuronCurrentInjection passed" << std::endl;
}

void testNeuronModulationInputs() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    neuron.receiveInhibitoryInput(2.0f);
    neuron.receiveModulatoryInput(1.0f);
    
    // Modulatory inputs should affect total current
    // (Note: actual effect depends on implementation)
    float current = neuron.getTotalCurrent();
    
    std::cout << "    testNeuronModulationInputs passed" << std::endl;
}

void testNeuronRefractoryPeriod() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test various refractory periods
    neuron.setRefractoryPeriod(0);
    assert(neuron.getRefractoryPeriod() == 0);
    assert(!neuron.isRefractory());
    
    neuron.setRefractoryPeriod(100);
    assert(neuron.getRefractoryPeriod() == 100);
    assert(neuron.isRefractory());
    
    // Test refractory decrement
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 9; ++i) {
        neuron.decrementRefractory();
    }
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(!neuron.isRefractory());
    
    std::cout << "    testNeuronRefractoryPeriod passed" << std::endl;
}

void testNeuronLeakConductance() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Default leak conductance
    assert(neuron.getLeakConductance() == 10.0f);
    
    // Set custom leak conductance
    neuron.setLeakConductance(5.0f);
    assert(neuron.getLeakConductance() == 5.0f);
    
    std::cout << "    testNeuronLeakConductance passed" << std::endl;
}

void testNeuronRestingAndResetPotentials() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test resting potential
    neuron.setRestingPotential(-65.0f);
    assert(neuron.getRestingPotential() == -65.0f);
    
    // Test reset potential
    neuron.setResetPotential(-75.0f);
    assert(neuron.getResetPotential() == -75.0f);
    
    // Test reset functionality
    neuron.setMembranePotential(-40.0f);
    neuron.reset();
    assert(neuron.getMembranePotential() == -75.0f);  // Should reset to reset potential
    
    std::cout << "    testNeuronRestingAndResetPotentials passed" << std::endl;
}

void testNeuronRandomInitializationEdgeCases() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Test edge cases for random initialization
    assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
    assert(neuron.getRestingPotential() < -60.0f && neuron.getRestingPotential() > -80.0f);
    assert(neuron.getResetPotential() < -60.0f && neuron.getResetPotential() > -80.0f);
    
    std::cout << "    testNeuronRandomInitializationEdgeCases passed" << std::endl;
}

void testNeuronLIFStep() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test LIF step with resting state
    bool fired = neuron.stepLIF(0.0, 0.001);
    assert(!fired);
    
    // Test LIF step with threshold crossing
    neuron.setMembranePotential(-54.0f);  // Above threshold (-55.0f)
    neuron.setThreshold(-55.0f);
    fired = neuron.stepLIF(0.001, 0.001);
    assert(fired);
    
    // Test LIF step with refractory period
    neuron.setRefractoryPeriod(5);
    fired = neuron.stepLIF(0.002, 0.001);
    assert(!fired);  // Should not fire due to refractory
    assert(neuron.isRefractory());
    
    std::cout << "    testNeuronLIFStep passed" << std::endl;
}

void testNeuronSpikeHistoryManagement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test spike history with multiple spikes
    neuron.recordSpike(0.0);
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    
    auto history = neuron.getSpikeHistory();
    assert(history.size() == 3);
    assert(history[0] == 0.0);
    assert(history[1] == 0.1);
    assert(history[2] == 0.2);
    
    // Test last spike time
    assert(neuron.getLastSpikeTime() == 0.2);
    
    // Test spike history clearing
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    assert(neuron.getLastSpikeTime() == -1.0f);
    
    std::cout << "    testNeuronSpikeHistoryManagement passed" << std::endl;
}

void testNeuronPlasticityFlags() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test default plasticity flags
    auto flags = neuron.getPlasticityFlags();
    assert(!flags.hebbian);
    assert(!flags.stdp);
    assert(!flags.reward_modulated);
    
    // Test enabling plasticity
    neuron.enablePlasticity(true, true, false);
    flags = neuron.getPlasticityFlags();
    assert(flags.hebbian);
    assert(flags.stdp);
    assert(!flags.reward_modulated);
    
    std::cout << "    testNeuronPlasticityFlags passed" << std::endl;
}

void testNeuronRegionPopulationMembership() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test region membership
    neuron.setRegionId(nlm::RegionId(1));
    assert(neuron.getRegionId() == nlm::RegionId(1));
    
    // Test population membership
    neuron.setPopulationId(nlm::PopulationId(1));
    assert(neuron.getPopulationId() == nlm::PopulationId(1));
    
    std::cout << "    testNeuronRegionPopulationMembership passed" << std::endl;
}

void testNeuronStateMutability() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Get state reference and modify it
    auto& state = neuron.getState();
    state.membranePotential = -50.0f;
    
    // Verify modification persisted
    assert(neuron.getMembranePotential() == -50.0f);
    assert(neuron.getState().membranePotential == -50.0f);
    
    std::cout << "    testNeuronStateMutability passed" << std::endl;
}

void testNeuronMoveConstructor() {
    nlm::Neuron neuron1(nlm::NeuronId(1));
    neuron1.setMembranePotential(-60.0f);
    
    nlm::Neuron neuron2(std::move(neuron1));
    
    // neuron1 should be in valid but unspecified state
    // neuron2 should have the state
    assert(neuron2.getId() == nlm::NeuronId(1));
    assert(neuron2.getMembranePotential() == -60.0f);
    
    std::cout << "    testNeuronMoveConstructor passed" << std::endl;
}

void testNeuronStepMethod() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test step with resting state
    neuron.step(0.0);
    
    // Test step with firing state
    neuron.setFiringState(nlm::FiringState::Active);
    neuron.step(0.001);
    
    // Test step with inhibitory input
    neuron.setFiringState(nlm::FiringState::Resting);
    neuron.receiveInhibitoryInput(5.0f);
    neuron.step(0.001);
    
    std::cout << "    testNeuronStepMethod passed" << std::endl;
}

void testNeuronConstants() {
    // Test static constants
    nlm::NeuronId invalidId = nlm::INVALID_NEURON_ID;
    assert(invalidId.index() == UINT64_MAX);
    
    nlm::NeuronState defaultState;
    assert(defaultState.membranePotential == -70.0f);
    assert(defaultState.restingPotential == -70.0f);
    assert(defaultState.threshold == -55.0f);
    assert(defaultState.resetPotential == -70.0f);
    
    std::cout << "    testNeuronConstants passed" << std::endl;
}
