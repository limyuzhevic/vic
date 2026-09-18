#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float baselinePlasticity;
    float potentiationThreshold;
    float depressionThreshold;
    
    Impl() : learningRate(0.01f), baselinePlasticity(0.0f), 
             potentiationThreshold(0.6f), depressionThreshold(-0.6f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }

    // Compute firing rate statistics for covariance rule
    // Δw = η * (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
    size_t preSpikeCount = preSpikes.size();
    size_t postSpikeCount = postSpikes.size();
    
    // Normalize firing rates by number of spikes
    float preRate = static_cast<float>(preSpikeCount) / static_cast<float>(preSpikes.size());
    float postRate = static_cast<float>(postSpikeCount) / static_cast<float>(postSpikes.size());
    
    // Compute covariance-like term based on spike timing
    // Real implementation would compute exact cross-correlation
    float coactivity = 0.0f;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            // Weight by similarity in firing time (Gaussian kernel)
            float dt = static_cast<float>(post - pre);
            coactivity += std::exp(-dt * dt / 100.0f); // 100ms width
        }
    }
    coactivity /= static_cast<float>(preSpikeCount * postSpikeCount);
    
    // Compute delta using covariance rule
    float delta = pImpl->learningRate * (coactivity - preRate * postRate);
    
    // Apply with bounds and threshold
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply biological bounds based on synapse type
    float minWeight = -1.0f; // Typical inhibitory minimum
    float maxWeight = 1.0f;  // Typical excitatory maximum
    
    // Adjust bounds based on plasticity flags
    if (synapse->getPlasticityFlags().stdp) {
        // STDP typically more constrained
        minWeight = -0.5f;
        maxWeight = 0.5f;
    }
    
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setBaselinePlasticity(float baseline) {
    pImpl->baselinePlasticity = baseline;
}

float HebbianRule::getBaselinePlasticity() const {
    return pImpl->baselinePlasticity;
}

void HebbianRule::setPotentiationThreshold(float threshold) {
    pImpl->potentiationThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float HebbianRule::getPotentiationThreshold() const {
    return pImpl->potentiationThreshold;
}

void HebbianRule::setDepressionThreshold(float threshold) {
    pImpl->depressionThreshold = std::clamp(threshold, -1.0f, 0.0f);
}

float HebbianRule::getDepressionThreshold() const {
    return pImpl->depressionThreshold;
}

} // namespace nlm
