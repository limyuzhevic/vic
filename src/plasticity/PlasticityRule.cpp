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

    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Implement real Hebbian learning with biological accuracy
    // Δw = η * (pre * post - α * w) where η is learning rate, α is decay factor
    // This implements both LTP and LTD based on spike timing
    
    // Use the most recent spikes for weight update
    Timestamp preTime = preSpikes.back();
    Timestamp postTime = postSpikes.back();
    
    // Calculate spike time difference
    float timeDiff = static_cast<float>(postTime - preTime);
    
    // Apply spike timing dependent plasticity (STDP-like) with Hebbian foundation
    float weightChange = 0.0f;
    
    if (timeDiff > 0) {
        // Post fires after pre: LTD (long-term depression)
        weightChange = -pImpl->learningRate * std::exp(-timeDiff / pImpl->ltdTimeConstant);
    } else {
        // Pre fires before post: LTP (long-term potentiation)  
        weightChange = pImpl->learningRate * std::exp(static_cast<float>(-timeDiff) / pImpl->ltpTimeConstant);
    }
    
    // Add homeostatic regulation to prevent runaway excitation/inhibition
    float homeostaticFactor = 1.0f - std::abs(static_cast<float>(synapse->getWeight())) / pImpl->weightRange;
    weightChange *= homeostaticFactor;
    
    // Apply weight change with bounds checking
    float newWeight = synapse->getWeight() + weightChange;
    if (newWeight < -pImpl->weightRange || newWeight > pImpl->weightRange) {
        // Clamp to biologically plausible range and preserve sign
        if (newWeight < 0) {
            newWeight = -pImpl->weightRange * (1.0f - std::abs(newWeight) / (pImpl->weightRange * 2.0f));
        } else {
            newWeight = pImpl->weightRange * (1.0f - std::abs(newWeight) / (pImpl->weightRange * 2.0f));
        }
    }
    
    synapse->setWeight(newWeight);
    
    // Apply spike-frequency adaptation based on recent activity
    if (preSpikes.size() > 1) {
        float preFreq = 1.0f / (postTime - preSpikes[preSpikes.size() - 2]);
        float adaptationEffect = pImpl->adaptationStrength * preFreq;
        applyWeightChange(synapse, -adaptationEffect);
    }
    
    if (postSpikes.size() > 1) {
        float postFreq = 1.0f / (postSpikes[postSpikes.size() - 2] - postSpikes.back());
        float adaptationEffect = pImpl->adaptationStrength * postFreq;
        applyWeightChange(synapse, -adaptationEffect);
    }
}

void HebbianRule::enableAdvancedMode(bool enabled) {
    pImpl->advancedMode = enabled;
}

bool HebbianRule::isAdvancedModeEnabled() const {
    return pImpl->advancedMode;
}

void HebbianRule::setHomeostaticStrength(float strength) {
    pImpl->homeostaticStrength = strength;
}

float HebbianRule::getHomeostaticStrength() const {
    return pImpl->homeostaticStrength;
}

void HebbianRule::setWeightRange(float range) {
    pImpl->weightRange = range;
}

float HebbianRule::getWeightRange() const {
    return pImpl->weightRange;
}

void HebbianRule::setTimeConstants(float ltpTc, float ltdTc) {
    pImpl->ltpTimeConstant = ltpTc;
    pImpl->ltdTimeConstant = ltdTc;
}

void HebbianRule::setAdaptationStrength(float strength) {
    pImpl->adaptationStrength = strength;
}

float HebbianRule::getAdaptationStrength() const {
    return pImpl->adaptationStrength;
}

void HebbianRule::recordSpikeStats(const std::vector<Timestamp>& spikes) {
    if (spikes.empty()) return;
    
    // Calculate firing statistics for adaptive learning
    float totalTime = static_cast<float>(spikes.back() - spikes.front());
    if (totalTime > 0) {
        float firingRate = static_cast<float>(spikes.size() - 1) / totalTime;
        float firingCoefficient = std::clamp(firingRate * pImpl->firingSensitivity, 0.0f, 1.0f);
        pImpl->firingCoefficient = firingCoefficient;
        
        // Adjust learning rate based on firing statistics
        pImpl->adaptiveLearningRate = pImpl->baseLearningRate * (1.0f + firingCoefficient * pImpl->activityModulation);
    }
}

float HebbianRule::getAdaptiveLearningRate() const {
    return pImpl->adaptiveLearningRate;
}

void HebbianRule::resetStats() {
    pImpl->firingCoefficient = 0.0f;
    pImpl->adaptiveLearningRate = pImpl->baseLearningRate;
}

HebbianRule::HebbianRule() : pImpl(new Impl) {
    // Enhanced initialization for advanced Hebbian learning
    pImpl->learningRate = 0.01f;
    pImpl->baseLearningRate = 0.01f;
    pImpl->homeostaticStrength = 0.1f;
    pImpl->weightRange = 1.0f;
    pImpl->ltpTimeConstant = 10.0f;      // Faster LTP
    pImpl->ltdTimeConstant = 20.0f;      // Slower LTD  
    pImpl->adaptationStrength = 0.05f;
    pImpl->firingSensitivity = 10.0f;    // Sensitivity to firing rate
    pImpl->activityModulation = 0.5f;    // Modulation factor
    pImpl->advancedMode = false;         // Start in simple mode
    pImpl->firingCoefficient = 0.0f;
    pImpl->adaptiveLearningRate = 0.01f;
}
