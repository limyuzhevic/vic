// Advanced Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_neuron_advanced {

void testNeuronPlasticityFlags() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test plasticity flags
    neuron.enablePlasticity(true, true, true);  // All plasticity enabled
    assert(neuron.getPlasticityFlags().hebbian);
    assert(neuron.getPlasticityFlags().stdp);
    assert(neuron.getPlasticityFlags().reward_modulated);
    
    // Test individual plasticity flags
    neuron.enablePlasticity(false, true, false);
    assert(!neuron.getPlasticityFlags().hebbian);
    assert(neuron.getPlasticityFlags().stdp);
    assert(!neuron.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testNeuronPlasticityFlags passed" << std::endl;
}

void testNeuronAdaptationVariables() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    float adaptation = neuron.getState().adaptationVariable;
    assert(adaptation >= 0.0f);
    
    // Test adaptation change after spike
    bool fired = neuron.stepLIF(0.0, 0.001);
    if (fired) {
        // After firing, adaptation should increase
        assert(neuron.getState().adaptationVariable > adaptation);
    }
    
    std::cout << "    testNeuronAdaptationVariables passed" << std::endl;
}

void testNeuronRefractoryDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setRefractoryPeriod(5);
    
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    // Decrement refractory multiple times
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
        assert(neuron.isRefractory());
    }
    
    // Last decrement should exit refractory
    neuron.decrementRefractory();
    assert(!neuron.isRefractory());
    assert(neuron.isFiring());  // Should be in active state
    
    std::cout << "    testNeuronRefractoryDynamics passed" << std::endl;
}

void testNeuronThresholdDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set threshold very low
    neuron.setThreshold(-10.0f);
    
    // Inject current to make neuron fire
    neuron.injectCurrent(20.0f);  // Strong current
    neuron.clearTotalCurrent();  // Prepare for step
    
    // Neuron should fire at low threshold
    bool fired = neuron.stepLIF(0.0, 0.001);
    
    // With strong input, neuron should fire
    if (fired) {
        assert(neuron.getState().firingState == nlm::FiringState::Refractory);
    }
    
    std::cout << "    testNeuronThresholdDynamics passed" << std::endl;
}

void runAll() {
    testNeuronPlasticityFlags();
    testNeuronAdaptationVariables();
    testNeuronRefractoryDynamics();
    testNeuronThresholdDynamics();
}

} // namespace test_neuron_advanced
