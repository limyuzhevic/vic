// Reward-modulated STDP (R-STDP) implementation
// Combines STDP with reward signals for neuromodulated learning

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLtpWeight;      // Base A+ for potentiation
    float baseLtdWeight;      // Base A- for depression
    float timeConstant;       // Tau for exponential window (ms)
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    float rewardModulation;   // Reward modulation factor (0-1)
    float rewardHistory;      // Recent reward history
    
    Impl() : baseLtpWeight(0.01f), baseLtdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), rewardModulation(1.0f), rewardHistory(0.0f) {}
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
     * Combines spike timing with reward signals to modulate synaptic plasticity:
     *   Δw = (A+ * f(t) + A- * g(t)) * R(t)
     * 
     * Where:
     *   R(t) = reward signal at time t
     *   f(t), g(t) = STDP temporal kernels
     *   A+, A- = base STDP amplitudes
     * 
     * Biological inspiration:
     *   - Dopamine signals modulate synaptic eligibility traces
     *   - STDP provides spike timing information
     *   - Reward signals gate plasticity eligibility
     *   - Implements temporal difference learning in spiking networks
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Update reward history with decay
    pImpl->rewardHistory *= 0.95f;  // Decay factor
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: Base potentiation
                float baseDelta = pImpl->baseLtpWeight * std::exp(-dt / tau);
                // Apply reward modulation
                float modulatedDelta = baseDelta * pImpl->rewardHistory;
                totalDelta += modulatedDelta;
            } else if (dt < 0) {
                // Post before pre: Base depression
                float baseDelta = -pImpl->baseLtdWeight * std::exp(dt / tau);  // dt is negative
                // Apply reward modulation
                float modulatedDelta = baseDelta * pImpl->rewardHistory;
                totalDelta += modulatedDelta;
            }
        }
    }
    
    // Apply weight change with bounds and eligibility trace
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Update eligibility trace for future reward modulation
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
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

void RewardModulatedSTDP::setBaseLTPWeight(float weight) {
    pImpl->baseLtpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTPWeight() const {
    return pImpl->baseLtpWeight;
}

void RewardModulatedSTDP::setBaseLTDWeight(float weight) {
    pImpl->baseLtdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTDWeight() const {
    return pImpl->baseLtdWeight;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void RewardModulatedSTDP::setRewardModulation(float modulation) {
    pImpl->rewardModulation = std::clamp(modulation, 0.0f, 2.0f);
}

float RewardModulatedSTDP::getRewardModulation() const {
    return pImpl->rewardModulation;
}

void RewardModulatedSTDP::updateRewardHistory(float reward) {
    pImpl->rewardHistory = pImpl->rewardHistory * 0.9f + reward * 0.1f;  // Running average
}

float RewardModulatedSTDP::getRewardHistory() const {
    return pImpl->rewardHistory;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau, float rewardMod) {
    setBaseLTPWeight(ltpWeight);
    setBaseLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setRewardModulation(rewardMod);
}

} // namespace nlm
