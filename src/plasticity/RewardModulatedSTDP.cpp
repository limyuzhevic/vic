#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLtpWeight;      // Baseline LTP weight
    float baseLtdWeight;      // Baseline LTD weight
    float timeConstant;       // STDP time constant (ms)
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    
    // Reward modulation factor
    float rewardFactor;        // Scales STDP with reward (1.0 = normal, >1.0 = enhanced, <1.0 = suppressed)
    
    // Eligibility traces for reward modulation
    float eligibilityTrace;   // Accumulated weight change eligibility
    float traceDecay;         // How quickly eligibility traces decay
    
    Impl() : baseLtpWeight(0.01f), baseLtdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), rewardFactor(1.0f),
             eligibilityTrace(0.0f), traceDecay(0.95f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    /*
     * Reward-Modulated STDP (R-STDP) implementation
     * 
     * R-STDP extends standard STDP by incorporating reward prediction error
     * as a multiplicative modulator of synaptic plasticity
     * 
     * Mathematical formulation:
     * 
     * Base STDP: Δw = A+ * exp(-Δt/τ+) - A- * exp(-Δt/τ-)  for Δt > 0
     *            Δw = -A- * exp(-Δt/τ-)                for Δt < 0
     *            
     * R-STDP: Δw = R * (A+ * exp(-Δt/τ+) - A- * exp(-Δt/τ-))  for Δt > 0
     *         Δw = R * (-A- * exp(-Δt/τ-))                 for Δt < 0
     *         
     * Where:
     *   R = reward factor (1.0 = normal, >1.0 = enhanced, <1.0 = suppressed)
     *   A+ = base LTP weight
     *   A- = base LTD weight
     *   τ+ = τ- = time constant
     *   Δt = t_post - t_pre
     *   
     * Biological inspiration:
     *   - Models dopamine's effect on STDP
     *   - Accounts for reward prediction errors
     *   - Implements reinforcement learning in spiking networks
     *   - Found in basal ganglia and dopaminergic pathways
     *   
     * Implementation details:
     *   - Uses reward prediction error to scale STDP updates
     *   - Maintains eligibility traces for delayed reward effects
     *   - Combines spike timing with reward information
     *   - Provides biological mechanism for RL in neural networks
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate reward-modulated weight change
    float weightChange = calculateR-STDPWeightChange(synapse, preSpikes, postSpikes, dt);
    
    // Update eligibility trace
    updateEligibilityTrace(synapse, weightChange, dt);
    
    // Apply weight change with reward modulation
    applyWeightChange(synapse, weightChange);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Calculate reward-modulated weight change
    float actualDelta = delta * pImpl->rewardFactor;
    
    // Apply weight change with bounds
    float newWeight = synapse->getWeight() + actualDelta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLTPWeight(float weight) {
    pImpl->baseLtpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTPWeight() const {
    return pImpl->baseLtpWeight;
}

void RewardModulatedSTDP::setLTDWeight(float weight) {
    pImpl->baseLtdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTDWeight() const {
    return pImpl->baseLtdWeight;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void RewardModulatedSTDP::setRewardFactor(float factor) {
    pImpl->rewardFactor = std::max(0.0f, factor);
}

float RewardModulatedSTDP::getRewardFactor() const {
    return pImpl->rewardFactor;
}

float RewardModulatedSTDP::calculateR-STDPWeightChange(Synapse* synapse,
                                                       const std::vector<Timestamp>& preSpikes,
                                                       const std::vector<Timestamp>& postSpikes,
                                                       TimestepDuration dt) {
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION with reward modulation
                float delta = pImpl->baseLtpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION with reward modulation
                float delta = -pImpl->baseLtdWeight * std::exp(dt / tau);  // dt is negative
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes)
        }
    }
    
    return totalDelta;
}

void RewardModulatedSTDP::updateEligibilityTrace(Synapse* synapse, float weightChange, TimestepDuration dt) {
    if (!synapse) return;
    
    // Update eligibility trace: E = E * decay + weightChange * rewardFactor
    float currentTrace = synapse->getEligibilityTrace();
    currentTrace = currentTrace * pImpl->traceDecay + weightChange * pImpl->rewardFactor;
    synapse->setEligibilityTrace(currentTrace);
}

} // namespace nlm
