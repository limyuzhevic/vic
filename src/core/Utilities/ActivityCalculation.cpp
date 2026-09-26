// ============================================================================
// ACTIVITY CALCULATION UTILITIES IMPLEMENTATION
// ============================================================================

#include "Utilities.hpp"

namespace nlm {

// ============================================================================
// ACTIVITY CALCULATION UTILITIES IMPLEMENTATION
// ============================================================================

float calculateAverageActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (const Neuron* neuron : neurons) {
        if (neuron) {
            float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            sum += activity;
        }
    }
    return sum / neurons.size();
}

float calculateMaxActivity(const std::vector<Neuron*>& neurons) {
    float maxActivity = 0.0f;
    for (const Neuron* neuron : neurons) {
        if (neuron) {
            float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            if (activity > maxActivity) {
                maxActivity = activity;
            }
        }
    }
    return maxActivity;
}

float calculateWeightedActivity(const std::vector<Neuron*>& neurons, const std::vector<float>& weights) {
    if (neurons.empty() || neurons.size() != weights.size()) {
        throw std::invalid_argument("Invalid neuron or weight vector size");
    }
    
    float weightedSum = 0.0f;
    for (size_t i = 0; i < neurons.size(); ++i) {
        if (neurons[i]) {
            float activity = std::abs(neurons[i]->getState().membranePotential - neurons[i]->getState().restingPotential);
            weightedSum += activity * weights[i];
        }
    }
    return weightedSum;
}

} // namespace nlm
