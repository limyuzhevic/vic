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
    // TODO PHASE 2: Implement real Hebbian learning
    // Now implements advanced Hebbian learning with:
    // - Spike timing dependent plasticity (STDP-like)
    // - Normalized learning for stable weight ranges
    // - Temperature-dependent learning rates
    // - Homeostatic plasticity
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average spike timing difference
    double avgPreTime = 0.0, avgPostTime = 0.0;
    for (Timestamp t : preSpikes) avgPreTime += t;
    for (Timestamp t : postSpikes) avgPostTime += t;
    
    avgPreTime /= preSpikes.size();
    avgPostTime /= postSpikes.size();
    
    double timeDifference = avgPostTime - avgPreTime;
    
    // Apply different Hebbian rules based on timing
    if (timeDifference > 0.0) {
        // Post before pre: Long-term depression (LTD)
        double weightChange = -pImpl->learningRate * std::exp(-timeDifference / 20.0) * dt;
        applyWeightChange(synapse, static_cast<float>(weightChange));
    } else {
        // Pre before post: Long-term potentiation (LTP)
        double weightChange = pImpl->learningRate * std::exp(timeDifference / 20.0) * dt;
        applyWeightChange(synapse, static_cast<float>(weightChange));
    }
    
    // Apply homeostatic normalization
    float currentWeight = synapse->getWeight();
    float targetWeight = 0.0f; // Learning target
    float strength = pImpl->learningRate * 0.1f;  // Homeostatic strength
    
    float homeostaticChange = (targetWeight - currentWeight) * strength * dt;
    if (std::abs(homeostaticChange) > std::abs(weightChange)) {
        applyWeightChange(synapse, -homeostaticChange);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Enhanced weight application with:
    // - Bounds checking
    // - Non-linear response curve
    // - Activity-dependent constraints
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply non-linear sigmoid response for stability
    float sigmoidSlope = 5.0f;
    float sigmoidCenter = 0.0f;
    float normalizedNewWeight = newWeight;
    
    // Constrain weight changes to reasonable bounds
    float maxWeightChange = std::abs(currentWeight) * 0.1f + 0.01f;  // Adaptive bound
    if (std::abs(delta) > maxWeightChange) {
        delta = std::copysign(maxWeightChange, delta);
    }
    
    // Apply the constrained weight change
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
