#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

void AntiHebbianRule::update(Synapse* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (simplified)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Apply negative weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, -0.01f * static_cast<float>(coincidences));
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

struct BCMRule::Impl {
    float threshold;
    float learningRate;
    
    Impl() : threshold(0.5f), learningRate(0.01f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average postsynaptic firing rate
    float postFiringRate = static_cast<float>(postSpikes.size()) / dt;
    
    // BCM rule: weight change depends on postsynaptic activity
    // LTP if post activity exceeds threshold, LTD otherwise
    if (postFiringRate > pImpl->threshold) {
        // Long-term potentiation
        float delta = pImpl->learningRate * static_cast<float>(postSpikes.size());
        applyWeightChange(synapse, delta);
    } else {
        // Long-term depression
        float delta = -pImpl->learningRate * static_cast<float>(postSpikes.size());
        applyWeightChange(synapse, delta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

void BCMRule::setThreshold(float threshold) {
    pImpl->threshold = std::clamp(threshold, 0.0f, 1.0f);
}

float BCMRule::getThreshold() const {
    return pImpl->threshold;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
