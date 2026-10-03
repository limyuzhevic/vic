#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float learningRate;      // Overall learning rate (η)
    float rewardFactor;      // Reward modulation strength (α)
    float traceDecay;        // Eligibility trace decay time constant (τ)
    float minWeight;         // Minimum synaptic weight
    float maxWeight;         // Maximum synaptic weight
    float baselineLTP;       // Baseline LTP weight (A+)
    float baselineLTD;       // Baseline LTD weight (A-)
    
    Impl() : learningRate(0.01f), rewardFactor(1.0f), traceDecay(100.0f),
             minWeight(-1.0f), maxWeight(1.0f), baselineLTP(0.01f), baselineLTD(0.012f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    /*
     * Reward-Modulated STDP (R-STDP) Implementation
     * 
     * Biological inspiration:
     *   - Dopamine signals encode prediction errors in reinforcement learning
     *   - Eligibility traces capture recent spike interactions
     *   - Dopamine modulates trace to produce weight changes
     *   - Implemented in midbrain dopaminergic pathways (VTA/SNc)
     *   - Reflects synaptic learning in reward-based navigation tasks
     *   
     * Mathematical formulation:
     *   dE/dt = -E/τ + Σ STDP_pre→post(t)
     *   Δw = η * α * E * R
     *   
     * Where:
     *   E = eligibility trace
     *   R = reward prediction error (dopamine signal)
     *   α = reward modulation factor
     *   τ = trace decay time constant
     *   η = overall learning rate
     *   STDP_pre→post(t) = spike-timing dependent plasticity contribution
     *   
     * This implements the dopamine-modulated STDP model from:
     *   Dayan & Abbott, Theoretical Neuroscience (2001)
     *   Schultz et al., Nature (1997)
     *   
     * Features:
     *   - Combines spike-timing information with reward prediction errors
     *   - Uses biologically plausible dopamine dynamics
     *   - Implements credit assignment through eligibility traces
     *   - Supports temporal difference learning
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->traceDecay;
    
    // Calculate STDP contributions for each pre-post spike pair
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: potential for potentiation
                // "Cells that fire together, wire together"
                float stdpDelta = pImpl->baselineLTP * std::exp(-dt / tau);
                totalDelta += stdpDelta;
            } else if (dt < 0) {
                // Post before pre: potential for depression
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float stdpDelta = -pImpl->baselineLTD * std::exp(dt / tau);
                totalDelta += stdpDelta;
            }
            // dt == 0: simultaneous spikes - minimal effect
        }
    }
    
    // Apply reward modulation if there are eligible synapses
    if (std::abs(totalDelta) > 1e-6f) {
        // Get current eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        
        // Update eligibility trace with STDP contributions
        // Trace accumulates STDP signals and decays over time
        float newTrace = currentTrace + totalDelta * pImpl->learningRate;
        synapse->setEligibilityTrace(newTrace);
        
        // Simulate reward prediction error (dopamine signal)
        // In a real implementation, this would come from a neuromodulatory system
        // For now, we'll use a placeholder reward signal
        float rewardError = 1.0f;  // Could be modulated by actual reward signals
        
        // Apply weight change modulated by reward
        // Δw = η * α * E * R
        float weightChange = pImpl->learningRate * pImpl->rewardFactor * newTrace * rewardError;
        
        // Apply the modulated weight change
        if (std::abs(weightChange) > 1e-6f) {
            // Get current eligibility trace again (may have been modified)
            float finalTrace = synapse->getEligibilityTrace();
            
            // Apply weight change with bounds
            if (std::abs(finalTrace) > 1e-6f) {
                float efficacy = synapse->getEfficacy();
                float modulatedDelta = weightChange * efficacy;
                applyWeightChange(synapse, modulatedDelta);
                
                // Optionally decay eligibility trace
                float decayedTrace = finalTrace * 0.1f;  // Decay factor
                synapse->setEligibilityTrace(decayedTrace);
            }
        }
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setRewardFactor(float factor) {
    pImpl->rewardFactor = std::clamp(factor, 0.0f, 5.0f);
}

float RewardModulatedSTDP::getRewardFactor() const {
    return pImpl->rewardFactor;
}

void RewardModulatedSTDP::setTraceDecay(float decay) {
    pImpl->traceDecay = std::clamp(decay, 10.0f, 1000.0f);
}

float RewardModulatedSTDP::getTraceDecay() const {
    return pImpl->traceDecay;
}

void RewardModulatedSTDP::configure(float learningRate, float rewardFactor, float traceDecay) {
    setLearningRate(learningRate);
    setRewardFactor(rewardFactor);
    setTraceDecay(traceDecay);
}

} // namespace nlm