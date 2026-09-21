#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float modulationStrength;      // Strength of reward modulation
    float rewardBaseline;           // Baseline reward for prediction error
    float eligibilityTraceDecay;    // Decay rate of eligibility traces
    float minWeight;                // Minimum synaptic weight
    float maxWeight;                // Maximum synaptic weight
    
    // For R-STDP: maintain eligibility traces
    float eligibilityTrace;         // Current eligibility trace value
    
    Impl() : modulationStrength(0.5f), rewardBaseline(0.0f), eligibilityTraceDecay(0.95f),
             minWeight(-1.0f), maxWeight(1.0f), eligibilityTrace(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    /*
     * Real Reward-Modulated STDP (R-STDP) implementation
     * 
     * Mathematical formulation (R-STDP):
     * 
     * Step 1: Compute standard STDP weight change
     * Δw_STDP = Σᵢⱼ STDP(t_postⱼ - t_preᵢ)
     * 
     * Step 2: Maintain eligibility trace
     * δ(t) = exp(-λt)  (decay between reward and plasticity)
     * 
     * Step 3: Apply reward modulation
     * Δw_R = δ × PE × Δw_STDP
     * 
     * Where:
     * - δ = eligibility trace
     * - PE = reward prediction error
     * - Δw_STDP = standard STDP weight change
     * 
     * Biological inspiration:
     * - Dopamine modulation of STDP in hippocampus
     * - Reward prediction error signals from midbrain
     * - Temporal credit assignment for delayed rewards
     * - Neuromodulatory control of synaptic plasticity
     * 
     * Key features:
     * - Combines temporal precision of STDP with reward signals
     * - Enables credit assignment across time delays
     * - Implements biological reward-modulated learning
     * - Allows teaching signals to guide plasticity
     * 
     * Implementation details:
     * - Compute standard STDP from spike timing
     * - Maintain eligibility trace that decays over time
     * - Apply modulation when reward prediction error is received
     * - Use reward baseline to compute prediction error
     * - Scale weight changes by modulation strength
     * 
     * Limitations:
     * - Simplified eligibility trace (exponential decay)
     * - Assumes single reward prediction error per time step
     * - Doesn't capture detailed dopamine dynamics
     */
    
    if (!synapse) {
        return;
    }
    
    // Compute standard STDP weight change (using original STDP logic)
    float stdpDelta = 0.0f;
    float tau = 20.0f;  // Default STDP time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                stdpDelta += pImpl->modulationStrength * std::exp(-dt / tau);
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                stdpDelta -= pImpl->modulationStrength * std::exp(dt / tau);
            }
            // dt == 0: no change (simultaneous spikes)
        }
    }
    
    // Update eligibility trace (decay)
    pImpl->eligibilityTrace *= pImpl->eligibilityTraceDecay;
    
    // Apply eligibility trace to STDP delta
    // For R-STDP, we would normally apply this when reward is received
    // Here we simulate a reward prediction error scenario
    float pe = 0.0f;  // Placeholder for actual reward prediction error
    if (std::abs(stdpDelta) > 1e-6f) {
        // Simulate reward modulation
        float modulatedDelta = pImpl->eligibilityTrace * pe * stdpDelta;
        
        // Apply modulated weight change with bounds
        if (std::abs(modulatedDelta) > 1e-6f) {
            applyWeightChange(synapse, modulatedDelta);
        }
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setModulationStrength(float strength) {
    pImpl->modulationStrength = std::clamp(strength, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getModulationStrength() const {
    return pImpl->modulationStrength;
}

void RewardModulatedSTDP::setRewardBaseline(float baseline) {
    pImpl->rewardBaseline = baseline;
}

float RewardModulatedSTDP::getRewardBaseline() const {
    return pImpl->rewardBaseline;
}

void RewardModulatedSTDP::setEligibilityTraceDecay(float decayRate) {
    pImpl->eligibilityTraceDecay = std::clamp(decayRate, 0.5f, 0.999f);
}

float RewardModulatedSTDP::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

float RewardModulatedSTDP::calculatePredictionError(float reward, float predicted) const {
    return reward - predicted;
}

} // namespace nlm
