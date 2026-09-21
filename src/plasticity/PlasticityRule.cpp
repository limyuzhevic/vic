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
    // REAL PHASE 2: Implement real Hebbian learning
    // True Hebbian learning: strengthen connections when pre- and post-synaptic neurons fire together
    // Uses spike timing-dependent correlation
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike correlation using time difference
    float totalCorrelation = 0.0f;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            // Time difference in milliseconds
            float timeDiff = static_cast<float>(post - pre);
            
            // Gaussian kernel for spike correlation
            // Peaked at zero difference, decays with increasing time difference
            totalCorrelation += std::exp(-(timeDiff * timeDiff) / (2.0f * 10.0f * 10.0f));  // 10ms sigma
        }
    }
    
    // Apply Hebbian weight change proportional to correlation
    // learningRate * correlation, with bounds checking
    float weightChange = pImpl->learningRate * totalCorrelation;
    
    // Apply change with soft bounds to prevent runaway excitation/inhibition
    float currentWeight = synapse->getWeight();
    float targetWeight = currentWeight + weightChange;
    
    // Constrain weight change to stay within reasonable bounds
    float maxChange = 0.1f;  // Maximum single update
    float clampedChange = std::clamp(weightChange, -maxChange, maxChange);
    
    // Apply the change
    float newWeight = currentWeight + clampedChange;
    // Clamp to synapse weight bounds
    newWeight = std::clamp(newWeight, synapse->getMinWeight(), synapse->getMaxWeight());
    
    synapse->setWeight(newWeight);
    
    // Also apply to eligibility trace for reward-modulated variants
    if (synapse->getPlasticityFlags().reward_modulated) {
        float eligibility = synapse->getEligibilityTrace();
        // Hebbian contributes to eligibility trace
        eligibility += clampedChange * 0.1f;  // Scale eligibility update
        synapse->setEligibilityTrace(eligibility);
    }
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
