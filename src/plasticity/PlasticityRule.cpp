#include "PlasticityRule.hpp"
#include "../core/Logger/Logger.hpp"

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

struct AntiHebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

struct BCMRule::Impl {
    float theta;        // BCM threshold
    float learningRate; // Learning rate
    float movingAverage; // Moving average of post-synaptic activity
    
    Impl() : theta(0.5f), learningRate(0.001f), movingAverage(0.0f) {}
};

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // TODO PHASE 2: Implement real Hebbian learning
    // PLACEHOLDER: Simple correlated firing increases weight
    
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
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(coincidences));
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse) {
        synapse->addToWeight(delta);
    }
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

// AntiHebbianRule implementations
AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Implement anti-Hebbian learning: decrease weight when neurons fire together
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
        applyWeightChange(synapse, -pImpl->learningRate * static_cast<float>(coincidences));
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse) {
        synapse->addToWeight(delta);
    }
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

// BCMRule implementations
BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // BCM learning rule: weight change depends on post-synaptic activity
    // Compute average post-synaptic activity
    float postActivity = 0.0f;
    for (Timestamp t : postSpikes) {
        postActivity += 1.0f;  // Simplified: each spike contributes 1
    }
    postActivity /= static_cast<float>(postSpikes.size());
    
    // Update moving average
    pImpl->movingAverage = pImpl->movingAverage * 0.9f + postActivity * 0.1f;
    
    // BCM threshold crossing determines weight change sign
    float bcmThreshold = pImpl->theta;
    float postActivityBelowThreshold = std::max(0.0f, 1.0f - postActivity / bcmThreshold);
    
    // Apply weight change based on post-activity
    float weightChange = pImpl->learningRate * postActivityBelowThreshold;
    applyWeightChange(synapse, weightChange);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse) {
        synapse->addToWeight(delta);
    }
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = theta;
}

float BCMRule::getTheta() const {
    return pImpl->theta;
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm