// Enhanced Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_enhanced_neuron {

void testNeuronLIFDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up neuron for LIF simulation
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-75.0f);
    neuron.setLeakConductance(10.0f);
    neuron.setRefractoryPeriod(5);
    
    // Inject current to make neuron fire
    neuron.injectCurrent(20.0f);
    
    // Run LIF step
    bool fired = neuron.stepLIF(0.0f, 0.001f);
    
    // Should have fired with strong current
    assert(fired);
    assert(neuron.isFiring());
    assert(neuron.isRefractory());
    
    std::cout << "    testNeuronLIFDynamics passed" << std::endl;
}

void testNeuronStateManagement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Get initial state
    const auto& initialState = neuron.getState();
    assert(initialState.membranePotential == -70.0f);
    assert(initialState.threshold == -55.0f);
    assert(initialState.firingState == nlm::FiringState::Resting);
    
    // Modify state
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-50.0f);
    neuron.setFiringState(nlm::FiringState::Active);
    
    // Verify modifications
    const auto& modifiedState = neuron.getState();
    assert(modifiedState.membranePotential == -65.0f);
    assert(modifiedState.threshold == -50.0f);
    assert(modifiedState.firingState == nlm::FiringState::Active);
    
    std::cout << "    testNeuronStateManagement passed" << std::endl;
}

void testNeuronPlasticityFlags() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initial state should have plasticity disabled by default
    const auto& plasticityFlags = neuron.getPlasticityFlags();
    assert(!plasticityFlags.hebbian);
    assert(!plasticityFlags.stdp);
    assert(!plasticityFlags.reward_modulated);
    
    // Enable plasticity
    neuron.enablePlasticity(true, true, true);
    
    const auto& enabledPlasticity = neuron.getPlasticityFlags();
    assert(enabledPlasticity.hebbian);
    assert(enabledPlasticity.stdp);
    assert(enabledPlasticity.reward_modulated);
    
    // Partial enable
    neuron.enablePlasticity(false, true, false);
    
    const auto& partialPlasticity = neuron.getPlasticityFlags();
    assert(!partialPlasticity.hebbian);
    assert(partialPlasticity.stdp);
    assert(!partialPlasticity.reward_modulated);
    
    std::cout << "    testNeuronPlasticityFlags passed" << std::endl;
}

void testNeuronSynapticInputs() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Clear any existing current
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Test excitatory input
    neuron.receiveExcitatoryInput(10.0f);
    assert(neuron.getTotalCurrent() == 10.0f);
    
    // Test inhibitory input
    neuron.receiveInhibitoryInput(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);  // 10 - 5 = 5
    
    // Test modulatory input
    neuron.clearTotalCurrent();
    neuron.receiveModulatoryInput(3.0f);
    // Modulatory input affects adaptation variable, not total current
    // So total current should remain 0
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Clear current
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    testNeuronSynapticInputs passed" << std::endl;
}

void testNeuronSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initially empty
    assert(neuron.getSpikeHistory().empty());
    
    // Record some spikes
    neuron.recordSpike(0.0f);
    neuron.recordSpike(0.001f);
    neuron.recordSpike(0.002f);
    
    const auto& spikeHistory = neuron.getSpikeHistory();
    assert(spikeHistory.size() == 3);
    assert(spikeHistory[0] == 0.0f);
    assert(spikeHistory[1] == 0.001f);
    assert(spikeHistory[2] == 0.002f);
    
    // Clear history
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    // Test history limit (should auto-manage)
    for (int i = 0; i < 150; ++i) {
        neuron.recordSpike(static_cast<float>(i) * 0.001f);
    }
    
    // History should not exceed max (100)
    assert(neuron.getSpikeHistory().size() <= 100);
    
    std::cout << "    testNeuronSpikeHistory passed" << std::endl;
}

void testNeuronRegionPopulation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set region and population
    neuron.setRegionId(nlm::RegionId(1));
    neuron.setPopulationId(nlm::PopulationId(2));
    
    // Verify
    assert(neuron.getRegionId() == nlm::RegionId(1));
    assert(neuron.getPopulationId() == nlm::PopulationId(2));
    
    std::cout << "    testNeuronRegionPopulation passed" << std::endl;
}

void testNeuronRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initialize with random parameters
    neuron.initializeRandom(rng);
    
    // Check that values are in reasonable ranges
    const auto& state = neuron.getState();
    
    // Membrane potential should be near resting (-70mV)
    assert(state.membranePotential > -80.0f && state.membranePotential < -60.0f);
    
    // Threshold should be between -60 and -50 mV
    assert(state.threshold > -60.0f && state.threshold < -50.0f);
    
    // Refractory period should be between 2 and 10
    assert(state.refractoryPeriod > 1 && state.refractoryPeriod <= 10);
    
    // Adaptation variable should be initialized
    assert(state.adaptationVariable >= 0.0f);
    
    std::cout << "    testNeuronRandomInitialization passed" << std::endl;
}

void testNeuronReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Modify neuron state
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(10.0f);
    neuron.recordSpike(0.1f);
    neuron.setFiringState(nlm::FiringState::Active);
    
    // Reset neuron
    neuron.reset();
    
    // Verify reset state
    assert(neuron.getMembranePotential() == -70.0f);  // Default resting potential
    assert(neuron.getTotalCurrent() == 0.0f);  // Current should be cleared
    assert(neuron.getSpikeHistory().empty());  // History should be cleared
    assert(neuron.getState().firingState == nlm::FiringState::Resting);
    
    std::cout << "    testNeuronReset passed" << std::endl;
}

void testNeuronMultipleInputTypes() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Clear state
    neuron.clearTotalCurrent();
    neuron.clearSpikeHistory();
    
    // Apply multiple types of inputs
    neuron.receiveExcitatoryInput(5.0f);
    neuron.receiveInhibitoryInput(2.0f);
    neuron.receiveModulatoryInput(1.0f);
    neuron.injectCurrent(3.0f);
    
    // Verify final current (excitatory - inhibitory + injected)
    // Note: modulatory doesn't add to total current
    float expected_current = 5.0f - 2.0f + 3.0f;  // = 6.0f
    assert(std::abs(neuron.getTotalCurrent() - expected_current) < 0.001f);
    
    // Record multiple spikes
    neuron.recordSpike(0.0f);
    neuron.recordSpike(0.001f);
    
    const auto& history = neuron.getSpikeHistory();
    assert(history.size() == 2);
    assert(history[0] == 0.0f);
    assert(history[1] == 0.001f);
    
    std::cout << "    testNeuronMultipleInputTypes passed" << std::endl;
}

void testNeuronStateConsistency() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test that state access is consistent
    const auto& state1 = neuron.getState();
    const auto& state2 = neuron.getState();
    
    // Should be same object
    assert(&state1 == &state2);
    
    // Modify via non-const access
    nlm::NeuronState& state = neuron.getState();
    state.membranePotential = -65.0f;
    
    // Verify change
    assert(neuron.getMembranePotential() == -65.0f);
    
    std::cout << "    testNeuronStateConsistency passed" << std::endl;
}

void runAllEnhanced() {
    testNeuronLIFDynamics();
    testNeuronStateManagement();
    testNeuronPlasticityFlags();
    testNeuronSynapticInputs();
    testNeuronSpikeHistory();
    testNeuronRegionPopulation();
    testNeuronRandomInitialization();
    testNeuronReset();
    testNeuronMultipleInputTypes();
    testNeuronStateConsistency();
}

} // namespace test_enhanced_neuron
