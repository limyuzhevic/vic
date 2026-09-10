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
    // Real Hebbian learning implementation (Covariance rule)
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate cross-correlation term: ⟨pre * post⟩
    float crossCorrelation = 0.0f;
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float timeDiff = static_cast<float>(postSpikes[j] - preSpikes[i]);
            // Use exponential kernel for spike timing similarity
            float kernel = std::exp(-std::abs(timeDiff) / 20.0f);  // 20ms time constant
            crossCorrelation += kernel;
        }
    }
    crossCorrelation /= (preSpikes.size() * postSpikes.size());
    
    // Calculate baseline: ⟨pre⟩⟨post⟩ (product of mean firing rates)
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    float baseline = preRate * postRate * 0.001f;  // Scale factor
    
    // Compute covariance-based weight change
    float covariance = crossCorrelation - baseline;
    float delta = pImpl->learningRate * covariance;
    
    // Apply with bounds
    if (std::abs(delta) > 1e-8f) {
        applyWeightChange(synapse, delta);
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
