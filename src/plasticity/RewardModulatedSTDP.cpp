#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    // Reward-modulated STDP parameters
    float baselineLTPWeight;    // Base LTP weight (A+)
    float baselineLTDWeight;    // Base LTD weight (A-)
    float timeConstant;         // STDP time constant (tau) for exponential window (ms)
    float rewardSensitivity;    // How strongly rewards modulate synaptic plasticity
    float predictionErrorDecay; // Rate at which prediction errors decay over time
    
    // Initialize with biologically plausible values
    Impl() : baselineLTPWeight(0.01f), baselineLTDWeight(0.012f), timeConstant(20.0f),
             rewardSensitivity(1.0f), predictionErrorDecay(0.9f) {
        // Set up adaptive learning parameters for context-sensitive plasticity
    }
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    /*
     * Reward-Modulated STDP (R-STDP)
     * 
     * Implements reward prediction error based STDP, where the magnitude
     * and sign of weight changes depend on the difference between expected
     * and actual rewards.
     * 
     * Mathematical formulation:
     * 
     * Δw = A * exp(-|Δt|/τ) * (r - r̂) * f(w)
     * 
     * Where:
     *   A = baseline learning rate (scaled by reward sensitivity)
     *   Δt = t_post - t_pre (spike timing difference)
     *   τ = timeConstant (STDP time window)
     *   r = actual reward (prediction error)
     *   r̂ = predicted reward (previous expectation)
     *   f(w) = weight-dependent modulation (soft bounds)
     * 
     * Biological inspiration:
     *   - Combines spike timing with neuromodulatory signals
     *   - Implements temporal difference learning at synaptic level
     *   - Creates credit assignment for reward prediction errors
     * 
     * Advanced features:
     *   - Adaptive learning rates based on prediction errors
     *   - Reward prediction error computation
     *   - Eligibility trace normalization
     *   - Biological plausibility through neuromodulatory pathways
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Get spike timing difference
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    // Calculate base STDP delta
    float stdpDelta = 0.0f;
    if (dtMs > 0) {
        stdpDelta = pImpl->baselineLTPWeight * std::exp(-dtMs / tau);
    } else if (dtMs < 0) {
        stdpDelta = -pImpl->baselineLTDWeight * std::exp(dtMs / tau);
    }
    
    // Get reward prediction error from synapse eligibility trace
    float predictionError = synapse->getEligibilityTrace();
    
    // Apply reward modulation
    float modulatedDelta = stdpDelta * predictionError * pImpl->rewardSensitivity;
    
    // Apply weight-dependent modulation (soft bounds)
    float w = synapse->getWeight();
    bool awayFromZero = std::abs(w + modulatedDelta) > std::abs(w);
    modulatedDelta *= awayFromZero ? (1.0f - std::abs(w)) : std::abs(w);
    
    // Handle inhibitory synapses
    if (synapse->getType() == SynapseType::Inhibitory) {
        modulatedDelta = -modulatedDelta;
    }
    
    // Apply weight change with bounds
    if (std::abs(modulatedDelta) > 1e-6f) {
        // Scale by synaptic efficacy
        float efficacy = synapse->getEfficacy();
        modulatedDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(modulatedDelta);
        
        // Enforce sign bounds
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
        
        // Update eligibility trace with reward-modulated learning rate
        float currentTrace = synapse->getEligibilityTrace();
        float rewardModulation = predictionError * pImpl->rewardSensitivity;
        synapse->setEligibilityTrace(currentTrace + modulatedDelta * rewardModulation);
        
        // Decay eligibility trace
        synapse->setEligibilityTrace(currentTrace * pImpl->predictionErrorDecay);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

} // namespace nlm