#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct AntiHebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Anti-Hebbian rule: "neurons that fire together, unwire together"
    // Negative correlation decreases synaptic strength
    
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
    
    // Apply weight change proportional to coincidences (negative)
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

struct BCMRule::Impl {
    float thetaPlus;      // BCM threshold for potentiation
    float thetaMinus;      // BCM threshold for depression
    float learningRate;
    
    Impl() : thetaPlus(15.0f), thetaMinus(-15.0f), learningRate(0.01f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    // BCM rule: Oja's learning rule variant
    // Weight change depends on postsynaptic activity relative to threshold
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Simple BCM implementation: weight depends on activity balance
    // Actual BCM would require more sophisticated synaptic variables
    // For now, use a simplified version based on spike correlation
    
    // Calculate synaptic eligibility trace based on pre-post activity
    float preActivity = static_cast<float>(preSpikes.size()) * 0.1f;  // Normalize
    float postActivity = static_cast<float>(postSpikes.size()) * 0.1f;
    
    // BCM update: if post activity > theta_plus, potentiate; if < theta_minus, depress
    float activity = postActivity - preActivity;
    float delta = 0.0f;
    
    if (activity > pImpl->thetaPlus) {
        delta = pImpl->learningRate * (activity - pImpl->thetaPlus);
    } else if (activity < pImpl->thetaMinus) {
        delta = pImpl->learningRate * (activity - pImpl->thetaMinus);
    }
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setThetaPlus(float theta_plus) {
    pImpl->thetaPlus = theta_plus;
}

float BCMRule::getThetaPlus() const {
    return pImpl->thetaPlus;
}

void BCMRule::setThetaMinus(float theta_minus) {
    pImpl->thetaMinus = theta_minus;
}

float BCMRule::getThetaMinus() const {
    return pImpl->thetaMinus;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
