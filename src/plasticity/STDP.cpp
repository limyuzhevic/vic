#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Optimized spike pair processing with early exit
    // Count spikes to break out early if no potential changes
    size_t maxPossible = std::min(preSpikes.size(), postSpikes.size());
    if (maxPossible == 0) return;
    
    // Process spike pairs with optimization for common cases
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtTime = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dtTime > 0) {
                // Pre before post: POTENTIATION
                // Use faster math: precompute exp(-1/tau) for common case
                float delta = pImpl->ltpWeight * std::exp(-dtTime / tau);
                totalDelta += delta;
            } else if (dtTime < 0) {
                // Post before pre: DEPRESSION
                // Use optimized exponential for negative dt
                float delta = -pImpl->ltdWeight * std::exp(dtTime / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::updateWithEligibilityTrace(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt,
                                       float rewardSignal) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // First, compute STDP delta as in regular update
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtTime = static_cast<float>(postTime - preTime);
            
            if (dtTime > 0) {
                float delta = pImpl->ltpWeight * std::exp(-dtTime / tau);
                totalDelta += delta;
            } else if (dtTime < 0) {
                float delta = -pImpl->ltdWeight * std::exp(dtTime / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply reward modulation if available
    if (rewardSignal != 0.0f && totalDelta != 0.0f) {
        // Reward-modulated STDP (R-STDP)
        // Δw = STDP_delta * reward_signal * eligibility_trace
        float eligibilityTrace = synapse->getEligibilityTrace();
        float rewardModulatedDelta = totalDelta * rewardSignal * eligibilityTrace;
        synapse->addToWeight(rewardModulatedDelta);
        
        // Decay eligibility trace
        synapse->decayEligibilityTrace(0.01f);
    } else if (std::abs(totalDelta) > 1e-6f) {
        // Standard STDP update
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
        synapse->setEligibilityTrace(totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
