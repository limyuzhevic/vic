#include "CurrentInjector.hpp"
#include <algorithm>

namespace nlm {

void CurrentInjector::injectToNeuron(Neuron* neuron, float current) {
    if (neuron) {
        neuron->injectCurrent(current);
    }
}

void CurrentInjector::injectToNeurons(const std::vector<Neuron*>& neurons, 
                                      const std::vector<float>& values,
                                      float scaleFactor) {
    size_t count = std::min(neurons.size(), values.size());
    for (size_t i = 0; i < count; ++i) {
        if (neurons[i]) {
            float current = values[i] * scaleFactor;
            neurons[i]->injectCurrent(current);
        }
    }
}

void CurrentInjector::injectToPopulation(const std::vector<Neuron*>& neurons,
                                         float current) {
    for (Neuron* neuron : neurons) {
        if (neuron) {
            neuron->injectCurrent(current);
        }
    }
}

} // namespace nlm