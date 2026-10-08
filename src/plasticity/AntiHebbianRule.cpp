#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float depressionFactor;
    float minWeight;
    float maxWeight;
    
    Impl() : learningRate(0.01f), depressionFactor(1.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
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
    
    // Anti-Hebbian rule: decrease weight when neurons fire together
    // This is the opposite of Hebbian learning
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 50.0) {  // 50ms window
                ++coincidences;
            }
        }
    }
    
    // Apply negative weight change proportional to coincidences
    float delta = -pImpl->learningRate * pImpl->depressionFactor * static_cast<float>(coincidences);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
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

void AntiHebbianRule::setDepressionFactor(float factor) {
    pImpl->depressionFactor = std::clamp(factor, 0.1f, 10.0f);
}

float AntiHebbianRule::getDepressionFactor() const {
    return pImpl->depressionFactor;
}

} // namespace nlm