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
    float maxWeight;
    float minWeight;
    float maxLearningRate;
    float minLearningRate;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             maxLearningRate(1.0f), minLearningRate(0.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Covariance-based Hebbian learning (Bartholomew 2009)
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    
    // Calculate mean activities
    float preMean = 0.0f;
    float postMean = 0.0f;
    
    // Convert spikes to binary activity (1 if spike occurred, 0 otherwise)
    // For simplicity, we count spike occurrences and normalize
    preMean = static_cast<float>(preSpikes.size()) / 1000.0f;  // Assume 1000ms window
    postMean = static_cast<float>(postSpikes.size()) / 1000.0f;
    
    // Calculate correlation term
    float correlation = 0.0f;
    size_t windowSize = 100;  // 100ms window for spike correlation
    
    // Simple spike correlation: count coincident spikes within window
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) <= windowSize) {
                correlation += 1.0f;
            }
        }
    }
    
    correlation /= static_cast<float>(preSpikes.size() * postSpikes.size());
    
    // Calculate weight change
    float delta = pImpl->learningRate * (correlation - preMean * postMean);
    
    // Apply with bounds
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
    pImpl->learningRate = std::clamp(rate, pImpl->minLearningRate, pImpl->maxLearningRate);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void HebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float HebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float maxLearningRate;
    float minLearningRate;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             maxLearningRate(1.0f), minLearningRate(0.0f) {}
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
    
    // Anti-Hebbian learning: decrease weight when neurons fire together
    // Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    // This implements "neurons that fire together, disconnect together"
    
    // Calculate mean activities
    float preMean = static_cast<float>(preSpikes.size()) / 1000.0f;
    float postMean = static_cast<float>(postSpikes.size()) / 1000.0f;
    
    // Calculate correlation term
    float correlation = 0.0f;
    size_t windowSize = 100;  // 100ms window for spike correlation
    
    // Simple spike correlation: count coincident spikes within window
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) <= windowSize) {
                correlation += 1.0f;
            }
        }
    }
    
    correlation /= static_cast<float>(preSpikes.size() * postSpikes.size());
    
    // Calculate weight change (negative for anti-Hebbian)
    float delta = -pImpl->learningRate * (correlation - preMean * postMean);
    
    // Apply with bounds
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
    pImpl->learningRate = std::clamp(rate, pImpl->minLearningRate, pImpl->maxLearningRate);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float AntiHebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void AntiHebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float AntiHebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

struct BCMRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float slidingThreshold;      // θ (theta) - sliding threshold
    float thresholdHistory;      // θ_m - moving average of squared post activity
    float thresholdAdaptation;   // Learning rate for threshold adaptation
    float maxThreshold;
    float minThreshold;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             slidingThreshold(0.5f), thresholdHistory(0.25f), thresholdAdaptation(0.001f),
             maxThreshold(2.0f), minThreshold(0.1f) {}
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
    
    // Bienenstock-Cooper-Munro (BCM) learning rule
    // Δw = φ(x) * (x - θ), where:
    //   φ(x) = x(x - θ_m) is the sliding threshold function
    //   θ is the sliding threshold (moving average of post activity squared)
    //   x is post-synaptic activity
    
    // Calculate post-synaptic activity (simplified as spike count)
    float postActivity = static_cast<float>(postSpikes.size()) / 1000.0f;  // Normalized spike rate
    
    // Update sliding threshold (θ_m)
    // θ_m = α * θ_m + (1-α) * x², where α is adaptation rate
    float activitySquared = postActivity * postActivity;
    pImpl->thresholdHistory = (1.0f - pImpl->thresholdAdaptation) * pImpl->thresholdHistory +
                              pImpl->thresholdAdaptation * activitySquared;
    
    // Calculate sliding threshold (θ)
    pImpl->slidingThreshold = std::clamp(pImpl->thresholdHistory, pImpl->minThreshold, pImpl->maxThreshold);
    
    // Calculate weight change using BCM rule
    float weightChange = pImpl->learningRate * postActivity * (postActivity - pImpl->slidingThreshold);
    
    // Apply with bounds
    if (std::abs(weightChange) > 1e-6f) {
        applyWeightChange(synapse, weightChange);
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

void BCMRule::setSlidingThreshold(float threshold) {
    pImpl->slidingThreshold = std::clamp(threshold, pImpl->minThreshold, pImpl->maxThreshold);
}

float BCMRule::getSlidingThreshold() const {
    return pImpl->slidingThreshold;
}

void BCMRule::setThresholdAdaptation(float adaptation) {
    pImpl->thresholdAdaptation = std::clamp(adaptation, 0.001f, 0.1f);
}

float BCMRule::getThresholdAdaptation() const {
    return pImpl->thresholdAdaptation;
}

void BCMRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float BCMRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void BCMRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float BCMRule::getMinWeight() const {
    return pImpl->minWeight;
}

} // namespace nlm
