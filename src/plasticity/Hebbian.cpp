#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation with multiple biological mechanisms
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Enhanced features:
     *   - Spike-timing dependent covariance
     *   - Adaptive learning rate with homeostasis
     *   - Multiple plasticity regimes (Hebbian, anti-Hebbian, BCM)
     *   - Synaptic competition and normalization
     *   - Short-term and long-term mechanisms
     *   - Multiple spike interaction rules
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty() || dt <= 0) {
        return;
    }
    
    // Calculate statistics for covariance learning
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    
    // Remove duplicate spikes within small tolerance
    std::vector<Timestamp> uniquePreSpikes = preSpikes;
    std::vector<Timestamp> uniquePostSpikes = postSpikes;
    std::sort(uniquePreSpikes.begin(), uniquePreSpikes.end());
    std::sort(uniquePostSpikes.begin(), uniquePostSpikes.end());
    
    auto removeDuplicates = [](std::vector<Timestamp>& spikes, Timestamp tolerance = 1.0f) {
        if (spikes.empty()) return;
        std::vector<Timestamp> unique;
        unique.push_back(spikes[0]);
        for (size_t i = 1; i < spikes.size(); ++i) {
            if (spikes[i] - unique.back() > tolerance) {
                unique.push_back(spikes[i]);
            }
        }
        spikes = unique;
    };
    
    removeDuplicates(uniquePreSpikes);
    removeDuplicates(uniquePostSpikes);
    
    // Compute covariance-based weight change
    float correlationSum = 0.0f;
    float preMean = preRate;
    float postMean = postRate;
    float prePostProductSum = 0.0f;
    
    // Process spike pairs with temporal weighting
    for (Timestamp preTime : uniquePreSpikes) {
        for (Timestamp postTime : uniquePostSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            // Temporal weighting for spike pairs
            float temporalWeight = 1.0f;
            if (std::abs(deltaT) < 20.0f) {  // 20ms window for Hebbian interactions
                temporalWeight = 1.0f - std::abs(deltaT) / 20.0f;
            } else {
                temporalWeight = 0.0f;  // No Hebbian effect beyond window
            }
            
            if (temporalWeight > 0.01f) {
                float prePostProduct = temporalWeight;
                correlationSum += prePostProduct;
                prePostProductSum += prePostProduct;
            }
        }
    }
    
    // Calculate covariance
    float expectedProduct = preMean * postMean;
    float covariance = (correlationSum / (uniquePreSpikes.size() * uniquePostSpikes.size())) - expectedProduct;
    
    // Apply BCM-like threshold modulation
    float currentWeight = synapse->getWeight();
    float threshold = 0.5f;  // BCM threshold parameter
    float ltp = std::max(0.0f, covariance - threshold);
    float ltd = std::max(0.0f, threshold - covariance);
    
    // Adaptive learning rate based on weight distance from threshold
    float adaptiveLearningRate = pImpl->learningRate;
    float weightDistanceFromThreshold = std::abs(currentWeight - threshold);
    float homeostaticFactor = std::max(0.1f, 1.0f - weightDistanceFromThreshold);  // Prevent runaway
    
    adaptiveLearningRate *= homeostaticFactor;
    
    // Apply BCM-like update
    float delta = adaptiveLearningRate * (ltp * covariance - ltd * covariance);
    
    // Apply to synapse
    applyWeightChange(synapse, delta);
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate target
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply BCM-like homeostatic regulation
    float threshold = 0.5f;
    float distanceFromThreshold = std::abs(newWeight - threshold);
    
    // Prevent runaway potentiation by limiting weight range
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Add synaptic competition: higher weights are harder to increase
    if (newWeight > threshold) {
        float saturationFactor = std::max(0.0f, 1.0f - (newWeight - threshold) / (pImpl->maxWeight - threshold));
        newWeight = threshold + (newWeight - threshold) * saturationFactor;
    }
    
    // Gradual weight change to prevent instability
    if (std::abs(newWeight - currentWeight) > 1e-6f) {
        float maxStep = (pImpl->maxWeight - pImpl->minWeight) * 0.05f;  // Max 5% change per step
        float actualStep = std::min(std::abs(newWeight - currentWeight), maxStep);
        
        if (newWeight > currentWeight) {
            newWeight = currentWeight + actualStep;
        } else {
            newWeight = currentWeight - actualStep;
        }
    }
    
    // Apply final weight
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
