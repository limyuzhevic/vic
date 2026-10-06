#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

struct HebbianRule::Impl {
    float learningRate;        // Learning rate (η)
    float maxWeight;           // Maximum synaptic weight
    float minWeight;           // Minimum synaptic weight
    float timeWindow;          // Time window for Hebbian learning (ms)
    float eligibilityTraceDecay; // Decay rate for eligibility traces
    float rewardModulation;    // Reward modulation factor
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             timeWindow(50.0f), eligibilityTraceDecay(0.01f),
             rewardModulation(1.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Standard Hebbian rule: "neurons that fire together, wire together"
    // Δw = η × activity_pre × activity_post
    
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Calculate correlation strength based on timing
            float dt = static_cast<float>(postTime - preTime); // Δt in ms
            
            // Temporal window for Hebbian learning
            if (std::abs(dt) <= pImpl->timeWindow) {
                // Within time window: positive correlation
                // Use exponential window function to weight by temporal proximity
                float temporalWeight = std::exp(-std::abs(dt) / pImpl->timeWindow);
                
                // Activity product: each spike contributes 1.0 (simplified)
                // In more sophisticated models, this could be firing rates or firing traces
                float correlation = temporalWeight;
                totalDelta += correlation;
            }
        }
    }
    
    // Apply Hebbian learning with learning rate
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
        synapse->setEligibilityTrace(currentTrace + totalDelta * pImpl->eligibilityTraceDecay);
        
        // Apply reward modulation if configured
        if (pImpl->rewardModulation > 0.0f) {
            float reward = synapse->getRecentReward();
            if (reward > 0.0f) {
                // Reward-modulated Hebbian: stronger learning with reward
                float modulatedDelta = totalDelta * pImpl->rewardModulation * reward;
                synapse->addToWeight(modulatedDelta);
            }
        }
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, std::min(rate, 1.0f)); // Clamp to [0, 1]
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeight(float weight) {
    pImpl->maxWeight = weight;
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void HebbianRule::setMinWeight(float weight) {
    pImpl->minWeight = weight;
}

float HebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void HebbianRule::setTimeWindow(float window) {
    pImpl->timeWindow = std::max(1.0f, window); // Minimum 1ms
}

float HebbianRule::getTimeWindow() const {
    return pImpl->timeWindow;
}

void HebbianRule::setEligibilityTraceDecay(float decay) {
    pImpl->eligibilityTraceDecay = std::max(0.0f, std::min(decay, 1.0f)); // Clamp to [0, 1]
}

float HebbianRule::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

void HebbianRule::setRewardModulation(float modulation) {
    pImpl->rewardModulation = std::max(0.0f, modulation);
}

float HebbianRule::getRewardModulation() const {
    return pImpl->rewardModulation;
}

void HebbianRule::configure(float learningRate, float maxWeight, float minWeight, 
                           float timeWindow, float traceDecay, float rewardModulation) {
    setLearningRate(learningRate);
    setMaxWeight(maxWeight);
    setMinWeight(minWeight);
    setTimeWindow(timeWindow);
    setEligibilityTraceDecay(traceDecay);
    setRewardModulation(rewardModulation);
}

} // namespace nlm
