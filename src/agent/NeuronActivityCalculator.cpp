#include "NeuronActivityCalculator.hpp"
#include <cmath>

namespace nlm {

float NeuronActivityCalculator::calculateActivity(const Neuron& neuron) {
    return std::abs(neuron.getState().membranePotential - neuron.getState().restingPotential);
}

float NeuronActivityCalculator::calculatePopulationActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

std::vector<float> NeuronActivityCalculator::calculateAllActivities(const std::vector<Neuron*>& neurons) {
    std::vector<float> activities;
    activities.reserve(neurons.size());
    for (Neuron* n : neurons) {
        activities.push_back(std::abs(n->getState().membranePotential - n->getState().restingPotential));
    }
    return activities;
}

} // namespace nlm