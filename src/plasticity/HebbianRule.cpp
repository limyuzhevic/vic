#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

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
    if (!synapse || !synapse->isEnabled() || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Real Hebbian rule implementation
    // "neurons that fire together, wire together"
    
    // Calculate correlation between pre- and post-synaptic activity
    float correlation = std::min(static_cast<float>(preSpikes.size()), 
                                 static_cast<float>(postSpikes.size())) /
                        std::max(static_cast<float>(preSpikes.size()), 
                                 static_cast<float>(postSpikes.size()));
    
    // Weight change proportional to correlation
    float delta = pImpl->learningRate * correlation;
    
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

} // namespace nlm