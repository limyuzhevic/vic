#include "RewardModulatedPlasticity.hpp"
#include "../../brain/Synapse.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedPlasticity::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    
    // Reward modulation parameters
    float rewardStrength;      // Reward sensitivity (alpha)
    float predictionErrorWeight; // Prediction error weight (beta)
    float dopamineGain;        // Dopamine gain modulation
    float dopamineThreshold;   // Dopamine threshold for learning
    
    // Integration
    Neuromodulator* neuromodulator; // Reward neuromodulator (dopamine)
    
    // State tracking
    float accumulatedPredictionError; // For prediction error-based learning
    float rewardHistory[10];           // Recent reward history (for temporal difference)
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             rewardStrength(0.1f), predictionErrorWeight(0.05f),
             dopamineGain(1.0f), dopamineThreshold(0.01f),
             neuromodulator(nullptr), accumulatedPredictionError(0.0f) {
        for (int i = 0; i < 10; ++i) rewardHistory[i] = 0.0f;
    }
};

RewardModulatedPlasticity::RewardModulatedPlasticity() : pImpl(new Impl) {}

RewardModulatedPlasticity::~RewardModulatedPlasticity() = default;

void RewardModulatedPlasticity::update(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    if (!synapse) return;
    
    // Get neuromodulatory influence (dopamine level)
    float neuromodulatoryInfluence = 0.0f;
    if (pImpl->neuromodulator) {
        neuromodulatoryInfluence = pImpl->neuromodulator->getLevel();
    }
    
    // Calculate basic Hebbian contribution
    float delta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potential potentiation
                float eligibility = std::exp(-dt / 20.0f); // 20ms time constant
                delta += eligibility;
            } else if (dt < 0) {
                // Post before pre: potential depression
                float eligibility = -std::exp(dt / 20.0f);
                delta += eligibility;
            }
        }
    }
    
    // Apply reward modulation
    if (neuromodulatoryInfluence > pImpl->dopamineThreshold) {
        float rewardModulation = neuromodulatoryInfluence * pImpl->rewardStrength;
        
        // Apply reward-based scaling
        delta *= (1.0f + rewardModulation);
        
        // Update prediction error (simplified)
        pImpl->accumulatedPredictionError += delta * pImpl->predictionErrorWeight;
    }
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void RewardModulatedPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply dopamine gain modulation if available
    if (pImpl->neuromodulator && pImpl->dopamineGain > 0.0f) {
        delta *= pImpl->dopamineGain;
    }
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedPlasticity::getName() const {
    return "RewardModulatedPlasticity";
}

void RewardModulatedPlasticity::setRewardStrength(float strength) {
    pImpl->rewardStrength = std::clamp(strength, 0.0f, 1.0f);
}

float RewardModulatedPlasticity::getRewardStrength() const {
    return pImpl->rewardStrength;
}

void RewardModulatedPlasticity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedPlasticity::getPredictionErrorWeight() const {
    return pImpl->predictionErrorWeight;
}

void RewardModulatedPlasticity::setNeuromodulator(Neuromodulator* neuromodulator) {
    pImpl->neuromodulator = neuromodulator;
}

Neuromodulator* RewardModulatedPlasticity::getNeuromodulator() const {
    return pImpl->neuromodulator;
}

void RewardModulatedPlasticity::setDopamineGain(float gain) {
    pImpl->dopamineGain = std::clamp(gain, 0.0f, 5.0f);
}

float RewardModulatedPlasticity::getDopamineGain() const {
    return pImpl->dopamineGain;
}

void RewardModulatedPlasticity::setDopamineThreshold(float threshold) {
    pImpl->dopamineThreshold = std::clamp(threshold, 0.0f, 0.5f);
}

float RewardModulatedPlasticity::getDopamineThreshold() const {
    return pImpl->dopamineThreshold;
}

} // namespace nlm
