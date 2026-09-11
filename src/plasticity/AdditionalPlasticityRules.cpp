#include "PlasticityRule.hpp"

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
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
    
    // Count correlated spike pairs (simplified)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Anti-Hebbian: decrease weight when neurons fire together
    // Δw = -η * (coactivity - baseline)
    float delta = -pImpl->learningRate * static_cast<float>(correlationCount);
    
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    // Anti-Hebbian weights are typically bounded around 0.5
    newWeight = std::clamp(newWeight, 0.0f, 1.0f);
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

struct BCMRule::Impl {
    float learningRate;
    float theta;  // Sliding threshold
    float maxWeight;
    float minWeight;
    
    Impl() : learningRate(0.01f), theta(0.5f), maxWeight(1.0f), minWeight(-1.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Simple BCM implementation
    // Δw = η * (post * (pre - θ))
    // Where θ is the sliding threshold based on recent activity
    
    // Calculate average pre and post activity
    float preActivity = static_cast<float>(preSpikes.size()) / 10.0f;  // Normalize
    float postActivity = static_cast<float>(postSpikes.size()) / 10.0f;  // Normalize
    
    // BCM update rule
    float delta = pImpl->learningRate * postActivity * (preActivity - pImpl->theta);
    
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

} // namespace nlm
