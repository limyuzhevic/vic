#include "PlasticityRule.hpp"
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
    float minWeight;
    float maxWeight;
    
    Impl() : learningRate(0.01f), minWeight(-1.0f), maxWeight(1.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Real Hebbian learning implementation using spike covariance
    // 
    // Mathematical formulation (covariance rule):
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    // 
    // For spike-based systems, we approximate using co-activity
    // 
    // This implements "neurons that fire together, wire together"
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike statistics
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    
    // Count correlated spike pairs
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms coincidence window
                ++coincidences;
            }
        }
    }
    
    // Compute weight change based on covariance
    // Δw = η * (coincidences - expectedCoincidences)
    float expectedCoincidences = preRate * postRate * dt * 0.01f;
    float delta = pImpl->learningRate * (static_cast<float>(coincidences) - expectedCoincidences);
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
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

void HebbianRule::setWeightBounds(float min, float max) {
    pImpl->minWeight = std::min(min, max);
    pImpl->maxWeight = std::max(min, max);
}

void HebbianRule::getWeightBounds(float& min, float& max) const {
    min = pImpl->minWeight;
    max = pImpl->maxWeight;
}

struct AntiHebbianRule::Impl {
    float learningRate;
    float minWeight;
    float maxWeight;
    
    Impl() : learningRate(0.01f), minWeight(-1.0f), maxWeight(1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Anti-Hebbian: decrease weight when pre and post fire together
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {
                ++coincidences;
            }
        }
    }
    
    // Apply negative weight change
    float delta = -pImpl->learningRate * static_cast<float>(coincidences);
    
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

struct BCMRule::Impl {
    float learningRate;
    float theta;  // Sliding threshold (BCM theta)
    float sigma;  // Threshold learning rate
    float minWeight;
    float maxWeight;
    float postHistorySum;  // Sum of recent postsynaptic activities
    
    Impl() : learningRate(0.01f), theta(0.5f), sigma(0.01f), minWeight(-1.0f), maxWeight(1.0f), postHistorySum(0.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate postsynaptic activity
    float postActivity = static_cast<float>(postSpikes.size()) / dt;
    
    // Update theta using sliding threshold rule
    pImpl->theta += pImpl->sigma * (postActivity - pImpl->theta);
    
    // BCM weight change rule: Δw = η * x * y * (y - θ)
    // where x = pre-synaptic activity, y = post-synaptic activity
    
    float preActivity = static_cast<float>(preSpikes.size()) / dt;
    float delta = pImpl->learningRate * preActivity * postActivity * (postActivity - pImpl->theta);
    
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setSigma(float sigma) {
    pImpl->sigma = std::clamp(sigma, 0.0f, 0.1f);
}

float BCMRule::getSigma() const {
    return pImpl->sigma;
}

} // namespace nlm
