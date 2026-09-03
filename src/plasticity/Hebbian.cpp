#include "Hebbian.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    // TODO PHASE 2: Implement real Hebbian learning
    // PLACEHOLDER
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Simplified: weight increases when pre and post spike together
    // More sophisticated versions would consider spike timing
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    float newWeight = synapse->getWeight() + delta * pImpl->learningRate;
    newWeight = std::clamp(newWeight, -pImpl->maxWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = maxWeight;
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
