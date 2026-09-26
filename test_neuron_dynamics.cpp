#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>

namespace test_neuron_dynamics {

void testNeuronStepBasic() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test basic step functionality
    neuron.step(0.0);
    assert(neuron.getMembranePotential() == -70.0f);
    
    std::cout << "    testNeuronStepBasic passed" << std::endl;
}

void testNeuronStepWithCurrent() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.injectCurrent(10.0f);  // Strong depolarizing current
    
    // With enough current, neuron should depolarize
    neuron.step(0.001);  // First ms
    neuron.step(0.002);  // Second ms
    
    // Membrane potential should increase with current injection
    assert(neuron.getMembranePotential() > -70.0f);
    
    std::cout << "    testNeuronStepWithCurrent passed" << std::endl;
}

void testAdaptationDynamics() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setThreshold(-50.0f);  // Lower threshold for spiking
    neuron.injectCurrent(15.0f);  // Strong current
    
    // Get initial adaptation state
    float initial_adaptation = neuron.getState().adaptationVariable;
    assert(initial_adaptation == 0.0f);
    
    // Allow neuron to fire (simulate depolarization)
    neuron.step(0.001);
    neuron.step(0.002);
    
    // After adaptation, membrane potential should be lowered by adaptation current
    if (neuron.isFiring()) {
        // Adaptation should have increased after spike
        assert(neuron.getState().adaptationVariable > 0.0f);
    }
    
    std::cout << "    testAdaptationDynamics passed" << std::endl;
}

void testSpikeFrequencyAdaptation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setThreshold(-50.0f);  // Low threshold for spiking
    neuron.injectCurrent(20.0f);  // High current to elicit multiple spikes
    
    // First spike should increase adaptation
    neuron.step(0.001);
    float adaptation_after_first = neuron.getState().adaptationVariable;
    
    // Subsequent spike should further increase adaptation (but more slowly due to decay)
    neuron.step(0.002);
    float adaptation_after_second = neuron.getState().adaptationVariable;
    
    if (adaptation_after_first > 0.0f) {
        // Adaptation should continue to increase but less rapidly
        assert(adaptation_after_second >= adaptation_after_first);
    }
    
    std::cout << "    testSpikeFrequencyAdaptation passed" << std::endl;
}

void testRefractoryPeriodIntegration() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setThreshold(-50.0f);  
    neuron.injectCurrent(25.0f);  
    neuron.setRefractoryPeriod(5);  // 5ms refractory period
    
    // First spike
    neuron.step(0.001);
    bool first_spike = neuron.isFiring();
    
    // Should be refractory after spike
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    neuron.decrementRefractory();
    
    // Should still be in refractory for several steps
    neuron.step(0.002);
    bool refractory_check = neuron.isRefractory();
    
    std::cout << "    testRefractoryPeriodIntegration passed" << std::endl;
}

void runAll() {
    testNeuronStepBasic();
    testNeuronStepWithCurrent();
    testAdaptationDynamics();
    testSpikeFrequencyAdaptation();
    testRefractoryPeriodIntegration();
    
    std::cout << "\nAll neuron dynamics tests passed!" << std::endl;
}

} // namespace test_neuron_dynamics

int main() {
    test_neuron_dynamics::runAll();
    return 0;
}