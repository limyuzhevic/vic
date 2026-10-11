#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baselineLTP;        // Baseline LTP amplitude (A+)
    float baselineLTD;        // Baseline LTD amplitude (A-)
    float ltpLander;           // LTP landing rate (λ)
    float ltdLanderr;          // LTD landing rate (λ')
    float rewardLearningRate;  // Reward modulation strength (η)
    float predictionError;     // Prediction error δ
    
    Impl() : baselineLTP(0.01f), baselineLTD(0.012f), ltpLander(0.5f),
             ltdLanderr(0.5f), rewardLearningRate(0.1f), predictionError(0.0f) {}
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
     * Extends standard STDP with reward prediction error modulation:
     *   Δw(t) = A(t) * STDP(Δt) + η * δ(t) * E(t)
     * 
     * Where:
     *   A(t) = baseline amplitude modulated by reward
     *   δ(t) = prediction error (actual reward - predicted reward)
     *   E(t) = synaptic eligibility trace
     *   η = reward learning rate
     * 
     * Biological inspiration:
     *   - Dopamine encodes reward prediction error
     *   - Dopamine modulates synaptic plasticity
     *   - Dopamine acts on eligibility traces
     *   - Implements temporal difference learning
     *   
     * Implementation details:
     *   - Uses reward-modulated amplitudes
     *   - Updates eligibility traces with STDP
     *   - Applies reward modulation at the end
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = 20.0f;  // STDP time constant (ms)
    float baselineA = pImpl->baselineLTP;
    float baselineB = pImpl->baselineLTD;
    
    // Calculate reward-modulated amplitudes
    float rewardA = baselineA * (1.0f + pImpl->rewardLearningRate * pImpl->predictionError);
    float rewardB = baselineB * (1.0f - pImpl->rewardLearningRate * pImpl->predictionError);
    
    // Compute STDP contribution
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // Reward-modulated LTP
                float stdpDelta = rewardA * std::exp(-dt / tau);
                totalDelta += stdpDelta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // Reward-modulated LTD
                float stdpDelta = -rewardB * std::exp(dt / tau);
                totalDelta += stdpDelta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Update eligibility trace with STDP contribution
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + totalDelta);
    
    // Apply reward modulation if prediction error is significant
    if (std::abs(pImpl->predictionError) > 0.01f) {
        float eligibilityTrace = synapse->getEligibilityTrace();
        float rewardModulation = pImpl->rewardLearningRate * pImpl->predictionError * eligibilityTrace;
        totalDelta += rewardModulation;
    }
    
    // Apply total weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
        
        // Clear eligibility trace after weight update (standard practice)
        synapse->setEligibilityTrace(0.0f);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Standard weight application with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setBaselineLTP(float ltp) {
    pImpl->baselineLTP = std::clamp(ltp, 0.0f, 0.1f);
}

float RewardModulatedSTDP::getBaselineLTP() const {
    return pImpl->baselineLTP;
}

void RewardModulatedSTDP::setBaselineLTD(float ltd) {
    pImpl->baselineLTD = std::clamp(ltd, 0.0f, 0.1f);
}

float RewardModulatedSTDP::getBaselineLTD() const {
    return pImpl->baselineLTD;
}

void RewardModulatedSTDP::setLander(float lander) {
    pImpl->ltpLander = std::clamp(lander, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLander() const {
    return pImpl->ltpLander;
}

void RewardModulatedSTDP::setLanderr(float landerr) {
    pImpl->ltdLanderr = std::clamp(landerr, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLanderr() const {
    return pImpl->ltdLanderr;
}

void RewardModulatedSTDP::setRewardLearningRate(float rate) {
    pImpl->rewardLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardLearningRate() const {
    return pImpl->rewardLearningRate;
}

void RewardModulatedSTDP::setPredictionError(float error) {
    pImpl->predictionError = std::clamp(error, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getPredictionError() const {
    return pImpl->predictionError;
}

void RewardModulatedSTDP::configure(float ltp, float ltd, float lander, float landerr, 
                                   float rewardRate) {
    setBaselineLTP(ltp);
    setBaselineLTD(ltd);
    setLander(lander);
    setLanderr(landerr);
    setRewardLearningRate(rewardRate);
}

} // namespace nlm
