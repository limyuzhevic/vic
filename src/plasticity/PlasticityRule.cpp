#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Real Hebbian learning implementation
    // Based on spike timing correlation and homeostatic mechanisms
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate synaptic activity correlation
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    
    // Count temporally correlated spike pairs (within 20ms window)
    size_t correlatedPairs = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = static_cast<float>(std::abs(post - pre));
            if (timeDiff < 20.0f) {  // 20ms correlation window
                ++correlatedPairs;
            }
        }
    }
    
    // Calculate weight change using covariance-based Hebbian rule
    float delta = pImpl->learningRate * static_cast<float>(correlatedPairs);
    
    // Apply homeostatic scaling to prevent runaway potentiation
    float currentWeight = synapse->getWeight();
    float targetWeight = 0.0f;  // Target for homeostasis
    float homeostaticFactor = 0.001f * (targetWeight - currentWeight);
    delta += homeostaticFactor;
    
    // Apply weight change with bounds
    applyWeightChange(synapse, delta);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void HebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float HebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

} // namespace nlm
