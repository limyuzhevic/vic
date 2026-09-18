#include "PlasticityRule.hpp"
#include "Hebbian.hpp"
#include <stdexcept>

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

HebbianRule::HebbianRule() : pImpl(std::make_unique<Impl>()) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Implement real Hebbian learning based on covariance rule
    // Δw = η × (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
    
    if (!synapse) {
        throw std::invalid_argument("HebbianRule::update called with null synapse");
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return; // No activity, no learning
    }
    
    // Validate spike timestamps
    for (Timestamp spike : preSpikes) {
        if (spike < 0) {
            throw std::invalid_argument("HebbianRule::update called with negative pre-spike timestamp");
        }
    }
    
    for (Timestamp spike : postSpikes) {
        if (spike < 0) {
            throw std::invalid_argument("HebbianRule::update called with negative post-spike timestamp");
        }
    }
    
    if (dt <= 0.0) {
        throw std::invalid_argument("HebbianRule::update called with invalid dt: " + std::to_string(dt));
    }
    
    // Calculate spike statistics for covariance computation
    // For efficiency, compute running statistics without storing all spikes
    float preMean = 0.0f, postMean = 0.0f;
    float prePostProductSum = 0.0f;
    
    size_t count = 0;
    for (Timestamp preTime : preSpikes) {
        float preActivity = 1.0f / (std::abs(static_cast<float>(preTime)) + 1.0f);
        preMean += preActivity;
        
        for (Timestamp postTime : postSpikes) {
            float postActivity = 1.0f / (std::abs(static_cast<float>(postTime)) + 1.0f);
            postMean += postActivity;
            prePostProductSum += preActivity * postActivity;
            count++;
        }
    }
    
    if (count > 0) {
        preMean /= static_cast<float>(count);
        postMean /= static_cast<float>(count);
    }
    
    // Compute covariance: cov(pre, post) = ⟨pre×post⟩ - ⟨pre⟩⟨post⟩
    float covariance = prePostProductSum / static_cast<float>(count) - (preMean * postMean);
    
    // Apply weight change based on covariance
    float delta = pImpl->learningRate * covariance;
    
    // Clamp to reasonable range
    delta = std::clamp(delta, -0.1f, 0.1f);
    
    // Only apply if change is significant
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        throw std::invalid_argument("HebbianRule::applyWeightChange called with null synapse");
    }
    
    // Apply weight change with bounds checking
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Clamp to biologically plausible range
    newWeight = std::clamp(newWeight, -10.0f, 10.0f);
    
    // Log significant weight changes for debugging
    float weightChange = newWeight - currentWeight;
    if (std::abs(weightChange) > 0.01f) {
        // Could add logging here if logger is available
    }
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    if (rate < 0.0f || rate > 1.0f) {
        throw std::invalid_argument("HebbianRule::setLearningRate called with out-of-range value: " + std::to_string(rate));
    }
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
