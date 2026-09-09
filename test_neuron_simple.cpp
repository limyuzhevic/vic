#include "src/brain/Neuron.hpp"
#include "src/core/Random/Random.hpp"
#include <iostream>

int main() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.initializeRandom(nlm::RandomGenerator(42));
    
    std::cout << "Neuron ID: " << neuron.getId().value << std::endl;
    std::cout << "Membrane potential: " << neuron.getMembranePotential() << " mV" << std::endl;
    std::cout << "Threshold: " << neuron.getThreshold() << " mV" << std::endl;
    std::cout << "Refractory period: " << neuron.getRefractoryPeriod() << " steps" << std::endl;
    
    // Test step with no input - should not fire
    neuron.step(0.0);
    std::cout << "After step: " << neuron.getMembranePotential() << " mV" << std::endl;
    
    // Inject current to cause firing
    neuron.injectCurrent(10.0f);
    neuron.step(0.001);
    std::cout << "After injection and step: " << neuron.getMembranePotential() << " mV" << std::endl;
    
    return 0;
}