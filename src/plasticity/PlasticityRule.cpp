#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// Anti-Hebbian rule implementation
class AntiHebbianRule::Impl {
public:
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // TODO PHASE 2: Implement real anti-Hebbian learning
    // PLACEHOLDER: Simple anti-correlation decreases weight
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Anti-Hebbian: decrease weight when neurons fire together
    // This implements "neurons that fire together, unwire together"
    
    // Count coincident spikes (simplified)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, -pImpl->learningRate * static_cast<float>(coincidences));
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// BCM rule implementation
class BCMRule::Impl {
public:
    float learningRate;
    float threshold;
    
    Impl() : learningRate(0.01f), threshold(0.5f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    // TODO PHASE 2: Implement real BCM learning
    // PLACEHOLDER: Simple BCM rule implementation
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // BCM (Bienenstock-Cooper-Munro) rule:
    // Δw = η * (post - θ) * pre
    // where θ is the sliding threshold based on post-synaptic activity
    
    // Simple implementation: use average post-synaptic activity
    double avgPostActivity = 0.0;
    for (Timestamp t : postSpikes) {
        avgPostActivity += static_cast<double>(t) / 1000.0;  // Scale to reasonable range
    }
    avgPostActivity /= postSpikes.size();
    
    // Compute weight change based on BCM rule
    double weightChange = pImpl->learningRate * (avgPostActivity - pImpl->threshold);
    
    // Apply for each pre-synaptic spike
    for (Timestamp t : preSpikes) {
        applyWeightChange(synapse, static_cast<float>(weightChange * static_cast<double>(t) / 1000.0));
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float BCMRule::getThreshold() const {
    return pImpl->threshold;
}

} // namespace nlm
