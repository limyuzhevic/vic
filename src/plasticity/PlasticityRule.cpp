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
    // Implement real Hebbian learning: Δw = η * (preSpike * postSpike - w)
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate temporal correlation between pre- and post-synaptic spikes
    float correlation = 0.0f;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            // Temporal window for Hebbian co-activation (typically 10-50ms)
            float timeDiff = std::abs(post - pre);
            if (timeDiff < 50.0) {  // 50ms window for co-activation
                // Exponential decay of correlation with time difference
                correlation += std::exp(-timeDiff / 20.0f);
            }
        }
    }
    
    // Normalize by number of spike pairs
    if (!preSpikes.empty() && !postSpikes.empty()) {
        correlation /= (preSpikes.size() * postSpikes.size());
    }
    
    // Apply weight change: increase weight for correlated activity
    // The change is proportional to learning rate and correlation
    float weightChange = pImpl->learningRate * correlation;
    
    // Ensure weight stays within reasonable bounds
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + weightChange;
    
    // Constrain weight between 0.0 and 1.0
    newWeight = std::max(0.0f, std::min(1.0f, newWeight));
    
    applyWeightChange(synapse, newWeight - currentWeight);
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
