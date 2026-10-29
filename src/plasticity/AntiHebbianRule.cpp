#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    /*
     * Anti-Hebbian Learning Implementation
     * 
     * Mathematical formulation:
     * Δw = -η * x_pre * x_post
     * 
     * Where:
     *   Δw = weight change (negative for anti-Hebbian)
     *   η = learning rate
     *   x_pre = pre-synaptic activity
     *   x_post = post-synaptic activity
     * 
     * Biological basis:
     *   "Neurons that fire together, untie together"
     *   - Opposes the standard Hebbian rule
     *   - Implements competitive learning
     *   - Found in inhibitory circuits and competition
     *   - Reflects homeostatic plasticity mechanisms
     *   - Important for gain control and stability
     * 
     * Applications:
     *   - Competitive learning (e.g., Weber-McClelland)
     *   - Winner-take-all networks
     *   - Attention mechanisms
     *   - Prevention of runaway excitation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs
    size_t correlationCount = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = std::abs(static_cast<float>(postTime - preTime));
            if (timeDiff < 50.0f) {  // 50ms window
                correlationCount++;
            }
        }
    }
    
    // Apply negative weight change proportional to correlation
    if (correlationCount > 0) {
        float delta = -pImpl->learningRate * static_cast<float>(correlationCount);
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
    return "AntiHebbianRule";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm