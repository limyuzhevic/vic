#include "AntiHebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float decayRate;      // Homeostatic decay rate
    float targetWeight;   // Target weight for homeostasis
    float learningRate;   // Anti-Hebbian learning rate
    
    Impl() : decayRate(0.001f), targetWeight(0.0f), learningRate(0.01f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Apply homeostatic decay based on current weight
    float currentWeight = synapse->getWeight();
    float weightDeviation = currentWeight - pImpl->targetWeight;
    
    // Anti-Hebbian decay: weight moves toward target
    // If weight is too high, decay toward target; if too low, may strengthen slightly
    float decayAmount = -pImpl->decayRate * weightDeviation;
    
    // Apply with biological constraint: stronger synapses decay faster (STDP-like relationship)
    float strengthFactor = std::min(std::abs(currentWeight), 1.0f) * 2.0f - 0.5f;  // -0.5 to 1.5
    decayAmount *= (1.0f + strengthFactor * 0.5f);  // Amplify for strong synapses
    
    // Combine with Hebbian-like coincidence detection for more nuanced regulation
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    if (coincidences > 0) {
        // Anti-Hebbian component: weaken connection due to co-activation
        float antiHebbianDelta = -pImpl->learningRate * static_cast<float>(coincidences);
        decayAmount += antiHebbianDelta;
    }
    
    if (std::abs(decayAmount) > 1e-6f) {
        applyWeightChange(synapse, decayAmount);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biological bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 0.1f);
}

float AntiHebbianRule::getDecayRate() const {
    return pImpl->decayRate;
}

void AntiHebbianRule::setTargetWeight(float weight) {
    pImpl->targetWeight = std::clamp(weight, -1.0f, 1.0f);
}

float AntiHebbianRule::getTargetWeight() const {
    return pImpl->targetWeight;
}

} // namespace nlm