// Implementation of AntiHebbianRule
// Anti-Hebbian plasticity: decreases synaptic weight when neurons fire together

#include "AntiHebbianRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;      // Learning rate for weight depression
    float minWeight;         // Minimum synaptic weight
    float maxWeight;         // Maximum synaptic weight (usually 0 for anti-Hebbian)
    float targetWeight;      // Target weight for depression
    
    Impl() : learningRate(0.01f), minWeight(-1.0f), maxWeight(0.0f), targetWeight(0.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    /*
     * Anti-Hebbian learning implementation
     * 
     * Mathematical formulation:
     * Δw = -η * f(x_pre, x_post) when x_pre AND x_post are active
     * 
     * Where:
     *   η = learningRate (negative learning)
     *   f = correlation function (usually 1.0 when both pre and post spike)
     * 
     * Biological inspiration:
     *   - Homosynaptic depression
     *   - BCM-like homeostatic scaling
     *   - Predictive coding frameworks
     *   - Prevents runaway excitation
     *   
     * This implements "neurons that fire together, disconnect"
     * Helps maintain stability in neural networks
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (simplified anti-correlation)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 20.0) {  // 20ms window for coincidence detection
                ++coincidences;
            }
        }
    }
    
    // Apply weight change (negative - depression)
    if (coincidences > 0) {
        float delta = -pImpl->learningRate * static_cast<float>(coincidences);
        
        // Apply with bounds and target
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + delta;
        
        // If target is specified, approach it
        if (pImpl->targetWeight > -1.0f) {
            float distance = pImpl->targetWeight - newWeight;
            newWeight += 0.1f * distance;  // Move toward target
        }
        
        // Clamp to bounds
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        
        if (std::abs(newWeight - currentWeight) > 1e-6f) {
            synapse->setWeight(newWeight);
        }
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Anti-Hebbian applies negative weight changes
    // Clamp to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setMinWeight(float weight) {
    pImpl->minWeight = std::clamp(weight, -2.0f, 0.0f);
}

float AntiHebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void AntiHebbianRule::setMaxWeight(float weight) {
    pImpl->maxWeight = std::clamp(weight, 0.0f, 1.0f);
}

float AntiHebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void AntiHebbianRule::setTargetWeight(float weight) {
    pImpl->targetWeight = std::clamp(weight, -1.0f, 0.0f);
}

float AntiHebbianRule::getTargetWeight() const {
    return pImpl->targetWeight;
}

} // namespace nlm