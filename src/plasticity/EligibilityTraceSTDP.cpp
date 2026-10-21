#include "EligibilityTraceSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

struct EligibilityTraceSTDP::Impl {
    // Basic learning parameters
    float learningRate;     // Learning rate (η)
    float traceDecayRate;   // Eligibility trace decay rate (λ)
    
    // Trace bounds
    float maxTraceValue;    // Maximum trace value
    float minTraceValue;    // Minimum trace value
    
    // Reward modulation
    float rewardModulationGain;  // Reward modulation gain (γ)
    float rewardHistory;         // Reward history for modulation
    
    // Trace management
    float currentTrace;     // Current eligibility trace value
    std::map<Timestamp, float> traceHistory;  // History of trace values
    Timestamp lastTraceUpdate;  // Time of last trace update
    Timestamp lastRewardUpdate; // Time of last reward update
    
    // Temporal filtering
    float traceFilter;      // Trace temporal filter constant
    
    Impl() : learningRate(0.01f), traceDecayRate(0.001f), maxTraceValue(10.0f),
             minTraceValue(0.0f), rewardModulationGain(0.5f), rewardHistory(0.0f),
             currentTrace(0.0f), lastTraceUpdate(0.0), lastRewardUpdate(0.0),
             traceFilter(0.1f) {}
};

EligibilityTraceSTDP::EligibilityTraceSTDP() : pImpl(new Impl) {}

EligibilityTraceSTDP::~EligibilityTraceSTDP() = default;

void EligibilityTraceSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate weight change based on eligibility trace and recent activity
    float totalDelta = 0.0f;
    
    // For each pre-synaptic spike
    for (Timestamp preTime : preSpikes) {
        // Update eligibility trace for this pre-synaptic spike
        // Trace represents the potential for future learning
        float traceContribution = pImpl->learningRate * std::exp(-(pImpl->traceDecayRate) * (preTime - pImpl->lastTraceUpdate));
        
        // Integrate post-synaptic spike contribution
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);
            
            if (delta_t > 0) {
                // Pre before post (positive Δt): potentiation potential
                float contribution = traceContribution * std::exp(-delta_t * pImpl->traceDecayRate);
                totalDelta += contribution;
            }
        }
    }
    
    // Apply trace decay over time
    float timeSinceLastUpdate = 0.0f;  // Simplified - in practice, use simulation time
    float decayFactor = std::exp(-pImpl->traceDecayRate * timeSinceLastUpdate);
    totalDelta *= decayFactor;
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
    
    // Update last trace update time
    pImpl->lastTraceUpdate = preSpikes.back();
}

void EligibilityTraceSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate new weight with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);  // Standard weight bounds
    synapse->setWeight(newWeight);
}

const char* EligibilityTraceSTDP::getName() const {
    return "EligibilityTraceSTDP";
}

void EligibilityTraceSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float EligibilityTraceSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void EligibilityTraceSTDP::setTraceDecayRate(float rate) {
    pImpl->traceDecayRate = std::clamp(rate, 0.0001f, 0.01f);
}

float EligibilityTraceSTDP::getTraceDecayRate() const {
    return pImpl->traceDecayRate;
}

void EligibilityTraceSTDP::setMaxTraceValue(float max) {
    pImpl->maxTraceValue = std::clamp(max, 0.0f, 100.0f);
}

float EligibilityTraceSTDP::getMaxTraceValue() const {
    return pImpl->maxTraceValue;
}

void EligibilityTraceSTDP::setMinTraceValue(float min) {
    pImpl->minTraceValue = std::clamp(min, 0.0f, 10.0f);
}

float EligibilityTraceSTDP::getMinTraceValue() const {
    return pImpl->minTraceValue;
}

void EligibilityTraceSTDP::setRewardModulationGain(float gain) {
    pImpl->rewardModulationGain = std::clamp(gain, 0.0f, 2.0f);
}

float EligibilityTraceSTDP::getRewardModulationGain() const {
    return pImpl->rewardModulationGain;
}

void EligibilityTraceSTDP::updateEligibilityTrace(float trace, Timestamp spikeTime, Timestamp currentTime) {
    // Update eligibility trace with spike information
    float timeDiff = currentTime - spikeTime;
    float decay = std::exp(-pImpl->traceDecayRate * timeDiff);
    
    // Apply decay and add new contribution
    pImpl->currentTrace = pImpl->currentTrace * decay + trace;
    pImpl->currentTrace = std::clamp(pImpl->currentTrace, pImpl->minTraceValue, pImpl->maxTraceValue);
    
    // Store in history
    pImpl->traceHistory[currentTime] = pImpl->currentTrace;
    pImpl->lastTraceUpdate = currentTime;
}

void EligibilityTraceSTDP::updateTraceWithReward(float reward, Timestamp rewardTime) {
    // Update eligibility trace with reward signal
    // This allows delayed reward modulation of learning
    float rewardModulation = 1.0f + pImpl->rewardModulationGain * reward;
    
    // Update trace with reward modulation
    pImpl->rewardHistory = reward;
    pImpl->lastRewardUpdate = rewardTime;
}

float EligibilityTraceSTDP::getEligibilityTrace() const {
    return pImpl->currentTrace;
}

float EligibilityTraceSTDP::getTraceHistory(Timestamp time) const {
    // Get trace value at specific time
    auto it = pImpl->traceHistory.find(time);
    if (it != pImpl->traceHistory.end()) {
        return it->second;
    }
    return 0.0f;
}

void EligibilityTraceSTDP::resetTrace() {
    // Reset eligibility trace to zero
    pImpl->currentTrace = 0.0f;
    pImpl->traceHistory.clear();
}

} // namespace nlm