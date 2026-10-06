#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;      // Learning rate (η)
    float maxWeight;         // Maximum synaptic weight
    float minWeight;         // Minimum synaptic weight
    float timeWindow;        // Time window for anti-Hebbian learning (ms)
    
    Impl() : learningRate(0.005f), maxWeight(1.0f), minWeight(-1.0f),
             timeWindow(20.0f) {}
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
    
    // Anti-Hebbian rule: "neurons that fire together, weaken together"
    // Δw = -η × activity_pre × activity_post
    // Decrease weight when pre and post fire together
    
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime); // Δt in ms
            
            // Anti-Hebbian applies regardless of timing (simplified)
            // More sophisticated versions might consider timing windows
            totalDelta -= 1.0f; // Each pre-post pair contributes negative change
        }
    }
    
    // Apply anti-Hebbian learning with learning rate
    totalDelta *= pImpl->learningRate;
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change using applyWeightChange to ensure bounds checking
        applyWeightChange(synapse, totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.01f);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, std::min(rate, 1.0f)); // Clamp to [0, 1]
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setMaxWeight(float weight) {
    pImpl->maxWeight = weight;
}

float AntiHebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void AntiHebbianRule::setMinWeight(float weight) {
    pImpl->minWeight = weight;
}

float AntiHebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void AntiHebbianRule::setTimeWindow(float window) {
    pImpl->timeWindow = std::max(1.0f, window); // Minimum 1ms
}

float AntiHebbianRule::getTimeWindow() const {
    return pImpl->timeWindow;
}

void AntiHebbianRule::configure(float learningRate, float maxWeight, float minWeight, float timeWindow) {
    setLearningRate(learningRate);
    setMaxWeight(maxWeight);
    setMinWeight(minWeight);
    setTimeWindow(timeWindow);
}

} // namespace nlm
