// Advanced Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_neuron_advanced {

void testNeuronStep() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initialize neuron with random parameters
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    // Set neuron to be close to threshold
    neuron.setMembranePotential(-55.0f);  // Near threshold
    neuron.setRefractoryPeriod(0);  // Not refractory
    
    // Step neuron (should not fire since it's below threshold)
    bool fired = neuron.stepLIF(0.0, 0.001);
    assert(!fired);
    
    // Set neuron above threshold
    neuron.setMembranePotential(-50.0f);  // Above threshold
    neuron.setRefractoryPeriod(0);
    
    // Step neuron (should fire)
    fired = neuron.stepLIF(0.0, 0.001);
    assert(fired);
    
    // Neuron should now be in refractory period
    assert(neuron.isRefractory());
    
    // Step again (should not fire due to refractory)
    fired = neuron.stepLIF(0.0, 0.001);
    assert(!fired);
    
    std::cout << "    testNeuronStep passed" << std::endl;
}

void testNeuronMembranePotentialIntegration() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set up neuron parameters
    neuron.setMembranePotential(-70.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setLeakConductance(10.0f);
    neuron.setRefractoryPeriod(5);
    
    // Inject current
    neuron.injectCurrent(10.0f);
    
    // Step neuron
    bool fired = neuron.stepLIF(0.0, 0.001);
    assert(!fired);  // Should not fire yet
    
    // Membrane potential should have increased
    assert(neuron.getMembranePotential() > -70.0f);
    
    std::cout << "    testNeuronMembranePotentialIntegration passed" << std::endl;
}

void testNeuronFiringRate() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initially no firing rate
    assert(neuron.getFiringRate() == 0.0f);
    
    // Set firing rate
    neuron.setFiringRate(10.0f);
    assert(neuron.getFiringRate() == 10.0f);
    
    // Record some spikes
    neuron.recordSpike(0.0);
    neuron.recordSpike(1.0);
    
    // Firing rate should have changed (implementation dependent)
    
    std::cout << "    testNeuronFiringRate passed" << std::endl;
}

void testNeuronRefractory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Not refractory initially
    assert(!neuron.isRefractory());
    assert(!neuron.isFiring());
    
    // Set refractory period
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    
    // Decrement refractory period
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    // Decrement more
    for (int i = 0; i < 8; ++i) {
        neuron.decrementRefractory();
    }
    assert(!neuron.isRefractory());
    
    std::cout << "    testNeuronRefractory passed" << std::endl;
}

void testNeuronCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initially no current
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Inject current
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    // Clear current
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Add to current
    neuron.injectCurrent(3.0f);
    neuron.injectCurrent(2.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    std::cout << "    testNeuronCurrentInjection passed" << std::endl;
}

void testNeuronSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Initially no current
    assert(neuron.getTotalCurrent() == 0.0f);
    
    // Receive excitatory input
    neuron.receiveExcitatoryInput(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    // Clear current and receive inhibitory input
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    assert(neuron.getTotalCurrent() == -3.0f);
    
    // Clear current and receive modulatory input
    neuron.clearTotalCurrent();
    neuron.receiveModulatoryInput(2.0f);
    assert(neuron.getTotalCurrent() == 0.0f);  // Modulatory doesn't affect total current
    assert(neuron.getState().adaptationVariable > 0.0f);  // But affects adaptation variable
    
    std::cout << "    testNeuronSynapticInput passed" << std::endl;
}

void testNeuronStateReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Modify neuron state
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.0);
    
    // Reset neuron
    neuron.reset();
    
    // State should be back to initial values
    assert(neuron.getMembranePotential() == -70.0f);  // Default initial value
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
    assert(neuron.getState().firingState == nlm::FiringState::Resting);
    
    std::cout << "    testNeuronStateReset passed" << std::endl;
}

void runAll() {
    testNeuronStep();
    testNeuronMembranePotentialIntegration();
    testNeuronFiringRate();
    testNeuronRefractory();
    testNeuronCurrentInjection();
    testNeuronSynapticInput();
    testNeuronStateReset();
}

} // namespace test_neuron_advanced
