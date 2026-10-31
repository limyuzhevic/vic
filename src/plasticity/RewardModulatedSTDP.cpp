// Reward-modulated STDP (R-STDP)
// Implementation of RewardModulatedSTDP

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLTPWeight;    // Base LTP weight
    float baseLTDWeight;    // Base LTD weight
    float timeConstant;     // Time constant for exponential window (ms)
    float rewardModulation; // Reward modulation factor
    float rewardTrace;      // Eligibility trace for reward modulation
    float decayRate;        // Decay rate of reward trace
    
    Impl() : baseLTPWeight(0.01f), baseLTDWeight(0.012f), timeConstant(20.0f),
             rewardModulation(1.0f), rewardTrace(0.0f), decayRate(0.001f) {}
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
     * Combines spike-timing dependent plasticity with reward prediction errors
     * to guide learning toward rewarding outcomes.
     * 
     * Mathematical formulation:
     * Δw = STDP(t_post - t_pre) * R(t)
     * 
     * Where R(t) is the reward prediction error at time t.
     * 
     * Key components:
     *   - Standard STDP for spike timing
     *   - Reward prediction error modulates plasticity magnitude
     *   - Eligibility traces store recent spike pairings
     *   - Credit assignment determines which synapses contributed to reward
     *   
     * Biological inspiration:
     *   - Dopamine modulates STDP through reward prediction errors
     *   - Implements temporal difference learning in spiking networks
     *   - Addresses the credit assignment problem in reinforcement learning
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Decay reward trace
    pImpl->rewardTrace *= std::exp(-static_cast<float>(dt) * 1000.0f / pImpl->decayRate);
    
    // Compute standard STDP contribution from spike timing
    float stdpDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                stdpDelta += pImpl->baseLTPWeight * std::exp(-dt / tau);
            } else if (dt < 0) {
                // Post before pre: depression
                stdpDelta += -pImpl->baseLTDWeight * std::exp(dt / tau);
            }
        }
    }
    
    // Apply reward modulation
    if (pImpl->rewardTrace != 0.0f) {
        stdpDelta *= pImpl->rewardModulation;
        
        // Apply weight change
        if (std::abs(stdpDelta) > 1e-6f) {
            // Scale by synaptic efficacy
            float efficacy = synapse->getEfficacy();
            stdpDelta *= efficacy;
            
            // Apply weight change
            synapse->addToWeight(stdpDelta);
            
            // Update eligibility trace
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + stdpDelta);
        }
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to reasonable bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

} // namespace nlm
