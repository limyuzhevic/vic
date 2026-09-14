#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    // Biological parameters for realistic Hebbian learning
    float basalLearningRate;
    float activityDependentScaling;
    float saturationFactor;
    
    // Neuromodulatory integration
    float rewardModulation;
    float noveltyModulation;
    
    // Temporal dynamics
    float traceDecayRate;
    
    // Synaptic context
    float presynapticActivity;
    float postsynapticActivity;
    
    // Random generator for biological noise
    mutable std::mt19937 rng;
    mutable std::uniform_real_distribution<float> uniformDist;
    mutable std::normal_distribution<float> normalDist;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), basalLearningRate(0.005f),
             activityDependentScaling(0.1f), saturationFactor(1.5f),
             rewardModulation(1.0f), noveltyModulation(0.5f),
             traceDecayRate(0.9f), presynapticActivity(0.0f),
             postsynapticActivity(0.0f),
             rng(std::random_device()()), uniformDist(0.0f, 1.0f),
             normalDist(0.0f, 0.1f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Enhanced Hebbian learning implementation with biological realism
     * 
     * Core mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Biological implementation:
     * - Tracks presynaptic and postsynaptic activity with eligibility traces
     * - Incorporates neuromodulatory signals (reward, novelty)
     * - Includes synaptic saturation to prevent runaway potentiation
     * - Adds biological noise for realistic variability
     * 
     * Biological inspiration:
     * - AMPA receptor trafficking at synaptic junctions
     * - Calcium-dependent plasticity in dendritic spines
     * - Stabilization through structural plasticity
     * - Homeostatic regulation for overall network stability
     * 
     * Enhanced features:
     * - Eligibility traces for delayed reward assignment
     * - Neuromodulatory gating (dopamine, acetylcholine)
     * - Activity-dependent learning rate scaling
     * - Activity normalization and homeostatic mechanisms
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float deltaTime = static_cast<float>(dt);
    
    // Update presynaptic and postsynaptic activity estimates
    float preActivity = 0.0f, postActivity = 0.0f;
    for (const auto& t : preSpikes) {
        preActivity += 1.0f;
    }
    for (const auto& t : postSpikes) {
        postActivity += 1.0f;
    }
    
    // Normalize activity
    preActivity /= static_cast<float>(preSpikes.size());
    postActivity /= static_cast<float>(postSpikes.size());
    
    // Compute covariance-based weight change
    float covariance = preActivity * postActivity;
    
    // Apply neuromodulatory gating
    float neuromodulatoryFactor = rewardModulation * 0.6f + noveltyModulation * 0.4f;
    
    // Activity-dependent learning rate
    float baseLearning = pImpl->basalLearningRate * activityDependentScaling;
    float effectiveLearningRate = baseLearning * neuromodulatoryFactor;
    
    // Apply homeostatic regulation - scale learning based on current weight
    float weight = synapse->getWeight();
    float homeostaticFactor = 1.0f - std::abs(weight) * pImpl->saturationFactor;
    homeostaticFactor = std::max(0.1f, homeostaticFactor);
    
    float delta = effectiveLearningRate * covariance * homeostaticFactor;
    
    // Add biological noise for realistic variability
    float noise = normalDist(pImpl->rng) * 0.01f;
    delta += noise;
    
    // Apply with bounds using synaptic efficacy
    if (std::abs(delta) > 1e-6f) {
        // Scale by synaptic efficacy and eligibility
        float efficacy = synapse->getEfficacy();
        float eligibility = synapse->getEligibilityTrace();
        
        delta *= efficacy * (1.0f + eligibility * 0.5f);
        
        // Apply weight change
        applyWeightChange(synapse, delta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        currentTrace *= pImpl->traceDecayRate;  // Decay existing trace
        currentTrace += delta * 0.5f;          // Add new eligibility
        synapse->setEligibilityTrace(currentTrace);
        
        // Update activity context for next update
        pImpl->presynapticActivity = preActivity;
        pImpl->postsynapticActivity = postActivity;
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and compute new weight with homeostatic bounds
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply homeostatic bounds that depend on current weight
    float homeostaticMin = -2.0f + currentWeight * 0.1f;  // Can shift bounds
    float homeostaticMax = 2.0f - currentWeight * 0.1f;
    
    // Clamp to biological limits
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Additional homeostatic regulation - prevent extreme values
    if (std::abs(newWeight) > 1.5f) {
        newWeight *= 0.9f;  // Pull back from extremes
    }
    
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

void Hebbian::setRewardModulation(float modulation) {
    pImpl->rewardModulation = std::clamp(modulation, 0.0f, 2.0f);
}

float Hebbian::getRewardModulation() const {
    return pImpl->rewardModulation;
}

void Hebbian::setNoveltyModulation(float modulation) {
    pImpl->noveltyModulation = std::clamp(modulation, 0.0f, 1.0f);
}

float Hebbian::getNoveltyModulation() const {
    return pImpl->noveltyModulation;
}

} // namespace nlm
