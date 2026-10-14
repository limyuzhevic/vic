#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float rewardSensitivity;  // β parameter [0.0, 1.0]
    float baselineLearningRate; // Base LTP/LTD weight
    float adaptationTime;   // Time constant for adaptation
    
    Impl() : rewardSensitivity(0.2f), baselineLearningRate(0.01f), adaptationTime(100.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {
    predictionError = 0.0f;
    rewardModulationEnabled = true;
}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt,
                                float rewardPredictionError) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update prediction error
    predictionError = rewardPredictionError;
    
    if (!rewardModulationEnabled) {
        // Fall back to standard STDP
        STDP::update(synapse, preSpikes, postSpikes, dt);
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = getTimeConstant();
    
    // Reward modulation function
    // F(δ) = 1 + β * δ for δ ≥ 0 (positive prediction error)
    // F(δ) = 1 - β * |δ| for δ < 0 (negative prediction error)
    float modulationFactor;
    if (rewardPredictionError >= 0.0f) {
        modulationFactor = 1.0f + pImpl->rewardSensitivity * rewardPredictionError;
    } else {
        modulationFactor = 1.0f - pImpl->rewardSensitivity * std::abs(rewardPredictionError);
    }
    
    // Clamp modulation factor to reasonable range
    modulationFactor = std::max(0.1f, std::min(modulationFactor, 5.0f));
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtMs = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dtMs > 0) {
                // Pre before post: POTENTIATION with reward modulation
                float baseDelta = pImpl->baselineLearningRate * std::exp(-dtMs / tau);
                float modulatedDelta = baseDelta * modulationFactor;
                totalDelta += modulatedDelta;
            } else if (dtMs < 0) {
                // Post before pre: DEPRESSION with reward modulation
                float baseDelta = -pImpl->baselineLearningRate * std::exp(dtMs / tau);
                float modulatedDelta = baseDelta * modulationFactor;
                totalDelta += modulatedDelta;
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace with modulated delta
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void RewardModulatedSTDP::setRewardSensitivity(float beta) {
    pImpl->rewardSensitivity = std::max(0.0f, std::min(beta, 1.0f));
}

float RewardModulatedSTDP::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

void RewardModulatedSTDP::enableRewardModulation(bool enable) {
    rewardModulationEnabled = enable;
}

bool RewardModulatedSTDP::isRewardModulationEnabled() const {
    return rewardModulationEnabled;
}

float RewardModulatedSTDP::getCurrentModulationFactor() const {
    if (!rewardModulationEnabled) return 1.0f;
    
    float modulationFactor;
    if (predictionError >= 0.0f) {
        modulationFactor = 1.0f + pImpl->rewardSensitivity * predictionError;
    } else {
        modulationFactor = 1.0f - pImpl->rewardSensitivity * std::abs(predictionError);
    }
    
    return std::max(0.1f, std::min(modulationFactor, 5.0f));
}

} // namespace nlm
