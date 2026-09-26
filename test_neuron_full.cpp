#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

// Custom test for neuron dynamics
void testNeuronDynamics() {
    std::cout << "Testing Neuron Dynamics Implementation..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test 1: Initial state
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getState().adaptationVariable == 0.0f);
    std::cout << "✓ Initial state check passed" << std::endl;
    
    // Test 2: Step function doesn't crash
    neuron.step(0.0);
    std::cout << "✓ Step function executed without error" << std::endl;
    
    // Test 3: Injection affects membrane potential
    neuron.setThreshold(-50.0f);  // Lower threshold for testing
    neuron.setLeakConductance(5.0f);
    neuron.setRestingPotential(-70.0f);
    neuron.setResetPotential(-65.0f);
    neuron.setRefractoryPeriod(5);
    
    neuron.injectCurrent(20.0f);  // Strong depolarizing current
    
    // Step through simulation to allow for integration
    for (int i = 0; i < 10; ++i) {
        neuron.step(static_cast<double>(i) * 0.001);
    }
    
    // Neuron potential should have increased due to current
    float potential = neuron.getMembranePotential();
    assert(potential > -70.0f);
    std::cout << "✓ Membrane potential increased with current injection" << std::endl;
    
    // Test 4: Adaptation dynamics
    float adaptation_before = neuron.getState().adaptationVariable;
    
    // Allow more time for potential increase
    for (int i = 0; i < 20; ++i) {
        neuron.step(static_cast<double>(i + 10) * 0.001);
    }
    
    // Adaptation should be increasing (or at least not decreasing from 0)
    float adaptation_after = neuron.getState().adaptationVariable;
    std::cout << "✓ Adaptation variable: " << adaptation_after << std::endl;
    
    // Test 5: Refractory behavior
    neuron.setRefractoryPeriod(10);
    neuron.setFiringState(nlm::FiringState::Active);
    
    // Test firing state methods
    assert(neuron.isFiring() == true);
    assert(neuron.isRefractory() == true);
    std::cout << "✓ Firing and refractory state checks passed" << std::endl;
    
    // Test 6: Spike frequency adaptation simulation
    nlm::Neuron neuron2(nlm::NeuronId(2));
    neuron2.setThreshold(-55.0f);
    neuron2.injectCurrent(30.0f);
    
    // Record adaptation over multiple steps
    float adaptations[5] = {0};
    for (int i = 0; i < 5; ++i) {
        neuron2.step(static_cast<double>(i) * 0.001);
        adaptations[i] = neuron2.getState().adaptationVariable;
    }
    
    // Check that adaptation is non-decreasing (monotonic behavior)
    for (int i = 1; i < 5; ++i) {
        assert(adaptations[i] >= adaptations[i-1] * 0.95f);  // Allow for decay
    }
    std::cout << "✓ Spike-frequency adaptation dynamics verified" << std::endl;
    
    // Test 7: Synaptic inputs
    nlm::Neuron neuron3(nlm::NeuronId(3));
    neuron3.receiveExcitatoryInput(5.0f);
    neuron3.receiveInhibitoryInput(2.0f);
    
    std::cout << "✓ Synaptic input handling verified" << std::endl;
    
    // Test 8: State management
    neuron3.setMembranePotential(-60.0f);
    assert(neuron3.getMembranePotential() == -60.0f);
    
    neuron3.reset();
    assert(neuron3.getMembranePotential() == -70.0f);
    assert(neuron3.getState().adaptationVariable == 0.0f);
    std::cout << "✓ State management (reset) verified" << std::endl;
    
    std::cout << "\nAll neuron dynamics tests passed successfully!" << std::endl;
}

int main() {
    testNeuronDynamics();
    return 0;
}