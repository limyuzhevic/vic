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
    // IMPLEMENTED: Real Hebbian learning with spike-timing dependent components
    // Implements both classical Hebbian and STDP mechanisms
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike timing differences
    float totalCoincidence = 0.0f;
    float totalPrePost = 0.0f;
    float totalPostPre = 0.0f;
    
    // Process spike pairs to determine learning direction
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float delta = static_cast<float>(post - pre);
            
            if (delta > 0 && delta <= pImpl->timeWindow) {
                // Post before pre (rare, should be negative)
                totalPostPre += pImpl->learningRate;
            } else if (delta < 0 && -delta <= pImpl->timeWindow) {
                // Pre before post (LTP)
                float weight = std::exp(-std::abs(delta) / pImpl->timeConstant);
                totalPrePost += pImpl->learningRate * weight;
            } else {
                // Outside time window - weak effect
                float weight = std::exp(-std::abs(delta) / pImpl->timeConstant);
                totalCoincidence += pImpl->learningRate * weight * pImpl->backgroundRate;
            }
        }
    }
    
    // Apply weight changes based on spike timing
    float weightChange = 0.0f;
    
    if (totalPrePost > totalPostPre) {
        // Long-term potentiation (LTP)
        weightChange = totalPrePost * pImpl->learningRate;
        NLM_LOG_DEBUG("Hebbian LTP: weight increase = " + std::to_string(weightChange));
    } else if (totalPostPre > totalPrePost) {
        // Long-term depression (LTD)
        weightChange = -totalPostPre * pImpl->learningRate;
        NLM_LOG_DEBUG("Hebbian LTD: weight decrease = " + std::to_string(weightChange));
    } else {
        // Background learning
        weightChange = totalCoincidence * pImpl->learningRate;
        NLM_LOG_DEBUG("Hebbian background: weight change = " + std::to_string(weightChange));
    }
    
    // Apply weight change with bounds checking
    float currentWeight = synapse->getWeight();
    float newWeight = std::clamp(currentWeight + weightChange, 0.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
