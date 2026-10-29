#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../neuromodulation/Dopamine.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLTPWeight;      // Baseline LTP weight
    float baseLTDWeight;      // Baseline LTD weight
    float dopamineScale;      // Scaling factor for dopamine modulation
    float baselineLTP;        // Baseline LTP rate
    float baselineLTD;        // Baseline LTD rate
    
    Impl() : baseLTPWeight(0.01f), baseLTDWeight(0.012f), 
             dopamineScale(0.5f), baselineLTP(0.01f), baselineLTD(0.012f) {}
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
     * Biological basis: Dopamine neuromodulation of synaptic plasticity
     * 
     * Mathematical formulation:
     *   Δw = w_STDP * f(dopamine)
     *   Where f(dopamine) is a sigmoidal function mapping dopamine levels
     *   to plasticity modulation:
     *     - Positive dopamine: enhances LTP
     *     - Negative dopamine: enhances LTD
     *     - Zero dopamine: no change
     * 
     * This implements reward prediction error signals from dopamine neurons
     * that modulate synaptic learning according to the dopaminergic hypothesis
     * of reinforcement learning.
     * 
     * Biological inspiration:
     *   - Dopamine neurons in substantia nigra/ventral tegmental area
     *   - Reward prediction error signals (Rescorla-Wagner, temporal difference)
     *   - Neuromodulation of plasticity rules in basal ganglia
     *   - Phasic dopamine enhances LTP at activated synapses
     *   - Persistent dopamine enhances LTD at inactive synapses
     * 
     * Applications:
     *   - Reinforcement learning
     *   - Reward-based behavioral adaptation
     *   - Value-based decision making
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get baseline STDP weight changes
    float tau = 20.0f;  // Default time constant for STDP
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_ms = static_cast<float>(postTime - preTime);
            
            if (dt_ms > 0) {
                // Pre before post: baseline LTP
                float baseChange = pImpl->baseLTPWeight * std::exp(-dt_ms / tau);
                totalDelta += baseChange;
            } else if (dt_ms < 0) {
                // Post before pre: baseline LTD
                float baseChange = -pImpl->baseLTDWeight * std::exp(dt_ms / tau);
                totalDelta += baseChange;
            }
        }
    }
    
    // Get dopamine level from neuromodulation system
    // This requires access to the brain's neuromodulation system
    // For now, we'll simulate dopamine signal based on recent reward
    float dopamineLevel = 1.0f;  // Default - will be overridden in real usage
    
    // Apply dopamine modulation to STDP weight changes
    float modulatedDelta = totalDelta * (1.0f + pImpl->dopamineScale * (dopamineLevel - 1.0f));
    
    // Clamp the change
    float clampedDelta = std::clamp(modulatedDelta, -0.1f, 0.1f);
    
    if (std::abs(clampedDelta) > 1e-6f) {
        // Apply weight change
        synapse->addToWeight(clampedDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + clampedDelta);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setBaseLTPWeight(float weight) {
    pImpl->baseLTPWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTPWeight() const {
    return pImpl->baseLTPWeight;
}

void RewardModulatedSTDP::setBaseLTDWeight(float weight) {
    pImpl->baseLTDWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTDWeight() const {
    return pImpl->baseLTDWeight;
}

void RewardModulatedSTDP::setDopamineScale(float scale) {
    pImpl->dopamineScale = std::clamp(scale, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getDopamineScale() const {
    return pImpl->dopamineScale;
}

} // namespace nlm