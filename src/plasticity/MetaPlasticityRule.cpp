#include "MetaPlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct MetaPlasticityRule::Impl {
    // Metaplasticity parameters
    float metaPlasticityRate;      // Metaplasticity rate (γ)
    float plasticityThreshold;     // Plasticity threshold (θp)
    float hebbianSaturationThreshold; // Hebbian saturation threshold
    bool metaPlasticityEnabled;    // Whether metaplasticity is enabled
    
    // Metaplasticity state
    float hebbianActivity;         // Current Hebbian activity
    float stdpActivity;            // Current STDP activity
    float metaPlasticityState;     // Current metaplasticity state
    float metaPlasticityHistory;   // History of metaplasticity state
    
    // Implementation details
    Timestamp lastStateUpdate;     // Time of last state update
    float stateUpdateInterval;     // Interval for state updates
    
    Impl() : metaPlasticityRate(0.001f), plasticityThreshold(0.1f),
             hebbianSaturationThreshold(1.0f), metaPlasticityEnabled(true),
             hebbianActivity(0.0f), stdpActivity(0.0f), metaPlasticityState(0.5f),
             metaPlasticityHistory(0.5f), lastStateUpdate(0.0), stateUpdateInterval(1.0f) {}
};

MetaPlasticityRule::MetaPlasticityRule() : pImpl(new Impl) {}

MetaPlasticityRule::~MetaPlasticityRule() = default;

void MetaPlasticityRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate Hebbian and STDP activities
    float hebbianActivity = static_cast<float>(preSpikes.size() * postSpikes.size());
    float stdpActivity = calculateMetaPlasticityDelta(hebbianActivity, 0.0f);  // Simplified
    
    // Update metaplasticity state
    updateMetaPlasticityState(hebbianActivity, stdpActivity);
    
    // Modulate learning rate based on metaplasticity
    // This is a simplified implementation - in practice, this would
    // modulate the learning rates of other plasticity rules
    
    // Apply weight change
    // This is a placeholder - metaplasticity typically doesn't directly change weights
    // but instead modulates the learning rates of other plasticity rules
    applyWeightChange(synapse, 0.0f);
}

void MetaPlasticityRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Metaplasticity typically doesn't directly change weights
    // but instead modulates the learning rates of other plasticity rules
    // This is a placeholder implementation
    if (!synapse) return;
    
    // Metaplasticity might indirectly affect weights through
    // modulation of other plasticity rules
    float currentWeight = synapse->getWeight();
    
    // Apply very small weight change based on metaplasticity state
    // This is just for demonstration
    float modulatedDelta = delta * pImpl->metaPlasticityState * pImpl->metaPlasticityRate;
    
    if (std::abs(modulatedDelta) > 1e-6f) {
        float newWeight = currentWeight + modulatedDelta;
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        synapse->setWeight(newWeight);
    }
}

const char* MetaPlasticityRule::getName() const {
    return "MetaPlasticityRule";
}

void MetaPlasticityRule::setMetaPlasticityRate(float rate) {
    pImpl->metaPlasticityRate = std::clamp(rate, 0.0001f, 0.01f);
}

float MetaPlasticityRule::getMetaPlasticityRate() const {
    return pImpl->metaPlasticityRate;
}

void MetaPlasticityRule::setPlasticityThreshold(float threshold) {
    pImpl->plasticityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float MetaPlasticityRule::getPlasticityThreshold() const {
    return pImpl->plasticityThreshold;
}

void MetaPlasticityRule::setHebbianSaturationThreshold(float threshold) {
    pImpl->hebbianSaturationThreshold = std::clamp(threshold, 0.1f, 5.0f);
}

float MetaPlasticityRule::getHebbianSaturationThreshold() const {
    return pImpl->hebbianSaturationThreshold;
}

void MetaPlasticityRule::setMetaPlasticityEnabled(bool enabled) {
    pImpl->metaPlasticityEnabled = enabled;
}

bool MetaPlasticityRule::isMetaPlasticityEnabled() const {
    return pImpl->metaPlasticityEnabled;
}

void MetaPlasticityRule::updateMetaPlasticityState(float hebbianActivity, float stdpActivity) {
    // Update metaplasticity state based on current activities
    if (!pImpl->metaPlasticityEnabled) return;
    
    // Calculate metaplasticity delta
    float delta = calculateMetaPlasticityDelta(hebbianActivity, stdpActivity);
    
    // Update state with exponential moving average
    float alpha = 1.0f / (pImpl->stateUpdateInterval + 1.0f);
    pImpl->metaPlasticityState = (1.0f - alpha) * pImpl->metaPlasticityState + alpha * delta;
    pImpl->metaPlasticityHistory = pImpl->metaPlasticityState;
    
    pImpl->hebbianActivity = hebbianActivity;
    pImpl->stdpActivity = stdpActivity;
    
    pImpl->lastStateUpdate = static_cast<Timestamp>(pImpl->stateUpdateInterval);
}

void MetaPlasticityRule::modulateLearningRate(float &rate, float hebbianActivity, float stdpActivity) const {
    // Modulate learning rate based on metaplasticity state
    if (!pImpl->metaPlasticityEnabled) return;
    
    // Calculate modulation factor based on current state
    float modulationFactor = 1.0f;
    
    // If activities are high, reduce learning rate (metaplasticity limits runaway potentiation)
    if (hebbianActivity > pImpl->hebbianSaturationThreshold) {
        modulationFactor *= (pImpl->hebbianSaturationThreshold / hebbianActivity);
    }
    
    if (stdpActivity > pImpl->plasticityThreshold) {
        modulationFactor *= (pImpl->plasticityThreshold / stdpActivity);
    }
    
    // Apply modulation
    rate *= modulationFactor * pImpl->metaPlasticityState;
}

float MetaPlasticityRule::calculateMetaPlasticityDelta(float hebbianActivity, float stdpActivity) const {
    // Calculate metaplasticity delta based on activities
    // This represents the need for metaplasticity to regulate overall learning
    
    float normalizedHebbian = hebbianActivity / pImpl->hebbianSaturationThreshold;
    float normalizedSTDP = stdpActivity / pImpl->plasticityThreshold;
    
    float totalActivity = normalizedHebbian + normalizedSTDP;
    
    // Delta represents the need for regulation
    float delta = 1.0f / (1.0f + std::exp(-totalActivity + 2.0f));  // Sigmoid function
    
    return delta;
}

} // namespace nlm