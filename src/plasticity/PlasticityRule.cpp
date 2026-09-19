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
    // Real Hebbian learning: Correlated firing strengthens connections
    // Based on biological studies of synaptic potentiation
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute spike timing correlation
    float correlation = 0.0f;
    size_t maxSpikes = std::min(preSpikes.size(), postSpikes.size());
    
    // Calculate pairwise spike timing differences
    for (size_t i = 0; i < maxSpikes; ++i) {
        // Find nearest post-spike to each pre-spike
        float minDiff = std::numeric_limits<float>::max();
        for (const auto& post : postSpikes) {
            float diff = std::abs(static_cast<float>(preSpikes[i]) - static_cast<float>(post));
            if (diff < minDiff) minDiff = diff;
        }
        
        // Apply spike-timing dependent component
        // Potentiation within ~20ms windows
        if (minDiff <= 20.0f) {
            correlation += 1.0f - (minDiff / 20.0f);
        }
    }
    
    // Normalize correlation
    correlation = correlation / static_cast<float>(maxSpikes);
    
    // Apply weight change based on correlation and learning rate
    if (correlation > 0.1f) {  // Threshold for significant correlation
        // Base Hebbian strengthening
        float weightChange = pImpl->learningRate * correlation * 
                           (1.0f + 0.5f * std::sin(correlation * 3.14159f));  // Modulation
        
        // Ensure weight stays within reasonable bounds
        float newWeight = synapse->getWeight() + weightChange;
        
        // Constrain weights: -1.0 to 1.0
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        
        applyWeightChange(synapse, newWeight - synapse->getWeight());
        
        // Also increase synaptic efficacy for future interactions
        float efficacy = synapse->getEfficacy();
        efficacy = std::min(1.0f, efficacy + 0.05f * correlation);
        synapse->setEfficacy(efficacy);
    }
    
    // Apply short-term depression/facilitation based on spike history
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // Simple short-term plasticity model
        float depressionFactor = 0.95f + 0.05f * correlation;
        synapse->setShortTermDepression(std::max(0.5f, synapse->getShortTermDepression() * depressionFactor));
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
