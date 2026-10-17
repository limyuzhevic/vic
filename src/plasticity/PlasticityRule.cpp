#include "PlasticityRule.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
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
    float weightSaturation;
    float depressionRate;
    float normalizationThreshold;
    float nmdaThreshold;
    float spikeHistoryDecay;
    float homeostaticWeight;
    
    Impl() 
        : learningRate(0.01f)
        , weightSaturation(1.0f)
        , depressionRate(0.001f)
        , normalizationThreshold(0.5f)
        , nmdaThreshold(0.2f)
        , spikeHistoryDecay(0.95f)
        , homeostaticWeight(0.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse) {
        return;  // Should log error but not crash
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Enhanced Hebbian learning with NMDA receptor dynamics
    // 1. Detect coincident pre- and post-synaptic spikes
    size_t coincidentPairs = 0;
    std::vector<float> spikePairDelays;
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float delay = std::abs(post - pre);
            if (delay < 20.0) {  // Within 20ms window for Hebbian interaction
                ++coincidentPairs;
                spikePairDelays.push_back(delay);
            }
        }
    }
    
    if (coincidentPairs == 0) {
        return;
    }
    
    // 2. Apply weight change based on coincident spike pairs
    float weightChange = 0.0f;
    float averageDelay = 0.0f;
    
    if (!spikePairDelays.empty()) {
        averageDelay = std::accumulate(spikePairDelays.begin(), spikePairDelays.end(), 0.0f) / 
                      static_cast<float>(spikePairDelays.size());
        
        // NMDA receptor dynamics: stronger effect for longer delays (temporal integration)
        float nmdaFactor = std::exp(-averageDelay / pImpl->nmdaThreshold);
        
        // Weight change proportional to coincident pairs and NMDA factor
        weightChange = pImpl->learningRate * static_cast<float>(coincidentPairs) * nmdaFactor;
        
        // Apply synaptic depression for anti-Hebbian effects
        weightChange += pImpl->depressionRate * static_cast<float>(coincidentPairs);
    }
    
    // 3. Apply weight normalization and homeostatic regulation
    applyWeightChange(synapse, weightChange);
    
    // 4. Apply homeostatic weight regulation
    float currentWeight = synapse->getWeight();
    if (std::abs(currentWeight) > pImpl->normalizationThreshold) {
        float homeostaticAdjustment = -pImpl->homeostaticWeight * currentWeight * 0.1f;
        applyWeightChange(synapse, homeostaticAdjustment);
    }
    
    // 5. Apply weight saturation limits
    float saturatedWeight = std::clamp(currentWeight, -pImpl->weightSaturation, pImpl->weightSaturation);
    synapse->setWeight(saturatedWeight);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return;
    }
    
    synapse->addToWeight(delta);
    
    // Additional homeostatic regulation to prevent runaway excitation
    float weight = synapse->getWeight();
    if (weight > pImpl->weightSaturation * 0.8f || weight < -pImpl->weightSaturation * 0.8f) {
        float homeostaticCorrection = -weight * 0.01f;
        synapse->addToWeight(homeostaticCorrection);
    }
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, std::min(rate, 0.1f));  // Limit learning rate
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setWeightSaturation(float saturation) {
    pImpl->weightSaturation = std::max(0.1f, saturation);
}

float HebbianRule::getWeightSaturation() const {
    return pImpl->weightSaturation;
}

void HebbianRule::setDepressionRate(float rate) {
    pImpl->depressionRate = std::max(0.0f, rate);
}

float HebbianRule::getDepressionRate() const {
    return pImpl->depressionRate;
}

} // namespace nlm
