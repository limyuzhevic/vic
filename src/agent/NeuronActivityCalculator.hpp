#pragma once

#include "../brain/Neuron.hpp"
#include <vector>

namespace nlm {

struct NeuronActivityCalculator {
    static float calculateActivity(const Neuron& neuron);
    static float calculatePopulationActivity(const std::vector<Neuron*>& neurons);
    static std::vector<float> calculateAllActivities(const std::vector<Neuron*>& neurons);
};

} // namespace nlm