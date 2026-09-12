#pragma once

#include "../brain/Neuron.hpp"
#include <vector>

namespace nlm {

struct CurrentInjector {
    static void injectToNeuron(Neuron* neuron, float current);
    static void injectToNeurons(const std::vector<Neuron*>& neurons, 
                                const std::vector<float>& values,
                                float scaleFactor = 1.0f);
    static void injectToPopulation(const std::vector<Neuron*>& neurons,
                                   float current);
};

} // namespace nlm