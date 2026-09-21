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
    // PLACEHOLDER: Simple correlated firing increases weight
    
    if (preSpikes.empty() || postSpikes.empty() || !synapse) {
        return;
    }
    
    // Check if rule is enabled
    if (!isEnabled()) {
        return;
    }
    
    // Implementation of real Hebbian learning (Covariance rule):
    // Δw = η × (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
    // 
    // First, compute average pre and post synaptic activities
    float preActivity = 0.0f;
    float postActivity = 0.0f;
    
    for (const auto& spike : preSpikes) {
        preActivity += 1.0f;  // Each spike contributes to activity
    }
    preActivity /= preSpikes.size();
    
    for (const auto& spike : postSpikes) {
        postActivity += 1.0f;
    }
    postActivity /= postSpikes.size();
    
    // Compute correlated activity (coincidence detection)
    size_t coincidences = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            // Consider spikes within 20ms as temporally correlated
            float dt_ms = static_cast<float>(std::abs(postTime - preTime) * 1000.0f);
            if (dt_ms < 20.0f) {
                ++coincidences;
            }
        }
    }
    
    // Normalize coincidences by maximum possible
    float maxCoincidences = static_cast<float>(preSpikes.size() * postSpikes.size());
    float correlation = (maxCoincidences > 0.0f) ? (static_cast<float>(coincidences) / maxCoincidences) : 0.0f;
    
    // Compute covariance-like term: correlation - independent activity
    float independentProduct = preActivity * postActivity;
    float delta = pImpl->learningRate * (correlation - independentProduct);
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-8f) {
        applyWeightChange(synapse, delta);
    }
    
    // Additional homeostatic scaling to prevent runaway potentiation
    // Use BCM-like sliding threshold
    static constexpr float BCM_THRESHOLD = 0.5f;
    float slidingThreshold = BCM_THRESHOLD + 0.01f * (correlation - independentProduct);
    slidingThreshold = std::clamp(slidingThreshold, 0.1f, 1.0f);
    
    // Apply homeostatic regulation if weight exceeds threshold
    float currentWeight = synapse->getWeight();
    if (currentWeight > slidingThreshold && delta > 0.0f) {
        // Reduce weight to maintain stability
        float homeostasisFactor = 0.5f * (currentWeight - slidingThreshold) / currentWeight;
        float homeostaticDelta = -homeostasisFactor * delta;
        applyWeightChange(synapse, homeostaticDelta);
    }
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
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
