#include "AntiHebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;      // Learning rate (0.0 to 1.0)
    float depressionRate;    // Depression rate (0.0 to 1.0)
    float minWeight;         // Minimum synaptic weight
    float maxWeight;         // Maximum synaptic weight
    
    Impl() : learningRate(0.01f), depressionRate(0.012f), minWeight(-1.0f), maxWeight(1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::AntiHebbianRule(AntiHebbianRule&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

AntiHebbianRule& AntiHebbianRule::operator=(AntiHebbianRule&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

AntiHebbianRule::~AntiHebbianRule() {
    delete pImpl;
}

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    /*
     * Anti-Hebbian learning implementation
     * 
     * Mathematical formulation:
     * Δw = -η * (coactivity) for "fire together, don't wire together"
     * 
     * Biological inspiration:
     * - Spike-timing dependent depression (reverse STDP)
     * - Prevents runaway excitation in neural circuits
     * - Homeostatic synaptic scaling
     * 
     * Limitations:
     * - Simplified correlation model
     * - Single learning rate (no separate potentiation/depression)
     * - Assumes stationary statistics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate coactivity (spikes firing at similar times)
    size_t coactivityCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(std::abs(postTime - preTime));
            // Count spikes within correlation window
            if (dt < 50.0f) {  // 50ms correlation window
                ++coactivityCount;
            }
        }
    }
    
    // Anti-Hebbian rule: decrease weight when neurons fire together
    float delta = -pImpl->learningRate * static_cast<float>(coactivityCount);
    
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

void AntiHebbianRule::setDepressionRate(float rate) {
    pImpl->depressionRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getDepressionRate() const {
    return pImpl->depressionRate;
}

void AntiHebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float AntiHebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void AntiHebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float AntiHebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void AntiHebbianRule::configure(float learningRate, float depressionRate, float minWeight, float maxWeight) {
    if (learningRate < 0.0f || depressionRate < 0.0f || minWeight > maxWeight) {
        NLM_LOG_WARNING("Invalid AntiHebbian parameters: learningRate=%f, depressionRate=%f, minWeight=%f, maxWeight=%f", 
                        learningRate, depressionRate, minWeight, maxWeight);
        return;
    }
    
    setLearningRate(learningRate);
    setDepressionRate(depressionRate);
    setMinWeight(minWeight);
    setMaxWeight(maxWeight);
}

} // namespace nlm