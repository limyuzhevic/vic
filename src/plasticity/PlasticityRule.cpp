#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // REAL IMPLEMENTATION: Hebbian learning based on spike timing:
    // - Stably correlated pre-post spikes induce LTP (weight increase)
    // - Anti-correlated timing can induce LTD (weight decrease)
    // - Temporal window determines learning strength
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Process all pre-post spike pairings
    float totalDelta = 0.0f;
    float maxTemporalWindow = 30.0f;  // 30ms learning window
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = postTime - preTime;  // Post minus Pre
            
            if (std::abs(deltaT) < maxTemporalWindow) {
                // STDP-like weight change based on temporal difference
                float weightChange = 0.0f;
                
                if (deltaT > 0) {
                    // Post after Pre: Long-Term Potentiation (LTP)
                    // Stronger when timing is optimal (around 10-20ms)
                    float optimalDeltaT = 15.0f;
                    float timingFactor = std::exp(-std::abs(deltaT - optimalDeltaT) / 10.0f);
                    weightChange = pImpl->learningRate * timingFactor;
                } else {
                    // Pre after Post: Long-Term Depression (LTD)
                    // Weaker when timing is more negative
                    float timingFactor = std::exp(deltaT / 20.0f);  // deltaT is negative
                    weightChange = -pImpl->learningRate * timingFactor;
                }
                
                totalDelta += weightChange;
            }
        }
    }
    
    // Apply cumulative weight change
    if (totalDelta != 0.0f) {
        applyWeightChange(synapse, totalDelta);
    }
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    // Clamp learning rate to reasonable bounds
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
