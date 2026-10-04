#include "BCM_STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct BCMSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float bcmThreshold;   // BCM threshold (theta_m)
    float bcmLearningRate; // BCM learning rate (eta)
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), bcmThreshold(0.5f),
             bcmLearningRate(0.01f) {}
};

BCMSTDP::BCMSTDP() : pImpl(new Impl) {}

BCMSTDP::~BCMSTDP() = default;

void BCMSTDP::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // BCM potentiation with sliding threshold
                float eligibility = std::exp(-dt / tau);
                float postActivity = 1.0f; // Simplified - in real system would use actual post activity
                
                // BCM rule: delta = eta * (post - theta_m) * pre * post
                if (postActivity > pImpl->bcmThreshold) {
                    float delta = pImpl->bcmLearningRate * 
                                 (postActivity - pImpl->bcmThreshold) * 
                                 eligibility;
                    totalDelta += delta;
                }
            } else if (dt < 0) {
                // Depression in BCM framework
                float eligibility = std::exp(dt / tau);
                float postActivity = 1.0f;
                
                if (postActivity < pImpl->bcmThreshold) {
                    float delta = -pImpl->ltdWeight * eligibility;
                    totalDelta += delta;
                }
            }
        }
    }
    
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void BCMSTDP::setBCMThreshold(float threshold) {
    pImpl->bcmThreshold = std::clamp(threshold, 0.0f, 2.0f);
}

float BCMSTDP::getBCMThreshold() const {
    return pImpl->bcmThreshold;
}

void BCMSTDP::setBCMLearningRate(float rate) {
    pImpl->bcmLearningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMSTDP::getBCMLearningRate() const {
    return pImpl->bcmLearningRate;
}

} // namespace nlm
